//
//  LW_A_EMeteo.cpp        Сервис программы моделирования
//                         Метеорологического циклона
//=====================================================================
//                                              Ver 1.0 from 94.07.25
//
#include <String.h>
#include "..\Ani\LongWave.h"
//
//   Статический уровень моря под действием атмосферного давления
//
static Real  SF;          // Масштабный коэффициент реальных расстояний
static Point CI;          // Индексные координаты текущего центра циклона

Real TWave::Atmosphere_Level( int y, int x )
{ Real Ds;
  if( WaveAs&0x100 ) // учет сферичности Земли
  { Real C; Point P={ x,y }; Chart.ItD( P ); Sphere( aM.G,P,C,Ds ); Ds*=1000.0;
  } else Ds=SF*hypot( ( CI.x-x )*Dx,( CI.y-y )*Dy );
  return aM.H*Pressure_Profile( Ds/aM.R );
}
static int yU,yD,xL,xR; // Граничные индексы зоны действия циклона
//
//   Вектор скорости ветра преобразованный в приращение потока
//
Point TWave::Atmosphere_Stress( int y, int x )
{ if( y<=yU && y>=yD )
  if( x<=xR && x>=xL )
  {
   Point P,V;
   Real  C,D,A=( Chart.Cs-90 )/Radian; // 110 )*Pr; // Здесь 20° внутрь циклона
    if( aM.W<0   )A=-A;
    if( aM.G.y<0 )A=-A;
    if( x<Mx-1 )
    { P.y = Real( y )-0.5; P.x=x;
      if( WaveAs&0x100 )               // учет сферичности Земли
      { Chart.ItD( P ); Sphere( aM.G,P,C,D ); D*=1000.0; C/=Radian; } else
      { P-=CI; P.y*=Dy; P.x*=Dx; D=abs( P )*SF; C=Ph-arg( P );
      } V.y = Wind_Profile( D/aM.R );
        V.y*=V.y*cos( C+A );
    }
    if( y<My-1 )
    { P.x = Real( x )-0.5; P.y=y;
      if( WaveAs&0x100 )               // учет сферичности Земли
      { Chart.ItD( P ); Sphere( aM.G,P,C,D ); D*=1000.0; C/=Radian; } else
      { P-=CI; P.y*=Dy; P.x*=Dx; D=abs( P )*SF; C=Ph-arg( P );
      } V.x = Wind_Profile( D/aM.R );
        V.x*=V.x*sin( C+A );
    } return V*( aM.W*aM.Avs );
  }   return (Point){ 0.0,0.0 };
}
//
//   Оперативная отметка местоположения циклона
//
//static void Meteo_Mark( const METEO& aM )
//{ color( CYAN );
//  setwritemode( XOR_PUT );  Geo_Circle( aM.G,aM.R/1000 );
//  setwritemode( COPY_PUT );
//}
//
//   Пересчет атмосферных полей скорости и градиентов уровня
//
void TWave::EMeteo()
{ if( Na>0 )                     //
  { int K;                       // Номер интервала на маршруте
    Real To=0,Ti=0;              // Время на опорных точках
    if( Na>1 )                   //
    { if( Tlaps<Ma[0].T || Tlaps>Ma[Na-1].T ){ WaveAs&=~0x800; return; }
      for( K=0; K<Na; K++ )if( !K )Ti=Ma[0].T;
                     else { To=Ti;
                            Ti=Ma[K].T; if( To<=Tlaps && Ti>Tlaps )break; }
      if( K>=Na )K=Na-2; else K--;
//    if( (VideoM&2)==0 )Meteo_Mark( aM );
      Ti=(Tlaps-To)/(Ti-To);       // Поправка на смещение внутри интервала
      To=1.0-Ti;                   //
//    aM.G = ( To*Ma[K].G )+( Ti*Ma[K+1].G );
      aM.G.y=ReLine( &(Ma[0].G.y),Na,K+Ti,sizeof( METEO ) );
      aM.G.x=ReLine( &(Ma[0].G.x),Na,K+Ti,sizeof( METEO ) );
      aM.R = ( To*Ma[K].R   )+( Ti*Ma[K+1].R   );
      aM.W = ( To*Ma[K].W   )+( Ti*Ma[K+1].W   );
      aM.H = ( To*Ma[K].H   )+( Ti*Ma[K+1].H   );
      aM.Avs=( To*Ma[K].Avs )+( Ti*Ma[K+1].Avs );
      aM.T=Tlaps;
//    Meteo_Mark( aM );
    } else
    { aM=Ma[0]; //if( VideoM&2 )Meteo_Mark( aM );
    }
   Chart.DtI( CI=aM.G ); SF=cos( aM.G.y/Radian )/cos( Chart.Fe );
   Point P;
   int y,x;                       Ti=aM.R/1000.0;
      P=Sphere( aM.G,Chart.Cs,    Ti ); Chart.DtI( P ); yU=int( P.y+2 );
      P=Sphere( aM.G,Chart.Cs+90, Ti ); Chart.DtI( P ); xR=int( P.x+2 );
      P=Sphere( aM.G,Chart.Cs+180,Ti ); Chart.DtI( P ); yD=int( P.y-1 );
      P=Sphere( aM.G,Chart.Cs-90, Ti ); Chart.DtI( P ); xL=int( P.x-1 );
    if( yU>My )yU=My;
    if( xR>Mx )xR=Mx;
    if( yD<0  )yD=0;
    if( xL<0  )xL=0;
    for( y=0; y<My; y++ )memset( Fa[y],0,Mx*sizeof( Real ) );
    if( (WaveAs&0x800)==0 )
    {    WaveAs|=0x800;
//    for( y=0;  y<Ny; y++ )memset( Fa[y],0,Nx*sizeof( Real ) );
      for( y=yD; y<yU; y++ )
      for( x=xL; x<xR; x++ )
       if( Dp[y][x]>DZ )Hg[y][x]-=( Fa[y][x]=Atmosphere_Level( y,x ) );
    } else
      for( y=yD; y<yU; y++ )
      for( x=xL; x<xR; x++ )
       if( Dp[y][x]>DZ )Fa[y][x]=Atmosphere_Level( y,x );
} }
