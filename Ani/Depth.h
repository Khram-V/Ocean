//
//      Depth                                                      /90.10.22/
//      �������� �������� ���������, ������������� � ������������� ����������
//
#ifndef _Depth_Hxx
#define _Depth_Hxx
#include <StdIO.h>
#include "..\Matrix\Basis.h"
#include "..\Graphics\Window.h"
#define _English false
//
//    ��������� ����������� ��� ������� ���� ������
//      a = 6378245 m           ������� �����������
//      b = 6356863.0188 sn\ .
//      c = 6399698.9018 ew/    (b+c)/2 = 6367554.0094
//      e = 298.3
//      g = 9.8106
//           1855.35626248109543 ������������� ����
#define Mile 1852.24637937659918                     // Sea Mile �������������
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
                // 0x0001 ���������� ����������
                // 0x0006 Empty, Letters, Numbers, Identify (4) ��� Mario
                // 0x0020 ���������� ����� ����� � F4 (����� ��� ����������)
                // 0x0018 ��� ������ ����� ���� � F4  (����� � ������������)
                //    - 8 = 0 - ����������� ����������� ������ ��������� F4
#define as_Globus  0x0100  // ������� ��������� ����������
#define as_Simula  0x1000  // ������� ����������� ���������� �������������
                // 0x8000 ������� �������������� ������������ �������
                         //
struct MARIO{ Point G;   // �������������� ���������� �����
             char *cp;   // ��������� �������� ������
             Real D,A;   // ��������� � ������ �� ��������� �����
            };           //
class Depth              // ������� ��������� �������� ����� ����������
{                        // ==============================================
 float **__Depth;        // ������������/�������������� ������� ����������
 Basis  Geo;             // ����� �������� � ������������
 Point  Base;            // Latitude & Longitude                10
//protected:
public: operator float**(){ return __Depth; }
 Real   operator()( _Point R ){ return Value( __Depth,R.x,R.y,Nx,Ny ); }
 unsigned Type;          // 0:Text; 1:Byte; 2:Integer; 4:Float   4
//      Dat;             -- 0:Var; 1:Line; 2:Field; 3:Space      5
 char   Ms;              // '-minute d-degree k-kilometre m-metre
 bool isMercator;        // '=>G - GeoGraphy else M- Mercatory  -/-
 int    Ny,Nx;           // ����������� ����� ������� ���������� � ������
 Real   Sy,Sx,           // Grid's Steps [rad]                   9
        Fe,              // Average (middle) Latitude of chart  -/-
        Cs,              // Meridian's Course [ �-degree ]      -/-
        Max,Min;         // Bathimetry Extrema
 static char    InfName[MAX_PATH],   // Information File Name
  Msg[2][MAX_PATH],Name[MAX_PATH];   // Comment & Bathimetry File Name 1-3
 static int Nmp;                     // Number of Marigrams            11
 static MARIO *M;                    // List of Marigrams              12..
 static FILE *Fs;                    // ��������� ����
 Depth(): __Depth( NULL )            // Constructor cFe=1.0
                 { Nx=Ny=0; Sy=Sx=1.0; Fe=Cs=Base.y=Base.x=0.0; }
  void allocate( int sY=0,int sX=0 );
 double scale( );                    // ������� �� ������� Ms->Map(radian)
 Depth& basis( bool=false );         // ����������� ������
 Depth& read ( char* );              // ������ ��������������� �����
  void  write( );                    // ������ ������
  void  write( const char* );        // ������ ��������������� �����
  bool  edits( bool=false );         // ��������� ��������������
  void ItD( Point& W );              // �������� �������� � ���.����������
  void D2I( Point& W );              // -- ��� ��������� � �������
  void DtI( Point& );                // ���������� � ������� �� ��������� �����
  void D2I( Point Q,int &y,int &x ); // ���������� -> �������
  void DtI(_Point P,int &y,int &x ); // - � ��������� � ��� ����� �� �������
  void sphere( Point,Point,bool=false ); // ���������� ������ �� P � Q
  bool is_Globus();
  void mario_control();                  // ��������� ����������
  int  mario_utility( int&,int&,int=0 ); // 0 ���� ������, ����� mode:
                                         // +- ���������, <Enter>���������
                                         // ins - ����� �����, del - ��������
  void find_coast( MARIO& );             // ��������� � ���������� �� ������
  void move_to_coast( MARIO&, Real D, int knots ); // ������� ����� �� Depth
                                                   //   �� �������� �� knots
  int Seismic_List();               // ���������� ������ ������ ������ �� �����
  int Seismic_List( Point& Geo );   //     � ������� ��������� ���������� �����
  int Seismic_List( Point& Geo,     // �������������� ����������
                    Event& Jul,     // ��������� ���� � ��� �� ������ �����
                    Real& Mag,      // ���������
                    Real& Hei );    // ������� ����� ������������� � ��
private:                            //
 void  projection( byte Sym );      // ��������� ���������������� ��������
 void  medit();                     // ������ �������������� �����
 char* open( char*,const char*,const char* ); // ��� ����� Fs( name,ext,mode )
 void  open( char regime='r' );     // ��� ����� Fs ������
 void  infout( const char* );       // ����� ������ ����� .inf
 void  remove();                    // �������� ����� Fs � ���������
 void  close ();                    // ������ �������� ����� Fs
//FILE* save( const char* =0 );     // ���������� �
//int   restore( char* );           //  �������������� ����������
};                      //
extern Depth Chart;     //! ������ �� �������/�������� ����������� ����������
                        //
