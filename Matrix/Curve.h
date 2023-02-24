//
//      Curve.h                                              /91.04.12/
//      ��������� �������� ������� "Point" ��������
//
#ifndef __CURVE_TYPE
#define __CURVE_TYPE
#include "..\Type.h"
class Curve                //
{     Point *_C;           // ����� �������
      Real *_2;            // ������ ����������� � ����� �������
public: int N;             // ��������� ����� �����
  Curve( int=0 );          // Constructor
 ~Curve();                 // Destructor
 Curve& allocate( int=0 ); // ������������� ������
 Curve& free();            //
        operator Point* () { return _C;    } // ������ ������ � �������
 Point& operator[]( int k ){ return _C[k]; } // ����� ����� �� �������
 int    find( Real );            // �������� ����� ������ �������
 Real   operator()( Real );      // �������� ��� ������ ������������
 Curve& operator+=( Point& );    // ������������ �������� ����
 void Linear(){ allocate( N ); } // ������� � �������� ������������
 void Extreme( Field &_Fm );     // ����� ����������� �� �������� ������
 Real value( Real,int=1 );       // ��������� ������������ �� ��������
 void SpLine();                  // ����������� ������-������������
//oid SpLine( Real Y1,Real Yn ); // C ����������� � ������ � � ����� �������
};                               //
#endif
