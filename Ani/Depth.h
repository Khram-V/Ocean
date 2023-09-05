//
//      Depth                                                      /90.10.22/
//      Комплекс процедур просмотра, корректировки и трансформации батиметрии
//
#ifndef _Depth_Hxx
#define _Depth_Hxx
#include <StdIO.h>
#include "..\Matrix\Basis.h"
#include "..\Graphics\Window.h"
#define _English false
//
//    Эллипсоид Красовского для морских карт России
//      a = 6378245 m           Сфероид Красовского
//      b = 6356863.0188 sn\ .
//      c = 6399698.9018 ew/    (b+c)/2 = 6367554.0094
//      e = 298.3
//      g = 9.8106
//           1855.35626248109543 Сфероидальная миля
#define Mile 1852.24637937659918                     // Sea Mile равнообъёмная
#define Radian 57.295779513082320876798154814105     // 180 / Pi
#define Pr      0.01745329251994329576923690768489   //  Pi / 180
#define Pd      6.283185307179586476925286766559     //  Pi x 2
#define Pi      3.1415926535897932384626433832795    //          = M_PI
#define iP      0.31830988618379067153776752674503   //  1 / Pi  = M_1_PI
#define Ph      1.5707963267948966192313216916398    //  Pi / 2  = M_PI_2
#define wD      1.4544410433286079807697423070738e-4 // 4*Pi/1440/60 = 2*w
//#define wE    6.6314559621623056570368234738548e-4 // 180/Pi/1440/60 = w
//#define wD    0.00132629119243246113140736469477   // 360/Pi/1440/60 = 2*w
//      e       2.718281828459045235360287471352662497757247           = M_E
#define Mercator "Mercatory"
#define Geograph "Geography"
#define UnKnown  "UnKnown"
inline Real Merkatory( _Real Fi ){ return log( tan( ( Fi+Ph )/2.0 ) ); }
inline Real Geography( _Real Rm ){ return 2.0*atan( exp( Rm ) )-Ph; }
extern unsigned Active;   // 1:Read; 2:Edit
                // 0x0001 готовность батиметрии
                // 0x0006 Empty, Letters, Numbers, Identify (4) для Mario
                // 0x0020 произведен сдвиг карты в F4 (сброс при считывании)
                // 0x0018 был считан новый файл в F4  (сброс с перерисовкой)
                //    - 8 = 0 - Произведена перерисовка внутри программы F4
#define as_Globus  0x0100  // Признак замыкания параллелей
#define as_Simula  0x1000  // Признак активизации численного моделирования
                // 0x8000 Процесс восстановления сохраненного расчета
                         //
struct MARIO{ Point G;   // географические координаты точки
             char *cp;   // текстовое название пункта
             Real D,A;   // дистанция и пеленг на ближайший берег
            };           //
