/*  dma-proxy.c - The simplest kernel module.
*
* Copyright (C) 2013 - 2016 Xilinx, Inc
* Copyright (C) 2021 BlackLynx, Inc
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program. If not, see <http://www.gnu.org/licenses/>.
*   
*   DMA Proxy
*
*   This module is designed to be a small example of a DMA device driver that is
*   a client to the DMA Engine using the AXI DMA driver. It serves as a proxy for
*   kernel space DMA control to a user space application.
*
*   A zero copy scheme is provided by allowing user space to mmap a kernel allocated
*   memory region into user space, referred to as a proxy channel interface. The
*   ioctl function is provided to start a DMA transfer which then blocks until the
*   transfer is complete. No input arguments are being used in the ioctl function.
*
*   There is an associated user space application, dma_proxy_test.c, and dma_proxy.h
*   that works with this device driver.
*
*   The hardware design was tested with an AXI DMA without scatter gather and
*   with the transmit channel looped back to the receive channel.
*
*   This driver is character driver which creates 2 devices that user space can
*   access for each DMA channel, /dev/dma_proxy_rx and /dev/dma_proxy_tx.

*   An internal test mode is provided to allow it to be self testing without the 
*   need for a user space application and this mode is good for making bigger
*   changes to this driver.
*
*   This driver is designed to be simple to help users get familiar with how to 
*   use the DMA driver provided by Xilinx which uses the Linux DMA Engine. The 
*   performance of this example is not expected to be high without more work.
*
*   To use this driver a node must be added into the device tree.  Add the 
*   following node while adjusting the dmas property to match the name of 
*   the AXI DMA node.
* 
*   dma_proxy_test_0: dma_proxy@0 {
*   	compatible ="xlnx,dma_proxy";
*		dmas = <&axi_dma_0 0
*				&axi_dma_0 1>;
*		dma-names = "dma_proxy_tx", "dma_proxy_rx";
*	};
*
*/

#include <linux/kernel.h>
#include <linux/dmaengine.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/log2.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/of_dma.h>
#include <linux/dma-map-ops.h>
#include <linux/mutex.h>

#include <linux/delay.h> // just for debug

#include "dma-proxy.h"

#undef INTERNAL_TEST

MODULE_LICENSE("GPL");

#define DRIVER_NAME 		"dma_proxy"
#define CHANNEL_COUNT 		2
#define TX_CHANNEL			0
#define RX_CHANNEL			1
#define ERROR 			   -1
#define NOT_LAST_CHANNEL 	0

/*
 * Globals
 */  
DEFINE_MUTEX(PROXY_GLOBAL_MUTEX);
struct class* PROXY_GLOBAL_CLASS = NULL;
uint32_t DEVICE_COUNT = 0;

/* 
 * The following module parameter controls if the internal test runs when the module is inserted.
 */
static unsigned internal_test = 0;
module_param(internal_test, int, S_IRUGO);


/* 
 * The following data structure represents a single channel of DMA, transmit or receive in the case
 * when using AXI DMA.  It contains all the data to be maintained for the channel.
 */
struct dma_proxy_channel {
	struct xilinx_dma_t* parent;		/* pointer back to parent */
	struct dma_chan *channel_p;			/* DMA support */
	struct completion cmp;
	dma_cookie_t cookie;
	u32 direction;						/* DMA_MEM_TO_DEV or DMA_DEV_TO_MEM */
	struct scatterlist* sglist;
};

struct xilinx_dma_t {
	struct device *proxy_device_p;		/* character device support */
	struct device *dma_device_p;
	dev_t dev_node;
	struct cdev cdev;
	struct class *class_p;
	uint32_t device_index;
	
	struct dma_proxy_channel tx_channel;
	struct dma_proxy_channel rx_channel;
};

enum proxy_status { PROXY_NO_ERROR = 0, PROXY_BUSY = 1, PROXY_TIMEOUT = 2, PROXY_ERROR = 3 };

struct dma_proxy_file_buffer_t {
	dma_addr_t phys_addr;			/* bus address of DMA buffer */
	void *virt_address;     		/* kernel virtual address of the DMA buffer */
	size_t size;          			/* buffer size in bytes */
};

