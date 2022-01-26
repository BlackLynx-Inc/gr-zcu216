

#include "pccc_example.h"

#include <limits>

using namespace std;

int _turbo_interleaver[] = {
    27,  515, 257, 141, 657, 407, 73,  579, 335, 205, 705, 461, 53,  563, 289,
    189, 689, 441, 105, 621, 375, 237, 737, 493, 22,  518, 258, 154, 642, 398,
    82,  582, 350, 218, 706, 474, 42,  550, 290, 186, 674, 434, 114, 634, 366,
    250, 738, 506, 17,  521, 259, 135, 659, 389, 91,  585, 333, 199, 707, 455,
    63,  569, 291, 183, 691, 427, 123, 615, 357, 231, 739, 487, 12,  524, 260,
    148, 644, 412, 68,  588, 348, 212, 708, 468, 52,  556, 292, 180, 676, 420,
    100, 628, 380, 244, 740, 500, 7,   527, 261, 129, 661, 403, 77,  591, 331,
    193, 709, 449, 41,  575, 293, 177, 693, 445, 109, 609, 371, 225, 741, 481,
    2,   530, 262, 142, 646, 394, 86,  594, 346, 206, 710, 462, 62,  562, 294,
    174, 678, 438, 118, 622, 362, 238, 742, 494, 29,  533, 263, 155, 663, 385,
    95,  597, 329, 219, 711, 475, 51,  549, 295, 171, 695, 431, 127, 635, 353,
    251, 743, 507, 24,  536, 264, 136, 648, 408, 72,  600, 344, 200, 712, 456,
    40,  568, 296, 168, 680, 424, 104, 616, 376, 232, 744, 488, 19,  539, 265,
    149, 665, 399, 81,  603, 327, 213, 713, 469, 61,  555, 297, 165, 697, 417,
    113, 629, 367, 245, 745, 501, 14,  542, 266, 130, 650, 390, 90,  606, 342,
    194, 714, 450, 50,  574, 298, 162, 682, 442, 122, 610, 358, 226, 746, 482,
    9,   513, 267, 143, 667, 413, 67,  577, 325, 207, 715, 463, 39,  561, 299,
    191, 699, 435, 99,  623, 381, 239, 747, 495, 4,   516, 268, 156, 652, 404,
    76,  580, 340, 220, 716, 476, 60,  548, 300, 188, 684, 428, 108, 636, 372,
    252, 748, 508, 31,  519, 269, 137, 669, 395, 85,  583, 323, 201, 717, 457,
    49,  567, 301, 185, 701, 421, 117, 617, 363, 233, 749, 489, 26,  522, 270,
    150, 654, 386, 94,  586, 338, 214, 718, 470, 38,  554, 302, 182, 686, 446,
    126, 630, 354, 246, 750, 502, 21,  525, 271, 131, 671, 409, 71,  589, 321,
    195, 719, 451, 59,  573, 303, 179, 703, 439, 103, 611, 377, 227, 751, 483,
    16,  528, 272, 144, 656, 400, 80,  592, 336, 208, 720, 464, 48,  560, 304,
    176, 688, 432, 112, 624, 368, 240, 752, 496, 11,  531, 273, 157, 641, 391,
    89,  595, 351, 221, 721, 477, 37,  547, 305, 173, 673, 425, 121, 637, 359,
    253, 753, 509, 6,   534, 274, 138, 658, 414, 66,  598, 334, 202, 722, 458,
    58,  566, 306, 170, 690, 418, 98,  618, 382, 234, 754, 490, 1,   537, 275,
    151, 643, 405, 75,  601, 349, 215, 723, 471, 47,  553, 307, 167, 675, 443,
    107, 631, 373, 247, 755, 503, 28,  540, 276, 132, 660, 396, 84,  604, 332,
    196, 724, 452, 36,  572, 308, 164, 692, 436, 116, 612, 364, 228, 756, 484,
    23,  543, 277, 145, 645, 387, 93,  607, 347, 209, 725, 465, 57,  559, 309,
    161, 677, 429, 125, 625, 355, 241, 757, 497, 18,  514, 278, 158, 662, 410,
    70,  578, 330, 222, 726, 478, 46,  546, 310, 190, 694, 422, 102, 638, 378,
    254, 758, 510, 13,  517, 279, 139, 647, 401, 79,  581, 345, 203, 727, 459,
    35,  565, 311, 187, 679, 447, 111, 619, 369, 235, 759, 491, 8,   520, 280,
    152, 664, 392, 88,  584, 328, 216, 728, 472, 56,  552, 312, 184, 696, 440,
    120, 632, 360, 248, 760, 504, 3,   523, 281, 133, 649, 415, 65,  587, 343,
    197, 729, 453, 45,  571, 313, 181, 681, 433, 97,  613, 383, 229, 761, 485,
    30,  526, 282, 146, 666, 406, 74,  590, 326, 210, 730, 466, 34,  558, 314,
    178, 698, 426, 106, 626, 374, 242, 498, 25,  529, 283, 159, 651, 397, 83,
    593, 341, 223, 731, 479, 55,  545, 315, 175, 683, 419, 115, 639, 365, 255,
    511, 20,  532, 284, 140, 668, 388, 92,  596, 324, 204, 732, 460, 44,  564,
    316, 172, 700, 444, 124, 620, 356, 236, 492, 15,  535, 285, 153, 653, 411,
    69,  599, 339, 217, 733, 473, 33,  551, 317, 169, 685, 437, 101, 633, 379,
    249, 505, 10,  538, 286, 134, 670, 402, 78,  602, 322, 198, 734, 454, 54,
    570, 318, 166, 702, 430, 110, 614, 370, 230, 486, 5,   541, 287, 147, 655,
    393, 87,  605, 337, 211, 735, 467, 43,  557, 319, 163, 687, 423, 119, 627,
    361, 243, 499, 0,   512, 256, 128, 640, 384, 64,  576, 320, 192, 704, 448,
    32,  544, 288, 160, 672, 416, 96,  608, 352, 224, 736, 480};
