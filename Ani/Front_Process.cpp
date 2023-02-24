//
//   Процедура расчета времен добегания длинных волн по мелкой воде
//
//   Алгоритм выбора минимальных времен по принципу Гюйгенса.
//   Для расчета лучей используется дополнительная матрица,
//        в которой хранятся направления нормалей к фронту.
//   Основной алгоритм расчета времен основывается на матрице скоростей _D[]
//
//   2013-04-16 - ускорение расчетов сложением-сортировкой
//
#include <Time.h>
#include "..\Ani\Depth.h"
#include "..\Graphics\Tf_Topo.h"
//
//      последовательный список фронтальных точек с указанием его оконечностями
//
static int FrontLength=0;     // !! надо контролировать каждое начало = 0
static struct item{ int y,x; Real t; void Insert(); } P={ 0,0,0.0 };
static struct Front: item     // здесь координатные индексы контрольной точки
{ Front *prev,*next;          // смежные точки для образования связного списка
  void Insert( const item* ); // собственно создание и
  void Detach();              //         последующее удаление фронтальной точки
  void Less( Real );          // уменьшение времени в одной расчетной точке
} First,Last;                 // условная вечность в списке событий во времени
                              //
//Front::Front():                                // здесь задаются крайние
// prev( &First ),next( &Last ){ x=y=0; t=0.0; } // значения граничных точек

void item::Insert(){ ((Front*)malloc( sizeof( Front ) ))->Insert( this ); }
void Front::Insert( const item *Q )                   // адрес не должен обнулиться
{ t=Q->t; x=Q->x; y=Q->y;
  for( prev=Last.prev;; prev=prev->prev )
   if( t>=prev->t )
     { next=prev->next;
       next->prev=prev->next=this; ++FrontLength; return;
     } Break( "?...\7 front mistake ... %lg[%dx%d]",t,x,y );
}
static void Detach( Front *F ){ if( F ){ F->Detach(); free( F ); } }
void Front::Detach()
{ if( FrontLength>0 ){ prev->next=next; next->prev=prev; --FrontLength; }
}
void Front::Less( Real m )      /// уменьшение времени в одной расчетной точке
{    prev->next=next;            // точка полностью исключается из списка
     next->prev=prev; while( m<prev->t )prev=prev->prev; t=m;
     next=prev->next;            // независимый сдвиг на одну позицию вниз
     next->prev=prev->next=this; // и вставка самоё-себя в новое место
}                                //
static item *Fb=NULL;            // исходный установочный массив [Lb]
static float **_P,               // Матрица индексов принадлежности
             **_D, D,            // Текущие значения времен
             **_T, T,            // Времена для контрольной точки
             **_S,               // Результирующие разности
             **_R,               // Массив углов подхода фронта волны
             **_A;               // Массив амплитудных коэффициентов
static Real Sy,Sx,Dd,            // Две стороны и диагональ
            Rst=0.6,Rsr=-0.866,  // Шаг луча в долях квадр.ячейки
            TMin=0.0,            // Минимальное время в стартовых точках
            TerminalStep=6.0,    // Регулируемый шаг [мин] и новое
            TerminalFront=0.0;   //    время для следующей прорисовки фронта
static int Lb=0,                 // Длина списка и активного фронта
          Nbr=12,                // Ограничение крутых поворотов лучей
           Ny,Nx,                // == на всякий случай ==
          Iden=0;                // Признак принадлежности к станции
                                 // -1 - блокировка изображения
static byte Aht=0,               // Указание на необходимость расчета амплитуд
            Solve=0;             // Признак выполненных решений
static bool Globus=false;        // Признак полной Земной сферы
static char Msg[MAX_PATH];       //
                                 //
#define Y P.y
#define X P.x
#define ubnd( a,b ) ( unsigned( a )>=unsigned( b ) ) // Точка за границей ?
#define isea( y,x ) ( Chart[ y ][ x ]>0 )            // Точка в море ?

static void Cross( const item& Q  )
{  if( Q.t>0.0 )line( Q.x,Q.y-2,Q.x,Q.y+3 ),line( Q.x-2,Q.y,Q.x+3,Q.y );
      else  line( Q.x-2,Q.y-2,Q.x+3,Q.y+3 ),line( Q.x-2,Q.y+2,Q.x+3,Q.y-3 );
}
static void Cross()                    // Отметка фронта на графическом экране
{ static item *F=NULL; Front *L;
  static int LF=0,LM=0; int x,y,k=LF, c=color( MAGENTA^WHITE );
   hidevisualpage();
   setwritemode( XOR_PUT );
   while( --k>=0 )Cross( F[k] );
   if( LM<512 )
   if( FrontLength>LM )F=(item*)Allocate( (LM=FrontLength)*sizeof(item),F );
       LF=min( FrontLength,LM );
   for( L=First.next,k=0; k<LF; k++,L=L->next )
   { item &Q=F[k]; Q.x=Tv_x( L->x );
                   Q.y=Tv_y( L->y ); Q.t=L->t; Cross( Q );
   } color( c );
   setwritemode( COPY_PUT );
   if( T>0.0 )Ghelp( "Time[%d+%0.3lg'] = %s",FrontLength,TerminalStep,DtoH( TerminalFront/60.0 ) );
   showactivepage();
}
inline bool leq( item *L ){ return L->y==Y && L->x==X; }

