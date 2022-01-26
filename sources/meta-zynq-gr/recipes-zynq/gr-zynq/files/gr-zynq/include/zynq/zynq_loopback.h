/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_ZYNQ_LOOPBACK_H
#define INCLUDED_ZYNQ_ZYNQ_LOOPBACK_H

#include <gnuradio/block.h>
#include <zynq/api.h>

namespace gr {
namespace zynq {

/*!
 * \brief <+description of block+>
 * \ingroup zynq
 *
 */
class ZYNQ_API zynq_loopback : virtual public gr::block
{
public:
    typedef std::shared_ptr<zynq_loopback> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zynq::zynq_loopback.
     *
     * To avoid accidental use of raw pointers, zynq::zynq_loopback's
     * constructor is in a private implementation
     * class. zynq::zynq_loopback::make is the public interface for
     * creating new instances.
     */
    static sptr make(int device_index, int batch_size, int load);
};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_ZYNQ_LOOPBACK_H */