#define NturboInterleaver (sizeof(_turbo_interleaver) / sizeof(int))

void pccc_decode_example(char* output_bits, float* input_LLRs)
{
    Trellis trellis1, trellis2;
    string s1 = "4:1:13:15";
    string puncture = "3:2:1,1,1,0,0,1";
    int infoBlkSize = 762;
    int maxItr = 8;

    vector<unsigned int> turbo_interleaver(infoBlkSize);
    for (unsigned int n = 0; n < NturboInterleaver; n++)
    {
        turbo_interleaver[n] = _turbo_interleaver[n];
    }

    trellis1 = singleConstraintPolyString2trellis(s1);
    trellis2 = trellis1;

    int memory = (int)(log2(trellis1.numStates));
    int p0 = log2(trellis1.numOutputSymbols) - log2(trellis1.numInputSymbols);
    int u0 = log2(trellis1.numInputSymbols);

    vector<vector<int>> punctMat;

    int depunctCodeBlkSize = infoBlkSize * (u0 + 2 * p0);
    depunctCodeBlkSize += 2 * memory * (u0 + p0);  // tail bits

    int punctSize = infoBlkSize * (u0 + 2 * p0);

    parsePuncturePattern(puncture, p0, u0, infoBlkSize, punctMat);
    punctSize = 0;
    for (unsigned int ii = 0; ii < punctMat.size(); ii++)
    {
        for (unsigned int jj = 0; jj < punctMat[ii].size(); jj++)
        {
            if (punctMat[ii][jj] == 1)
            {
                punctSize++;
            }
        }
    }

    punctSize += 2 * memory * (u0 + p0);  // include tail bits

    int parityLen = (infoBlkSize + memory) * p0;

    short maxMode = 0;
    short zeroTerm = 1;

    Turbo turbo(trellis1, trellis2, turbo_interleaver, infoBlkSize, zeroTerm,
                maxMode);

    turbo.set_maxIterations(maxItr);

    vector<float> depunct(depunctCodeBlkSize);

    vector<float> u_1, u_2, u_3, u_4;
    vector<float> vp_1, vp_2, vp_3, vp_4;
    vector<float> vq_1, vq_2, vq_3, vq_4;
    vector<float> tail1_1, tail1_2, tail1_3, tail1_4;
    vector<float> tail2_1, tail2_2, tail2_3, tail2_4;
    vector<float> chat_1, chat_2, chat_3, chat_4;

    vector<float> chat(infoBlkSize);
    vector<float> msg;
    vector<float> p(parityLen);
    vector<float> q(parityLen);
    vector<float> u_t1(memory);
    vector<float> u_t2(memory);
    vector<float> p_t(memory * p0);
    vector<float> q_t(memory * p0);
    vector<float>::iterator it;

    // Depuncture the data
    memset(&depunct[0], 0, sizeof(float) * depunctCodeBlkSize);
    int index = 0;
    int punctIndex = 0;
    for (unsigned int ii = 0; ii < punctMat.size(); ii++)
    {
        for (unsigned int jj = 0; jj < punctMat[ii].size(); jj++)
        {
            if (punctMat[ii][jj] == 1)
            {
                depunct[index++] = input_LLRs[punctIndex++];
            }
            else
            {
                depunct[index++] = 0;  // this bit wasn't sent.
            }
        }
    }

    // Now add all tail bits to the end:
    int tailIndex = index;
    for (unsigned int ii = tailIndex; ii < depunct.size(); ii++)
    {
        depunct[index++] = input_LLRs[punctIndex++];
    }

    it = depunct.begin();
    msg.assign(it, it + infoBlkSize);
    it += infoBlkSize;

    // reorder the parity to decoder parity symbols
    int paritySegments = msg.size();
    int pindex = msg.size();
    int qindex = msg.size() + msg.size() * p0;
    for (int ii = 0; ii < paritySegments; ii++)
    {
        for (int jj = 0; jj < p0; jj++)
        {
            p[ii * p0 + jj] = depunct[pindex + jj * paritySegments + ii];
            q[ii * p0 + jj] = depunct[qindex + jj * paritySegments + ii];
        }
    }
    it += 2 * msg.size() * p0;

    int tailParitySegments = memory;
    u_t1.assign(it, it + memory);
    it += memory;
    p_t.assign(it, it + memory * p0);
    it += memory * p0;
    u_t2.assign(it, it + memory);
    it += memory;
    q_t.assign(it, it + memory * p0);
    it += memory * p0;

    for (int ii = 0; ii < tailParitySegments; ii++)
    {
        for (int jj = 0; jj < p0; jj++)
        {
            p[paritySegments * p0 + ii * p0 + jj] =
                p_t[jj * tailParitySegments + ii];
            q[paritySegments * p0 + ii * p0 + jj] =
                q_t[jj * tailParitySegments + ii];
        }
    }

    for (unsigned int n = 0; n < chat.size(); n++)
    {
        chat[n] = 0;
    }

    turbo.decode_zeroTerm(msg, p, q, u_t1, u_t2, chat);
    vector<char> hard = reformat_to_byte(chat, false);

    for (unsigned int n = 0; n < hard.size(); n++)
    {
        output_bits[n] = hard[n];
    }
}

