//
//      Function.h                                                 /2008.11.09/
//      ��������� ���������� ������� �������
//
#include <Math.h>
#include "Function.h"
//
//      ���������� ������� �������� �������
//
//      ������� ����� ���� ���������� �� ������������ ������� ����������
//      ���� �������� �� �����, �� �� ��������� ����� ������� �����
//            (�.�. - ��������� ��� �� 0 �� length-1)
//
Function::Function():_Y(0),_X(0),_2(0),length(0),met(easy),ext(false){}
Function::Function( int S ):_Y(0),_X(0),_2(0),length(0),met(easy),ext(false)
                  { set( S ); }         // �������� ������ ������� �������
Function::Function( Real *Y, Real *X ):_2( 0 ),length( 0 ),met( easy )
                  { set( Y,X ); }       // ����������� ������� � ���������
Function& Function::set( size_t S )     // ����� ������� ����������
{ if( ext )_Y=NULL;                     //  ���������� ������� �������
          _Y=(Real*)Allocate( S*sizeof( Real ),_Y );
  if( _X )_X=(Real*)Allocate( S*sizeof( Real ),_X );
  if( _2 )Allocate( 0,_2 ); _2=NULL;    // ���������� ������� ���������� �����
  length=S; met=easy; ext=false;        // � ��� �� ������������ ������� "easy"
  return *this;
}
Function& Function::set( Real *Y )      // ������� ���������� �������
{ _Y=Y; length=Allocate( Y )/sizeof( Real ); met=easy; ext=true; return *this;
}
Function& Function::free()              // ���� ���� ����� ���������
{ if( !ext )                            // !!! � �� ������� ����� �������
    { if( _Y )Allocate( 0,_Y );         //
      if( _X )Allocate( 0,_X );
    } if( _2 )Allocate( 0,_2 ); _2=_Y=_X=NULL; length=0; met=easy; ext=false;
  return *this;
}                                       //
//      In/Out ������ � ���������� �����
//      ������ ��� �������-��������� ���������� ��� ����� ������ ���������
//
Real& Function::X( int k )                       // ���� ������� ���������
{ if( !_X )                                      // ��� ���, �� �� ����������
  { if( ext ){ static Real w; return w=k; }      // � ����������� ���������,
    _X=(Real*)Allocate( length*sizeof( Real ) ); // ��� �������� ������, ��
     for( size_t i=0; i<length; i++ )_X[i]=i;    // ����� �� ��������
  } return _X[k];                                // �� ���������
}                                 //
int Function::find( Real Ar )     // �������� ����� ���������� ������
{ if( !_X )return int( Ar ); else // ������� ��� ��������� ���������
  { int i=0,k,n=length-1;         // � ������ ����� ��� ��������
    bool d=_X[1]>_X[0];           //
    while( n-i>1 ){ k=(n+i)/2; if( d ){ if( Ar<_X[k] )n=k; else i=k; }
                                 else { if( Ar>_X[k] )n=k; else i=k; } }
    return i;
} }
//
//      ���������� ������������� �������
//
Function&                    // Numerical Recipes - Cambridge university
Function::SpLine( Real yp1,  // ����������� ������-������������
                  Real ypn ) // ����������� � ������ � � ����� �������
{ if( length>=3 )            // ���� ��� 4 �����, �� - �����
  { int  i,k,n=length-1;
    Real un,qn,*u;
      _2=(Real*)Allocate( length*sizeof( Real ),_2 );
       u=(Real*)Allocate( n*sizeof( Real ) );
    met=spline;
    if( _X )
    { Real p,sig,*x=_X,*y=_Y;
      if( yp1>.99e30 )_2[0]=u[0]=0.0; else
        { _2[0]=-0.5; p=x[1]-x[0]; u[0]=3*( (y[1]-y[0])/p - yp1 )/p;
        } x++,y++;
      for( i=1; i<n; i++,x++,y++ )
      { sig=( x[0]-x[-1] )/( x[1]-x[-1] ); p=sig*_2[i-1]+2;
       _2[i]=( sig-1 )/p;
        un = ( y[1]-y[0]  )/( x[1]-x[0] )
            -( y[0]-y[-1] )/( x[0]-x[-1] );
        u[i]=(6.0*un/(x[1]-x[-1])-sig*u[i-1])/p;
      }
      if( ypn>.99e30 )qn=un=0.0; else
        { qn=0.5; p=x[0]-x[-1]; un=3*( ypn-( y[0]-y[-1] )/p )/p;
        }
    } else
    { Real p,*y=_Y+1;
      if( yp1>.99e30 )_2[0]=u[0]=0.0;
                 else _2[0]=-0.5,u[0]=3*( y[0]-y[-1]-yp1 );
      for( i=1; i<n; i++,y++ )
      { p=2/(_2[i-1]+4);
        _2[i]=p/-2;
        u[i] = p*( 3*( y[1] - 2*y[0] + y[-1] ) - u[i-1]/2 );
      }
      if( ypn>.99e30 )qn=un=0.0;
                 else qn=0.5,un=3*( ypn-y[0]+y[-1] );
    }
                           _2[n]=( un-qn*u[n-1] )/( qn*_2[n-1]+1.0 );
    for( k=n-1; k>=0; k-- )_2[k]=_2[k]*_2[k+1]+u[k];
    Allocate( 0,u );
  } return *this;
}
//
//      ������ ������ ����������� � ����� ����������������� �������
//
#define Iph 1.6180339887     // ��� ������� �������
#define Ihp 0.3819660113     //  � �������� ��� �������
                             //
