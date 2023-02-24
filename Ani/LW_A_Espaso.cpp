//
//   ESPASO.c                                /90.11.01-92.08.06/
//
//      One step in the Time for Ocean wave propagation
//      �������� �������� ���������� ������ ���� ��������������
//
//      ������� ������������ ����� �� [1]:
//      _         _  _
//      s = Cd��|w|�w(+20�)    ���������� ����� � ��������� �=20� ��
//                              ����������� ���������� ����� (10� ������)
//      _    �    _  _
//      u += �Cd�|w|�w(+�) dt   �������� � ������ �� �������
//           �                  �������������� �����
//      _
//      s  - ������ �������������� ����������
//      w  - ������ �������� ������������ ����� (�� ������ ~ 10m)
//      Cd - ������������ �����������
//             Cd~0.001, a~20�  - ���������� �������
//             Cd~0.002, a~ 0�  - ������������ ������� (������ � �����)
//             Cd~.0001, a~40�  -//- ������������ (�������� �����������)
//      �  - ��������� ��������
//
//      [1] - �.����, �������� ��������� � ������, �-2, �,���,1992
//
#include "..\Ani\LongWave.h"
        //
        // WaveIs: 1 � ����� ������ �������� �����
        //         2 � ������������� ������������� ��������� �����
        //         4 � ���� �������������
        //         8 � �� ����� �������������� ������� Wt+Rand( Wk );
        //      1.00 � Spherical Earth    \ .
        //      2.00 � Coriolis  Forces    \ ��������������
        //      4.00 � Viscosity Friction  / �������� ����������
        //      8.00 � Atmosphere Cyclone /
        //     10.00 - ������� ������� �������� ������ ����������
        //
inline Real cub( _Real a ){ return a*a*a; }

