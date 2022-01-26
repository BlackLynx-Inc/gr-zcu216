

#ifndef __PCCC_EXAMPLE_H__
#define __PCCC_EXAMPLE_H__

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

void pccc_decode_example(char* output_bits, float* input_LLRs);

typedef struct
{
    unsigned int numInputSymbols;
    unsigned int numOutputSymbols;
    unsigned int numStates;
    unsigned int denominator;  // only stored if feedback present
    vector<vector<unsigned int>> nextStates;
    vector<vector<unsigned int>> codeOutputs;
    vector<vector<float>> eqOutputs;
} Trellis;

Trellis singleConstraintPolyString2trellis(const string& s);
int parsePuncturePattern(string puncture, unsigned int p0, unsigned int u0,
                         unsigned int infoBlkSize, vector<vector<int>>& mymat);

float* allocReal4Vector(int n);
void freeVector(void* pVector);
void vsfillo_r4(float* vec1, float scal, int n);

vector<char> reformat_to_byte(const vector<float>& vec, bool invert = false);

vector<unsigned int> dec2baseM(unsigned int n, unsigned int M, unsigned int L);
vector<vector<unsigned int>> dec2baseM(vector<unsigned int> v, unsigned int M,
                                       unsigned int L);

class LogMap
{
public:
    // Constructors
    LogMap();

    LogMap(const Trellis& trellis, const unsigned int stages, short maxMode = 0)
    {
        init(trellis, stages, maxMode);
    }

    // Destructor
    ~LogMap();

    /*
     * Decoder Functions
     */
    // Decoder function for a zero terminated code:
    void decode_zeroTerm(const float* ptrYs, const float* ptrYp,
                         const float* ptrLa, const float sigmaSquared,
                         float* ptrLu)
    {
        (this->*ptr2ZTdec)(ptrYs, ptrYp, ptrLa, sigmaSquared, ptrLu);
    }

    /*
     * Setup function
     */
    // setup (for using with the empty constructor)
    void setup(const Trellis& trellis, const unsigned int stages,
               short maxMode = 0)
    {
        init(trellis, stages, maxMode);
    }

    /*
     * Get Functions:
     */
    // get the final state
    int get_final_stateA() { return finalStateAlpha; }
    int get_final_stateB() { return finalStateBeta; }

    /*
     * Set Functions:
     */
    void set_verbose(const bool verbose) { this->verbose = verbose; }

private:
    bool verbose;

    unsigned int numStates;      // Number of states in the Trellis
    unsigned int bitsPerSymbol;  // Number of bits/symbol
    unsigned int numParity;      // Number of parity bits

    unsigned int* nextStates1;  // Next states for a 0 input
    unsigned int* nextStates2;  // Next states for a 1 input

    unsigned int* outputs1;  // Outputs for a 0 input
    unsigned int* outputs2;  // Outputs for a 1 input

    int* symbol1;  // Output code symbols {-1,1} for a 0 input
    int* symbol2;  // Output code symbols {-1,1} for a 1 input

    unsigned int* nextStates;  // Next states
    unsigned int* prevStates;  // Previous states for traceback

    unsigned int prevRows;
    unsigned int nextRows;
    unsigned int symbolRows;
    unsigned int symbolElems;
    unsigned int stateElems;

    int finalStateAlpha;  // state the alpha decoder ends in.
    int finalStateBeta;   // state the beta decoder ends in

    // array for traversing the trellis
    float* ptrAk;
    float* ptrBk;
    float* ptrG;

    float* m1;
    float* m2;
    float* m3;
    float* m7;

    // For SSE 15/13 decoder
    float* ptrAkS[8];
    float* ptrBkS[8];
    float* m1s;
    float* m3s;
    float* minval;
    float* ptrLuS;

    int stride;
    unsigned int stages;

    void init(const Trellis& trellis, const unsigned int stages, short maxMode);

    float (LogMap::*ptr2Max)(const float&, const float&);

    void (LogMap::*ptr2ZTdec)(const float* ptrYs, const float* ptrYp,
                              const float* ptrLa, const float sigmaSquared,
                              float* ptrLu);

    // for max star table lookup
    float* maxstarTable;
    float maxstarTableMfact;
    int maxstarTableMaxIndex;
    float maxstarTableMaxVal;
    // Table generator for myMaxstarTL
    void genMaxstarTable(int npts, float res)
    {
        // Since abs(), range of inputs should go from 0 to res*(npts-1)
        maxstarTableMfact = 1.0 / res;
        maxstarTableMaxIndex = npts - 1;
        maxstarTableMaxVal = res * (npts - 1);

        maxstarTable = allocReal4Vector(npts);
        for (int i = 0; i < npts; i++)
            maxstarTable[i] = log(1.0 + exp(-1 * (i * res)));
    }

    // Max function
    inline float myMaxstar(const float& x, const float& y)
    {
        float temp = x - y;
        float out = (float)std::max(x, y) + (float)log(1.0 + exp(-fabs(temp)));
        return out;
    }

    // Max function via table lookup
    inline float myMaxstarTL(const float& x, const float& y)
    {
        float fabsval =
            min(maxstarTableMaxVal, max((float)0, (float)fabs(x - y)));
        // float fabsval = clamp((float)fabs(x-y));

        // int tIndex = fabs(x-y) * maxstarTableMfact; // pointer into table
        // tIndex
        // printf("tIndex = %d \n",tIndex);
        return std::max(x, y) +
               maxstarTable[(int)(fabsval * maxstarTableMfact)];
    }

