///                                   +Var ++2.1.12  /90.12.18-11.05.15/
///    Tv_GRAPHics.cpp
///                                   -bgi = 0 -  ( Mode )   - VGAMAX
#ifndef __Tv_Graph__                    //   1 -  320 x 240  - CGA
#define __Tv_Graph__                    //   2 -  640 x 350  - EGA
#include <StdIO.h>                      //   3 -  640 x 480  - VGA
#include "Graphics.h"                   //   4 -  800 x 600  - SVGA
#include "..\Matrix\Basis.h"            //   5 - 1024 x 768  - VESA
                                        //   6 - 1280 x 1024
typedef void *HIMG;                     //   7 - 1600 x 1200
                                        //   8 - 1920 x 1440
int Tv_Graphics_Start( int Mode=-1,int Width=0,int Height=0 ); // �������������

class Bfont:Basis_Area  //
{ char *CHR;            // ����� ��������� ������� ��� ���������� ���� � ������
  short H,b;            // �������� � ��������� ������ � ��������� ����
  int __OutText(char*); // ���������� ����� ������ � ���������� ����� Base_Area
public:                                // + �������������� ��������� �����:
  short Tw,Th;                         // ������ � ������ ����� ��������� �����
  Bfont( void* _f=0,const int _H=-1 ); // ����� � ��� ������, _f=0: �����
void Font( void* _f,const int _H=-1 ); // -1 - �� ��������
int Height( const int _Height );       //  0 - ����� ������ Tv.Th
                                       // >1 - ����� ������
                                       // ��� ��������� ���������� ������
                                       // 0: 10x8 <= 640x480; 18x9 >= 800x600
                                      // >0: ���������� � ���������� �� �������
  int StrLen( const char* );          // ����� ������ � ������ ������
  int Text(        Real,Real, const char* );
  int Text( Course,Real,Real, const char* );
  int Text(        int x,int y,const char* );  // ������ ��������������� ������
  int Text( Course,int x,int y,const char* );  // � ������ ��������
  int Text(        point p,    const char* );  // ������������ ������ �����
  int Text( Course,point p,    const char* );  //
//int OutText( Point,                  char* ); // ?�� ��������� ������� ������
  int OutText( Point,Point,            char* ); // ������ ����� ����� �������
  int OutText( Point,Point,Point,      char* ); // --//-- � ��������
  int OutText( Point,Point,Point,Point,char* ); // -- �� ���� ����� � ���������
                                                // � �������� ����������������
};                      //
struct  TvMode: Bfont   // ��������� �������� ����������
{ short Color,          // ������� ���� ��� ������� � ��������
        BkColor,FColor, // ����� ���� pattern � ��� ���������
        Fill,           // ����� �������� ����
        mX,mY;          // ���������� �������� X-width � Y-height ��� ���������
  TvMode();             // Constructor
};                      // ~~~~~~~~//
extern TvMode Tv;                  //
extern point Tpoint;               // ������� ����������� �����
extern viewporttype Tv_port,Tport; // ����������� � ��������� ����
extern unsigned char _Small_font[];
extern unsigned char _Simplex_font[];
//
//      Tv_place ��������� ������������� ������������ ����
//            AT - {}>0  ����� ������ ���� � ������ ���� � ���������
//                 {}<=0 ������� �� ������(Jy) - �������(Ly),
//                                  ����� (Jx) � ������ (Lx)
//                                  ������ �������� ���� � ��������
//            AF - ���������� ������� ������������� �������
//
   void Tv_place ( field *AT, Field *AF=0 ); // �������� �������
   void Tv_window( field *AT, Field *AF=0 ); // ��������� �
