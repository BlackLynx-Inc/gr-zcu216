//#include "hls_linear_algebra.h"
#include "hls_x_complex.h"
#include "ap_int.h"
#include <cstddef>

const unsigned ROWS_COLS_A = 10;
const unsigned INVERSE_BATCH_SIZE = 4;
const unsigned INVERSE_PARALLELIZATION_FACTOR = 1; // This has two be 1 for a 10x10 matrix or
const unsigned MGS_DATA_VECTORIZATION_FACTOR = 1;
const unsigned NUM_KEEP = MGS_DATA_VECTORIZATION_FACTOR*8;

typedef hls::x_complex<float> MATRIX_IN_T;
typedef hls::x_complex<float> MATRIX_OUT_T;

template<size_t vectorization_factor>
struct COMPLEX_FP_WIDE_T {
  hls::x_complex<float> data[vectorization_factor];
};

/*
typedef struct {
     hls::x_complex<float> data[MGS_DATA_VECTORIZATION_FACTOR];
  } data_t;


typedef struct {
  data_t data;
  ap_uint<8> keep;
  ap_uint<1> last;
} COMPLEX_MGS_FP_WIDE_T;
*/