    // Approximation to max function (faster)
    inline float myMax(const float& x, const float& y)
    {
        return std::max(x, y);
    }

    // Generic ZT decoder
    void decode_zt_generic(const float* ptrYs, const float* ptrYp,
                           const float* ptrLa, const float sigmaSquared,
                           float* ptrLu);
};

class ConvEncode
{
public:
    // Constructors
    ConvEncode() {}

    ConvEncode(const Trellis& trellis) { init(trellis); }

    void setup(const Trellis& trellis) { init(trellis); }

    vector<char> encode(const vector<char>& msg, int init_state = 0);
    vector<char> get_parity_only(const vector<char>& msg, int init_state = 0);

    int get_state() { return FinalState; }
    int get_numStates() { return NumberStates; }

private:
    int k;
    int n;
    float R;
    int NumberStates;
    int NumberSymbols;
    int FinalState;

    vector<vector<unsigned int>> StateMap;
    vector<vector<unsigned int>> EncodeMap;

    void init(const Trellis& trellis);
};

template<class T>
vector<T> permute(const vector<T>& vec, const vector<unsigned int>& perm);

void quickpermute(float* out, const float* in, const unsigned int* perm,
                  const unsigned int sz);

class Turbo
{
public:
    // Constructors
    Turbo() {}

    Turbo(const Trellis& t1, const Trellis& t2,
          const vector<unsigned int>& interleaver, const unsigned int K,
          const short zeroTerminate, short maxMode = 0)
    {
        init(t1, t2, interleaver, K, zeroTerminate, maxMode);
    }

    // Destructors
    ~Turbo();

    /* Decoder:
     */

    void decode_zeroTerm(const vector<float>& u, const vector<float>& vp,
                         const vector<float>& vq, const vector<float>& tail1,
                         const vector<float>& tail2, vector<float>& chat);

    void decode_zeroTerm_sse(
        const vector<float>& u_1, const vector<float>& u_2,
        const vector<float>& u_3, const vector<float>& u_4,
        const vector<float>& vp_1, const vector<float>& vp_2,
        const vector<float>& vp_3, const vector<float>& vp_4,
        const vector<float>& vq_1, const vector<float>& vq_2,
        const vector<float>& vq_3, const vector<float>& vq_4,
        const vector<float>& tail1_1, const vector<float>& tail1_2,
        const vector<float>& tail1_3, const vector<float>& tail1_4,
        const vector<float>& tail2_1, const vector<float>& tail2_2,
        const vector<float>& tail2_3, const vector<float>& tail2_4,
        vector<float>& chat_1, vector<float>& chat_2, vector<float>& chat_3,
        vector<float>& chat_4);

    void decode_circularTerm(const vector<float>& u, const vector<float>& vp,
                             const vector<float>& vq, vector<float>& chat);

    // Encoders
    void encode_zeroTerm(const vector<char>& u, vector<char>& vp,
                         vector<char>& vq, vector<char>& tail1,
                         vector<char>& tail2);

    // return false if the code doesn't exist.
    bool encode_circularTerm(const vector<char>& u, vector<char>& vp,
                             vector<char>& vq);

    // Setup
    void setup(const Trellis& t1, const Trellis& t2,
               const vector<unsigned int>& interleaver, const unsigned int K,
               const short zeroTerminate, short maxMode = 0)
    {
        init(t1, t2, interleaver, K, zeroTerminate, maxMode);
    }

    // Set functions
    void set_maxIterations(const unsigned int maxItr);
    void set_variance(const float sigmaSquared)
    {
        this->sigmaSquared = sigmaSquared;
    }
    void set_earlyTerm(const bool earlyTerm);
    void set_verbose(const bool verbose)
    {
        this->verbose = verbose;
        D1.set_verbose(verbose);
        D2.set_verbose(verbose);
    }

    // Get functions
    unsigned int get_numIterations() { return numIterations; }

private:
    int debug;

    // objects, containers and variables:

    LogMap D1;
    LogMap D2;
    ConvEncode E1;
    ConvEncode E2;

    unsigned int memory;
    unsigned int maxIterations;
    unsigned int
        numIterations;  // number of iterations performed during the last decode
    unsigned int maxRepeats;
    bool verbose;
    bool earlyTermFlag;

    vector<unsigned int> interleaver;
    vector<unsigned int> deinterleaver;

    unsigned int denominator1;
    unsigned int denominator2;

    float sigmaSquared;

    // vectors used internally:
    float* Le12;
    float* Le21;
    float* Lu;
    float* Le;
    float* La;

    // sse vectors
    float* myu_all;
    float* myw_all;
    float* vp_all;
    float* vq_all;
    float* La_all;
    float* Lu_all;
    float* Le12_all;
    float* Le21_all;
    int sse_stride;

    vector<char> cwlast;
    vector<char> cwcurrent;

    unsigned int tc_K;       // info length
    unsigned int lm_stages;  // number of stages in the logmap decoder

    short zeroTerminate;

    // functions:

    void init(const Trellis& t1, const Trellis& t2,
              const vector<unsigned int>& interleaver, const unsigned int K,
              const short zeroTerminate, const short maxMode);
};

#endif