// void Tv_back();                          //  ����� ����
   void TvClip();                          // ��������� ������� �������
   void TvUnClip();                       //  � �� �������������
   void Tv_resize( int w, int h );       //  ��������� �������� ����
   void Tv_revert(); // bool=false );   // ��������� �������� �������
   void Tv_rect();                     // ����� �
   void Tv_bar();                     //  �������� �������
   void Tv_bar( int l, int t, int r, int b );
   void Tv_rect_x( int l, int t, int r, int b );
   void Tv_rect_up( int l, int t, int r, int b, int w );
   void Tv_rect_UpXOR( int l, int t, int r, int b );
   HIMG Tv_store( int,int,int,int, const field *A=0 );
  field Tv_restore();      // ���������� ��������� � �������� ����
                           // � ������������ ������ ���������� ������������:(A)
//
//     Tv_fl: �������� ������������ ������ � ���������� �����������
//
   int Tv_x ( Real X ); Real Tv_X( int x );
   int Tv_y ( Real Y ); Real Tv_Y( int y );
  bool Tv_fc( Point *P );                     // �������� ��������� �����������
//
//      Tv_cross: ������� ����� ���������
//      Tv_Pause: ����������� ��������� ��� ������������ �������
//         Size < 0 ������� ������� ��� ����������
//              = 0 ������� ����������� �������
//              > 0 ��������� ������� � ����� �����
//         Step - ��� �������, ����������� � ����������
//
void    Tv_cross( Point P, int Size=16 );
Course  Tv_pause( Point *P, int Size=16, Real Step=0.0 );
//
//  �==========================================================�
//  �  ���� �������� �������� (�������������) ������� � �����  �
//  L-----------------------------------------------------------
//
enum{ NoCheck=-20, NoMouse=-10, MouseInstalled=1 };
enum{ _MouseLeftPress   = 2, _MouseRightPress   = 4,
      _MouseLeftDblClick= 8, _MouseRightDblClick=16,
      _MouseLeftRelease =32, _MouseRightRelease =64 };
extern int Tv_MouseState;       //
 int Tv_Start_Mouse();          // ����������� ������������ �����
 int Tv_Stop_Mouse();           //
 int Tv_MouseStatus();          //
 int Tv_MouseX();               // ������� ����������
 int Tv_MouseY();               //  �������� �������
 int Tv_GetCursor( int&,int& ); // ������ ����������
void Tv_SetCursor( int, int  ); //  � ��������� ��������� �������
void Tv_CursorEnable();         // ������������
void Tv_CursorDisable();        //  � ������� �������� �������
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//      Tv_getc : ������ ������� �� ���������
//      Tv_getw : -//- ��� ��������� ���������
//      Tv_getk : ������ �������������� ������ � ���������� �������
//      Tv_draw : ����������� ��������� ��� ������������� ��������
//        key:  @P - ������ ����������� ����� ��������� �� ��������
//              @S - ����� � DOS ����� SYSTEM (command.com)
//              @D - ����������� ��������� ������������ �������
//              ^C - exit
//      return(Cr) - <Enter>  - ������� ����� �� ���������� ������
//            (14) - <Insert> - ����� �� ������� ��������� �����
//            (15) - <Delete> - ����� �� ������� ���������� �����
//            (16) - <BackSpace>
//            (17) - <Tab>
//            (27) - <Escape>  __________
//       (0xF1..a) - <Function>         //
   void Tv_draw ();                     // Key = <alt>D
   void Tv_close();                     // Key = <ctrl>C
   void Tv_bell ();                     //
   bool Tv_isKey();                     // ����� Win32 - ��� �����������
 Course Tv_getc( const int _mSec=0 );   //     BLACK ... DARKGRAY
 Course Tv_getw();                      //      BLUE ..x LIGHTBLUE
 Course Tv_getw( Course Simb );         //     GREEN .x. LIGHTGREEN
 Course Tv_getk();                      //      CYAN .xx LIGHTCYAN
//                                      ~~       RED x.. LIGHTRED
//      ��� ��������� ��� �������� ���� ~~   MAGENTA x.x LIGHTMAGENTA
   void Tv_axis( bool Lab=false );       //     BROWN xx. YELLOW
   Real Tv_step( Real );                 // LIGHTGRAY xxx WHITE
