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
//      << Русское географическое общество, Сахалинское отделение >>
//      -  Computational Fluid Mechanics and Marine Researches lab.
//         Sakhalin State University. Devision of Mathematics
/*
                        Gorkiy str., 22/40
                        Yuzhno-Sakhalinsk, 693010, Sakhalin, Russia
                        88.09.19 / DEC PASCAL v1.0  RT-11v5.2 +
                        90.11.03 / Turbo C          MsDos 3.3 -
                        91.02.15 / Turbo C++        DrDos 3.4 +
                        92.06.22 / Borland C++ 2.0  DrDos 6.0 •
                        93.10.03 / Borland C++ 3.1 & Lite-286 -
                        94.09.05 / Turbo C++ 3.1  Windows 3.1 +
                        2000 --\\-- DPMI/16
                        2006 перенастройка управления памятью
                        2007.02.15 / Gnu-C++ / DevC++ / Win32
                        2008.01.05 / wxDevC++6.11 + GCC-3.4.5
                        2010.01.22 / Code::Blocks + GCC-4.3.3
                        2010.09.12 / GCC-4.6.0/4.3.3 + OpenMP
                        2011.04.05 / Изменена и ускорена графика
                        2013.05 -- безуспешная переделка фронтов
                        2014.02.12 / переход на 64-разрядную арифметику
                        2016.12.12 / фрагментирование изображений */
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
Depth::InfName[MAX_PATH]="\n\r\n\r `ANI'       и морские исследования  \0"
                         " Ani  - Вычислительная гидромеханика",
Depth::Name[MAX_PATH] =  "\n\r  (c)\0_1987-2017, В.Н.Храмушин \0"
                         " < Исправления от 28 января 2017 г. > ",
Depth::Msg[][MAX_PATH]={ "\n\r\n\r(c)2010, Госрегистрация N 2010615848\0\n\r",
                         "-= лаб. Цунами ИМГиГ, СахГУ, СахНТО А.Крылова =-" },
       Str[MAX_PATH*2];  // - и просто длинная удвоенная строка для сообщений
const char
#if _English
*_Month[]={ "January","February","March","April","May","June","July",
            "August","September","October","November","December" },
*_Week[]={ "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday" };
#else
*_Month[]={ "Январь","Февраль","Март","Апрель","Май","Июнь","Июль",
            "Август","Сентябрь","Октябрь","Ноябрь","Декабрь" },
*_Week[] ={ "понедельник","вторник","среда","четверг",
            "пятница","суббота","воскресенье" };