void TWave::ESpaso()
{
 const Real cFe=cos( Chart.Fe );
 Real D,d,fi,W,fy=Tstep/Dy,
               fx=Tstep/Dx;
 unsigned Mode=WaveAs>>8;
 const bool Globus=Active&as_Globus?true:false;  // ������ ��������� ����������
//
//  1: ���������� ������ ���������� ���� ������ ���� (��������� �������������)
//     Compute field of sea elevation
//
  if( Mode&1 )fy*=cFe,
              fx*=cFe;                     // Cosine of latitude of scale

#pragma omp parallel for                   // firstprivate( iGlob )
  for( int y=1; y<My-1; y++ )              // y := [ 1..Ny-2 ]
  { for( int x=Globus?0:1; x<Mx-1; x++ )   // x := [ 1..Nx-2 ]
    if( Dp[y][x]>DZ )                      //
    { Real W = ( Uy[y+1][x]-Uy[y][x] )*fy  //
             + ( Ux[y][x+1]-Ux[y][x] )*fx; //
      if( Mode&1 )W/=_Fc[y][x];            // Spherical coordinates
                  Hg[y][x] -= W;           //
    }
    if( Globus )Hg[y][Mx-1] = Hg[y][0];    // ��������� ������ ����������
  }
#if 0
//
//  3: ������������ ��������
//
   for( y=1; y<Ny-2; y++ )
   for( x=1; x<Nx-2; x++ )
   {
    double wX,wY;  wX =Ux[y][x]*( Ux[y][x+1]-Ux[y][x] )*fx;
       if( y<Ny-2 )wX+=Uy[y][x]*( Ux[y+1][x]-Ux[y][x] )*fy;
                   wY =Uy[y][x]*( Uy[y+1][x]-Uy[y][x] )*fy;
       if( x<Nx-2 )wY+=Ux[y][x]*( Uy[y][x+1]-Uy[y][x] )*fx;
                       Ux[y][x]-=wX;
                       Uy[y][x]-=wY;
   }
#endif
//
//  2: ������ ������ ���� �������� �� ��������� ������� (��������� ��������)
//     Compute the new flow field
//
 const Real g2=9.8106/2.0; fy *= g2;
                           fx *= g2; fi=1.0;

#pragma omp parallel for private( D,d ) firstprivate( fi )
  for( int y=1; y<My; y++ )
  { for( int x=1; x<Mx; x++ )
    if( (D=Dp[y][x])>DZ )
    { Real h=Hg[y][x];                      //
      if( Mode&1 )fi=_Fc[y][x];             // Spherical coordinates
//    if( x<Mx-1 )                          //
      if( ( d=Dp[y-1][x] )>DZ )             // Vy := [1..Nx-2,1..Ny-1]
      { Real W = h-Hg[y-1][x];              //
        if( Mode&8 )W-=Fa[y-1][x]-Fa[y][x]; // �������� ������������ ��������
        if( Mode&1 )W/=fi;                  // ����������� ����������
                    Uy[y][x]-=fy*( D+d )*W; //
      }                                     //
//    if( y<My-1 )                          // Vx := [1..Nx-1,1..Ny-2]
      if( ( d=Dp[y][x-1] )>DZ )             //
      { Real W = h-Hg[y][x-1];              //
        if( Mode&8 )W-=Fa[y][x-1]-Fa[y][x]; // �������� ������������ ��������
        if( Mode&1 )W/=fi;                  // ����������� ����������
                    Ux[y][x]-=fx*( D+d )*W; //
      }
    } //if( Globus )Uy[y][0]=Uy[y][Mx-1],
      //            Ux[y][0]=Ux[y][Mx-1];
  }
//
// +3: Correction for viscosity and Coriolis forces of Earth rotation
//
// #define wE 0.000663145596216231  180 / Pi /1440 /60 = w
// #define wD 0.001326291192432462  180 / Pi /1440 /60 = 2*w
// Coriolis constant: 2(w)=2��/1440/60 = 7.272205217e-5
//
  if( aM.W==0. )Mode&=~8;     //
  if( Mode&0xE )              // ������������ ������� �������� �����
  { Point V={ 0,0 };          //
  #pragma omp parallel for private( d,D ) firstprivate( fi,V,W )
    for( int y=1; y<My; y++ )
    for( int x=Globus?0:1; x<Mx; x++ )
    if( ( D=Dp[y][x] )>DZ )
    { if( Mode&2 )fi=_Fs[y][x]*wD; // *sqrt( 1-sqr( _Fc[y][x] ) );// 2�w�sin(�)
      if( Mode&4 )W=Kvs*hypot( Uy[y][x],Ux[y][x] );         // Viscosity
      if( Mode&8 )V=Atmosphere_Stress( y,x );               // Wind
//    if( x<Mx-1 )
      if( ( d=Dp[y-1][x] )>DZ )
      { if( Mode&2 )Uy[y][x] -= Ux[y][x]*fi;                // Coriolis forces
        if( Mode&4 )Uy[y][x] -= Uy[y][x]*W/cub((D+d)/2.0);  // Viscosity
        if( Mode&8 )Uy[y][x] += V.y;                        // Wind
      }
//    if( y<My-1 )
      if( (d=Dp[y][x>0?x-1:Mx-1])>DZ )
      { if( Mode&2 )Ux[y][x] += Uy[y][x]*fi;                // ���������� ����
        if( Mode&4 )Ux[y][x] -= Ux[y][x]*W/cub((D+d)/2.0);  // ��������� ������
        if( Mode&8 )Ux[y][x] += V.x;                        // ���������� �����
    } }
  }
//
// +4: �������� ����������� ������� ������������� ���� � ������ (???)
//
  if( WaveAs&4 )        //
  { fi=cFe;             // ���������� �������
    if( !Hf )           //
    { Hf=(Real**)Allocate( My,Mx*sizeof( Real ) );         // ����������
      if( Kt>1 )                                           // ������������� ���
      { Init_Sun_and_Moon( Instant + Tstep/-3600.0 );      // ����������� ����
       #pragma omp parallel for
        for( int y=0; y<My; y++ )
        for( int x=0; x<Mx; x++ )Hf[y][x]=Geo_Sun_and_Moon( _Fs[y][x],_Fc[y][x],_La[y][x] );
      } else                                    //
        for( int y=0; y<My; y++ )               // ��������� �������������
        for( int x=0; x<Mx; x++ )Hf[y][x]=0;    // � �������� ��������� �������
    }                                           //
    Init_Sun_and_Moon( Instant );               // ��������� ���� � ����� [���]
//  if( Mode&1 )fi=cFe;                         //
   #pragma omp parallel for private( d,D,W ) firstprivate( fi ) // nowait
    for( int y=0; y<My; y++ )                   // ���������� ����
    for( int x=0; x<Mx; x++ )                   // ���������� �������������
    if( (D=Dp[y][x])>DZ  )                      //
    { Real h = Geo_Sun_and_Moon( _Fs[y][x],_Fc[y][x],_La[y][x] );
      Hg[y][x]+=( W=h-Hf[y][x] );
      Hf[y][x] = h;
      if( x && y ) //&& y<Ny-1 )
      { int z=x>0?x-1:Mx-1;
        if( (d=Dp[y][z])>DZ )                           // !! ???????????????
        { if( Mode&1 )fi=cFe*(_Fc[y][x]+_Fc[y][z])/2.0; // *tan( _Fi[y][x] );
          Ux[y][x]-= W * sqrt( 9.8106*(D+d)/2.0 ) * fi; //  sin( � )/cos( � )
        }
        if( (d=Dp[y-1][x])>DZ )
        { if( (Mode&1)==0 )fi=cFe;
          Uy[y][x]+= W * sqrt( 9.8106*(D+d)/2.0 )
               * fi * sin( acos( (_Fc[y][x]+_Fc[y-1][x])/2.0 ) ); //! ���������
      } }
  } }
//
//      � ������ ������� ������� ��������
//        ����������� ������� ������� ���� ���������
//
  if( Globus )
  for( int y=0; y<My; y++ )Uy[y][0]=Uy[y][Mx-1],
                           Ux[y][0]=Ux[y][Mx-1];
//
//! ����������� ������ ���� �� ������� (����� ������ ����� �������)
//
    Kt++;
    VideoM&=~2;
    Tlaps += Tstep;
    Instant += Tstep/3600.0;                          // ��������� ���� � Event
}
//
//   ��������������� ����
//   ������ ��������� ������ � ���� ������������ �������� � ��������
//
void TWave::Init_Sun_and_Moon( _Event D ) // ��������� ���� � ����� [���]
{ //extern long _timezone;        // ts+s = th+h = Ts -  �������� �����
 double s,                        // ������� ����
        h,                        // ������� ������
        T;                        // - �����
//D += double(_timezone)/3600.0;  // ������� ��������� ����������� �����
                                  // ����������� (�������������) �����
  T = ( (D.T-12.0)/24.0           // ������������� �� 31 ������� 1899 ����
       + D.D-2415020.0            //  � �������� ������������ �������
      ) / 36525.0;                //   � ��������� ���������  ~~~~~~~
                                  //              ~~~~~~~~~~~~~~~~~~~
  s = Angle( 270.43659 +T*( 481276.89057 +T*(0.00198 +T*0.000002)) )/Radian;
  h = Angle( 279.69668 +T*(  36000.76892 +T* 0.00030 ) )/Radian;
  Sun.x = Pi*(D.T+12.0)/12.0;     // ������� ��������� �����  [���]
  Moon.x = Sun.x + h-s;           // ������� ������ �����     (t=T+h-s)
  Sun.y  = 23.450833*sin( h )/Radian;
  Moon.y = Sun.y + 5.1467*sin( s )/Radian;
}
//
//          �������� ���-����������
//
Real TWave::Geo_Sun_and_Moon              // �������� (�������) �������������
( Real Fs, Real Fc, Real La )             //   h = Om/g = Hm*( cos�Z - 1/3 )
{ Real cM = Fs * sin( Moon.y ) + Fc*cos( Moon.y )*cos( La+Moon.x ),  // ���� �
       cS = Fs * sin( Sun.y  ) + Fc*cos( Sun.y  )*cos( La+Sun.x );   // ������
  return 0.81*cM*cM                                  // ����   .54 x 1.5 = .81
      + 0.375*cS*cS - 0.395;                         // ������ .25 x 1.5 = .375
}
#if 0
//
//          �������� ���-����������
//
double TWave::Geo_Sun_and_Moon              // �������� (�������) �������������
( double Fc, double La )                    //   h = Om/g = Hm*( cos�Z - 1/3 )
{ double Fs = sqrt( 1-Fc*Fc ),               //
        cM = Fs * sin( Moon.y ) + Fc*cos( Moon.y )*cos( La+Moon.x ),  // ���� �
        cS = Fs * sin( Sun.y  ) + Fc*cos( Sun.y  )*cos( La+Sun.x );   // ������
  return 0.81*cM*cM                                  // ����   .54 x 1.5 = .81
      + 0.375*cS*cS - 0.395;                         // ������ .25 x 1.5 = .375
}_______________________________________________________________________________
 return                                                          //
  0.125 * cos( (La+Sun.x)*2.0 )*( cos( (Sun.y-Fi)*2.0 )+1 )      // ������ 0.25
 + 0.27 * cos( (La+Moon.x)*2.0 )*( cos( (Moon.y-Fi)*2.0 )+1 );   // ����   0.54
                                                                 //
        fi=1.0;                         //
    for( y=1; y<Ny; y++ )               // Vx := [1..Nx-1,1..Ny-2]
    for( x=1; x<Nx; x++ )               // Vy := [1..Nx-2,1..Ny-1]
    if( ( D=Glubina[y][x] )>Dz )        //
    {     h=Hf[y][x];                   //
      if( Mode&1 )fi=Fk[y][x];          // Spherical coordinates
      if( x<Nx-1 )                      //
      if( ( d=Glubina[y-1][x] )>Dz )Uy[y][x]+=fy*( D+d )*(h-Hf[y-1][x])/fi;
      if( y<Ny-1 )
      if( ( d=Glubina[y][x-1] )>Dz )Ux[y][x]+=fx*( D+d )*(h-Hf[y][x-1])/fi;
    }
