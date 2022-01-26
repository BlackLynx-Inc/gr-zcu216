#include <cmath>
#include <complex>

template<int ROWS_COLS_A>
float magnitudeColumn(std::complex<float> matrixData[ROWS_COLS_A][ROWS_COLS_A],
                      int targetColumn)
{
    float sum = 0;
    for (int row = 0; row < ROWS_COLS_A; row++)
    {
        std::complex<float> val = matrixData[row][targetColumn];
        float val_real = val.real();
        float val_imag = val.imag();
        sum += (val_real * val_real) + (val_imag * val_imag);
    }
    return sqrtf(sum);
}

template<int ROWS_COLS_A>
void invert_R(std::complex<float> R[ROWS_COLS_A][ROWS_COLS_A],
              std::complex<float> InverseR[ROWS_COLS_A][ROWS_COLS_A])
{
    std::complex<float> R_reciprocal_diag[ROWS_COLS_A];

    for (int m = 0; m < ROWS_COLS_A; m++)
    {
        std::complex<float> one = 1.0f;
        R_reciprocal_diag[m] = one / R[m][m];
    }

    for (int m = 0; m < ROWS_COLS_A; m++)
    {
        for (int n = 0; n < ROWS_COLS_A; n++)
        {
            std::complex<float> S;
            if (n >= m)
            {
                S = (m == n) ? 1.0f : 0.0f;

                for (int p = 0; p < ROWS_COLS_A; p++)
                {
                    if ((p >= m) && (p < n))
                        S -= InverseR[m][p] * R[p][n];
                }
                InverseR[m][n] = S * R_reciprocal_diag[n];
            }
        }
    }
}

template<int ROWS_COLS_A>
void matrix_multiply_RinvQt(
    std::complex<float> InverseR[ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> Q[ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> InverseA[ROWS_COLS_A][ROWS_COLS_A])
{
    for (int c = 0; c < ROWS_COLS_A; c++)
    {
        for (int d = 0; d < ROWS_COLS_A; d++)
        {
            std::complex<float> sum = 0.0f;
            for (int k = 0; k < ROWS_COLS_A; k++)
            {
                if (k >= d)
                    sum += InverseR[d][k] * std::conj(Q[c][k]);
            }
            InverseA[d][c] = sum;
        }
    }
}

template<int NUM_ITERATIONS, int ROWS_COLS_A>
void QR_to_InverseA_vectorized(
    std::complex<float> Q_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> R_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> InverseA_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A])
{
    for (int x = 0; x < NUM_ITERATIONS; x++)
    {
        std::complex<float> InverseR[ROWS_COLS_A][ROWS_COLS_A];

        // Invert R matrix (upper triangular)
        invert_R<ROWS_COLS_A>(R_vec[x], InverseR);

        // Compute Ainv = Rinv * Qtranspose
        matrix_multiply_RinvQt<ROWS_COLS_A>(InverseR, Q_vec[x],
                                            InverseA_vec[x]);
    }
}

template<int ROWS_COLS_A>
void mgs_qrd_complex(std::complex<float> a_matrix[ROWS_COLS_A][ROWS_COLS_A],
                     std::complex<float> q_matrix[ROWS_COLS_A][ROWS_COLS_A],
                     std::complex<float> r_matrix[ROWS_COLS_A][ROWS_COLS_A])
{
    /* Modified Gram Schmidt QR Decomposition Algorithm
       QRD(input matrixA, output Q, output R)
    1. copy matrixA into T

       for (i= 1:n)                 // main loop (over columns)
    2.   l2_norm = ||Ti||           // l2-norm(magnitude) of ith column vector
    3.   Rii = l2_norm              // Assign alpha to R diagonal element
    4.   Qi = Ti / l2_norm          // Compute the ith Q column: divide ith T
    column vector by alpha for (j=i+1:n)              // inner loop
    5.     dotProduct = Qi.Tj       // Compute the dot product between the ith
    column of Q and the jth column of T
    6.     Rij= dotProduct          // Assign the the ith row of R off diagonal
    elements the computed dot product
    7.     Tj= Ti - dotProduct x Qi // Update the jth column of T: Current jth
    column of T minus the dot product times the ith column of Q end end end
    //QRD
    */

    std::complex<float> t_matrix[ROWS_COLS_A][ROWS_COLS_A];
    std::complex<float> q_matrix_temp[ROWS_COLS_A][ROWS_COLS_A];

    for (int r = 0; r < ROWS_COLS_A; r++)
    {
        for (int c = 0; c < ROWS_COLS_A; c++)
        {
            t_matrix[r][c] = a_matrix[r][c];
        }
    }

    for (int i = 0; i < ROWS_COLS_A; i++)
    {
        // line 2, 3 of algorithm: calculated alpha (t_magnitude)
        float t_magnitude = magnitudeColumn<ROWS_COLS_A>(t_matrix, i);
        float t_magnitude_reciprocal = 1.0f / t_magnitude;
        r_matrix[i][i].real(t_magnitude);
        r_matrix[i][i].imag(0.0f);

        // line 4 of algorithm
        // generate the normalized ith column of the q_matrix matrix
        for (int mRow = 0; mRow < ROWS_COLS_A; mRow++)
        {
            std::complex<float> temp = t_matrix[mRow][i] * t_magnitude_reciprocal;
            q_matrix_temp[mRow][i] = temp;
            q_matrix[mRow][i] = temp;
        }

        for (int j = 0; j < ROWS_COLS_A; j++)
        {
            if (j > i)
            {
                // line 5, 6 of algorithm
                std::complex<float> dotProduct = 0.0f;
                for (int mRow = 0; mRow < ROWS_COLS_A; mRow++)
                {
                    dotProduct +=
                        std::conj(q_matrix_temp[mRow][i]) * t_matrix[mRow][j];
                }
                r_matrix[i][j] = dotProduct;
                r_matrix[j][i] = 0.0f;

                // line 7 of algorithm
                // generate the next column of the T-matrix.
                for (int mRow = 0; mRow < ROWS_COLS_A; mRow++)
                {
                    std::complex<float> tj = t_matrix[mRow][j];
                    std::complex<float> qi = q_matrix_temp[mRow][i];
                    t_matrix[mRow][j] = tj - (dotProduct * qi);
                }
            }
        }
    }
}

template<int NUM_ITERATIONS, int ROWS_COLS_A>
void mgs_qrd_complex_vectorized(
    std::complex<float> a_matrix_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> q_matrix_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> r_matrix_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A])
{
    for (int x = 0; x < NUM_ITERATIONS; x++)
    {
        mgs_qrd_complex(a_matrix_vec[x], q_matrix_vec[x], r_matrix_vec[x]);
    }
}

