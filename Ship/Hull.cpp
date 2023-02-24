//
//      ��������� ��������� �������������� ������� ������� �����
//
//      � ������� ��������� ������������� ���������� �������
//              � �������������� �������
//
//                                 (c)1975-03.08.31, �.�.��������
//                              ���. �������������� �������������
//                                   � ������� ������������
//                      ������������ ���������������� ������������
//
// ������� �����������:
// 02-02-09 -- � ������� ���������� �������� ����� ����� � ��������� ����
//          +- ����������� ������������� ����������, ���������� � ��������
// 03-07-25 -- �������� � ����� ����������� ������
//          ++ ��� �������� � ������� ����������� ���������� ������
//
#include "..\Ship\Hull.h"
#include "..\Graphics\Tf_topo.h"
//extern "C"
//{ _cdecl matherr( struct exception *a ){ a->retval=0.0; return 1; }
//  _cdecl _matherrl( struct _exceptionl *a ){ a->retval=0.0; return 1; }
//}
const char
*Months[]={ "������","�������","����","������","���","����",
            "����","������","��������","�������","������","�������" },
*WeekDay[]={ "�����������","�������","�����","�������","�������",
             "�������","�����������"
           };
static char Msgs[]="%ld ��, H ������� "
        "F1 ����� F2 ������ F3 ������ F4 ������ F5 ������� F8 ����� F9 ������";

int  Hull_Keys=0x00;                 // ����� ������ - ����������
                                     // x01 -  ������������� �����-������������

Real Xo=0,Xm=1,Length=2,Lwl=2,       // �����
               Breadth=.2,Bwl=.2,    // ������
     Do=0,     Draught=.1,Height=.2, // ������
               Volume=0.4,           // �������������
               Surface=0.2;          // ������� ��������� �����������
                                    //
void Hull_Wave( const int Type );  // ���������������� � �������� �������������
void Hull_Statics();               // ������ ��������� �������������� �������
//
//   ��������� ���������� �������� �����
//  � �������� �� ��������� �������������
// ---------------------------------------
                                        //
DrawPlane wH( "������",    "Y","Z" ),   // ���� �������� ������,
          wM( "���",       "X","Z" ),   //  ���
          wW( "����������","X","Y" );   //   � ����������
                                        //
static int Console();

