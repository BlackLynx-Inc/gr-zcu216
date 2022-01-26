/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_ZYNQ_CDMA2000_TC_DECODER_IMPL_H
#define INCLUDED_ZYNQ_ZYNQ_CDMA2000_TC_DECODER_IMPL_H

#include <cstdint>

#include <zynq/zynq_CDMA2000_TC_decoder.h>

namespace gr {
namespace zynq {

class zynq_CDMA2000_TC_decoder_impl : public zynq_CDMA2000_TC_decoder
{
private:
    uint32_t d_device_index;
    bool d_debug;
    
public:

    static constexpr uint32_t MSG_BITS{762};    // aka K - message length
    static constexpr uint32_t MSG_BYTES{96};

    static constexpr uint32_t CW_BITS{1536};   // aka N - codeword length
    static constexpr uint32_t CW_BYTES{192};
    
    static constexpr uint32_t FIRMWARE_TC_DEC_ID{0x4};
    
    static constexpr double CODE_RATE{static_cast<double>(MSG_BITS) / 
                                      static_cast<double>(CW_BITS)}; // K / N

    zynq_CDMA2000_TC_decoder_impl(uint32_t device_index, int debug);
    ~zynq_CDMA2000_TC_decoder_impl();

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

#endif /* INCLUDED_ZYNQ_ZYNQ_CDMA2000_TC_DECODER_IMPL_H */
