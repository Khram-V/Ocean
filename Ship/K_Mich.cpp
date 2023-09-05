#include <Math.h>
#include <StdLib.h>
#include "..\Ship\Hull.h"
                        //
const  double g=9.8106, // ��������� ���������� �������
              ro=1.025; //  � ��������� ������� ����
static double WL;       // ����� ���������� �����
                        //
double Plaze::Amplint( const double &La )
{   int i,j;                  //
 double X,Z,Wk=2.0*M_PI/La,   // �������� �����
            Mw=sqrt( WL/La ); // ����� ����
 Point A={ 0,0 },W;           // ������������ ���������
  for( i=Nx-1; i>=0; i--  )   //
  { Z=0;
    W=0;
    X=Wk*dX*(Nx-1-i)/Mw;
    for( j=Nz-1; j>=0; j-- )
    { W += exp( (Point){ -Z,X } )*QV[j][i];
      Z += Wk*dZ;
    } A += W;
     Wx[i]=norm( A )*Mw/sqrt( Mw*Mw-1 )/La/La;
  } return norm( A )*Mw/sqrt( Mw*Mw-1 )/La/La;
}
double Plaze::QG4( const double &Xl, const double &Xu )
{ double A = 0.5*( Xu+Xl ),B=Xu-Xl,C,D;
         C = .4305681557970263 * B;
         D = .1739274225687269 * ( Amplint( A+C )+Amplint( A-C ) );
         C = .1699905217924281 * B;
  return B * (D+.3260725774312731 * ( Amplint( A+C )+Amplint( A-C )));
}
Real Plaze::Michell( Real Fn )
{   int i;      //
 double Rw=0.0, // ������� �������� �������������
          Vo,   // �������� � �/�
           l;   // �������� ��������������
                //
  Vo = Fn*sqrt( g*Lwl );                //
  WL = 2.0*M_PI * Vo*Vo/g;              // ����. ����� �����
#if 0                                   //
  for( j=0; j<Nz;   j++ )
  for( i=0; i<Nx-1; i++ )               //
  { QV[j][i]=( Y[j][i+1]-Y[j][i] )*dZ;  // ������������� ���������
//  QV[j][i] = dX*dZ*X; //*Wide;        // � ������������� �������
//  YY[j][i] =( Y[j][i]+Y[j][i+1] )/2.0;
//  if( j>Nz ) //>0 )
//  { Z = ( Y[j][i]-Y[j-1][i] )/Dz;
//    QV[j][i] *= sqrt( 1+ X*X+ Z*Z );  //= sqrt( 1+ X*X );
//  }
  }
#endif
                                        //
 const int la=24;                       // ���������� ��������� �� ������
 static                                 //
 double La[la+1]={ 0.0 };               // ������� ����� �����
       l=double( 1.0 )/la;              //
  for( i=1; i<=la; i++ )La[i]+=La[i-1]+sin( M_PI_2*l*i )*sqrt( 1.0-l*i );
  for( i=1; i<=la; i++ )La[i]*=WL/La[la];
  for( i=1; i<=la; i++ )if( La[i]>dX )
                        Rw+=QG4( La[i-1],La[i] );
 return 4*g*ro*Rw;
}
#if 0
struct Michell{ Real Vo;        // �������� � �/�
                Real Vl;        // �������������
                Real Sf;        // ��������� �����������
                Real Rw;        // �������������
                Real Cw;        // ����������� �������������
} Michell_Results={ 0,0,0,0 };  //
  Michell_Results.Vo=Vo;        // �������� � �/�
  Michell_Results.Vl=Vl;        // �������������
  Michell_Results.Sf=Sf;        // ��������� �����������
  Michell_Results.Rw=Rw;        // �������������
  Michell_Results.Cw=Cw;        // ����������� �������������

  for( j=0; j<Nz; j++ )                         //
  { Z = Dz*Wk*j;        if( Z>69 )continue;     //
    X = Dx/2;           // -0.5*Length;         // �� ���� � �����
    for( i=0; i<Nx-1; i++)                      //
    { A += QV[j][i] * exp( complex( -Z,Wk*X/Mw ) );
      X += Dx;
  } }

#endif
