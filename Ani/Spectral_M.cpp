//
//  Spectral_M.cpp                               Ver 3.2  90.12.23 - 2015.04.12
//
//  ������ ������� ���������� ������� - ��������� �������
//
#include "Spectral.h"
//
//  ���������� �������� � ��������� ���� � ��� ��� ������� "Vector"
//
static Vector            // �������-��� �� ����������� ���������
   XX( "*.*  First File" ),
   YY( "*.* Second File" ),
   SX( "Spectrum ONE"     ,true            ),
   SY( "Spectrum TWO"     ,true            ), SS( "Spectrum ALL",true ),
  CXY( "Co-Spectr"        ,true            ),
  QXY( "Quadr-Spectr"     ,true            ),
  RXY( "Coherence"        ,true,       0,1 ),
  HXY( "Transfer Function",true            ),
  FXY( "Phase angle"      ,true,-M_PI,M_PI ),
  *Lv[3];             // ++ ������ ���������� ������ (����)
static int Ng=0;      // � �� ���������� �� ����� �������
//
//  ���������� ������ ������ ������������ ������
//
static Bfont S( _Small_font,12 );
static Field F={ 0,0,1,1 };   //
static bool FtL=false;        // �������� ������� �� ����� �����
static int  kWin,nWin;        //  ��������� � �������� ����� �� ��������
static Real Fcut=16.0,        // ��������� ������� ������
              dT=1.0,         // ������ ��� �� ������� ��� �����������
         Vd=0,Vu=0,Cx=1.0;    // ���������� ����������� � ��� �� �������
//       Yf=0,Yk=1,Xf=0,Xk=1; //
                              //
#define _FF(A) ( !Spec ? dT*(A)/Cx : ( !FtL ? dT/(A):2*dT*(1.0+(A)/Cx/Fcut) ) )
                                     //
void Vector::Axis_Series( Real dY )  // ���������� �������� ������������ ����
{ Real  Y,X,u,v,w;                   //
  point p; color( CYAN );
  Point P; Tv_rect();
           P.y=Tv_Y( Tv_port.bottom+3 );
           Y=F.Ly+F.Jy;
           X=F.Lx+F.Jx; // - �����
  if( !Spec )
  { u=Tv_step( (v=_FF( F.Lx ))/60.0 ); w=60.0*F.Lx*u/v; v=w*floor( F.Jx/w+1 );
  } else
  { if( !FtL ){ w=Tv_step( F.Lx ); v=w*floor( F.Jx/w+1 ); } else
    { u=Tv_step( (v=( _FF( F.Lx )-2*dT ))/2 ); w=F.Lx*u/v; v=-Cx*Fcut;
  } }
  for( P.x=v; P.x<X; P.x+=w ) //  axis-X
  if( P.x>F.Jx )
  { color( LIGHTGRAY ); line( P.x,F.Jy,P.x,Y );
    if( P.x>F.Jx+w/3 && P.x<X-w/3 )
    { color( BLUE ); S.Text( South,P,_Fmt( "%s",DtoA( _FF( P.x )/60.0 ) ) );
  } }
  //                 ������� ��������� �������� �������
  color( RED );                                                P.x=F.Jx;
  S.Text( South,     P,_Fmt( "%s",DtoA( _FF( P.x )/60.0 ) ) ); P.x=X;
  S.Text( South_West,P,_Fmt( "%s",DtoA( _FF( P.x )/60.0 ) ) );
  P.x=Tv_X( Tv_port.left-1 );
  for( P.y=dY*floor( F.Jy/dY+1 ); P.y<Y; P.y+=dY )               //  axis-Y
  { color( LIGHTGRAY ); line( F.Jx,P.y,X,P.y ); p=P;
    color( BLUE );    S.Text( West,p.x-6,p.y,_Fmt( "%0.3lg",P.y ) );
} }
#undef _FF
static void Interval()
          { kWin=(int)ceil( F.Jx/Cx ); nWin=(int)floor( (F.Jx+F.Lx)/Cx ); }
                                //
