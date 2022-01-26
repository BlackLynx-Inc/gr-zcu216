#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class bit_utils
{
public:

    static const char hex_bytes[16];    
    static std::map<uint8_t, uint8_t> ascii_table;

    /**
     * Unpack packed binary bits to the one-bit-per-byte format.
     * 
     * input_data - the input data as a series of bytes
     * input_length - length of the input data as bytes
     * ouput_data - output data that will contain output bytes, one per bit
     * output_length - length of the output data as bytes
     */ 
    static void unpack_bits(
        const uint8_t* input_data, 
        uint32_t input_length, 
        uint8_t* output_data,
        uint32_t output_length);
    
    /**
     * Pack one-bit-per-byte format back into raw binary data.
     */ 
    static void pack_bits(
        const uint8_t* input_data, 
        uint32_t input_length, 
        uint8_t* output_data,
        uint32_t output_length);
    
    /**
     * Fast lowercase a character
     */ 
    static inline uint8_t __lower(uint8_t chr)
    {
        uint8_t converted = chr;
        if ((converted >= 'A') && (converted <= 'Z'))
        {
            converted |= 32;
        }
        return converted;
    }
        
    /**
     * Convert bytes of raw binary to a string of ASCII hex
     */ 
    static void rawbytes_to_ascii(
        const uint8_t* bytes, 
        uint32_t length, 
        std::string& output);
    
    /**
     * Convert a string of ASCII hex to bytes of raw binary
     */ 
    static void ascii_to_rawbytes(
        const std::string& input, 
        uint8_t* raw_bytes, 
        uint32_t length);

};

#endif
