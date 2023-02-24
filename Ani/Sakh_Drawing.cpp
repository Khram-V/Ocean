///
///  <<  Процедуры масштабирования текущего изображения  >>
///       ++ основные графические прорисовки
///
#include "Sakhalin.h"

static double Zoom=1.0;        // !! это используется в последующем контексте !!

int PcX( int px ){ return ((Tv.mX+1)*px)/100; } // из процентов ширины
int PcY( int py ){ return ((Tv.mY+1)*py)/100; } //  и высоты экрана

void Map_Tv_place( Field &Ft, const viewporttype &pl, bool MapType )
{                                                                // к прямому
 field f; f.Jx=-pl.left; f.Lx=pl.right -Tv.mX-1; f.wb=0;         // заданию
          f.Ly=-pl.top;  f.Jy=pl.bottom-Tv.mY-1; Tv_place( &f ); // координат
  if( MapType )                       // Для Н/Р это > 1.0
  { float As=Tv.mX*float(0.64)/Tv.mY; // под приведенную широту
    double W=(pl.bottom-pl.top)/Ft.Ly; // arcos( 0.64 )= ровно 50° - чуть больше
        Zoom=(pl.right-pl.left)/Ft.Lx /As;
    if( Zoom>W )
      { Zoom=W; Ft.Jx-=( ( pl.right-pl.left )/Zoom/As-Ft.Lx )/3.0; }
           else Ft.Jy-=( ( pl.bottom-pl.top )/Zoom-Ft.Ly )/3.0;
                Ft.Lx =  ( pl.right-pl.left )/Zoom/As;
                Ft.Ly =  ( pl.bottom-pl.top )/Zoom;
  }
  { Field F=Ft; Tv_place( 0,&F ); }
}
HIMG Save( viewporttype& s, HIMG _Sci )
{ if( _Sci )Tv_FreeImage( _Sci );
  return Tv_GetImage( s.left,s.top,s.right,s.bottom );
}
void Restore( HIMG _Sci, int x, int y )
{ if( _Sci ){ Tv_PutImage( x,y,_Sci ); Tv_FreeImage( _Sci ); }
}                             //
int Key_Answer()              // Требование безусловного
{ Tv_bell();                  // подтверждающего ответа клавиатуры
  for(;;)switch( (char)Tv_getc() )  //
  { case 'y': case 'Y': case _Enter: return 1;
    case 'n': case 'N': case _Esc:   return 0; default:;
} }
void Cross( Point& P, float size )
{ int CrossSize = size==0.0? 10:int( Zoom*size );
   moveto ( Tv_x(P.x)-CrossSize,Tv_y(P.y) ); linerel( CrossSize*2,0 );
   moverel( -CrossSize,-CrossSize );         linerel( 0,CrossSize*2 );
}
void LineWide( int c, float width )
{ color( c );
  setlinestyle( SOLID_LINE,0,
   ( int( Zoom*width ) )>2 ? THICK_WIDTH:NORM_WIDTH );
}
void FillPoly( int N, Point *P, int /*Pattern*/, int Color )
{ point *p=new point[N];
  if( p )
  { for( int i=0; i<N; i++ )p[i]=P[i];
    pattern( /*Pattern,*/ Color ); fillpoly( N,(int*)p ); delete p;
} }
void Circle( Point P, float R )
{    circle( Tv_x( P.x ),Tv_y( P.y ),int( Zoom*R ) );
}
void Circle( Point P, Point Q )
{ int dx= Tv_x( P.x )-Tv_x( Q.x ),
      dy= Tv_y( P.y )-Tv_y( Q.y );
  circle( Tv_x( P.x ),Tv_y( P.y ),int( hypot( dx,dy )+0.5 ) );
}
void Circle( Point P, Point Q, Point q )
{
 static point p[3]; p[0].x=Tv_x( P.x );
                    p[0].y=Tv_y( P.y );
                    p[1].x=Tv_x( Q.x )-p[0].x;
                    p[1].y=Tv_y( Q.y )-p[0].y;
                    p[2].x=Tv_x( q.x )-p[0].x;
                    p[2].y=Tv_y( q.y )-p[0].y;
    if( ( p[1].x || p[1].y )
     && ( p[2].x || p[2].y ) )
    arc ( p[0].x,p[0].y,int( atan2( p[1].x,p[1].y )*180.0/M_PI-89.5 ),
                        int( atan2( p[2].x,p[2].y )*180.0/M_PI-89.5 ),
                        int( hypot( p[1].x,p[1].y )+0.5 ) );
}    //
     // Согласно флажку (3-ий параметр):
     //   1  - проценты
     //   0  - x1,y1 - проценты,  x2,y2 - ширина и высота в символах
     //   2  - физич. экранные координаты (необходимого пространства)
     //