//////////////////////////////////////////////////////////////////////////////////////

vector<char> reformat_to_byte(const vector<float>& vec, bool invert)
{
    vector<char> hard(vec.size());
    if (invert == false)
    {
        for (unsigned int ii = 0; ii < vec.size(); ii++)
        {
            hard[ii] = 0;
            if (vec[ii] > 0)
                hard[ii] = 1;
        }
    }
    else
    {
        for (unsigned int ii = 0; ii < vec.size(); ii++)
        {
            hard[ii] = 0;
            if (vec[ii] < 0)
                hard[ii] = 1;
        }
    }
    return hard;
}

inline void* allocVector(int n, int typesize)
{
    return (void*)malloc((size_t)n * (size_t)typesize);
}

inline float* allocReal4Vector(int n)
{
    return (float*)allocVector(n, sizeof(float));
}

inline void freeVector(void* pVector)
{
    if (pVector)
    {
        free(pVector);
    }
}

inline void vsfillo_r4(float* vec1, float scal, int n)
{
    long ii;
    for (ii = 0; ii < n; ii++)
    {
        vec1[ii] = scal;
    }
}

void ConvEncode::init(const Trellis& trellis)
{
    StateMap = trellis.nextStates;
    EncodeMap = trellis.codeOutputs;
    k = static_cast<int>(log2(trellis.numInputSymbols));
    n = static_cast<int>(log2(trellis.numOutputSymbols));
    R = (float)k / (float)n;
    NumberStates = trellis.numStates;
    NumberSymbols = trellis.numInputSymbols;
}

LogMap::LogMap()
{
    m1 = NULL;
    m2 = NULL;
    m3 = NULL;
    m7 = NULL;
    ptrAk = NULL;
    ptrBk = NULL;
    ptrG = NULL;
    maxstarTable = NULL;
}

LogMap::~LogMap()
{
    if (m1 != NULL)
        freeVector(m1);
    if (m2 != NULL)
        freeVector(m2);
    if (m3 != NULL)
        freeVector(m3);
    if (m7 != NULL)
        freeVector(m7);
    if (ptrAk != NULL)
        freeVector(ptrAk);
    if (ptrBk != NULL)
        freeVector(ptrBk);
    if (ptrG != NULL)
        freeVector(ptrG);
    if (maxstarTable != NULL)
        freeVector(maxstarTable);
}

