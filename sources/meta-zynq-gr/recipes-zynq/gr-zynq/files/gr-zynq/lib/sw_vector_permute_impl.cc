/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <chrono>
#include <sstream>
#include <thread>
 
#include "sw_vector_permute_impl.h"
#include <gnuradio/io_signature.h>


namespace gr {
namespace zynq {

using input_type = gr_complex;
using output_type = gr_complex;


sw_vector_permute::sptr sw_vector_permute::make(int debug)
{
    return gnuradio::make_block_sptr<sw_vector_permute_impl>(debug);
}

/*
 * The private constructor
 */
sw_vector_permute_impl::sw_vector_permute_impl(int debug)
    : gr::block("sw_vector_permute",
                gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 
                                       sizeof(input_type) * VECTOR_SIZE),
                gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, 
                                       sizeof(output_type) * VECTOR_SIZE)),
      d_debug(debug)
{
    if (d_debug)
    {
        std::cerr << "DEBUG ENABLED" << std::endl;
    }
    
    // NOTE: this block expects vector input so this really means: 1 * VECTOR_SIZE
    set_output_multiple(1);
}

/*
 * Our virtual destructor.
 */
sw_vector_permute_impl::~sw_vector_permute_impl() {}

bool sw_vector_permute_impl::start()
{
    if (d_debug)
    {
        std::cerr << "START" << std::endl;
    }
    return true;
}

bool sw_vector_permute_impl::stop()
{
    if (d_debug)
    {
        std::cerr << "STOP" << std::endl;
    }
    return true;
}

void sw_vector_permute_impl::forecast(int noutput_items, 
                                      gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int sw_vector_permute_impl::general_work(int noutput_items,
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
    
    const uint32_t half_vec_size = VECTOR_SIZE / 2;
    for (uint32_t iter_idx = 0; iter_idx < noutput_items; ++iter_idx)
    {
        input_type* in_item = const_cast<input_type*>(in) + (iter_idx * VECTOR_SIZE);
        output_type* out_item = out + (iter_idx * VECTOR_SIZE);
        
        // Copy second half of input vector to first half of output vector
        std::memcpy(out_item, 
                    reinterpret_cast<void*>(in_item) + (half_vec_size * sizeof(input_type)),
                    (half_vec_size * sizeof(input_type)));
        
        // Copy first half of input vector to second half of output vector
        std::memcpy(reinterpret_cast<void*>(out_item) + (half_vec_size * sizeof(input_type)),
                    in_item,
                    (half_vec_size * sizeof(input_type)));
    }
    
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(noutput_items);

    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    if (d_debug)
    {
        std::cerr << " -- " 
                  << std::chrono::duration <double, std::milli>(diff).count() 
                  << " ms" << std::endl;
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace zynq */
} /* namespace gr */
