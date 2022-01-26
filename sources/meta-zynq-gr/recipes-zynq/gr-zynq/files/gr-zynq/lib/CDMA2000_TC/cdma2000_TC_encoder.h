#ifndef HADRON_TC_ENCODER_H
#define HADRON_TC_ENCODER_H

//#define __DEBUG_TC_ENCODER

void component_encoder(const char* turbo_type, uint8_t* uncoded_bits,
                       char* encoded_bytes, char* channel_bytes);

#endif