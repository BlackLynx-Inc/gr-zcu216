#include <errno.h>
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

int64_t get_file_size(FILE* file)
{    
    if (fseek(file, 0, SEEK_END) == -1)
    {
        return -1;
    }
    
    int64_t size = ftell(file);
    if (size < 0)
    {
        return -2;
    }
    
    // Backup to the beginning of the file
    rewind(file);
    
    return size;
}

int main(int argc, char** argv)
{
    printf("DMA Proxy Util File (NonBlocking)\n\n");
    
    // Set default arguments and override with command line arguments as needed
    uint32_t device_index = 0;
    size_t buf_size = 1024 * 1024;
    uint32_t xfer_len = buf_size;
    char* input_filepath = NULL;
    char* output_filepath = NULL;
    
    if (argc == 5)
    {
        device_index = strtol(argv[1], NULL, 10);
        buf_size = strtol(argv[2], NULL, 0);
        xfer_len = buf_size;
        input_filepath = argv[3];
        output_filepath = argv[4];
    }
    else
    {
        fprintf(stderr, "ERROR: improper arguments specified: "
                "./dma-proxy-util [device index] [buffer size] [input filepath]"
                " [output filepath]\n");
        return -1;
    }
    
    printf("Buffer size: %u\n", buf_size);
    printf("Xfer length: %u\n", xfer_len);
    printf("Input filepath: %s\n", input_filepath);
    printf("Output filepath: %s\n", output_filepath);
    fflush(stdout);
    
    // Open the input file for reading
    FILE* input_file = fopen(input_filepath, "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "ERROR: unable to open input file: %s\n", input_filepath);
        return -1;
    }
    
    // Open the output file for writing
    FILE* output_file = fopen(output_filepath, "w");
    if (output_file == NULL)
    {
        fprintf(stderr, "ERROR: unable to open output file: %s\n", output_filepath);
        return -1;
    }
    
    // Determine input file size
    int64_t file_size = get_file_size(input_file);
    if (file_size < 0)
    {
        fprintf(stderr, "ERROR: unable to determine input file size\n");
        return -1;
    }
    
    // Allocate transmit buffer
    char* tx_dma_buffer = (char*)dmap_alloc_buffer(buf_size);
    if (tx_dma_buffer == NULL)
    {
        fprintf(stderr, "TX DMA buffer allocation failed\n");
        return -1;
    }
    
    // Write unique pattern to transmit buffer
    memset(tx_dma_buffer, 0xC3, buf_size);
    
    // Allocate receive buffer
    char* rx_dma_buffer = (char*)dmap_alloc_buffer(buf_size);
    if (rx_dma_buffer == NULL)
    {
        fprintf(stderr, "RX DMA buffer allocation failed\n");
        return -1;
    }
    
    int64_t bytes_remaining = file_size;
    uint32_t xfer_bytes = xfer_len;
    size_t bytes_read = 0;
    int rc = 0;
    size_t bytes_written = 0;
    
    // Set start time
    struct timeval tval_start, tval_end, tval_result;
    gettimeofday(&tval_start, NULL);
    
    while (bytes_remaining > 0)
    {
        xfer_bytes = ((bytes_remaining > xfer_bytes) ? xfer_len : bytes_remaining);
        
        // Read the input file data into 
        bytes_read = fread((void*)tx_dma_buffer, 1, xfer_bytes, input_file);
        if (bytes_read != xfer_bytes)
        {
            fprintf(stderr, "Unexpected number of bytes read: %d -- %d\n", 
                    bytes_read, errno);
        }
        
        // Kick off the read operation
        rc = dmap_read_nb(device_index, (void*)rx_dma_buffer, xfer_bytes);
        if (rc)
        {
            fprintf(stderr, "DMA read failed: %d\n", rc);
        }
        
        // Write/transmit the data
        rc = dmap_write(device_index, (void*)tx_dma_buffer, xfer_bytes);
        if (rc)
        {
            fprintf(stderr, "DMA write failed: %d\n", rc);
        }
        
        // Wait for the read operation to complete
        rc = dmap_read_complete(device_index, (void*)rx_dma_buffer);
        if (rc)
        {
            fprintf(stderr, "DMA read failed: %d\n", rc);
        }
        
        bytes_written = fwrite(rx_dma_buffer, 1, xfer_bytes, output_file);
        if (bytes_written != xfer_bytes)
        {
            fprintf(stderr, "Unexpected number of bytes written: %d -- %d\n", 
                    bytes_written, errno);
        }
        
        bytes_remaining -= xfer_bytes;
    }
    
    // Set end time
    gettimeofday(&tval_end, NULL);
    timersub(&tval_end, &tval_start, &tval_result);
    
    // Display elapsed time
    printf("Time elapsed: %ld.%06ld s\n", (long int)tval_result.tv_sec, 
                                          (long int)tval_result.tv_usec);
    double time = (double)tval_result.tv_sec;
    
    // Calculate data rate and scale it 
    time += ((double)tval_result.tv_usec / (double)1000000.0);    
    char* unit = NULL;
    double rate = calc_data_rate(file_size, time, &unit);
    
    printf("Data rate:    %.2f %s\n", rate, unit);
    
    // Close the files
    fclose(input_file);
    fclose(output_file);
    
    // Free the buffers
    dmap_free_buffer(tx_dma_buffer);
    dmap_free_buffer(rx_dma_buffer);
    
    printf("[FIN]\n");
    
    return 0;
}