struct dma_proxy_file_t {
	struct xilinx_dma_t* proxy_dev;	/* owning device for this buffer */
	struct dma_proxy_file_buffer_t* buffer_list; /* array of underlying buffers */
	uint32_t buffer_list_size;		/* length in entries of the buffer list array */
	uint32_t buffer_list_max_idx;	/* index of last used entry in buffer list */
	enum proxy_status status;		/* status of the proxy channel */
};

static struct xilinx_dma_t* DEVICES[MAX_DEVICES];


/*
 * Helper function to increase the buffer list in increments of BUFFER_LIST_INCREMENT
 */ 
static void _grow_buffer_list(struct dma_proxy_file_t* proxy_file)
{
	if (proxy_file == NULL)
	{
		return;
	}
	
	proxy_file->buffer_list_size += BUFFER_LIST_INCREMENT;
	size_t new_size = proxy_file->buffer_list_size * 
					  sizeof(struct dma_proxy_file_buffer_t);
					  
	proxy_file->buffer_list = krealloc(proxy_file->buffer_list, new_size, GFP_KERNEL);
}

/* 
 * Handle a callback and indicate the DMA transfer is complete to another
 * thread of control
 */
static void sync_callback(void *completion)
{
	/* 
	 * Indicate the DMA transaction completed to allow the other
	 * thread of control to finish processing
	 */
	complete(completion);
}

/* 
 * Prepare a DMA buffer to be used in a DMA transaction, submit it to the DMA engine
 * to be queued and return a cookie that can be used to track that status of the
 * transaction
 */
static int start_transfer(struct dma_proxy_channel* pchannel_p, 
						  struct dma_proxy_file_t* proxy_file,
						  struct dma_proxy_rw_info* rw_info)
{
	// Determine the start index into the buffer/SG list and the offset into
	// the first buffer
	uint32_t offset_remaining = rw_info->offset;
	uint32_t start_idx = 0;
	for (start_idx = 0; start_idx < proxy_file->buffer_list_max_idx + 1; start_idx++)
	{
		if (offset_remaining > 0 &&
			offset_remaining >= proxy_file->buffer_list[start_idx].size)
		{
			offset_remaining -= proxy_file->buffer_list[start_idx].size;
		}
		else
		{
			// The transfer should start at an offset (possibly zero) within
			// the buffer at this index
			break;
		}
	}
	
	// Determine the end index and "tail size"
	uint32_t tail_size = 0;
	uint32_t size_counter = 0;
	uint32_t end_idx = 0;
	for (end_idx = start_idx; end_idx < proxy_file->buffer_list_max_idx + 1; end_idx++)
	{
		if (end_idx == start_idx)
		{
			size_counter = proxy_file->buffer_list[end_idx].size - offset_remaining;
		}
		else
		{
			size_counter += proxy_file->buffer_list[end_idx].size;
		}
		
		if (size_counter >= rw_info->length)
		{
			tail_size = rw_info->length - (size_counter - proxy_file->buffer_list[end_idx].size);
			
			// The transfer should end at the buffer at this index
			break;
		}
	}
	
	enum dma_ctrl_flags flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
	struct dma_device *dma_device = pchannel_p->channel_p->device;
	
	// Allocate scatter/gather list
	uint32_t num_buffers = (end_idx - start_idx) + 1;
	pchannel_p->sglist = kmalloc(sizeof(struct scatterlist) * num_buffers, GFP_KERNEL);
	
	// Setup the scatter/gather table and add each buffer to it
	sg_init_table(pchannel_p->sglist, num_buffers);
	struct scatterlist* sg_entry;
	uint32_t idx = 0;
	uint32_t buf_idx = start_idx;
	for_each_sg(pchannel_p->sglist, sg_entry, num_buffers, idx)
	{
		if (buf_idx == start_idx)
		{
			// Start buffer possibly with offset
			sg_dma_address(sg_entry) = proxy_file->buffer_list[buf_idx].phys_addr + 
								   	   offset_remaining;
			if (num_buffers > 1)
			{
				sg_dma_len(sg_entry) = proxy_file->buffer_list[buf_idx].size - 
									   offset_remaining;
			}
			else
			{
				sg_dma_len(sg_entry) = rw_info->length;
			}
		}
		else if (buf_idx > start_idx && buf_idx < end_idx)
		{
			// Middle buffer
			sg_dma_address(sg_entry) = proxy_file->buffer_list[buf_idx].phys_addr;
			sg_dma_len(sg_entry) = proxy_file->buffer_list[buf_idx].size;
		}
		else if (buf_idx == end_idx)
		{
			// End buffer possibly with tail size
			sg_dma_address(sg_entry) = proxy_file->buffer_list[buf_idx].phys_addr;
			sg_dma_len(sg_entry) = tail_size;
		}
		
		++buf_idx;
	}
	
