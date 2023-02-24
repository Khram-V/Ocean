//
//      BASIS.cpp                               Ver 1.0  90.11.20
//
//      ��������� ���������� ��������������� ��������������� ������
//             � ������ �� ��������������� ��������������� ��������
//
//          � xX  yX �         � Xx  Yx �       Y   y         Def
//      M = �        �     m = �        �       A   A         --�
//          � xY  yY �         � Xy  Yy �       �   �  A �a    -�
//                                              �   �   /       �
//           -1   conjg( M )   � yY -yX �       �   �  /
//      m = M   = ---------- = �        � / �M� �   � /
//                 det( M )    �-xY  xX �       �   � r
//                                              �   +------------>x
//      A = Re + a � M                          �  Re
//      a = ( A-Re ) / M                        �
//                                              L==================>X
#include "..\Matrix\Basis.h"
//
//   ����� ����������� ����������� ���������� ������,    ( 93.11.23 )
//         ��� ������������ ��������� �� Basis.
//    Area                                                    Lu  Ru
//    ( Aim     - �������������� ������������� ��������      3-----�4
//      Corners - ������ ������� ����� ��������������� ���:   � Cs �
//    )         - �� ������ ������� ��� �������� �������     1L-----2
//                                                            Ld  Rd
//                                                          Aim
Basis_Area::Basis_Area():Basis(),B( (Field){ 0,0,1,1 } ),ky( 0.0 ),kx( 0.0 )
{   S.Axis( (Point){ 1.0,1.0 },-y,-x );
}
Real Basis_Area::Area( Field Aim, Point Ld, Point Rd, Point Lu, Point Ru )
{  Real A=Basis::Axis( Ld,Lu-Ld,Rd-Ld ); B=Aim;
   Point D=Basis::Goint( Ru ); kx=D.x-1;
                               ky=D.y-1;
  return ( A+S.Axis( Ru,Rd-Ru,Lu-Ru ) )/2.0;
}
//    Area
//    ( Aim     - �������������� ������������� ��������     Lu�
//      Corners - ������ ������� ����� ��������������� ���:   �Crs[3]
//    )         - �� ������ ������� ��� �������� �������    Ld+---�Rd
//                                                            Aim
Real Basis_Area::Area( Field Aim, Point Ld, Point Rd, Point Lu )
{ return Area( Aim,Ld,Rd,Lu,Rd+Lu-Ld );
}
Real Basis_Area::Area( Field Aim, Point Ld, Point Rd ) // ������� � ����������
{ Real s=Aim.Ly/Aim.Lx;                                 // �� ������� ������ !!
  point ld=(Point)Ld,
        rd=(Point)Rd,lu={ int(ld.x+s*(rd.y-ld.y)),int(ld.y-s*(rd.x-ld.x)) };
  return Area( Aim,Ld,Rd,lu );
}                                              //
Point Basis_Area::Goint( Point P )             // ������� � ��������� ����� Aim
{ Point A=S.Goint( P ),                        //
        a=Basis::Goint( P ); A.x=B.Jx+B.Lx*( a.x/( A.x+a.x ) );
                             A.y=B.Jy+B.Ly*( a.y/( A.y+a.y ) ); return A;
}                                              //
Point Basis_Area::Goext( Point p )             // ������� � ����������� �����
{ Point A,a; A.x=( p.x-B.Jx )/B.Lx;            //  It's Right !!!
             A.y=( p.y-B.Jy )/B.Ly; a.x=A.x*( 1+kx*A.y );
                                    a.y=A.y*( 1+ky*A.x );
  return Basis::Goext( a );
}
//   ��������� ���������� ��������������� ��� ��������������� ������
//   ( �������� ���������� )                                  Ver.1.0 90.11.20
//   E U L E R - ����������� ���������� ������ ������ �����������
//           - ���� ������������� �� ��������� (������������ ��� Y) � ��������
//     Re    - �������������� ���������
//     angle - ���������� ��������� � �������� (�� Y ������ �� ������� �������)
//     size  - ������� �������� ������������� �������
//
Real Basis::Euler( _Point Re, Real angle, _Point size )
{ Real x=sin( angle*=M_PI/180.0 ),y=cos( angle );
  return Axis( Re,size.y*(Point){ x,y },size.x*(Point){ y,-x } );
}
//
//  ����� ����� ����������� ���� ������ �����, ���� ���� ������� �� ��������
//
int Cross( Point L11, Point L12, Point L21, Point L22, Point& Cx )
{ Base Det;
   if( fabs(Det.Axis( L22-L21,L12-L11 ))<1.e-12 )return -2; // -- �����
   L21=L22=Det.Goint( L22-L11 );                           // ����� �����������
   L21.y=0; Cx=Det.Goext( L21 )+L11;
   if( L22.x<0.0 || L22.y<0.0 )return -1;
   if( L22.x>1.0 || L22.y>1.0 )return 1; return 0;
}
//
//  ���������� ������������ �� ����� ����� � ������ �����
//
#if 0
Real Value( float **F, Real X, Real Y, int Nx, int Ny )
{                                              // ������ �������
 int y=Y,x=X;                                  // ��������� �����
 double dy,dx,yd,xd;                           //
  if( y<-1 || x<-1 || y>Ny || x>Nx )return 0;  // ����� ����� +���- 10,001 ��?
  if( y<0 )y=0; else if( y>=Ny-1 )y=Ny-2;      //
  if( x<0 )x=0; else if( x>=Nx-1 )x=Nx-2;      //
  dy=Y-y,yd=1.0-dy,                            // ������ ��������� ��������� �
  dx=X-x,xd=1.0-dx;  F+=y;                     //  �������� �� ������� �������
  return ( xd*( F[0][x] ) + dx*( F[0][x+1] ) )*yd
       + ( xd*( F[1][x] ) + dx*( F[1][x+1] ) )*dy;
}
#endif
//
//   ������ ������������ �� ��������� ���� � ������� ������ ������ �������
//
Real Sleek( float *Y, Real X, int Nx )        // �� ����� ����� - �� �� �������
{ int x=X;                              Nx--; // ������ ������� ��������� �����
  if( x<0 )x=0; else if( x>=Nx )x=Nx-1; Y+=x; // � ����� ��������� �� ������
 Real a=X-x,c=Y[1]-Y[0],f1,f0;
  if( x>0     )f0=( Y[1]-Y[-1] )/2;
  if( x<Nx-1  )f1=( Y[2]-Y[0] )/2;
  if( !x      )f0=( c-f1*0.3819660113 )*1.6180339887;     // ����� � ����������
  if( x==Nx-1 )f1=( c-f0*0.3819660113 )*1.6180339887;     // "�������� �������"
//!       D = f0 + a*( 2*(c-f0) + (3*a-2)*(f0+f1-2*c) );  // ��� - �����������
  return Y[0] + a*( f0 + a*( c-f0 + (a-1)*(f0-2*c+f1) ) );
}
Real Value( float **F, Real X, Real Y, int Nx, int Ny )
{ int y=Y-1.0; float FY[4];                   // ������ ������� ��������� �����
//if( y<-1 || x<-1 || y>Ny || x>Nx )return 0; // ����� ����� +���- 10,001 ��?
  if( y<0 )y=0; else if( y>Ny-4 )y=Ny-4;      //
  for( int k=0; k<4; k++ )FY[k]=Sleek( F[y+k],X,Nx );
  return Sleek( FY,Y-y,4 );
}

