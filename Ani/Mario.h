//
//      ������� ������ ��� ���������� ��������� ����
//              � ���������� ����� ���������
//
#include "..\Graphics\Tv_graph.h"
#define Mario_Title_Length 20

struct Mario            //
{   Real *V,dt,         // ������� �������� ������ � ��� ������������ [���] ~~=
          Longitude,    // � �������������� ���������� [deg]                                                  8
          Latitude,     // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                                  8
          Min,Mid,Max,  // ����������, ������� �
          Level,        //      ������� ��� �����������
          Depth,        //++ ������� ����                                     4
          Bearing,      //++ ������ �� ����� [�]                              4
          Distance;     //++ ��������� �� ������ [��] (0 - ����� ���������)   4
    Event JT;           // � ��������� ����, ����� � ����� �                 12
    int   N;            // ����������� ������������ �������                   4
    char  Title[Mario_Title_Length]; //+� ��������                           20
    fixed Fmt,          // 0-float; 1-byte; 2-int; 3-���; 4-word              1
          Typ,          // 1 Series; 2 Curve; 3 Tide...                       1
          flag;         // ����� ��������
                        //    1 - ������ ����������� ������� ����
                        //    2 - ������ ���� �� ����������� �� ������
 void Init();           // 0x10 - ���������� �� ������ � List
     Mario(){ Init(); } // 0x10<<1 - ���������� ��� ���������� � List
                        // ============ //
        operator Real*(){ return V; }; // ������ ������ � ������
  Real& operator[]( int );            // � ��������� ������
  Real  operator()( const Event& );  // ������������ �� ��������
  Real  value( const Event&, int ); // � ����� �� �������� �������
                                   //
  Real* allocate( size_t=0 );     // ������������� ������
  void  free();                  //
  void  Initial();               // ��������� �����������              /M_ImpEx
  void  Cross();                 // ������� ��� ��������� ��������
//void  Spectrum( fixed Vv );    // ���������� ������� � �������� ����
//
//      Sio:: ����� �������� �����/������
//
        operator char*();                   // ��� ��������� �������� �/�
 Mario& _IO_( const char *S="", char mode='s' ); // ����� ������� � ������
 Mario& operator <<= ( const char *S ){ return _IO_( S,'r' ); }
 Mario& operator >>= ( const char *S ){ return _IO_( S,'w' ); }
};                   //
extern Event  Tm;    // ������,
extern double dT,Tn; // ��� � ����� �� ������� [���]
extern int Nb,Nx,    // ����� ����� � �������
              Pw;    // ������� ���������� ������������
extern fixed  Vv;    // ��������� ������������
                     //  X0 - �������������� �
                     //  0X - ������������ �����
                     //   8 - 1-������ �������
                     //   4 - 1-����� �� ������� ��������
                     //   2 - 0-����������, 1-������������� ��������
                     //   1 - 1-������ �������� ��������
                     // 100 - 1-������� �������� ������� �������
                     // 200 - 1-�������������� ������� ��������
                     //
extern class Series_Array
{ Mario **Array;                    //
public: int n,k;                    // ���������� � ����� �������
  Series_Array(){ Array=0; n=k=0; } //
  Mario& operator[]( int k ){ return Array[k][0]; };
  void allocate( unsigned );
} Ms;

#define Nm (Ms.n)
#define Km (Ms.k)
//
//      ���� ����������� �������������� �������
//       ��� ������ �������� ����������
//
void m_Tide();          // ���������� ���������� ����                /M_ImpEx
void m_Karta();         // ���������� ����� � �������� �� �����      /M_Karta
 int m_GetCom();        // ������������� ������ ����������
 int m_Segment();       // ������������ ����������� ����
void m_Move();          // ������� ����� �� ������� �������          /M_ImpEx
void m_Copy();          // ���������� ����� �����������              /M_ImpEx
void m_Mark( int=0 );   // ������� ��� ������� ������� �����         /M_View
void m_List();          // �������� ������ ����� � ������ � �� ����� /M_IO
void m_Show();          // �������� ���� � ��������������
void m_View();          // ����������� ��������� ������
void m_ReView( int=0 ); // ����������� ���� ������� ������           /M_ImpEx
void m_Edit();          // ��������� ���������� �������
void m_Read();          // ������ ����� ������������ ����� ������    /M_IO
void m_Write( int=0 );  // ����������� ������ ������                 /M_IO
void m_InPut();         // ������ ��������� �����������              /M_ImpEx
void m_OutPut();        // ������� ������ ����                       /M_ImpEx
void m_Delete();        // ��������                                  /M_ImpEx
                        //
void Function();        // ������� �������������� �����
void Filtration();      // ��������� ���������� � ����-�������
void Mario_main();      // ~~~~~~~~~~~~~~.
void MinMax( Real*,int,Real&,Real& );   // � Mario.cpp
                                        //
extern char Months[12][10],             //
            Weeks[7][12];               // Mario.cpp
extern Bfont Tw;                        //