Function& Function::First()  // ����������� ������� �� �������� ���������������
{ if( length>=3 )            //  ��� �������� � ���������� ����������� � �����
  { Real *y=_Y+1; size_t k;  //
    _2 = (Real*)Allocate( length*sizeof( Real ),_2 );
    if( _X )
    { Real *x=_X+1;
      for( k=1; k<length-1; k++,x++,y++ )
       _2[k] = ( (y[1]-y[0])/(x[1]-x[0]) + (y[0]-y[-1])/(x[0]-x[-1]) )/2.0;
       _2[0] = ((_Y[1]-_Y[0])/(_X[1]-_X[0])-_2[1]*Ihp )*Iph;
       _2[length-1] = ((y[0]-y[-1])/(x[0]-x[-1])-_2[length-2]*Ihp )*Iph;
    } else
    { for( k=1; k<length-1; k++,y++ )_2[k] = ( y[1]-y[-1] )/2.0;
       _2[0] = ( _Y[1]-_Y[0]-_2[1]*Ihp )*Iph;
       _2[length-1] = ( y[0]-y[-1]-_2[length-2]*Ihp )*Iph;
    }    met=first;
  } else met=easy; return *this;
}
//
//   �������������� ����� �������������� ������������ �������� �������
//
inline int minmax( int a, int x, int c ){ return a>=x?a : ( c<=x?c : x ); }

