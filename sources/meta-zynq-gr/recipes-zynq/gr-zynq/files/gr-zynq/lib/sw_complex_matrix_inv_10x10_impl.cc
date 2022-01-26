/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <chrono>
#include <sstream>
#include <thread>

#include <gnuradio/io_signature.h>
 
#include "sw_complex_matrix_inv_10x10_impl.h"
#include "matrix_inv.h"


namespace gr {
namespace zynq {

using input_type = gr_complex;
using output_type = gr_complex;


sw_complex_matrix_inv_10x10::sptr sw_complex_matrix_inv_10x10::make(int debug)
{
    return gnuradio::make_block_sptr<sw_complex_matrix_inv_10x10_impl>(debug);
}


/*
 * The private constructor
 */
sw_complex_matrix_inv_10x10_impl::sw_complex_matrix_inv_10x10_impl(int debug)
    : gr::block("sw_complex_matrix_inv_10x10",
                gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 
                                       sizeof(input_type)),
                gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, 
                                       sizeof(output_type))),
      d_debug(debug)
{
    set_output_multiple(SIZE_SAMPLES);
    //~ set_max_noutput_items(1048576);
}

/*
 * Our virtual destructor.
 */
sw_complex_matrix_inv_10x10_impl::~sw_complex_matrix_inv_10x10_impl() {}

bool sw_complex_matrix_inv_10x10_impl::start()
{
    if (d_debug)
    {
        std::cerr << "START" << std::endl;
    }
    return true;
}

bool sw_complex_matrix_inv_10x10_impl::stop()
{
    if (d_debug)
    {
        std::cerr << "STOP" << std::endl;
    }
    return true;
}

void sw_complex_matrix_inv_10x10_impl::forecast(
    int noutput_items, 
    gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int sw_complex_matrix_inv_10x10_impl::general_work(int noutput_items,
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

    auto num_iters = noutput_items / SIZE_SAMPLES;
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        const Matrix& matrix_in = *reinterpret_cast<const Matrix*>(in + (iter_idx * SIZE_SAMPLES));
        Matrix& matrix_out = *reinterpret_cast<Matrix*>(out + (iter_idx * SIZE_SAMPLES));
        
        mat_inverse<1, 1, DIMENSION>(matrix_in, matrix_out);
    }
    
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(num_iters * SIZE_SAMPLES);

    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    if (d_debug)
    {
        std::cerr << " -- " 
                  << std::chrono::duration<double, std::milli>(diff).count() 
                  << " ms" << std::endl;
    }

    // Tell runtime system how many output items we produced.
    return (num_iters * SIZE_SAMPLES);
}

} /* namespace zynq */
} /* namespace gr */
