//
//  Spectral.h                                   Ver 3.2  90.12.23 - 2015.04.08
//
#include "..\Graphics\Tv_Graph.h"
/**
        ������� ������ ��� ���������� ��������� ����
        � ���������� ����� ���������, �������� ����������
        ����������� ��������� ����� �� ��������������.
*/
//            -- ��� ���� ����� ��� ��������������� � ��������� �������� �����
//            -- ��������� ������������ ������ ���� � ���������������� ��������
//
struct Series //
{ Real *_V;  // ������� �������� ������
  int    N;   // ����������� ������������ �������
              // ~~~~~~~~~~~~~~~~~~~~~~~~ //
 Series( int s=0 ){ N=0; allocate( s ); } // Constructor
~Series(){ if( N )allocate( 0 ); };       // Destructor
        operator Real*(){ return _V; };   // ������ ������ � ������
 Real* allocate( int s=0 )                // ������������� ������
       { _V=( Real* )Allocate( s*sizeof( Real ),N?_V:0 ); N=s; return _V; }
 Real operator ()( Real );   // ������������
 Real value( Real, int );    // ������������ �� ��������
};
//
//     ���������� �������� � ��������� ���� � ��� ��� ������� "Vector"
//
struct Vector: Series        // �������-��� �� ����������� ���������
{ Real Min,Max;              // - ���������� �������
  int  Start,End;            // ������� ����������� � ��������
  field f;                   // ������� ������������ ����
  const char *IdenS;         // ������� �� ���������� �������
  bool Spec;                 // ������� ������������� ����
   Vector& allocate( int s ) // ������������� ������
         { Series::allocate( s ); Start=0; End=max( N-1,1 ); return *this; }
   void Get( const char *s="",char='s' ); // ���� ������ �� �����
   void Out( int S=20,int H=60,int J=-64,int L=0 ){ f.Jy=S,f.Ly=H,f.Jx=J,f.Lx=L; }
                                     // ������ ��������� � ���������
   void Axis( int=0,int=0 );         // ������� ������� ����
   void Axis_Series( Real dY );     // ���������� �������� ������������ ����
   void Show();                      // ����� ������� �� ��������
   void View();                      // �������� �������
   void Extrem();                    // ������ ����������� ��� �������
   Vector& Plus( Vector& Pl );       // ���������� ������ ���������
   Vector( const char *Id, bool Sp=false, Real mn=0, Real mx=0  ) // Constructor
     { IdenS=Id; Start=0; End=1; Out(); Spec=Sp; Min=mn; Max=mx; }
};
struct Sio
{                       //
 char  Ident[MAX_PATH], // ��������� �������������
       Name[MAX_PATH];  // ��� �����
 int   Fl,J,K, L;       // ����� � ������, ������ � ��� � ����� �������
 byte  Typ,             // 0-Text; 1-Byte;  2-Integer; 4-Float
       Mod;             // 0-Read; 1-Write; 2-Added;   3-Update
 bool  Act;             // ������� ������������ �����������
 viewporttype v,vi;     //
 FILE *Fs;              // ������ (���������) ��������� �����
 void Fput( int Typ, Real w );
 void MENU   ( Series& );
 void SView  ( Series& );
 void Dread  ( Series& );
 void Dwrite ( Series& );
 void Example( Series& );
};
