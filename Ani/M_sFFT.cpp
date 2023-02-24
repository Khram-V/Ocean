#include <Math.h>
#include "../Type.h"
//
//г---------------------------------------------------------¬
//¦                                                         ¦
//¦ I0 Подпрограмма, рассчитывающая функции Кайзера-Бесселя ¦
//¦             мнимого аргумента нулевого порядка          ¦
//¦                                                         ¦
//L----------------------------------------------------------
//
static Real I0( Real Z ) //Z=fabs( Z )
{ if( Z>3.75 )
  { Real A=exp( Z )/sqrt( Z ); Z=3.75/Z;
    return A*
    (((((((3.923770e-3 *Z -1.647633e-2)*Z +2.635537e-2)*Z
          -2.057706e-2)*Z +9.162810e-3)*Z -1.575650e-3)*Z
          +2.253190e-3)*Z +1.328592e-2)*Z +3.989423e-1;
  }
  Z *= Z*7.111111e-2;
  return
    (((((4.5813e-3*Z +3.60768e-2)*Z +2.659732e-1)*Z
        +1.206749)*Z +3.089942 )*Z +3.515623 )*Z+1.0;
}
//     K I Z E R S
//г--------------------------------------¬
//¦                                      ¦
//¦    Программа, рассчитывающая весовые ¦
//¦     множители окна Кайзера-Бесселя   ¦
//¦                                      ¦
//L---------------------------------------
//
void KIZERS( int N, Real* AA )
{ const Real RIO=12.513;                 //=I0( M_PI*3.0 )*0.539319;
  Real A=-1.0,dA=2.0/Real( N-1 );
    for( int I=0; I<N; I++,A+=dA )
     *AA++ = I0( sqrt( fabs( 1.0-A*A ) )*( M_PI*3.0 ) )/RIO;
}
void TRIANGLE( int N, Real* AA )
{ Real dA=3.3/Real( N-1 ); int i;
  for( i=1; i<=N; i++,AA++ )if( i<=N/2 )AA[0]=dA*i; else  AA[0]=dA*(N-i+1);
}
// г----------------------------¬
// ¦   BESM-6 D800 10.01.72     ¦
// ¦    X Комплексный массив    ¦
// ¦    N 2^M-2 размерность X   ¦
// L-----------------------------
struct comp{ Real r,i;
       comp( Real re=0, Real im=0 ): r(re),i(im){}
friend comp  operator * ( const comp& a, const comp& b )
              { return comp( a.r*b.r-a.i*b.i, a.r*b.i+a.i*b.r ); }
       comp& operator *=( const comp& z ){ return *this=*this*z; }
};
void FFT( int N, Real* X, Real* x )
{
 comp U,W,T;
 int  i,j,k;
  for( i=j=k=0; i<N-1; i++,j+=k )
  { if( i<j )
    { T.r=X[j]; X[j]=X[i]; X[i]=T.r;
      T.i=x[j]; x[j]=x[i]; x[i]=T.i;
    }
    for( k=N/2; k<=j; ){ j-=k; k/=2; }
  }
  for( k=1; k<N; k+=k )
  { U=1.0;
    W.i=M_PI/Real(-k);  W.r=cos( W.i );
                          W.i=sin( W.i );
    for( j=0; j<k; j++,U*=W )
    for( i=j; i<N; i+=k+k )
    { T=comp( X[i+k],x[i+k] )*U;
      X[i+k]=X[i]-T.r; X[i]+=T.r;
      x[i+k]=x[i]-T.i; x[i]+=T.i;
    }
  } for( k=0; k<N; k++ ){ *X++/=N; *x++/=N; }
}
void FFT( int N, Real* X )
{
 comp U,W;
 Real T;
 int i,j,k;
  for( i=j=k=0; i<N-1; i++,j+=k )
  { if( i<j ){ T=X[j]; X[j]=X[i]; X[i]=T; }
    for( k=N/2; k<=j; ){ j-=k; k/=2; }
  }
  for( k=1; k<N; k+=k )
  { U=1.0;
    W.i=M_PI/Real(-k); W.r=cos( W.i );
                       W.i=sin( W.i );
    for( j=0; j<k; j++,U*=W )
    for( i=j; i<N; i+=k+k )
    { T=U.r*X[i+k];
      X[i+k]=X[i]-T;
      X[i]+=T;
  } }
  for( k=0; k<N; k++ )*X++ /= N;
}
