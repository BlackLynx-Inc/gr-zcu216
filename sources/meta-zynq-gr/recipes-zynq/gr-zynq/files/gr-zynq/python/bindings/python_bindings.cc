/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
// The following comment block is used for
// gr_modtool to insert function prototypes
// Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
    void bind_sw_CDMA2000_TC_encoder(py::module& m);
    void bind_sw_CDMA2000_TC_decoder(py::module& m);
    void bind_sw_complex_matrix_difference_ratio(py::module& m);
    void bind_sw_complex_matrix_inv_10x10(py::module& m);
    void bind_sw_vector_permute(py::module& m);
    void bind_zynq_CDMA2000_TC_decoder(py::module& m);
    void bind_zynq_complex_matrix_inv_10x10(py::module& m);
    void bind_zynq_fft(py::module& m);
    void bind_zynq_loopback(py::module& m);
    void bind_zynq_loopback_legacy(py::module& m);
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(zynq_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    /**************************************/
    // The following comment block is used for
    // gr_modtool to insert binding function calls
    // Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
    bind_sw_CDMA2000_TC_encoder(m);
    bind_sw_CDMA2000_TC_decoder(m);
    bind_sw_complex_matrix_difference_ratio(m);
    bind_sw_complex_matrix_inv_10x10(m);
    bind_sw_vector_permute(m);
    bind_zynq_CDMA2000_TC_decoder(m);
    bind_zynq_complex_matrix_inv_10x10(m);
    bind_zynq_fft(m);
    bind_zynq_loopback(m);
    bind_zynq_loopback_legacy(m);
    // ) END BINDING_FUNCTION_CALLS
}