	struct dma_async_tx_descriptor *chan_desc;
	chan_desc = dma_device->device_prep_slave_sg(pchannel_p->channel_p, 
												 pchannel_p->sglist, num_buffers, 
												 pchannel_p->direction, flags, 
												 NULL);

	/*
	 *  Make sure the operation was completed successfully
	 */
	if (! chan_desc) 
	{
		pr_err("dmaengine_prep*() error\n");
		return -1;
	} 
	else 
	{
		chan_desc->callback = sync_callback;
		chan_desc->callback_param = &pchannel_p->cmp;

		/* 
		 * Initialize the completion for the transfer and before using it
		 * then submit the transaction to the DMA engine so that it's queued
		 * up to be processed later and get a cookie to track it's status
		 */
		init_completion(&pchannel_p->cmp);
		
		pchannel_p->cookie = dmaengine_submit(chan_desc);
		if (dma_submit_error(pchannel_p->cookie)) 
		{
			pr_err("Submit error\n");
	 		return -2;
		}
		
		/* 
		 * Start the DMA transaction which was previously queued up in the DMA engine
		 */
		dma_async_issue_pending(pchannel_p->channel_p);
	}

	return 0;
}

/* Wait for a DMA transfer that was previously submitted to the DMA engine
 */
static void wait_for_transfer(struct dma_proxy_channel *pchannel_p,
							  struct dma_proxy_file_t* proxy_file)
{
	unsigned long timeout = msecs_to_jiffies(3000);
	enum dma_status status;

	proxy_file->status = PROXY_BUSY;
	
	/* 
	 * Wait for the transaction to complete, or timeout, or get an error
	 */
	timeout = wait_for_completion_timeout(&pchannel_p->cmp, timeout);
	dma_cookie_t last_completed;
	dma_cookie_t last_issued;
	
	status = dma_async_is_tx_complete(pchannel_p->channel_p, pchannel_p->cookie, 
									  &last_completed, &last_issued);
	
	if (timeout == 0)  
	{
		proxy_file->status = PROXY_TIMEOUT;
		pr_err("DMA timed out\n");
	} 
	else if (status != DMA_COMPLETE) 
	{
		proxy_file->status = PROXY_ERROR;
		pr_err("DMA returned completion callback status of: %s\n",
			   status == DMA_ERROR ? "error" : "in progress");
	} 
	else
	{
		proxy_file->status = PROXY_NO_ERROR;
	}
	
	if (pchannel_p->sglist)
	{
		kfree(pchannel_p->sglist);
		pchannel_p->sglist = NULL;
	}
}

/**
 * Empty placeholder function for a required function
 */
static void dma_proxy_vma_open(struct vm_area_struct *vma)
{
}

/**
 * Free all of the DMA buffers that were allocated in the mmap call. This
 * function gets called after the munmap syscall returns success.
 */
static void dma_proxy_vma_close(struct vm_area_struct *vma)
{
	struct file *file = vma->vm_file;
	struct dma_proxy_file_t* proxy_file = (struct dma_proxy_file_t*)file->private_data;
	struct xilinx_dma_t* xilinx_dma = proxy_file->proxy_dev;
	
	for (uint32_t idx = 0; idx < proxy_file->buffer_list_max_idx + 1; ++idx)
	{
		// Free the DMA buffer that was allocated in the mmap call
		dma_free_coherent(xilinx_dma->dma_device_p, 
						  proxy_file->buffer_list[idx].size, 
						  proxy_file->buffer_list[idx].virt_address, 
						  proxy_file->buffer_list[idx].phys_addr);
	
		// Reset values just in case
		memset(&proxy_file->buffer_list[idx], 0, 
			   sizeof(struct dma_proxy_file_buffer_t));
	}
}

static struct vm_operations_struct dma_proxy_vm_ops = {
    .open  = dma_proxy_vma_open,
    .close = dma_proxy_vma_close,
};

/** 
 * Map buffer memory.
 */
