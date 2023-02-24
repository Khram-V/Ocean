/* A conio implementation for Mingw/Dev-C++. Version 1.4
 * Written by:  Hongli Lai <hongli@telekabel.nl>
 *              Tkorrovi <tkorrovi@altavista.net> on 2002/02/26.
 *              Andrew Westcott <ajwestco@users.sourceforge.net>
 *              Adrian Sandor <aditsu@yahoo.com> 2004/11/07
 * Offered for use in the public domain without any warranty.
 */
#include <windows.h>
#include "ConIO.h"
#ifdef __cplusplus
extern "C" {
#endif
static int __BACKGROUND = BLACK, __FOREGROUND = LIGHTGRAY;
static CHAR_INFO screen[65536];
static con_info cinf;

#define getinf con_getinfo(&cinf)
#define conout GetStdHandle( STD_OUTPUT_HANDLE )

void clrscr()
{    getinf; con_fillcr( ' ',cinf.totalwidth*cinf.totalheight,0,0 );
     con_gotoxy( 0,0 );
}
void clreol()
{    getinf; con_fillcr( ' ',cinf.right-cinf.cx+1,cinf.cx,cinf.cy );
}
void gotoxy( int x,int y ){ con_gotoxy( x-1,y-1 ); }
int wherex(){ int x,y; con_wherexy( &x,&y ); return x+1; }
int wherey(){ int x,y; con_wherexy( &x,&y ); return y+1; }
void textattr(int attr)
{    SetConsoleTextAttribute( conout, attr );
     __BACKGROUND=( attr>>4 )&15;
     __FOREGROUND=attr&15;
}
void textbackground( int color ){ textattr(__FOREGROUND + (color<<4)); }
void textcolor     ( int color ){ textattr(color + (__BACKGROUND<<4)); }
void insline()
{    getinf; con_scrolldown( cinf.left,cinf.cy,cinf.right,cinf.bottom );
}
void delline()
{    getinf; con_scrollup( cinf.left,cinf.cy,cinf.right,cinf.bottom );
}
/* 1.2: workaround, see conio.h */
int _gettext( int left, int top, int right, int bottom, void*buf )
{   if( !con_get( left-1,top-1,right-1,bottom-1,screen ) )return 0;
    for( int i=0; i<(right-left+1)*(bottom-top+1); ++i )
    { ((char*)buf)[2*i]   = screen[i].Char.AsciiChar;
      ((char*)buf)[2*i+1] = screen[i].Attributes;
    } return 1;
}
int puttext(int left, int top, int right, int bottom, void*buf)
{   for( int i=0; i<(right-left+1)*(bottom-top+1); ++i )
    { screen[i].Char.AsciiChar=((char*)buf)[2*i];
      screen[i].Attributes=((char*)buf)[2*i+1];
    }
    return con_put(left-1, top-1, right-1, bottom-1, screen);
}
int movetext( int left, int top, int right, int bottom, int newleft, int newtop )
{   if( !con_get( left-1,top-1,right-1,bottom-1,screen ) )return 0;
    return con_put( newleft-1,newtop-1,newleft-1+right-left,newtop-1+bottom-top,screen );
}
void gettextinfo( text_info *r )
{ CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo( conout,&info );
    r->winleft=1;                       // no conio-window support
    r->wintop=1;
    r->winright=info.dwSize.X;
    r->winbottom=info.dwSize.Y;
    r->attribute=info.wAttributes;
    r->normattr=info.wAttributes;       // ??
    r->currmode=0;                      // meaningless
    r->screenwidth=info.dwSize.X;
    r->screenheight=info.dwSize.Y;
    r->curx=info.dwCursorPosition.X+1;
    r->cury=info.dwCursorPosition.Y+1;
}
void _setcursortype (int type)
{ CONSOLE_CURSOR_INFO info;
    info.dwSize = ( type==0 )?1:type;
    info.bVisible = ( type!=0 );
    SetConsoleCursorInfo( conout,&info );
}
void delay( unsigned msec ){ Sleep( msec ); }
void sleep( unsigned sec ){ Sleep( sec*1000 ); }
void con_gotoxy( int x,int y )
{ SetConsoleCursorPosition( conout,(COORD){ x,y } );
}
void con_wherexy(int*x, int*y)
{ CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo( conout,&info );
    *x = info.dwCursorPosition.X;
    *y = info.dwCursorPosition.Y;
}
void con_getinfo(con_info *inf)
{ CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo (conout, &info);
    inf->left       =info.srWindow.Left;
    inf->top        =info.srWindow.Top;
    inf->right      =info.srWindow.Right;
    inf->bottom     =info.srWindow.Bottom;
    inf->width      =inf->right-inf->left+1;
    inf->height     =inf->bottom-inf->top+1;
    inf->totalwidth =info.dwSize.X;
    inf->totalheight=info.dwSize.Y;
    inf->maxwidth   =info.dwMaximumWindowSize.X;
    inf->maxheight  =info.dwMaximumWindowSize.Y;
    inf->attr       =info.wAttributes;
    inf->cx         =info.dwCursorPosition.X;
    inf->cy         =info.dwCursorPosition.Y;
}
void con_setwindow(int left, int top, int right, int bottom)
{ SMALL_RECT sr={ left,top,right,bottom };
    SetConsoleWindowInfo( conout,TRUE,&sr );
}
void con_resize( int width,int height )
{ int w=width, h=height;
  getinf; if( w<cinf.totalwidth  )w=cinf.totalwidth;
          if( h<cinf.totalheight )h=cinf.totalheight;
          con_setbuffer( w,h );
          con_setwindow( 0,0,width-1,height-1 );
          con_setbuffer( width,height );
}
void con_setbuffer(int width, int height)
{    SetConsoleScreenBufferSize( conout, (COORD){ width,height } );
}
void con_crop(){ getinf; con_setbuffer( cinf.width,cinf.height ); }
void con_settitle( const char*str ){ SetConsoleTitle( str ); }

void con_fillchar( char c,int n,int x,int y )
{ DWORD written;
    FillConsoleOutputCharacter( conout,c,n,(COORD){ x,y },&written );
}
void con_fillattr(int fg, int bg, int n, int x, int y)
{ DWORD written;
    FillConsoleOutputAttribute( conout,fg+(bg<<4),n,(COORD){ x,y },&written );
}
void con_fill(char c, int fg, int bg, int n, int x, int y)
{    con_fillchar( c, n, x, y );
     con_fillattr( fg, bg, n, x, y );
}
void con_fillcr( char c, int n,int x,int y )
{    con_fill( c,__FOREGROUND,__BACKGROUND, n,x,y );
}
int con_get( int left, int top, int right, int bottom, void *buf )
{ SMALL_RECT sr={ left,top,right,bottom };
  return ReadConsoleOutput
  ( conout,(_CHAR_INFO*)buf,(COORD){right-left+1,bottom-top+1},(COORD){0,0},&sr );
}
int con_put(int left, int top, int right, int bottom, void*buf)
{  SMALL_RECT sr={ left,top,right,bottom };
   return WriteConsoleOutput
   ( conout,(_CHAR_INFO*)buf,(COORD){ right-left+1,bottom-top+1 },(COORD){ 0,0 },&sr);
}
int con_getchars(int left, int top, int right, int bottom, char *str)
{   if(!con_get(left, top, right, bottom, screen))return 0;
    for(int i=0; i<(right-left+1)*(bottom-top+1); ++i)
        str[i]=screen[i].Char.AsciiChar;
    return 1;
}
int con_putchars( int left, int top, int right, int bottom, char *str )
{   if( !con_get(left,top,right,bottom,screen ) )return 0;
    for( int i=0; i<(right-left+1)*(bottom-top+1); ++i )screen[i].Char.AsciiChar=str[i];
    return con_put(left, top, right, bottom, screen);
}
int con_getattrs(int left, int top, int right, int bottom, int*buf)
{   if(!con_get(left, top, right, bottom, screen))return 0;
    for( int i=0; i<(right-left+1)*(bottom-top+1); ++i )buf[i]=screen[i].Attributes;
    return 1;
}
int con_putattrs( int left, int top, int right, int bottom, int*buf )
{   if( !con_get( left,top,right,bottom,screen ) )return 0;
    for( int i=0; i<(right-left+1)*(bottom-top+1); ++i )screen[i].Attributes=buf[i];
    return con_put( left,top,right,bottom,screen );
}
void con_scroll( int left, int top, int right, int bottom, int newleft, int newtop )
{ SMALL_RECT sr={left, top, right, bottom};
  CHAR_INFO c={ {' '},__FOREGROUND+(__BACKGROUND<<4) };
    ScrollConsoleScreenBuffer( conout,&sr,&sr,(COORD){newleft,newtop},&c );
}
void con_scrollup   ( int left, int top, int right, int bottom )
{    con_scroll     ( left,top,right,bottom,left,top-1 ); }
void con_scrolldown ( int left, int top, int right, int bottom )
{    con_scroll     ( left,top,right,bottom,left,top+1 ); }
void con_scrollleft ( int left, int top, int right, int bottom )
{    con_scroll     ( left,top,right,bottom,left-1,top ); }
void con_scrollright( int left, int top, int right, int bottom )
{    con_scroll     ( left,top,right,bottom,left+1,top );
}
#ifdef __cplusplus
}
#endif