static bool Examine()         //
{ if( FrontLength )           // Поиск фронтовой точки с минимальным временем
  { P=*First.next;            //
    float& Tm=_D[Y][X]; D=Tm; Tm=T=P.t;
    Detach( First.next );     // исключение первой точки из фронтового списка
    if( T>=TerminalFront )    //
    { static double cT,cTime;           //! по 3 минуты через 2 секунды
      if( !T )cTime=onetime(); else     // 1 сек
      if( (cT=onetime())>=cTime+2.7777e-4 )
      { TerminalStep=min( 60.0,max( 3.0,TerminalStep*(5.555e-4/(cT-cTime)) ) );
        cTime=cT;
      }
      Cross();                // перерисовка текущего положения фронта
      if( Tv_getw()==_Esc )   // обработка прерываний по Escape от клавиатуры
      { Tv_bell(); Thelp( "?Прервать счет [n/y] " );
        if( (Tv_getc()|0x20)=='y' )goto Ret;
      }
      TerminalFront += TerminalStep;
    } return true;
  } Cross(); return false;
Ret:
  { Front *L=First.next;
    int k,l=FrontLength;
    for( k=0; k<l; k++ ){ L=L->next; if( Aht!=3 )Detach( First.next ); }
  } return false;
}
//     Просмотр и анализ окружения для новой точки
//           Dr - расстояние до центра шаблона
//
static void Setim( Real Dr )
{ if( ubnd( Y,Ny ) || ubnd( X,Nx ) )return;
  Real d=_D[Y][X];             //
  if( d<0 )                     // Включение новой точки в список
  { Front *L=First.next;        //
           P.t=T-Dr/(D+d);
    if( _P )_P[Y][X]=Iden;
    while( L->next )
      if( leq( L ) ){ if( P.t<L->t )L->Less( P.t ); return; } else L=L->next;
    P.Insert();
  }
}
//      Просмотр окружения фронтовой точки для вычисления
//                      дивергенции фронтового потока
static bool Height()
{ if( ubnd( Y,Ny ) || ubnd( X,Nx ) )return false;
  if( (P.t=_D[Y][X])>T )                // Включение новой точки в список
  { int k;
    Front *L=First.next;
     for( k=0; k<FrontLength && !leq( L ); k++ )L=L->next;
      if( k==FrontLength )P.Insert();
  } return P.t>0.0;
}
///
//!  DecideFour - расчеты с использованием градиента скоростей для поля изохрон
///
inline double Front_Angle( double Ty, double Tx )
{ double Tt=hypot( Ty,Tx )/1.36; Ty/=Sy;
                                 Tx/=Sx;
     if( Ty>Tx )return Tt*( 1.01-Ty/hypot( Ty,Tx ) );
          else  return Tt*( 1.01-Tx/hypot( Ty,Tx ) );
}
#define I( x ) if( Globus ){ if( x<0 )x=Nx-1; else if( x==Nx )x=0; }

