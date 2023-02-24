//
//	Curve.Hxx				  /91.04.12/
//	Несколько программ анализа "Point" векторов
//
#ifndef __CURVE_TYPE
#define __CURVE_TYPE
#include "..\Type.Hxx"

class Curve
{				//
 Point *_C;			// Адрес массива
 float *_2;			// Вторая производная в узлах Сплайна
				//
public:				//
 fixed N;			// Индексная длина линии
 Curve( fixed=0 );		// Constructor
~Curve();			// Destructor
				//
 Curve& allocate( fixed=0 );	// Распределение памяти
 Curve& free();			//
	operator Point* () { return _C;    } // Прямой доступ к вектору
 Point& operator[]( int k ){ return _C[k]; } // Выбор точки по индексу
	find( float );		// Двоичный поиск левого индекса
 float  operator()( float );	// Линейная или Сплайн интерполяция
 Curve& operator+=( Point& );	// Паралленльное смещение ряда
 void SpLine( float Yx1=1e32,	// Активизация Сплайн-интерполяции
	      float YxN=1e32	// Производная в начале и в конце отрезка
	    );			//
 void Linear(){ allocate( N ); }// Возврат к линейной интерполяции
 void Extreme( Field &_Fm );	// Выбор экстремумов по исходным точкам
 float value( float,int=1 );	// Степенная интерполяция по Лагранжу
};				//
struct Surface			// Простая криволинейная поверхность
{      Curve _A,		// Аргументы для одномерных функций
	    *_S;		// Список сплайновых (ломаных) кривых
       fixed  M,		// Количество интерполирующих кривых
	    _sp;		// Признак сплайн интерполяции
       float dF1,dFN;		// Производные на крайних кривых
				//
 Surface( fixed=0 );		// Constructor
~Surface();			// Destructor
 Surface& allocate( fixed=0 );	// Распределение памяти
 Surface& free();		//
 Curve& operator[]( int k ){ return _S[k]; } // Выбор кривой по индексу
 float  operator()( float,float ); // Линейная или сплайн интерполяция
 void Linear(){ allocate( M ); }  // Возврат к линейной интерполяции
 void SpLine( float Yx1=1e32,	 // Активизация Сплайн-интерполяции
	      float YxN=1e32	// Производная в начале и в конце
	    );			// поверхности (по нормали к кривым)
 void Extreme( float &Xmin, float &Xmax,   // Аргумент кривых
	       float &Ymin, float &Ymax,   // Ширина полосы
	       float &Zmin, float &Zmax ); // Значения функций
};
#endif
