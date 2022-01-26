/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <cstring>
#include <sstream>
#include <stdexcept>

#include <gnuradio/block.h>
#include "zynq_buffer.h"

namespace gr {
namespace zynq {
    
buffer_type zynq_buffer::type(buftype<zynq_buffer, zynq_buffer>{});

void* zynq_buffer::device_memcpy(void* dest, const void* src, std::size_t count)
{
    return std::memcpy(dest, src, count);
}

void* zynq_buffer::device_memmove(void* dest, const void* src, std::size_t count)
{
    return std::memmove(dest, src, count);
}


zynq_buffer::zynq_buffer(int nitems,
                         size_t sizeof_item,
                         uint64_t downstream_lcm_nitems,
                         uint32_t downstream_max_out_mult,
                         block_sptr link,
                         block_sptr buf_owner)
    : buffer_single_mapped(nitems, sizeof_item, downstream_lcm_nitems, 
                           downstream_max_out_mult, link, buf_owner),
      d_buffer(nullptr)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "zynq_buffer");
    if (!allocate_buffer(nitems))
        throw std::bad_alloc();
}

zynq_buffer::~zynq_buffer() {}

void zynq_buffer::post_work(int nitems)
{
#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << this << "] "
        << "zynq_buffer [" << d_transfer_type << "] -- post_work: " << nitems;
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    if (nitems <= 0) {
        return;
    }

    // NOTE: when this function is called the write pointer has not yet been
    // advanced so it can be used directly as the source ptr
    switch (d_transfer_type) {
    case transfer_type::HOST_TO_DEVICE:
    case transfer_type::DEVICE_TO_HOST:
    case transfer_type::DEVICE_TO_DEVICE:
        // No op
        break;

    default:
        std::ostringstream msg;
        msg << "Unexpected transfer type for zynq_buffer: " << d_transfer_type;
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error(msg.str());
    }
}

bool zynq_buffer::do_allocate_buffer(size_t final_nitems, size_t sizeof_item)
{
#ifdef BUFFER_DEBUG
    {
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "zynq_buffer constructor -- nitems: " << final_nitems;
        GR_LOG_DEBUG(d_logger, msg.str());
    }
#endif

    d_buffer = (char*)dmap_alloc_buffer(final_nitems * sizeof_item);
    if (d_buffer == nullptr)
    {
        return false;
    }
    
    d_base = d_buffer;

    return true;
}

void* zynq_buffer::write_pointer()
{
    void* ptr = nullptr;
    switch (d_transfer_type) {
    case transfer_type::HOST_TO_DEVICE:
    case transfer_type::DEVICE_TO_HOST:
    case transfer_type::DEVICE_TO_DEVICE:
        // Zynq has unified memory
        ptr = &d_base[d_write_index * d_sizeof_item];
        break;

    default:
        std::ostringstream msg;
        msg << "Unexpected transfer type for zynq_buffer: " << d_transfer_type;
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error(msg.str());
    }

    return ptr;
}

const void* zynq_buffer::_read_pointer(unsigned int read_index)
{
    void* ptr = nullptr;
    switch (d_transfer_type) {
    case transfer_type::HOST_TO_DEVICE:
    case transfer_type::DEVICE_TO_DEVICE:
    case transfer_type::DEVICE_TO_HOST:
        // Zynq has unified memory
        ptr = &d_base[read_index * d_sizeof_item];
        break;

    default:
        std::ostringstream msg;
        msg << "Unexpected transfer type for zynq_buffer: " << d_transfer_type;
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error(msg.str());
    }

    return ptr;
}

bool zynq_buffer::input_blocked_callback(int items_required,
                                         int items_avail,
                                         unsigned read_index)
{
#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << this << "] "
        << "zynq_buffer [" << d_transfer_type << "] -- input_blocked_callback";
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    bool rc = false;
    switch (d_transfer_type) {
    case transfer_type::HOST_TO_DEVICE:
    case transfer_type::DEVICE_TO_DEVICE:
    case transfer_type::DEVICE_TO_HOST:
        // Adjust host buffer
        rc = input_blocked_callback_logic(
            items_required, items_avail, read_index, d_base, std::memcpy, std::memmove);
        break;

    default:
        std::ostringstream msg;
        msg << "Unexpected transfer type for zynq_buffer: " << d_transfer_type;
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error(msg.str());
    }

    return rc;
}

bool zynq_buffer::output_blocked_callback(int output_multiple, bool force)
{
#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << this << "] "
        << "zynq_buffer [" << d_transfer_type << "] -- output_blocked_callback";
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    bool rc = false;
    switch (d_transfer_type) {
    case transfer_type::HOST_TO_DEVICE:
    case transfer_type::DEVICE_TO_HOST:
    case transfer_type::DEVICE_TO_DEVICE:
        // Adjust host buffer
        rc = output_blocked_callback_logic(output_multiple, force, d_base, std::memmove);
        break;
    
    default:
        std::ostringstream msg;
        msg << "Unexpected transfer type for zynq_buffer: " << d_transfer_type;
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error(msg.str());
    }

    return rc;
}

buffer_sptr zynq_buffer::make_buffer(int nitems,
                                     size_t sizeof_item,
                                     uint64_t downstream_lcm_nitems,
                                     uint32_t downstream_max_out_mult,
                                     block_sptr link,
                                     block_sptr buf_owner)
{
    return buffer_sptr(new zynq_buffer(nitems, 
                                       sizeof_item, 
                                       downstream_lcm_nitems, 
                                       downstream_max_out_mult,
                                       link, 
                                       buf_owner));
}


} /* namespace zynq */
} /* namespace gr */
