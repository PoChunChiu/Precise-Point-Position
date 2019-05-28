#include "matrix_tool.h"

#ifdef DEBUG_MEM
void _Assert(char *strFile, unsigned uLine)
{
  fflush(NULL);
  fprintf(stderr, "\nAssertion failed: %s, line%u\n",
                    strFile, uLine);
  fflush(stderr);
  abort();
}
#endif
/* ================================================================ */


/* -----------------------------------------------------------------
 * matrix allocation, still allocate single pointer array for matrix
 *------------------------------------------------------------------ */
double* MatAlloc(size_t nrow, size_t ncol)
{
  double *ret;

  ASSERT((nrow!= 0) && (ncol!= 0));

  ret= (double *) calloc(nrow*ncol, sizeof(double));

  if( ret == NULL )
      printf("Memory allocation error in MatAlloc()\n");

  return (ret);
}

/* ----------------------------------------------------
 * matrix de-allocation
 *----------------------------------------------------- */
void MatFree(void *pv)
{
  ASSERT(pv != NULL);

  free(pv);
}

/*--------------------------------------------------------------*/
/* prints an m x n matrix to the screen */
void MatPrint (double *m1, int m, int n, char *matname)
{
    int i, j, in;
    float tm1;

    printf ("%s\n", matname );
    printf ("\n");

    for ( i = 0; i < m; i++ )
    {
        in = i * n;
        for ( j = 0; j < n; j++ )
        {
            tm1 = (float) m1[in + j];
            printf ("%10.3f  ", tm1);
        }
        printf ("\n");
    }
    printf ("\n");
}

/*--------------------------------------------------------------*/
/* returns the product of l x m * m x n matrices. */
void MatProd (double *m1, double *m2, int l, int m,
             int n, double *m3)
{
    int i, j, k, in, im;

    for ( i = 0; i < l; i++ )
    {
        in = i * n;
        im = i * m;
        for ( j = 0; j < n; j++ )
        {
            m3[in + j] = 0;
            for ( k = 0; k < m; k++ )
            {
                m3[in + j] = m3[in + j] + m1[im + k] * m2[k * n + j];
            }
        }
    }
}