static int mmap(struct file *file, struct vm_area_struct *vma)
{
	struct dma_proxy_file_t* proxy_file = (struct dma_proxy_file_t*)file->private_data;
	struct xilinx_dma_t* xilinx_dma = proxy_file->proxy_dev;
	struct dma_proxy_file_buffer_t* buffer_entry = NULL;

	// Determine the size that userspace is attempting to mmap
    size_t size = vma->vm_end - vma->vm_start;
	size_t size_remaining = size;
	
	// NOTE: assume each file only has one buffer which is enforced by the library layer
	uint32_t idx = 0; 
	size_t attempt_size = 0;
	while (size_remaining > 0)
	{
		// Determine how much to allocate for this buffer
		attempt_size = min(size_remaining, (size_t)MAX_BUF_SIZE);
		
		// Increase the size of the buffer list if needed
		if (idx == proxy_file->buffer_list_size)
		{
			_grow_buffer_list(proxy_file);
		}
		buffer_entry = &proxy_file->buffer_list[idx];
		
		if (! is_power_of_2(attempt_size))
		{
			/*
			 * Round down to the nearest power of 2; NOTE: the library will
			 * round the user requested size up to the nearest PAGE_SIZE 
			 * multiple, so this operation should be safe.
			 */ 
			attempt_size = __rounddown_pow_of_two(attempt_size);
		}
		
		/*
		 * Start by attempting to allocate the largest segment possible. If
		 * the allocation fails decrease the size by a power of 2 and try again.
		 */ 
		for (int order = order_base_2(attempt_size / PAGE_SIZE); order >= 0; order--)
		{
			// Attempt to allocate the DMA buffer segment
			attempt_size = (1 << order) * PAGE_SIZE;
			
			buffer_entry->virt_address = dma_alloc_coherent(xilinx_dma->dma_device_p, 
															attempt_size, 
															&buffer_entry->phys_addr, 
															GFP_KERNEL);
															
			// Check if the allocation succeeded, if so then stop
			if (buffer_entry->virt_address != NULL)
			{
				break;
			}
		}
		
		if (buffer_entry->virt_address == NULL)
		{
			// Allocation failed even at order 0 (i.e. 1 page)... error out
			pr_err("dma_alloc_coherent() -- FAILED\n");
			return -ENOMEM;
		}
		
		// Record the buffer's size
		buffer_entry->size = attempt_size;
		size_remaining -= attempt_size;
		++idx;
	}
	proxy_file->buffer_list_max_idx = idx - 1;
	
	// Install custom VM operations
    vma->vm_ops = &dma_proxy_vm_ops;

    // Sneakily map each of the buffers into the VMA one by one
    int rc;
    unsigned long vm_start_orig = vma->vm_start;
    unsigned long vm_end_orig = vma->vm_end;    
    for (idx = 0; idx < proxy_file->buffer_list_max_idx + 1; idx++) 
    {
		// Relabel the buffer list entry
		buffer_entry = &proxy_file->buffer_list[idx];
		
		// Increment VMA start and end addresses (offset)
		if (idx > 0)
		{
			vma->vm_start = vma->vm_end;
		}
		vma->vm_end = vma->vm_start + buffer_entry->size;
		
		rc = dma_mmap_coherent(xilinx_dma->dma_device_p, vma,
						   	   buffer_entry->virt_address, 
							   buffer_entry->phys_addr, 
							   buffer_entry->size);
							   
        if (rc != 0) 
        {
            pr_err("dma_mmap_coherent: %d (IDX = %u)\n", rc, idx);
            
            // Mapping failed unroll buffer allocation 
			for (uint32_t idx = 0; idx < proxy_file->buffer_list_max_idx + 1; ++idx)
			{
				// Free the DMA buffer 
				dma_free_coherent(xilinx_dma->dma_device_p, 
								  proxy_file->buffer_list[idx].size, 
								  proxy_file->buffer_list[idx].virt_address, 
								  proxy_file->buffer_list[idx].phys_addr);
			
				// Reset values just in case
				memset(&proxy_file->buffer_list[idx], 0,
					   sizeof(struct dma_proxy_file_buffer_t));
			}
			proxy_file->buffer_list_max_idx = 0;
            
            return -EAGAIN;
        }
    }
    
    // Restore original VMA bounds addresses
    vma->vm_start = vm_start_orig;
    vma->vm_end = vm_end_orig;
    
    return rc;
}

