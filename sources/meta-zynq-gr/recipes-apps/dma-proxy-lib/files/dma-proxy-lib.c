#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dma-proxy-lib.h"

#define PAGE_SIZE   sysconf(_SC_PAGESIZE)

typedef struct proxy_buffer_info {
    int fd;                     //!< file descriptor used to create buffer
    size_t size;                //!< buffer size in bytes
    void* buffer;               //!< pointer to buffer
} proxy_buffer_info_t;

typedef struct device_info {
    pthread_mutex_t rd_mutex;      //!< protect READ CV and busy flag
    pthread_mutex_t wr_mutex;      //!< protect WRITE CV and busy flag
    pthread_cond_t rd_cv;          //!< used to wait and signal READ status
    pthread_cond_t wr_cv;          //!< used to wait and signal WRITE status
    bool rd_busy;                  //!< is READ channel busy (i.e. in use)
    bool wr_busy;                  //!< is WRITE channel busy (i.e. in use)
} device_info_t;

enum rw { READ, WRITE };

pthread_mutex_t master_mutex;   // used to make buffer list thread safe
static uint32_t buffer_list_inc = 20;   // increment size by this amount
static proxy_buffer_info_t* buffer_list = NULL;
static uint32_t buffer_list_size = 0;
static uint32_t buffer_list_idx = 0;

static device_info_t* device_status_table[MAX_DEVICES];

static int __dma_proxy_reg_space_fd;
static void *__dma_proxy_reg_space_ptr;


// Make init and fini functions as constructor and destructor
void dma_proxy_lib_init() __attribute__((constructor));
void dma_proxy_lib_fini() __attribute__((destructor));

// Buffer list function
static inline int _resize_buffer_list();
static inline void* _alloc_proxy_buffer(proxy_buffer_info_t* proxy_buffer, size_t size);
static inline int _free_proxy_buffer(proxy_buffer_info_t* proxy_buffer);

/**
 * DMA proxy userspace library "constructor"
 */
void dma_proxy_lib_init()
{
    // Initialize the register space FD and pointer
    __dma_proxy_reg_space_fd = 0;
    __dma_proxy_reg_space_ptr = NULL;
    
    // Allocate initial buffer list
    pthread_mutex_lock(&master_mutex);
    _resize_buffer_list();
    pthread_mutex_unlock(&master_mutex);
    
    // Setup device status table
    for (uint32_t device_index = 0; device_index < MAX_DEVICES; ++device_index)
    {
        // Build representative path
        char path[24];
        snprintf(path, 24, "%s%d", DMA_PROXY_DEVICE_BASE, device_index);
        
        // Check that path exists and is a character device
        struct stat stat_buf;
        int rc = lstat(path, &stat_buf);
        if (rc == 0 && (stat_buf.st_mode & S_IFMT) == S_IFCHR)
        {
            device_status_table[device_index] = calloc(1, sizeof(device_info_t));
            pthread_mutex_init(&device_status_table[device_index]->rd_mutex, NULL);
            pthread_mutex_init(&device_status_table[device_index]->wr_mutex, NULL);
            
            pthread_cond_init(&device_status_table[device_index]->rd_cv, NULL);
            pthread_cond_init(&device_status_table[device_index]->wr_cv, NULL);
            
            device_status_table[device_index]->rd_busy = false;
            device_status_table[device_index]->wr_busy = false;
        }
        else
        {
            device_status_table[device_index] = NULL;
        }
    }
}

/**
 * DMA proxy userspace library "destructor"
 */ 
