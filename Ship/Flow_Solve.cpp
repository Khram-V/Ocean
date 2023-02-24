//
//      Клайв Флетчер                   C.A.J.Fletcher
//      Вычислительные методы           Computational Techniques
//      в динамике жидкостей            for Fluid Dynamics 2
//
//              М."МИР", 1991           Specific Techniques for
//              Том 1, стр.234-238      Different Flow Categories
//
//      Factorises a into permuted L.U so that PERM*A = L*U
//      JPVT(k) Gives index of KTH pivot row
//      Sets JPVT(n) = -1 if zero pivot occurs
//
#include <Math.h>

static const double Eps=1e-15;
// double abs(double P){ return *((double*)(&(*((long*)(&P))&=0x7FFFFFFFL))); }

void FACT( int N, double **A, int *jpvt )
{
 int i,j,k,l; double s;
//
//      Gauss elimination with partial pivoting
//
  for( k=0; k<N-1; k++ )                         //      Select pivot
  { for( l=k,i=k+1; i<N; i++ )if( fabs( A[i][k] )>fabs( A[l][k]) )l=i;
    jpvt[k]=l;
    s=A[l][k]; A[l][k]=A[k][k]; A[k][k]=s;

    if( fabs( s )<Eps ){ jpvt[N-1]=-1; return; }  //      Check for zero pivot
    for( i=k+1; i<N; i++ )A[i][k]/=-s;           //      Calculate multipliers
//
//      Interchange and eliminate by columns
//
    for( j=k+1; j<N; j++ )
    { s=A[l][j]; A[l][j]=A[k][j]; A[k][j]=s; if( fabs( s )<Eps )continue;
      for( i=k+1; i<N; i++ )A[i][j]+=A[i][k]*s;
    }
} }
//      Solves linear system, A*X = B
//      Assumes A is factorised into L.U form (by FACT)
//      Returns solution, X, in B
//
void SOLVE( int N, double **A, int *jpvt, double *B )
{ double s;
  for( int k=0; k<N-1; k++ )                       //      Forward elimination
  { int l=jpvt[k]; s=B[l]; B[l]=B[k]; B[k]=s;
    for( int i=k+1; i<N; i++ )B[i]+=A[i][k]*s;
  }
  for( int k=0; k<N-1; k++ )                        //      Back substitution
  { int j=N-k-1; B[j]/=A[j][j]; s=-B[j];
    for( int i=0; i<j; i++ )B[i]+=A[i][j]*s;
  } B[0]/=A[0][0];
}
