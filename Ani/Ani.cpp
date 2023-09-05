//
//     ANI.cpp                                                /1987-2017.01.28/
//     (c) Khramushin V.N.
//      -  Tsunami Laboratory, Institute of Marine Geology and Geophysics,
//         USSR Academy of Sciences;
//      -  Sakhalin Research Institutes of Fisheries & Oceanography;
//      -  Computational Fluid Mechanics & Oceanography lab.
//         Special Research Bureau for Automation of Marine Researches
//         Far Eastern Branch of Russian Academy of Sciences
//      -  Sakhalin branch of Russian Geographical Society
//      << ������� �������������� ��������, ����������� ��������� >>
//      -  Computational Fluid Mechanics and Marine Researches lab.
//         Sakhalin State University. Devision of Mathematics
/*
                        Gorkiy str., 22/40
                        Yuzhno-Sakhalinsk, 693010, Sakhalin, Russia
                        88.09.19 / DEC PASCAL v1.0  RT-11v5.2 +
                        90.11.03 / Turbo C          MsDos 3.3 -
                        91.02.15 / Turbo C++        DrDos 3.4 +
                        92.06.22 / Borland C++ 2.0  DrDos 6.0 �
                        93.10.03 / Borland C++ 3.1 & Lite-286 -
                        94.09.05 / Turbo C++ 3.1  Windows 3.1 +
                        2000 --\\-- DPMI/16
                        2006 ������������� ���������� �������
                        2007.02.15 / Gnu-C++ / DevC++ / Win32
                        2008.01.05 / wxDevC++6.11 + GCC-3.4.5
                        2010.01.22 / Code::Blocks + GCC-4.3.3
                        2010.09.12 / GCC-4.6.0/4.3.3 + OpenMP
                        2011.04.05 / �������� � �������� �������
                        2013.05 -- ����������� ��������� �������
                        2014.02.12 / ������� �� 64-��������� ����������
                        2016.12.12 / ���������������� ����������� */
#include <OMP.h>
#include <Fenv.h>
#include "Depth.h"
/*
int matherr (struct _exception *a){ a->retval=0.0; return 1; }
int __cdecl _matherr (struct _exception *a){ a->retval=0.0; return 1; }
-Ofast
-floop-parallelize-all
*/
#define _0 (Depth::InfName)
#define _1 (Depth::Name)
char
Depth::InfName[MAX_PATH]="\n\r\n\r `ANI'       � ������� ������������  \0"
                         " Ani  - �������������� �������������",
Depth::Name[MAX_PATH] =  "\n\r  (c)\0_1987-2017, �.�.�������� \0"
                         " < ����������� �� 28 ������ 2017 �. > ",
Depth::Msg[][MAX_PATH]={ "\n\r\n\r(c)2010, �������������� N 2010615848\0\n\r",
                         "-= ���. ������ �����, �����, ������ �.������� =-" },
       Str[MAX_PATH*2];  // - � ������ ������� ��������� ������ ��� ���������
const char
#if _English
*_Month[]={ "January","February","March","April","May","June","July",
            "August","September","October","November","December" },
*_Week[]={ "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday" };
#else
*_Month[]={ "������","�������","����","������","���","����","����",
            "������","��������","�������","������","�������" },
*_Week[] ={ "�����������","�������","�����","�������",
            "�������","�������","�����������" };
