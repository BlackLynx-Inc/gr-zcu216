/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_ZYNQ_COMPLEX_MATRIX_INV_10x10_IMPL_H
#define INCLUDED_ZYNQ_ZYNQ_COMPLEX_MATRIX_INV_10x10_IMPL_H

#include <complex>
#include <cstdint>

#include <zynq/zynq_complex_matrix_inv_10x10.h>

#include "zynq_buffer.h"

namespace gr {
namespace zynq {

class zynq_complex_matrix_inv_10x10_impl : public zynq_complex_matrix_inv_10x10
{
private:
    uint32_t d_device_index;
    bool d_debug;
    
public:

    static const uint32_t FIRMWARE_COMP_MAT_INV_ID{0x3};

    static const uint32_t BATCH_SIZE{4};
    static const uint32_t DIMENSION{9};
    static const uint32_t SIZE_SAMPLES{BATCH_SIZE * DIMENSION * DIMENSION};
    static const uint32_t SIZE_BYTES{SIZE_SAMPLES * sizeof(gr_complex)};
    
    typedef std::complex<float> Matrix[1][DIMENSION][DIMENSION];
    
    zynq_complex_matrix_inv_10x10_impl(int device_index, int debug);
    ~zynq_complex_matrix_inv_10x10_impl();

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

#endif /* INCLUDED_ZYNQ_ZYNQ_COMPLEX_MATRIX_INV_10x10_IMPL_H */
