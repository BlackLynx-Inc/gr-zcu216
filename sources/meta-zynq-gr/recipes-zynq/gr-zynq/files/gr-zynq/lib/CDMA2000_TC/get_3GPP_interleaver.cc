/*! \file get_3GPP_interleaver.cpp
 * \brief as the file name says...
 * <pre>
 * Author: Bill Douskalis 2018
 * Copyright 2018, DFLX.COM, INC.
 * For Hadron version 7.0 and later
 *</pre>*/

#ifndef T4
#define T4
#include "T4.h"
#endif

#include "get_3GPP_interleaver.h"

uint8_t turbo_ilv[] = {27, 3,  1, 15, 13, 17, 23, 13, 9,  3,  15,
                       3,  13, 1, 13, 29, 21, 19, 1,  3,  29, 17,
                       25, 29, 9, 13, 23, 13, 13, 1,  13, 13};

uint8_t bit_reversed_lsbs[] = {
    0b00000, 0b10000, 0b01000, 0b11000, 0b00100, 0b10100, 0b01100, 0b11100,
    0b00010, 0b10010, 0b01010, 0b11010, 0b00110, 0b10110, 0b01110, 0b11110,
    0b00001, 0b10001, 0b01001, 0b11001, 0b00101, 0b10101, 0b01101, 0b11101,
    0b00011, 0b10011, 0b01011, 0b11011, 0b00111, 0b10111, 0b01111, 0b11111};

// uint8_t transitionsN0[TRANSITIONS][4], transitionsN1[TRANSITIONS][4];

void get_T4_interleaver(uint32_t* interleaver, uint32_t* channel_interleaver,
                        int length)
{
    uint32_t counter = 0;  // 10 bit counter for T4
    uint32_t n = 5;
    uint32_t ix, iy, iz;
    uint32_t nLSBs, MSBs;
    uint32_t ilv_addr = 0;
    uint32_t pass = 0;
    uint32_t good_bits = 0;
    uint32_t TLU;
#ifdef __DEBUG_3GPP_INTERLEAVER
    FILE *ilv, *cilv;
    ilv = fopen("turbo_interleaver.txt", "wb");
    cilv = fopen("channel_interleaver.txt", "wb");
#endif
    while (pass < NTURBO_LIMIT)  // max of 1024 iterations
    {
        nLSBs = ((counter >> n) + 1) & 0x1f;
        MSBs = bit_reversed_lsbs[(counter & 0x1f)];
        TLU = turbo_ilv[(counter & 0x1f)];
        ilv_addr = (MSBs << n) + ((TLU * nLSBs) & 0x1f);
        if (int(ilv_addr) >= length)
        {
            pass++;
            counter++;
            counter &= 0x3ff;
            continue;
        }
        else
        {
            interleaver[good_bits] = ilv_addr;
#ifdef __DEBUG_3GPP_INTERLEAVER
            fprintf(ilv, "%d\n", interleaver[good_bits++]);
#else
            good_bits++;
#endif
        }
        pass++;
        counter++;
        counter &= 0x3ff;
    }

    for (ix = 0, iz = 0; ix < 48; ix++)
    {
        for (iy = 0; iy < ENC_BITS; iy += 48)
        {
            channel_interleaver[iz] = ix + iy;
#ifdef __DEBUG_3GPP_INTERLEAVER
            fprintf(cilv, "%d\n", channel_interleaver[iz++]);
#else
            iz++;
#endif
        }
    }

#ifdef __DEBUG_3GPP_INTERLEAVER
    printf("Number of good bits: %d\n", good_bits);
    fclose(ilv);
    fclose(cilv);
#endif
}
