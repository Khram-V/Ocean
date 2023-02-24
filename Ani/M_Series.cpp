//
//      Series                                     Ver 2.2 90.12.23
//      Второй вариант построения объекта-вектора
//      Реализуются процедуры ввода/вывода с графическим интерфейсом
//
#include "Mario.h"

void Mario::Init()
{ N=0;          // Constructor
  V=NULL;       //
  Title[0]=0;   //
  Longitude=142.75; Latitude=46.95; dt=1.0; flag=0; Fmt=0; Typ=1;
  Depth=Distance=Bearing=0.0;
}
Real* Mario::allocate( size_t s )
{   V=(Real*)Allocate( s*sizeof(Real),N?V:0 ); N=s; return V;
}
void Mario::free(){ if( V )allocate( 0 ); N=0; }
                                 //
Real& Mario::operator[]( int k ) // С контролем границ
{                                //
  if( unsigned( k )>=unsigned( N ) )Break( "\nIndex:%d of [%d]",k,N ); return V[k];
}
Real Mario::operator()( const Event& E )
{ double x=(E-JT)/dt;
  if( x<0 || x>N )return Mid;
  {  int k=(int)x; if( k<0 )k=0; else if( k>N-2 )k=N-2;
   Real *v=V+k;
    if( (x-=k)==0 )return v[0];
                   return v[0]+x*(v[1]-v[0]);
  }
}
Real Mario::value( const Event& E, int Power )
{                               //
 double Ar=(E-JT)/dt;           // Интерполяция по Лагранжу
  if( Ar<0 || Ar>N )return Mid; //
  { double  yo,p;               //
   int Ns,Nf,j=(int)Ar,k=N-1;   // Поиск ближайшего левого индекса
    if( j>k )j=k; else          //
    if( j<0 )j=0;
    if( Power>k )Power=k;
    if( Power<=0 )return V[j];
    if( (Nf=++j+Power/2)>k ){ Nf=k; Ns=k-Power; }else
    if( (Ns=Nf-Power)<0    ){ Ns=0; Nf=Power;   }
    for(  yo=0,j=Ns; j<=Nf; j++ )
    { for( p=1,k=Ns; k<=Nf; k++ )if( j!=k )p*=(Ar-k)/(j-k); yo+=V[j]*p;
    } return yo;
  }
}