PanelWin::PanelWin( viewporttype &sc,
          fixed id,
          fixed tm, int flag, int pf, int ld, int bw, int ic )
        : WinObject( sc,id,tm ),
          PanelFlags ( pf ),
          Ledge      ( ld ),
          Border     ( bw ),
          InsideColor( ic )
{ switch( flag )
  { case 0: scr.left  = PcX( scr.left );
            scr.right = scr.left+scr.right*Tv.Tw;
            scr.top   = PcY( scr.top );
            scr.bottom= scr.top + scr.bottom*Tv.Th; break;
    case 1: scr.left=PcX( scr.left ); scr.right =PcX( scr.right );
            scr.top =PcY( scr.top  ); scr.bottom=PcY( scr.bottom );
    case 2: scr=sc;  break;
    case 3: scr.right = scr.left+PcX( scr.right  );
            scr.bottom= scr.top +PcY( scr.bottom ); break;
  }
  if( scr.left<0      ){ scr.right-=scr.left;       scr.left=0;       }
  if( scr.right>Tv.mX ){ scr.left-=scr.right-Tv.mX; scr.right=Tv.mX;  }
  if( scr.top<0       ){ scr.bottom-=scr.top;       scr.top=0;        }
  if( scr.bottom>Tv.mY){ scr.top-=scr.bottom-Tv.mY; scr.bottom=Tv.mY; }
  Win=scr;
  Win.left   += Ledge*2 + Border + 2;
  Win.right  -= Ledge + Border; // + 2;
  Win.top    += Ledge*2 + Border + 2;
  Win.bottom -= Ledge + Border; // + 2;
  Draw(); // Show();
}
//   fl=1 - Raizing,
//   fl=2 - Lowering
//
static void Raizing( int x1, int y1, int x2, int y2, int fl, int w=1 )
{ int i;
  if( !fl )return;      color(  fl==1?WHITE:BLACK  );
  for( i=0; i<w; i++ ){  line( x1+i,y1+i,x2-i,y1+i );
                         line( x1+i,y1+i,x1+i,y2-i );
                      } color(  fl!=1?WHITE:BLACK  );
  for( i=0; i<w; i++ ){  line( x1+i,y2-i,x2-i,y2-i );
                         line( x2-i,y1+i,x2-i,y2-i );
                      }
}
void Panel( int x1, int y1, int x2, int y2,     // fl=1 - Raizing,
            int fl, int ld, int w,  int ic )    // fl=2 - Lowering
{  pattern( SOLID_FILL,LIGHTGRAY );
       bar( x1, y1, x2, y1+2*ld+w );
       bar( x1, y1, x1+2*ld+w, y2 );
       bar( x2-2*ld-w, y1, x2, y2 );
       bar( x1, y2-2*ld-w, x2, y2 );
   Raizing( x1,     y1,     x2,     y2,    fl>>2,ld );
   Raizing( x1+ld+w,y1+ld+w,x2-ld-w,y2-ld-w,fl&2,ld );
   if( ic>=0 )
   { bkcolor( ic );
     pattern( SOLID_FILL,ic );
     bar( x1+2*ld+w,y1+2*ld+w,x2-2*ld-w,y2-2*ld-w );
}  }
void PanelWin::Draw()
{ Panel( scr.left,scr.top,scr.right,scr.bottom,PanelFlags,Ledge,Border,InsideColor );
}
static void __Marker_Size( viewporttype &scr, char *Image ); // это где-то
static void __Marker_Draw( int x, int y, char *Image );      // в конце файла

