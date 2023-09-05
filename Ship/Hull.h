//
//  ����������� �������������� ������������� � ������� ������������
//              ������������ ���������������� ������������
//
#include "..\Graphics\Tv_Graph.h"
extern Real               //
     Xo,Xm,Length, Lwl,    // �����
           Breadth,Bwl,    // ������
     Do,   Draught,Height, // ������ � ������ �����
           Volume,         // �������������
           Surface;        // ��������� �����������
extern int Hull_Keys;      // ����� ������ - ����������
//              0x01 -  ������������� ������-������������
//
// ��� ��������� ��������� � ������ (������������) ��������
//
//   Set_Stem();  // ������������� ����������� �������
 int Set_Sinus(); // ������ �� ���������-��������������� �������
 int Set_Power(); // ������ �� ��������� (��������������) �������
 int Set_Parameters();
void Building();  // ����������� �������� ������� �� ����������� ������
                  //
void Draw_Hull( int Axis=0 ); // �������� ������ � �������� ������
//
//     +--------------------------------------------------------+
//     �                                                        �
//     �       ��������� ����������� ������ ���������           �
//     �                                                        �
//     �  X(0),Y(0) - ����� ����                                �
//     �  X(1),Y(1) - ����� �� ����� ����� � ������ (��������)  �
//     �  X(-1),Y...-//- �� �� - ������ �����                   �
//     �                                                        �
//     ~========================================================~
struct Frame         //
{ int  N;            // ���������� ������� ���������������� �����
  Real X,            // �������� �������������� ���������
      *z,*y,         // ��������� � �������� ����� ������� ����������
     *_z,*_y;        // ������ ����������� � ����� ������-�������
                     // ~~~~~~~~~~~~~~~~~~~~
  Frame( int=0 );  Frame& allocate( int );
 ~Frame();         Frame& free();       //
  Frame& SpLine();                      // ���������� ������-�������
  Real operator()( Real Z );            // �������� �������� ��������
  Real Z( Real );                       // ��������������� ������� ���������
  Real Y( Real );                       // � �������� ���������
  void YZ( Real A, Real &Y, Real &Z );  // �, - ���������������� �����
  int  iZ( Real &_y_ );                 // ������ ������������� �������
  void Double( int k );                 // ������������ ����� ����� �� ���������
};                                      //
class Curve                //
{     Point *_C;           // ����� �������
 Curve& free();            //
 int    find( Real );      // �������� ����� ������ �������
public: int N;             // ��������� ����� �����
  Curve( int S=0 ){ _C=0; N=0; if( S>0 )allocate(S); } // Constructor
 ~Curve(){ free(); }                                   // Destructor
 Curve& allocate( int=0 );                             // ������������� ������
        operator Point* () { return _C;    } // ������ ������ � �������
 Real  operator()( Real );      // �������� ��� ������ ������������
 void Extreme( Field &_Fm );     // ����� ����������� �� �������� ������
};                               //
//    +---------------------------------------------------------+
//    �                                                         �
//    �  ���������� ������� �������, ������������ �� ���������  �
//    �                                                         �
//    +=========================================================+
class Hull
{
public:                  //
 int  Ns,Ms;             // ���������� ���������� � ����� ������
 char Name[256];         // �������� ������
 Frame *F;               // ������ ������������� ����������
 Curve Stx,Sty,          // ����������
       Asx,Asy;          // ������������ (�����)
  void allocate( int N ) //
     { F=(Frame*)Allocate( (Ns=N)*sizeof(Frame),F ); Ms=-1; }
  void Init();          //
  void Simple_Hull( int Nx, int Nz );
  void Set_SpLine();    // ���������
  void Del_SpLine();    //  � ������ ���������� ��������
  Hull():Ns(0),F(0){ Name[0]=0; };
 ~Hull(){ allocate( 0 ); }
  Real operator()( Real x, Real z );
  int Read( const char* name="HULL.vsl" );
  int Write();
private:
  int Read_from_Polland();
  int Read_from_Frames();
};              //
extern Hull Kh; // ���������� ������, �������� ���������� �����������
// ~~~~~~~~~~~~ //
//      ���� ����������� ��� ������������ ����� �������
//
struct Window
{ viewporttype f; Field F; Window(); void Set( field a );
  bool Is( int x,int y ); // �������� ��������� �����
  void Focus();  // ������������� � ��������� ������������ ����
//void Check( int &x, int &y ); bool Is();
};
struct DrawPlane: public Window
{ const char *iD,*aX,*aY;
  DrawPlane( const char i[], const char x[], const char y[] ):
             Window(),iD(i),aX(x),aY(y){}
  bool Info( int x, int y );
};
//
// ������� �������� ������� � ���� �������� ������� �������� ���������
//
struct Plaze          //
{ Real **Y,           // ������� ������� ������������� ����������
      dX,dZ,  Depth,  // ��� ����� �� ������� �������� ������� � ������
     *Xa,*Xs,*Ya,*Ys, // ������� ����������, �������� � �������� �������
      V,S,            // ������������� � ������� ��������� �����������
     **QV,            // ������ "����������", ���������� ������ � ��������
     *Wx;             // ������� ������ ��� ������������ ����������������
  int Nx,Nz;          // ����������� �������� �������
  Plaze( int z, int x )
  { Depth=V=S=0; dX=dZ=1; QV=Y=NULL; Wx=Xa=Xs=Ya=Ys=NULL; allocate( z,x ); }
 ~Plaze(){ allocate( 0,0 ); }
  void allocate( int z, int x );      // ������������� �������� � �������
  void build( Real Z=Draught );       // ���������� ������� �������
  void drawlines();                   // ���������� �������� �������
  Real  Michell( Real Fn );           // ������������� �� �������
  double Amplint( const double &La ); //
  double QG4( const double &Xl, const double &Xu );
  void DrawWaves( Real Fn=0.0 );
};
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                      //
extern DrawPlane wH,    // ���� �������� ������,
                 wM,    //  ���
                 wW;    //   � ����������
                        //
extern const char *Months[],*WeekDay[];
