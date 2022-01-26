/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_ZYNQ_LOOPBACK_LEGACY_H
#define INCLUDED_ZYNQ_ZYNQ_LOOPBACK_LEGACY_H

#include <gnuradio/block.h>
#include <zynq/api.h>

namespace gr {
namespace zynq {

/*!
 * \brief <+description of block+>
 * \ingroup zynq
 *
 */
class ZYNQ_API zynq_loopback_legacy : virtual public gr::block
{
public:
    typedef std::shared_ptr<zynq_loopback_legacy> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zynq::zynq_loopback_legacy.
     *
     * To avoid accidental use of raw pointers, zynq::zynq_loopback_legacy's
     * constructor is in a private implementation
     * class. zynq::zynq_loopback_legacy::make is the public interface for
     * creating new instances.
     */
    static sptr make(int device_index, int batch_size, int load, int debug);
};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_ZYNQ_LOOPBACK_LEGACY_H */
