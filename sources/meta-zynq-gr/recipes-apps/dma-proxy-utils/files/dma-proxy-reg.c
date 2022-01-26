#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <dma-proxy-lib.h>

int main(int argc, char** argv)
{
    printf("DMA Proxy Reg Util\n\n");
    
    if (argc == 2)
    {
        uint32_t offset = strtoul(argv[1], NULL, 0);
        printf("Offset: 0x%08X -- 0x%08X\n", offset, dmap_reg_read32(offset));
    }
    else if (argc == 3)
    {
        uint32_t offset = strtoul(argv[1], NULL, 0);
        uint32_t value = strtoul(argv[2], NULL, 0);
        int rc = dmap_reg_write32(offset, value);
        printf("Wrote 0x%08X to offset: 0x%08X -- %s\n", 
               value, offset, ((rc == 0) ? "OKAY" : "ERROR"));
    }
    else
    {
        fprintf(stderr, "Invalid number of arguments provided: %d\n", argc);
    }    
    
    return 0;
}
