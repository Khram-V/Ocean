/* Long_Waves.cpp
   .-------------------------------------------------------------------------.
   �                                                                         �
   �    ��������� ������������� �������� �������� �������� � ��������������  �
   �                     �������� ���������� ����                            �
   �                                                                         �
   �    ����������� ������� ��������� ������� � ����� ��������������� �����  �
   �            ������������ ����� � ������������ ������� �������� �����     �
   �    ���� ������ ������������ ������� � ���� ��������� ����� ��������     �
   �            � �������� �����                                             �
   �    ��������� ������������� ���������� ������ �� ������ ��������� ����   �
   �            �� ������������ �������                                      �
   �    -- ����������� �������������� ����������� ������� �� ������ �������� �
   �            � ����������� �����                                          �
   �                                           (c)1987-2017, �������� �.�.   �
   '-------------------------------------------------------------------------'
*/
//                      88.09.19/DEC PASCAL v1.0  RT-11v5.2
//                      90.11.03/Turbo C          MsDos 3.3
//                      91.02.15/Turbo C++        DrDos 3.4
//                      92.06.22/Borland C++ 2.0  DrDos 6.0
//                      93.10.03/Borland C++ 3.1 & Lite-286
//                      94.09.05/Turbo C++ 3.1  Windows 3.1
//                      96.06.16/Borland C++ 3.1  & DPMI-16
//                      07.02.16/GNU-Dev-C++     Windows-XP
//                      08.09.05/B::C_GNU-C++ Windows-Vista
//                      10.08.12/�::B_GCC.4.3.3   Windows-7
//                      10.09.16/�::B_GCC.4.6.0 + OpenMP
//                      16.12.12/�::B_GCC.6.3.0 + Window&Button
//
#include "LongWave.h"
                        //
void Simulation()       // Main singular subroutine
{                       //
 TWave Ts; //( &Chart );// �������� �������� ��������� ������ ��� �������������
 DWORD Tp=0,Tc;         // ���� ������� .5� ��� �������� ���������� �����������
 bool draw=false;       // ������� ���������� ������������� ����� �����������

#pragma omp master
  {        Ts.Start();        //! Constructor ++Initial -- � ������ ���� ���!
    while( Ts.Interrupt() )   //
    {      Ts.Mariom();       // ������ ���������� �� ������������ �����
           Ts.EMeteo();       // ������� ����������� ������������ �������
           Ts.ESpaso();       // ������� ��������� ��������
           Ts.HBound();       // ��������� ������� � ��������� �������� ����
      if( !draw )             // ���������� �������� ��� ������ ������� �������
      if( (Tc=GetTickCount())-Tp > 1000 )
      {    Tp=Tc;
   //   #pragma omp task untied shared( draw )
        {
          draw=true; Ts.EGraph( false ); draw=false;
        }
  } } }
}
//#include "..\Ani\LW_A_Espaso.cpp" // ���������
//#include "..\Ani\LW_A_HBound.cpp" // ��������� �������
//#include "..\Ani\LW_A_EMeteo.cpp" // ������������
//#include "..\Ani\LW_A_Mariom.cpp" // ����������� ������������ ������
                                    //
