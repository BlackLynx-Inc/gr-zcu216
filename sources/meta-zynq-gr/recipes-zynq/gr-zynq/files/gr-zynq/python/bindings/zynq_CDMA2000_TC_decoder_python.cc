/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(zynq_CDMA2000_TC_decoder.h)                                  */
/* BINDTOOL_HEADER_FILE_HASH(11dc6317eb0b876297db728dd8149fba)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <zynq/zynq_CDMA2000_TC_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <zynq_CDMA2000_TC_decoder_pydoc.h>

void bind_zynq_CDMA2000_TC_decoder(py::module& m)
{

    using zynq_CDMA2000_TC_decoder = gr::zynq::zynq_CDMA2000_TC_decoder;


    py::class_<zynq_CDMA2000_TC_decoder, gr::block, gr::basic_block,
        std::shared_ptr<zynq_CDMA2000_TC_decoder>>(m, "zynq_CDMA2000_TC_decoder", D(zynq_CDMA2000_TC_decoder))

        .def(py::init(&zynq_CDMA2000_TC_decoder::make),
           py::arg("device_index"),
           py::arg("debug"),
           D(zynq_CDMA2000_TC_decoder,make)
        )
        



        ;




}
