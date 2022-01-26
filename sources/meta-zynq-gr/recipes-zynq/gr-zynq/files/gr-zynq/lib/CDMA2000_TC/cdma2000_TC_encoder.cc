/*! \file component_encode.cpp
 * \brief Algorithm for turbo codes
 * <pre>
 * DFLX.COM, INC.
 * Author: Bill Douskalis
 * For hadron version 7.0 and later
 * DFLX.COM, INC.
 *</pre>*/

#include <string.h>

#include <chrono>

#ifndef T4
#define T4
#include "T4.h"
#endif
#include "cdma2000_TC_encoder.h"
#include "get_3GPP_interleaver.h"
#include "tc_utils.h"

void component_encoder(
    const char* turbo_type,  // either "U" or "T"
    uint8_t* uncoded_bits,   // input 1D array
    char* encoded_bytes,
    char* channel_bytes)  // output 1D array // Channel Bytes are not used
{
#ifdef __DEBUG_TC_ENCODER
    FILE* coded_file;
    FILE* coded_bits;
    FILE *diag, *coded_bits_txt, *channel_interleaver_bits_txt;
#endif

    int ix, iy, iz;
    uint8_t ilv_bit;

    uint8_t encoded_bits[1600];

    uint32_t *interleaver, *channel_interleaver;
    interleaver = new uint32_t[UNC_BITS];
    channel_interleaver = new uint32_t[ENC_BITS];

    uint32_t pass = 0, tail_pass = 0;
    uint32_t curr_state[2] = {0, 0};

    uint32_t good_bit_0, good_bit_1;
    uint8_t tail_bits[12];
    // uint8_t curr_byte;
    uint8_t encoded_Yo_prime[UNC_BITS];
    uint8_t encoded_Yo[UNC_BITS];

    uint8_t* channel_bits = new uint8_t[ENC_BITS];

    if (strncmp(turbo_type, "T", 1) == 0)
    {
#ifdef __DEBUG_TC_ENCODER
        printf("Requested T4 turbo encoder\n");
#endif
    }
    else
    {
        printf("Turbo code type error, exiting");
        exit(-1);
    }

    // encoded_bits is supplied pointer to a big enough vector of encoded bits
    // T4 outputs 1536 bits for a packet of 762 input bits

    int half_bits = UNC_BITS / 2;

#ifdef __DEBUG_TC_ENCODER
    diag = fopen("encoder_states.csv", "wb");
    setbuf(diag, NULL);
#endif

    if (strncmp(turbo_type, "T", 1) == 0)
    {
#ifdef __DEBUG_TC_ENCODER
        auto start = std::chrono::system_clock::now();
#endif

        get_T4_interleaver(interleaver, channel_interleaver, (int)UNC_BITS);

#ifdef __DEBUG_TC_ENCODER
        printf("Start the tubro coder\n");
#endif

        pass = 0;
        good_bit_0 = 0;
        good_bit_1 = 0;

#ifdef __DEBUG_TC_ENCODER
        fprintf(diag, "curr_state[0],uncoded_bits[pass],Y0 IDX "
                      "[x][3],Y0,curr_state[1],ilv_bit,Y'0 IDX "
                      "[x][3],Y'0,pass,interleaver[pass]\n");
#endif

        while (pass < UNC_BITS)
        {
            ilv_bit = uncoded_bits[interleaver[pass]];

            // puncture the code to 1/2 (Default CDMA2000 Turbo encoder
            // Rate=1/6) three outputs for one input
            if (pass & 0x01)  // odd bit period
            {
                if (punc_pat_data[1][4] == 1)  // means keep the bit Y0prime
                {
                    encoded_Yo_prime[good_bit_1++] = (uint8_t)
                        transitionsN0[curr_state[1] | (ilv_bit << 3)][3];
                }
            }
            else  // even bit perod
            {
                if (punc_pat_data[0][1] == 1)  // means keep the bit Y0
                {
                    encoded_Yo[good_bit_0++] =
                        (uint8_t)transitionsN0[curr_state[0] |
                                               (uncoded_bits[pass] << 3)][3];
                }
            }
#ifdef __DEBUG_TC_ENCODER
            if (pass & 0x01)  // odd bit period
            {
                fprintf(diag, "%d,%d,%d,%s,%d,%d,%d,%d,%d,%d\n", curr_state[0],
                        uncoded_bits[pass],
                        (curr_state[0] | (uncoded_bits[pass] << 3)), "null",
                        curr_state[1], ilv_bit, curr_state[1] | (ilv_bit << 3),
                        transitionsN0[curr_state[1] | (ilv_bit << 3)][3], pass,
                        interleaver[pass]);
            }
            else
            {
                fprintf(
                    diag, "%d,%d,%d,%d,%d,%d,%d,%s,%d,%d\n", curr_state[0],
                    uncoded_bits[pass],
                    (curr_state[0] | (uncoded_bits[pass] << 3)),
                    transitionsN0[curr_state[0] | (uncoded_bits[pass] << 3)][3],
                    curr_state[1], ilv_bit, curr_state[1] | (ilv_bit << 3),
                    "null", pass, interleaver[pass]);
            }
#endif
            curr_state[0] = (uint8_t)
                transitionsN0[curr_state[0] | (uncoded_bits[pass] << 3)]
                             [1];  // This is for constituent encoder 1
            curr_state[1] =
                (uint8_t)transitionsN0[curr_state[1] | (ilv_bit << 3)]
                                      [1];  // This is for constituent encoder 1

            pass++;
        }

#ifdef __DEBUG_TC_ENCODER
        printf("X0 good bits %d, X0' good bits %d\n", good_bit_0, good_bit_1);
        printf("Terminate the trellis: ");
#endif

        uint32_t tail_bit_X;
        uint32_t tail_bit_Y;
        uint32_t tb = 0;

#ifdef __DEBUG_TC_ENCODER
        fprintf(diag, "curr_state[0],tail_bit_X,tail_bit_Y, "
                      "curr_state[1],tail_bit_X,tail_bit_Y,tail_pass\n");
#endif

        for (tail_pass = 0, tb = 0; tail_pass < 3; tail_pass++, tb++)
        {
            if ((curr_state[0] == 0x02) || (curr_state[0] == 0x01) ||
                (curr_state[0] == 0x05) || (curr_state[0] == 0x06))
            {
                tail_bit_X = 1;
            }
            else
            {
                tail_bit_X = 0;
            }

            tail_bits[tb] = tail_bit_X;

            if (((curr_state[0] == 0x01) || (curr_state[0] == 0x04) ||
                 (curr_state[0] == 0x06) || (curr_state[0] == 0x03)))
            {
                tail_bit_Y = 1;
            }
            else
            {
                tail_bit_Y = 0;
            }
            tail_bits[tb + 3] = tail_bit_Y;
            curr_state[0] >>= 1;

#ifdef __DEBUG_TC_ENCODER
            fprintf(diag, "%d,%d,%d,%d,%d,%d,%d\n", curr_state[0], tail_bit_X,
                    tail_bit_Y, curr_state[1], tail_bits[tail_pass],
                    tail_bits[tail_pass + 3], tail_pass);
#endif
        }

        for (tail_pass = 0, tb = 6; tail_pass < 3; tail_pass++, tb++)
        {
            if ((curr_state[1] == 0x02) || (curr_state[1] == 0x01) ||
                (curr_state[1] == 0x05) || (curr_state[1] == 0x06))
            {
                tail_bit_X = 1;
            }
            else
            {
                tail_bit_X = 0;
            }
            tail_bits[tb] = tail_bit_X;

            if (((curr_state[1] == 0x01) || (curr_state[1] == 0x04) ||
                 (curr_state[1] == 0x06) || (curr_state[1] == 0x03)))

            {
                tail_bit_Y = 1;
            }
            else
            {
                tail_bit_Y = 0;
            }
            tail_bits[tb + 3] = tail_bit_Y;
            curr_state[1] >>= 1;

#ifdef __DEBUG_TC_ENCODER
            fprintf(diag, "%d,%d,%d,%d,%d,%d,%d\n", curr_state[0], tail_bit_X,
                    tail_bit_Y, curr_state[1], tail_bits[tail_pass + 6],
                    tail_bits[tail_pass + 6 + 3], tail_pass);
#endif
        }

        // CDMA2000 transmission order
        // Iterleave transmission order Systematic and parity
        // X0,Y0,X1,Y'1,X2,Y2,X3, ... ,X761,Y'761
        for (iy = 0, ix = 0; ix < UNC_BITS; ix++, iy += 2)
        {
            encoded_bits[iy] =
                uncoded_bits[ix];  // get the systematic bits first
        }
        for (iy = 1, ix = 0; ix < half_bits; iy += 4, ix++)
        {
            encoded_bits[iy] = encoded_Yo[ix];
        }
        for (iy = 3, ix = 0; ix < half_bits; iy += 4, ix++)
        {
            encoded_bits[iy] = encoded_Yo_prime[ix];
        }
        //------------------- Tail --------------------
        // Iterleave transmission order tail bits
        // X0,Y0,X1,Y1,X2,Y2,X'0,Y'0,X'1,Y'1,X'2,Y'2
        iy -= 3;
        int start_iy = iy;
        for (iz = 0; iz < 3; iz++)
        {
            encoded_bits[iy] = tail_bits[iz];
            iy += 2;
#ifdef __DEBUG_TC_ENCODER
            printf("%d", tail_bits[iz]);
#endif
        }
        for (iy = start_iy + 1; iz < 6; iz++)
        {
            encoded_bits[iy] = tail_bits[iz];
            iy += 2;
#ifdef __DEBUG_TC_ENCODER
            printf("%d", tail_bits[iz]);
#endif
        }
        for (iy = start_iy + 6; iz < 9; iz++)
        {
            encoded_bits[iy] = tail_bits[iz];
            iy += 2;
#ifdef __DEBUG_TC_ENCODER
            printf("%d", tail_bits[iz]);
#endif
        }
        for (iy = start_iy + 7; iz < 12; iz++)
        {
            encoded_bits[iy] = tail_bits[iz];
            iy += 2;
#ifdef __DEBUG_TC_ENCODER
            printf("%d", tail_bits[iz]);
#endif
        }

        for (iz = 0; iz < ENC_BITS; iz++)
        {
            channel_bits[iz] = encoded_bits[channel_interleaver[iz]];
        }

        bits_to_bytes(encoded_bits, encoded_bytes, ENC_BITS);
        bits_to_bytes(channel_bits, channel_bytes, ENC_BITS);

#ifdef __DEBUG_TC_ENCODER
        auto end = std::chrono::system_clock::now();
        auto diff = end - start;
        std::cout << std::endl;
        std::cout << "Hadron encoder duration: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms "
                  << "(Input size N = 762 inputs)" << std::endl;
#endif

#ifdef __DEBUG_TC_ENCODER
        printf("Write out %d bits\n", ix);

        coded_bits = fopen("coded_bits.bin", "wb");
        coded_bits_txt = fopen("coded_bits.txt", "w");
        channel_interleaver_bits_txt = fopen("channel_bits.txt", "w");

        for (ix = 0; ix < ENC_BITS; ix++)
        {
            fprintf(coded_bits_txt, "%d\n", encoded_bits[ix]);
            fprintf(channel_interleaver_bits_txt, "%d\n", channel_bits[ix]);
            if (encoded_bits[ix] > 1)
            {
                printf("Weird encoded bit value %02x, pos %d, exiting\n",
                       encoded_bits[ix], ix);
                exit(-1);
            }
        }
        fclose(channel_interleaver_bits_txt);
        fclose(coded_bits_txt);

        coded_file = fopen("coded_file.bin", "wb");
        if (coded_file == NULL)
        {
            printf("Could not open coded packet file, exiting\n");
            exit(-1);
        }
        else
        {
            // bits_to_bytes(encoded_bits, encoded_bytes, ENC_BITS);
            fwrite(encoded_bytes, sizeof(char), ENC_BYTES, coded_file);
            // bits_to_bytes(channel_bits, channel_bytes, ENC_BITS);
            // fwrite(channel_bytes,  sizeof(char), ENC_BYTES,
            // channel_interleaver_bits);
            fwrite(encoded_bits, sizeof(char), ENC_BITS, coded_bits);
        }
        fclose(coded_file);
        fclose(coded_bits);
#endif
    }
    delete[] interleaver;
    delete[] channel_interleaver;
    delete[] channel_bits;

#ifdef __DEBUG_TC_ENCODER
    fclose(diag);
#endif
}
