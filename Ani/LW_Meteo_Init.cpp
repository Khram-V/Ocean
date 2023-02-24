//
// T_SMeteo.cpp         Подготовка к гидрофизическому моделированию
//                      прохождения метео-циклона (тайфуна) и т.п.
//=====================================================================
//
//  На экране изображаются все опорные точки, образующие маршрут движения
//      циклона. С помощью стрелок на клавиатуре или движением "мыши"
//      по экрану перемещается образ вновь создаваемой опорной точки.
//
//  Enter    Меню ручного редактирование параметров циклона
//   -------------------------------¬
//   ¦Ratio Time of Start 0d 1°12'6"¦ Время отн. старта вычислений
//   ¦Geo. Coordinates 45°39'149°45'¦ Географические координаты циклона
//   ¦Cyclone Diameter 1500 km      ¦ Диаметр до "полного" стихания ветра
//   ¦Cyclone Pressure 25.5 water'cm¦ Перепад давления для водного столба
//   ¦Maxima Wind Speed  30 m/sec   ¦ Максимальная скорость ветра
//   ¦Surface Stress Factor 0.0020 ў¦ Коэфф. поверхностного напряжения
//   L-------------------------------
//
//  Space (пробел) - Поиск и установка ближайшей к точки на маршруте
//              движения циклона.
//  +,-      Перебор точек в списке определяющем маршрут циклона
//              Команды "+|-|Space|Insert" включают режим редактирования
//              уже существующей точки, это отмечается утроенной линией
//              окружности циклона.
//  Insert   Установка новой точки на маршруте циклона или корректировка
//              существующей точки. Если новая точка совпадает по времени
//              с любой из точек на маршруте циклона, то подается звуковой
//              сигнал ошибки и маршрут не обновляется.
//  Delete   Удаление точки из маршрута циклона, если эта точка отмечена
//              как захваченная.
//  "S|s"    Увеличение/уменьшение размеров текущего очага циклона в
//              соответствии с последним шагом перемещения по поверхности.
//
//  F1       Установка информационного окна о текущем состоянии
//              редактируемого очага циклона.
//  F2       Выход на основное меню настройки процесса вычислений
//  F3       Выход на меню построения волн входящих через свободные,
//              которые определяются с помощью периода во времени.
//  F4       Выход на построение источника волнообразования, который
//              контролируется как сохраняющий геометрические размеры
//              на свободных границах.
//  Esc      Выход из программы определения метео-источника морских волн.
//
#include <String.h>
#include <StdLib.h>
#include "..\Ani\LongWave.h"
                                  //
static bool Fix=false;            // Признак захвата одной из точек на маршруте
static METEO Met;                 // Контрольная точка для построения маршрута
                                  //