void Vector::Extrem()           // Ng - ������������ �� ������ �����������
{ Real Mn=Min,Mx=Max;           // ����� ������������ ������
   if( !Ng )Vd=Vu=0;            // ����������� ��� ���������� ��������
   if( Spec )Cx=0.5/( N-1 );    //
        else Cx=1.0/N;          F.Jx=Start*Cx;
   if( Spec && !FtL )F.Jx+=Cx;  F.Lx=End*Cx-F.Jx;  Interval();
   if( Mx<=Mn ){ Vd=Vu=Mx=Mn=_V[kWin++];
                 for( ; kWin<=nWin && kWin<N; kWin++ )
                 { if( Vu<_V[kWin] )Vu=_V[kWin];
                   if( Vd>_V[kWin] )Vd=_V[kWin];
                 } if( Vu<=Vd  )Vu=Vd+1e-6;
               }   if( Vd>Mn )Vd=Mn;
                   if( Vu<Mx )Vu=Mx;
}
void Vector::View()
{ if( !N ) return;
 linesettingstype lv;
 point p,q;
 int k;
  Extrem(); Lv[0]=this; ++Ng;
  { Field Wf;
    field wf;
    Real D=Tv_step( Vu-Vd ); F.Jy=D*floor( Vd/D );
       color( CYAN );        F.Ly=D*ceil ( Vu/D )-F.Jy;
    wf=f; Tv_place( &wf );
    Wf=F; Tv_place( 0,&Wf );
    bar( 0,Tv_port.top-Ng*10,Tv_port.right,Tv_port.bottom+16 );
    Axis_Series( D );
  } p.x=q.x=Tv_port.right; getlinesettings( &lv );
    p.y=q.y=Tv_port.top+5;
  if( Ng>1 )p.x-=90;
  for( k=0; k<Ng; k++ )
  { q.y = p.y -= 10; //S.Th-2;
    setlinestyle( lv.linestyle,lv.upattern,lv.thickness ); color( BLUE );
    S.Text( West,p,_Fmt( "%s [%d] ",Lv[k]->IdenS,Lv[k]->N ) );
    color( GREEN );
    if( k==1 ){ setlinestyle( DOTTED_LINE,0,lv.thickness ); color( BLUE ); }
    if( k==2 ){ setlinestyle( SOLID_LINE,0,lv.thickness+1 ); color( LIGHTRED ); }
    { int L,Ly,y; Interval(); L=min( nWin+1,N )-kWin;
      if( L>0 )
      { int Pl[360],*pl; Real *w=*Lv[k];
        while( L>1 )
        { Ly=L<180?L:180;
          for( pl=Pl,y=0; y<Ly; y++ )
          { Point P; P.y=w[kWin]; P.x=Cx*kWin;
            point p; p=P;
             *pl=p.x; pl++;
             *pl=p.y; pl++;     kWin++; L--;
          } drawpoly( Ly,Pl );  L++; kWin--;
      } }
    } if( Ng>1 )line( p,q );
  }   setlinestyle( lv.linestyle,lv.upattern,lv.thickness ); Ng=0;
}
void Vector::Show(){ if( !N )return;
 static int Len,ans=1;
        int c=getcolor();
 extern void Move( field& f );
 Mlist Menu[]={ { 0,4,"Start  %4d", &Start  } // ��������� �����
              , { 1,4,"Length %4d", &Len    } // ����� ����
              , { 1,4,"  Step  %4.1lf",&dT  } // ��� ���������� ����
              , { 1,4,"Maximum %4.1lf",&Max } //
              , { 1,4,"Minimum %4.1lf",&Min } }; //dTs=dts;
  do
  { Len=End-Start+1;
    View();
    if( ans==0xF8 )Tv_getc();
    Ghelp( "F3 I/O  F8 Delay  F9 Clear  F10 Place  \"%s\"",IdenS );
    ans=Tmenu( Mlist( Menu ),1,-1,ans ); Ghelp();
    switch( ans )
    { case 0xF3: Get();   break;
      case 0xF9: clear(); break;
      case 0xFa: Move(f); break;
    }
    if( Start<0 )Start=0;
    if( Len<=0  )Len=N-Start; End=Start+Len-1;
    if( End>=N  )End=N-1;
    if( Start>=End ){ if( Start==0 )End=Start+1; else Start=End-1; }
  } while( ans!=_Esc ); color( c ); //dts=dTs; // Mode=0;
}
//      �===========================================�
//      � ������ ��������, �������� ��������,       �
//      � �������������, ������� ��� � ������������ �
//      � ������� ��� �������� ������������ �����   �
//      � �� �������� �� N ����� �� ������� �� N/2. �
//      � �������������� ������������ 2 ��������    �
//      +-------------------------------------------+
//      �       ������������ ��������� FFT � ����-6 �
//      +--------------------------------------------
                                //