void LogMap::init(const Trellis& trellis, const unsigned int stages,
                  short maxMode)
{
    if (maxMode == 0)
        ptr2Max = &LogMap::myMaxstar;
    else if (maxMode == 2)
    {
        ptr2Max = &LogMap::myMaxstarTL;
        // 0 0-9.8304 in inc. of 3e-4 for fabs(x-y)
        genMaxstarTable(32768, (float)3e-4);
    }
    else
        ptr2Max = &LogMap::myMax;

    // Default the ZT and CT decoders to the generic zero terminated decoder
    ptr2ZTdec = &LogMap::decode_zt_generic;

    this->stages = stages;

    // Get information from trellis structure
    this->numStates = trellis.numStates;
    this->bitsPerSymbol = static_cast<unsigned int>(
        round(log2((double)(trellis.numOutputSymbols))));

    // Number of parity bits (assume rate 1/n)
    this->numParity = this->bitsPerSymbol - 1;

    this->nextStates1 = new unsigned int[trellis.nextStates.size()];
    this->nextStates2 = new unsigned int[trellis.nextStates.size()];
    for (unsigned int ii = 0; ii < trellis.nextStates.size(); ii++)
    {
        this->nextStates1[ii] = trellis.nextStates[ii][0];
        this->nextStates2[ii] = trellis.nextStates[ii][1];
    }

    vector<unsigned int> vecOutputs1(trellis.codeOutputs.size());
    vector<unsigned int> vecOutputs2(trellis.codeOutputs.size());
    this->outputs1 = new unsigned int[trellis.codeOutputs.size()];
    this->outputs2 = new unsigned int[trellis.codeOutputs.size()];
    for (unsigned int ii = 0; ii < trellis.codeOutputs.size(); ii++)
    {
        this->outputs1[ii] = trellis.codeOutputs[ii][0];
        this->outputs2[ii] = trellis.codeOutputs[ii][1];
        vecOutputs1[ii] = trellis.codeOutputs[ii][0];
        vecOutputs2[ii] = trellis.codeOutputs[ii][1];
    }

    vector<vector<unsigned int>> tempsymbol1 =
        dec2baseM(vecOutputs1, 2, this->bitsPerSymbol);
    vector<vector<unsigned int>> tempsymbol2 =
        dec2baseM(vecOutputs2, 2, this->bitsPerSymbol);

    this->symbol1 = new int[tempsymbol1.size() * tempsymbol1[0].size()];
    this->symbol2 = new int[tempsymbol1.size() * tempsymbol1[0].size()];
    this->symbolRows = tempsymbol1.size();
    this->symbolElems = tempsymbol1[0].size();

    for (unsigned int ii = 0; ii < tempsymbol1.size(); ii++)
    {
        for (unsigned int jj = 0; jj < tempsymbol1[ii].size(); jj++)
        {
            this->symbol1[ii * this->symbolElems + jj] =
                2 * tempsymbol1[ii][jj] - 1;
            this->symbol2[ii * this->symbolElems + jj] =
                2 * tempsymbol2[ii][jj] - 1;
        }
    }

    // Save the structure of next states
    this->nextStates = new unsigned int[trellis.nextStates.size() *
                                        trellis.nextStates[0].size()];
    this->prevStates = new unsigned int[trellis.nextStates.size() *
                                        trellis.nextStates[0].size()];
    this->prevRows = trellis.nextStates.size();
    this->nextRows = trellis.nextStates.size();
    this->stateElems = trellis.nextStates[0].size();

    // Previous states. Needed for branch metrics
    for (unsigned int ss = 0; ss < this->numStates; ss++)
    {
        unsigned int s0 = trellis.nextStates[ss][0];
        unsigned int s1 = trellis.nextStates[ss][1];

        this->nextStates[ss * this->stateElems + 0] = s0;
        this->nextStates[ss * this->stateElems + 1] = s1;

        this->prevStates[s0 * this->stateElems + 0] = ss;
        this->prevStates[s1 * this->stateElems + 1] = ss;
    }

    // Allocate memory for arrays used to traverse trellis
    unsigned int akRows = numStates;
    unsigned int bkRows = numStates;
    unsigned int nrows = 2 * numStates;

    ptrAk = allocReal4Vector(
        akRows *
        (stages + 1));  //(float *)malloc((akRows*(stages+1))*sizeof(float));
    ptrBk = allocReal4Vector(
        bkRows *
        (stages + 1));  //(float *)malloc((bkRows*(stages+1))*sizeof(float));
    ptrG = allocReal4Vector(
        nrows * stages);  //(float *)malloc((nrows*(stages))*sizeof(float));

    // For the optimized function
    m1 = allocReal4Vector(stages);  //(float *)malloc(stages*sizeof(float));
    m2 = allocReal4Vector(stages);  //(float *)malloc(stages*sizeof(float));
    m3 = allocReal4Vector(stages);  //(float *)malloc(stages*sizeof(float));
    m7 = allocReal4Vector(stages);  //(float *)malloc(stages*sizeof(float));
}

void Turbo::set_maxIterations(const unsigned int maxItr)
{
    if (maxItr > 0 && maxItr < 1000)
        this->maxIterations = maxItr;
}

template<class T>
vector<T> permute(const vector<T>& vec, const vector<unsigned int>& perm)
{
    if (vec.size() != perm.size())
        cout << "Error: permutation vector is not the same size as vec."
             << endl;

    vector<T> out(vec.size());
    for (unsigned int ii = 0; ii < vec.size(); ii++)
        out[ii] = vec[perm[ii]];

    return out;
}

void quickpermute(float* out, const float* in, const unsigned int* perm,
                  const unsigned int sz)
{
    for (unsigned int ii = 0; ii < sz; ii++)
        out[ii] = in[perm[ii]];
}

Turbo::~Turbo()
{
    freeVector(Le12);
    freeVector(Le21);
    freeVector(Lu);
    freeVector(Le);
    freeVector(La);

    freeVector(myu_all);
    freeVector(myw_all);
    freeVector(vp_all);
    freeVector(vq_all);
    freeVector(La_all);
    freeVector(Lu_all);
    freeVector(Le12_all);
    freeVector(Le21_all);
}

