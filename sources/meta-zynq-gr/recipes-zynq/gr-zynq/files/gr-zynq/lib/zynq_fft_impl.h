/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_ZYNQ_FFT_IMPL_H
#define INCLUDED_ZYNQ_ZYNQ_FFT_IMPL_H

#include <zynq/zynq_fft.h>

#include "zynq_buffer.h"

namespace gr {
namespace zynq {

class zynq_fft_impl : public zynq_fft
{
private:
    uint32_t d_device_index;
    bool d_debug;
    
    static const uint32_t d_fft_size{16384}; // FFT size is fixed in the firmware to 16k

public:
    
    static const uint32_t FIRMWARE_FFT_ID{0x2};

    zynq_fft_impl(int device_index, int debug);
    ~zynq_fft_impl();

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

#endif /* INCLUDED_ZYNQ_ZYNQ_FFT_IMPL_H */
