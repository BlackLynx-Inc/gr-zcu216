// -------------------------------------------------------------------------------
// --                   PROPRIETARY NOTICE
// -------------------------------------------------------------------------------
// -- (c) Copyright 2021 BlackLynx, Inc. All rights reserved.
// --
// -- This file contains BlackLynx, Inc. proprietary information. It is the
// -- property of BlackLynx, Inc. and shall not be used, disclosed to others or
// -- reproduced without the express written consent of BlackLynx, Inc.,
// -- including, but without limitation, it is not to be used in the creation,
// -- manufacture, development, or derivation of any designs, or configuration.
// --
// -------------------------------------------------------------------------------
// --
// -- Company:     BlackLynx, Inc.
// --
// -- File Name: bl_tb_utils.h
// -- Description: Matrix Utilities header include file for Vivado HLS testbenches.
// --
// -------------------------------------------------------------------------------
// --                                DETAILS
// -------------------------------------------------------------------------------
// --
// -- TBD
// --
// -------------------------------------------------------------------------------
// --                                HISTORY
// -------------------------------------------------------------------------------
// --
// -------------------------------------------------------------------------------

#ifndef BL_TB_UTILS_H
#define BL_TB_UTILS_H

#include <complex>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdio.h>

template <typename T> double norm1_abs(T x)  {
  const double ONE  = 1.0;
  const double ZERO = 0.0;
  double t, re, im;

  re = fabs((double)x.real());
  im = fabs((double)x.imag());

  if (re > im) {
    t = im/re;
    return re*sqrt(ONE+t*t);
  } else {
    if (im == ZERO) {
      return ZERO;
    }
    t = re/im;
    return im*sqrt(ONE+t*t);
  }
}

template<>
double norm1_abs<float>(float x)
{
  const double ONE  = 1.0;
  const double ZERO = 0.0;
  double t, re, im;

  re = fabs((double)x);
  im = 0.0;
  if (re > im) {
    t = im/re;
    return re*sqrt(ONE+t*t);
  } else {
    if (im == ZERO) {
      return ZERO;
    }
    t = re/im;
    return im*sqrt(ONE+t*t);
  }

}
// Compute 1-norm of a matrix directly
template <int ROWS, int COLS, typename T> double norm1(T in[ROWS][COLS]) {

  double norm = 0;
  double norm_cols[COLS];

  // Initialise
  for (int c = 0; c < COLS; c++) {
    norm_cols[c] = 0;
  }

  // Sum column absolute values
  for (int r=0; r < ROWS; r++) {
    for (int c=0; c < COLS; c++) {
      norm_cols[c] += norm1_abs(in[r][c]);
    }
  }

  // Find largest column sum
  for (int c=0; c < COLS; c++) {
    if (norm_cols[c] > norm) {
      norm = norm_cols[c];
    }
  }

  return norm;

}

template<int ROWS, int COLS, class T_IN, class T_OUT>
int msub(
  const T_IN A[ROWS][COLS],
  const T_IN B[ROWS][COLS],
  T_OUT C [ROWS][COLS] )
{
  T_IN tmp;
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      tmp = A[row][col];
      C[row][col] = tmp - B[row][col];
    }
  }
  return 0;
};

// Helper struct to determine base type
template <typename T> struct tb_traits {
  typedef T BASE_T;
};
template <typename T> struct tb_traits<std::complex<T> > {
  typedef T BASE_T;
};

// Generate a difference ratio between a matrix and a reference matrix
// - LAPACK error measurement method
// - Scaled by matrix size and data type precision.
template <int ROWS, int COLS, typename T> double difference_ratio(T in[ROWS][COLS], T ref[ROWS][COLS]) {
  const int MAX_DIM = (ROWS > COLS ? ROWS : COLS);
  typedef typename tb_traits<T>::BASE_T IN_BASE_T;
  const IN_BASE_T eps_base_t = std::numeric_limits<IN_BASE_T>::epsilon();
  //const IN_BASE_T eps_base_t = hls::numeric_limits<IN_BASE_T>::epsilon();
  //const T eps_base_t = std::numeric_limits<T>::epsilon();
  //std::cout << "epsilon " << eps_base_t << std:: endl;
  const double eps  = (double)eps_base_t;

  T diff[ROWS][COLS];
  double diff_norm, ref_norm;

  // Calculate difference between matrix and reference
  msub<ROWS,COLS>(in,ref,diff);
  // Calculate norm-1s of the difference and the reference
  diff_norm = norm1<ROWS,COLS>(diff);
  ref_norm  = norm1<ROWS,COLS>(ref);
  // Return difference ratio
  return (diff_norm / (ref_norm * MAX_DIM * eps));
};

#ifdef HLS_COMPLEX
template <typename T> struct tb_traits<hls::x_complex<T> > {
  typedef T BASE_T;
};
#endif
// ===================================================================================================================
// Transpose Types & Operation
struct NoTranspose {
  const static int TransposeType = 0;
  template <
    int RowsA,
    int ColsA,
    typename InputType>
  static InputType GetElement(const InputType A [RowsA][ColsA], unsigned  Row, unsigned  Col)
  {
    // No transpose, default
    return A[Row][Col];
  }
};