void Turbo::decode_zeroTerm(const vector<float>& u, const vector<float>& vp,
                            const vector<float>& vq, const vector<float>& tail1,
                            const vector<float>& tail2, vector<float>& chat)
{
    if (chat.size() != tc_K)
        chat.resize(tc_K);
    float myLc = 2.0 / sigmaSquared;

    memset(&Le21[0], 0, sizeof(float) * lm_stages);

    // zero vector to append the the extrinsic messages because each
    // decoder knows nothing about the tail bits of the other decoder.
    vector<float> zerovec(memory);
    memset(&zerovec[0], 0, memory * sizeof(float));

    // Systematic with tail1
    // tail1 contains the bits used for termintaion of D1
    vector<float> myu = u;
    myu.insert(myu.end(), tail1.begin(), tail1.end());

    // Permuted with tail2
    // tail2 contains the bits used for termination of D2
    vector<float> myw = permute(u, interleaver);
    myw.insert(myw.end(), tail2.begin(), tail2.end());

    unsigned int repeats = 0;
    for (unsigned int zz = 0; zz < maxIterations; zz++)
    {
        numIterations = zz + 1;
        // cout << " -------------------------------- " << endl;
        // cout << " Iteration " << numIterations << endl;
        // Reorder message from D2 to pass to D1.
        quickpermute(&La[0], Le21, &deinterleaver[0], tc_K);
        memset(
            &La[tc_K], 0,
            memory *
                sizeof(
                    float));  // La.insert(La.end(),zerovec.begin(),zerovec.end());
        // Call D1 decoder for the systematic data, vp and tail1
        D1.decode_zeroTerm(&myu[0], &vp[0], &La[0], sigmaSquared, &Lu[0]);
        for (unsigned int ii = 0; ii < tc_K; ii++)
            Le12[ii] = Lu[ii] - myLc * myu[ii] - La[ii];

        // Reorder message from D1 to pass to D2.
        quickpermute(&La[0], &Le12[0], &interleaver[0], tc_K);
        memset(&La[tc_K], 0,
               memory * sizeof(float));  // La.insert(La.end(), zerovec.begin(),
                                         // zerovec.end());
        // Call D2 decoder for the permuted data, vq and tail2
        D2.decode_zeroTerm(&myw[0], &vq[0], &La[0], sigmaSquared, &Lu[0]);
        for (unsigned int ii = 0; ii < tc_K; ii++)
            Le21[ii] = Lu[ii] - myLc * myw[ii] - La[ii];
        quickpermute(&chat[0], &Lu[0], &deinterleaver[0], tc_K);

        if (earlyTermFlag)
        {  // check exit conditions here
            // make hard decisions
            unsigned int diffs = 0;
            for (unsigned int ii = 0; ii < tc_K; ii++)
            {
                if (chat[ii] > 0)
                    cwcurrent[ii] = 1;
                else
                    cwcurrent[ii] = 0;
                if (zz > 0)
                {
                    if (cwlast[ii] != cwcurrent[ii])
                        diffs++;
                }
            }
            cwlast = cwcurrent;
            if (zz > 0)
            {
                if (diffs == 0)
                    repeats++;
                else
                    repeats = 0;
            }
            if (verbose)
                cout << "Iteration " << zz << ": Delta = " << diffs
                     << ", Repeats = " << repeats << endl;
            if (repeats > maxRepeats)
                return;
        }
    }
}

void LogMap::decode_zt_generic(const float* ptrYs, const float* ptrYp,
                               const float* ptrLa, const float sigmaSquared,
                               float* ptrLu)
{
    float myLc = 2.0 / sigmaSquared;

    // Construct and intialize the forward and backward metrics
    unsigned int akRows = numStates;
    vsfillo_r4(ptrAk, -numeric_limits<float>::max(), akRows);
    ptrAk[0] = 0;

    unsigned int bkRows = numStates;
    vsfillo_r4(&ptrBk[stages * bkRows], -numeric_limits<float>::max(), bkRows);
    ptrBk[0 + (stages)*bkRows] = 0;

    // Compute branch metrics
    unsigned int nrows = 2 * numStates;

    // Loop over time
    unsigned int jj = 0;

    for (unsigned int ii = 0; ii < stages; ii++)
    {
        // cout << "ii: " << ii << endl;
        // Loop over all possible state transitions
        for (unsigned int ss = 0; ss < numStates; ss++)
        {
            // cout << "ss: " << ss << endl;
            // Get possible state transitions where ss is the
            // end state and 'index' is the start state
            unsigned int index1 = prevStates[ss * stateElems + 0];
            unsigned int index2 = prevStates[ss * stateElems + 1];

            // Compute branch metrics
            float exp0 = ptrYs[ii] * symbol1[index1 * bitsPerSymbol];
            float exp1 = ptrYs[ii] * symbol2[index2 * bitsPerSymbol];
            for (unsigned int p = 0; p < numParity; p++)
            {
                exp0 +=
                    ptrYp[jj + p] * symbol1[index1 * bitsPerSymbol + (p + 1)];
                exp1 +=
                    ptrYp[jj + p] * symbol2[index2 * bitsPerSymbol + (p + 1)];
            }
            float gk1 = -ptrLa[ii] / 2 + myLc / 2 * exp0;
            float gk2 = ptrLa[ii] / 2 + myLc / 2 * exp1;

            // cout << "-- g1: " << gk1 << endl;
            // cout << "-- g2: " << gk2 << endl;

            // Store branch metrics for later use
            ptrG[2 * index1 + ii * nrows] = gk1;
            ptrG[2 * index2 + 1 + ii * nrows] = gk2;
        }

        // Update index into parity vector yp[]
        jj = jj + numParity;
    }

    // Forward recursion
    for (unsigned int ii = 0; ii < stages; ii++)
    {
        // cout << "---- A ii: " << ii << endl;
        // Loop over all possible state transitions
        for (unsigned int ss = 0; ss < numStates; ss++)
        {
            // Get possible state transitions where ss is the
            // end state and 'index' is the start state
            unsigned int index1 = prevStates[ss * stateElems + 0];
            unsigned int index2 = prevStates[ss * stateElems + 1];

            // Update and store the path metric
            float z1 =
                ptrAk[index1 + ii * akRows] + ptrG[2 * index1 + ii * nrows];
            float z2 =
                ptrAk[index2 + ii * akRows] + ptrG[2 * index2 + 1 + ii * nrows];
            ptrAk[nextStates[index1 * stateElems + 0] + (ii + 1) * akRows] =
                (this->*ptr2Max)(z1, z2);
            // cout << "  z1: " << ptrAk[index1+ii*akRows] << " + " <<
            // ptrG[2*index1+ii*nrows] << " = " << z1 << endl; cout << "  z2: "
            // << ptrAk[index2+ii*akRows] << " + " << ptrG[2*index2+1+ii*nrows]
            // << " = " << z2 << endl; cout << "ss " << ss << " Ak: " <<
            // ptrAk[nextStates[index1*stateElems+0]+(ii+1)*akRows] << endl;
        }
    }

    // Backward recursion
    for (int ii = stages - 1; ii >= 0; ii--)
    {
        // cout << "---- B ii: " << ii << endl;
        // Loop over all possible state transitions
        for (unsigned int ss = 0; ss < numStates; ss++)
        {
            unsigned int s0 = nextStates[ss * stateElems + 0];
            unsigned int s1 = nextStates[ss * stateElems + 1];

            // Update and store the path metric
            float z1 =
                ptrBk[s0 + (ii + 1) * bkRows] + ptrG[2 * ss + ii * nrows];
            float z2 =
                ptrBk[s1 + (ii + 1) * bkRows] + ptrG[2 * ss + 1 + ii * nrows];
            ptrBk[ss + ii * bkRows] = (this->*ptr2Max)(z1, z2);
            // cout << "ss " << ss << " Bk: " << ptrBk[ss+ii*bkRows] << endl;
        }
    }

    for (unsigned int ii = 0; ii < stages; ii++)
    {
        float zn = -numeric_limits<float>::max();
        float zd = -numeric_limits<float>::max();
        for (unsigned int ss = 0; ss < numStates; ss++)
        {
            unsigned int s0 = nextStates[ss * stateElems + 0];
            unsigned int s1 = nextStates[ss * stateElems + 1];

            float temp1 = ptrAk[ss + ii * akRows] +
                          ptrG[2 * ss + 1 + ii * nrows] +
                          ptrBk[s1 + (ii + 1) * bkRows];
            zn = (this->*ptr2Max)(zn, temp1);

            float temp2 = ptrAk[ss + ii * akRows] + ptrG[2 * ss + ii * nrows] +
                          ptrBk[s0 + (ii + 1) * bkRows];
            zd = (this->*ptr2Max)(zd, temp2);
        }
        // cout << "For ii = " << ii << ", zn: " << zn << "  zd: " << zd <<
        // endl;
        ptrLu[ii] = zn - zd;
    }
}