void dma_proxy_lib_fini()
{
    // If the register address space was mapped, unmap it and clean up
    if (__dma_proxy_reg_space_ptr != NULL)
    {
        munmap(__dma_proxy_reg_space_ptr, REG_SPACE_SIZE);
        __dma_proxy_reg_space_ptr = NULL;
        close(__dma_proxy_reg_space_fd);
        __dma_proxy_reg_space_ptr = 0;
    }
    
    // Clean up any remaining buffers
    pthread_mutex_lock(&master_mutex);
    for (uint32_t idx = 0; idx < buffer_list_size; ++idx)
    {
        if (buffer_list[idx].fd > 0)
        {
            _free_proxy_buffer(&buffer_list[idx]);
        } 
    }
    free(buffer_list);
    pthread_mutex_unlock(&master_mutex);
    
    // Teardown device status table
    for (uint32_t device_index = 0; device_index < MAX_DEVICES; ++device_index)
    {
        if (device_status_table[device_index] != NULL)
        {
            free(device_status_table[device_index]);
        }
    }
}

/**
 * Helper function to round the specified size up to the nearest page size
 */ 
static inline size_t _roundup_nearest_pagesize(size_t size)
{
    // NOTE: integer math used here
    return ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
}

static inline int _resize_buffer_list()
{
    if (buffer_list_size == 0)
    {
        // Buffer list was empty, so create it
        buffer_list = calloc(buffer_list_inc, sizeof(proxy_buffer_info_t));
        if (buffer_list == NULL)
        {
            return -1;
        }
        buffer_list_size = buffer_list_inc;
    }
    else
    {
        // Buffer list not empty, resize it
        size_t new_size = sizeof(proxy_buffer_info_t) * 
                          (buffer_list_size + buffer_list_inc);
        buffer_list = realloc(buffer_list, new_size);
        if (buffer_list == NULL)
        {
            return -1;
        }
        buffer_list_size += buffer_list_inc;
    }
    
    return 0;
}

static inline void* _alloc_proxy_buffer(proxy_buffer_info_t* proxy_buffer, size_t size)
{
    if (proxy_buffer == NULL)
    {
        return NULL;
    }
    
    // Open the default proxy device
    int fd = open(DMA_PROXY_DEVICE_DEFAULT, O_RDWR);
    if (fd < 1) 
    {
        return NULL;
    }
    
    // Round up the user specified size to the nearest page multiple size
    size_t buffer_size = _roundup_nearest_pagesize(size);
    
    // Map the buffer
    proxy_buffer->buffer = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, 
                                MAP_SHARED, fd, 0);
    if (proxy_buffer->buffer == MAP_FAILED)
    {
        return NULL;
    }
    
    proxy_buffer->size = buffer_size;
    proxy_buffer->fd = fd;
    
    return proxy_buffer->buffer;
}

static inline int _free_proxy_buffer(proxy_buffer_info_t* proxy_buffer)
{
    if (proxy_buffer == NULL)
    {
        return -1;
    }
    
    if (proxy_buffer->fd <= 2)
    {
        return -2;
    }
    
    munmap(proxy_buffer->buffer, proxy_buffer->size);
    close(proxy_buffer->fd);
    
    // Mark that this buffer is no longer active
    proxy_buffer->fd = 0;
    
    return 0;
}

static int _map_reg_space()
{
    // Open the /dev/mem device and mmap the register space
    __dma_proxy_reg_space_fd = open("/dev/mem", O_RDWR);
    if (__dma_proxy_reg_space_fd < 1) 
    {
        return -1;
    }
    
    __dma_proxy_reg_space_ptr = mmap(NULL, REG_SPACE_SIZE, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, __dma_proxy_reg_space_fd, 
                                     REG_SPACE_BASE_ADDR);
    if (__dma_proxy_reg_space_ptr == MAP_FAILED) 
    {
        return -2;
    }
    
    return 0;
}

uint32_t dmap_reg_read32(uint32_t offset)
{
    uint32_t value = 0xDEADDEAD;
    
    // Make sure the offset doesn't exceed the size of the mapped space
    if (offset > REG_SPACE_SIZE)
    {
        return value;
    }
    
    // Map the register space if it is not already mapped
    if (__dma_proxy_reg_space_ptr == NULL)
    {
        if (_map_reg_space() != 0)
        {
            return value;
        }
    }
    
    value = *(uint32_t*)(__dma_proxy_reg_space_ptr + offset);
    return value;
}