��������� �����
???   Hg[y][x] -= P.y * Dt/10000;
      if( !x || x==Nx-1 || !y || y==Ny-1 )P.y*=1.33;
//    ---------
//                           //
void Astronomy( long   Day,  // ����
                double Time, // ����������� �����
                double &s,   // ������� ����
                double &h,   // ������� ������
                double &p,   // ������� ������� �������
                double &N,   // ������� ����������� ���� ����
                double &ps   // ������� ������� ������
)                            // ts+s = th+h = Ts - �������� �����
{ Day -= 2415020L;           // ������ �� 31 ������� 1899 ����
  Time = ( (Time-12.0)/24.0  //  � �������� ������������ �������
           + Day )/36525.0;  //   � ��������� ���������
                             //
  s =( 270.43659 + Time*(481276.89057 + Time*(0.00198 + Time*0.000002)))*Pr;
  h =( 279.69668 + Time*( 36000.76892 + Time* 0.00030                 ))*Pr;
  p =( 334.32956 + Time*(  4069.03403 - Time*(0.01032 + Time*0.00001 )))*Pr;
  N =( 259.18328 + Time*( -1934.14201 + Time*(0.00208 + Time*0.000002)))*Pr;
  ps=( 281.22083 + Time*(     1.71902 + Time*(0.00045 + Time*0.000003)))*Pr;
}
#endif
