#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>

#include "cdma2000_TC_encoder.h"


const uint32_t MESSAGE_SIZE_BYTES{96};  // NOTE: this is rounded up
const uint32_t MESSAGE_SIZE_BITS{762};
const uint32_t CODEWORD_SIZE_BYTES{192};
const uint32_t CODEWORD_SIZE_BITS{1536};

const char hex_bytes[16]{'0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
                                    
std::map<uint8_t, uint8_t> ascii_table{
    {'0', 0x0}, {'1', 0x1}, {'2', 0x2}, {'3', 0x3}, {'4', 0x4}, {'5', 0x5},
    {'6', 0x6}, {'7', 0x7}, {'8', 0x8}, {'9', 0x9}, {'a', 0xa}, {'b', 0xb},
    {'c', 0xc}, {'d', 0xd}, {'e', 0xe}, {'f', 0xf}
};

inline uint8_t __lower(uint8_t chr)
{
    uint8_t converted = chr;
    if ((converted >= 'A') && (converted <= 'Z'))
    {
        converted |= 32;
    }
    return converted;
}

void rawbytes_to_ascii(
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
void ascii_to_rawbytes(
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

void unpack_bits(
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

void pack_bits(
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


uint64_t get_file_size(const char* filename)
{
    uint64_t file_size = 0;
    struct stat stat_buffer;

    stat(filename, &stat_buffer);
    return stat_buffer.st_size;
}

int main(int argc, char** argv)
{
    std::cout << "TC Encoder Test\n" << std::endl;
    
    if (argc < 2)
    {
        std::cerr << "ERROR: please provide an input file" << std::endl;
        return -1;
    }
    else if (argc < 3)
    {
        std::cerr << "ERROR: please provide an output file" << std::endl;
        return -1;
    }
    
    std::ifstream instream(argv[1], std::ios::in);
    if (! instream)
    {
        std::cerr << "ERROR: unable to open input file: " << argv[1] << std::endl;
        return -2;
    }
    
    std::ofstream outstream(argv[2], std::ios::out);
    if (! outstream)
    {
        std::cerr << "ERROR: unable to open output file: " << argv[2] << std::endl;
        return -2;
    }
    
    uint64_t file_size = get_file_size(argv[1]);
    if (file_size % MESSAGE_SIZE_BITS)
    {
        std::cerr << "WARNING: file size (" << file_size << ") is not a "
                  << "multiple of the message size (" << MESSAGE_SIZE_BITS << ")"
                  << std::endl;
    }
    uint32_t num_iters = file_size / MESSAGE_SIZE_BITS;
    std::cout << "Num iters: " << num_iters << std::endl;
    
    // Resize input buffer to hold input data then read in the input file
    std::vector<uint8_t> input_buffer;
    input_buffer.resize(file_size);
    instream.read(reinterpret_cast<char*>(input_buffer.data()), file_size);
    
    
#if 0 
    // Read in the packed input data one message's worth at a time and then
    // convert to unpacked format
    std::vector<uint8_t> temp_buffer(MESSAGE_SIZE_BYTES);
    std::string message;
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        instream.read(reinterpret_cast<char*>(temp_buffer.data()), MESSAGE_SIZE_BYTES);
        
        // Print the message
        rawbytes_to_ascii(temp_buffer.data(), MESSAGE_SIZE_BYTES, message);
        std::cout << "[" << std::setw(3) << std::setfill('0') << iter_idx 
                  << "] -- " << message << std::endl;
        
        unpack_bits(temp_buffer.data(), MESSAGE_SIZE_BYTES,
                    input_buffer.data() + (iter_idx * MESSAGE_SIZE_BITS),
                    MESSAGE_SIZE_BITS);
    }
#endif
    
    instream.close();
    
    // Resize output buffer to hold encoded bits in unpacked "one-bit-per-byte"
    // format
    std::vector<char> output_buffer;
    output_buffer.resize(num_iters * CODEWORD_SIZE_BITS);
    
    std::vector<char> codeword(CODEWORD_SIZE_BYTES);
    std::vector<char> codeword_bits(CODEWORD_SIZE_BITS);
    std::vector<char> channel_bytes(CODEWORD_SIZE_BYTES); // not clear what this is for
    
        
    /*
     * void component_encoder(const char* turbo_type, uint8_t* uncoded_bits,
                              char* encoded_bytes, char* channel_bytes);
     */ 
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        char* output_ptr = output_buffer.data() + (iter_idx * CODEWORD_SIZE_BITS);
        
        component_encoder("T", 
                          input_buffer.data() + (iter_idx * MESSAGE_SIZE_BYTES),
                          codeword.data(), channel_bytes.data());
        /*
         void unpack_bits(
            const uint8_t* input_data, 
            uint32_t input_length, 
            uint8_t* output_data,
            uint32_t output_length)
        */
                          
        // The call above outputs packed data, unpack it
        unpack_bits((const uint8_t*)codeword.data(), CODEWORD_SIZE_BYTES,
                    reinterpret_cast<uint8_t*>(codeword_bits.data()), 
                    CODEWORD_SIZE_BITS);
                    
        // Reorder the data: systematic followed by parity followed by
        // tail.(Hadron decoder order)

        //--- Systematic ------------------------------------------------------
        for (int idx = 0; idx < MESSAGE_SIZE_BITS; ++idx)  // 762 bits
        {
            // This pulls out all 762 Xs(systematic) 0, 2, 4, 6, 8 ...
            output_ptr[idx] = codeword_bits[2 * idx];
        }
        
        //--- Parity ----------------------------------------------------------
        for (int idx = 0; idx < MESSAGE_SIZE_BITS / 2; ++idx)  // 381 bits
        {
            // This pulls out 381 Ys 1, 5, 9, 13, 17, ...
            output_ptr[MESSAGE_SIZE_BITS + idx] = codeword_bits[4 * idx + 1];  
        }
        for (int idx = 0; idx < MESSAGE_SIZE_BITS / 2; ++idx)  // 381 bits
        {
            // This pulls out 381  Y's  3, 7, 11, 15, 19 ...
            output_ptr[MESSAGE_SIZE_BITS + MESSAGE_SIZE_BITS / 2 + idx] = 
                codeword_bits[4 * idx + 3];
        }
        
        //--- Tail ------------------------------------------------------------
        for (int idx = 0; idx < 6; ++idx)  // 6 bits
        {
            // This pulls out 6 Tail bits  X0, X1, X2, X'0, X'1, X'2
            output_ptr[MESSAGE_SIZE_BITS + MESSAGE_SIZE_BITS + idx] =
                codeword_bits[MESSAGE_SIZE_BITS + MESSAGE_SIZE_BITS + 2 * idx];  
        }
        for (int idx = 0; idx < 6; ++idx)  // 6 bits
        {
            // This pulls out 6 Tail bits  Y0, Y1, Y2, Y'0, Y'1, Y'2
            output_ptr[MESSAGE_SIZE_BITS + MESSAGE_SIZE_BITS + 6 + idx] =
                codeword_bits[MESSAGE_SIZE_BITS + MESSAGE_SIZE_BITS + 2 * idx + 1]; 
        }
    }
    
    outstream.write(output_buffer.data(), num_iters * CODEWORD_SIZE_BITS);
    outstream.close();
    
    return 0;
}
