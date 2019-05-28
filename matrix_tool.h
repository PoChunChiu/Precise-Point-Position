/*-----------------------------------------------------------------
 * Description: -Routines for matrix math
 * This is an attempt of modification of original matmath.c used
 *  1. The major change is the MatAlloc() and MatFree() funcitons.
 *     User should use these two functions to memory allocation and free.
 *  2. An #define ASSERT() function is provided if in DEBUG_MEM mode
 *  3. matrix.h enumerate the error message types for return purpose
 *      instead of the original exit()
 *  4. This matrix routines uses ONLY single pointers for arrays and matrices.
 *  5. User has always to check the (if NULL) for the return matrix
 *
 *   Functions:
 *      MatAlloc   MatFree      MatFill
 *	MatPrint   MatProd	MatScalarProd	MatAdd	MatTranspose
 *      MatDeter   MatInvert	NormalizeEigenVectors	JacobiRotation
 *
 *   Modification:
 *       Error handling, YC Chao, Aug, 1996
 *-----------------------------------------------------------------
 */
#ifndef _matrix_h_
#define _matrix_h_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* ===================================================
 * ASSERT() implementation from "Writing Solid Code"
 * =================================================== */
#ifdef DEBUG_MEM
void _Assert(char*, unsigned);  /* prototype */

#define  ASSERT(f)    \
    if (f)      \
    NULL;  \
    else        \
    _Assert(__FILE__, __LINE__)
#else
#define ASSERT(f)   NULL
#endif

/* -----------------------------------------------------------------
 * matrix allocation, still allocate single pointer array for matrix
 *------------------------------------------------------------------ */
double*  MatAlloc(size_t nrow, size_t ncol);

/* -----------------------------------------------------------------
 * matrix de-allocation
 *------------------------------------------------------------------ */
void MatFree(void *pv);

/* -------------------------------------------- */
void MatPrint(double *m1, int m, int n, char *matname);

/* -------------------------------------------- */
void MatProd(double *m1, double *m2, int l, int m, int n, double *m3);

/* ================================================================ */


#endif


