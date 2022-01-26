#include <sys/time.h>

#include <cmath>
#include <iostream>

long long current_timestamp()
{
    struct timeval te;
    gettimeofday(&te, NULL);  // get current time
    long long milliseconds =
        te.tv_sec * 1000LL + te.tv_usec / 1000;  // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

void bits_to_bytes(uint8_t* src, char* dst, int Nsrc)  // Nsrc is in bits
{
    int n, k;
    char x;
    int cntr = 0;

    for (n = 0; n < Nsrc / 8; n++)
    {
        x = 0;
        for (k = 0; k < 8; k++)
        {
            x = x << 1;
            if (src[n * 8 + k])
            {
                x = x | 1;
            }
        }
        dst[cntr++] = x;
    }
}

/** Convert a character to bits (bits are 0 or 1 uint8_t)

    @param chr  - character
    @param bits - bits, modified in place
    @return void
*/
/*
void char_to_bits(char chr, uint8_t *bits){
    uint32_t bi; //bit index
    uint8_t  mask = 0x80;

    for(bi=0; bi<8; bi++){
        bits[bi] = 0; //clear bit
        if(chr & mask)
            bits[bi] = 1;
        mask >>= 1;
    }
}
*/
/** Convert chars to bits

    @param chrs - ASCII characters
    @param bits - bits, modified in place
    @param n    - number of characters
    @return void
*/
/*
void chrs_to_bits(char *chrs, uint8_t *bits, uint32_t n){
    uint32_t bi; //bit index
    uint32_t ci; //character index

    bi = 0;
    for(ci=0; ci<n; ci++){
        char_to_bits(chrs[ci], (uint8_t *)&bits[bi]);
        bi += 8;
    }
}
*/
//
//
/** Jacobian logarithm using a lookup table
    C=jac(A,B) A, B and C are scalar LLRs TPB

    @param A - float Log Likelyhood Ratio(LLR)
    @param B - float Log Likelyhood Ratio(LLR)
    @param C - float Jacobian
    @return void
*/
float jac(float A, float B)
{
    float C = std::fmax(A, B) + 0.65;
    float difference = abs(A - B);

    if (difference >= 4.5)
        C = std::fmax(A, B);
    else if (difference >= 2.252)
        C = std::fmax(A, B) + 0.05;
    else if (difference >= 1.508)
        C = std::fmax(A, B) + 0.15;
    else if (difference >= 1.05)
        C = std::fmax(A, B) + 0.25;
    else if (difference >= 0.71)
        C = std::fmax(A, B) + 0.35;
    else if (difference >= 0.433)
        C = std::fmax(A, B) + 0.45;
    else if (difference >= 0.196)
        C = std::fmax(A, B) + 0.55;
    else if (difference >= 0.0)
        C = std::fmax(A, B) + 0.65;

    return (C);
}

/** Convert a character to bits (bits are -1.0<-0 or 1.0<-1 floats)

    @param chr  - character
    @param bits - bits, modified in place(floating point values)
    @return void
*/
void char_to_bits(char chr, float* bits)
{
    uint32_t bi;  // bit index
    uint8_t mask = 0x80;

    for (bi = 0; bi < 8; bi++)
    {
        // bits[bi] = -1.0; //clear bit 0 -> -1.0
        bits[bi] = 0.0;
        if (chr & mask)
            bits[bi] = 1.0;  // set bit 1 -> 1.0
        mask >>= 1;
    }
}

void chrs_to_bits(char* chrs, float* bits, uint32_t n)
{
    uint32_t bi;  // bit index
    uint32_t ci;  // character index

    bi = 0;
    for (ci = 0; ci < n; ci++)
    {
        char_to_bits(chrs[ci], &bits[bi]);
        bi += 8;
    }
}

void bpsk_channel(float* bits, float* bpsk_bits, uint32_t n)
{
    uint32_t bi;  // bit index

    for (bi = 0; bi < n; bi++)
    {
        if (bits[bi] == 0.0)
        {
            bpsk_bits[bi] = -1.0;
        }
        else  // 180 degree phase shift
        {
            bpsk_bits[bi] = 1.0;
        }
    }
}

// 8-state 16 transitions
// FromState,  ToState,  UncodedBit, EncodedBit
uint8_t transitionsN0[16][4] = {
    {0, 0, 0, 0}, {1, 4, 0, 0}, {2, 5, 0, 1}, {3, 1, 0, 1},
    {4, 2, 0, 1}, {5, 6, 0, 1}, {6, 7, 0, 0}, {7, 3, 0, 0},
    {0, 4, 1, 1}, {1, 0, 1, 1}, {2, 1, 1, 0}, {3, 5, 1, 0},
    {4, 6, 1, 0}, {5, 2, 1, 0}, {6, 3, 1, 1}, {7, 7, 1, 1}};

uint8_t transitionsN1[16][4] = {
    {0, 0, 0, 0}, {1, 4, 0, 0}, {2, 5, 0, 0}, {3, 1, 0, 0},
    {4, 2, 0, 1}, {5, 6, 0, 1}, {6, 7, 0, 1}, {7, 3, 0, 1},
    {0, 4, 1, 1}, {1, 0, 1, 1}, {2, 1, 1, 1}, {3, 5, 1, 1},
    {4, 6, 1, 0}, {5, 2, 1, 0}, {6, 3, 1, 0}, {7, 7, 1, 0}};

uint8_t punc_pat_data[2][6] = {{1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 1, 0}};
uint8_t punc_pat_tail[6][6] = {{1, 1, 1, 0, 0, 0},   // X
                               {1, 1, 1, 0, 0, 0},   // Y0
                               {0, 0, 0, 0, 0, 0},   // Y1
                               {0, 0, 0, 1, 1, 1},   // Xprime
                               {0, 0, 0, 1, 1, 1},   // Y0prime
                               {0, 0, 0, 0, 0, 0}};  // Y1prime