bool TWave::Interrupt()             // ������� � ���������� � �������� ��������
{                                   //
 int ans=1,WS;                      // ������� ��� ���������� � ����� .mif
//static bool act=false;            // ������� ������ � ���� Service
//#pragma omp parallel shared( ans )
//#pragma omp master
  if( Kt )                          // ... � ���� ��������������� ������������?
  { ans=0; while( (WS=Tv_getw())!=0 )     // ������ �� ������� ������ �����
              if( WS<_MouseMoved || WS>_MouseReleased )ans=1;
    if( ans )
    if( !(VideoM&2) )               // -- �� ������������� ���� �����������
    {
//   #pragma omp taskwait
      { EGraph( true ); }
    }
  }
  if( !ans ){ ans=Tv_getw(); if( ans>=_MouseMoved && ans<=_MouseReleased )ans=0; }
  if( ans )
  for( ans=WS=0;; )
  { const char Com[]="F2 Service F3 Period F4 Source F5 Meteo "
                     "F6 Chart F7 Seismic F8 Mario F9 Start";
    Thelp( ans==' '?0:Com );
    if( VideoM&0x100 )ans=0xF2; else                      //
    if( ans==0xF1 )ans=Initial_Mode_and_View( ans ); else // ������� � ��������
    { ans=Map.Console( true );                            //   �����
      if( ans==-2 )
        { Map.Focus(); EGraph( true ); ans=0;
        }
    }                                         //
   Select: Thelp( Com );                      // ���������� ������� ��� �������
    switch( ans )                             //
    { case 0xF2: ans=Initial_Mode_and_View( ans );
                 if( ans ){ VideoM|=0x100; goto Select; }
                            VideoM&=~0x100; break;
      case 0xF3: ans=Period(); WS|=4;
                 if( ans>0xF0 )goto Select; break;
      case 0xF4: ans=Source(); WS|=8;
                 if( ans>0xF0 )goto Select; break;
      case 0xF5: ans=SMeteo(); WS|=0x10; WaveAs&=~0x800;
                 if( ans>0xF0 )goto Select; break;
      case 0xF6: WaveIn_state( 1 );  Thelp( "����������..." );
                 Vo.y/=Dy; Vo.x/=Dx; Chart.ItD( Vo );
                 Wo.y/=Dy; Wo.x/=Dx; Chart.ItD( Wo );
                 FreeTemp();          // ������� ���� ��������� �����
                 Edit_Chart();        // �������������� ����������
                 WS|=2;               // WS=2 ������� � ����� .MIF
                 Ghelp();             // ����� ��������� � ����������� ��������
                 if( Tlaps>0 )EGraph( true ); break;
      case 0xF7:                                   //
        { Event Est( 0 ); Point Geo; Real Mg,Hg;   // ����� ����� �������������
           if( ( Chart.Seismic_List( Geo,Est,Mg,Hg ) )>=0 )
            if( ( ans=Seismic_Meteo( Est,Geo,Mg,Hg ) )>0xF0 )goto Select;
        } break;
      case 0xF8: Chart.mario_control(); Mg=Chart.M; Nmp=Chart.Nmp; break;
        //
        // ����� �������� � ���������� � ������ ����������
        //! WaveIs:     1 � ����� ������ �������� �����
        //              2 � ������������� ������������� ��������� �����
        //              4 � ���� �������������
        //              8 � �� ����� �������������� ������� Wt+Rand( Wk );
        //           1.00 � Spherical Earth    \ .
        //           2.00 � Coriolis  Forces    \ ��������������
        //           4.00 � Viscosity Friction  / �������� ����������
        //           8.00 � Atmosphere Cyclone /
        //          10.00 - ������� ������� �������� ������ ����������
        //! ??      20.00 - ������� ������� ������� ������ (������ �����)
        //
      case 0xF9: WaveAs|=0x1000;       // ��������� �������� ������ ����������
        if( Mario_Init()<0 )break;     // ������ ���������� ����������
        if( !_Fc || !_La )             // �����������, �������� � ������������
        if( (WaveAs&0x0304)!=0 )       //
        { Real La=0.0,dL=0.0;    Thelp( "������� �����������" );
          if( Active&as_Globus )       // ������� ��������� ����������
          { Point P={ 0.0,0.0 }; Chart.ItD( P ); dL=Pd/Real( Mx );
            if( Chart.Cs!=0.0 )dL=-dL;           La=P.x/Radian;
          }                      // ����� ������� ��� �������� ������ � �������
          if( WaveAs&4 )         //
          _La=(Real**)Allocate( My,Mx*sizeof( Real ),_La ); //! ������ � ����
          _Fs=(Real**)Allocate( My,Mx*sizeof( Real ),_Fs ); //  sin( ������ )
          _Fc=(Real**)Allocate( My,Mx*sizeof( Real ),_Fc ); //  cos( ������ )
        #pragma omp parallel for
          for( int y=0; y<My; y++ )
          for( int x=0; x<Mx; x++ )
          { Point P={ Real( x )+0.5,Real( y )+0.5 }; Chart.ItD( P ); P/=Radian;
                     _Fs[y][x]=sin( P.y );
                     _Fc[y][x]=cos( P.y );
            if( _La )_La[y][x]=Active&as_Globus ? La+x*dL : P.x;
          }
        }                            // ��������� ������������ ������� �
        if( Na>0 && Fa==0 )          // ������������� ������ ��� ���� ��������
            Fa=(Real**)Allocate( My,Mx*sizeof( Real ) );
        //
        //  ���������� ����� �� �������� ������ �� ������� < F9 >
        //
        if( Kt>0 )WaveIn_state( WS ); Ghelp(); return true;
      case _Esc:
        Thelp( "��� ��������� ?" ); ans=Tv_getc();
        if( (ans|0x20)=='y' || ans==_Enter )
        { point p=(Point){Mx,0}; Ghelp(); bar( 0,p.y,p.x,Tv.mY ); Map.axis();
          if( Kt>0 && Rt>0.0 )save();
          FreeTemp();
          return false;
        } ans=_Enter;
     default: if( ans>' ' )ans=0xF1;
    }
  } return true;                          // Ts.Interrupt
}
void TWave::FreeTemp()
{ if(_Fs )Allocate( 0,0,_Fs ); _Fs=NULL;  // ����� �
  if(_Fc )Allocate( 0,0,_Fc ); _Fc=NULL;  //  ������� �� �������������� ������
  if(_La )Allocate( 0,0,_La ); _La=NULL;  // ������� ��� ������� �������������
  if( Hf )Allocate( 0,0,Hf  ); Hf =NULL;  // ������������, ����������� � Espaso
  if( Fa )Allocate( 0,0,Fa  ); Fa =NULL;  // ����������� �������� [����]
}