int dmap_reg_write32(uint32_t offset, uint32_t value)
{
    // Make sure the offset doesn't exceed the size of the mapped space
    if (offset > REG_SPACE_SIZE)
    {
        return DMAP_INVALID_REG_OFFSET;
    }
    
    // Map the register space if it is not already mapped
    if (__dma_proxy_reg_space_ptr == NULL)
    {
        _map_reg_space();
    }
    
    *(uint32_t*)(__dma_proxy_reg_space_ptr + offset) = value;
    
    return 0;
}

void* dmap_alloc_buffer(size_t size)
{
    void* ret = NULL;
    
    // Make sure buffer size does not exceed amount of device memory
    if (size > MAX_DEVICE_MEMORY)
    {
        return ret;
    }
    
    pthread_mutex_lock(&master_mutex);
    
    // Perform linear scan looking for first empty slot in the buffer list
    uint32_t idx = 0;
    for (; idx < buffer_list_size; ++idx)
    {
        if (buffer_list[idx].fd == 0)
        {
            break;
        } 
    }
    if (idx == buffer_list_size)
    {
        // Attempt to resize the buffer list
        if (_resize_buffer_list())
        {
            pthread_mutex_unlock(&master_mutex);
            return ret;
        }
    }
    
    ret = _alloc_proxy_buffer(&buffer_list[idx], size);
    pthread_mutex_unlock(&master_mutex);
    return ret;
}

void dmap_free_buffer(void* buffer)
{
    if (buffer == NULL)
    {
        return;
    }
    
    pthread_mutex_lock(&master_mutex);
    
    // Find the proxy_buffer_info_t that is active who's value matches the 
    // passed buffer pointer, if found free the entry
    for (uint32_t idx = 0; idx < buffer_list_size; ++idx)
    {
        if (buffer_list[idx].buffer == buffer && buffer_list[idx].fd > 0)
        {
            _free_proxy_buffer(&buffer_list[idx]);
            break;
        }
    }
    
    pthread_mutex_unlock(&master_mutex);
    
    return;
}

static int _buffer_select_helper(void* buffer, uint32_t length, struct proxy_buffer_info* selected)
{
    int rc = 0;
    
    pthread_mutex_lock(&master_mutex);
    
    // Find the proxy_buffer_info_t that is active who's value matches the 
    // passed buffer pointer. Note "buffer" may include an offset and therefore
    // not be the start of the buffer.
    uint32_t idx = 0;
    for (; idx < buffer_list_size; ++idx)
    {
        if (buffer && buffer_list[idx].fd > 0 && 
            buffer >= buffer_list[idx].buffer && 
            buffer < (buffer_list[idx].buffer + buffer_list[idx].size))
        {
            // Copy the selected element as a realloc might happen at any time
            // the mutex is not locked
            memcpy(selected, &buffer_list[idx], sizeof(struct proxy_buffer_info));
            break;
        }
    }
    
    // The buffer was not found
    if (idx == buffer_list_size)
    {
        rc = DMAP_INVALID_BUF_PTR;
    }
    
    pthread_mutex_unlock(&master_mutex);
    
    if (rc)
    {
        return rc;
    }
    
    // Validate that buffer address + read length fall in bounds
    if ((buffer + length) > (selected->buffer + selected->size))
    {
        rc = DMAP_ACCESS_OUT_OF_BOUNDS;
    }
    
    return rc;
}

static int _validate_device_index(uint32_t device_index)
{
    if (device_index >= MAX_DEVICES)
    {
        return 0;
    }
    
    return (device_status_table[device_index] != NULL);
}

