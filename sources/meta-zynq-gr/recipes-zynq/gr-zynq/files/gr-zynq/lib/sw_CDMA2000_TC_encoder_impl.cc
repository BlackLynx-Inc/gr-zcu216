/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <chrono>
#include <cstdint>
#include <sstream>
#include <thread>

#include <gnuradio/io_signature.h>
 
#include "sw_CDMA2000_TC_encoder_impl.h"
#include "CDMA2000_TC/bit_utils.h"
#include "CDMA2000_TC/cdma2000_TC_encoder.h"


namespace gr {
namespace zynq {

using input_type = uint8_t;
using output_type = char;


sw_CDMA2000_TC_encoder::sptr sw_CDMA2000_TC_encoder::make(int debug)
{
    return gnuradio::make_block_sptr<sw_CDMA2000_TC_encoder_impl>(debug);
}


/*
 * The private constructor
 */
sw_CDMA2000_TC_encoder_impl::sw_CDMA2000_TC_encoder_impl(int debug)
    : gr::block("sw_CDMA2000_TC",
                gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 
                                       sizeof(input_type)),
                gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, 
                                       sizeof(output_type))),
      d_debug(debug)
{
    
    set_relative_rate((uint64_t)CW_BITS, (uint64_t)MSG_BITS);
    
    // This call causes the scheduler to ensure that the noutput_items
    // argument passed to forecast and general_work will be an integer multiple
    // of the passed value.
    set_output_multiple(CW_BITS);
    set_min_noutput_items(CW_BITS);
}

/*
 * Our virtual destructor.
 */
sw_CDMA2000_TC_encoder_impl::~sw_CDMA2000_TC_encoder_impl() {}

bool sw_CDMA2000_TC_encoder_impl::start()
{
    if (d_debug)
    {
        std::cerr << "START" << std::endl;
    }
    return true;
}

bool sw_CDMA2000_TC_encoder_impl::stop()
{
    if (d_debug)
    {
        std::cerr << "STOP" << std::endl;
    }
    return true;
}

void sw_CDMA2000_TC_encoder_impl::forecast(
    int noutput_items, 
    gr_vector_int& ninput_items_required)
{    
    ninput_items_required[0] = (noutput_items * CODE_RATE);
}

int sw_CDMA2000_TC_encoder_impl::general_work(int noutput_items,
                                              gr_vector_int& ninput_items,
                                              gr_vector_const_void_star& input_items,
                                              gr_vector_void_star& output_items)
{
    static uint32_t ctr = 0;
    if (d_debug)
    {
        std::cerr << "WORK called: " << ++ctr << " -- noutput_items: " 
                  << noutput_items;
    }
    
    auto start = std::chrono::system_clock::now();
    
    auto in = reinterpret_cast<const input_type*>(input_items[0]);
    auto out = reinterpret_cast<output_type*>(output_items[0]);
    
    uint32_t num_input_items = ninput_items[0];
    uint32_t num_iters = std::min(num_input_items / MSG_BITS, 
                                  noutput_items / CW_BITS);
    char* output_ptr = out;
    
    if (d_debug)
    {
        std::cerr << " -- num_in_items: " << num_input_items 
                  << " -- num iters: " << num_iters;
    }
    
    // Call the encoder for each input message
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        output_ptr = out + (iter_idx * CW_BITS);
        
        // The encoder directly outputs packed bytes
        component_encoder("T", ((uint8_t*)in + (iter_idx * MSG_BITS)), 
                          m_codeword_bytes, m_channel_bytes);
        
        // Next unpack the bits to "one-bit-per-byte" format
        bit_utils::unpack_bits(reinterpret_cast<uint8_t*>(m_codeword_bytes), CW_BYTES,
                               reinterpret_cast<uint8_t*>(m_codeword_bits), CW_BITS);
                               
        // Reorder the data: systematic followed by parity followed by
        // tail. (Hadron decoder order)

        //--- Systematic ------------------------------------------------------
        for (int idx = 0; idx < MSG_BITS; ++idx)  // 762 bits
        {
            // This pulls out all 762 Xs(systematic) 0, 2, 4, 6, 8 ...
            output_ptr[idx] = m_codeword_bits[2 * idx];
        }
        
        //--- Parity ----------------------------------------------------------
        for (int idx = 0; idx < MSG_BITS / 2; ++idx)  // 381 bits
        {
            // This pulls out 381 Ys 1, 5, 9, 13, 17, ...
            output_ptr[MSG_BITS + idx] = m_codeword_bits[(4 * idx) + 1];
        }
        for (int idx = 0; idx < MSG_BITS / 2; ++idx)  // 381 bits
        {
            // This pulls out 381  Y's  3, 7, 11, 15, 19 ...
            output_ptr[MSG_BITS + (MSG_BITS / 2) + idx] = 
                m_codeword_bits[(4 * idx) + 3];
        }
        
        //--- Tail ------------------------------------------------------------
        for (int idx = 0; idx < 6; ++idx)  // 6 bits
        {
            // This pulls out 6 Tail bits  X0, X1, X2, X'0, X'1, X'2
            output_ptr[MSG_BITS + MSG_BITS + idx] =
                m_codeword_bits[MSG_BITS + MSG_BITS + (2 * idx)];  
        }
        for (int idx = 0; idx < 6; ++idx)  // 6 bits
        {
            // This pulls out 6 Tail bits  Y0, Y1, Y2, Y'0, Y'1, Y'2
            output_ptr[MSG_BITS + MSG_BITS + 6 + idx] =
                m_codeword_bits[MSG_BITS + MSG_BITS + ((2 * idx) + 1)]; 
        }
        
    }
    
    consume_each(num_iters * MSG_BITS);

    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    if (d_debug)
    {
        std::cerr << " -- " 
                  << std::chrono::duration<double, std::milli>(diff).count() 
                  << " ms" << std::endl;
    }

    return (num_iters * CW_BITS);
}

} /* namespace zynq */
} /* namespace gr */
