/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_INV_10x10_H
#define INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_INV_10x10_H

#include <gnuradio/block.h>
#include <zynq/api.h>

namespace gr {
namespace zynq {

/*!
 * \brief <+description of block+>
 * \ingroup zynq
 *
 */
class ZYNQ_API zynq_complex_matrix_inv_10x10 : virtual public gr::block
{
public:
    typedef std::shared_ptr<zynq_complex_matrix_inv_10x10> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of 
     * zynq::zynq_complex_matrix_inv_10x10.
     *
     * To avoid accidental use of raw pointers, zynq::zynq_complex_matrix_inv_10x10's
     * constructor is in a private implementation
     * class. zynq::zynq_complex_matrix_inv_10x10::make is the public interface for
     * creating new instances.
     */
    static sptr make(int device_index, int debug);
};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_INV_10x10_H */