void FFT( int,Real*,Real* );    // ������� �������������� �����
void KIZERS( int,Real* );       // ������ �������-�������
void TRIANGLE( int,Real* );     //      � �����������
                                //
static int   Nff,Nsc,N8=3,MM,   // ����� ���� � ������-�������
             P0=0,PL=0,         // ������������ �������
//           Fmt=0,             // 1 - ������; 2 - ����� ������ ������
             Y,X;               // ���������� �����
static char Name[MAX_PATH]="",  // ��� Mario �����
            Iden[MAX_PATH];     //       � �������������
static int   NW=0,              // ���������� ���������� � ����� Mario
             M1=2,M2=1;         // ������ Mario �����
static fixed IKA=0;             // ���������: 1-�������; 2-������������
static bool  Auto=false;        // ������� �������������� ������
static FILE *Ms;                //
static void Axises()            // ��������������� �����������
{ if( PL>=MM )PL=MM-1;          //
  if( PL<=0  )PL=0;
  if( P0>=PL || P0<0 )P0=0;
  SY.Axis( P0,PL ); HXY.Axis( P0,PL ); FXY.Axis( P0,PL );
  SX.Axis( P0,PL ); RXY.Axis( P0,PL ); CXY.Axis( P0,PL );
  SS.Axis( P0,PL ); QXY.Axis( P0,PL );
}
static void Scaling()
{  XX.Out( 53,40 );   YY.Out(  6,40 );
   SX.Out( 53,40 );   SY.Out( 53,40 ); SS.Out( 53,40 );
  CXY.Out(  6,40 );  QXY.Out(  6,40 );
  RXY.Out(  5,27 );  HXY.Out( 37,27 ); FXY.Out( 69,27 ); Axises();
}
static void Pause( int& ans )
{ Ghelp( "Mario[ %d ]",M2 );
  if( ans ){ Tv_bell(); if( Tv_getc()==_Enter )ans=0; } else ans=Tv_getw();
}
Vector& Vector::Plus( Vector& Pl )           // ���������� ������ ���������
{ if( !Ng )Lv[0]=this; Pl.Extrem(); ++Ng; Lv[Ng]=&Pl; return Pl;
}
static void Drawing()
{ int i,ans=0;
  do
  { clear(); Ng=0; SX.Plus( SY );
    if( Auto )
    { for( i=0; i<SS.N; i++ )SS[i]/=Real( M2 ); SX.Plus( SS );
    }                                           SX.View();
    if( Auto )for( i=0; i<SS.N; i++ )SS[i]*=Real( M2 );
    Ng=0; CXY.Plus( QXY );     CXY.View();
    if( !Auto )Tv_getc(); else Pause( ans );
    if( X&Y ){ if( !Auto || ans ){ clear(); RXY.View();
                                            HXY.View();
                                            FXY.View(); }
      if( !Auto )Tv_getc(); else Pause( ans );
  } } while( ans );              // �������� Mario �����
}                                // NW <- ���������� ����������
static void Open_Mario()         // dT <- ������������ ���������
{                                // Id <- ������ ��� ������ �����������
  if( ( Ms=fopen( Name,"rb" ) )==0 )Break( "? \"%s\" not open",Name );
  fread( Iden,4,1,Ms );                                       //
  if( !strncmp( "Mar\xFA",Iden,4 ) )NW=getw( Ms ); else       // ����������
  { fread( Iden+4,2,1,Ms );                                   //  ����� � �����
    if( !strncmp( "Mario\xFA",Iden,6 ) )NW=(fixed)get2( Ms ); //
      else Break( "? \"%s\" wrong file",Name );
  }
  fread( Iden,1,32,Ms );         // ������������� � ������ ����� ������
  fread( &dT,1,8,Ms ); dT*=60.0; // ������������ �� ������� - � ������
  fread( Iden+40,1,20,Ms );      // ����������, ������� � �������� ������
}                                // == 8 + 60 ���� �� ���������
static void Read_Mario()         // ���� ���������� ��������� ����� ����������
{                                //
 static unsigned L=0; int l,k;
  if( L<10 )L=Tfile( Name );
  if( L<10 )Break( "?\nFile \"%s\"[%d] is bad",Name,L );
ReRead:
  Open_Mario(); k=M2;
  if( !Auto )
  { if( _ac_>1 )
    { k=(NW+1)/2; XX.allocate( (L-8)/NW/4-15 );
                  YY.allocate( (L-8)/NW/4-15 );
    }
    sprintf( Iden,"{ 1..%d } by %lg min. Number of series? (%d)->",NW,dT,M1 );
    Y=X=true;           Ghelp( Iden );
    l=strlen( Iden )+1; color( BLUE );
    do
    {     Tgo( l,0 );               sprintf( Iden,"%d",k );
      if( Tgets( Iden,-10 )==_Enter )sscanf( Iden,"%d",&k );
    } while( k<1 || k>NW );          Ghelp();
  }
  else Ghelp( "Read File: %02d",k );
  for( l=1; l<NW; l++ )fread( Iden,60,1,Ms );
                       fseek( Ms,(k-1)*4L,SEEK_CUR );
 Real *Adr;                       // ��������� ����� ������������
  if( _ac_>1 )Adr=(Real*)XX;      //  ��� ���������� ������ �� ����
      else    Adr=(Real*)YY;      //   �������� �������� �����
 float w;                         // ������� ������
 int lp=( NW-1 )*4L;              //  � �������� ����� �������
  for( l=0; l<XX.N; l++ ){ fread( &w,4,1,Ms ); *Adr=w; ++Adr;
                           fseek( Ms,lp,SEEK_CUR );
                         } fclose( Ms );
  if( _ac_>1 ){ _ac_=0; M1=k; XX.View(); goto ReRead; }
                        M2=k; YY.View();
}
static void Write_Mario()  //
{                          // ������ �������� � ������� �����
 int j,k,l;                // ������ ���������: Auto<=True; M2<=1.
 double w,dS;              //
 const int NWf=4;          // ��� ������ ����� ����������� ��� �����,
 static FILE *WF[NWf];     // ���������� ������� � ���������� ����������������
  if( !Auto )              // ������� �� ������ ����� �2, ���������������� SY
  { const char *MName[NWf]={ "~SYY.mar","~HXY.mar","~RXY.mar","~FXY.mar" };
    Open_Mario();
    if( FtL )dS=dT/Cx/Fcut/(MM+1)/60.0;         // !!! ��� ����� ����� ---
        else dS=0.5/dT/(MM+1);                  // ??? - � �������
//  if( Fmt<2 )dS*=60;
    for( k=0; k<NWf; k++ )
    { if( (WF[k]=fopen( MName[k],"wb" ))==0 )Break( "\n?%s not open",MName[k] );
      fwrite( "Mar\xFA",4,1,WF[k] ); putw( NW,WF[k] );
    }
    if( !FtL )w=0.0; else w=dT/30.0;
    for( M2=1;; )
    { for( k=0; k<NWf; k++ )
      { fwrite( Iden,1,24,   WF[k] ); // ������������� � ��������� ����
        fwrite( &w, 1,8,     WF[k] ); // ����������� ������ �� �������������
        fwrite( &dS,1,8,     WF[k] ); // ������������ - � �������
        fwrite( Iden+40,1,20,WF[k] ); // ����������, ������� � �������� ������
      } M2++;
      if( M2>NW )break;
      fread( Iden,1,60,Ms );
    } fclose( Ms );
           w=0;
    for(   k=0; k<NWf; k++ )
    { for( j=0; j<NW;  j++ )
      for( l=0; l<=SY.End; l++ )fwrite( &w,4,1,WF[k] );
    } Auto=true; M2=1;
  } else
  { Ghelp( "Write to File: %02d",M2 ); Real *V[4]={ SY,HXY,RXY,FXY };
    for( k=0; k<NWf; k++ )
    { fseek( WF[k],NW*60+(M2+1)*4L,SEEK_SET ); j=( NW-1 )*4;
      for( l=0;; )
      { fwrite( (Real*)V[k]+l,4,1,WF[k] ); l++; if( l>SY.End )break;
        fseek( WF[k],j,SEEK_CUR );
    } }
    if( M2>=NW )
    { for( k=0; k<NWf; k++ )fclose( WF[k] ); Auto=false; } else M2++;
} }
//     ------------------------------+
//     � ������ ������� � ���������  �
//     +------------------------------
static void MiDsp( int Ln, Real* A, Real& Arg )
     { int L=Ln; double R=0; while( --L>=0 )R+=A[L]; Arg=R/Ln;
     }
