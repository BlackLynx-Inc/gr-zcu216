#include <algorithm>

#include "bit_utils.h"


const char bit_utils::hex_bytes[16]{'0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::map<uint8_t, uint8_t> bit_utils::ascii_table{
    {'0', 0x0}, {'1', 0x1}, {'2', 0x2}, {'3', 0x3}, {'4', 0x4}, {'5', 0x5},
    {'6', 0x6}, {'7', 0x7}, {'8', 0x8}, {'9', 0x9}, {'a', 0xa}, {'b', 0xb},
    {'c', 0xc}, {'d', 0xd}, {'e', 0xe}, {'f', 0xf}
};
    
void bit_utils::unpack_bits(
    const uint8_t* input_data, 
    uint32_t input_length, 
    uint8_t* output_data,
    uint32_t output_length)
{
    // Output needs to contain 8 times as many bytes as the input; limit
    // iteration over the input bytes to the number of bytes that the output
    // can hold 
    uint32_t limit = std::min(input_length, output_length * 8);
    
    uint64_t output_idx = 0;
    for (uint32_t idx = 0; idx < limit; ++idx)
    {
        output_data[output_idx++] = (input_data[idx] >> 7) & 0x1;
        output_data[output_idx++] = (input_data[idx] >> 6) & 0x1;
        output_data[output_idx++] = (input_data[idx] >> 5) & 0x1;
        output_data[output_idx++] = (input_data[idx] >> 4) & 0x1;
        output_data[output_idx++] = (input_data[idx] >> 3) & 0x1;
        output_data[output_idx++] = (input_data[idx] >> 2) & 0x1;
        output_data[output_idx++] = (input_data[idx] >> 1) & 0x1;
        output_data[output_idx++] = (input_data[idx] >> 0) & 0x1;
    }
}


void bit_utils::pack_bits(
    const uint8_t* input_data, 
    uint32_t input_length, 
    uint8_t* output_data,
    uint32_t output_length)
{
    // Output needs to contain 8 times fewer bytes as the input; limit
    // iteration over the input bytes to the number of bytes that the output
    // can hold 
    uint32_t limit = std::min(input_length / 8, output_length);
    
    uint64_t input_idx = 0;
    for (uint32_t idx = 0; idx < limit; ++idx)
    {
        output_data[idx] = 0;
        
        output_data[idx] |= (input_data[input_idx++] << 7);
        output_data[idx] |= (input_data[input_idx++] << 6);
        output_data[idx] |= (input_data[input_idx++] << 5);
        output_data[idx] |= (input_data[input_idx++] << 4);
        output_data[idx] |= (input_data[input_idx++] << 3);
        output_data[idx] |= (input_data[input_idx++] << 2);
        output_data[idx] |= (input_data[input_idx++] << 1);
        output_data[idx] |= (input_data[input_idx++] << 0);
    }
}

void bit_utils::rawbytes_to_ascii(
    const uint8_t* bytes, 
    uint32_t length, 
    std::string& output) 
{
    output.clear();
    for (uint32_t idx = 0; idx < length; ++idx) 
    {
        output.append(&hex_bytes[(bytes[idx] & 0xF0) >> 4], 1);
        output.append(&hex_bytes[bytes[idx] & 0x0F], 1);
    }
}

void bit_utils::ascii_to_rawbytes(
    const std::string& input, 
    uint8_t* raw_bytes, 
    uint32_t length)
{
    const char* input_chars = input.c_str();
    uint32_t output_len = std::min(length, (uint32_t)(input.size() / 2));
    
    for (uint32_t idx = 0; idx < output_len; ++idx)
    {
        raw_bytes[idx] = (ascii_table[__lower(input_chars[(idx * 2)])] << 4);
        raw_bytes[idx] |= ascii_table[__lower(input_chars[(idx * 2) + 1])];
    }
}
