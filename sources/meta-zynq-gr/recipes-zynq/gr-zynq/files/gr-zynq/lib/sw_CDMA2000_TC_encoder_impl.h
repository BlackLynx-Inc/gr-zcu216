/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_SW_CDMA2000_TC_ENCODER_IMPL_H
#define INCLUDED_ZYNQ_SW_CDMA2000_TC_ENCODER_IMPL_H

#include <cstdint>

#include <zynq/sw_CDMA2000_TC_encoder.h>

namespace gr {
namespace zynq {

class sw_CDMA2000_TC_encoder_impl : public sw_CDMA2000_TC_encoder
{
public:

    static constexpr uint32_t MSG_BITS{762};    // aka K - message length
    static constexpr uint32_t MSG_BYTES{96};

    static constexpr uint32_t CW_BITS{1536};   // aka N - codeword length
    static constexpr uint32_t CW_BYTES{192};
    
    static constexpr double CODE_RATE{static_cast<double>(MSG_BITS) / 
                                      static_cast<double>(CW_BITS)}; // K / N

    sw_CDMA2000_TC_encoder_impl(int debug);
    ~sw_CDMA2000_TC_encoder_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    bool start();
    bool stop();

private:
    bool d_debug;
    char m_channel_bytes[CW_BYTES];
    char m_codeword_bytes[CW_BYTES];
    char m_codeword_bits[CW_BITS];

};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_SW_CDMA2000_TC_ENCODER_IMPL_H */
