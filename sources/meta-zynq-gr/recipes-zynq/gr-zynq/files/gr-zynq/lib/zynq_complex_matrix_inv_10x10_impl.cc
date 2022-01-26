/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <chrono>
#include <sstream>
#include <thread>
 
#include "zynq_complex_matrix_inv_10x10_impl.h"
#include <gnuradio/io_signature.h>
#include <dma-proxy-lib.h>


namespace gr {
namespace zynq {

using input_type = gr_complex;
using output_type = gr_complex;


zynq_complex_matrix_inv_10x10::sptr zynq_complex_matrix_inv_10x10::make(int device_index, int debug)
{
    return gnuradio::make_block_sptr<zynq_complex_matrix_inv_10x10_impl>(device_index, debug);
}


/*
 * The private constructor
 */
zynq_complex_matrix_inv_10x10_impl::zynq_complex_matrix_inv_10x10_impl(int device_index, int debug)
    : gr::block("zynq_fft",
                gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 
                                       sizeof(input_type), zynq_buffer::type),
                gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, 
                                       sizeof(output_type), zynq_buffer::type)),
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
    if (firmware_id != FIRMWARE_COMP_MAT_INV_ID)
    {
        std::ostringstream msg;
        msg << "Firmware ID: " << firmware_id << " does not match expected "
            << "value: " << FIRMWARE_COMP_MAT_INV_ID << "! Please select a different "
            << "device/partition ID or load the correct firmware image.\n";
        //~ std::cerr << msg.str() << std::endl;
        throw std::runtime_error(msg.str());
    }

    set_output_multiple(SIZE_SAMPLES);
    //~ set_max_noutput_items(1048576);
}

/*
 * Our virtual destructor.
 */
zynq_complex_matrix_inv_10x10_impl::~zynq_complex_matrix_inv_10x10_impl() {}

bool zynq_complex_matrix_inv_10x10_impl::start()
{
    if (d_debug)
    {
        std::cerr << "START" << std::endl;
    }
    return true;
}

bool zynq_complex_matrix_inv_10x10_impl::stop()
{
    if (d_debug)
    {
        std::cerr << "STOP" << std::endl;
    }
    return true;
}

void zynq_complex_matrix_inv_10x10_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int zynq_complex_matrix_inv_10x10_impl::general_work(int noutput_items,
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
    
    auto num_iters = noutput_items / SIZE_SAMPLES;
    for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
    {
        // Kick off the read operation
        int rc = dmap_read_nb(d_device_index, 
                              (void*)(out + (iter_idx * SIZE_SAMPLES)), 
                              SIZE_BYTES);
        if (rc)
        {
            std::cerr << (d_debug ? "\n" : "") 
                      << "DMA read failed: " << rc << std::endl;
        }
        
        // Write/transmit the data
        rc = dmap_write(d_device_index, 
                        (void*)(in + (iter_idx * SIZE_SAMPLES)), 
                        SIZE_BYTES);
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
    consume_each(num_iters * SIZE_SAMPLES);

    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    if (d_debug)
    {
        std::cerr << " -- " 
                  << std::chrono::duration <double, std::milli>(diff).count() 
                  << " ms" << std::endl;
    }

    // Tell runtime system how many output items we produced.
    return (num_iters * SIZE_SAMPLES);
}

} /* namespace zynq */
} /* namespace gr */
