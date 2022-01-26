#ifndef COMPONENT_TC_DECODER_H
#define COMPONENT_TC_DECODER_H

constant uint32_t DEBUG = 0;
// float* component_decoder(char* turbo_type, float* apriori_uncoded_llrs,
// float* apriori_encoded_llrs);
void component_decoder(char* turbo_type, float* apriori_uncoded_llrs,
                       float* apriori_encoded_llrs,
                       float* extrinsic_uncoded_llrs);

#endif