static int exp2( int p ){ return 1<<p; }
static int log2( int Ln )
     { int Lk=0x8000,l=15; while( (Lk&Ln)==0 ){ l--; Lk>>=1; } return l;
     }
static void Segmentation()              // ������������ ��� ������
{                                       // ��������� ��� ��������� ��������
// static char Kz;                      // � ������������� ������
 static int Nf,Pw; int ans=0;
 static Mlist Menu[]
   = { { 0,1,"START: " }
     , { 0,8 }                          // Clear - Kizer-Bessel - Triangle
     , { 0,8 }                          // Freq - Time
     , { 0,2,", ( 2^%d",&Pw },{ 0,4,"= %d",&Nsc }
     , { 0,2,  ") x %d",&N8 },{ 0,4,"=> %d",&Nf },{ 0,1," New Input" }
     };
 Display T( Mlist( Menu ),1,1 );
  for(;;)
   { Menu[1].Msg=(char*)(IKA>1?"Triangle" : IKA?"  Kizer ":"  Clear " );
     Menu[2].Msg=(char*)(!FtL?"  Frequency "         //
                      : (Fcut<1.5? " Full Period"    // �� ������ �������
                      : (Fcut<3.0? " Length - 2 "    // ������� ����������
                      : (Fcut<7.0? " Length - 3 "    //
                      : (Fcut<15.0?" Length - 4 ": " Length - 5 " )))));
     if( Nsc<=0 || Nsc>Nff )Nsc=Nff;
     Pw=log2( Nsc );                    // ������� �������������� FFT
     Nsc=exp2( Pw );                    // ����� ���� FFT
     MM=Nsc/2;                          // ����� ���� ��� ��������� ����� ����
     N8=Nff/MM;                         // ���������� ��������� ����������
     Nf=N8*MM; --N8;                    // ��������� ����� ����
     if( ans==-1 )break;                // ����� ����� ������������
         ans=T.Answer();
     if( ans==0 )ans=-1;
     if( ans==1 ){ ++IKA; if( IKA>2 )IKA=0; }
     if( ans==2 ){ if( !FtL ){ FtL=true; Fcut=16.0; } else
                   { if( Fcut>1.5 )Fcut/=2.0; else FtL=false;
                 } }
     if( ans==3 )Nsc=exp2( max( Pw,2 ) );
     if( ans==4 ){ if( Nsc<4 )Nsc=4; }
     if( ans==5 )Nsc=Nff*2/++N8;
     if( ans==6 )Nsc= Nf*2/++N8;
     if( ans==7 ){ N8=0; return; }              //
   } SX.allocate( MM+1 );  SY.allocate( MM+1 ); // "+1"- ��� ��� ��� �����
    CXY.allocate( MM+1 ); QXY.allocate( MM+1 ); //       ��� X(0)=0.0
  if( X&&Y )                                    //
   { SS.allocate( MM+1 ); RXY.allocate( MM+1 );
                          FXY.allocate( MM+1 );
                          HXY.allocate( MM+1 );
   }
}
#define FreqLen( S )                                                       \
   for( k=0; k<=MM; k++ )AA[k]=S( double( MM+1 )/(double( k )/Fcut+1.0) ); \
   for( k=0; k<=MM; k++ )S[k]=AA[k];
