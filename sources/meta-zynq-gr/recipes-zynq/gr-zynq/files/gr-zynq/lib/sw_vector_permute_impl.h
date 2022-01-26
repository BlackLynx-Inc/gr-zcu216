/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SW_VECTOR_PERMUTE_IMPL_H
#define INCLUDED_SW_VECTOR_PERMUTE_IMPL_H

#include <zynq/sw_vector_permute.h>


namespace gr {
namespace zynq {

class sw_vector_permute_impl : public sw_vector_permute
{
private:
    bool d_debug;

public:
    
    static const uint32_t VECTOR_SIZE{16384}; // for now this is fixed
    
    sw_vector_permute_impl(int debug);
    ~sw_vector_permute_impl();

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

#endif /* INCLUDED_SW_VECTOR_PERMUTE_IMPL_H */
