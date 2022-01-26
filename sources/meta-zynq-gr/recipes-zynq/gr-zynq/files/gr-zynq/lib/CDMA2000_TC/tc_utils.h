#ifndef TC_UTILS_H
#define TC_UTILS_H

float jac(float A, float B);

void char_to_bits(char chr, int8_t* bits);
void chrs_to_bits(char* chrs, float* bits, uint32_t n);
void bpsk_channel(float* bits, float* bpsk_bits, uint32_t n);

void bits_to_bytes(uint8_t* src, char* dst, int Nsrc);
long long current_timestamp();

extern uint8_t transitionsN0[16][4];
extern uint8_t transitionsN1[16][4];

extern uint8_t punc_pat_data[2][6];
extern uint8_t punc_pat_tail[6][6];

#endif