#endif
Screen Map;             // ������ ����� ��� ������������� �� ����������� ������
Button All;             //  �������� �������� � �������/��������� ��������
static void Titles()    //   ��������� ��������� ��� �� ������
{ int ans,x;            //     � ��������� ��������� ��������� ����� ����������
  const char            //
     _3[]="�������������� ������������ ���",   // Computational Experiment in
     _4[]="������������� ��������, �������",   // Ocean Fluid Mechanics for a
     _5[]=" ���������� � ��������� �������";   // Seismic and Meteo Solutions
  color( YELLOW );      Tv.Height( 8 );        //
  ans=Tv.mX/Tv.Tw;      Tgo( ans-40,-3 ); Tprintf( W2D( _1+4 ) );
  color( LIGHTCYAN ); x=Tpoint.x-27;      Tprintf( W2D( _1+9 ) );
  color( LIGHTBLUE );   Tgo( ans-40,-1 ); Tprintf( W2D( Depth::Msg[0]+4 ) );
  color( WHITE );       Tgo( ans-45,-2 ); Tprintf( W2D( Depth::Msg[1] ) );
  color( LIGHTGRAY );   Tpoint.y-=( ans=3*Tv.Tw/2 )*3;
                        Tpoint.x=x;                Tprintf( W2D( _5 ) );
                        Tpoint.x=x; Tpoint.y-=ans; Tprintf( W2D( _4 ) );
                        Tpoint.x=x; Tpoint.y-=ans; Tprintf( W2D( _3 ) );
  Tv.Height( 18 );                                      x-=24;
  color( WHITE );  Tpoint.x=x; Tpoint.y-=( 3*Tv.Th/2 ); x+=12;
                                                   Tprintf( W2D( _0+10 ) );
  color( YELLOW ); Tpoint.x=x-24; Tpoint.y-=Tv.Th; Tprintf( W2D( _0+42 ) );
  Tv.Height( 0 );
}
static void Tv_Set_Palette();
       void Simulation();
       void Time_hex();
       Depth Chart;   //! ������� ��������� ����� - ����������
int    Depth::Nmp=0;   // Number of Marigrams
MARIO *Depth::M = 0;   // List of Marigrams
FILE  *Depth::Fs=NULL; // ��������� ����
unsigned Active=6;  // ���� ��������� ����� ����������
                    // x0001 ���������� ����������
                    // x0006 Empty, Letters, Numbers, Identify (4) ��� Mario
                    // x0020 ���������� ����� ����� � F4 (����� ��� ����������)
                    // x0018 ��� ������ ����� ���� � F4  (����� � ������������)
                    //   - 8 = 0 - ����������� ����������� ������ ��������� F4
                    // x0100 ������� ��������� ����������
                    // x8000 ������� �������������� ������������ �������
                    //