#endif
Screen Map;             // Сектор карты для представления на графическом экране
Button All;             //  Кнопочка возврата к полному/исходному масштабу
static void Titles()    //   Заголовок программы при ее старте
{ int ans,x;            //     с текстовым описанием исходного файла батиметрии
  const char            //
     _3[]="Вычислительные эксперименты при",   // Computational Experiment in
     _4[]="моделировании приливов, морских",   // Ocean Fluid Mechanics for a
     _5[]=" наводнений и штормовых нагонов";   // Seismic and Meteo Solutions
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
       Depth Chart;   //! Главная структура карты - батиметрии
int    Depth::Nmp=0;   // Number of Marigrams
MARIO *Depth::M = 0;   // List of Marigrams
FILE  *Depth::Fs=NULL; // Локальный файл
unsigned Active=6;  // Биты состояния карты батиметрии
                    // x0001 Готовность батиметрии
                    // x0006 Empty, Letters, Numbers, Identify (4) для Mario
                    // x0020 Произведен сдвиг карты в F4 (сброс при считывании)
                    // x0018 Был считан новый файл в F4  (сброс с перерисовкой)
                    //   - 8 = 0 - Произведена перерисовка внутри программы F4
                    // x0100 Признак замыкания параллелей
                    // x8000 Процесс восстановления сохраненного расчета
                    //
int main()          // int argc, char **argv - ушли в Tv_Graphics_Start();
{                   //
 static int  ans;
 const char *Name[]={ "ANI   ","Главная процедура инициализации    ",
                                "и управления блоками программ",0 },
            *Text[]={ "   Вычислительная гидромеханика океана:", "",
                      "","   длинноволновое моделирование приливов,",
                      "","   штормовых нагонов и тягунов в портах,",
                      "","   волн цунами и других опасных морских",
                      "","                                явлений.",
                      "F1 или <h>","справки или подсказки",
                      " - ","(если F1-справка, то <h>-подсказка)",
                      "F2 ","расстановка морских постов наблюдения",
                      "F3 ","перезапись информации и/или батиметрии",
                      "F4 ","географический редактор и интерполяция",
                      "F5 ","кинематические расчеты лучей и фронтов",
                      "F6 ","длинноволновые модели динамики океана",
                      "F7 ","вызов каталога землетрясений и цунами",0 },
            *Plus[]={ "<Пробел> ","снятие подсказки или справки",
                 //   " <altP>  ","запись графического изображения",
                      " <altC>  ","обращение цветовой палитры",
                      " <altD>  ","вызов встроенного редактора",
                      " <altX> или <ctrlC> ","остановка программы",
                      "<стрелки> ","подвижки по экрану",0,
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
  //  В качестве параметра принимается имя информационного файла
  //                                             (и пока только)
  Tv_Graphics_Start( 0,380,160 );
  setwindowtitle( "Ani - вычислительная гидромеханика и морские исследования" );
         Titles();                                /// заголовок чистого вызова
         Chart.read( _ac_<2?NULL:_av_[1] );       /// и после считывания карты
  if( !( Active&1 ) )return 1;                    /// выбираются размеры экрана
  Map.f=(field){-10*Tv.Tw-1,-4*Tv.Th-8,-4,-24,0}; /// и графическое отображение
     Tv_Start_Mouse();                  // активизация работы указателя "мышка"
     Tv_Set_Palette(); clear();         // чистая палитра графических расцветок
        Map.axis();                     // здесь формируется графическое поле
        Map.view( Chart );              // это простая прорисовка по готовому
  ans = Active&0x8000 ? 0xF6 : _Enter; color( LIGHTCYAN );
        ViewPointMario();
  All.Initial( " 1:1 ",0,Map.b.top ); // кнопочка для возврата к исходной карте
  for( ;; )
  { switch( ans )
    { case 0xF2: Chart.mario_control(); break; // Расстановка постов наблюдения
      case 0xF3: Chart.write("");       break; // Вывод текущей информации
      case 0xF4: Chart.edits();         break; // Редактор полей батиметрии
      case 0xF5: Time_hex();            break; // КИНЕМАТИКА и
      case 0xF6: Simulation();          break; //   ДИНАМИКА Океана на сфероиде
      case 0xF7: Chart.Seismic_List();  break; // Сейсмологический список
      default: if( ans>' ' )ans=0xF1;
    }
    Thelp( ans==' '?0:"F1 Help  F2 Mario  F3 Write  F4 Chart"
                    "  F5 Time  F6 Waves  F7 Seismic" );
    if( ans==0xF1 )ans=Help( Name,Text,Plus ); else   // ans=Tv_Help( "ANI " );
    if( (ans=Map.Console( true ))==-2 )
    { Map.axis( Chart ); // здесь формируется собственно графическое поле
      Map.view( Chart ); // затем выполняется простая прорисовка на готовом
    }
    if( ans==_Esc )
    { Thelp( "Завершение ?" ); ans=Tv_getc();
      if( (ans|0x20)=='y' || ans==_Enter )break;
} } }
//
//  Установка палитры и вызов программ прорисовки топографических файлов
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
     Set_Palette(  16,96,sea );                // палитра синего моря
     Set_Palette( 112,96,seg );                // палитра пониженного уровня
     Set_Palette( 208,48,grn );                // береговая палитра
     pattern( SOLID_FILL,BLACK );              //
  for( x=255; x>=0; x-- )
       pattern( x ),bar( (x*Tv.mX)/256,Tv_port.bottom,
                    ( (x+1)*Tv.mX)/256,Tv_port.bottom-6 );
  pattern( EMPTY_FILL,WHITE );
  bkcolor( WHITE );
}
#undef _0
#undef _1
