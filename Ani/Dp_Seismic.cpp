/*
      List.cpp                                                1993.05.15
                                                              �.�.��������

  ��������� ���������� ���������������� ������� �� ��������� ����� ����������

  ( ����������� ��������� �������� � �������� World, � ������� ��������������
    ����� �������� � ������������ � ��������������� ������ ������������� )

    � ��������� ����� ��������� ��������� �� ���������� ��������� � �������
������������� ��������� TDB (Tsunami Data Base). � �������� ��� ������,
������������ �.�.���������� � ��� ��� ��.
    ��� ���������� �������� (F2), ��������� ������ � ����� ��������� �� ������
�������, ������� ������� ����� ������������ � ������� ������� �� ����������.
    �������� ������� ��������� � ������ ����� ������, ��� ������� ���������,
� �������� ������������� ������������ ����������� ������ �� �����.
    ����������� ����� ����������� ������� ������ �� ������ ��������� ��� ����
����������, ����������, �������� �������� � ���� ���������� ������ � ������
������� � ����.

    ������ ������ ���������� ��������:

YYYY MM DD hh mm ss.s �FF.ff �LLL.ll DDD M.s II.i HHH.hh
---------- ---------- ------ ------- --- --- ---- ------
      ����            ������ ������� �������
            �����������                   ���������
            �����                             ������������� �
                                                  ������������ ���������
                                                  ������
    ������ ������ ������

1952 11  4 16 58 21.6  52.75  159.50  30 8.5  4.0  20.00
1953  3 17 13  4 42.0  50.00  156.40  70 6.2  1.0   3.00
1964  3 28  3 36 13.9  61.05 -147.48  23 8.5  4.0  51.80
1964  6 16  4  1 39.9  38.35  139.18  40 7.5  2.0   6.40
1965  2  4  5  1 21.8  51.30  178.60  36 8.2  2.5  10.00
1982  3 21  2 32  5.9  42.15  142.55  43 7.2  0.0   1.30
1983  5 26  2 59 57.5  40.36  139.08  20 7.8  2.0  18.00
1983  6 21  6 25 25.0  41.26  139.00   6 6.9  0.0   1.00

     ����������� ��������� ������� ���������� ����������

  F1 ����������� ������� ����������� ����������� ������:

     Depth - ������� ����� �������������, ����������� �������
             ��������� ���������� ������� ����� ���������.
             ( 6 �������� = 0-33-80-150-300- � ����� ���������� )
     Ms    - ��������� ������������� ����������� �������� �����
             ���������. �������� ����� �� ������������, � �����
             ������������ ������� ���������� ��� 4-� ��������.
     Its   - ������������� ������ �� �.��������� ������������
             ������ � "�����������" ����������� ���������, ���
             ���� ��� ������ ��������� ������� ����� ����������
             ���������, ��������, �� ��������� ���������.
        <0    ����� ����
       0.5    ������� ��� ���� ���������� ���������
       0.5-1  ������� ��� �������� � ������� ���������� �������
         1-2  ������ � ������ ��������� � ����������� ��������
         2-3  ���� ���������� � ���������� ������ ������� �������
        >3    ������� ���� � ������ ����������� �������� ����������

  F2 ������ �������� �������� � ����.
  F3 ���������� �������� � �������� ������ ��� ������� ����������������
     ����������� �� ������. ��� ���������� ������ ������������ ���������
     ���������� ������� ������, � ����� ������ �������� ������ ���
     ������� �������. ��� ������ � ������� � ����������� ������
     ���������� �� ������ ����� ���� ����� �� ��������, �� ��� ���� �
     ������ �������� ������ ������� ��� ��� �������� ������ �������
     ������������ ������� �������.
  F4 ����������� ���������� ������� �������������� � ������� ��������,
     � ������� ����� ���������� ������ �������� ������� �������������
     ������� ������:
     ---------------------------�
     �    Current Boundaries    �
     �                          �
     � 1747-10-17 .. 1994-04-08 � ���� ��� ������ � ��������� ������
     �                          �
     � Depth    0 .. 357 km     � �������� ������ ��� ����������
     �    Ms  0.0 .. 8.5        �  � ��������� �������������.
     �    It -4.0 .. 9.9        � ������������� ������.
     L---------------------------
     ����������� ��������� ����������� �� ������� <Enter>, ���� ��
     ����� �� ���� �� ������� <Escape>, �� ������ �� ����������.
  F5 ����������� ������ ��������� ������������� ��� ����� ��������.
     ��� ����������� ���������, �� ��� ������ ���� ����� ����������
     ������� ���� ����� � ��� ������������ ������.
  F6 ���������� �������� ������, � ������ ���������� ��������� ���
     ������ ���������� ���������� ��������� ����� ��������, �������
     ������������� ���������� �� ����������� ���� � ������� �������.
  F7 ����������� ���������� ������ �������, �������� ������� ��
     �������� �������������� �������� ��������� �� ������, �����
     ����������� ����������� ���������� ���� ������ ������� �
     ����������.
  <Insert> - ����������� �� ������ ����� ������ - �������� �������.
  <�����..>- ������� ����� ������ ����� ������ �� �� ����������.
  <Escape> - ����� �� ���������, �� �������� �� ������������ �����
             �������� ��������, ��� ��������, ��� ���� � ���������,
             ���� �� �����-���� ������ ���������, ����� �� ��������
             ������� ������� �� ��������.
*/
#include <Ctype.h>
#include "Depth.h"
#define Start_File "Ani.lts"