BitButton::BitButton( viewporttype &f, const char *_mark, int _id, bool _st )
         : WinObject( f,_id,1 ),   // Отметка кнопки может быть текстовой
           Image( (char*)_mark ),  //  или в виде векторной картинки
           State( _st )            //
       { __Marker_Size( scr,(char*)_mark ); Draw();
       }
void BitButton::Draw()
{ Panel( scr.left,scr.top,scr.right,scr.bottom,ptOuterRaise,1,0,LIGHTGRAY );
  __Marker_Draw( scr.left,scr.top,Image );
  Raizing( scr.left,scr.top,scr.right,scr.bottom,State?ptInnerLower:ptInnerRaise );
}
CheckButton::CheckButton
( viewporttype &f, char *im1,char *im2, char *msg, int _id, bool state )
  : WinObject( f,_id,1 ),Stat( state ),
    Im1( im1 ),Im2( im2 ),_Msg( NULL )
{   __Marker_Size( scr,Im1 );
    if( msg )scr.right+=Tv.StrLen( _Msg=strdup( msg ) ); Draw();
}
CheckButton::~CheckButton(){ if( _Msg )free( _Msg ); }

void CheckButton::Draw()
{ Panel( scr.left,scr.top,scr.left+19,scr.bottom,ptOuterRaise,1,0,LIGHTGRAY );
     __Marker_Draw( scr.left,scr.top,Stat?Im1:Im2 );
  if( _Msg )
  if( *_Msg ){ color( BLUE ); Tv.Text( scr.left+25,scr.top+4,_Msg ); }
}
void CheckButton::Press(){ Stat=!Stat; Draw(); }
 //
///   << Набор картинок для элементов управления (кнопочек) >>
 //
void Geo_Star( int x, int y, int radius, float angle )
{ float A,R;
  int i,_x,_y;
  for( i=0; i<=16; i++ )
  { A = angle+float( i )*( M_PI_4*0.5 );
    R = radius*((i&1)?0.18:(i%4?0.5:0.8)); _x=int( 0.5+x+R*cos( A ) );
                                           _y=int( 0.5+y+R*sin( A ) );
    setlinestyle( SOLID_LINE,0,(i-1)%4?NORM_WIDTH:THICK_WIDTH );
    if( !i )moveto( _x,_y );
      else  lineto( _x,_y );
} }
void Geo_Page( int x, int y, int size )
{
 const int _x[]={ -40,40, 40,-40,-40, 2,-6,-40, 2 },
           _y[]={  50,50,-50,-50, 48,50, 8, -5,50 };
  for( int i=0; i<9; i++ )
  { setlinestyle( SOLID_LINE,0,(i<=7?THICK_WIDTH:NORM_WIDTH) );
    if( !i )moveto( x+(_x[i]*size)/100, y-(_y[i]*size)/100 );
      else  lineto( x+(_x[i]*size)/100, y-(_y[i]*size)/100 );
} }
char __ExitBtn[] = {  0, 16,16,
    4,BLACK,YELLOW, 11,1,  2,1,  2,13, 13,13,
    5,BLUE, GREEN,  13,1, 13,13, 6,16,  6,4,  13,1,
    2,BLACK,WHITE,  8,9,   8,11,
      0xFF, BLUE, 'В','ы','х','о','д',
    0 };
char __CheckBtn[] = { 0, 16,16,
    2,     BLACK,BLACK,     4,9, 5,10,
    3,  DARKGRAY,DARKGRAY,  6,14,8,14,12,4,
   10,LIGHTGREEN,LIGHTGREEN,1,8,2,8,6,12,7,11,10,6,12,4,10,6,7,13,5,13,2,8,
    0 };
char __UncheckBtn[] = { 0, 16,16,
   17,YELLOW,LIGHTRED, 4,2, 7,5, 10,2, 12,2, 12,4, 9,7, 12,10, 12,12,
                      10,12,7,9, 4,12, 2,12, 2,10, 5,7, 2,4, 2,2, 4,2,
    0 };
