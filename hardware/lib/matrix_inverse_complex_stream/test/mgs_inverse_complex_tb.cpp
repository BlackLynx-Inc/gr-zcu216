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
// -- File Name: mgs_inverse_complex_tb.cpp
// -- Description: Vivado HLS 2020.1 testbench for complex matrix inversion.
// --
// -------------------------------------------------------------------------------

#include "../include/constants.h"
#include "../include/mgs_inverse_complex.h"
#include "./bl_tb_utils.h"
//#include "hls/linear_algebra/utils/x_hls_matrix_utils.h"
//#include "hls/linear_algebra/utils/x_hls_matrix_tb_utils.h"
#include <random>
#include <chrono>
#include <ctime>

int main (void){

  unsigned int seed;
  std::random_device rd{};
  seed = 13; // Use a fixed number as the seed to reproduce the name numbers each time the testbench is run.
  std::cout << "Random number seed = " << seed << std::endl;
  std::mt19937 gen{seed};
  std::uniform_real_distribution<> dist(-50, 50);

  hls::stream<COMPLEX_MGS_FP_WIDE_T> A_vec_strm;
  hls::stream<COMPLEX_MGS_FP_WIDE_T> InverseA_vec_strm;

  std::complex<float> A_vec       [INVERSE_BATCH_SIZE][ROWS_COLS_A][ROWS_COLS_A];
  std::complex<float> InverseA_vec[INVERSE_BATCH_SIZE][ROWS_COLS_A][ROWS_COLS_A];
  std::complex<float> Ir          [ROWS_COLS_A][ROWS_COLS_A];
  std::complex<float> I_ref       [ROWS_COLS_A][ROWS_COLS_A];
  float val_real;
  float val_imag;
  int data_size = INVERSE_BATCH_SIZE*ROWS_COLS_A*ROWS_COLS_A;

  // Set up example data
  for (int n=0;n<INVERSE_BATCH_SIZE;n++)
  {
    for (int r=0;r<ROWS_COLS_A;r++)
    {
      for (int c=0;c<ROWS_COLS_A;c++)
      {
        std::complex<float> tmp;
        val_real=dist(gen);
        val_imag=dist(gen);
        A_vec[n][r][c].real(val_real);
        A_vec[n][r][c].imag(val_imag);
      }
    }
  }

  // Write Stream
  for (int n=0, count=0; n<INVERSE_BATCH_SIZE;n++)
  {
    for (int r=0;r<ROWS_COLS_A;r++)
    {
      for (int c=0;c<ROWS_COLS_A;c++, count++)
      {
         COMPLEX_MGS_FP_WIDE_T temp;
         temp.data.data[0] = A_vec[n][r][c];
         temp.last = (count == (data_size-1)) ? 1 : 0;
         temp.keep = -1;
         A_vec_strm.write(temp);
      }
    }
  }

  // Call wrapped QR Inverse
  mgs_inverse_complex(A_vec_strm,InverseA_vec_strm);

  // Read stream
  for (int x=0; x<INVERSE_BATCH_SIZE; x++)
  {
    for (int r=0;r<ROWS_COLS_A;r++)
    {
      for (int c=0;c<ROWS_COLS_A;c++)
      {
        COMPLEX_MGS_FP_WIDE_T temp;
        #pragma HLS PIPELINE
        temp = InverseA_vec_strm.read();
        //InverseA_vec[x][r][c] = temp.data.data[0];
        InverseA_vec[x][r][c] = temp.data.data[0];
      }
    }
  }

  // Generate reference I
  for (int r=0;r<ROWS_COLS_A;r++)
  {
    for (int c=0;c<ROWS_COLS_A;c++)
    {
      I_ref[r][c] = 0.0;
      if ( r == c )
      {
        I_ref[r][c] = 1.0;
      }
    }
  }

  int fail = 0;
  float dr = 0;
  float largestDR = 0;
  int drIdxMax = 0;

  for (int n=0;n<INVERSE_BATCH_SIZE;n++) {
    // Compute Ir from A and A inverse: Ir = A * InverseA
    MatMult<ROWS_COLS_A,ROWS_COLS_A,ROWS_COLS_A,std::complex<float>> (A_vec[n], InverseA_vec[n], Ir);

    // Print results
    printf("A = \n");
    print_matrix<ROWS_COLS_A, ROWS_COLS_A, std::complex<float>, NoTranspose>(A_vec[n], "   ");

    printf("InverseA = \n");
    print_matrix<ROWS_COLS_A, ROWS_COLS_A, std::complex<float>, NoTranspose>(InverseA_vec[n], "   ");

    printf("I reconstructed = \n");
    print_matrix<ROWS_COLS_A, ROWS_COLS_A, std::complex<float>, NoTranspose>(Ir, "   ");

    dr = difference_ratio<ROWS_COLS_A, ROWS_COLS_A>(Ir,I_ref);
    printf("Difference Test Ratio Matrix %i = %f.\n\n", n, dr);

    if (dr > largestDR)
    {
       largestDR = dr;
       drIdxMax = n;
    }
    // Generate error ratio and compare against threshold value
    // - LAPACK error measurement method
    // - Threshold taken from LAPACK test functions
    if ( dr > 30.0 )
    {
      fail = 1;
    }
  }

  if ( fail == 1 )
  {
     printf("TEST FAILED\n");
     printf("Maximum DR for all Matrix Inverses = %f at matrix %i.\n\n", largestDR, drIdxMax);

  }
  else
  {
     printf("TEST PASSED\n");
     printf("Maximum DR for all Matrix Inverses = %f at matrix %i.\n\n", largestDR, drIdxMax);
  }

  return (0);
}

