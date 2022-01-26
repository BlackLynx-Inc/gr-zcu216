// -------------------------------------------------------------------------------
// --                   LICENSE NOTICE
// -------------------------------------------------------------------------------
// --
// -- Copyright 2006-2011,2013-2014 Free Software Foundation, Inc.
// --
// -- This file is part of GNU Radio
// --
// -- SPDX-License-Identifier: GPL-3.0-or-later
// --
// --
// -------------------------------------------------------------------------------
// --
// -- Company:     BlackLynx, Inc.
// --
// -- File Name: mgs_inverse_complex.h
// -- Description: Header include file  for Vivado 2020.1 HLS Implementation
// --              streamed/batched complex matrix inversion.
// --
// -- Dependencies: "hls_linear_algebra.h" Vivado HLS Linear Algebra library.
// --
// -------------------------------------------------------------------------------


#ifndef MGS_INVERSE_COMPLEX_H
#define MGS_INVERSE_COMPLEX_H

//#include "hls_linear_algebra.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

// AXI stream interface type
typedef hls::axis<COMPLEX_FP_WIDE_T<MGS_DATA_VECTORIZATION_FACTOR>, 0, 0, 0> COMPLEX_MGS_FP_WIDE_T;

void mgs_inverse_complex(hls::stream<COMPLEX_MGS_FP_WIDE_T> &A_vec,
                         hls::stream<COMPLEX_MGS_FP_WIDE_T> &InverseA_vec);

// BlackLynx implementation of matrix inversion using modified Gram-Schmidt QR decomposition
template <int NUM_ITERATIONS, int ROWS_COLS_A>
void mgs_qr_inverse_complex_stream_vectorized(MATRIX_IN_T A_vec[INVERSE_PARALLELIZATION_FACTOR][ROWS_COLS_A][ROWS_COLS_A],
                                              MATRIX_OUT_T InverseA_vec[INVERSE_PARALLELIZATION_FACTOR][ROWS_COLS_A][ROWS_COLS_A]);

// Custom Gram-Schmidt Reduced Decomposition for rectangular matrices
template <int NUM_ITERATIONS, int ROWS_COLS_A>
void mgs_qrd_complex_vectorized(MATRIX_IN_T  a_matrix_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
                                MATRIX_OUT_T q_matrix_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
                                MATRIX_OUT_T r_matrix_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A]);

template <int ROWS_COLS_A>
void mgs_qrd_complex(MATRIX_IN_T  a_matrix[ROWS_COLS_A][ROWS_COLS_A],
                     MATRIX_OUT_T q_matrix[ROWS_COLS_A][ROWS_COLS_A],
                     MATRIX_OUT_T r_matrix[ROWS_COLS_A][ROWS_COLS_A]);

template <int ROWS_COLS_A>
float magnitudeColumn(MATRIX_IN_T matrixData[ROWS_COLS_A][ROWS_COLS_A], int targetColumn);

template <int NUM_ITERATIONS, int ROWS_COLS_A>
void QR_to_InverseA_vectorized(MATRIX_IN_T Q_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
                               MATRIX_IN_T R_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
                               MATRIX_OUT_T InverseA_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A]);

template <int ROWS_COLS_A>
void invert_R(MATRIX_IN_T  R_vec[ROWS_COLS_A][ROWS_COLS_A],
              MATRIX_OUT_T InverseR_vec[ROWS_COLS_A][ROWS_COLS_A]);

template <int ROWS_COLS_A>
void matrix_multiply_RinvQt(MATRIX_IN_T  InverseR[ROWS_COLS_A][ROWS_COLS_A],
                            MATRIX_IN_T  Q_[ROWS_COLS_A][ROWS_COLS_A],
                            MATRIX_OUT_T InverseA[ROWS_COLS_A][ROWS_COLS_A]);

#endif