static void DecideFour()
{ int x,m,M;
  double A=0.0,N=0.0,W=0.0,E=0.0,S=0.0; TerminalFront=0.0; TerminalStep=6.0;
  while( Examine() )
  { M=( North_West|South_East );
    if( ubnd( Y+1,Ny ) )continue; if( ( N=_D[Y+1][X] )>0 ){ M&=~North; N=T-N; }
    if( ubnd( Y-1,Ny ) )continue; if( ( S=_D[Y-1][X] )>0 ){ M&=~South; S=T-S; }
    x=X+1; I( x ) else
      if( ubnd( x,Nx ) )continue; if( ( E=_D[Y][x] )>0 ){ M&=~East; E=T-E; }
    x=X-1; I( x ) else
      if( ubnd( x,Nx ) )continue; if( ( W=_D[Y][x] )>0 ){ M&=~West; W=T-W; }
                                                           m=0;
    if( !( M&North_East ) ){         A=Front_Angle( N,E ); m++; }
    if( !( M&South_East ) ){ if( !m )A=Front_Angle( S,E ); m++; }
    if( !( M&South_West ) ){ if( !m )A=Front_Angle( S,W ); m++; }
    if( !( M&North_West ) ){ if( !m )A=Front_Angle( N,W ); m++; }
    if( m==1 )_D[Y][X]=T-=A;     //
    Y++,x=X;        Setim( Sy ); // North     1
    Y--,X--; I( X ) Setim( Sx ); //   West  2<+>4
    Y--,X=x;        Setim( Sy ); // South     3
    Y++,X++; I( X ) Setim( Sx ); //   East
        X=x;                     //
    if( Iden )Iden=int( _P[Y][X]+0.1 );
  }
  if( Aht )
  { Aht=1;
    TerminalFront=0.0;
    while( FrontLength )Detach( First.next );
    for( m=0; m<Lb; m++ )Fb[m].Insert();
    while( Examine() )
    { float& Ah=_A[Y][X];
      Y++,x=X; M=m=0; if( Height() ){ M++; if((N=(P.t-T)/Sy)<=0)Ah+=_A[Y][X],m++; }
      Y--,X--; I( X ) if( Height() ){ M++; if((W=(P.t-T)/Sx)<=0)Ah+=_A[Y][X],m++; }
      Y--,X=x;        if( Height() ){ M++; if((S=(P.t-T)/Sy)<=0)Ah+=_A[Y][X],m++; }
      Y++,X++; I( X ) if( Height() ){ M++; if((E=(P.t-T)/Sx)<=0)Ah+=_A[Y][X],m++; }
          X=x;
      if( T>0.0 )
      { if( m>0  )Ah =(Ah-1)/m;
        if( M==4 )Ah*=(1-( N+W+E+S )/hypot( N-S,E-W )/2);
  } } }
}
#undef I
#include "Front_Rays.cpp"
//
//     Добавление точек фронта
//
static void IncExc( int Mode=0 ){ if( Chart[Y][X]<=0 )return; int k;
  Front *L=First.next;
  for( k=0; k<FrontLength && !leq( L ); k++ )L=L->next;
  if( k==FrontLength )                             // с текущей отметкой
    { if( !Mode || Mode==_Ins )P.Insert(); }       // времени на будущем фронте
  else if( !Mode || Mode==_Del )Detach( L ); Cross();
}
//     Старый вариант расчета с использованием многоугольника
//              и принципа Гюйгенса с нулевым приближением
//
static void Decide_Oct()
{
  Real Dy=hypot( Sy*2,Sx ),
       Dx=hypot( Sy,Sx*2 );
  while( Examine() )
  { if( Iden )Iden=_P[Y][X];
    Y++;       Setim( Sy ); // North
         X--;  Setim( Dd ); // North West
    Y--;       Setim( Sx ); //       West           2..1..8
    Y--;       Setim( Dd ); // South West           :\   /:
         X++;  Setim( Sy ); // South                3 <+> 7
         X++;  Setim( Dd ); // South East           :/   \:
    Y++;       Setim( Sx ); //       East           4..5..6
    Y++;       Setim( Dd ); // North East
    Y++, X-=2; Setim( Dy ); // North West t'North ~~~~~~~~~~~
    Y--, X--;  Setim( Dx ); // North West t'West     1 ¦ 8
    Y-=2;      Setim( Dx ); // South West t'West    2 +¦+ 7
    Y--, X++;  Setim( Dy ); // South West t'South   --¦•¦--
         X+=2; Setim( Dy ); // South East t'South   3 +-+ 6
    Y++, X++;  Setim( Dx ); // South East t'East     4 ¦ 5
    Y+=2;      Setim( Dx ); // North East t'East
    Y++, X--;  Setim( Dy ); // North East t'North
    Y-=2,X--;
} }
//    Здесь готовится матрица скоростей в каждой морской точке
//    с учетом сферичности Земли.
//      _P - Матрица индексов для определения зон ответственности
//      _A - Матрица для расчета амплитуд волн (не отлажено)
//    В стартовых точках Ft[Lf] производится контроль присутствия
//    отрицательных Времен, и производится смещение счета в (++)
//
static void Prepare()
{  int  y,x;                                      //
 double cFe=-30.0*cos( Chart.Fe )*sqrt( 9.8106 ); // -M/Min/2
  if( Iden ) if( !_P )_P=(float**)Allocate( Ny,Nx*sizeof(float) );
  if( Aht ){ if( !_A )_A=(float**)Allocate( Ny,Nx*sizeof(float) ); Aht=3; }
 Front *L=First.next;
  Fb = (item*)Allocate( (Lb=FrontLength)*sizeof( item ),Fb );
  for( x=0; x<FrontLength; x++,L=L->next )Fb[x]=*L;
  for( y=0; y<Ny; y++ )                 //
  for( x=0; x<Nx; x++ )                 // строится поле скорости длинных волн
  { float &d=_D[y][x],b=Chart[y][x];       // с поправкой на меркаторскую широту
    if( Aht )_A[y][x]=1.0;              //
    if( b<=0 )
    { d=0; if( Iden )_P[y][x]=1.0e+6; } else
    { Point Q={ x,y }; Chart.ItD( Q ); d=sqrt( b )*cFe/cos( Q.y/Radian );
  } }
  if( Iden )
  { L=First.next; for( x=0; x<FrontLength; x++,L=L->next )_P[L->y][L->x]=x+1;
  }
//      Обработка стартового списка фронтовых точек
//
  TerminalFront=0.0;           // время для следующей прорисовки фронта
  TMin=First.next->t;          // все начальные отсчеты приводятся к нулю
  if( TMin<0.0 )
  { L=First.next; for( x=0; x<FrontLength; x++,L=L->next )L->t-=TMin;
  }
}
//    Перераспределение результирующих массивов, где:
//      _T - Разности между первым и вторым расчетом
//      _S - Уточненные разности для новых постов наблюдения
//
static void Result()
{ int  y,x;
  Real Max=P.t;
  if( TMin<0.0 )                        //
  for( y=0; y<Ny; y++ )                 // Восстановление отсчетов Времени
  for( x=0; x<Nx; x++ )_D[y][x]+=TMin;  //
  if( _T!=NULL ){ Max=0.0;
    for( y=0; y<Ny; y++ )
    for( x=0; x<Nx; x++ )
    if( Chart[y][x]>0 ){ float& d=_D[y][x]; d-=_T[y][x]; if( d>Max )Max=d; }
  }
  if( _S!=NULL ){ Max=0.0;
    for( y=0; y<Ny; y++ )
    for( x=0; x<Nx; x++ )if( Chart[y][x]>0 )
    { float& W=_D[y][x];
      Real w=_S[y][x]; if( W>w )W=w; if( Max<W )Max=W;
  } }
  for( y=0; y<Ny; y++ )
  for( x=0; x<Nx; x++ )if( Chart[y][x]<=0 )_D[y][x]=Max; P.t=0.0;
}
//     Выбор погрешности для подходящей к берегу волны в (') минутах времени
//
static Real Quality( int y, int x )
{ if( !_D || !isea( y,x ) )return 0.0;
  {  int m=0;
   Real Tt=_D[y][x],dT=0.0,d;
    if( isea( y+1,x ) ){ d=_D[y+1][x]; if( d<Tt )m++,dT+=Tt-d; }
    if( isea( y,x+1 ) ){ d=_D[y][x+1]; if( d<Tt )m++,dT+=Tt-d; }
    if( isea( y-1,x ) ){ d=_D[y-1][x]; if( d<Tt )m++,dT+=Tt-d; }
    if( isea( y,x-1 ) ){ d=_D[y][x-1]; if( d<Tt )m++,dT+=Tt-d; }
    if( m>1 )dT/=m;
    return dT;
  }
}                                       //
static byte WIND=false;                 // 1:информационное, 2:<Shift+> окна
                                        //
