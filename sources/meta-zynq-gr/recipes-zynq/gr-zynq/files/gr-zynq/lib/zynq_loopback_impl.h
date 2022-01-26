/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_ZYNQ_LOOPBACK_IMPL_H
#define INCLUDED_ZYNQ_ZYNQ_LOOPBACK_IMPL_H

#include <zynq/zynq_loopback.h>

#include "zynq_buffer.h"

namespace gr {
namespace zynq {

class zynq_loopback_impl : public zynq_loopback
{
private:
    uint32_t d_device_index;
    int d_batch_size;
    int d_load;

public:
    zynq_loopback_impl(int device_index, int batch_size, int load);
    ~zynq_loopback_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_ZYNQ_LOOPBACK_IMPL_H */