static void Init()
{ int i,k;
  color( LIGHTGRAY );
  k=Tv.Th; Tv.Th=10; i=Tv.mY/20+3;
  Tgo( 0,++i ); Tprintf( "L=%.3lg",Length );
  if( fabs( Length-Lwl )>Length/500 )
                Tprintf(  "/%.3lg",Lwl );
  Tgo( 0,++i ); Tprintf( "B=%.3lg",Breadth );
  if( fabs( Breadth-Bwl )>Breadth/500 )
                Tprintf(  "/%.3lg",Bwl );
  Tgo( 0,++i ); Tprintf( "T=%.3lg",Draught ); //Kh.Init();
  Tgo( 0,++i ); Tprintf( "V=%.6lg",Volume  );
  Tgo( 0,++i ); Tprintf( "S=%.5lg",Surface );
  Tgo( 0,++i ); Tprintf( "\x7F=%.3lf",Volume/Bwl/Lwl/Draught );
  Tgo( 0,++i ); Tprintf( "\x0F=%d",Kh.Ms );
  Tv.Th=k;
}
int main() // int ac, char** av )   // ��������������� ����������� �����
{
  Tfile_Filter=(char*)"Ship Hull Lines Drawing (*.vsl)\0*.vsl\0"
                      "All Files (*.*)\0*.*\0";
  Tv_Graphics_Start();          // ��������� �������� �� 256 ������
  setwindowtitle( "Ship - computational fluid mechanics" );
  Tv_Start_Mouse();             // ������ ��������� "�����"
  Tv_SetCursor( 0,0 );          // ���������� ����� � �������� ������ ����
  Tv.Font( _Small_font,16 );    // ����� ����������� ������ �� 12 ��������
  if( _ac_==1 )Kh.Read();       // ��� ������ ��� ���������� -> Hull.vsl
      else Kh.Read( _av_[1] );  // ������������, �������� ��� ����� �������
  Building();                   // ������ ���������� �� ������
  Init();                       // ����� ������� �� �������
  while( Console() );           //
}
static Course Console_Wait()
{
 int M,D,Y,k,c=color( WHITE );
 Course Ans;
  do
  { Event Record_Time; //.Now();
    Record_Time.UnPack( M,D,Y ); Tpoint.y=Tv.mY-Tv.Th-10; Tpoint.x=1;
    k=Tv.Th; Tv.Th=10;
    Tprintf( "%04d.%s.%02d ",Y,W2D( Months[M-1] ),D );
    Tprintf( "%s%-10s",W2D(WeekDay[int(Record_Time.D%7)]),DtoA(Record_Time.T) );
    Tv.Th=k;
    Ans=Tv_getc( 250 );
  } while( !Ans );
  color( c );
  return Ans;           //
}                       // ������� ���������� � ������ ������
static fixed Is_Info=0; // 0x01 - ������� ���������� �� ���� ��������
static int Console()    // 0x02 - ������������� ���������
{                       //
 const char *Name[]={ "Hull","  ������ � ���������",
                             "������������ �������",0 },
            *Text[]={ "F1    ","������",
                      "F2/F3 ","������, ����������",
                      "F4    ","�������������� ������",
                      "F5    ","����� � �������",
                      "F8    ","����������������",
                      "F9    ","������ - �������",0 },
            *Plus[]={ "<������> ","������ �������",
                      " <altP>  ","������ �������",
                      " <altC>  ","����� �������",
                      " <altD>  ","���������",
                      " <altX>/<ctrlC> ","����",0, };
 int x,y,ans=1; color( CYAN );
  do
  { Tv_GetCursor( x,y );
    if( wH.Info( x,y ) )
    { int i; Real Z,Y; Z=Y=Tv_X( x );
      color( YELLOW ); Tgo( 0,-3 ); Tprintf( "Y=%5.3lf, ",Y );
      color( WHITE );
      while( (i=Kh.F[10].iZ( Z ))>0 )Tprintf( "{%d} Z=%5.3lf,  ",i,Z );
      Tprintf( "\n" );
    } else
    if( wW.Info( x,y ) ){} else
    if( wM.Info( x,y ) ){} else
    { if( Is_Info&0x01 ){ Tgo( 0,0 ); Tprintf( "\n" ); Is_Info&=~0x01; }
      if( !(Is_Info&0x02) )
      { Is_Info|=0x02; Thelp( ans==' '?"":Msgs,coreleft()>>20 );
    } }
    ans=Console_Wait();
    switch( ans )
    { case 0xF1: ans=Set_Parameters();  break;
      case 0xF2: ans=Kh.Write();        break;
      case 0xF3: ans=Kh.Read( "*.vsl"); break;
      case 0xF4:
      { int k;
        const char p[]={ 2,11 },*n[]={ "�������","������" },
                                *m[]={ "�������","������" };
        for(;;)           //
        { Tgo( 1,0 );     // ���������� ������� ����� ���� ���������
          color( GREEN ); //
          for( k=0; k<sizeof p; k++ )Tprintf( " %s ",W2D( n[k] ) );
                                     Tprintf( " \x1b\x09\x1a\n" );
          if( ans<0 )ans=sizeof( p )-1; else
          if( ans>=sizeof p )ans=0;
          color( LIGHTCYAN ); Tgo( p[ans],0 ); Tprintf( W2D( m[ans]) );
              k=Tv_getc();
          if( k==East  )ans++; else
          if( k==West  )ans--; else
          if( k==_Esc  ){ ans=1; break; } else
          if( k==_Enter){ if( ans )ans=Set_Sinus();
                              else ans=Set_Power(); break;
                        }
        } Tgo( 1,0 ); Tprintf( "\n" );
      }               Is_Info=0; break;
      case 0xF5: Hull_Statics(); break;
      case 0xF8: Hull_Wave( 0 ); break;
      case 0xF9: Hull_Wave( 1 ); break;
      case ' ' : Ghelp();        break;
      case _Esc: Thelp( "? ����� �� ��������� [y/n]" );
           ans=Tv_getc(); if( ans==_Enter || (ans|0x20)=='y' )return 0;
      default: if( ans=='h' )ans=Help( Name,Text,Plus ); break;
    }
    if( ans!=_MouseMoved )Is_Info&=~0x02;
  } while( ans>_Enter ); if( !ans || ans==_Enter ){ Building(); Init(); }
  return 1;
}
//
//      �������������� ������ � ��������� �������
//
bool DrawPlane::Info( int x, int y )
{
 static DrawPlane *_Prev=NULL; static int k=1;
  if( Is( x,y ) )
  { int c=color( LIGHTGRAY ); Tpoint.y=Tv.mY-Tv.Th+2; Tpoint.x=1;
    if( _Prev!=this ){ _Prev=this; Focus(); Is_Info&=~0x01; }
    if( Is_Info&0x02 ){ Ghelp(); Is_Info&=~0x02; }
    if( Is_Info&0x01 )Tpoint.x=k; else
      { Tprintf( "%s ",W2D( iD ) ); k=Tpoint.x; Is_Info|=0x01;
      }
    color( CYAN   ); Tprintf( aX );
    color( YELLOW ); Tprintf( " %.2lf ",Tv_X( x ) );
    color( CYAN   ); Tprintf( aY );
    color( YELLOW ); Tprintf( " %-12.2lf",Tv_Y( y ) );
    color( c );
    return true;
  } return false;
}