static inline int _wait_for_device(uint32_t device_index, enum rw type, bool block)
{
    pthread_mutex_t* mutex = NULL;
    pthread_cond_t* cv = NULL;
    bool* busy = NULL;
    
    if (type == READ)
    {
        mutex = &device_status_table[device_index]->rd_mutex;
        cv = &device_status_table[device_index]->rd_cv;
        busy = &device_status_table[device_index]->rd_busy;
    }
    else
    {
        mutex = &device_status_table[device_index]->wr_mutex;
        cv = &device_status_table[device_index]->wr_cv;
        busy = &device_status_table[device_index]->wr_busy;
    }
    
    pthread_mutex_lock(mutex);
    if (*busy)
    {
        if (block)
        {
            // Blocking wait for device to be not busy
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 5000; // 5 sec timeout (in ms)
            
            int rc = 0;
            rc = pthread_cond_timedwait(cv, mutex, &ts);
            if (rc == ETIMEDOUT)
            {
                DMAP_DEVICE_TIMEOUT;
            }
        }
        else
        {
            // Nonblocking, return immediately if device is busy
            pthread_mutex_unlock(mutex);
            return DMAP_DEVICE_IN_USE;
        }
    }

    *busy = true;
    pthread_mutex_unlock(mutex);
    
    return 0;    
}

static inline void _mark_device_ready(uint32_t device_index, enum rw type)
{
    pthread_mutex_t* mutex = NULL;
    pthread_cond_t* cv = NULL;
    bool* busy = NULL;
    
    if (type == READ)
    {
        mutex = &device_status_table[device_index]->rd_mutex;
        cv = &device_status_table[device_index]->rd_cv;
        busy = &device_status_table[device_index]->rd_busy;
    }
    else
    {
        mutex = &device_status_table[device_index]->wr_mutex;
        cv = &device_status_table[device_index]->wr_cv;
        busy = &device_status_table[device_index]->wr_busy;
    }
    
    pthread_mutex_lock(mutex);
    *busy = false;
    pthread_cond_signal(cv);
    pthread_mutex_unlock(mutex);
}

int dmap_read(uint32_t device_index, void* buffer, uint32_t length)
{
    if (buffer == NULL)
    {
        return DMAP_INVALID_BUF_PTR;
    }
    
    if (! _validate_device_index(device_index))
    {
        return DMAP_INVALID_DEVICE_IDX;
    }
    
    struct proxy_buffer_info selected;
    int rc = _buffer_select_helper(buffer, length, &selected);
    if (rc)
    {
        return rc;
    }
    else
    {
        // Wait for device to be ready
        rc = _wait_for_device(device_index, READ, true);
        if (rc)
        {
            return rc;
        }

        struct dma_proxy_rw_info rw_info;
        rw_info.offset = buffer - selected.buffer;;
        rw_info.length = length;
        rw_info.device_index = device_index;
    
        rc = ioctl(selected.fd, DMA_PROXY_IOC_READ_BLOCKING, (unsigned long)&rw_info);
        if (rc == -17)
        {
            rc = DMAP_INVALID_DEVICE_IDX;
        }
        
        // Mark device ready again
        _mark_device_ready(device_index, READ);
    }
    
    return rc;
}

int dmap_read_nb(uint32_t device_index, void* buffer, uint32_t length)
{
    if (buffer == NULL)
    {
        return DMAP_INVALID_BUF_PTR;
    }
    
    if (! _validate_device_index(device_index))
    {
        return DMAP_INVALID_DEVICE_IDX;
    }
    
    struct proxy_buffer_info selected;
    int rc = _buffer_select_helper(buffer, length, &selected);
    if (rc)
    {
        return rc;
    }
    else
    {
        // Wait for device to be ready
        rc = _wait_for_device(device_index, READ, false);
        if (rc)
        {
            return rc;
        }
        
        struct dma_proxy_rw_info rw_info;
        rw_info.offset = buffer - selected.buffer;;
        rw_info.length = length;
        rw_info.device_index = device_index;
    
        rc = ioctl(selected.fd, DMA_PROXY_IOC_START_READ, (unsigned long)&rw_info);
        if (rc == -17)
        {
            rc = DMAP_INVALID_DEVICE_IDX;
        }
    }
    
    return rc;
}

