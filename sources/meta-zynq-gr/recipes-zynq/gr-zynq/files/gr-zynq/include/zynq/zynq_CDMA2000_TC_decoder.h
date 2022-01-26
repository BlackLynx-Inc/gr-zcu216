/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZYNQ_ZYNQ_CDMA2000_TC_DECODER_H
#define INCLUDED_ZYNQ_ZYNQ_CDMA2000_TC_DECODER_H

#include <gnuradio/block.h>
#include <zynq/api.h>

namespace gr {
namespace zynq {

/*!
 * \brief <+description of block+>
 * \ingroup zynq
 *
 */
class ZYNQ_API zynq_CDMA2000_TC_decoder : virtual public gr::block
{
public:
    typedef std::shared_ptr<zynq_CDMA2000_TC_decoder> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of 
     * zynq::zynq_CDMA2000_TC_decoder.
     *
     * To avoid accidental use of raw pointers, zynq_CDMA2000_TC_decoder's 
     * constructor is in a private implementation class. 
     * zynq::zynq_CDMA2000_TC_decoder::make is the  public interface for creating
     * new instances.
     */
    static sptr make(int device_index, int debug);
};

} // namespace zynq
} // namespace gr

#endif /* INCLUDED_ZYNQ_ZYNQ_CDMA2000_TC_DECODER_H */
