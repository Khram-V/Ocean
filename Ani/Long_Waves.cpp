/* Long_Waves.cpp
   .-------------------------------------------------------------------------.
   ¦                                                                         ¦
   ¦    Программа моделирования динамики волновых движений с использованием  ¦
   ¦                     линейных разностных схем                            ¦
   ¦                                                                         ¦
   ¦    Реализуется задание начальных условий в форме геометрического очага  ¦
   ¦            произвольной формы и определением периода входящей волны     ¦
   ¦    Плюс модель атмосферного циклона в виде подвижных полей давления     ¦
   ¦            и скорости ветра                                             ¦
   ¦    Добавлено моделирование приливного режима на основе генерации волн   ¦
   ¦            из мореографных станций                                      ¦
   ¦    -- Реализовано восстановление прерванного расчета на основе рестарта ¦
   ¦            с контрольной точки                                          ¦
   ¦                                           (c)1987-2017, Храмушин В.Н.   ¦
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
//                      10.08.12/С::B_GCC.4.3.3   Windows-7
//                      10.09.16/С::B_GCC.4.6.0 + OpenMP
//                      16.12.12/С::B_GCC.6.3.0 + Window&Button
//
#include "LongWave.h"
                        //
void Simulation()       // Main singular subroutine
{                       //
 TWave Ts; //( &Chart );// Основная закрытая структура данных для моделирования
 DWORD Tp=0,Tc;         // Тики таймера .5с для контроля интервалов перерисовки
 bool draw=false;       // Признак завершения параллельного блока перерисовки

#pragma omp master
  {        Ts.Start();        //! Constructor ++Initial -- и только один раз!
    while( Ts.Interrupt() )   //
    {      Ts.Mariom();       // Запись информации из мореографных точек
           Ts.EMeteo();       // Расчеты воздействия атмосферного циклона
           Ts.ESpaso();       // Главный расчетный алгоритм
           Ts.HBound();       // Граничные условия и генерация входящих волн
      if( !draw )             // Прорисовка картинки или просто отметка времени
      if( (Tc=GetTickCount())-Tp > 1000 )
      {    Tp=Tc;
   //   #pragma omp task untied shared( draw )
        {
          draw=true; Ts.EGraph( false ); draw=false;
        }
  } } }
}
//#include "..\Ani\LW_A_Espaso.cpp" // Уравнения
//#include "..\Ani\LW_A_HBound.cpp" // Граничные условия
//#include "..\Ani\LW_A_EMeteo.cpp" // Метеорология
//#include "..\Ani\LW_A_Mariom.cpp" // Регистрация мореографных данных
                                    //
bool TWave::Interrupt()             // Общение с терминалом в процессе расчетов
{                                   //
 int ans=1,WS;                      // Признак для обновления в файле .mif
//static bool act=false;            // Признак работы с меню Service
//#pragma omp parallel shared( ans )
//#pragma omp master
  if( Kt )                          // ... в ходе вычислительного эксперимента?
  { ans=0; while( (WS=Tv_getw())!=0 )     // запрос со сбросом буфера ввода
              if( WS<_MouseMoved || WS>_MouseReleased )ans=1;
    if( ans )
    if( !(VideoM&2) )               // -- по необходимости идет перерисовка
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
    if( ans==0xF1 )ans=Initial_Mode_and_View( ans ); else // запросы к стоящему
    { ans=Map.Console( true );                            //   счету
      if( ans==-2 )
        { Map.Focus(); EGraph( true ); ans=0;
        }
    }                                         //
   Select: Thelp( Com );                      // Исполнение команды без запроса
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
      case 0xF6: WaveIn_state( 1 );  Thelp( "Подготовка..." );
                 Vo.y/=Dy; Vo.x/=Dx; Chart.ItD( Vo );
                 Wo.y/=Dy; Wo.x/=Dx; Chart.ItD( Wo );
                 FreeTemp();          // Очистка всех временных полей
                 Edit_Chart();        // Редактирование батиметрии
                 WS|=2;               // WS=2 отметка в файле .MIF
                 Ghelp();             // Сброс подсказки и перерисовка картинки
                 if( Tlaps>0 )EGraph( true ); break;
      case 0xF7:                                   //
        { Event Est( 0 ); Point Geo; Real Mg,Hg;   // Выбор очага землетрясения
           if( ( Chart.Seismic_List( Geo,Est,Mg,Hg ) )>=0 )
            if( ( ans=Seismic_Meteo( Est,Geo,Mg,Hg ) )>0xF0 )goto Select;
        } break;
      case 0xF8: Chart.mario_control(); Mg=Chart.M; Nmp=Chart.Nmp; break;
        //
        // Старт расчетов и подготовка к записи протоколов
        //! WaveIs:     1 ¦ Задан период входящей волны
        //              2 ¦ Отслеживается протяженность граничной волны
        //              4 ¦ Учет геопотенциала
        //              8 ¦ На входе стохастический процесс Wt+Rand( Wk );
        //           1.00 ¦ Spherical Earth    \ .
        //           2.00 ¦ Coriolis  Forces    \ Дополнительные
        //           4.00 ¦ Viscosity Friction  / элементы вычислений
        //           8.00 ¦ Atmosphere Cyclone /
        //          10.00 - Признак запуска процесса записи мореограмм
        //! ??      20.00 - Признак слияния боковых границ (Земная сфера)
        //
      case 0xF9: WaveAs|=0x1000;       // Инициация процесса записи мореограмм
        if( Mario_Init()<0 )break;     // Запрос готовности протоколов
        if( !_Fc || !_La )             // сферичность, вращение и геопотенциал
        if( (WaveAs&0x0304)!=0 )       //
        { Real La=0.0,dL=0.0;    Thelp( "Узловые перерасчеты" );
          if( Active&as_Globus )       // признак замыкания параллелей
          { Point P={ 0.0,0.0 }; Chart.ItD( P ); dL=Pd/Real( Mx );
            if( Chart.Cs!=0.0 )dL=-dL;           La=P.x/Radian;
          }                      // Новая матрица для хранения широты и долготы
          if( WaveAs&4 )         //
          _La=(Real**)Allocate( My,Mx*sizeof( Real ),_La ); //! Солнце и Луна
          _Fs=(Real**)Allocate( My,Mx*sizeof( Real ),_Fs ); //  sin( широты )
          _Fc=(Real**)Allocate( My,Mx*sizeof( Real ),_Fc ); //  cos( широты )
        #pragma omp parallel for
          for( int y=0; y<My; y++ )
          for( int x=0; x<Mx; x++ )
          { Point P={ Real( x )+0.5,Real( y )+0.5 }; Chart.ItD( P ); P/=Radian;
                     _Fs[y][x]=sin( P.y );
                     _Fc[y][x]=cos( P.y );
            if( _La )_La[y][x]=Active&as_Globus ? La+x*dL : P.x;
          }
        }                            // Включение атмосферного циклона и
        if( Na>0 && Fa==0 )          // распределение памяти для поля давления
            Fa=(Real**)Allocate( My,Mx*sizeof( Real ) );
        //
        //  Нормальный выход из останова только по команде < F9 >
        //
        if( Kt>0 )WaveIn_state( WS ); Ghelp(); return true;
      case _Esc:
        Thelp( "Это остановка ?" ); ans=Tv_getc();
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
{ if(_Fs )Allocate( 0,0,_Fs ); _Fs=NULL;  // синус и
  if(_Fc )Allocate( 0,0,_Fc ); _Fc=NULL;  //  косинус от географической широты
  if(_La )Allocate( 0,0,_La ); _La=NULL;  // долгота для расчета геопотенциала
  if( Hf )Allocate( 0,0,Hf  ); Hf =NULL;  // геопотенциал, заполняется в Espaso
  if( Fa )Allocate( 0,0,Fa  ); Fa =NULL;  // атмосферное давление [метр]
}
