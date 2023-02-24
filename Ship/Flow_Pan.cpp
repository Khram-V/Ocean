//
//      Клайв Флетчер                   C.A.J.Fletcher
//      Вычислительные методы           Computational Techniques
//      в динамике жидкостей            for Fluid Dynamics 2
//
//              М."МИР", 1991           Specific Technicues for
//              Том 2, стр.155-163      Different Flow Categories
//
//      Panel calculates velocity and pressure about
//      an arbitrary closed body using the PANEL method.
//
//                      (c) Compiled to C++ by V.Khramushin, 1993
#include "..\Type.h"
                        //
static   int  N=0;      // Расчетное количество точек контура
static Point  Vo={1,0}, // Компоненты скорости набегающего потока
              *C=0,     // Координаты контрольных точек панелей
              *D=0;     // Косинусы в связной с панелью системе отсчета
static Real  *Ds=0,     // Длина панели
            *Sde=0;     // Вектор плотностей источников
void Fletcher           //
(       int   _N,       // Количество точек контура (без соединения)
        Point *P,       // Координаты узловых точек окаймляющих тело
        Point _V,       // Компоненты скорости набегающего потока
        Point *V,       // Требование вычислить скорости в абсолютной
        Point *Q        //  и локальной системах координат
){                      //
 Point **F;             // Нормальная и касательная к панели (k)
 int k,j;               //   скорость, создаваемая элементом (j)
  N =_N;                //
  Vo=_V;
  C=(Point*)Allocate( N*sizeof(Point),C );
  D=(Point*)Allocate( N*sizeof(Point),D );
  Ds=(Real*)Allocate( N*sizeof(Real),Ds );
 Sde=(Real*)Allocate( N*sizeof(Real),Sde );
//
//      Calculates body and control point coordinates
//      for an ellipse with minor SEMI-AXIS, B
//
  for( k=0,j=1; k<N; k++,j++ )
  { if( j==N )j=0;         //
    C[k] =  (P[j]+P[k])/2; // Place control points at the center of panels
   Ds[k]=abs(P[j]-P[k] );  // Calculate panel spans, COS and SIN of angles
    D[k] =  (P[j]-P[k])/Ds[k];
  }
//      Calculates matrix elements and right hand side vector
//
  F=(Point**)Allocate( N,N*sizeof(Point) );
  { Point e,q;
    Real sph;
    for(   k=0; k<N; k++ )
    { for( j=0; j<N; j++ )
      { if( k==j ){ F[k][j]=(Point){ 0,2.0*M_PI }; continue; }
        q = C[k]-C[j];
        e = conj( polar( abs( q ),arg( D[j] )-arg( q ) ) );
        sph=Ds[j]/2;
        q.x=log( norm( e+sph )/norm( e-sph ) );
        q.y=2*atan2( e.y*Ds[j],norm( e )-sqr( sph ) );
                                        //
        F[k][j] = q*D[j]*conj( D[k] );  // Construct the matrix equation
                                        //
      } Sde[k]=Vo.x*D[k].y-Vo.y*D[k].x; // Right Hand size
    }                                   //
  }
//      Решение системы линейных уравнений для
//      вычисления распределенных источников Sde
  {             //
    int   *ik;  // Вектор перестановок матрицы AA
   Real **AA;  //
    ik=  (int*) Allocate(   N*sizeof(int) );
    AA=(Real**)Allocate( N,N*sizeof(Real) );
    for( k=0; k<N; k++ )
    for( j=0; j<N; j++ )AA[k][j]=F[k][j].y;

   void FACT ( int,Real**,int* );
   void SOLVE( int,Real**,int*,Real* );
    FACT ( N,AA,ik );    // Factorise AA into L.U
    if( ik[N-1]<0 )N=0; else
    SOLVE( N,AA,ik,Sde );// Solve for the source densities, Sde(k)
    Allocate( 0,0,AA );  //
    Allocate( 0,ik );
  }
//      Calculates velocities and pressure at the control points
//
  if( N>0 )
  { if( Q!=0 )P=Q; else
    if( V!=0 )P=V; else P=0;
    if( P!=0 )
    for( k=0; k<N; k++ )
    { for( P[k]=0,j=0; j<N; j++ )P[k]+=F[k][j]*Sde[j];
      if( V!=0 )V[k] =Vo+P[k]*D[k];
      if( Q!=0 )Q[k]+=Vo*conj(D[k]);
    }
  } Allocate( 0,0,F );
}
//      Calculates the flow at the given points, (Xp,Yp)
//
Point Fletcher( Point P )
{ Point V,Q,e;
  Real  sph;
    V=0.0;
  for( int j=0; j<N; j++ )
  { Q = P-C[j];
    e = conj( polar( abs( Q ),arg( D[j] )-arg( Q ) ) );
    sph=Ds[j]/2;
    Q.x=log( norm( e+sph )/norm( e-sph ) );
    Q.y=2*atan2( e.y*Ds[j],norm( e )-sqr( sph ) );
    V +=Q*D[j]*Sde[j];
  } return V;
}