static void Twindow( int Fly )          //
{ if( P.t!=0.0 )Fly++;
  if( WIND ){ Tback(); WIND=1; } color( CYAN ); Twindow( 0,1,15,Fly,2 );
}
static Course InfoLine( int k );
static char* InfList( int k )
{ Point R=Chart.M[k].G; int y,x,D; Chart.D2I( R,y,x ); D=(int)Chart[y][x];
  k=sprintf( Msg,"%-20s %4d%7s",Chart.M[k].cp,int( D+0.5 ),
                 D<=0?" coastal": Solve?DtoH( _D[y][x]/60,2 ):"" );
  if( Solve && D>0 )
  { if( _A )sprintf( Msg+k," %4g",_A[y][x] );
       else sprintf( Msg+k,"%5s",DtoH( Quality( y,x )/60,2 ) );
  } return Msg;
}
#define xcolor( c ) color( Tv.BkColor^( c ) )
//
//  .-------------------------------.
//  ¦    T S U N A M I  W A V E     ¦
//  ¦         Main Procedure        ¦
//  +===============================+
//
void Time_hex()                /// ?? List ??
{                              //
 bool Mark=false,              // Включен режим маркировки точек
      RRay=false;              // Готовность лучевого поля
 int  ans=0xFF,y,x,st=1,FLy=3, // Активное информационное окно
      c=getcolor(),RMod=1;     // None - Rays - Front - Front+Rays
 Point M,Q;                    // Контрольная точка
 // Front --- здесь задаются крайние значения граничных точек
    FrontLength=0;             // статическая установка для двух первых вызовов
    First.prev=Last.next=NULL; // вариант сильно упрощен до всего лишь одного
    First.next=&Last;          // статического списка, где собственно *List для
    Last.prev=&First;          // контрольной точки в разнонаправленном поиске
    First.t=-(Last.t=1.0e+24); //
 Solve= 0;                     //
 WIND = false;                 // 1:информационное, 2:<Shift+> окна
  P.t = 0.0;                   // Смещение во времени маркировки фронта
     Chart.edits( true );      // Автоматическое преобразование
   D=Chart.scale();            // Масштаб выхода на Depth
//_B=Chart;                    // Массив глубин моря
  Ny=Chart.Ny;                 //! одно и единственное касание
  Nx=Chart.Nx;                 //! исходной размерности батиметрии
  Dd=hypot( Sy=Chart.Sy/D,Sx=Chart.Sx/D ); // Длина короткой диагонали
  _D=(float**)Allocate( Ny,Nx*sizeof(float) );
  _A=_R=_S=_T=NULL;
  Fb=NULL;
  _P=NULL;
  Globus=Chart.is_Globus(); // If Chart is Sphere;
  Lb=0;
  Q=M=(Point){ X=int( Nx/2 ),Y=int( Ny/3 ) };
  for( y=0; y<Ny; y++ )
  for( x=0; x<Nx; x++ )_D[y][x]=Chart[y][x];
  Mario_view( Y,X );
  do
  { switch( ans )
    { case 0xF1: if( WIND ){ WIND=false; Tback(); } else
                 { FLy = (Solve!=0)+(_T!=0)+(_R!=0)+(_A!=0)+3;
                   Twindow( FLy ); WIND=true;
                 }
      case 0xFF: break;
      case 0xF2: if( WIND )Tback(); WIND=true;
      { // Переопределение основных расчетных параметров
        const char
             *Name[]={ "Draw  ","   Настройка параметров",
                                "кинематических вычислений.",
                                "   Графическое",
                                "представление результатов.",0 },
             *Text[]={ "F2  ","батиметрия и оси координат",
                       "F3  ","- текущее расчетное поле",
                       "F4  ","- начальное поле изохрон",
                       "F5  ","- последнее поле разностей",
                       "F6  ","- зоны ответственности",
                       "    ","  (по номерам точки + 0.5)",
                       "F7  ","- поле факторов усиления",0 },
             *Plus[]={ "    ","Если <Enter> задан в окне:",
                       "Identity ","- зоны ответственности",
                       "Amplitude ","- факторы усиления",
                       "Mario  ","- тип отметки мореографов",
                       "     ","~~~~~~~~~~~~~~~~~~~~~~~~~~~",
                       "RAY  ","представление лучей/фронтов",
                       "Step  ","точность (шаг) прорисовки",
                       "Arc  ","угол раскрытия линии фронта",
                       "Break ","допустимые переломы лучей",0, };
       int ans=0,
           c=color( CYAN ),ar=int( acos( Rsr )*(Radian*2.0)+0.5 );
       Mlist Menu[] =                                  //
        { { 1,0," Identity " },{ 0,3 }                 //  0
        , { 1,0,"Amplitude " },{ 0,3 }                 //  2
        , { 1,0,"Mario "     },{ 0,3 }                 //  4
        , { 0,0 },{ 0,0 }                              //  6
        , { 1,0 },{ 0,11 }                             //  8 - Ray
        , { 1,4,"Step %4.2lf",&Rst },{ 0,0," cells"  } // 10
        , { 1,3," Arc %3d",   &ar  },{ 0,0," degree" } // 12
        , { 1,3,"Break %2d",  &Nbr },{ 0,0, "angles" } // 14
        };                                             //
       Display Tm( Menu,_R?16:8, 11,-4 );
        do                                            //
        { Tf_Color( Solve>1?LIGHTMAGENTA:LIGHTRED );  // расцветка изохрон
          if( ans>=0xF2 && ans<=0xF7 )Tm.Back();      //
          switch( ans )
          { case 1: if( !_P )Iden^=TRUE; else
                    if( Iden>0 )Iden=-1; else
                    if( Iden<0 )Iden=0; else Iden=1; break;
            case 3: Aht^=TRUE; break;
            case 5: x=color( DARKGRAY );
                    ViewPointMario();
                    if( (Active&6)==6 )Active&=~6;
                    else Active+=2; color( x );
                    ViewPointMario();     break;
            case 9: RMod=( RMod+1 )&3;    break;
            case 0xF2: Map.show( Chart ); break;
            case 0xF3: Dp_Show( _D,0,0,Ny,Nx,BLUE       ); break; // текущие фронты
            case 0xF4: Dp_Show( _T,0,0,Ny,Nx,LIGHTGREEN ); break; // исходное время
            case 0xF5: Dp_Show( _S,0,0,Ny,Nx,GREEN      ); break; // разности
            case 0xF6: Dp_Show( _P,0,0,Ny,Nx,LIGHTGRAY  ); break; // привязки
            case 0xF7: Dp_Show( _A,0,0,Ny,Nx,LIGHTMAGENTA ); break; // оценки высот
           default   : if( ans>' ' )ans=0xF1;
          }
          if(  !_P   )Menu[1].Msg=(char*)(Iden?"YES":"NO"); else
          if( Iden<0 )Menu[1].Msg="Hide"; else
          if( Iden>0 )Menu[1].Msg="Show"; else
                      Menu[1].Msg="Away";
          Menu[3].Msg =(char*)(Aht? "YES": _A?"OFF":"NO");
         const char *MId[]={ "Invisible","in Letter",
                              "by Number","Identify " };
          Menu[5].Msg = MId[ (Active&6)>>1 ];
          Menu[8].Msg =(char*)( RMod==3 ? "Ray:":"RAY " );
          Menu[9].Msg =(char*)
                      (  RMod>2 ?" FRONT only"
                      :  RMod>1 ?"& FRONT"
                      :  RMod>0 ?"only":"switch OFF" );
          Thelp( "F1 Help F2 Depth F3 Current%s%s%s%s",
             _T?" F4 City Times":"",
             _S?" F5 Old Times" :"",
             _P?" F6 Identity"  :"",
             _A?" F7 Amplitude" :""
          );
          if( ans==0xF1 )ans=Help( Name,Text,Plus ); else ans=0;
          if( ans<=0xF1 )ans=Tm.Answer();
        } while( ans!=_Esc ); color( c );
        Rsr = cos( double( ar )/( Radian*2.0 ) );
        if( !Aht  && _A ){ Allocate( 0,0,_A ); _A=NULL; }
        if( !Iden && _P ){ Allocate( 0,0,_P ); _P=NULL; }
        if( !RMod && _R ){ Allocate( 0,0,_R ); _R=NULL; RRay=false; }
      } Mario_view( Y,X );
        if( WIND ){ WIND=false; ans=0xF1; continue; } break;
      case 0xF3:
        if( Solve )
        { float **_W=NULL;
          if( WIND )Tback();
          if( _T==NULL )_W=_T=(float**)Allocate( Ny,Nx*sizeof( float ) ); else
          if( Solve==2 )
          { if( _S==NULL )_S=(float**)Allocate( Ny,Nx*sizeof( float ) ); _W=_S;
          }
          if( _W )for( y=0; y<Ny; y++ )memcpy( _W[y],_D[y],Nx*sizeof( float ) );
          if( WIND ){ WIND=false; ans=0xF1; continue; }
        } break;
      case 0xF4: if( !FrontLength )IncExc(); Mark=false;
                 if( FrontLength )                      //
                 { if( WIND )Tback();  Solve=_T?2:1;    // Тип решения
                                       RRay=false;      //
                   Prepare();                           //
                   if( Tv_getk()&SHIFT )Decide_Oct();   // Запуск расчета
                                   else DecideFour();   // волновых фронтов
                   Y = (int)M.y,                        //
                   X = (int)M.x; Result();              //
                   P.t=0.0;
                   if( WIND ){ WIND=false; ans=0xF1; continue; }
                 } break;
      case 0xF5: if( RRay )++RMod&=3; else
                 if( Solve )
                 { if( WIND )Tback(); RRay=true; Time_Ray();
                   if( WIND ){ WIND=false; ans=0xF1; continue; }
                 } break;
      case 0xF6: if( Chart.Nmp>0 )
                 { static field f={ 1,3,1,18,0 }; int k=-1,b=Tv.BkColor;
                   Thelp( "< %d > F2 Write",Chart.Nmp );
                   bkcolor( BLUE );     Twindow( 1,1,39,1,1 );
                   color( LIGHTGREEN ); Tprintf( " %-20s Depth Time %s",
                                                 "Name",_A?"Heihgt":"Delta" );
                   color( YELLOW ); //,LIGHTGRAY );
                   Tcase( f,37,InfList,InfoLine,Chart.Nmp,k ); Tback();
                   bkcolor( b );
                 } break;
      case 0xF7: { static Point NewPoint;
                   Chart.Seismic_List( NewPoint );
                   Chart.DtI( NewPoint ); Y=(int)NewPoint.y; X=(int)NewPoint.x;
                 } break;
      case 0xF8: {
//                 int i,Lw; list *Fw,F=P;                    color( LIGHTRED );
//                 for( i=0; i<Lf; i++ ){ P=Ft[i]; Cross(); } color( LIGHTMAGENTA );
//                 for( i=0; i<La; i++ ){ P=Fa[i]; Cross(); }
//                 Lw=La; La=Lf; Lf=Lw;
//                 Fw=Fa; Fa=Ft; Ft=Fw; P=F;
                   while( FrontLength )Detach( First.next );
                   for( int m=0; m<Lb; m++ )Fb[m].Insert();
                   P=*(First.next);
                   Cross();
                   if( Iden && _P ){ Allocate( 0,0,_P ); _P=NULL; }
                 } break;
      case _Del: if( Tv_getk()&SHIFT )break;
      case _Ins:
      case _Enter: Chart.mario_utility( Y,X,ans ); /* IncExc( ans ); */ break;
      case '+':
      case '-': Chart.mario_utility(Y,X,ans=='-'?-1:1); M=(Point){X,Y}; break;
      case 't':
      case 'T': sprintf( Msg,"%6.2lf",P.t );
                Thelp( "The Time for Source Points:       in minutes" );
                color( GREEN );
                Tgo( -27,0 );
                if( Tgets( Msg,-6,1 )==_Enter )
                  { sscanf( Msg,"%lf",&P.t ); if( WIND )Twindow( FLy );
                  }
      case 'o': case 'O': IncExc(); break;
      case 'm': ans='M';
      case 'M': Mark=!Mark;
     default  : if( ans<_Enter || ans=='M' ){ if( Mark )IncExc(); } else
                if( unsigned( ans-'0' )<10u )st=ans<'2'?1:st*( ans-'0' ); else
                if( ans>' ' )ans='H';
    }
//      Информационное окно для расчетных параметров
//
   Point p,q;
    if( WIND )
    { Chart.ItD( p=(Point){ X,Y } );
      if( (Tv_getk()&SHIFT)==0 )
      { if( WIND&2 )Twindow( FLy ); } else
      if( ans==_Del ){ Twindow( FLy ); Q=(Point){ X,Y }; } else
      { if( ( WIND&2 )==0 ){ Twindow( FLy+2 ); WIND|=2; }
        if( ans==_BkSp ){ M=Q; Q=(Point){ X,Y }; Y=int( M.y+0.5 );
                          Chart.ItD( p=M );      X=int( M.x+0.5 ); }
       Real Ds,Cs,Cz;       WIND|=0x4;
        Chart.ItD( q=Q );    Tgo( 1,FLy+1 ); color( LIGHTGRAY );
        Sphere( p,q,Cz,Ds ); Tprintf( "dst %6.2lfkm\n",Ds );
        Sphere( q,p,Cs,Ds ); Tprintf( "%6.1lf>\xF8<%.1lf\n",Cs,Cz ); //°
        color( LIGHTGRAY );  Chart.sphere( p,q );
      }
      Tgo( 1,1 );                           color( CYAN );
      Tprintf( "Fi: %-12s\n",DtoA( p.y ) );
      Tprintf( "La:%-12s\n", DtoA( p.x ) ); color( LIGHTGRAY );
      Tprintf( "D[%d,%d]=%1.5g\n",Y,X,Chart[Y][X] );
      if( P.t!=0.0 )
      { color( LIGHTMAGENTA ); Tprintf( "Offset:%s\n",DtoH( P.t/60.0 ) );
      }
      if( Solve )
      { color( GREEN );
        if( _A )Tprintf("Height= %4.2f~\n",_A[Y][X] );
        if( _R )Tprintf("Angle = %3.1f\xF8\n",_R[Y][X]*Radian ); //°
                Tprintf(" Time =%s\n",DtoA( (_T?_T[Y][X]:_D[Y][X])/60.0,2,0xFC ) );
//              Tprintf(" Time = %3.1f'\n",_T?_T[Y][X]:_D[Y][X]);
        if( _T )Tprintf("dTime = %3.1f'\n",_D[Y][X] );
    } }
    if( ans>=_Enter )
    { static char Com[]
          = "%d F1 Inf F2 Show %s F4 Front %s<InsёDel>List <TOM>Mark%s",
      MsgHlp[]=" <H>elp",
      MsgRay[]=" F5 Rays ";
      MsgRay[7]=RRay?'+':'s';
      if( ans==' ' )Ghelp(); else
      if( Solve )Thelp( Com,FrontLength," F3 Save ",MsgRay,_R&&RMod?" <Tab>Trace":MsgHlp );
            else Thelp( Com,FrontLength,""," F6 Results <0..1>Step ",MsgHlp );
    } M.y=Y;
      M.x=X;
//
//    Блок запросов к клавиатуре
//     и динамической прорисовки лучей и фронтов
    {
      const char
             *Name[]={ "Time  ","Кинематические расчеты распространения",
                                "длинных волн в океане, построение карт",
                                "    волновых фронтов и лучей",0 },
             *Text[]={ "F1 ","информационное окно для активной точки",
                       "F2 ","настройка параметров и вывод результатов",
                       "F3 ","сохранение и переход к расчету разностей",
                       "F4 ","запуск процедуры расчета поля изохрон",
                       "<Shift+F4> ","-//- старый 8/16 точечный шаблон",
                       "F5 ","запуск счета/переключение лучевых полей",
                       "F6 ","вывод результатов по постам наблюдения",
                       "F7 ","лист исторических землетрясений и цунами",
                       "F8 ","восстановление списка точек излучения",0 },
             *Plus[]={ "< T > ","- установка точки и времени излучения",
                       "< O > ","- выбор/сброс точки нового излучателя",
                       "< M > ","- пометка всех точек на пути курсора ",
                       "<Enter>   ","выбор ближайшего поста наблюдения",
                       "<+,-,Ins,Del>  ","перебор/создание мореографов",
                       "<Shift+стрелки>   ","расчет углов и расстояний",
                       "<Shift+Del>    ","сброс исходной точки отсчета",
                       "<Shift+BkSp>   ","замена курсора второй точкой",
                       "<Tab> ","прорисовка подвижного луча или фронта",
                       "<Shift+Tab> "," - запись его параметров в файл",
                       "<0,1..9>  ","множитель шага подвижного курсора",0, };
     int a,b,c,d,m; a=b=c=d=0; m=-1;
      if( _R && RMod )
      { xcolor( RRay?LIGHTRED:RED ); setwritemode( XOR_PUT );
        if( RMod<3 )
        { if( RMod!=2 || Rsr<0 )m=Current_Ray( a );
          if( m                )m=Current_Ray( b,Pi );
        }
        if( RMod>1 && m )
        { if( Current_Ray( c,Ph,true ) )Current_Ray( d,-Ph,true );
        } setwritemode( COPY_PUT );
      }
      if( ans=='H' )
        { color( RED ); Tv_cross( M,6 ); ans=Help( Name,Text,Plus ); }
          else ans=0;
      if( ans<0xF1 )
      { if( _P && Iden>0 && Chart[Y][X]>0 )
        { Point W=M,w={ Fb[int(_P[Y][X]-0.99)].x,
                        Fb[int(_P[Y][X]-0.99)].y }; Chart.ItD( W );
          setwritemode(  XOR_PUT );                 Chart.ItD( w );
          xcolor( CYAN );  Chart.sphere( W,w );
          xcolor( GREEN ); ans=Tv_pause( &M,6,st );
          xcolor( CYAN );  Chart.sphere( W,w );
          setwritemode( COPY_PUT );
        } else
        { color( _T==NULL ? MAGENTA: _S==NULL ? GREEN:LIGHTRED );
          ans=Tv_pause( &M,16,st );
      } }
      if( ans==_Esc )
      { Thelp( "... завершить Time?" );
        m=Tv_getc(); if( m!=_Enter && (m|0x20)!='y' )ans=0xFF;
      }
      if( _R && RMod )
      { if( ans!=_Tab )
          { m=xcolor( RRay?LIGHTRED:RED ); setwritemode( XOR_PUT );
          } else m=Tv.Color;
        if( a>0 ){ if( ans==_Tab )color( LIGHTMAGENTA ); Current_Ray( a );     }
        if( b>0 ){ if( ans==_Tab )color( LIGHTMAGENTA ); Current_Ray( b,Pi );  }
        if( c>0 ){ if( ans==_Tab )color( MAGENTA ); Current_Ray( c,Ph,true );  }
        if( d>0 ){ if( ans==_Tab )color( MAGENTA ); Current_Ray( d,-Ph,true ); }
        setwritemode( COPY_PUT );
        color( m );
      } Y=(int)M.y;
    }   X=(int)M.x;
    if( WIND&4 )
    { color( LIGHTGRAY ); Chart.sphere( p,q ); WIND&=~0x4;
    }
  } while( ans!=_Esc );
  while( FrontLength )Detach( First.next );          color( c );
  if( WIND )Tback(); Tf_Color(); Allocate( 0,0,_D ); Ghelp();
                                 Allocate( 0,0,_R ); Allocate( 0,Fb );
                                 Allocate( 0,0,_S );
                                 Allocate( 0,0,_T );
                                 Allocate( 0,0,_A );
}
//      Отклик на строку в списке мореографных станций
//              с отработкой команды записи протокола
//
static Course InfoLine( int k )
{ int c=color( YELLOW );
  Course ans;
  Point R=Chart.M[k].G; Chart.DtI( R ); ans=Tv_pause( &R );
  color( c );           //
  if( ans==0xF2 )       // Процедура записи сводных данных в файл
  {                     //
   int i,c=color( LIGHTGRAY );
    Msg[0]='~'; scpy( Msg+1,sname( Chart.InfName ) );
    Msg[8]=0;   fext( Msg,"mif" );
        Thelp( "?FileName:" );
        Tgo( -10,0 );
    if( Tgets( Msg,-72,int( fname( Msg )-Msg ) )!=_Enter )ans=_Esc; else
    { FILE *F;
      OemToCharBuff( Msg,Msg,strcut( Msg )+1 );
      if( (F=Topen( Msg,"at","mif" ))!=NULL )
      { time_t t=time( 0 );
        Thelp( "Saving: %s",Msg );
        fprintf( F,"\n<< ANI >> Ocean Wave Investigation, %s"
                   "~~~~~~~~~"
                   "\nFile: %s\n~~~~: %s"
                   "\nName: %s\n~~~~: %s\n"
                   "\nFrame Direction is %-11s",ctime( &t ),
                 Chart.InfName,Chart.Name,Chart.Msg[0],Chart.Msg[1],
                !Chart.Cs?"North":DtoA( Chart.Cs ) );
        fprintf( F,"\nLatitude of Scale:%-11s Size [%dx%d]",
                 DtoA( Chart.Fe*Radian ),Ny,Nx );
        R=0.0; Chart.ItD( R );
        fprintf(F,"\n  Basis Latitude :%-11s   dY %lgkm",DtoA( R.y ),Sy/1000 );
        fprintf(F,"\n       Longitude :%-11s   dX %lgkm",DtoA( R.x ),Sx/1000 );
        if( Lb>0 )
        { int j,l;
          fprintf( F,"\n\nList of %d Source's Points\n"
                         "~~~~~~~~~~~~~~~~~~~~~~~~~~",Lb );
          for( l=72,i=0; i<Lb; i++ )
          { item &A=Fb[i];
            j=sprintf( Msg,"%s(%2d,%-2d)",!A.t?"":DtoH( A.t/60,2 ),A.y,A.x );
            if( Lb<=12 )
            { if( !i )fprintf( F,"\nLatitude-Longitude  Depth    Time ( y,x )" );
              R.x=A.x; R.y=A.y; Chart.ItD( R );
                       fprintf( F,"\n%3d %7s",i+1,DtoA( R.y,2 ) );
                       fprintf( F,"%8s %5.5g%16s",
                          DtoA( R.x,2 ),Chart[A.y][A.x],Msg );
            } else
            { if( l>=72 ){ fprintf( F,"\n" ); l=0; }
              l+=fprintf( F,j<8?"%8s":"%16s",Msg );
        } } }
        if( Solve && Chart.Nmp>0 )
        { MARIO *M=Chart.M;
          fprintf( F,
          "\n\n Latit  Longit %3d Mario-Stations   Src Depth %s Height St-Time Direct"
          "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
               Chart.Nmp,_T==NULL?"Times ":"Differ" );
          for( int k=0; k<Chart.Nmp; M++,k++ )
          { Real D;
           int y,x; Chart.D2I( M->G,y,x );
            fprintf( F,"\n%7s",     DtoA( M->G.y,2 ) );
            fprintf( F,"%-8s %-20s",DtoA( M->G.x,2 ),M->cp );
            if( ubnd(y,Ny) || ubnd(x,Nx) )fprintf( F," alien" ); else
            if( (D=Chart[y][x])<=0.0 )    fprintf( F,"%9.5lg",D ); else
            { Real dT;
              fprintf( F,_P?"%3.0f":"  -",_P?_P[y][x]:0.0 );
              fprintf( F," %5.5lg%8s",D,DtoA( _D[y][x]/60,2,':' ) );
              fprintf( F,_A?"%6.3f":"  --- ",_A?_A[y][x]:0 );
              if( _T )dT=_T[y][x]; else dT=Quality( y,x );
                      fprintf( F,"%8s",DtoA( dT/60,2,':' ) );
              if( _R )fprintf( F,"%8s",DtoA( _R[y][x]*Radian,2 ) );
            }
          }
        } fprintf( F,"\n\n(c)1995 V.Khramushin, Sakhalin, Russia\n" );
          fclose( F );
             Thelp( "< %d > F2 O'key",Chart.Nmp );
      } else Thelp( "< %d > F2 Write",Chart.Nmp );
    } color( c );
  } return Course( ans<0xF0 && ans!=_Enter ? ans:0 );
}
#if 0
//     Здесь восстанавливается локальная скорость
//                 по базису временных разностей:
//      Qx = Tx/Sx;
//      Qy = Ty/Sy  --  Cx = Qx/( Qx*Qx+Qy*Qy )
//                      Cy = Qy/( Qx*Qx+Qy*Qy )
//
inline void Front_Wave( float Qy, float& Cy, float Qx, float& Cx )
{ float C=Qx*Qx+Qy*Qy;  Cy += Qy/C;
                        Cx += Qx/C;
}
//      Вычисление градиента скорости
//
int Wave_Gradient( float Py, float Px, float& Cy, float& Cx )
{ int y=Py+0.5,y1,
      x=Px+0.5,x1;
  if( ubnd( y,Ny ) || ubnd( x,Nx ) || _D[y][x]>=0 )return 0;

  if( (Py-y>=0 || y==0) && y<Ny-1 )y1=y+1; else y1=y-1;
  if( (Px-x>=0 || x==0) && x<Nx-1 )x1=x+1; else x1=x-1;

  Cy=(_D[y1][x]-_D[y][x])/Sy; if( y1<y )Cy=-Cy;
  Cx=(_D[y][x1]-_D[y][x])/Sx; if( x1<x )Cx=-Cx; return 1;
}
     float Cf=0,Cb=0,C;

      Y++;      M+=Height(); C=(Tf-T)/Sy; if( C<0 )Cb+=C*C; else Cf+=C*C;
           X--; M+=Height(); C=(Tf-T)/Dd; if( C<0 )Cb+=C*C; else Cf+=C*C;
      Y--;      M+=Height(); C=(Tf-T)/Sx; if( C<0 )Cb+=C*C; else Cf+=C*C;
      Y--;      M+=Height(); C=(Tf-T)/Dd; if( C<0 )Cb+=C*C; else Cf+=C*C;
           X++; M+=Height(); C=(Tf-T)/Sy; if( C<0 )Cb+=C*C; else Cf+=C*C;
           X++; M+=Height(); C=(Tf-T)/Dd; if( C<0 )Cb+=C*C; else Cf+=C*C;
      Y++;      M+=Height(); C=(Tf-T)/Sx; if( C<0 )Cb+=C*C; else Cf+=C*C;
      Y++;      M+=Height(); C=(Tf-T)/Dd; if( C<0 )Cb+=C*C; else Cf+=C*C;
      --Y; --X;
