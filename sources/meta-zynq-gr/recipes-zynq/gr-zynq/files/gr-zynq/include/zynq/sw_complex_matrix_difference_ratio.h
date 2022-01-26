/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_DIFFERENCE_RATIO_H
#define INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_DIFFERENCE_RATIO_H

#include <gnuradio/block.h>
#include <zynq/api.h>

namespace gr {
namespace zynq {

/*!
 * \brief <+description of block+>
 * \ingroup zynq
 *
 */
class ZYNQ_API sw_complex_matrix_difference_ratio : virtual public gr::block
{
public:
    typedef std::shared_ptr<sw_complex_matrix_difference_ratio> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of 
     * zynq::sw_complex_matrix_difference_ratio.
     *
     * To avoid accidental use of raw pointers, zynq::sw_complex_matrix_difference_ratio's
     * constructor is in a private implementation
     * class. zynq::sw_complex_matrix_difference_ratio::make is the public 
     * interface for creating new instances.
     */
    static sptr make(int debug);
};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_SW_COMPLEX_MATRIX_DIFFERENCE_RATIO_H */