struct Transpose {
  const static int TransposeType = 1;
  template <
    int RowsA,
    int ColsA,
    typename InputType>
  static InputType GetElement(const InputType A [RowsA][ColsA], unsigned  Row, unsigned  Col)
  {
    // Transpose, no conjugate
    return A[Col][Row];
  }
};

struct ConjugateTranspose {
  const static int TransposeType = 2;
  template <
    int RowsA,
    int ColsA,
    typename InputType>
  static InputType GetElement(const InputType A [RowsA][ColsA], unsigned  Row, unsigned  Col)
  {
    // Complex conjugate transpose.
    // o For non std::complex types this function will return an unaltered value.
    return x_conj(A[Col][Row]);
  }
};

template <
  class TransposeForm,
  int RowsA,
  int ColsA,
  typename InputType>
InputType GetMatrixElement(const InputType A [RowsA][ColsA], unsigned  Row, unsigned  Col)
{
  // Need to help the compiler identify that the GetElement member function is a template
  return TransposeForm::template GetElement<RowsA, ColsA, InputType>(A,Row,Col);
}

// Matrix Display
// ============================================================================

// Non-specific data
// -----------------
// This can be used by PoD or any class supporting "<<"
// setprecision may fail on non floating point types
template <typename T> struct xil_printer {
  static std::string to_s(T x, unsigned prec=10){

    std::stringstream ss;
    ss << std::setiosflags(std::ios::fixed) << std::setprecision(prec) << x;

    return ss.str();
  }
};

// Complex data
// ----------------
// This is used by complex data of any type.  A printer is called to print the real and imaginary
// parts, so this just handles the formating of x+jy
template <typename T> struct xil_printer<std::complex <T> > {
  static std::string to_s(std::complex<T> x, unsigned prec=10){

    // Use the basic type printer to print the real and imaginary parts
    typedef xil_printer<T> printer;

    std::stringstream ss;
    // Remember to deal with -0
    bool neg_imag = x.imag() <= -0 ? true : false;

    // Casting to "T" avoids "error: operands to ?: have different types." when using ap_fixed.
    T imag        = neg_imag ? (T)-x.imag() : (T)x.imag();

    ss << printer::to_s(x.real(), prec) << (neg_imag ? " - j*":" + j*") << printer::to_s(imag, prec);
    return ss.str();
  }
};

template <unsigned ROWS, unsigned COLS, typename T, class TransposeForm>
void print_matrix(T a[ROWS][COLS], std::string prefix = "", unsigned prec=10, unsigned matlab_format=0)
{
  typedef xil_printer<T> printer;

  std::string res[ROWS][COLS];
  unsigned widest_entry = 0;

  // Get the individual fields
  for(int r = 0; r < ROWS; r++){
    for(int c = 0; c < COLS; c++){
      T tmp;

      //tmp = hls::GetMatrixElement<TransposeForm,ROWS,COLS,T>(a, r, c);
      tmp = GetMatrixElement<TransposeForm,ROWS,COLS,T>(a, r, c);

      res[r][c] = printer::to_s(tmp, prec);
      if(res[r][c].length() > widest_entry){
        widest_entry = res[r][c].length();
      }
    }
  }

  // Print fields.  Each column should be "widest_entry" chars wide
  char col_gap_str[5] = "    ";
  unsigned col_width = widest_entry;

  for(int r = 0; r < ROWS; r++){
    if ( !matlab_format ) {
      printf("%s|", prefix.c_str());
    } else {
      if (r==0) {
        printf("%s[", prefix.c_str());
      } else {
        printf("%s ", prefix.c_str());
      }
    }
    for(int c = 0; c < COLS; c++){

      unsigned num_spaces_needed = col_width - res[r][c].length();
      for(int x = 0; x< num_spaces_needed; x++){
        printf(" ");
      }
      if ( !matlab_format ) {
        printf( "(%s)", res[r][c].c_str());
      } else {
        printf( "%s", res[r][c].c_str());
      }

      if(c!=COLS-1){
        printf("%s", col_gap_str);
        if ( matlab_format ) {
          printf(",");
        }
      }
    }
    if ( !matlab_format ) {
      printf(" |\n");
    } else {
      if (r==ROWS-1) {
        printf(" ];\n");
      } else {
        printf(" ;\n");
      }
    }
  }
}

template<int rowsA,int colsA, int colsB, typename T>
void MatMult(T A[rowsA][colsA],
             T B[colsA][colsB],
             T C[rowsA][colsB])
{
   T tmp[rowsA][colsB];
   for(int j=0;j<colsB;j++)
   {
      for(int k=0;k<colsA;k++)
      {
         tmp[j][k]= (0.0,0.0);
      }
   }
   for(int i=0;i<rowsA;i++)
   {
      for(int j=0;j<colsB;j++)
      {
         for(int k=0;k<colsA;k++)
         {
            tmp[i][j]+=A[i][k]*B[k][j];
            C[i][j]=tmp[i][j];
         }
      }
   }
}

#endif
