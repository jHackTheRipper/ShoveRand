/*
 *  utils.h
 *  
 *
 *  Created by Jonathan PASSERAT-PALMBACH on 4/7/11.
 *  Copyright 2011 ISIMA/LIMOS. All rights reserved.
 *
 */

/** Some defines to spare constant memory... */
#define        m1         4294967087.0   // 2^32 - 209
#define        m2         4294944443.0   // 2^32 - 22853
#define        norm       1.0 / (m1 + 1.0)
#define        a12        1403580.0       // a1_2
#define        a13n       810728.0        // -a1_3
#define        a21        527612.0        //  a2_1 
#define        a23n       1370589.0       // -a2_3
#define        two17      131072.0        // 2^17
#define        two53      9007199254740992.0 // 2^53
#define        fact       5.9604644775390625e-8f     /* 1 / 2^24  */


/* The following are the transition matrices of the two MRG components
 * (in matrix form), raised to the powers 1, 2^76, and 2^127, resp.
 */
__constant__ double A1p0[3][3]; 
__constant__ double A2p0[3][3]; 
__constant__ double A1p76[3][3];
__constant__ double A2p76[3][3];
__constant__ double A1p127[3][3];
__constant__ double A2p127[3][3];


/*  A matrices powers from (A^76)^1 up to (A^76)^512 */
__constant__ double A1_pows [11][3][3];
__constant__ double A2_pows [11][3][3];


// the following functions are used on both host and device sides
// they are marked inlined so that utils.h can be included
// in several compilation units without any linking problem


/**
 * Return (a*s + c) MOD m; a, s, c and m must be < 2^35
 *  \author Pierre L'Ecuyer
 */
inline __host__ __device__ 
double MultModM (double a, double s, double c, double m)
{
   double v;
   long a1;
   
   v = a * s + c;
   
   if (v >= two53 || v <= -two53) {
      // changed static_casts to regular casts due to cuda
      a1 = (long) (a / two17); 
      a -= a1 * two17;
      
      v  = a1 * s;
      
      a1 = (long) (v / m);
      v -= a1 * m;
      
      v = v * two17 + a * s + c;
   }
   
   a1 = (long) (v / m);
   
   /* in case v < 0)*/
   if ((v -= a1 * m) < 0.0)    return v += m;   
   else                        return v;
}


/**
 * Compute the vector v = A*s MOD m. Assume that -m < s[i] < m.
 * Works also when v = s.
 *  \author Pierre L'Ecuyer
 */
inline __host__ __device__
void MatVecModM (const double A[3][3], const double s[3], double v[3],
                 double m)
{
   int i;
   double x[3];               // Necessary if v = s
   
   for (i = 0; i < 3; ++i) {
      x[i] = MultModM (A[i][0], s[0], 0.0, m);
      x[i] = MultModM (A[i][1], s[1], x[i], m);
      x[i] = MultModM (A[i][2], s[2], x[i], m);
   }
   for (i = 0; i < 3; ++i)
      v[i] = x[i];
}


/**
 * Compute the matrix C = A*B MOD m. Assume that -m < s[i] < m.
 * Note: works also if A = C or B = C or A = B = C.
 *  \author Pierre L'Ecuyer
 */
inline __host__ __device__
void MatMatModM (const double A[3][3], const double B[3][3],
                 double C[3][3], double m)
{
   int i, j;
   double V[3], W[3][3];
   
   for (i = 0; i < 3; ++i) {
      for (j = 0; j < 3; ++j)
         V[j] = B[j][i];
      MatVecModM (A, V, V, m);
      for (j = 0; j < 3; ++j)
         W[j][i] = V[j];
   }
   for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
         C[i][j] = W[i][j];
}


/**
 * Compute the matrix B = (A^(2^e) Mod m);  works also if A = B. 
 *
 *  \author Pierre L'Ecuyer
 */
inline __host__ __device__
void MatTwoPowModM (const double A[3][3], double B[3][3], double m, long e)
{
   int i, j;
   
   /* initialize: B = A */
   if (A != B) {
      for (i = 0; i < 3; ++i)
         for (j = 0; j < 3; ++j)
            B[i][j] = A[i][j];
   }
   /* Compute B = A^(2^e) mod m */
   for (i = 0; i < e; i++)
      MatMatModM (B, B, B, m);
}



/** Raise matrices to given power using a quick exponentiation 
 algorithm.
 \param A1 A1 matrix to be raised to given power
 \param A2 A2 matrix to be raised to given power
 \param pow Power to raise both matrices to
 */
inline __host__ __device__
void MyMatPowModM (double A1[3][3], double A2[3][3], long pow) {
   int i = 0;
   
   // figure out which of the precomputed need to be used 
   // (stored from indice 1 to 10 in arrays)
   // if a power is not needed array[0] is reached, that is
   // identity
   while (i < 11) { 
      MatMatModM(A1p76, A1_pows[i & (pow >> i)], A1, m1);
      MatMatModM(A2p76, A2_pows[i & (pow >> i)], A2, m2);
      ++i;
   }
}