struct Seismo                   //
{ Event  T;                     // ���� � �����
  Point  G;                     // �������������� ����������
  Real   Hg,Ms,It,Am;           // �������, ���������, �������������, ���������
  bool   Read( FILE *F );       // ������ ������� ������ �� �����
  Course Edit( bool=false );    // false - �����, ��� �������������� �����
  void   Item( bool _x=false ); // ���������� ������ ������� �� ������
  void   Inform();              // ��� ��� ��������� ������ Tmp
  void   PutString( FILE *F );  // ������ ����� �������� ������� - ��� ��������
};                              //
static Seismo *Lsm=NULL,Tmp;    //
static int     Nsm=0,ist=0;     // ���������� � ����� �������� ������
static const Real Mit=-10;      // ����������� ������� �������������
                                //
static char FileName[MAX_PATH]=Start_File,
            Msg[133];       // ����������� �������������� �������
static Event J1,J2;         // �������� ������� � ��������� ����
static Real  Hg1,Hg2,       // ������� �����
             Ms1,Ms2,       // ���������
             It1,It2,       // ������������� ������ �� ���������
             Yasp;          // ������ ������������ ���������
static point msp={ 0,0 };   // �������������� ����� �� ������
static fillsettingstype FL; //
inline bool GetMouse()      // ����������� � �������� ��������� ����� � msp
{ Tv_GetCursor( msp.x,msp.y );
  return msp.x>Tv_port.left && msp.x<Tv_port.right
      && msp.y>Tv_port.top && msp.y<Tv_port.bottom;
}
//
//   ����� ������������ ������� ���������� ������ �� ������ � ������
//
bool Seismo::Read( FILE *F )
{ char *s=Msg;
  int l,x,y,Year,Hour,Minute; // ���� � ����� �������� �������
  Real Sec;
    if(  !fgets( Msg,132,F ) )return false; while( *s && *s<=' ' )s++;
    if( !(*s) || (*s!='-' && !isdigit( *s )) )return false;
    sscanf( s,"%d",&Year ); while( *s && *s>' ' )s++;    // � ����� ��� �����
    if( (l=strcut( s ))<30 )return false;               // ������������� ������
    s[3]=0; x=max( atoi( s ),1 ); s+=4;                //
    s[2]=0; y=max( atoi( s ),1 ); s+=3; T.Pack( x,y,Year );
    s[2]=0;   Hour=atoi( s ); s+=3;
    s[2]=0; Minute=atoi( s ); s+=3;
    s[4]=0;    Sec=atof( s ); s+=5;    T.T = (Sec/60.0 + Minute )/60.0 + Hour;
    s[6]=0;    G.y=atof( s ); s+=7;
    s[7]=0;    G.x=Angle( atof( s ) ); s+=8; It=Mit; Hg=Ms=Am=0.0; Hour=0;
  R: Chart.D2I( G,y,x );                              // ����� �� ������� �����
    if( x<0 || x>=Chart.Nx || y<0 || y>=Chart.Ny )
    { if( !Hour ){ Hour=1; if( G.x<0 )G.x+=360; else G.x-=360; goto R; }
      return false;
    }
    if( l>33 )
    { s[3]=0; Hg=atof( s ); s+=4;
      if( l>37  )
      { s[3]=0; Ms=atof( s ); s+=4; if( l>41 )sscanf( s,"%lg%lg",&It,&Am );
    } } if( It>9.89 )It=Mit;
        if( Ms>9.9 )Ms=9.9; return true;
}
static void Open()
{ static bool New=true; FILE *F;          //
  if( New )                               // �������� ������� � ������
  { New=false;                            // ������ � ������ ���
    F=fopen( Start_File,"r" );            // ������� �������� "Ani.lts"
    if( !F )                              //
    { strcpy( fname( strcpy( FileName,_av_[0] ) ),Start_File );
      F=fopen( FileName,"r" );
    }
    if( !F ){ Tmp.Edit(); return; }       // ���� ��� - �������� ������ �������
  } else
  { char *M=Tfile_Filter; // ����� �������� ����� ����� ����� �����������������
    strcpy( sname( FileName ),"*" );
    Tfile_Filter=(char*)( "Seismic Events Catalogue (*.lts)\0*.lts\0"
                          "All Files (*.*)\0*.*\0" );
    F=Topen( FileName,"r","lts","����� �������� ������������ ������ <.lts>" );
    Tfile_Filter=M;
  }
  if( !F ){ Tmp.Edit(); return; } else    // ������� ������� ������������ �����
  { int n=Nsm;                            // �������� ���������� ����� � ������
    if( Nsm )
    { Thelp( "<Tab>-�������  <Enter>-��������?" );       // � ������ �� ������
      int i; do{ if( (i=Tv_getc())==_Enter ){ Nsm=0; break; } }while( i!=_Tab );
    } Thelp( "������ ����������������� ������, ���������" );
    while( !feof( F ) )
    { if( Nsm>=n )Lsm=(Seismo*)Allocate( (n=Nsm+136)*sizeof(Seismo),Lsm );
      if( !Lsm[Nsm].Read( F ) )continue;
     Seismo &L=Lsm[Nsm];
      if( !Nsm ){ J1=J2=L.T; Hg1=Hg2=L.Hg; Ms1=Ms2=L.Ms; It1=It2=L.It; } else
      { if( L.It>Mit )
        if( It1>L.It )It1=L.It; else if( It2<L.It )It2=L.It;
        if( J1>L.T   )J1=L.T;   else if( J2<L.T   )J2=L.T;
        if( Hg1>L.Hg )Hg1=L.Hg; else if( Hg2<L.Hg )Hg2=L.Hg;
        if( Ms1>L.Ms )Ms1=L.Ms; else if( Ms2<L.Ms )Ms2=L.Ms;
      } ++Nsm;
    } fclose( F ); if( !Nsm )ist=0; else if( ist>=Nsm )ist=Nsm-1;
  }
}
static int sort_forward( const void *a, const void *b )
     { return ((Seismo*)a)->T < ((Seismo*)b)->T ? -1:1; }