// K is the data size, without the tail bits used for driving the encoder to
// zero. If the code is circular then there are no tail bits
void Turbo::init(const Trellis& t1, const Trellis& t2,
                 const vector<unsigned int>& interleaver, const unsigned int K,
                 const short zeroTerminate, short maxMode)
{
    this->zeroTerminate = zeroTerminate;

    denominator1 = t1.denominator;
    denominator2 = t2.denominator;
    memory = static_cast<unsigned int>(log2(t1.numStates));

    tc_K = K;       // Info length
    lm_stages = K;  // Codeword length
    if (zeroTerminate)
        lm_stages +=
            memory;  // if zero terminated, memory extra bits are appended

    D1.setup(t1, lm_stages, maxMode);
    D2.setup(t2, lm_stages, maxMode);
    E1.setup(t1);
    E2.setup(t2);

    maxIterations = 8;
    verbose = false;
    sigmaSquared = 1.0 / 2.0;
    debug = 0;
    numIterations = 0;
    verbose = 0;
    earlyTermFlag = true;
    maxRepeats = 3;  // early exit if 3 cws in a row have no diffs

    this->interleaver = interleaver;
    deinterleaver.resize(interleaver.size());
    for (unsigned int ii = 0; ii < interleaver.size(); ii++)
        deinterleaver[interleaver[ii]] = ii;
    // extrinsic message vectors to be passed between decoders
    Le12 = allocReal4Vector(lm_stages);
    Le21 = allocReal4Vector(lm_stages);
    Lu = allocReal4Vector(lm_stages);
    Le = allocReal4Vector(lm_stages);

    // temporary vectors to hold extrinsic messages.
    La = allocReal4Vector(lm_stages);

    // if using SSE
    sse_stride = 4;
    Le12_all = allocReal4Vector(lm_stages * sse_stride);
    Le21_all = allocReal4Vector(lm_stages * sse_stride);
    Lu_all = allocReal4Vector(lm_stages * sse_stride);
    La_all = allocReal4Vector(lm_stages * sse_stride);

    myu_all = allocReal4Vector(lm_stages * sse_stride);
    myw_all = allocReal4Vector(lm_stages * sse_stride);
    vp_all = allocReal4Vector(lm_stages * sse_stride);
    vq_all = allocReal4Vector(lm_stages * sse_stride);
    cwlast.resize(lm_stages);
    cwcurrent.resize(lm_stages);
}

