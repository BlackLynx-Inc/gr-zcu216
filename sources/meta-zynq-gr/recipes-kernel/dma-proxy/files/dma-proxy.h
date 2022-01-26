/*  dma-proxy.h - The simplest kernel module.
*
* Copyright (C) 2013 - 2016 Xilinx, Inc
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
*   This header file is shared between the DMA Proxy test application and the DMA Proxy device driver. It defines the
*   shared interface to allow DMA transfers to be done from user space.
*
*   Note: the buffer in the data structure should be 1st in the channel interface so that the buffer is cached aligned,
*   otherwise there may be issues when using cached memory. The issues were typically the 1st 32 bytes of the buffer
*   not working in the driver test.
*/
#include <linux/ioctl.h>

#define DMA_PROXY_IOCTL_MAGIC  'b'
#define DMA_PROXY_IOC_READ_BLOCKING		_IOW(DMA_PROXY_IOCTL_MAGIC,  0, int)  //!< IOCTL: read from PL blocking
#define DMA_PROXY_IOC_WRITE_BLOCKING	_IOW(DMA_PROXY_IOCTL_MAGIC,  1, int)  //!< IOCTL: write to PL blocking
#define DMA_PROXY_IOC_START_READ		_IOW(DMA_PROXY_IOCTL_MAGIC,  2, int)  //!< IOCTL: start read from PL (nonblocking)
#define DMA_PROXY_IOC_COMPLETE_READ		_IOW(DMA_PROXY_IOCTL_MAGIC,  3, int)  //!< IOCTL: complete read from PL (nonblocking)
#define DMA_PROXY_IOC_START_WRITE		_IOW(DMA_PROXY_IOCTL_MAGIC,  4, int)  //!< IOCTL: start write from PL (nonblocking)
#define DMA_PROXY_IOC_COMPLETE_WRITE	_IOW(DMA_PROXY_IOCTL_MAGIC,  5, int)  //!< IOCTL: complete write from PL (nonblocking)

#define MAX_DEVICES             8
#define MAX_BUF_SIZE        	((1 << (MAX_ORDER - 1)) * PAGE_SIZE)  
#define BUFFER_LIST_INCREMENT 	10

// NOTE the library needs this too
struct dma_proxy_rw_info {
    uint32_t offset;       	//!< offset in bytes from start of buffer
    uint32_t length;        //!< buffer length
    uint32_t device_index;  //!< index of device to use for operation
};