int main()          // int argc, char **argv - ���� � Tv_Graphics_Start();
{                   //
 static int  ans;
 const char *Name[]={ "ANI   ","������� ��������� �������������    ",
                                "� ���������� ������� ��������",0 },
            *Text[]={ "   �������������� ������������� ������:", "",
                      "","   �������������� ������������� ��������,",
                      "","   ��������� ������� � ������� � ������,",
                      "","   ���� ������ � ������ ������� �������",
                      "","                                �������.",
                      "F1 ��� <h>","������� ��� ���������",
                      " - ","(���� F1-�������, �� <h>-���������)",
                      "F2 ","����������� ������� ������ ����������",
                      "F3 ","���������� ���������� �/��� ����������",
                      "F4 ","�������������� �������� � ������������",
                      "F5 ","�������������� ������� ����� � �������",
                      "F6 ","�������������� ������ �������� ������",
                      "F7 ","����� �������� ������������� � ������",0 },
            *Plus[]={ "<������> ","������ ��������� ��� �������",
                 //   " <altP>  ","������ ������������ �����������",
                      " <altC>  ","��������� �������� �������",
                      " <altD>  ","����� ����������� ���������",
                      " <altX> ��� <ctrlC> ","��������� ���������",
                      "<�������> ","�������� �� ������",0,
                    };
        feclearexcept( FE_ALL_EXCEPT );
#ifdef _OPENMP
        omp_set_dynamic( true );
        omp_set_nested( true );
#endif
  printf( "\n   << ANI >>\tComputational Fluidmechanics and Marine Researches lab."
          "\n\t\t(c)2010, Sakhalin State University, Patent N 2010625848"
          "\n\t\t(c)1987-2016, Vasily Khramushin, Russia\n\n" );
  Tfile_Filter = "Bathimetry Data (*.inf)\0*.inf;*.int;*.dat;*.byt\0"
                 "Seismic Listing (*.lts)\0*.lts;*.txt\0"
                 "All Files (*.*)\0*.*\0";
  //
  //  � �������� ��������� ����������� ��� ��������������� �����
  //                                             (� ���� ������)
  Tv_Graphics_Start( 0,380,160 );
  setwindowtitle( "Ani - �������������� ������������� � ������� ������������" );
         Titles();                                /// ��������� ������� ������
         Chart.read( _ac_<2?NULL:_av_[1] );       /// � ����� ���������� �����
  if( !( Active&1 ) )return 1;                    /// ���������� ������� ������
  Map.f=(field){-10*Tv.Tw-1,-4*Tv.Th-8,-4,-24,0}; /// � ����������� �����������
     Tv_Start_Mouse();                  // ����������� ������ ��������� "�����"
     Tv_Set_Palette(); clear();         // ������ ������� ����������� ���������
        Map.axis();                     // ����� ����������� ����������� ����
        Map.view( Chart );              // ��� ������� ���������� �� ��������
  ans = Active&0x8000 ? 0xF6 : _Enter; color( LIGHTCYAN );
        ViewPointMario();
  All.Initial( " 1:1 ",0,Map.b.top ); // �������� ��� �������� � �������� �����
  for( ;; )
  { switch( ans )
    { case 0xF2: Chart.mario_control(); break; // ����������� ������ ����������
      case 0xF3: Chart.write("");       break; // ����� ������� ����������
      case 0xF4: Chart.edits();         break; // �������� ����� ����������
      case 0xF5: Time_hex();            break; // ���������� �
      case 0xF6: Simulation();          break; //   �������� ������ �� ��������
      case 0xF7: Chart.Seismic_List();  break; // ���������������� ������
      default: if( ans>' ' )ans=0xF1;
    }
    Thelp( ans==' '?0:"F1 Help  F2 Mario  F3 Write  F4 Chart"
                    "  F5 Time  F6 Waves  F7 Seismic" );
    if( ans==0xF1 )ans=Help( Name,Text,Plus ); else   // ans=Tv_Help( "ANI " );
    if( (ans=Map.Console( true ))==-2 )
    { Map.axis( Chart ); // ����� ����������� ���������� ����������� ����
      Map.view( Chart ); // ����� ����������� ������� ���������� �� �������
    }
    if( ans==_Esc )
    { Thelp( "���������� ?" ); ans=Tv_getc();
      if( (ans|0x20)=='y' || ans==_Enter )break;
} } }
//
//  ��������� ������� � ����� �������� ���������� ��������������� ������
//
static void Tv_Set_Palette()
{ int x;
 RGB_colors sea[96],seg[96],grn[48];
  for( x=0; x<96; x++ ){ sea[x].r=12+4*x/5; sea[x].g=32+x*2; sea[x].b=64+x*2;
                         seg[x].r=12+2*x/3; seg[x].g=64+x*2; seg[x].b=48+x*2; }
  for( x=0; x<48; x++ ){ grn[x].r= 96+(x<=12?x*12:159);               //
                         grn[x].g=255+(x<32?-4*x:-128+(x-32)*8);      // 3;
                         grn[x].b= 96+x*2;                            //
                       }                       //
     Set_Palette(  16,96,sea );                // ������� ������ ����
     Set_Palette( 112,96,seg );                // ������� ����������� ������
     Set_Palette( 208,48,grn );                // ��������� �������
     pattern( SOLID_FILL,BLACK );              //
  for( x=255; x>=0; x-- )
       pattern( x ),bar( (x*Tv.mX)/256,Tv_port.bottom,
                    ( (x+1)*Tv.mX)/256,Tv_port.bottom-6 );
  pattern( EMPTY_FILL,WHITE );
  bkcolor( WHITE );
}
#undef _0
#undef _1