/** 
 * Open the device file, allocate per file structure, and set up the data pointer 
 * such that the ioctl function can access the data structure later.
 */
static int local_open(struct inode *ino, struct file *file)
{
	// Grab pointer to the proxy device structure
	struct xilinx_dma_t* xilinx_dma = container_of(ino->i_cdev, struct xilinx_dma_t, cdev);
	 
	// Allocate per file (i.e. buffer) structure
	struct dma_proxy_file_t* proxy_file = kzalloc(sizeof(struct dma_proxy_file_t), GFP_KERNEL);
	if (proxy_file == NULL) 
	{
		return -ENOMEM;
	}
	
	// Setup the file buffer list (array)
	size_t size = sizeof(struct dma_proxy_file_buffer_t) * BUFFER_LIST_INCREMENT;
	proxy_file->buffer_list = kzalloc(size, GFP_KERNEL);
	if (proxy_file->buffer_list == NULL) 
	{
		return -ENOMEM;
	}
	
	proxy_file->buffer_list_size = BUFFER_LIST_INCREMENT;
	
	// Store pointer to parent device and stash in file
	proxy_file->proxy_dev = xilinx_dma;
	file->private_data = proxy_file;
	
	return 0;
}

/** 
 * Close the file and free the per file data structure
 */
static int release(struct inode *ino, struct file *file)
{
	struct dma_proxy_file_t* proxy_file = (struct dma_proxy_file_t*)file->private_data;
	
	// Check status and wait if busy???
	
	// Free the buffer list (array)
	kfree(proxy_file->buffer_list);
	
	// Free file struct
    kfree(proxy_file);
	
	return 0;
}

/* Perform I/O control to start a DMA transfer.
 */
static long ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct dma_proxy_file_t* proxy_file = (struct dma_proxy_file_t*)file->private_data;
	struct xilinx_dma_t* xilinx_dma = proxy_file->proxy_dev;
	
	long ret = 0;
	int rc = 0;
	struct dma_proxy_rw_info rw_info;

	// Copy the R/W info struct in from userspace
	ret = copy_from_user(&rw_info, (void *)arg, sizeof(struct dma_proxy_rw_info));
	if (ret) 
	{
		return ret;
	}
				
	// Double check before indexing into array
	if (rw_info.device_index >= MAX_DEVICES)
	{
		return -17;
	}
	
	// Grab the specified device and attempt to use it
	struct xilinx_dma_t* device_to_use = DEVICES[rw_info.device_index];
	if (device_to_use == NULL)
	{
		return -17;
	}

	// Decode the IOCTL
    switch (cmd) {
		
		case DMA_PROXY_IOC_READ_BLOCKING:
            rc = start_transfer(&device_to_use->rx_channel, proxy_file, &rw_info);
            if (rc)
            {
				return -1;
			}
            
            wait_for_transfer(&device_to_use->rx_channel, proxy_file);
			break;
			
		case DMA_PROXY_IOC_START_READ:
            rc = start_transfer(&device_to_use->rx_channel, proxy_file, &rw_info);
            if (rc)
            {
				return -1;
			}
			break;
		
		case DMA_PROXY_IOC_COMPLETE_READ:
			wait_for_transfer(&device_to_use->rx_channel, proxy_file);
			break;
		
		case DMA_PROXY_IOC_WRITE_BLOCKING:
            rc = start_transfer(&device_to_use->tx_channel, proxy_file, &rw_info);
            if (rc)
            {
				return -1;
			}
            
            wait_for_transfer(&device_to_use->tx_channel, proxy_file);
			break;
		
		case DMA_PROXY_IOC_START_WRITE:
            rc = start_transfer(&device_to_use->tx_channel, proxy_file, &rw_info);
            if (rc)
            {
				return -1;
			}
			break;
			
		case DMA_PROXY_IOC_COMPLETE_WRITE:
			wait_for_transfer(&device_to_use->tx_channel, proxy_file);
			break;
		
		default:
            ret = -ENOTTY;
            break;
	}

	return ret;
}

static struct file_operations dm_fops = {
	.owner    = THIS_MODULE,
	.open     = local_open,
	.release  = release,
	.unlocked_ioctl = ioctl,
	.mmap	= mmap
};

/** 
 * Initialize the driver to be a character device such that is responds to
 * file operations.
 */
