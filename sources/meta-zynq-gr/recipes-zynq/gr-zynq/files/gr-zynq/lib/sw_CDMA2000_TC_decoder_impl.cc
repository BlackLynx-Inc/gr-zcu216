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
 
#include "sw_CDMA2000_TC_decoder_impl.h"
#include "CDMA2000_TC/pccc_example.h"

namespace gr {
namespace zynq {

using input_type = float;
using output_type = char;


sw_CDMA2000_TC_decoder::sptr sw_CDMA2000_TC_decoder::make(int debug)
{
    return gnuradio::make_block_sptr<sw_CDMA2000_TC_decoder_impl>(debug);
}


/*
 * The private constructor
 */
sw_CDMA2000_TC_decoder_impl::sw_CDMA2000_TC_decoder_impl(int debug)
    : gr::block("sw_CDMA2000_TC",
                gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 
                                       sizeof(input_type)),
                gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, 
                                       sizeof(output_type))),
      d_debug(debug)
{
    set_relative_rate((uint64_t)MSG_BITS, (uint64_t)CW_BITS);
    
    // This call causes the scheduler to ensure that the noutput_items
    // argument passed to forecast and general_work will be an integer multiple
    // of the passed value.
    set_output_multiple(MSG_BITS);
    set_min_noutput_items(MSG_BITS);
}

/*
 * Our virtual destructor.
 */
sw_CDMA2000_TC_decoder_impl::~sw_CDMA2000_TC_decoder_impl() {}

bool sw_CDMA2000_TC_decoder_impl::start()
{
    if (d_debug)
    {
        std::cerr << "START" << std::endl;
    }
    return true;
}

bool sw_CDMA2000_TC_decoder_impl::stop()
{
    if (d_debug)
    {
        std::cerr << "STOP" << std::endl;
    }
    return true;
}

void sw_CDMA2000_TC_decoder_impl::forecast(
    int noutput_items, 
    gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items / CODE_RATE;
}

int sw_CDMA2000_TC_decoder_impl::general_work(int noutput_items,
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
    uint32_t num_iters = std::min(num_input_items / CW_BITS, 
                                  noutput_items / MSG_BITS);
    
    if (d_debug)
    {
        std::cerr << " -- num_in_items: " << num_input_items 
                  << " -- num iters: " << num_iters;
    }
    
    // Call the encoder for each input message
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        pccc_decode_example(out + (iter_idx * MSG_BITS), 
                            const_cast<float*>(in + (iter_idx * CW_BITS)));
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(num_iters * CW_BITS);

    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    if (d_debug)
    {
        std::cerr << " -- " 
                  << std::chrono::duration<double, std::milli>(diff).count() 
                  << " ms" << std::endl;
    }

    // Tell runtime system how many output items we produced.
    return (num_iters * MSG_BITS);
}

} /* namespace zynq */
} /* namespace gr */