int dmap_read_complete(uint32_t device_index, void* buffer)
{
    if (buffer == NULL)
    {
        return DMAP_INVALID_BUF_PTR;
    }
    
    if (! _validate_device_index(device_index))
    {
        return DMAP_INVALID_DEVICE_IDX;
    }
    
    struct proxy_buffer_info selected;
    int rc = _buffer_select_helper(buffer, 0, &selected);
    if (rc)
    {
        return rc;
    }
    else
    {
        struct dma_proxy_rw_info rw_info;
        rw_info.offset = buffer - selected.buffer;;
        rw_info.length = 0;
        rw_info.device_index = device_index;
        
        rc = ioctl(selected.fd, DMA_PROXY_IOC_COMPLETE_READ, (unsigned long)&rw_info);
        if (rc == -17)
        {
            rc = DMAP_INVALID_DEVICE_IDX;
        }
        
        // Mark device ready again
        _mark_device_ready(device_index, READ);
    }
    
    return rc;
}

int dmap_write(uint32_t device_index, void* buffer, uint32_t length)
{
    if (buffer == NULL)
    {
        return DMAP_INVALID_BUF_PTR;
    }
    
    if (! _validate_device_index(device_index))
    {
        return DMAP_INVALID_DEVICE_IDX;
    }
    
    struct proxy_buffer_info selected;
    int rc = _buffer_select_helper(buffer, length, &selected);
    if (rc)
    {
        return rc;
    }
    else
    {
        // Wait for device to be ready
        rc = _wait_for_device(device_index, WRITE, true);
        if (rc)
        {
            return rc;
        }
        
        struct dma_proxy_rw_info rw_info;
        rw_info.offset = buffer - selected.buffer;;
        rw_info.length = length;
        rw_info.device_index = device_index;
    
        rc = ioctl(selected.fd, DMA_PROXY_IOC_WRITE_BLOCKING, (unsigned long)&rw_info);
        if (rc == -17)
        {
            rc = DMAP_INVALID_DEVICE_IDX;
        }
        
        // Mark device ready again
        _mark_device_ready(device_index, WRITE);
    }
    
    return rc;
}

int dmap_write_nb(uint32_t device_index, void* buffer, uint32_t length)
{
    if (buffer == NULL)
    {
        return DMAP_INVALID_BUF_PTR;
    }
    
    struct proxy_buffer_info selected;
    int rc = _buffer_select_helper(buffer, length, &selected);
    if (rc)
    {
        return rc;
    }
    else
    {
        // Wait for device to be ready
        rc = _wait_for_device(device_index, WRITE, false);
        if (rc)
        {
            return rc;
        }
        
        struct dma_proxy_rw_info rw_info;
        rw_info.offset = buffer - selected.buffer;;
        rw_info.length = length;
        rw_info.device_index = device_index;
    
        rc = ioctl(selected.fd, DMA_PROXY_IOC_START_WRITE, (unsigned long)&rw_info);
        if (rc == -17)
        {
            rc = DMAP_INVALID_DEVICE_IDX;
        }
    }
    
    return rc;
}

int dmap_write_complete(uint32_t device_index, void* buffer)
{
    if (buffer == NULL)
    {
        return DMAP_INVALID_BUF_PTR;
    }
    
    if (! _validate_device_index(device_index))
    {
        return DMAP_INVALID_DEVICE_IDX;
    }
    
    struct proxy_buffer_info selected;
    int rc = _buffer_select_helper(buffer, 0, &selected);
    if (rc)
    {
        return rc;
    }
    else
    {
        struct dma_proxy_rw_info rw_info;
        rw_info.offset = buffer - selected.buffer;;
        rw_info.length = 0;
        rw_info.device_index = device_index;
        
        rc = ioctl(selected.fd, DMA_PROXY_IOC_COMPLETE_WRITE, 0);
        if (rc == -17)
        {
            rc = DMAP_INVALID_DEVICE_IDX;
        }
        
        // Mark device ready again
        _mark_device_ready(device_index, WRITE);
    }
    
    return rc;
}