class Depth              // Главный описатель входного файла батиметрии
{                        // ==============================================
 float **__Depth;        // меркаторская/географическая матрица батиметрии
 Basis  Geo;             // связь индексов с координатами
 Point  Base;            // Latitude & Longitude                10
//protected:
public: operator float**(){ return __Depth; }
 Real   operator()( _Point R ){ return Value( __Depth,R.x,R.y,Nx,Ny ); }
 unsigned Type;          // 0:Text; 1:Byte; 2:Integer; 4:Float   4
//      Dat;             -- 0:Var; 1:Line; 2:Field; 3:Space      5
 char   Ms;              // '-minute d-degree k-kilometre m-metre
 bool isMercator;        // '=>G - GeoGraphy else M- Mercatory  -/-
 int    Ny,Nx;           // Размерности всего массива батиметрии в памяти
 Real   Sy,Sx,           // Grid's Steps [rad]                   9
        Fe,              // Average (middle) Latitude of chart  -/-
        Cs,              // Meridian's Course [ °-degree ]      -/-
        Max,Min;         // Bathimetry Extrema
 static char    InfName[MAX_PATH],   // Information File Name
  Msg[2][MAX_PATH],Name[MAX_PATH];   // Comment & Bathimetry File Name 1-3
 static int Nmp;                     // Number of Marigrams            11
 static MARIO *M;                    // List of Marigrams              12..
 static FILE *Fs;                    // Локальный файл
 Depth(): __Depth( NULL )            // Constructor cFe=1.0
                 { Nx=Ny=0; Sy=Sx=1.0; Fe=Cs=Base.y=Base.x=0.0; }
  void allocate( int sY=0,int sX=0 );
 double scale( );                    // Масштаб по символу Ms->Map(radian)
 Depth& basis( bool=false );         // Определение базиса
 Depth& read ( char* );              // Чтение информационного файла
  void  write( );                    // Запись данных
  void  write( const char* );        // Запись информационного файла
  bool  edits( bool=false );         // Программа преобразования
  void ItD( Point& W );              // Пересчет индексов в гео.координаты
  void D2I( Point& W );              // -- без отсечений и изъятий
  void DtI( Point& );                // Координаты в индексы по замкнутой сфере
  void D2I( Point Q,int &y,int &x ); // Координаты -> Индексы
  void DtI(_Point P,int &y,int &x ); // - с контролем и без ухода за границы
  void sphere( Point,Point,bool=false ); // Соединение линией от P к Q
  bool is_Globus();
  void mario_control();                  // Обработка мореограмм
  int  mario_utility( int&,int&,int=0 ); // 0 весь список, иначе mode:
                                         // +- следующий, <Enter>ближайший
                                         // ins - новая точка, del - удаление
  void find_coast( MARIO& );             // Дистанция и расстояние до берега
  void move_to_coast( MARIO&, Real D, int knots ); // Перенос точки за Depth
                                                   //   на удаление до knots
  int Seismic_List();               // Прорисовка списка очагов цунами на карте
  int Seismic_List( Point& Geo );   //     с выводом положения последнего очага
  int Seismic_List( Point& Geo,     // Географические координаты
                    Event& Jul,     // Юлианский день и час от начала суток
                    Real& Mag,      // Магнитуда
                    Real& Hei );    // Глубина очага землетрясения в км
private:                            //
 void  projection( byte Sym );      // Изменение картографической проекции
 void  medit();                     // Ручное редактирование карты
 char* open( char*,const char*,const char* ); // Для файла Fs( name,ext,mode )
 void  open( char regime='r' );     // Для файла Fs данных
 void  infout( const char* );       // Вывод текста файла .inf
 void  remove();                    // Закрытие файла Fs с удалением
 void  close ();                    // Просто закрытие файла Fs
//FILE* save( const char* =0 );     // Сохранение и
//int   restore( char* );           //  восстановление батиметрии
};                      //
extern Depth Chart;     //! Ссылка на главное/исходное определение батиметрии
                        //
struct Screen:          // Объекты и операции интерактивного управления счетом
  Window,               // Окно изображения с активированным указателем мышки
  field                 // - видимый сегмент географической карты на экране
{ int Mx,My;            // - максимальная размерность отображаемой батиметрии
  Screen(): Window(),field( (field){ 0,0,0,0,0 } ),Mx( 0 ),My( 0 ){}
  Field Focus(){ F=(Field){ Jx,Jy,Lx,Ly }; return Window::Focus(); }
  int Console( bool );  // команды от клавиатуры с затененной мышкой - 1 hold
  void window();        // установка пропорционального отображения
  void axis( Depth &Dp=Chart, int=CYAN ); // Разметка осей CYAN=3
  void view( float** ); // Просмотр файла батиметрии
  void show( float** ); // Рисование любых изобат
};                      //
extern Screen Map;      //! Сектор карты с представлением на графическом экране
extern Button All;      //! Кнопочка для возврата к полному/исходному масштабу
extern char Str[MAX_PATH*2]; // Строка для различных сообщений
extern const char *_Month[], // Списки текстовых имен месяцев
                  *_Week[];  //      и календарных дней недели
//!
//!     здесь концентрируются консольные запросы,
//!      собранные для единообразного сегментирования исходной карты
//!
//
//      Расстояние и направление между двумя точками
//              Cs[°]  направление из точки Op на Np
//              Ds[km] расстояние между точками
//
void     Sphere( Point Op, Point Np, Real& Cs, Real& Ds );
Point    Sphere( Point Op, Real Cs, Real Ds );
void Geo_Circle(_Point Op,_Real Ds );
//
//   Сплайн интерполяция на равномерном разбиении аргумента
//
Real ReLine( Real *Y, int N, Real Ar, int Offs=sizeof( Real ) );
                                      //
void ViewPointMario( int=LIGHTBLUE ); // Простая рисовка Mario точек
void Thelp( const char*,... );        // Стандартная подсказка
                                      //
inline void Mario_view( int& y,int& x ){ Chart.mario_utility( y,x,0 ); }
//
//   Программа управления выводом изолиний
//
void Dp_Show( float **_D, int Jy,int Jx, int Ly,int Lx, int=BLUE );
//
#endif