struct Screen:          // ������� � �������� �������������� ���������� ������
  Window,               // ���� ����������� � �������������� ���������� �����
  field                 // - ������� ������� �������������� ����� �� ������
{ int Mx,My;            // - ������������ ����������� ������������ ����������
  Screen(): Window(),field( (field){ 0,0,0,0,0 } ),Mx( 0 ),My( 0 ){}
  Field Focus(){ F=(Field){ Jx,Jy,Lx,Ly }; return Window::Focus(); }
  int Console( bool );  // ������� �� ���������� � ���������� ������ - 1 hold
  void window();        // ��������� ����������������� �����������
  void axis( Depth &Dp=Chart, int=CYAN ); // �������� ���� CYAN=3
  void view( float** ); // �������� ����� ����������
  void show( float** ); // ��������� ����� ������
};                      //
extern Screen Map;      //! ������ ����� � �������������� �� ����������� ������
extern Button All;      //! �������� ��� �������� � �������/��������� ��������
extern char Str[MAX_PATH*2]; // ������ ��� ��������� ���������
extern const char *_Month[], // ������ ��������� ���� �������
                  *_Week[];  //      � ����������� ���� ������
//!
//!     ����� ��������������� ���������� �������,
//!      ��������� ��� �������������� ��������������� �������� �����
//!
//
//      ���������� � ����������� ����� ����� �������
//              Cs[�]  ����������� �� ����� Op �� Np
//              Ds[km] ���������� ����� �������
//
void     Sphere( Point Op, Point Np, Real& Cs, Real& Ds );
Point    Sphere( Point Op, Real Cs, Real Ds );
void Geo_Circle(_Point Op,_Real Ds );
//
//   ������ ������������ �� ����������� ��������� ���������
//
Real ReLine( Real *Y, int N, Real Ar, int Offs=sizeof( Real ) );
                                      //
void ViewPointMario( int=LIGHTBLUE ); // ������� ������� Mario �����
void Thelp( const char*,... );        // ����������� ���������
                                      //
inline void Mario_view( int& y,int& x ){ Chart.mario_utility( y,x,0 ); }
//
//   ��������� ���������� ������� ��������
//
void Dp_Show( float **_D, int Jy,int Jx, int Ly,int Lx, int=BLUE );
//
#endif