int parsePuncturePattern(string puncture, unsigned int p0, unsigned int u0,
                         unsigned int infoBlkSize, vector<vector<int>>& mymat)
{
    // Rate of a single constituent encoder
    // double unpunctE1rate = (double)u0/(double)(p0+u0);
    unsigned int numDefs = 2 * p0 + u0;
    // cout << "p0: " << p0 << endl;
    // cout << "u0: " << u0 << endl;
    mymat.resize(2 * p0 + 1);
    mymat[0].resize(infoBlkSize);
    // total parity of a single constituent encoder
    // unsigned int totalParity = (infoBlkSize*p0);
    unsigned int indivParitySize = infoBlkSize;
    for (unsigned int ii = 1; ii < 2 * p0 + 1; ii++)
    {
        mymat[ii].resize(indivParitySize);
    }

    char* punctPattern = new char[puncture.length() + 1];
    strcpy(punctPattern, puncture.c_str());

    char* temp;

    if ((temp = strtok(punctPattern, ":")) == NULL)
    {
        cout << "1 Error parsing puncture string.\n";
        return (-1);
    }
    unsigned int n0 = atoi(temp);
    // cout << "n0: " << n0 << endl;
    if (n0 != numDefs)
        return 1;

    if ((temp = strtok(NULL, ":")) == NULL)
    {
        cout << "2 Error parsing puncture string.\n";
        return (-1);
    }
    unsigned int k0 = static_cast<unsigned int>(atoi(temp));
    // cout << "k0: " << k0 << endl;

    vector<vector<unsigned int>> tempmat(n0, vector<unsigned int>(k0));

    for (unsigned int ii = 0; ii < numDefs; ii++)
    {
        for (unsigned int jj = 0; jj < k0; jj++)
        {
            if ((temp = strtok(NULL, ",")) == NULL)
            {
                cout << "Error parsing code string. Looking for comma.\n";
                return (-1);
            }
            tempmat[ii][jj] = static_cast<unsigned int>(atoi(temp));
        }
    }

    for (unsigned int ii = 0; ii < tempmat.size(); ii++)
    {
        unsigned int kk = 0;
        // For each set of parity and the data go through and
        // define the whole puncture pattern for a codeblock
        for (unsigned int jj = 0; jj < mymat[ii].size(); jj++)
        {
            if (tempmat[ii][kk++] == 1)
                mymat[ii][jj] = 1;
            else
                mymat[ii][jj] = 0;
            if (kk == tempmat[ii].size())
                kk = 0;
        }
    }

    delete[] punctPattern;

    return 0;
}

unsigned int baseM2dec(vector<unsigned int>& s, unsigned int M, unsigned int L)
{
    unsigned int fact = 1;
    unsigned int n = 0;
    for (int ii = L - 1; ii >= 0; ii--)
    {
        n = n + s[ii] * fact;
        fact = fact * M;
    }
    return n;
}

unsigned int baseM2dec(vector<char>& s, unsigned int M, unsigned int L)
{
    unsigned int fact = 1;
    unsigned int n = 0;
    for (int ii = L - 1; ii >= 0; ii--)
    {
        n = n + s[ii] * fact;
        fact = fact * M;
    }
    return n;
}

vector<unsigned int> dec2baseM(unsigned int n, unsigned int M, unsigned int L)
{
    vector<unsigned int> out(L);

    for (int ii = L - 1; ii >= 0; ii--)
    {
        out[ii] = static_cast<unsigned int>(fmod((float)n, (float)M));
        n = (n - out[ii]) / M;
    }
    return out;
}

vector<vector<unsigned int>> dec2baseM(vector<unsigned int> v, unsigned int M,
                                       unsigned int L)
{
    vector<vector<unsigned int>> out(v.size(), vector<unsigned int>(L));
    for (size_t ii = 0; ii < v.size(); ii++)
        out[ii] = dec2baseM(v[ii], M, L);
    return out;
}

unsigned int oct2dec(const unsigned int& val)
{
    unsigned int dec_val;
    std::stringstream ss;

    ss << val << std::oct;
    ss >> dec_val;

    return dec_val;
}

vector<unsigned int> oct2dec(const vector<unsigned int>& v)
{
    vector<unsigned int> out(v.size());
    for (size_t ii = 0; ii < v.size(); ii++)
        out[ii] = oct2dec(v[ii]);
    return out;
}

