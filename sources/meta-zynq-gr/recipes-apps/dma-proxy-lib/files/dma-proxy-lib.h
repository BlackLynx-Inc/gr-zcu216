#ifndef __DMA_PROXY_LIB__
#define __DMA_PROXY_LIB__

#include <stdint.h>
#include <linux/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DMA_PROXY_DEVICE_DEFAULT    "/dev/xilinx_dma_proxy0"
#define DMA_PROXY_DEVICE_BASE		"/dev/xilinx_dma_proxy"
#define MAX_DEVICE_MEMORY		    0x80000000L

#define REG_SPACE_BASE_ADDR 	    0xA0000000
#define REG_SPACE_SIZE			    0x10000000			// 256 MB

// NOTE: this is currently duplicated, even though it really shouldn't be
#define DMA_PROXY_IOCTL_MAGIC  'b'
#define DMA_PROXY_IOC_READ_BLOCKING		_IOW(DMA_PROXY_IOCTL_MAGIC,  0, int)  //!< IOCTL: read from PL blocking
#define DMA_PROXY_IOC_WRITE_BLOCKING	_IOW(DMA_PROXY_IOCTL_MAGIC,  1, int)  //!< IOCTL: write to PL blocking
#define DMA_PROXY_IOC_START_READ		_IOW(DMA_PROXY_IOCTL_MAGIC,  2, int)  //!< IOCTL: start read from PL (nonblocking)
#define DMA_PROXY_IOC_COMPLETE_READ		_IOW(DMA_PROXY_IOCTL_MAGIC,  3, int)  //!< IOCTL: complete read from PL (nonblocking)
#define DMA_PROXY_IOC_START_WRITE		_IOW(DMA_PROXY_IOCTL_MAGIC,  4, int)  //!< IOCTL: start write from PL (nonblocking)
#define DMA_PROXY_IOC_COMPLETE_WRITE	_IOW(DMA_PROXY_IOCTL_MAGIC,  5, int)  //!< IOCTL: complete write from PL (nonblocking)
#define MAX_DEVICES     8

struct dma_proxy_rw_info {
    uint32_t offset;       	//!< offset in bytes from start of buffer
    uint32_t length;        //!< buffer length
    uint32_t device_index;  //!< index of device to use for operation
};

// RETURN CODES
#define DMAP_INVALID_REG_OFFSET		1
#define DMAP_INVALID_BUF_PTR		2
#define DMAP_INVALID_DEVICE_IDX 	3
#define DMAP_ACCESS_OUT_OF_BOUNDS	4
#define DMAP_DEVICE_IN_USE      	5
#define DMAP_DEVICE_TIMEOUT      	6


/**
 * Read 32 bit value from the specified offset in the register space.
 */ 
uint32_t dmap_reg_read32(uint32_t offset);

/**
 * Write 32 bit specified value to the specified offset in the register space.
 */ 
int dmap_reg_write32(uint32_t offset, uint32_t value);

/**
 * Allocate a DMA buffer of the specified size.
 */ 
void* dmap_alloc_buffer(size_t size);

/**
 * Free a previously allocated DMA buffer.
 */ 
void dmap_free_buffer(void* buffer);

/**
 * Perform blocking DMA read: PL --> host buffer
 */ 
int dmap_read(uint32_t device_index, void* buffer, uint32_t length);

/**
 * Start nonblocking DMA read: PL --> host buffer
 */ 
int dmap_read_nb(uint32_t device_index, void* buffer, uint32_t length);

/**
 * Wait for previously started read to complete. This call blocks.
 */ 
int dmap_read_complete(uint32_t device_index, void* buffer);

/**
 * Perform blocking DMA write: host buffer --> PL
 */ 
int dmap_write(uint32_t device_index, void* buffer, uint32_t length);

/**
 * Start nonblocking DMA write: host buffer --> PL
 */ 
int dmap_write_nb(uint32_t device_index, void* buffer, uint32_t length);

/**
 * Wait for previously started write operation to complete. This call blocks.
 */ 
int dmap_write_complete(uint32_t device_index, void* buffer);



#ifdef __cplusplus
}  // End of extern "C"
#endif

#endif