static int cdevice_init(struct xilinx_dma_t *xilinx_dma_p, char *name, uint32_t index)
{
	int rc;
	char device_name[20];
	snprintf(device_name, 20, "%s%d", name, index);

	/* 
	 * Allocate a character device from the kernel for this driver.
	 */
	rc = alloc_chrdev_region(&xilinx_dma_p->dev_node, 0, 1, device_name);
	if (rc) {
		dev_err(xilinx_dma_p->dma_device_p, "unable to get a char device number\n");
		return rc;
	}

	/* 
	 * Initialize the device data structure before registering the character 
	 * device with the kernel.
	 */
	cdev_init(&xilinx_dma_p->cdev, &dm_fops);
	xilinx_dma_p->cdev.owner = THIS_MODULE;
	rc = cdev_add(&xilinx_dma_p->cdev, xilinx_dma_p->dev_node, 1);
	if (rc) {
		dev_err(xilinx_dma_p->dma_device_p, "unable to add char device\n");
		goto init_error1;
	}

	/* 
	 * Create the global sysfs class which will be reused by other proxy 
	 * devices if present.
	 */
	if (PROXY_GLOBAL_CLASS == NULL)
	{
		PROXY_GLOBAL_CLASS = class_create(THIS_MODULE, DRIVER_NAME);
		if (IS_ERR(PROXY_GLOBAL_CLASS)) {
			dev_err(xilinx_dma_p->dma_device_p, "unable to create class\n");
			rc = ERROR;
			goto init_error2;
		}
	}
	 
	/*
	 * "Install" the class this this device
	 */ 
	xilinx_dma_p->class_p = PROXY_GLOBAL_CLASS;

	/* 
	 * Create the device node in /dev so the device is accessible as a 
	 * character device
	 */
	xilinx_dma_p->proxy_device_p = device_create(xilinx_dma_p->class_p, NULL,
												 xilinx_dma_p->dev_node, NULL, 
												 device_name);
	if (IS_ERR(xilinx_dma_p->proxy_device_p)) {
		dev_err(xilinx_dma_p->dma_device_p, "unable to create the device\n");
		goto init_error3;
	}

	return 0;

init_error3:
	class_destroy(xilinx_dma_p->class_p);

init_error2:
	cdev_del(&xilinx_dma_p->cdev);

init_error1:
	unregister_chrdev_region(xilinx_dma_p->dev_node, 1);
	return rc;
}

/* 
 * Exit the character device by freeing up the resources that it created and
 * disconnecting itself from the kernel.
 */
static void cdevice_exit(struct xilinx_dma_t *xilinx_dma_p)
{
	/* 
	 * Take everything down in the reverse order from how it was created for 
	 * the char device
	 */
	device_destroy(xilinx_dma_p->class_p, xilinx_dma_p->dev_node);
	--DEVICE_COUNT;
	
	if (DEVICE_COUNT == 0)
	{
		class_destroy(xilinx_dma_p->class_p);
	}
	cdev_del(&xilinx_dma_p->cdev);
	unregister_chrdev_region(xilinx_dma_p->dev_node, 1);
}

/**
 * Create a DMA channel by getting a DMA channel from the DMA Engine and then 
 * setting up the channel as a character device to allow user space control.
 */
static int create_channel(struct platform_device *pdev, struct dma_proxy_channel *pchannel_p, 
							char *name, u32 direction)
{
	int rc;

	/* 
	 * Request the DMA channel from the DMA engine and then use the device from
	 * the channel for the proxy channel also.
	 */
	pchannel_p->channel_p = dma_request_slave_channel(&pdev->dev, name);
	if (!pchannel_p->channel_p) 
	{
		dev_err(pchannel_p->parent->dma_device_p, "DMA channel request error\n");
		return ERROR;
	}
	
	pchannel_p->direction = direction;
	
	return 0;
}

/** 
 * Initialize the dma proxy device driver module.
 */
