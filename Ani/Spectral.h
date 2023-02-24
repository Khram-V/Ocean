//
//  Spectral.h                                   Ver 3.2  90.12.23 - 2015.04.08
//
#include "..\Graphics\Tv_Graph.h"
/**
        Простой объект для реализации числового ряда
        с постоянным шагом аргумента, контроль физической
        размерности аргумента здесь не поддерживается.
*/
//            -- нет имен фалов или идентификаторов в названиях исходных рядов
//            -- проверить правильность записи имен в пространственном переборе
//
struct Series //
{ Real *_V;  // Искомый числовой массив
  int    N;   // Размерность оперативного массива
              // ~~~~~~~~~~~~~~~~~~~~~~~~ //
 Series( int s=0 ){ N=0; allocate( s ); } // Constructor
~Series(){ if( N )allocate( 0 ); };       // Destructor
        operator Real*(){ return _V; };   // Прямой доступ к памяти
 Real* allocate( int s=0 )                // Распределение памяти
       { _V=( Real* )Allocate( s*sizeof( Real ),N?_V:0 ); N=s; return _V; }
 Real operator ()( Real );   // Интерполяция
 Real value( Real, int );    // Интерполяция по Лагранжу
};
//
//     Собственно исходные и расчетные ряды и все под кличкой "Vector"
//
struct Vector: Series        // Функция-ряд на равномерном интервале
{ Real Min,Max;              // - экстремумы функции
  int  Start,End;            // Границы изображения в индексах
  field f;                   // Размеры графического окна
  const char *IdenS;         // Надпись на прорисовке графика
  bool Spec;                 // Признак спектрального ряда
   Vector& allocate( int s ) // Распределение памяти
         { Series::allocate( s ); Start=0; End=max( N-1,1 ); return *this; }
   void Get( const char *s="",char='s' ); // Ввод данных из файла
   void Out( int S=20,int H=60,int J=-64,int L=0 ){ f.Jy=S,f.Ly=H,f.Jx=J,f.Lx=L; }
                                     // Полоса терминала в процентах
   void Axis( int=0,int=0 );         // Границы выборки ряда
   void Axis_Series( Real dY );     // Собственно разметка графического поля
   void Show();                      // Вывод массива на терминал
   void View();                      // Просмотр функции
   void Extrem();                    // Расчет экстремумов для функции
   Vector& Plus( Vector& Pl );       // Расширение списка просмотра
   Vector( const char *Id, bool Sp=false, Real mn=0, Real mx=0  ) // Constructor
     { IdenS=Id; Start=0; End=1; Out(); Spec=Sp; Min=mn; Max=mx; }
};
struct Sio
{                       //
 char  Ident[MAX_PATH], // Текстовый идентификатор
       Name[MAX_PATH];  // Имя файла
 int   Fl,J,K, L;       // Длина в байтах, отступ и шаг и длина выборки
 byte  Typ,             // 0-Text; 1-Byte;  2-Integer; 4-Float
       Mod;             // 0-Read; 1-Write; 2-Added;   3-Update
 bool  Act;             // Признак сохраненного изображения
 viewporttype v,vi;     //
 FILE *Fs;              // Единый (временный) указатель файла
 void Fput( int Typ, Real w );
 void MENU   ( Series& );
 void SView  ( Series& );
 void Dread  ( Series& );
 void Dwrite ( Series& );
 void Example( Series& );
};
