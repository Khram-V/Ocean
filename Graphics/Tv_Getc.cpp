//
//      Tv_getc : ������ ������� �� ���������
//         key: @D - ����������� ��������� ������������ �������
//              @X - ������� ����� �� ���������
//              ^C - exit
//      return(cr) - <Enter>    - ������� ����� �� ���������� ������
//            (14) - <Insert>   - ����� �� ������� ��������� �����
//            (15) - <Delete>   - ����� �� ������� ���������� �����
//            (16) - <BakSpace> - ��� ��������� �������
//            (17) - <Tab>      - ���������
//            (27) - <Escape>   - ����� � ������� �������
//       (0xF1..�) - <Function>
//
//      Tv_getk : ������ ��������� ���������� ����������
//            1 - RightShift    16 - ScrollLock
//            2 - LeftShift     32 - NumLock
//            4 - Ctrl          64 - CapsLock
//            8 - Alt          128 - Insert
//
#include "Tv_Graph.h"
                                //
       int  Tv_MouseState=0;    // ������� ����������� �������� ����������
static int  Tv_isMouse=NoMouse; //
static BYTE CKey,               // ��� ����������� ������
            VKey;               // ��� ��������� �������
static int KeyPos=0, KeyPas=0;  // Count of keys in KeyBuffer
static struct{ BYTE Code,Key; } //
               KeyBuffer[0x40]; // Keyboard type-ahead buffer
static void Kput()              //
{ KeyBuffer[KeyPas].Code=CKey;  // ����������� ��������� ����� ��� ���� � �����
  KeyBuffer[KeyPas].Key=VKey;        VKey=0;  ++KeyPas; KeyPas&=0x3F;
  if( KeyPas==KeyPos ){ MessageBeep( MB_OK ); ++KeyPos; KeyPos&=0x3F; }
}
static bool Kget()
{ if( KeyPas==KeyPos ){ VKey=CKey=0; return false; }
  CKey=KeyBuffer[KeyPos].Code;
  VKey=KeyBuffer[KeyPos].Key;  ++KeyPos; KeyPos&=0x3F; return true;
}                                    //
bool Tv_isKey()                      //! Return keyboard status
{ MSG Msg;                           //
   while( PeekMessage( &Msg,0,0,0,PM_NOREMOVE ) )
   {      GetMessage ( &Msg,0,0,0 ); //
//    switch( Msg.message )          // ���������� ��������� ��������
//    { case WM_SYSKEYUP:            //   ����� <altSpace>,<atlTab>
//      case WM_SYSKEYDOWN:          //
//       if( Msg.wParam!=VK_SPACE
//        && Msg.wParam!=VK_TAB )Msg.message&=WM_KEYDOWN;
//    }
      TranslateMessage( &Msg );
      DispatchMessage ( &Msg );
   }  return KeyPas!=KeyPos;
}
static BYTE WinAsyncKeyStates( BYTE code=0 )            // ������� �����
{ if( GetAsyncKeyState( VK_LSHIFT   ) )code|=LEFT;      // ��������� ����������
  if( GetAsyncKeyState( VK_RSHIFT   ) )code|=RIGHT;     // � ����������
  if( GetAsyncKeyState( VK_LCONTROL ) )code|=CTRL;     // ��������� ��������
  if( GetAsyncKeyState( VK_RCONTROL ) )code|=CTRL;
  if( GetAsyncKeyState( VK_LMENU    ) )code|=ALT;
  if( GetAsyncKeyState( VK_RMENU    ) )code|=ALT; return code;
}
// WM_CHAR message handler
//
void WindowChar( TCHAR Char )
{ if( Char==VK_CANCEL )_exit( 3 );     // 3
  if( Char==VK_BACK )VKey=_BkSp; else  // 8 -> 16
  if( Char==VK_TAB  )VKey=_Tab; else   // 9 -> 17
                     VKey=W2D( Char );
                     CKey=WinAsyncKeyStates(); Kput();
// printf( "Char=%d, CKey=%d \n",Char,CKey );
}
//#define VK_ALT 0x12 //=18
//   WM_KEYDOWN \ WM_KEYUP
//
void WindowKeyDown( WPARAM wVkey, LONG dwKeyData )
{ struct            // ������ ��� ��������� Tv_Graphics
  { WORD Count;     // ������� �������
    BYTE SCode;     // Scan-��� ������� �������
    BYTE ECode;     // ����������� ��� Windows
  }      WKey;      // ���������� �� ��������� �������
  memcpy( &WKey,&dwKeyData,sizeof( WKey ) ); VKey=0; //WKey.SCode;
  if( WKey.ECode&0x80 )               // ������� ��������
  { if( wVkey==VK_SHIFT   )CKey&=~SHIFT; else
    if( wVkey==VK_CONTROL )CKey&=~CTRL; else
    if( wVkey==VK_MENU    )CKey&=~ALT; return;
  }
//CKey=WinAsyncKeyStates();
  switch( wVkey )                  // 01 - �������, 02 - alt-�������
  { case VK_CONTROL: CKey|=CTRL;      break;
    case VK_SHIFT  : CKey|=SHIFT;     break;
    case VK_MENU   : CKey|=ALT;       break;
    case VK_UP     : VKey=North;      break;
    case VK_LEFT   : VKey=West;       break;
    case VK_DOWN   : VKey=South;      break;
    case VK_RIGHT  : VKey=East;       break;
    case VK_HOME   : VKey=North_West; break;
    case VK_NEXT   : VKey=South_East; break;
    case VK_END    : VKey=South_West; break;
    case VK_PRIOR  : VKey=North_East; break;
    case VK_INSERT : VKey=_Ins;       break;
    case VK_DELETE : VKey=_Del;       break;
//  case VK_TAB    : VKey=_Tab;       break;
    default        :
     if( !(WKey.ECode&0x40) )
     { if( wVkey>=VK_F1 && wVkey<=VK_F12 )VKey=wVkey+(0xF1-VK_F1); else
       if( CKey&ALT )VKey=wVkey;
     }
  }
  if( VKey ){
//  printf( "CKey=%d, VKey=%d, wVkey=%d, dwKeyDate={%d,x%X,x%X}  \n",CKey,VKey,wVkey,WKey.Count,WKey.SCode,WKey.ECode );
    Kput();
  }
}
//  ��������� ��������� � ���������� ������� � EasyWin ������    1994-August-26
//
  void Tv_bell(){ MessageBeep( MB_OK );  } // ������� ������
Course Tv_getk(){ return Course( CKey ); } // ��������� ����������
Course Tv_getc( const int mSec )           // ��������� ������� � ����������
{ while( !Tv_getw() ){ if( mSec ){ Sleep( mSec ); break; } Sleep( 24 ); }
  return (Course)VKey;                     // ~42 ������� � �������
}                                          //
Course Tv_getw()                           // ���� ������ � ������� ����������
{ A:  VKey=0;                              //
  if( Tv_isKey() )
  { Kget();

//if( GetAsyncKeyState( VK_LMENU ) )CKey|=ALT;
//if( GetAsyncKeyState( VK_RMENU ) )CKey|=ALT;
//printf( "CKey=%d, VKey=%d  \n",CKey,VKey );

    if( CKey==ALT && VKey>' ' )
    { switch( VKey|0x20 )
      { case 'c': case 225: Tv_revert(); break;             // 'c'
        case 'd': case 162: Tv_draw();   break;             // '�'
        case 'x': case 231:                                 // '�'
        { int h=Tv.Th; Tv.Height( 8 ); Thelp( "> ���������. �� �������?" );
          Tv_getc(); if( VKey==_Enter || (VKey|0x20)=='y' )exit( 0 );
          Ghelp();   Tv.Height( h ); // �������� �� <alt> !!
      } } goto A;
  } } return Course( VKey );
}
Course Tv_getw( Course Simb ){ if( Tv_getw()==Simb )return Simb; return Center; }
//
//!  <<  ���� �������� �������� (�������������) ������� � �����  >>
//
static int X=0,Y=0;                     // ����������� ������������ �����
 int Tv_Stop_Mouse(){ int i=Tv_isMouse; Tv_isMouse=NoMouse; return i; }
 int Tv_Start_Mouse(){ int i=Tv_isMouse; Tv_isMouse=MouseInstalled; return i; }
 int Tv_MouseStatus(){ return Tv_MouseState; } // { int St=Tv_MouseState; Tv_MouseState=0; return St; }