//
//   �������� ������������� �� ����������� ��������� �������� �������
//
Real Function::operator()( Real Ar )
{ Real *y,*d, a,B,h=1.0;
  if( _X )
  { int k=find( Ar );
    Real *x=_X+k;
    x=_X+k, y=_Y+k, d=_2+k, h=x[1]-x[0], a=Ar-x[0], B=a/h;
  } else
  { int k=minmax( 0,int( Ar ),length-2 ); y=_Y+k,d=_2+k; a=B=Ar-k;
  }
  if( !B )return y[0];
  if( _2 )
  { if( met==spline )
    { Real A=1-B; return A*y[0] + B*y[1] + ( ( A*A-1 )*A*d[0]
                                           + ( B*B-1 )*B*d[1] )*h*h/6.0;
    } else
    if( met==first )
    { Real c=(y[1]-y[0])/h;
      return y[0] + a*( d[0] + B*( c-d[0] + (B-1)*( d[0]+d[1]-2*c ) ) );
    }
  } return y[0] + B*( y[1]-y[0] );
}
//
//   ������� ����� ������������� ������������� �� ������� p-�������, ���� p>=0
//
Real Function::value( Real Ar, int p )       // ������������ �� ��������
{ if( p<0 )return (*this)( Ar );             // ���� p<0 - ������� ������������
 int i,kb,kf,n=length-1;                     // ����� ���������� ������ �������
  if( !_X )i=minmax( 0,int( Ar ),n ); else i=find( Ar );
  if( p>n )p=n;                              // ������� �� ���� ���� ������
  if( !p  )return _Y[i];  kb=( kf=i+(p/2)+1 )-p;
  if( kf>n )kf=n,kb=n-p;                    // ������������ ����������� ������
  if( kb<0 )kb=0,kf=p;                      // ��� ���������� ������� ��������
 Real po,pi,yo=0.0; i=kb;                   //
  do{ po=pi=1.0;    n=kb;                   //
    do if( i!=n )                           //
    { if( _X )pi*=_X[i]-_X[n],po*=Ar-_X[n]; // ���� ����� ��������, �����
        else  pi*=i-n,po*=Ar-n;             // ���������� ������� ��������
    }  while( ++n<=kf ); yo+=_Y[i]*po/pi;   //(����� if !_X ������� �� ����)
  }    while( ++i<=kf ); return yo;         //
}
//
//   � �������� ����������� ����������� ����������� ����� ������������
//
Real Function::derivative( Real Ar, int p ) // ������ �����������
{ if( p<0 )                                 // ����� ����� ������� � �������
  { Real *y,*d, a,b,q,c,h=1.0;
    if( _X )
    { int k=find( Ar );
      Real *x=_X+k;
      x=_X+k, y=_Y+k, d=_2+k, h=x[1]-x[0], a=Ar-x[0], b=a/h;
    } else
    { int k=minmax( 0,int( Ar ),length-2 ); y=_Y+k,d=_2+k; a=b=Ar-k;
    }
    c=( y[1]-y[0] )/h;
    q=1-b;
    if( _2 )
    { if( met==spline )return c - ( (3*q*q-1)*d[0] - (3*b*b-1)*d[1] )*h/6; else
      if( met==first )return d[0] + b*( 2*(c-d[0]) - ( 2*q-b )*(d[0]+d[1]-2*c) );
    } return c;
  }                                         //
 int i,l,kb,kf,n=length-1;                  // ����� ���������� ������ �������
  if( !_X )i=minmax( 0,int( Ar ),n ); else i=find( Ar );
  if( p>n )p=n;                             // ������� �� ���� ���� ������
  if( !p  )return 0; kb=( kf=i+(p/2)+1 )-p; //
  if( kf>n )kf=n,kb=n-p;                    // ������������ ����������� ������
  if( kb<0 )kb=0,kf=p;                      // ��� ���������� ������� ��������
 Real p2,pi,y1=0,s1; i=kb;                  //
  do{ pi=1.0; s1=0;  n=kb;                  //
    do if( i!=n )                           // ���� ����� ��������, �����
    { p2=1.0;                               //
      pi *= _X ? _X[i]-_X[n] : i-n;         // ���������� ������� ��������
      for( l=kb; l<=kf; l++ )if( l!=i && l!=n )p2 *= _X ? Ar-_X[l] : Ar-l;
      s1 += p2;
    } while( ++n<=kf ); y1+=_Y[i]*s1/pi;    //
  }   while( ++i<=kf ); return y1;
}
//
//   ������� ������� ��������� ������� ����������� ������� � ���������
//
void Function::extrem( Real &xMin, Real &yMin, Real &xMax, Real &yMax )
{ Real *x=_X,*y=_Y;
  for( size_t i=0; i<length; i++,x++,y++ )
  { if( i ){ if( yMin > *y )yMin = *y; else if( yMax < *y )yMax = *y;
             if( xMin > *x )xMin = *x; else if( xMax < *x )xMax = *x;
        } else { xMax = ( xMin = *x )+1.0e-12;
                 yMax = ( yMin = *y )+1.0e-12;
} } }
