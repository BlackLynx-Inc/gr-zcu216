#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <dma-proxy-lib.h>


char* RATE_UNITS[] = 
{
    "B/s",
    "KB/s",
    "MB/s",
    "GB/s",
    "TB/s"
};

double calc_data_rate(uint32_t bytes, double time, char** unit)
{
    uint32_t ctr = 0;
    double data_rate = (double)bytes / time;
    
    while (data_rate >= 1024.0)
    {
        data_rate /= 1024.0;
        ++ctr;
    }
    
    *unit = RATE_UNITS[ctr];
    return data_rate;
}

int main(int argc, char** argv)
{
    printf("DMA Proxy Util NonBlocking\n\n");
    
    // Set default arguments and override with command line arguments as needed
    uint32_t device_index = 0;
    size_t buf_size = 1024 * 1024;
    uint32_t offset = 0;
    uint32_t xfer_len = buf_size;
    if (argc == 1)
    {
        ;
    }
    else if (argc == 2)
    {
        device_index = strtol(argv[1], NULL, 10);
    }
    else if (argc == 3)
    {
        device_index = strtol(argv[1], NULL, 10);
        buf_size = strtol(argv[2], NULL, 0);
        xfer_len = buf_size;
    }
    else if (argc == 4)
    {
        fprintf(stderr, "ERROR: please specify offset and length: "
                "./dma-proxy-util [device index] [buffer size] [buffer offset] [xfer length]\n");
        return -1;
    }
    else if (argc == 5)
    {
        device_index = strtol(argv[1], NULL, 10);
        buf_size = strtol(argv[2], NULL, 0);
        offset = strtol(argv[3], NULL, 0);
        xfer_len = strtol(argv[4], NULL, 0);
        printf("Setting buffer size to: %u; offset: 0x%08X; xfer len: %u\n", 
               buf_size, offset, xfer_len);
    }
    else
    {
        fprintf(stderr, "ERROR: too many arguments specified: "
                "./dma-proxy-util [device index] [buffer size] [buffer offset] [xfer length]\n");
        return -1;
    }
    
    printf("Buffer size: %u\n", buf_size);
    printf("Buffer offset: %u\n", offset);
    printf("Xfer length: %u\n", xfer_len);
    fflush(stdout);
    
    // Allocate transmit buffer
    char* tx_dma_buffer = (char*)dmap_alloc_buffer(buf_size);
    if (tx_dma_buffer == NULL)
    {
        fprintf(stderr, "[WRT] DMA buffer allocation failed\n");
        return -1;
    }
    
    // Write unique pattern to transmit buffer
    memset(tx_dma_buffer, 0xC3, buf_size);
    
    // Allocate receive buffer
    char* rx_dma_buffer = (char*)dmap_alloc_buffer(buf_size);
    if (rx_dma_buffer == NULL)
    {
        fprintf(stderr, "DMA buffer allocation failed\n");
        return -1;
    }
    
    // Write a pattern to the recieve buffer 
    memset(rx_dma_buffer, 0x5A, buf_size);
    
    printf("Buffer contents BEFORE: 0x%08X\n", *(uint32_t*)rx_dma_buffer);

    struct timeval tval_start, tval_end, tval_result;
    gettimeofday(&tval_start, NULL);

    // Kick off the read operation
    int rc = dmap_read_nb(device_index, (void*)rx_dma_buffer + offset, xfer_len);
    if (rc)
    {
        fprintf(stderr, "[WRT] DMA read failed: %d\n", rc);
    }
    
    // Write/transmit the data
    rc = dmap_write(device_index, (void*)tx_dma_buffer + offset, xfer_len);
    if (rc)
    {
        fprintf(stderr, "[WRT] DMA write failed: %d\n", rc);
    }
    
    // Wait for the read operation to complete
    rc = dmap_read_complete(device_index, (void*)rx_dma_buffer);
    if (rc)
    {
        fprintf(stderr, "[WRT] DMA read failed: %d\n", rc);
    }
    
    gettimeofday(&tval_end, NULL);
    timersub(&tval_end, &tval_start, &tval_result);
    
    printf("Buffer contents AFTER:  0x%08X\n", *(uint32_t*)rx_dma_buffer);
    
    // Validate the read/receive buffer data if there were not transfer errors;
    // NOTE: this can be pretty slow for large buffer sizes
    if (! rc)
    {
        bool miscompares = false;
        uint64_t* tx_data = (uint64_t*)tx_dma_buffer;
        uint64_t* rx_data = (uint64_t*)rx_dma_buffer; 
        for (uint32_t idx = 0; idx < (buf_size / sizeof(uint64_t)); ++idx)
        {
            if (tx_data[idx] != rx_data[idx])
            {
                fprintf(stderr, "MISCOMPARE: 0x%08X -- 0x%016llX -- 0x%016llX\n", 
                        (idx * sizeof(uint64_t)), tx_data[idx], rx_data[idx]);
                miscompares = true;
            }
#if 0
            else
            {
                fprintf(stderr, "            0x%08X -- 0x%016llX -- 0x%016llX\n", 
                        (idx * sizeof(uint64_t)), tx_data[idx], rx_data[idx]);
            }
#endif
        }
        
        if (miscompares)
        {
            printf("\nCompare UNSUCCESSFUL\n\n");
        }
        else
        {
            printf("\nCompare SUCCESSFUL\n\n");
        }
    }
    
    // Display elapsed time
    printf("Time elapsed: %ld.%06ld s\n", (long int)tval_result.tv_sec, 
                                          (long int)tval_result.tv_usec);
    double time = (double)tval_result.tv_sec;
    
    // Calculate data rate and scale it 
    time += ((double)tval_result.tv_usec / (double)1000000.0);    
    char* unit = NULL;
    double rate = calc_data_rate(xfer_len, time, &unit);
    
    printf("Data rate:    %.2f %s\n", rate, unit);
    
    // Free the buffers
    dmap_free_buffer(tx_dma_buffer);
    dmap_free_buffer(rx_dma_buffer);
    
    printf("[FIN]\n");
    
    return 0;
}