//
//      Краткое описание программы по запросу с клавиатуры
//
static int Help()
{ const char
         *Name[]={ "TIME "," Rays & Fronts & Coefficients",
                          "Tsunami Investigation Warning",0 },
         *Text[]={ "F1  ","Window with Marker Information",
                   "F2  ","Entrance to Drawing & Settings",
                   "F3  ","Save Field for the Differences",
                   "F4  ","Start the First Order Solution",
                   "F5  ","Compute Field for Drawing Rays",
                   "F6  ","Observation Point Results List",
                   "F7  ","Seismo & Tsunami Activity List",
                   "F8  ","Restore Previous Sources",0 },
         *Plus[]={ "< T > ","The Start Time Settings and",
                   "< O > ","Marking the One Source Point",
                   "< M > ","XOR-Marking Mode for Sources",
                   "<Enter> ","Choise Nearest Mario Point",
                   "<+-,Ins,Del> ","for Observation Point",
                   "<Shift+Arrow> ","Distance Measurement",
                   "<Shift+Delete> ","Remove Second Point",
                   "<Shift+BkSp>  ","Twice Points Replace",
                   "<Tab> ","Drawing Trace of Ray & Front",
                   "<0,1..9> ","Marker Step Multiplier",0 };
  return Help( Name,Text,Plus );
}
Here Help_Drawing()
{ const char
         *Name[]={ "TIME Draw","Task Conditions Settings",
                           "and Stored Fields Drawing",0 },
         *Text[]={ "F1 ","This Help",
                   "F2 ","Drawing Batimetry and Axis" ,
                   "F3 ","- Current Computation Field",
                   "F4 ","- First Time Front    Field",
                   "F5 ","- Previous Difference Field",
                   "F6 ","- Bound Relation to Sources",
                   "   ","  by Set Point Number + 0.5",
                   "F7 ","- Amplitude Coefficients",0 },
         *Plus[]={ "","If <Enter> Pressing in Window:",
                   "Identity  ","Point Recognize Mode",
                   "Amplitude ","Coefficient Compute" ,
                   "Mario  ","Point Name Presentation","","",
                   "RAY -"," Fast Rays Drawing Mode",
                   "Step "," for One Ray Presentation" ,
                   "Arc  "," of Wave Front Expansion"  ,
                   "Break ","Ray Arrest Counter",0 };
  return Help( Name,Text,Plus );
}
//!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
       if( Chart.Active&as_Globus )     //! замыкание географических параллелей
       if( !x || x==Chart.Nx-1 )
       { Front *Pls = new Front;
         if( !x )Pls->x=Chart.Nx-1; else Pls->x=0; Pls->y=y; Pls->t=t;
         Pls->prev=prev->next;
         Pls->next=next; next=Pls; ++FrontLength;
       }
#endif
