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
 
#include "sw_complex_matrix_difference_ratio_impl.h"
#include "matrix_utils.h"


namespace gr {
namespace zynq {

using input_type = gr_complex;
using output_type = float;


sw_complex_matrix_difference_ratio::sptr sw_complex_matrix_difference_ratio::make(int debug)
{
    return gnuradio::make_block_sptr<sw_complex_matrix_difference_ratio_impl>(debug);
}


/*
 * The private constructor
 */
sw_complex_matrix_difference_ratio_impl::sw_complex_matrix_difference_ratio_impl(int debug)
    : gr::block("sw_complex_matrix_difference_ratio",
                gr::io_signature::make(2 /* min inputs */, 2 /* max inputs */, 
                                       sizeof(input_type)),
                gr::io_signature::make(2 /* min outputs */, 2 /*max outputs */, 
                                       sizeof(output_type))),
      d_debug(debug),
      d_accumulate(0.0),
      d_counter(0)
{
}

/*
 * Our virtual destructor.
 */
sw_complex_matrix_difference_ratio_impl::~sw_complex_matrix_difference_ratio_impl() {}

bool sw_complex_matrix_difference_ratio_impl::start()
{
    if (d_debug)
    {
        std::cerr << "START" << std::endl;
    }
    return true;
}

bool sw_complex_matrix_difference_ratio_impl::stop()
{
    if (d_debug)
    {
        std::cerr << "STOP" << std::endl;
    }
    
    // Reset
    d_accumulate = 0.0;
    d_counter = 0;
    return true;
}

void sw_complex_matrix_difference_ratio_impl::forecast(
    int noutput_items, 
    gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items * SIZE_SAMPLES;
    ninput_items_required[1] = noutput_items * SIZE_SAMPLES;
}

int sw_complex_matrix_difference_ratio_impl::general_work(int noutput_items,
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
    
    auto in0 = reinterpret_cast<const input_type*>(input_items[0]);
    auto in1 = reinterpret_cast<const input_type*>(input_items[1]);
    auto out_instantaneous = reinterpret_cast<output_type*>(output_items[0]);
    auto out_average = reinterpret_cast<output_type*>(output_items[1]);

    auto num_iters = std::min(ninput_items[0] / SIZE_SAMPLES, 
                              ninput_items[1] / SIZE_SAMPLES);
    num_iters = std::min(num_iters, static_cast<uint32_t>(noutput_items));
                              
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        const Matrix& matrix_in0 = *reinterpret_cast<const Matrix*>(in0 + (iter_idx * SIZE_SAMPLES));
        const Matrix& matrix_in1 = *reinterpret_cast<const Matrix*>(in1 + (iter_idx * SIZE_SAMPLES));
        
        ++d_counter;
        
        double ratio = difference_ratio<9, 9, std::complex<float>>(const_cast<Matrix&>(matrix_in0)[0], 
                                                                   const_cast<Matrix&>(matrix_in1)[0]);
        
        // Output both the instantaneous ratio as well as the updated average
        *(out_instantaneous + iter_idx) = static_cast<float>(ratio);
        d_accumulate += ratio;
        *(out_average + iter_idx) = static_cast<float>(d_accumulate / d_counter);
    }
    
    // Tell runtime system how many input items we consumed on each input stream.
    consume_each(num_iters * SIZE_SAMPLES);

    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    if (d_debug)
    {
        std::cerr << " -- accumulate: " << d_accumulate
                  << " -- average: " << static_cast<float>(d_accumulate / d_counter)
                  << " -- " 
                  << std::chrono::duration<double, std::milli>(diff).count() 
                  << " ms" << std::endl;
    }
    
    produce(0, num_iters);
    produce(1, num_iters);

    // Tell runtime system how many output items we produced.
    return WORK_CALLED_PRODUCE;
}

} /* namespace zynq */
} /* namespace gr */
