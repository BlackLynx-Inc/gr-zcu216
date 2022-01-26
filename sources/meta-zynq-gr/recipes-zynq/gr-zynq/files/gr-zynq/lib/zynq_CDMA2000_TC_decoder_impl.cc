/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <chrono>
#include <cstdint>
#include <sstream>
#include <thread>

#include <gnuradio/io_signature.h>
 
#include "zynq_CDMA2000_TC_decoder_impl.h"
#include <dma-proxy-lib.h>

namespace gr {
namespace zynq {

using input_type = float;
using output_type = char;


zynq_CDMA2000_TC_decoder::sptr zynq_CDMA2000_TC_decoder::make(
    int device_index, 
    int debug)
{
    return gnuradio::make_block_sptr<zynq_CDMA2000_TC_decoder_impl>(device_index, debug);
}


/*
 * The private constructor
 */
zynq_CDMA2000_TC_decoder_impl::zynq_CDMA2000_TC_decoder_impl(
    uint32_t device_index, 
    int debug)
    : gr::block("zynq_CDMA2000_TC",
                gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 
                                       sizeof(input_type)),
                gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, 
                                       sizeof(output_type))),
      d_device_index(device_index),
      d_debug(debug)
{
    if (device_index < 0 || device_index > 1)
    {
        throw std::runtime_error("Device index is out of range!\n");
    }
    
    // Read registers to confirm firmware availablility
    uint32_t offset = 0x40000;
    if (device_index == 1)
    {
        offset = 0x50000;
    }
    
    // Read the register space offset for the selected device/partition
    uint32_t reg_space_offset = dmap_reg_read32(offset);
    if (reg_space_offset > REG_SPACE_BASE_ADDR)
    {
        reg_space_offset -= REG_SPACE_BASE_ADDR;
    }
    
    // Perform version/ID check (id is upper 16 bits, version is lower 16)
    uint32_t firmware_id = dmap_reg_read32(reg_space_offset) >> 16;
    if (firmware_id != FIRMWARE_TC_DEC_ID)
    {
        std::ostringstream msg;
        msg << "Firmware ID: " << firmware_id << " does not match expected "
            << "value: " << FIRMWARE_TC_DEC_ID << "! Please select a different "
            << "device/partition ID or load the correct firmware image.\n";
        throw std::runtime_error(msg.str());
    }
    
    set_relative_rate((uint64_t)MSG_BITS, (uint64_t)CW_BITS);
    
    // This call causes the scheduler to ensure that the noutput_items
    // argument passed to forecast and general_work will be an integer multiple
    // of the passed value.
    set_output_multiple(MSG_BITS);
    set_min_noutput_items(MSG_BITS);
}

/*
 * Our virtual destructor.
 */
zynq_CDMA2000_TC_decoder_impl::~zynq_CDMA2000_TC_decoder_impl() {}

bool zynq_CDMA2000_TC_decoder_impl::start()
{
    if (d_debug)
    {
        std::cerr << "START" << std::endl;
    }
    return true;
}

bool zynq_CDMA2000_TC_decoder_impl::stop()
{
    if (d_debug)
    {
        std::cerr << "STOP" << std::endl;
    }
    return true;
}

void zynq_CDMA2000_TC_decoder_impl::forecast(
    int noutput_items, 
    gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items / CODE_RATE;
}

int zynq_CDMA2000_TC_decoder_impl::general_work(int noutput_items,
                                              gr_vector_int& ninput_items,
                                              gr_vector_const_void_star& input_items,
                                              gr_vector_void_star& output_items)
{
    static uint32_t ctr = 0;
    if (d_debug)
    {
        std::cerr << "WORK called: " << ++ctr << " -- noutput_items: " 
                  << noutput_items;
    }
    
    auto start = std::chrono::system_clock::now();
    
    auto in = reinterpret_cast<const input_type*>(input_items[0]);
    auto out = reinterpret_cast<output_type*>(output_items[0]);
    
    uint32_t num_input_items = ninput_items[0];
    uint32_t num_iters = num_input_items / CW_BITS;
    
    if (d_debug)
    {
        std::cerr << " -- num_in_items: " << num_input_items 
                  << " -- num iters: " << num_iters;
    }
    
    // Call the encoder for each input message
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        // Kick off the read operation
        int rc = dmap_read_nb(d_device_index, 
                              (void*)(out + (iter_idx * MSG_BITS)), 
                              MSG_BITS);
        if (rc)
        {
            std::cerr << (d_debug ? "\n" : "") 
                      << "DMA read failed: " << rc << std::endl;
        }
        
        // Write/transmit the data
        rc = dmap_write(d_device_index, 
                        (void*)(in + (iter_idx * CW_BITS)), 
                        CW_BITS);
        if (rc)
        {
            std::cerr << (d_debug ? "\n" : "") 
                      << "DMA write failed: " << rc << std::endl;
        }
        
        // Wait for the read operation to complete
        rc = dmap_read_complete(d_device_index, (void*)out);
        if (rc)
        {
            std::cerr << (d_debug ? "\n" : "") 
                      << "DMA read complete failed: " << rc << std::endl;
        }
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(num_iters * CW_BITS);

    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    if (d_debug)
    {
        std::cerr << " -- " 
                  << std::chrono::duration<double, std::milli>(diff).count() 
                  << " ms" << std::endl;
    }

    // Tell runtime system how many output items we produced.
    return (num_iters * MSG_BITS);
}

} /* namespace zynq */
} /* namespace gr */