static int sort_back( const void *a, const void *b )
     { return ((Seismo*)a)->T > ((Seismo*)b)->T ? -1:1; }
static void Sorting()
     { static bool F=true; Thelp( "...���� ����������" );
       qsort( (void*)Lsm,Nsm,sizeof( Seismo ),(F=!F)?sort_back:sort_forward );
     }
//!      ��������� ��� �������� ���������� �������
//
static void Select()
{
 static int  Y1,Y2,M1,M2,D1,D2;
 static Real G1,G2,I1,I2,H1,H2;
 static Mlist Menu[]=
  { { 1,0,"\t ������� � ������ �������" }
  , { 2,5,   "%5d",&Y1 },{ 0,2,"-%02d",&M1 },{ 0,2,"-%02d",&D1 }
  , { 0,5," ..%5d",&Y2 },{ 0,2,"-%02d",&M2 },{ 0,2,"-%02d",&D2 }
  , { 2,4," Depth %4.0lf",&H1},{0,4," .. %-4.0lf",&H2},{0,0,"\t ��    "}
  , { 1,4,"    Ms %4.1lf",&G1},{0,4," .. %-4.1lf",&G2}
  , { 1,4,"    It %4.1lf",&I1},{0,4," .. %-4.1lf",&I2} };
 int ans,k,n;
  J1.UnPack( M1,D1,Y1 ); H1=Hg1; G1=Ms1; I1=It1;
  J2.UnPack( M2,D2,Y2 ); H2=Hg2; G2=Ms2; I2=It2; Thelp( "<Enter>�������, <Esc>������");
  do
  { ans=Tmenu( Mlist( Menu ),0,1 );
    if( ans<=13 )
    { J1.Pack( M1,D1,Y1 ); Hg1=H1; Ms1=G1; It1=I1; Thelp( "���� ������� ������");
      J2.Pack( M2,D2,Y2 ); Hg2=H2; Ms2=G2; It2=I2;
      for( k=n=0; k<Nsm; k++ )
      { Seismo &L=Lsm[k];
        if( L.Ms>=G1 && L.Ms<=G2 )
        if( L.Hg>=H1 && L.Hg<=H2 )
        if(  L.T>=J1 && L.T<=J2  )
        if( L.It>=I1 && L.It<=I2 ){ if( k!=n )Lsm[n]=L; ++n; }
      } if( n<Nsm )Lsm=(Seismo*)Allocate( (Nsm=n)*sizeof(Seismo),Lsm ); break;
    }
  } while( ans!=_Esc ); Ghelp();
}
//
//      ��������� ��������� ������ �������
//
void Seismo::Item( bool _xor )
{ int i,cl;
 const point Star[]={ { 0,1000},{ 383, 924},{ 707, 707},{ 924, 383}
                    , { 1000,0},{ 924,-383},{ 707,-707},{ 383,-924}
                    , {0,-1000},{-383,-924},{-707,-707},{-924,-383}
                    , {-1000,0},{-924, 383},{-707, 707},{-383, 924},{0,1000} };
 point c[17],p;
 Point P=G; Chart.DtI( P ); p=P;
 Real D=Ms*Ms*Ms/4.0e4,d;
  if( !_xor )D*=1.12; d=D;
  if( It>0 ){ D*=1.6-0.6/(It+1.0);
              d*=0.4+0.6/(It+1.0);
            }
  for( i=0; i<=16; i++ )
  { c[i].x=p.x+int( Star[i].x*(i&1?d:D) );
    c[i].y=p.y+int( Star[i].y*(i&1?d:D)*Yasp );
  }
  color( cl=It <  0.0 ? LIGHTGRAY:
            It <= 0.5 ? CYAN     :
            It <= 1.0 ? GREEN    :
            It <= 2.0 ? LIGHTBLUE:
            It <= 3.0 ? MAGENTA  : BROWN );
  if( !_xor )
  { setwritemode( XOR_PUT ); drawpoly( 17,(int*)c );
    setwritemode( COPY_PUT );
  } else
  { if( Hg>=0.0 )
    { pattern( SOLID_FILL,WHITE ); fillpoly( 17,(int*)c ); }
      pattern( Hg>300? LTSLASH_FILL:
               Hg>150?   HATCH_FILL:
               Hg>80 ?  XHATCH_FILL:
               Hg>30?CLOSE_DOT_FILL:
               Hg>0 ? WIDE_DOT_FILL:EMPTY_FILL,cl );
      fillpoly( 17,(int*)c );
      pattern( FL.pattern,FL.color );
  }
}
//      ���������� ���� ������� �������
//
static void Draw()
{ for( int i=0; i<Nsm && !Tv_getw(); i++ )Lsm[i].Item( true ); //ViewPointMario();
}
//      ���������� ���������� � ��������� ����� ������������� �������
//
void Seismo::PutString( FILE *F )
{ int Year,Month,Day,Hour,Minute; Real Sec;
    T.UnPack( Month,Day,Year ); Hour=T.T;
                              Minute=(T.T-Hour)*60.0;
                                 Sec=((T.T-Hour)*60.0 - Minute)*60.0;
    fprintf( F,"%5d %02d %02d",Year,Month,Day );
    fprintf( F,!T.T?"  _  _  _  ":" %2d %02d %04.1lf",Hour,Minute,Sec );
    fprintf( F," %6.2lf %7.2lf",G.y,Angle( G.x ) );
    fprintf( F,Hg?" %3.0lf":"   _",Hg );
    fprintf( F," %3.1lf",Ms );
    if( It==Mit ){ if( Am!=0 )fprintf( F,"  _  " ); }
                         else fprintf( F," %4.1lf",It );
                              fprintf( F,Am?"%7.2lf\n":"\n",Am );
}
static
const char *Title=" Year Mn Dt Hr Mn Secn Latitu Lomgitu Dep Ms_ It._ Hmax__\n";
static void Save()
{ FILE *F; int i,c=color( GREEN ); // ����� ������ ���������� ���������
  Thelp( "?FileName: " );
  Tgo( -11,0 ); i=Wgets( FileName,-69,int( fname( FileName )-FileName ) );
  Ghelp();     // OemToCharBuff( FileName,FileName,strcut( FileName )+1 );
  if( i==_Enter )
  if( (F=Topen( FileName,"w","lts" ))!=NULL )
  { fputs( Title,F ); for( i=0; i<Nsm; i++ )Lsm[i].PutString( F ); fclose( F ); }
    color( c );
}
//      ����������� ������� � ����� ����� ������
//
void Seismo::Inform()   // ������������ ��������� ������ Tmp
{ int   y,h;
  color( WHITE );  Twindow( 1,1,9,33 ); point p; h=Tv.Th+3; y=32;
  color( GREEN );
  p.x=38,p.y=26;   Tv.Text( North,p,"Depth" );
  p.x=40,p.y+=h*8; Tv.Text( North,p,"Ms"  );
         p.y+=h*7; Tv.Text( North,p,"Its" );
  color( LIGHTGRAY ); It=Mit;
                      Hg=0.0;
  Tv.Height( 8 );          // settextjustify( LEFT_TEXT,CENTER_TEXT );
  p.x=36,p.y=42;   Tv.Text( East,p,W2D( " 0��" ) );
         p.y+=h;   Tv.Text( East,p,"-33"  ); // 55
         p.y+=h;   Tv.Text( East,p,"-80"  ); // 68
         p.y+=h;   Tv.Text( East,p,"-150" ); // 81
         p.y+=h;   Tv.Text( East,p,"-300" ); // 94
         p.y+=h;   Tv.Text( East,p," ..." ); // 104
  pattern(     EMPTY_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(  WIDE_DOT_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern( CLOSE_DOT_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(     HATCH_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(    XHATCH_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(   LTSLASH_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern( FL.pattern,FL.color ); rectangle( 5,34,31, ++y ); //114
  p.y+=h*3;                                                  //136
  Hg=50.0;                                                   //
  for( Ms=8.0; Ms>5.9; Ms-=0.5 )
  { p.x=16; Chart.ItD( G=p ); Item( true ); p.x=40; Tv.Text( East,p,_Fmt( "%3.1lf",Ms ));
    p.y+=h;
  } p.y+=h+h; h+=h/3;                                        //
  Hg=0.0;                                                    // 206
  for( Ms=7.5,It=9.9; It>-0.9; It-- )                        //
  { if( It<0 )It=-0.01; else
    if( It<1 )It+=0.50;
    p.x=15; Chart.ItD( G=p ); Item( true );
    p.x=40; Tv.Text( East,p,_Fmt( It>4?">3":It<0.4?"<0":"%3.1lf",It ) );
    p.y+=h;
    if( It>4 )It=4;
  } Tv.Height( 0 );
}
static char* InfList( int m )
{ static char Msg[50],Blk[]="    * "; int k,Year,Month,Day,Hour,Minute;
  Seismo &L=Lsm[m]; L.T.UnPack( Month,Day,Year ); Hour=L.T.T;
                                                  Minute=(L.T.T-Hour)*60.0;
  k =sprintf( Msg,"%4d.%02d.%02d ",Year,Month,Day );
  k+=sprintf( Msg+k,Hour>0||Minute>0?"%2d\xFC%02d'":Blk,Hour,Minute );
  k+=sprintf( Msg+k," %4.0lf",L.Hg );
  k+=sprintf( Msg+k,L.Ms>0.0?"  %3.1lf":Blk+1,L.Ms );
  k+=sprintf( Msg+k,L.It&&L.It>Mit?" %4.1lf":Blk+1,L.It );
  if( L.Am>0.0 )sprintf( Msg+k," %4.1lf",L.Am );
  L.Item();
  L.Item();
 return Msg;
}
static int Find()
{ Point P=msp; Chart.ItD( P );
  Real Cs,Ds,Dmin; int i,k;
  for( i=k=0; i<Nsm; i++ )
  { Sphere( P,Lsm[i].G,Cs,Ds ); if( !i )Dmin=Ds; else if( Ds<Dmin )Dmin=Ds,k=i;
  } return k;
}
static Course InfoLine( int im )
{
 Course ans=Center; int Year,Month,Day,Hour,Minute;
 Seismo &L=Lsm[ist=im];  L.T.UnPack( Month,Day,Year ); Hour=L.T.T;
                    L.Item();                     Minute=(L.T.T-Hour)*60.0;
  do
  { color( WHITE ); Twindow( 1,-1,72,1 ); //Tgo( 1,0 );
    color( CYAN  ); Tprintf( "<%d> ",ist+1 );
    color( BLACK ); Tprintf( "%4d-%s-%d",Year,W2D( _Month[Month-1] ),Day );
    if( Minute>0 || Hour>0 )
    { color( CYAN ); Tprintf( " %02d\xFC%02d'",Hour,Minute );
    } color( BLUE ); Tprintf( " Depth=%1.0f,Ms=%3.1f",L.Hg,L.Ms );
      color( BROWN );
    if( L.It>Mit )Tprintf( " I=%3.1lf",L.It );
    if( L.Am>0.0 )Tprintf( ",Am=%3.1lf",L.Am );
   Point E=L.G,Q;
   bool mIs =GetMouse();
    if( mIs )
    { Chart.ItD( Q=msp );                // ��� ��� �������� ���������
      Real Ds,Cs; Sphere( Q,E,Cs,Ds );  // �� � ������ ������� ������ ��������
      setwritemode( XOR_PUT ); color( LIGHTMAGENTA^WHITE ); Chart.sphere( Q,E,false );
      setwritemode( COPY_PUT );
      Tv.Height( 8 ); Tprintf( " {%1.0lf\xF8,%3.1lfkm}",Cs,Ds ); Tv.Height( 0 );
    } Chart.DtI( E ); Tprintf( " \n" );
    ans = Tv_pause( &E,12,1 );
    if( ans==_Enter || ans==_Ins )
      { if( (ans=Tmp.Edit( ans==_Enter ))==_Esc )ans=_MouseMoved;
      }
    if( mIs )
    { setwritemode( XOR_PUT ); color( LIGHTMAGENTA^WHITE ); Chart.sphere( Q,L.G,false );
      setwritemode( COPY_PUT );
    }
    if( (ans|0x20)=='h' )ans=Course( 0xFF ); // ��� ������ �� ����������
    Tback();                                 //
  } while( ans==_MouseMoved || ans==_MouseReleased ); L.Item(); return ans;
}
Course Seismo::Edit( bool eLsm )
{ if( eLsm )*this=Lsm[ist]; else // �������� � ���� ���� ��, ��� ����
  { T.Now();
    if( GetMouse() )G=msp; else
      { G.x=2*Chart.Nx/3;        // ��� �����
        G.y = Chart.Ny/3;        //   - � �������� ����
      } Chart.ItD( G );          // ������� � ���������
        Ms=7.0;                  // ���������
        Hg=0.0;                  // ������� ����� ������������� � ��
        It=Mit;                  // ������������� ������ �� ���������
        Am=0.0;                  // ������������ ������ ������ �� ���������
  }
 Course ans;
 int Year,Month,Day; Real Time=T.T;
 Mlist Menu[]=
  { { 1,0,eLsm?"\t ���������� ������:"
              :"\t ���� ������ �������:" }
  , { 2,5,"%5d",&Year },{ 0,2,"-%02d",&Month },{ 0,2,"-%02d",&Day },{ 0,11,",%3�",&Time }
  , { 1,11,"\t ������ :   %3�",&G.y }
  , { 1,11,"\t ������� :  %3�",&G.x }
  , { 1,4, "\t ��������� :    %4.1lf",&Ms }
  , { 1,4, "\t ������� �����: %4.0lf",&Hg },{ 0,0,"\t ��" }
  , { 1,4, "\t �������������: %4.1lf",&It },{ 0,0,"\t ��" }
  , { 1,4, "\t ������.������: %4.1lf",&Am },{ 0,0,"\t �" } };
  T.UnPack( Month,Day,Year );
  do
  { ans=(Course)Tmenu( Mlist( Menu ),-39,1 );
    if( ans<sizeof( Menu )/sizeof( Mlist ) ) // ������������� ������ �����
    { T.Pack( Month,Day,Year );              // � ���������� � ������������
      T.T = Time;                            // ����� ������, � ��������
      if( Ms<0 )Ms=-Ms;                      // � ����� ������
      if( Ms>9.9 )Ms=9.9;                    // � ���������� �� ������
      if( eLsm )Lsm[ist]=*this; else         //
      { FILE *F=fopen( FileName,"a" );       // ������ ������ � �������
        if( !F )F=fopen( Start_File,"a" );   // ��� � ���������� ����� ����
        if( !F )Tv_bell(); else              // � fseek �������� ������� ftell
        { ist=Nsm; fseek( F,0,SEEK_END );
          if( !ftell( F ) )fputs( Title,F ); PutString( F ); fclose( F );
        } (Lsm=(Seismo*)Allocate( (Nsm+1)*sizeof(Seismo),Lsm ))[Nsm++]=*this;
      }
      Item( true );                         // ������� ����� ����� �� ������ �
      ans=_Enter;                           // �������� �� ����� � ������������
    }
  } while( ans!=_Esc && ans!=_Enter ); Ghelp(); return ans;
}
//      ������� ���� � ������� ���������
//

int Depth::Seismic_List()
{
 const char *Name[]={ "List "," ������� ������������     ",
                              "������������� � ������",0 },
             *Text[]={ "F1 ","������� � �������� �������",
                       "F2 ","���������� ��������� �������",
                       "F3 ","���������� ������� ��������",
                       "F4 ","��������� ���������� �������",
                       "F5 ","���������� ������� �� �����",
                       "F6 ","���������� �������� ������",
                       "F7 ","����� ������ �� ����� ���",0 },
             *Plus[]={ "<H>elp","  ��� ���������",
                       "<Enter>"," ������� ��������� �����",
                       "<Insert>"," ��������� ������ �����",
                       "<mPress>"," ����� � ������� ������",
                       "<_2011.>"," ����� ������ �� ������",
                       "<������>"," ������ ���������",0 };
 int ans,inf=0,ha,lst=0,mouse=Tv_Start_Mouse();
                        color( LIGHTGRAY ); //,CYAN );
                        getfillsettings( &FL );
                        getaspectratio( &ans,&ha ); Yasp=Real( ans )/ha; ha=0;
  for( ans=(Nsm?South_West:0xF3); ; )
  { if( Nsm>0 && ans<_Enter )
    switch( ans )
    { case North_West: ist=0;     break;
      case South_West: ist=Nsm-1; break;
     default:
      if( ans&North_East ){ if( ++ist>=Nsm )ist=0; } else
      if( ans&South_West )  if( --ist<0 )ist=Nsm-1;
    } else
    { switch( ans )
      { case 0xF3: Open(); if( !Nsm )goto Leave; ist=Nsm-1; break;
        case 0xF1: if( inf^=1 )Tmp.Inform(); else Tback();
        case ' ' : break;
        case _Esc: goto Leave;
       default:
        if( Nsm>0 )switch( ans )
        { case _Enter: // ��������� �� ����� �� ���������
                       // Lsm[ist].Item( true );
                       // Lsm[ist].Edit( ist );
                                break;
          case 0xF2: Save();    break;
          case 0xF4: Select();  break;
          case 0xF5: Draw();    break;
          case 0xF6: Sorting(); //++ans;
          case 0xF7: break;
          case _Del: for( int i=ist; i<Nsm-1; i++ )Lsm[i]=Lsm[i+1]; --Nsm;
                     if( Nsm ){ if( ist>=Nsm )ist=Nsm-1; break; }
                     Lsm=(Seismo*)Allocate( 0,Lsm ); goto Leave;
          case _MousePressed: ist=Find(); break;
         default: ans='h';
        } else    ans='h';
      }            ha=0;
    }
    if( ans==' ' ){ Thelp(""); ha=0; } else
    if( !ha ){ ha=1; Thelp( "{%d} F1-info%s F3-open%s",Nsm,
           Nsm>0?" F2-save":" <h>elp",
           Nsm>0?" F4-select F5-draw F6-sort F7-list <Ins/Enter>edit <Del>":"" );
    }
    if( lst || (Nsm>0 && ans==0xF7) )
    { static field f={ 0,4,1,26,2 };
      if( Tv.mY==479  )f.Ly=40; else
      if( Tv.mY==767  )f.Ly=36; else
      if( Tv.mY==1023 )f.Ly=50;
                     Twindow( 0,2,39,1,2 );
      color( BLUE ); Tprintf( " Year.mm.dd  Time  Depth  Ms  Its  Amp" );
      color( DARKGRAY ); //,CYAN );
          ans = Tcase( f,37,InfList,InfoLine,Nsm,ist ); Tback(); lst=1;
      if( ans==_Esc )ans=lst=0;
    } else
    if( (ans|0x20)=='h' )ans=Help( Name,Text,Plus );
                    else ans=InfoLine( ist );
  }
Leave: if( inf&1 )Tback(); pattern( FL.pattern,FL.color );
       if( mouse!=MouseInstalled )Tv_Stop_Mouse(); return Nsm>0?ist:-1;
}
int Depth::Seismic_List       // ���������� ������ �� �����
(   Point& GeoK,              // �������������� ����������
    Event& JulD,              // ��������� ���� � ����� �� ������ ����� � �����
    Real& MagS,              // ���������
    Real& HeiG               // ������� ����� ������������� � km
){ int its=Seismic_List();
   if( its>=0 ){ Seismo &L=Lsm[its]; GeoK=L.G;
                                     JulD=L.T;
                                     HeiG=L.Hg;
                                     MagS=L.Ms; } return its;
}
int Depth::Seismic_List( Point& GeoK )
{ int its=Seismic_List(); if( its>=0 )GeoK=Lsm[its].G; return its;
}