void Tv_CursorEnable(){  ShowCursor( true ); }    // ������������ �
void Tv_CursorDisable(){ ShowCursor( false ); }   // ������� �������� �������
//void WindowMouse( int x, int y );               //
void WindowMouse( UINT messg, int x, int y )
{       Y=y; X=x;
  switch( messg )
  { case WM_MOUSEMOVE     : Tv_MouseState=1;                   break;
    case WM_LBUTTONDOWN   : Tv_MouseState=_MouseLeftPress;     break; //  2
    case WM_LBUTTONUP     : Tv_MouseState=_MouseLeftRelease;   break; // 32
    case WM_LBUTTONDBLCLK : Tv_MouseState=_MouseLeftDblClick;  break; //  8
    case WM_RBUTTONDOWN   : Tv_MouseState=_MouseRightPress;    break; //  4
    case WM_RBUTTONUP     : Tv_MouseState=_MouseRightRelease;  break; // 64
    case WM_RBUTTONDBLCLK : Tv_MouseState=_MouseRightDblClick; break; // 16
    case WM_MBUTTONDOWN   :
    case WM_MBUTTONUP     :      //
    case WM_MBUTTONDBLCLK :      // ������ ����������
    case WM_MOUSEWHEEL    :      //  � ��������� ��������� �������
   default: Tv_MouseState=0;     //
  }
  if( Tv_isMouse==MouseInstalled )
  {                       VKey=0;
    if( Tv_MouseState&30 )VKey=_MousePressed;  else // ������� � ������� ����
    if( Tv_MouseState&96 )VKey=_MouseReleased; else
    if( Tv_MouseState&1  )VKey=_MouseMoved;
    if( VKey )Kput();
} }
 int Tv_GetCursor( int &x, int &y ){ x=X; y=Y; return Tv_MouseState; }
void Tv_SetCursor( int x, int y  ){ setwindowmouse( X=x,Y=y ); }
 int Tv_MouseX(){ return X; }    // ������� ����������
 int Tv_MouseY(){ return Y; }    //  �������� �������
//
//!      �������������� ����� ������ ����-��������� �������
//
Real Tv_step( Real Di )              // ���������� ���������� ���� ��
{ Real iPart=floor( Di=log10( Di ) ); // ��������� ����������� �������� ����
       Di = exp( (Di-iPart)*M_LN10 );
 return pow( 10.0,iPart )*( Di>6.0 ?2.0 : Di>3.0 ?1.0 : Di>1.5 ?0.5:0.2 );
}
unsigned coreleft()
{ static MEMORYSTATUS Ms={ sizeof(MEMORYSTATUS),0,0,0,0,0,0,0 };
  GlobalMemoryStatus( &Ms ); return Ms.dwAvailPhys;
}
//unsigned long farcoreleft(){ return coreleft(); }  // GetFreeSpace( 0 );
//                                                  // GlobalCompact( 0 )/1024;
//!      ��������� ���������� ���������
//
void Break( const char Msg[],... )
{ va_list aV;
  va_start( aV,Msg ); va_end( aV ); char Str[120]; vsprintf( Str,Msg,aV );
  MessageBox( NULL,Str,"Break",MB_ICONASTERISK|MB_OK ); _exit( 1 );
}
//      �������������� ����� ������ ����� ���� ����������
//                   � ����� ������������� ��������
//
#include "..\Graphics\Tv_Draw.cpp"
//#include "..\Graphics\Tv_Axis.cpp"
//#include "..\Graphics\Tv_Text.cpp"
//#include "..\Graphics\Tv_Case.cpp"
//#include "..\Graphics\Tv_File.cpp"
//#include "..\Graphics\Tv_Help.cpp"