static int dma_proxy_probe(struct platform_device *pdev)
{
	int rc;

	pr_info("dma_proxy module initialized\n");
	
	/*
	 * Allocate private data for the platform device and stash
	 */ 
	struct xilinx_dma_t* xilinx_dma = kzalloc(sizeof(struct xilinx_dma_t), GFP_KERNEL);
	if (xilinx_dma == NULL) {
		return -EIO;
	}
	platform_set_drvdata(pdev, xilinx_dma);
	
	/*
	 * First, attempt to create TX and RX DMA channels
	 */ 
	xilinx_dma->dma_device_p = &pdev->dev; 
	xilinx_dma->tx_channel.parent = xilinx_dma;
	xilinx_dma->rx_channel.parent = xilinx_dma;
	
	char tx_chan_name[16];
	char rx_chan_name[16];
	
	mutex_lock(&PROXY_GLOBAL_MUTEX);
	
	uint32_t index = 0;
	for (; index < MAX_DEVICES; ++index)
	{
		// Build TX channel name
		snprintf(tx_chan_name, 16, "%s%d", "dma_proxy_tx_", index);
		pr_info("dma_proxy - trying TX chan: %s\n", tx_chan_name);
		
		// Build RX channel name
		snprintf(rx_chan_name, 16, "%s%d", "dma_proxy_rx_", index);
		pr_info("dma_proxy - trying RX chan: %s\n", rx_chan_name);
	 
		rc = create_channel(pdev, &xilinx_dma->tx_channel, tx_chan_name, DMA_MEM_TO_DEV);
		if (rc) {
			continue;
		}
		
		rc = create_channel(pdev, &xilinx_dma->rx_channel, rx_chan_name, DMA_DEV_TO_MEM);
		if (rc) {
			continue;
		}
		
		if (rc == 0)
		{
			break;
		}
	}
	
	if (index == MAX_DEVICES)
	{
		pr_err("unable to create TX/RX channels");
		mutex_unlock(&PROXY_GLOBAL_MUTEX);
		return rc;
	}
	
	/* 
	 * Now initialize the character device 
	 */
	rc = cdevice_init(xilinx_dma, "xilinx_dma_proxy", index);
	if (rc) {
		mutex_unlock(&PROXY_GLOBAL_MUTEX);
		return rc;
	}
	
	/**
	 * Globally register the device
	 */ 
	DEVICES[index] = xilinx_dma;
	xilinx_dma->device_index = index;
	++DEVICE_COUNT;
	mutex_unlock(&PROXY_GLOBAL_MUTEX);
	
	return 0;
}
 
/** 
 * Exit the dma proxy device driver module.
 */
static int dma_proxy_remove(struct platform_device *pdev)
{
	int i = 0;

	pr_info("dma_proxy module exited\n");
	
	/*
	 * Retrieve the xilinx_dma_t
	 */ 
	struct xilinx_dma_t* xilinx_dma = platform_get_drvdata(pdev);

	/* 
	 * Take care of the DMA channels and the any buffers allocated for the DMA 
	 * transfers. The DMA buffers are using managed memory such that it's 
	 * automatically done.
	 */
	struct dma_chan *tx_channel = xilinx_dma->tx_channel.channel_p;	 
	tx_channel->device->device_terminate_all(tx_channel);
	dma_release_channel(tx_channel);
	
	struct dma_chan *rx_channel = xilinx_dma->rx_channel.channel_p;	 
	rx_channel->device->device_terminate_all(rx_channel);
	dma_release_channel(rx_channel);

	/*
	 * Teardown character device
	 */ 
	mutex_lock(&PROXY_GLOBAL_MUTEX);
	cdevice_exit(xilinx_dma);
	DEVICES[xilinx_dma->device_index] = NULL;
	mutex_unlock(&PROXY_GLOBAL_MUTEX);
	
	kfree(xilinx_dma);

	return 0;
}

static const struct of_device_id dma_proxy_of_ids[] = {
	{ .compatible = "xlnx,dma_proxy",},
	{}
};

static struct platform_driver dma_proxy_driver = {
	.driver = {
		.name = "dma_proxy_driver",
		.owner = THIS_MODULE,
		.of_match_table = dma_proxy_of_ids,
	},
	.probe = dma_proxy_probe,
	.remove = dma_proxy_remove,
};

static int __init dma_proxy_init(void)
{
	memset(DEVICES, 0, sizeof(struct xilinx_dma_t*) * MAX_DEVICES);
	
	return platform_driver_register(&dma_proxy_driver);
}

static void __exit dma_proxy_exit(void)
{
	platform_driver_unregister(&dma_proxy_driver);
}

module_init(dma_proxy_init)
module_exit(dma_proxy_exit)

MODULE_AUTHOR("Xilinx, Inc.; BlackLynx Inc.");
MODULE_DESCRIPTION("Zynq DMA Proxy");
MODULE_LICENSE("GPL v2");