void Meteo_Mark( METEO& M )       // используется также в процессе расчетов
{ Point P; double A,W,R=M.R/1000.0;
  Geo_Circle( M.G,R );
  if( Fix )setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  if( M.W )                     //
  { W=R*0.4;                    // Отметка задания скорости ветра
    if( M.G.y<0 )W=-W;          // с учетом широты и знака
    if( M.W<0   )W=-W;          //
    for( A=1.0; A>-1.01; A-=0.1 )
    { P=Sphere( M.G,0,R*A );
      P=Sphere( P,-90,(A<0?-W:W)*Wind_Profile( A<0?-A:A ) ); Chart.DtI( P );
      if( A>0.99 )moveto( P );
           else   lineto( P );
  } }                           //
  if( M.H )                     // Отметка профиля перепада давления
  { W=R*0.4;                    //
    for( A=-1.0; A<1.01; A+=0.1 )
    { P=Sphere( M.G,90.0,R*A );
      P=Sphere( P,M.H>0?180.0:0.0,W*Pressure_Profile( A<0?-A:A ) );
      Chart.DtI( P );
      if( A<-0.99 )moveto( P ); else lineto( P );
  } }                           //
}                               // Отметка подвижной точки
static void Meteo_Mark()        //
{ color( Met.W==0.0 && Met.H==0.0?LIGHTGRAY:WHITE );
           setwritemode( XOR_PUT );
  if( Fix )setlinestyle( SOLID_LINE,0,THICK_WIDTH ); Meteo_Mark( Met );
           setwritemode( COPY_PUT );
}
static int meteo_sort( const void *a, const void *b )
{ return ((METEO*)a)->T < ((METEO*)b)->T ? -1:1;
}                               //
void TWave::Meteo_Marks()       // Сортировка и отметка маршрута циклона
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
//      Процедура определения и построения траектории движения циклона
//
int TWave::SMeteo()
{
 const char *Name[]={ "Meteo ","Построение траектории и расписания  ",
                                 "движения атмосферного циклона.",
                                 "Задание его характеристик, которые",
                                 "в процессе вычислений будут плавно",
                                 "изменяться сплайновой интерполяцией",0 },
             *Text[]={ " F1  ","вызов/гашение информационного окна",
                       " F2  ","условия и режимы моделирования",
                       " F3  ","определение внешних волн и приливов",
                       " F4  ","установка геометрических источников",
                       " F9  ","запуск процесса моделирования",0 },
             *Plus[]={ "<Enter>   ","задание характеристик циклона",
                       "<Ins/Del> ","ввод/удаление опорной точки",
                       "<пробел>  ","поиск ближайшей опорной точки",
                       "< +/- >   ","перебор точек из текущего списка",
                       "< S/s >   ","увеличение/уменьшение размеров",
                       "<Shift+...> ","операции с текущими точками",
                       "< 0,1..9 >  ","множители шага курсора",0, };
 static Real St=1;            // Шаг движения маркера
 static int K=-1; Fix=false;  // Контрольная точка на входе в программу
  Meteo_Marks();              //
  if( !Na && K<0 )            // Инициализация одной точки при первом обращении
  { K=0;                      //
    Met.T=0;                                  //
    Met.G=(Point){ 1,1 }; Chart.ItD( Met.G ); // Координаты точки [ 1,1 ]
    Met.R=min( My*Dy,Mx*Dx )/6;               // Размер в четверть малой грани
    Met.W=0.0;                                // Скорость в циклоническом вихре
    Met.Avs=0.002;                            // Коэффициент поверх.напряжения
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
    { case 'S':               Met.R+=St;  break;    // Изменение размеров
      case 's': if( Met.R>St )Met.R-=St;  break;    // очага циклона
      case '+': if( Na>0 )
                { ++K; if( K>=Na )K=0; memcpy( &Met,Ma+K,sizeof( METEO ) );
                  Fix=true;                //
                } break;                // Перебор точек на маршруте
      case '-': if( Na>0 )              //
                { --K; if( K<0 )K=Na-1; memcpy( &Met,Ma+K,sizeof( METEO ) );
                  Fix=true;
                } break;          //
      case ' ': if( Na>0 )        // Поиск ближайшей точки
                { Real Ds=0,s,c;  //
                  for( K=k=0; k<Na; k++ )
                  { Sphere( Met.G,Ma[k].G,c,s ); if( !k )Ds=s; else
                                                 if( Ds>s ){ Ds=s; K=k; }
                  } Fix=true; memcpy( &Met,Ma+K,sizeof( METEO ) );
                }   break;
      case _Enter:      //
      {                 // Ручное редактирование параметров
       int days;        //      текущей точки
       Real W,D,H;      //
       Mlist Menu[]
       ={ { 0,2,"Ratio Time of Start %2d",&days },{ 0,8,"d%3°",&Met.T }
        , { 1,7,"Geo. Coordinates%2°",    &Q.y  },{ 0,8,"%2°", &Q.x }
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
      case _Ins:                // Установка новой точки на основе текущей
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
      }    break;                       // Удаление точки, если таковая имеется
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
      case _Esc: ans=0;                 // Выход из программы
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
