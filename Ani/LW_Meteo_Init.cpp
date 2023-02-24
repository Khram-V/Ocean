//
// T_SMeteo.cpp         ���������� � ���������������� �������������
//                      ����������� �����-������� (�������) � �.�.
//=====================================================================
//
//  �� ������ ������������ ��� ������� �����, ���������� ������� ��������
//      �������. � ������� ������� �� ���������� ��� ��������� "����"
//      �� ������ ������������ ����� ����� ����������� ������� �����.
//
//  Enter    ���� ������� �������������� ���������� �������
//   -------------------------------�
//   �Ratio Time of Start 0d 1�12'6"� ����� ���. ������ ����������
//   �Geo. Coordinates 45�39'149�45'� �������������� ���������� �������
//   �Cyclone Diameter 1500 km      � ������� �� "�������" �������� �����
//   �Cyclone Pressure 25.5 water'cm� ������� �������� ��� ������� ������
//   �Maxima Wind Speed  30 m/sec   � ������������ �������� �����
//   �Surface Stress Factor 0.0020 �� �����. �������������� ����������
//   L-------------------------------
//
//  Space (������) - ����� � ��������� ��������� � ����� �� ��������
//              �������� �������.
//  +,-      ������� ����� � ������ ������������ ������� �������
//              ������� "+|-|Space|Insert" �������� ����� ��������������
//              ��� ������������ �����, ��� ���������� ��������� ������
//              ���������� �������.
//  Insert   ��������� ����� ����� �� �������� ������� ��� �������������
//              ������������ �����. ���� ����� ����� ��������� �� �������
//              � ����� �� ����� �� �������� �������, �� �������� ��������
//              ������ ������ � ������� �� �����������.
//  Delete   �������� ����� �� �������� �������, ���� ��� ����� ��������
//              ��� �����������.
//  "S|s"    ����������/���������� �������� �������� ����� ������� �
//              ������������ � ��������� ����� ����������� �� �����������.
//
//  F1       ��������� ��������������� ���� � ������� ���������
//              �������������� ����� �������.
//  F2       ����� �� �������� ���� ��������� �������� ����������
//  F3       ����� �� ���� ���������� ���� �������� ����� ���������,
//              ������� ������������ � ������� ������� �� �������.
//  F4       ����� �� ���������� ��������� ����������������, �������
//              �������������� ��� ����������� �������������� �������
//              �� ��������� ��������.
//  Esc      ����� �� ��������� ����������� �����-��������� ������� ����.
//
#include <String.h>
#include <StdLib.h>
#include "..\Ani\LongWave.h"
                                  //
static bool Fix=false;            // ������� ������� ����� �� ����� �� ��������
static METEO Met;                 // ����������� ����� ��� ���������� ��������
                                  //