char __MapLayerBtn[] = { 0,18,16,
   17,BLUE,BLUE,   1,7,  2,4, 4,2, 7,1, 8,1, 12,2,14,4,15,7, 15,9,
                  14,12,12,14,9,15,7,15,4,14,2,12, 1,9, 1,7,
    6,GREEN,GREEN, 6,9,  9,11, 8,13, 5,13, 4,11, 6,9,
    2,GREEN,GREEN,10,13,12,12,
   11,GREEN,GREEN, 1,7, 2,4, 4,2, 7,1, 9,3, 5,5, 6,7, 8,9, 4,6, 2,8, 1,7,
   0xFF,    BLACK, 'Г','р','а','ф','и','к','а', 0 };
char __TxtLayerBtn[] = { 0,20,16,
    5,LIGHTCYAN,   LIGHTCYAN,    1,0,  18,0,  18,5,  1,5,  1,0,
    5,LIGHTBLUE,   LIGHTBLUE,    1,6,  18,6,  18,11, 1,11, 1,6,
    5,LIGHTMAGENTA,LIGHTMAGENTA, 1,12, 18,12, 18,15, 1,15, 1,12,
    5,LIGHTGRAY,   WHITE,        2,15,  3,6,   5,6,  6,15, 2,15,
    2,BLUE,        BLUE,         4,15,  4,12,
   10,WHITE,       YELLOW,       3,6,2,5,2,4,3,3,4,1,5,3,6,4,6,5,5,6,3,6,
    2,CYAN,        CYAN,         4,0, 4,3,
    2,CYAN,        CYAN,         3,1, 5,1,
    5,LIGHTGRAY,   WHITE,        8,9,14,9, 14,15, 8,15, 8,9,
    5,LIGHTGRAY,   LIGHTGRAY,    7,9, 9,7, 13,7, 15,9,  7,9,
    2,LIGHTBLUE,   LIGHTBLUE,    10,10, 10,12,
    2,LIGHTBLUE,   LIGHTBLUE,    12,10, 12,12,
    0xFF,          BLACK, 'О','б','ъ','е','к','т','ы',
    0 };
//                      Большой исключающий крестик
//  13,YELLOW,LIGHTRED, 2,0, 6,4, 10,0, 12,2, 8,6, 12,10, 10,12,
//                      6,8, 2,12, 0,10, 4,6, 0,2, 2,0,
//
static void __Marker_Size( viewporttype &scr, char *Image )
{ if( Image[0]==0  )
  { scr.right =scr.left+Image[1]+3;
    scr.bottom=scr.top +Image[2]+4; Image+=3;
    while( *Image )
       if( *Image!=255 )Image+=unsigned(*Image)*2+3;   else
       { scr.right+=Tv.StrLen( Image+2 )+8; break; } } else
       { scr.right =scr.left+Tv.StrLen( Image )+8;
         scr.bottom=scr.top +Tv.Th+2;
}      }
static void __Marker_Draw( int x, int y, char *Image )
{
  if( Image[0] )Tv.Text( x+6,y+3,W2D( Image ) ); else
  {
   unsigned i,n;             //
   int   j,k,c=Tv.Color;     // Простая рисовалка с указанием
   point s; s.x= *++Image;   // цветов и раскрасок полигонов
            s.y= *++Image;   //
    while( ( n = *++Image )>0 )
    {   color( k=*++Image );
      if( n<255 )
      { pattern( SOLID_FILL,j=*++Image ); point *p=new point[n];
        if( p )
        { for( i=0; i<n; i++ ){ p[i].x = int( *++Image )+x+2;
                                p[i].y = int( *++Image )+y+2; }
          if( n>2          )fillpoly( n,(int*)p );
          if( n==2 || j!=k )drawpoly( n,(int*)p ); delete p;
        } else break;
      } else
      { Tv.Text( East,s.x+x+5,y+s.y/2+1,W2D( ++Image ) ); break;
    } } color( c );
} }