//
//      Twindow: ��������� ���������� ���� �� ����������� ����
//         Jx,Jy > 0 - ���������������� �� ������-�������� ����
//              <= 0 - ������������ ������� ������� ���� �����������
//         Lx,Ly     - ������ ���������� ���� � ��������
//         wb        - ������� ����� (0 - ����� ������ Color)
                                           //
   void Twindow( int,int,int,int, int=0 ); // ��������� ���������� ����
   void Tback();                           //   �������������� ��������
//
//      ��������� ������������ ������ ������ � ������ �� ����������� ��������
//        ������������ ������� ������ �� ������, ���:
//        Ki - ����� �������� � ������ � �� ������ �� ���������;
//      <ret> ������� � ���������� ��������� � ������ �� ���������:
//           ���� � ���������� ���������� ������� ������, �� ���������� �����
//           �� ����� ������, ���� ���������� ���� � ����������, �� ������
//           ��������������� �� ������ ������� ���������� �� ��������� ����.
//
Course Tcase            //
( field& M,             // ���������� ������� �� ������ ��������
    int  ls,            // ������������ ����� �������������� ������
  char*  Fl(int),       // ���������� ������� �� ������ � ������
  Course Hl(int),       // ���������� ��� ������� � ������������
    int  N,             // ������ ����� ������
    int& Ki,            // ����� �������� �������� (-1 �� ��������)
    int  ret=0          // 1 ������; 0 �������; -1 ��������� ����
);                      //
Course Tcase( field&,int,char* Fl(int),int,int&,int=0 );
//
//  Mlist - ������ ���������� ������ ��� ������� �� ��������
//      INPUT                           ACTIVE
//  skip:skip string                    :����� �����
//  lf  : 0 - ������ �� ������������    :����� �������� ���������
//  Msg :NULL - ������ ������� ����     :�������� ������
//  dat :NULL & lf<>0 - ����-������     :����� ����������� �������
//
#define Mlist( L ) L,(sizeof( L )/sizeof( Mlist ))
 struct Mlist{ short skip,lf; const char *Msg; void *dat; };
 struct mlist{ byte Y,X,lf,S; const char *Msg; void *dat; };
//
//      Tmenu   - ���������� ��������� ������� � ����������  /90.10.03/
//        Y,X   - ���������� ������ �������� ���� ��� ���� ��������
//        Mlist - ������ ���������� ��� �������� �� ��������
//        Nm    - ���������� �������� � ������ Mlist
//       <Home> - ����� ��� ����������
//       <End > - ����� �� ����������� ���������
//       <PgUp> - ������� � ������ ����
//       <PgDn> - ������� � ����� ����
//       return - ����� ��������� ������� ��� -1 ��� ������
//
   int Tmenu(             Mlist*, int,int x=1,int y=0,int ans=-1 );
   int Tmenu( void H(int),Mlist*, int,int x=1,int y=0,int ans=-1 );
struct Display
{
 int Active,Nm,Y,X,K; void( *Hl )(int); mlist *M;
  Display(              Mlist*,int, int=1,int=0 );
  Display( void H(int), Mlist*,int, int=1,int=0 );
 ~Display();
  int Answer( int=-1 ); void Back();
};
//
//   ��������� ��������� ������ � ����� � ���
//
extern char* Tfile_Filter;          // ��������������� ������ ���������� ������
FILE *Topen( char* Path=(char*)"*.*", // ����������� ��� ��� ����� ������ �����
       const char* Mode="r",          // ����� ������������� ��� ������/������
       const char* Extension=0,       // ���������� ��������� ������� �����
       const char* Commentary=0 );    // ����������� � ��������� ���� ������
long long Tfile( FILE* File );        // ���������� ����� ��������� �����
long long Tfile( char* PathName );
long long Tfile( char* PathName, const char* Com );
//
//      Tgo : ���������������� �������
//            ������������ �������� ������ ���� ���������
//       x,y>0 - ������� ���������������� ������ ���������� ����
//       x <=0 - ������ ���������� � ����������� ������ ���� ���������
//       y <=0 - ���� - ���������� ������ ������, ����� ������ �����-�����
//
   void Tgo( int x, int y );            // ��������� ������� ��� �����
   void Tis( int &x,int &y );           // ���������� ������� �������
   void Tln();                          // ������� �� ����� ������
   void Tputc( char ch );               // ���� ������ � ������� �������
   int  Tprintf( const char*, ... );    // ����� � ����������� ������
//
//      Tgetc,s : ������ ������� �� ���������
//        S     - ���������� ������
//        len   - ����� ���� �������        (len<0 - ���������� ����)
//        pos   - ��������� ������� ������� (pos<0 - ������ �������)
//      return  (cr) - <return> - ������� ����� �� ���������� ������
//              (14) - <insert>
//              (15) - <delete>
//              (27) - <escape> - ����� � ������� �������
//         (0xf1..a) - <function>
//
Course Tgetc( void );
Course Tgets( char* S,int len,int pos=0 );
Course Wgets( char* S,int len,int pos=0 ); // c ��������������
//
//       line : ���������� �����
//     moveto : ����� ������� ��������� �����
//     lineto : ���������� � ������� ��������� ������
//     needle : ����������� ���������
//
   void needle( int,int,int,int );
   void needle( Real,Real,Real,Real );
   void line  ( Real,Real,Real,Real );
   void moveto( Real,Real );
   void lineto( Real,Real );
                            //
   int  color  ( int );     // ��������� ������ �����
   int  bkcolor( int );     // ��������� ����� ����
   void pattern( int,int ); // ��������� ���� � ����� ���������
   void pattern( int );     // ��������� ������ ����� ����
                            //
inline void moveto( _point p ){ moveto( p.x,p.y ); }
inline void lineto( _point q ){ lineto( q.x,q.y ); }
inline void line  ( _point p,_point q ){ line  ( p.x,p.y,q.x,q.y ); }
inline void needle( _point p,_point q ){ needle( p.x,p.y,q.x,q.y ); }
//
//     ������� � ������� ���������
//
  void Ghelp(); inline void Thelp(){ Ghelp(); }
  void Ghelp( const char*,... );  // ��������� ���������
  void Thelp( const char*,... );  // -- � �������
Course Help( const char *Name[], const char** Text, const char** Plus=NULL );
              //
void clear(); // { clearviewport(); }
//
//  256-colors Palette
//
void Get_Palette( int Start, int Count, RGB_palette );
void Set_Palette( int Start, int Count, const RGB_palette );
//
//  ��������� ���������� ������� ������� � �������� �� <Graphics.h>
//
HIMG Tv_GetImage( int l,int t,int r,int b );
void Tv_PutImage( int l,int t,HIMG p,int OP=COPY_PUT );
void Tv_FreeImage( HIMG p );
//
//  ������� ���������� ��������� � ����������� ���������
//
void Map_View( Field wF ); // ������� ����� � �������� (..\Chart\WorldMap.cpp)
                  //
struct IsoLine    // ������ �������� �������� ��� ������ ��������� ������ Z
{                 //
 int N;           // ���������� ��������
 int *L;          // ����� ��������
 byte *T;         // ���� ��������: 0-��������� ������; 2-��������� �����
 float Z,**Y,**X; // ������� � ���������� ������� �����
};                //
IsoLine *Tf_curve // ��������� �������������� ������ ������ �������� �
( Real _Lvl,      // ������� _Lvl, �� ���� ������ � �������� ����� _Fs
  void *_Fs,      // -- ������������� ������ �������� �������� ���������� ����
   int _T,        // ��� ������
   int _Nx,       // ����� �������� ������ � ����� ��� � ������
   int _Jy,int _Jx, int _Ly,int _Lx // ������ ��� �������
);
inline short get2( FILE *F ){ short d=0; fread( &d,2,1,F ); return d; }
inline float getf( FILE *F ){ float d=0; fread( &d,4,1,F ); return d; }
inline void  put2( short d, FILE *F ){ fwrite( &d,2,1,F ); }
inline void  putf( float d, FILE *F ){ fwrite( &d,4,1,F ); }
#endif