void Meteo_Mark( METEO& M )       // ������������ ����� � �������� ��������
{ Point P; double A,W,R=M.R/1000.0;
  Geo_Circle( M.G,R );
  if( Fix )setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  if( M.W )                     //
  { W=R*0.4;                    // ������� ������� �������� �����
    if( M.G.y<0 )W=-W;          // � ������ ������ � �����
    if( M.W<0   )W=-W;          //
    for( A=1.0; A>-1.01; A-=0.1 )
    { P=Sphere( M.G,0,R*A );
      P=Sphere( P,-90,(A<0?-W:W)*Wind_Profile( A<0?-A:A ) ); Chart.DtI( P );
      if( A>0.99 )moveto( P );
           else   lineto( P );
  } }                           //
  if( M.H )                     // ������� ������� �������� ��������
  { W=R*0.4;                    //
    for( A=-1.0; A<1.01; A+=0.1 )
    { P=Sphere( M.G,90.0,R*A );
      P=Sphere( P,M.H>0?180.0:0.0,W*Pressure_Profile( A<0?-A:A ) );
      Chart.DtI( P );
      if( A<-0.99 )moveto( P ); else lineto( P );
  } }                           //
}                               // ������� ��������� �����
static void Meteo_Mark()        //
{ color( Met.W==0.0 && Met.H==0.0?LIGHTGRAY:WHITE );
           setwritemode( XOR_PUT );
  if( Fix )setlinestyle( SOLID_LINE,0,THICK_WIDTH ); Meteo_Mark( Met );
           setwritemode( COPY_PUT );
}
static int meteo_sort( const void *a, const void *b )
{ return ((METEO*)a)->T < ((METEO*)b)->T ? -1:1;
}                               //
void TWave::Meteo_Marks()       // ���������� � ������� �������� �������
{ int k;                        //
  if( Na>1 )qsort( (void*)Ma,Na,sizeof( METEO ),meteo_sort );
  setwritemode( XOR_PUT );
  setlinestyle( SOLID_LINE,0,THICK_WIDTH ); color( LIGHTGREEN );
  for( k=0; k<Na-1; k++ )
  { Point Q,P;
    if( !k )Chart.DtI( Q=Ma[0].G );
    for( Real a=0.1; a<1.05; a+=0.1 )
    { P.x=ReLine( &(Ma[0].G.x),Na,k+a,sizeof( METEO ) );
      P.y=ReLine( &(Ma[0].G.y),Na,k+a,sizeof( METEO ) ); Chart.DtI( P );
      line( Q,P ); Q=P;
  } }
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );  color( LIGHTCYAN );
  for( k=0; k<Na; k++ )Meteo_Mark( Ma[k] );
  setwritemode( COPY_PUT );
}
//
//      ��������� ����������� � ���������� ���������� �������� �������
//
int TWave::SMeteo()
{
 const char *Name[]={ "Meteo ","���������� ���������� � ����������  ",
                                 "�������� ������������ �������.",
                                 "������� ��� �������������, �������",
                                 "� �������� ���������� ����� ������",
                                 "���������� ���������� �������������",0 },
             *Text[]={ " F1  ","�����/������� ��������������� ����",
                       " F2  ","������� � ������ �������������",
                       " F3  ","����������� ������� ���� � ��������",
                       " F4  ","��������� �������������� ����������",
                       " F9  ","������ �������� �������������",0 },
             *Plus[]={ "<Enter>   ","������� ������������� �������",
                       "<Ins/Del> ","����/�������� ������� �����",
                       "<������>  ","����� ��������� ������� �����",
                       "< +/- >   ","������� ����� �� �������� ������",
                       "< S/s >   ","����������/���������� ��������",
                       "<Shift+...> ","�������� � �������� �������",
                       "< 0,1..9 >  ","��������� ���� �������",0, };
 static Real St=1;            // ��� �������� �������
 static int K=-1; Fix=false;  // ����������� ����� �� ����� � ���������
  Meteo_Marks();              //
  if( !Na && K<0 )            // ������������� ����� ����� ��� ������ ���������
  { K=0;                      //
    Met.T=0;                                  //
    Met.G=(Point){ 1,1 }; Chart.ItD( Met.G ); // ���������� ����� [ 1,1 ]
    Met.R=min( My*Dy,Mx*Dx )/6;               // ������ � �������� ����� �����
    Met.W=0.0;                                // �������� � ������������� �����
    Met.Avs=0.002;                            // ����������� ������.����������
    St=min( Dy,Dx )/3;                        //
  } else
  if( Na>0 && unsigned( K )>=unsigned( Na ) )K=Na-1;

 int k,y,x,WIND=0,ans=0xFF; Point P,Q;
  for(;;)
  { if( ans>=_Enter )
    { Thelp( ans==' '?"":"(%d/%d) F1 Inform"
           "  [Shift]+<Arrow><Space|+|-><Ins|Del><Enter>Point <S>ise",
            K,Na );
    } else if( (Tv_getk()&SHIFT)==0 )Fix=false;

    Meteo_Mark(); Chart.DtI( P=Met.G ); Q=P; ans=Tv_pause( &P,0  );
    Meteo_Mark(); Chart.ItD( Met.G=P );      y=int( P.y+0.5 );
                                             x=int( P.x+0.5 );
    if( ans<_Enter ){ Q.x=fabs( P.x-Q.x )*Dx;
                      Q.y=fabs( P.y-Q.y )*Dy; St=max( Q.y,Q.x ); }
    switch( ans )
    { case 'S':               Met.R+=St;  break;    // ��������� ��������
      case 's': if( Met.R>St )Met.R-=St;  break;    // ����� �������
      case '+': if( Na>0 )
                { ++K; if( K>=Na )K=0; memcpy( &Met,Ma+K,sizeof( METEO ) );
                  Fix=true;                //
                } break;                // ������� ����� �� ��������
      case '-': if( Na>0 )              //
                { --K; if( K<0 )K=Na-1; memcpy( &Met,Ma+K,sizeof( METEO ) );
                  Fix=true;
                } break;          //
      case ' ': if( Na>0 )        // ����� ��������� �����
                { Real Ds=0,s,c;  //
                  for( K=k=0; k<Na; k++ )
                  { Sphere( Met.G,Ma[k].G,c,s ); if( !k )Ds=s; else
                                                 if( Ds>s ){ Ds=s; K=k; }
                  } Fix=true; memcpy( &Met,Ma+K,sizeof( METEO ) );
                }   break;
      case _Enter:      //
      {                 // ������ �������������� ����������
       int days;        //      ������� �����
       Real W,D,H;      //
       Mlist Menu[]
       ={ { 0,2,"Ratio Time of Start %2d",&days },{ 0,8,"d%3�",&Met.T }
        , { 1,7,"Geo. Coordinates%2�",    &Q.y  },{ 0,8,"%2�", &Q.x }
        , { 1,5,"Cyclone Diameter %4.1lf",&D    },{ 0,0," km"       }
        , { 1,5,"Cyclone Pressure %4.1lf",&H    },{ 0,0," water'cm" }
        , { 1,4,"Maxima Wind Speed %.2lf",&W    },{ 0,0," m/sec"    }
        , { 1,7,"Surface Stress Factor %.5lf",&Met.Avs},{ 0,0," \xF7" }
        };
        if( !Na )Met.T=Tlaps;
        Met.T/=3600.0; days=int( Met.T/24 ); Q=Met.G;
        Met.T-= Real( days )*24.0; W=Met.W;
        D=Met.R/500.0;             H=Met.H*100;
        Meteo_Mark(); color( LIGHTGRAY ); Tmenu( Mlist( Menu ),1,1 );
        Meteo_Mark();                             Met.H=H/100;
        Met.R=D*500.0;                            Met.W=W;
        Met.T=( Met.T+Real( days )*24.0 )*3600.0; Met.G=Q;
      } if( !Fix )break;        //
      case _Ins:                // ��������� ����� ����� �� ������ �������
      { if( !Na )Ma=NULL;       //
        if( Na>0 && !Fix )
        { for( k=0; k<Na; k++ )if( fabs( Met.T-Ma[k].T )<1.0 )break;
          if( k<Na ){ Tv_bell(); break; }
        }
        Meteo_Marks();
        if( !Fix )
        { K=Na; Ma=(METEO*)Allocate( ( ++Na )*sizeof( METEO ),Ma );
        } memcpy( Ma+K,&Met,sizeof( METEO ) );
        Meteo_Marks(); Fix=true;        //
      }    break;                       // �������� �����, ���� ������� �������
      case _Del: if( Na>0 )             //
      { if( Fix && Ma[K].G==Met.G )
        { Meteo_Marks();
          if( K<Na-1 )memcpy( Ma+K,Ma+K+1,(Na-K+1)*sizeof( METEO ) );
          Ma=( METEO* )Allocate( ( --Na )*sizeof( METEO ),Ma );
          Meteo_Marks();
          if( K>=Na )K=Na-1;
          if( K<0 )K=0;
        } else Tv_bell();
      }    Fix=false; break;            //
      case _Esc: ans=0;                 // ����� �� ���������
      case 0xF9: case 0xF4: case 0xF3:  //
      case 0xF2: if( WIND )Tback(); Meteo_Marks(); Fix=false; return ans;
      case 0xF1: if( WIND^=1 ){ color( CYAN ); Twindow( 0,1,24,5 ); }
                          else{ Tback(); Ghelp(); } break;
     default : if( ans>' ' ){ Help( Name,Text,Plus ); ans=0xFF; }
    }
    if( WIND )
    { int Y=y,X=x; y=minmax( 0,y,My-1 );
                   x=minmax( 0,x,Mx-1 );
//      if( y<0 )y=0; else if( y>=Ny )y=Ny-1;
//      if( x<0 )x=0; else if( x>=Nx )x=Nx-1;
      Tgo( 1,1 );   color( Y==y&&X==x?LIGHTGRAY:CYAN );
      Tprintf( "Geo%6s",DtoA( Met.G.y ) );
      Tprintf(   "%s\n",DtoA( Met.G.x ) );
      Tprintf( "Depth[%d,%d] = %lg m\n", Y,X,Dp[y][x] );
      Tprintf( "Diameter %lg km\n",      Met.R/500  );
      Tprintf( "Pressure %lg water'mm\n",Met.H*1000 );
      Tprintf( "Wind Speed %lg m/sec\n", Met.W      );
    }
  }
}