template<int NUM_ITERATIONS, int ROWS_COLS_A>
void mgs_qr_inverse_complex_stream_vectorized(
    std::complex<float> A_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> InverseA_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A])
{
    std::complex<float> Q_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A];
    std::complex<float> R_vec[NUM_ITERATIONS][ROWS_COLS_A][ROWS_COLS_A];

    // Call wrapped QR Decomposition - Q is being output such that Q*R = A
    // Gram-Schmidt Reduced Decomposition
    mgs_qrd_complex_vectorized<NUM_ITERATIONS, ROWS_COLS_A>(A_vec, Q_vec,
                                                            R_vec);

    // Convert final A^-1 from Q and R.
    QR_to_InverseA_vectorized<NUM_ITERATIONS, ROWS_COLS_A>(Q_vec, R_vec,
                                                           InverseA_vec);
}

// The top-level function to synthesize
//
template<int INVERSE_PARALLELIZATION_FACTOR, int INVERSE_BATCH_SIZE,
         int ROWS_COLS_A>
void mat_inverse(
    const std::complex<float> A_vec[INVERSE_BATCH_SIZE][ROWS_COLS_A][ROWS_COLS_A],
    std::complex<float> InverseA_vec[INVERSE_BATCH_SIZE][ROWS_COLS_A]
                                    [ROWS_COLS_A])
{
    const int NUM_ITERATIONS =
        (INVERSE_BATCH_SIZE + INVERSE_PARALLELIZATION_FACTOR - 1) /
        INVERSE_PARALLELIZATION_FACTOR;

    std::complex<float> a_vec_i[INVERSE_PARALLELIZATION_FACTOR][NUM_ITERATIONS]
                               [ROWS_COLS_A][ROWS_COLS_A];
    std::complex<float> inverse_a_vec_i[INVERSE_PARALLELIZATION_FACTOR]
                                       [NUM_ITERATIONS][ROWS_COLS_A]
                                       [ROWS_COLS_A];

    // Copy input data to local memory
    for (int x = 0; x < INVERSE_BATCH_SIZE; x++)
    {
        for (int r = 0; r < ROWS_COLS_A; r++)
        {
            for (int c = 0; c < ROWS_COLS_A; c++)
            {
                a_vec_i[x / NUM_ITERATIONS][x % NUM_ITERATIONS][r][c] =
                    A_vec[x][r][c];
            }
        }
    }

    for (int x = 0; x < INVERSE_PARALLELIZATION_FACTOR; x++)
    {
        mgs_qr_inverse_complex_stream_vectorized<NUM_ITERATIONS, ROWS_COLS_A>(
            a_vec_i[x], inverse_a_vec_i[x]);
    }

    // Copy local memory contents to output
    for (int x = 0; x < INVERSE_BATCH_SIZE; x++)
    {
        for (int r = 0; r < ROWS_COLS_A; r++)
        {
            for (int c = 0; c < ROWS_COLS_A; c++)
            {
                InverseA_vec[x][r][c] =
                    inverse_a_vec_i[x / NUM_ITERATIONS][x % NUM_ITERATIONS][r]
                                   [c];
            }
        }
    }
}
