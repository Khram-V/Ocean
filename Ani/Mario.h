//
//      Простой объект для реализации числового ряда
//              с постоянным шагом аргумента
//
#include "..\Graphics\Tv_graph.h"
#define Mario_Title_Length 20

struct Mario            //
{   Real *V,dt,         // Искомый числовой массив и его дискретность [час] ~~=
          Longitude,    // ¦ Географические координаты [deg]                                                  8
          Latitude,     // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                                  8
          Min,Mid,Max,  // Экстремумы, среднее и
          Level,        //      уровень для изображения
          Depth,        //++ глубина моря                                     4
          Bearing,      //++ пеленг на берег [°]                              4
          Distance;     //++ отстояние от берега [км] (0 - точка отключена)   4
    Event JT;           // ¦ Юлианская дата, время в часах и                 12
    int   N;            // Размерность оперативного массива                   4
    char  Title[Mario_Title_Length]; //+¬ Название                           20
    fixed Fmt,          // 0-float; 1-byte; 2-int; 3-три; 4-word              1
          Typ,          // 1 Series; 2 Curve; 3 Tide...                       1
          flag;         // Флаги операций
                        //    1 - ручная оперативная отметка ряда
                        //    2 - снятие ряда из изображения на экране
 void Init();           // 0x10 - исключение из списка в List
     Mario(){ Init(); } // 0x10<<1 - сохранение при перезаписи в List
                        // ============ //
        operator Real*(){ return V; }; // Прямой доступ к памяти
  Real& operator[]( int );            // С контролем границ
  Real  operator()( const Event& );  // Интерполяция по Лагранжу
  Real  value( const Event&, int ); // а также по заданной степени
                                   //
  Real* allocate( size_t=0 );     // Распределение памяти
  void  free();                  //
  void  Initial();               // Параметры мореограммы              /M_ImpEx
  void  Cross();                 // Отметка для групповых операций
//void  Spectrum( fixed Vv );    // Прорисовка спектра в заданном окне
//
//      Sio:: пакет операций ввода/вывода
//
        operator char*();                   // Имя последней операции В/В
 Mario& _IO_( const char *S="", char mode='s' ); // Обмен данными с диском
 Mario& operator <<= ( const char *S ){ return _IO_( S,'r' ); }
 Mario& operator >>= ( const char *S ){ return _IO_( S,'w' ); }
};                   //
extern Event  Tm;    // Начало,
extern double dT,Tn; // шаг и длина во времени [час]
extern int Nb,Nx,    // Длина блока и импорта
              Pw;    // Степень встроенной интерполяции
extern fixed  Vv;    // Параметры визуализации
                     //  X0 - горизонтальная и
                     //  0X - вертикальная шкала
                     //   8 - 1-полное гашение
                     //   4 - 1-сетка на главных отметках
                     //   2 - 0-абсолютная, 1-относительная разметка
                     //   1 - 1-снятие числовых подписей
                     // 100 - 1-снимает основную подпись времени
                     // 200 - 1-принудительная надпись названия
                     //
extern class Series_Array
{ Mario **Array;                    //
public: int n,k;                    // Количество и номер текущей
  Series_Array(){ Array=0; n=k=0; } //
  Mario& operator[]( int k ){ return Array[k][0]; };
  void allocate( unsigned );
} Ms;

#define Nm (Ms.n)
#define Km (Ms.k)
//
//      Блок определения исполнительных функций
//       для списка активных мореограмм
//
void m_Tide();          // Построение приливного ряда                /M_ImpEx
void m_Karta();         // Прорисовка рядов и спектров на карте      /M_Karta
 int m_GetCom();        // Интерпретатор команд управления
 int m_Segment();       // Визуализация выбираемого окна
void m_Move();          // Перенос блока со снятием отметок          /M_ImpEx
void m_Copy();          // Построение копии мореограммы              /M_ImpEx
void m_Mark( int=0 );   // Отметка или перенос текущей метки         /M_View
void m_List();          // Просмотр списка рядов в памяти и на диске /M_IO
void m_Show();          // Просмотр ряда с корректировкой
void m_View();          // Изображение активного списка
void m_ReView( int=0 ); // Изображение всех видимых данных           /M_ImpEx
void m_Edit();          // Установка параметров выборки
void m_Read();          // Чтение блока стандартного блока данных    /M_IO
void m_Write( int=0 );  // Стандартная запись данных                 /M_IO
void m_InPut();         // Чтение инородной мореограммы              /M_ImpEx
void m_OutPut();        // Экспорт одного ряда                       /M_ImpEx
void m_Delete();        // Удаление                                  /M_ImpEx
                        //
void Function();        // Функции преобразования рядов
void Filtration();      // Алгоритмы фильтрации и спец-функции
void Mario_main();      // ~~~~~~~~~~~~~~.
void MinMax( Real*,int,Real&,Real& );   // в Mario.cpp
                                        //
extern char Months[12][10],             //
            Weeks[7][12];               // Mario.cpp
extern Bfont Tw;                        //


