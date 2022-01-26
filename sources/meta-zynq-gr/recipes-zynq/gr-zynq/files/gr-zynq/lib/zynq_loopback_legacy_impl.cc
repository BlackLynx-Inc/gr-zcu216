/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <chrono>
#include <sstream>
 
#include "zynq_loopback_legacy_impl.h"
#include <gnuradio/io_signature.h>
#include <dma-proxy-lib.h>

namespace gr {
namespace zynq {

using input_type = gr_complex;
using output_type = gr_complex;


zynq_loopback_legacy::sptr zynq_loopback_legacy::make(int device_index, 
                                                      int batch_size, int load,
                                                      int debug)
{
    return gnuradio::make_block_sptr<zynq_loopback_legacy_impl>(device_index, 
                                                                batch_size, 
                                                                load, debug);
}


/*
 * The private constructor
 */
zynq_loopback_legacy_impl::zynq_loopback_legacy_impl(int device_index, 
                                                     int batch_size, 
                                                     int load,
                                                     int debug)
    : gr::block("zynq_loopback_legacy",
                gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 
                                       sizeof(input_type)),
                gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, 
                                       sizeof(output_type))),
      d_device_index(device_index),
      d_batch_size(batch_size),
      d_load(load),
      d_debug(debug),
      d_dma_write_buffer(nullptr),
      d_dma_read_buffer(nullptr)
{
    if (device_index < 0)
    {
        throw std::runtime_error("Device index is out of range");
    }
    
    set_output_multiple(d_batch_size);
    
    // Chicken and egg problem... set buffer size up front
    set_max_noutput_items(1048576);
    uint32_t bufsize = d_batch_size * sizeof(input_type);
    
    // Allocate DMA buffers
    d_dma_write_buffer = dmap_alloc_buffer(bufsize);
    if (d_dma_write_buffer == NULL)
    {
        throw std::runtime_error("Unable to allocate DMA buffer");
    }
    if (d_debug)
    {
        std::cerr << "Write buffer: " << d_dma_write_buffer << std::endl;
    }
    
    d_dma_read_buffer = dmap_alloc_buffer(bufsize);
    if (d_dma_read_buffer == NULL)
    {
        throw std::runtime_error("Unable to allocate DMA buffer");
    }
    if (d_debug)
    {
        std::cerr << "Read buffer: " << d_dma_read_buffer << std::endl;
    }
}

/*
 * Our virtual destructor.
 */
zynq_loopback_legacy_impl::~zynq_loopback_legacy_impl() {}

void zynq_loopback_legacy_impl::forecast(int noutput_items, 
                                         gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int zynq_loopback_legacy_impl::general_work(int noutput_items,
                                            gr_vector_int& ninput_items,
                                            gr_vector_const_void_star& input_items,
                                            gr_vector_void_star& output_items)
{
    static uint32_t ctr = 0;
    if (d_debug)
    {
        std::cerr << "WORK called: " << identifier() << " " << ++ctr 
                  << " -- noutput_items: " << noutput_items;
    }
    
    auto start = std::chrono::system_clock::now();
    
    auto in = reinterpret_cast<const input_type*>(input_items[0]);
    auto out = reinterpret_cast<output_type*>(output_items[0]);
    
    // Copy data into input (write) DMA buffer
    //~ std::memcpy(d_dma_write_buffer, in, noutput_items * sizeof(input_type));
    
    auto pt1 = std::chrono::system_clock::now();
    
    auto num_iters = noutput_items / d_batch_size;
    uint32_t xfer_size = d_batch_size * sizeof(input_type);
    
    if (d_debug)
    {
        std::cerr << " -- num_iters: " << num_iters << " -- xfer size: " 
                  << xfer_size;
    }
    
    for (uint32_t idx = 0; idx < d_load; ++idx)
    {
        for (uint32_t iter_idx = 0; iter_idx < num_iters; ++iter_idx)
        {
            std::memcpy(d_dma_write_buffer, in + (iter_idx * d_batch_size), xfer_size);
            
            // Kick off the read operation
            int rc = dmap_read_nb(d_device_index, (void*)(d_dma_read_buffer), 
                                  xfer_size);
            if (rc)
            {
                std::cerr << "DMA read failed: " << rc << std::endl;
            }
            
            // Write/transmit the data
            rc = dmap_write(d_device_index, (void*)(d_dma_write_buffer), xfer_size);
            if (rc)
            {
                std::cerr << "DMA write failed: " << rc << std::endl;
            }
            
            // Wait for the read operation to complete
            rc = dmap_read_complete(d_device_index, (void*)d_dma_read_buffer);
            if (rc)
            {
                std::cerr << "DMA read complete failed: " << rc << std::endl;
            }
            
            std::memcpy(out + (iter_idx * d_batch_size), d_dma_read_buffer, xfer_size);
        }
    }
    
    auto pt2 = std::chrono::system_clock::now();
    
    // Copy data out of output (read) DMA buffer
    //~ std::memcpy(out, d_dma_read_buffer, noutput_items * sizeof(input_type));
    
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(num_iters * d_batch_size);
    
    if (d_debug)
    {
        auto end = std::chrono::system_clock::now();
        auto diff_memcpy1 = pt1 - start;
        auto diff_xfer = pt2 - pt1;
        auto diff_memcpy2 = end - pt2;
        auto diff = end - start;
        
        std::cerr << " -- " 
                  << std::chrono::duration <double, std::milli>(diff).count() << " "
                  << std::chrono::duration <double, std::milli>(diff_memcpy1).count() << " "
                  << std::chrono::duration <double, std::milli>(diff_xfer).count() << " "
                  << std::chrono::duration <double, std::milli>(diff_memcpy2).count() << " "
                  << " ms" << std::endl;
    }

    // Tell runtime system how many output items we produced.
    return (num_iters * d_batch_size);
}

} /* namespace zynq */
} /* namespace gr */