//                                            W=S._V; S._V=AA; AA=W;
int main()
{ int k,l; Real RX,RY,delt=0,w;
  Tv_Graphics_Start();
  setwindowtitle( "SpecrM - ���������� ��������������� ������������ Ani" );
  pattern( 92 );  // 92 = ������-����������
  bkcolor( WHITE ); if( _ac_>1 )strcpy( Name,_av_[1] );
  Scaling();
  clear();
 Begin: color( CYAN ); // ������ � ������ ��������� ANI
  if( _ac_>1 || !_ac_ )Read_Mario(); else
  do{ XX.Get(); X=XX.N>0;
      YY.Get(); Y=YY.N>0; if( Y && !X )XX.allocate( YY.N );
                          if( X && !Y )YY.allocate( XX.N );
  } while( !Y && !X );
  if( Auto )Ghelp( "Calculating: %02d",M2 );
      else { Nff=min( XX.N,YY.N ); Nsc=Nff*2/++N8; }
 Repeat:
  if( !Auto ){ Segmentation(); if( N8<=0 )goto Begin; delt=(Real)Nsc*2.0/N8; }
  MiDsp( Nff,XX,RX );
  MiDsp( Nff,YY,RY );      Tgo( 0,-4 );
  Tprintf( "RX=%.2lf",RX ); Tln();
  Tprintf( "RY=%.2lf",RY ); Tln();
  Tprintf( "AL=%d",N8*2 );
  { //   -------------------------�
    //   � ������ ������� ������� �
    //   +-------------------------
   Series AA(Nsc+1),X(Nsc),Y(Nsc);
   double R,A, R1,R2,A1,A2,R21,A21;   //
   int   K;                           // ����������� ������������� ���� �������
    if( IKA==1 )KIZERS( Nsc+1,AA );   else // ���� �������,
    if( IKA==2 )TRIANGLE( Nsc+1,AA ); else // ����������� ���
    { for( k=0; k<=Nsc; k++ )AA[k]=1.0;    // ������������ ������� ������� 1.0
      //   AA[0]=AA[Nsc]=0.714;            // � �������� �� ������
    }                                      //
    for( k=0; k<=MM; k++)SX[k]=SY[k]=CXY[k]=QXY[k]=0.0;
    for( K=0; K<N8; K++ )
    { for( l=MM*K,k=0; k<Nsc; k++ ){ X[k]=(XX[k+l]-RX)*AA[k];
                                     Y[k]=(YY[k+l]-RY)*AA[k]; }
      FFT( Nsc,X,Y );
      for( k=1; k<=MM; k++ )
      { R1=X[k]; R2=X[Nsc-k]; R=R1+R2; R21=R2-R1;
        A1=Y[k]; A2=Y[Nsc-k]; A=A1+A2; A21=A2-A1;
        //   --------------�
        //   � ����������� �
        //   +--------------
        SX[k]+=sqrt( k*(R*R+A21*A21)/MM )/2; // ���� +=(R*R+A21*A21)/4;
        SY[k]+=sqrt( k*(A*A+R21*R21)/MM )/2; //      +=(R21*R21+A*A)/4;
        //   -------------------�
        //   � �������� ������� �
        //   +-------------------
        CXY[k]+=(R2*A1+R1*A2)/2;
        QXY[k]+=(R21*R+A21*A)/4;
      } //   -----------------------------�
        //   � ������� ����� ������������ �
        //   +-----------------------------
      R=X[0]; SX[0]+=fabs( R ); //+=R*R;
      A=Y[0]; SY[0]+=fabs( A ); //  A*A;
        //   ----------------------------------�
        //   � ������� ����� �������� �������� �
        //   +----------------------------------
      CXY[0]+=R*A;
      QXY[0]=0.0;               // ??
  } }
//
//   ��� ������� ���������, ����� ������ ��������
//
  for( k=0; k<=MM; k++ )        //
  {  SX[k]*=delt;  SY[k]*=delt; // ���������� ������������
    CXY[k]*=delt; QXY[k]*=delt; // ���������� �������� ��������
  }                             //
  if( FtL )
  { static Real *AA=NULL; static int m=0;
    if( m!=MM+1 )AA=(Real*)Allocate( (m=MM+1)*sizeof( Real ),AA );
    FreqLen( SX ) FreqLen( CXY )
    FreqLen( SY ) FreqLen( QXY )
  }
  if( X&&Y )
  { for( k=0; k<=MM; k++ )
    { w=hypot( CXY[k],QXY[k] );    //
      HXY[k]=  w/SX[k];            // ������������ �������
      RXY[k]=w*w/SX[k]/SY[k];      // �������������
      FXY[k]=atan2(QXY[k],CXY[k]); // ������� �����
//    if( !Auto )                  //
//    { if( k==0 ){ if( FXY[k]<0.0          ) FXY[k]+=M_PI*2; }
//          else  { if( FXY[k-1]-FXY[k]>=4.8) FXY[k]+=M_PI*2; }
//    }
    }
//  for( k=1; k<=MM; k++ )
//  { w=(FXY[k]-FXY[k-1])/M_PI/2; w+= w>0.0?0.5:-0.5;
//    FXY[k]-=floor( w )*M_PI*2;
//  }
  }
  if( Auto )for( k=0; k<=MM; k++ )SS[k]+=SY[k];

  Drawing();
  if( Auto )Write_Mario();        // ����� ������� ���������������� ����������
  if( Auto )goto Begin;           //  � ������� � ���������� �����
  else{
   static int ans=0;
    Mlist Menu[]={ { 0,17,(char*) XX.IdenS },{ 1,17,(char*) YY.IdenS }
                 , { 1,17,(char*) SX.IdenS },{ 1,17,(char*) SY.IdenS }
                 , { 1,17,(char*)CXY.IdenS },{ 1,17,(char*)QXY.IdenS }
                 , { 1,17,(char*)RXY.IdenS },{ 1,17,(char*)HXY.IdenS }
                 , { 1,17,(char*)FXY.IdenS } };
    Mlist Outm[]={ { 0,4,"From %d",&P0 },{ 0,5," by %4.2lf",&dT }
                 , { 0,5, " to %d",&PL },{ 0,0,Iden } };
    sprintf( Iden,"at %d points",SX.N );
    do
    { Ghelp( "F1 Begin  F2 Repeat  F3 Segment  F4 Scale  F5 Draw %s F10 Clear",
              _ac_?"":" F6 Auto-Write " );
      ans=Tmenu( Menu,X&&Y?9:6,0,-1,ans ); Ghelp();
      switch( ans )
      { case    0:  XX.Show(); break;
        case    1:  YY.Show(); break;
        case    2:  SX.Show(); break;
        case    3:  SY.Show(); break;
        case    4: CXY.Show(); break;
        case    5: QXY.Show(); break;
        case    6: RXY.Show(); break;
        case    7: HXY.Show(); break;
        case    8: FXY.Show(); break;
        case 0xF1: goto Begin;
        case 0xF2: goto Repeat;
        case 0xF3: Tmenu( Outm,4,1,0 ); Axises(); Drawing(); break;
        case 0xF4: Scaling();
        case 0xF5: Drawing(); break;
        case 0xF6: if( !_ac_ )    // ������� �������������� �������
           {  for( k=0; k<SS.N; k++ )SS[k]=0.0;
              Write_Mario(); goto Begin;
           } else break;
        case 0xFa: clear(); break;
        case _Esc: Ghelp( "?Are you sure" ); ans=Tv_getc();
          if( ans!=_Enter && (ans|0x20)!='y' )ans=0; else ans=_Esc; break;
       default: Tv_bell();
      }
    } while( ans!=_Esc );
  }   Tv_close();
}