Trellis rscPoly2trellis(const unsigned int& constraint,
                        const unsigned int& denominator,
                        const vector<unsigned int>& numerators)
{
    unsigned int debug = 0;
    unsigned int n = numerators.size() + 1;  // +1 for the systematic bit

    unsigned int k = 1;  // 1 for each block of memory elements.  Hard code to 1

    // Set the constraint length
    unsigned int K = constraint;

    // Set the total number of memory elements.
    unsigned int L = K - 1;  // number of memory elems is constraint length - 1

    // Number of states, symbols and transitions
    unsigned int NumberStates = 1 << L;
    unsigned int NumberSymbols = k << 1;
    unsigned int NumberTransitions = 1 << K;

    if (debug)
        cout << "# states: " << NumberStates << endl;
    if (debug)
        cout << "# Symbols: " << NumberSymbols << endl;
    if (debug)
        cout << "# Transitions: " << NumberTransitions << endl;

    vector<unsigned int> btemp;
    vector<unsigned int> bits;
    vector<unsigned int> init(NumberTransitions);
    vector<unsigned int> final(NumberTransitions);
    vector<unsigned int> ns(NumberTransitions);
    vector<unsigned int> cword(NumberTransitions);
    vector<unsigned int> init_tmp;
    vector<unsigned int> final_tmp;
    vector<unsigned int> ns_tmp;
    vector<unsigned int> result(n);

    // vector containing integer representation of numerator taps
    vector<unsigned int> tempnum = oct2dec(numerators);
    if (debug)
    {
        for (unsigned int iii = 0; iii < tempnum.size(); iii++)
            cout << "tempnum[" << iii << "]: " << tempnum[iii] << endl;
    }
    // integer representation of denominator (feedback) taps
    unsigned int tempdenom = oct2dec(denominator);
    if (debug)
        cout << "tempdenom: " << tempdenom << endl;

    for (unsigned int ii = 0; ii < NumberTransitions; ii++)
    {
        if (debug)
            cout << "\nTransition ii: " << ii << endl;

        unsigned int Ktemp = constraint;
        // bits contains the input bit and bits in the state memory
        btemp = dec2baseM(ii, 2, K);
        bits.resize(0);
        bits.insert(bits.end(), btemp.begin(), btemp.begin() + Ktemp);
        // is the initial state
        init_tmp.resize(0);
        init_tmp.insert(init_tmp.end(), bits.begin() + 1, bits.begin() + Ktemp);
        // Next symbol
        ns_tmp.resize(0);
        ns_tmp.insert(ns_tmp.end(), bits[0]);

        // compute the feedback bit
        unsigned int feedbackBit = 0;
        for (unsigned int jj = 1; jj < bits.size(); jj++)
        {
            feedbackBit ^=
                bits[jj] * (tempdenom & (0x1 << (bits.size() - 1 - jj))) >>
                (bits.size() - 1 - jj);
        }
        if (debug)
            cout << "feedback bit is: " << feedbackBit << endl;
        // compute the first bit of the next state (feedback XOR inputbit)
        // is the final state (after the transition)
        unsigned int nextBit = feedbackBit ^ bits[0];
        if (debug)
            cout << "next bit into memory is : " << nextBit << endl;
        final_tmp.resize(0);
        final_tmp.insert(final_tmp.end(), nextBit);
        if (Ktemp > 1)
            final_tmp.insert(final_tmp.end(), bits.begin() + 1,
                             bits.begin() + Ktemp - 1);

        // compute the parity bits
        memset(&result[0], 0, sizeof(unsigned int) * n);
        result[0] = bits[0];  // systematic bit
        for (unsigned int iii = 1; iii < result.size();
             iii++)  // iterates over each parity.
        {
            result[iii] =
                nextBit * ((tempnum[iii - 1] & (0x1 << (bits.size() - 1))) >>
                           (bits.size() - 1));
            // unsigned int poly = tempnum[iii-1];
            if (debug)
                cout << "poly: " << numerators[iii - 1] << endl;
            for (unsigned int jj = 1; jj < bits.size();
                 jj++)  // iterates over each memory element
            {
                result[iii] ^= bits[jj] * ((tempnum[iii - 1] &
                                            (0x1 << (bits.size() - 1 - jj))) >>
                                           (bits.size() - 1 - jj));
            }
            if (debug)
                cout << "result: " << result[iii] << endl;
        }

        init[ii] = baseM2dec(init_tmp, 2, L);
        final[ii] = baseM2dec(final_tmp, 2, L);
        ns[ii] = baseM2dec(ns_tmp, 2, k);
        cword[ii] = baseM2dec(result, 2, n);
    }

    // Convert the information to lookup tables that encapsulate the trellis
    // structure
    vector<vector<unsigned int>> StateMap(
        NumberStates, vector<unsigned int>(NumberSymbols, 0));
    vector<vector<unsigned int>> EncodeMap(
        NumberStates, vector<unsigned int>(NumberSymbols, 0));
    for (unsigned int ii = 0; ii < NumberTransitions; ii++)
    {
        if (debug)
            cout << "init: " << init[ii] << "  to  " << final[ii]
                 << " given input " << ns[ii] << " results in cw " << cword[ii]
                 << endl;
        StateMap[init[ii]][ns[ii]] = final[ii];
        EncodeMap[init[ii]][ns[ii]] = cword[ii];
    }

    Trellis trellis;
    trellis.nextStates = StateMap;
    trellis.codeOutputs = EncodeMap;
    trellis.numInputSymbols = NumberSymbols;
    trellis.numOutputSymbols =
        static_cast<unsigned int>(pow((float)2, (float)n));
    trellis.numStates = NumberStates;
    trellis.denominator = tempdenom;
    return trellis;
}

Trellis singleConstraintPolyString2trellis(const string& s)
{
    char* c_s = new char[s.length() + 1];
    strcpy(c_s, s.c_str());

    char* temp;
    if ((temp = strtok(c_s, ":")) == NULL)
        cout << "Error parsing code string.\n";
    unsigned int constraint = static_cast<unsigned int>(atoi(temp));

    if ((temp = strtok(NULL, ":")) == NULL)
        cout << "Error parsing code string.\n";
    unsigned int numGenerators = static_cast<unsigned int>(atoi(temp));

    if ((temp = strtok(NULL, ":")) == NULL)
        cout << "Error parsing code string.\n";
    unsigned int denominator = static_cast<unsigned int>(atoi(temp));

    // get the values for the next states:
    char* next;
    if ((next = strtok(NULL, ":")) == NULL)
        cout << "Error parsing code string.\n";

    vector<unsigned int> polys(numGenerators);

    if ((temp = strtok(next, ",")) == NULL)
        cout << "Error parsing code string numerators.\n";
    polys[0] = static_cast<unsigned int>(atoi(temp));
    for (unsigned int ii = 1; ii < numGenerators; ii++)
    {
        if ((temp = strtok(NULL, ",")) == NULL)
            cout << "Error parsing code string numerators.\n";
        polys[ii] = static_cast<unsigned int>(atoi(temp));
    }

    Trellis trellis = rscPoly2trellis(constraint, denominator, polys);

    return trellis;
}
