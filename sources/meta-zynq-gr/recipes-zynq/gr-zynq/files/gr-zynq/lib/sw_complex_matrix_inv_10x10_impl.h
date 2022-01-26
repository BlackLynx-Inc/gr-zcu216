/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_INV_10x10_IMPL_H
#define INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_INV_10x10_IMPL_H

#include <complex>
#include <cstdint>

#include <zynq/sw_complex_matrix_inv_10x10.h>


namespace gr {
namespace zynq {

class sw_complex_matrix_inv_10x10_impl : public sw_complex_matrix_inv_10x10
{
private:
    bool d_debug;
    
public:

    static const uint32_t DIMENSION{9};
    static const uint32_t SIZE_SAMPLES{DIMENSION * DIMENSION};
    static const uint32_t SIZE_BYTES{SIZE_SAMPLES * sizeof(gr_complex)};
    
    typedef std::complex<float> Matrix[1][DIMENSION][DIMENSION];
    
    sw_complex_matrix_inv_10x10_impl(int debug);
    ~sw_complex_matrix_inv_10x10_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    bool start();
    bool stop();

};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_INV_10x10_IMPL_H */
