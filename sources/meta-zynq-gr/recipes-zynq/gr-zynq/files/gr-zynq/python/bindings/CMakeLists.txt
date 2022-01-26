# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT zynq_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT zynq_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND zynq_python_files
    sw_CDMA2000_TC_decoder_python.cc
    sw_CDMA2000_TC_encoder_python.cc
    sw_complex_matrix_difference_ratio_python.cc
    sw_complex_matrix_inv_10x10_python.cc
    sw_vector_permute_python.cc
    zynq_CDMA2000_TC_decoder_python.cc
    zynq_complex_matrix_inv_10x10_python.cc
    zynq_fft_python.cc
    zynq_loopback_python.cc
    zynq_loopback_legacy_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(zynq 
   ../..
   gr::zynq
   "${zynq_python_files}")

install(TARGETS zynq_python DESTINATION ${GR_PYTHON_DIR}/zynq COMPONENT pythonapi)
