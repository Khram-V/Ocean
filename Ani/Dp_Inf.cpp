/*
    Dp_Inf.cpp                                        /91.01.18/
                                                      /95.05.28/
   ��������� ������� � ������ ���������� � ������ ������ ����������
  -----------------------------------------------------------------

  ������������� ��������� ������� ��� ���������  ANI ������ ����
  �������� �� ����� �������� ����������, ������������ �� �����������
  ������������� �����, �������� � ������������ ��� � ��������������
  ��������.   �������� ��������������� ������ ������������ �
  �������������� ����� � ����������� (.inf), ������ ��������
  �������� ������ �� �������� �������� ������, ��������������
  � ��������� (.dat), ����-��������� (.int)
  ��� �������� (.byt) ��������.

  ������ ���������� ��������������� ����� ����������� � ������������
  � ������� ������, � ������ ��������� ���������� ���������� �������
  ����������� � ���������� ����� ������������ ������ ������.

     .-----------------------------------.
     �   ������� ��������������� �����   �
     '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
�������� ��� ���������� ��������������� �����
         � ���� ������� ������������
Okhotsk.int
   2                � Type        0:Text; 1:Byte; 2:Integer; 4:Float
   2                � Format      0:Var;  1:Line; 2:Field; 3:Space
 144 256            � Ny Nx       Field's Dimension
  12 16 62 76       � Jy Jx Ly Lx Field's Base & Size
8650 -2957 m        � Max Min     Bathimetry Extrema & Scale
  12 12 ' 50        � Sy Sx ' Fe  Grid's Step, <'dkm> & Lat.of scale
 -18 42�36' 141�55' � Cs Fi�'Lo�' Course, Latitude & Longitude
   5                � Nc        Number of Marigrams (control points)
  11.5 6.2         Korsakov        46�18'  142�53'   64
  59      21       Magadan         58�60'  142�39'   52
  55:50' 162:39'   ���� �������� ; 31 62  ����������� � ����� ������
; 22.3    50.1     Petropavlovsk --       ������ ��������� ���������
  52�50' 159�02'   ������������� ������^����, � ����� ������ 20 ����
  37      16       Okha 53�47' 143�30' 110 ����� ��� �� ������ �����

 1-2  ��� ������ ������������,
      ������� �������� ���������� � ������������ �����������;
  3 - ��� ��������� �����, ��� �������� ���� ������� ����� ����
      �������, ���� ���������� ��������� � ��� �� ����������, ��� �
      �������������� ����, ����� ����� ���� ������� � ���� ��� �����
      ������, ���� ��� ��� ��������� � ������ (.inf).
  4 - ��� �������� ������ � ����� ����������:
      0-���������(.dat); 1-��������(.byt) ��� 2-�����(.int);
  5 - ����� ������������� �������� ������.
      � ��������� �������� ANI ����� ������ ���� ����� 2;
  6 - ����������� (nY,nX) ��� ������ ������� � ����� ����������;
  7 - �������� � ������� �����������  �� ����������  ��������������.
      ������ ��� ����� ���������� ������ ����� ���� � ��������
      ���������� ���������� ������, �� ����� ���� ����� ���� ������
      ������� - ��������� �������, ���� � ��������� �������� ���������
      ���� � ������� �������� ( �:'" ). ������� ������������ �������
      ������ ������ �������� ������� ������ � ������;
  8 - ���������� (Max,Min) ���������� � ������ �������� m,cm ��� dm.
      ���� Max<Min, �� ��� ��������, ��� ������� ������ ��������������
      �������, � ��������� ������ ��������������. � ����� �������
      ������������ �������������� ��������� ����� � ����� �������
      ������.
  9 - ��� ����� dY,dX: � �������� ��� ������� (deg ��� ') ���
      �������������� ��������,  �  ������ ��� ���������� (m ��� km)
      ��� ������������. ���� ������ �����, Dy - ������������, �� ���
      ��������, ��� ������ �������� ������� ������������� ������ ����.
      ������ ������ (m,km,deg,') ���������� ��� ��������,
      ��������������. ���������, 4-� ����� ���������� �����������
      ������ ��� ������������ �����.
 10 - ����������� ����� ������� ���������� ������������ ��������
      ��������� � �������������� ����������  ��� ������ ������� ����
      ������� ���������� (��� �����). ���������� ��������  � �������
      ������� *): +ddd�mm'ss"zz ��� +ddd:mm'ss"zz, ���  ������ ����
      ����� ���� ��������� ������ � ������, � ������� ����� ���
      ������� ��������� �������������� ��� ��������� ��������.
 11 - ���������� ����������� �������, ���������� ������� �������� �
      ��������������� ���������� ��������� �����.
 12+Nc - � ������ ������� ������ �������� ������ ���������� ��������
      �� ��������� ��������� (ky,kx )  ������������  ������  �������
      ���������� � ������ ����� �������� ��� ����� ������.
         ����� ������ �������� ���������� ���� � ����� �� ���������-
      ��� ����� ���������� ������ ��������� ���� ( �:'" ), ����� ��-
      �������� ��������� ���������������,  �  �������� ���������� ��
      ����������� ���� ���� ����� ������ �� �������� 20 ��������.
         ������ ����� � ������� (;) ��������  ���������  ������
      �����������, ������� ����� �������� ����� � � ������ ������.
         ������ ������� ���������  �������  ����������  ������������
      ��� ������ ��������� ANI ��� ��� ������  �����  ����������  ��
      ����������������� ��������� Chart. � ����  ������  ���  ������
      ��������� �� ������� ������������ �������� [Jy+Ly,Jx+Lx],
      ����������� �� ������ � �� �������� � ���������. ��� ���������
      ���������� ��������� ����������� �����  �����  �����,  �������
      ������������ �� 1/3 ������� ������ � �����  ��  ���  ���������
      �����.
         ���� �� ���������� ���������� ������������  �����  ��������
      (Chart)  ���������  �  ������   ���������������   ������������
      (Simulation), �� ����� ������ ���������� �� �����������, � ���
      ��������� � ����������� ������ ����� ����������� ��� ���������
      �� ���������, �� ������ �� ����� �� �� ������� ���������
      �������.

*) � ����������� ��������� ����
  ������ (�) ����� ��������� ���������� (:).

  ��� ������� ��������� ANI ���������� �������������� ���������
  DispCCCP.com ��� ����������� ������� (���������� ������),
  ���� � ������������ ��������������� ����� ���� �����
  � �������� �������.

    D:\Path\>Ani [FileName.[inf]]   ��� ����� �� �����������

  ��������� ����� ���� ��������� ���� ���, ��� ����������� �����
  ����������. ��� �� �������� ������������ ������������ �.�.
  ����� ���� ����� ���� �������� � ������ ��������������.
*/
#include "Depth.h"
                               //
static char Nm[MAX_PATH+4]=""; // ��� ���������� ��������� �����
static bool First=true;        // ������� ������� ������ ������
static FILE *Fi;               // ��� ��������������� ����

void Depth::ItD( Point& W )               // �������� �������� � ���.����������
{ W=Geo.Goext( W ); if( isMercator )W.y=Geography( W.y ); W*=Radian; }
void Depth::D2I( Point& W )               // -- ��� ��������� � �������
{ W/=Radian; if( isMercator )W.y=Merkatory( W.y ); W=Geo.Goint( W ); }
void Depth::D2I( Point Q,int &y,int &x ) // ���������� -> �������
{ DtI( Q ); y=Q.y+0.5,x=Q.x+0.5; }
void Depth::DtI(_Point P,int &y,int &x ) // � ��������� � ��� ����� �� �������
{ D2I( P,y,x ); if( y<0 )y=0; else if( y>=Ny )y=Ny-1;
                if( x<0 )x=0; else if( x>=Nx )x=Nx-1; }
bool Depth::is_Globus()
{ if( (Cs!=0.0 && Cs!=180.0) || fabs( Pd-Nx*Sx )>Sx/2.0 )Active&=~as_Globus;
  else { Active|=as_Globus; Sx=Pd/Real(Nx); } return (Active&as_Globus)!=0; }
void Depth::close(){ if( Fs )fclose( Fs ); Fs=NULL; Nm[0]=0; }
void Depth::remove()
{ if( Fs ){ fclose( Fs ); Fs=NULL; if( Nm[0] )::remove( Nm ); } Nm[0]=0;
}
//
//   �������� ������ ����� ���������� � ��������� ����������
//
char* Depth::open( char* name, const char* ext, const char* mode )
{     close();
      OemToCharBuff( name,Nm,strcut( name )+1 );
  if( strchr( name,'.' )==NULL )fext( Nm,ext );
      strcpy( Str,Nm );
  if( mode[0]=='w' )
  { char exb[4];
    strcpy( exb,ext ); exb[0]='~'; fext( Str,exb );
                               ::remove( Str );
                                 rename( Nm,Str );
  } Fs=fopen( Nm,mode ); return Str;
}
//   ������������ ����� ����� ������
//
void Depth::open( char regime )
{
 static char reg[3]="rt"; reg[0]=regime;
 char *s,*S=reg+2;        reg[1]='b';
  switch( Type ){ case 0: reg[1]='t';
                  case 4: S=(char*)"dat"; break;
                  case 1: S=(char*)"byt"; break;
                  case 2: S=(char*)"int";
                }            //
  s=strcpy( Str,InfName );  // ����� ��������
  if( Name[0]>=' ' )        // ������� ����� �����
  { if( Name[1]!=':' )      //
    if( (s=strrchr( Str,'\\'))!=NULL )s++; else s=Str;
    strcpy( s,Name );
  } fext( strcpy( Name,Str ),S );
//
//      �������� ����� ������
//
  open( Name,S,reg );
}
//
//   ������� ������ ����� �� ������� Str � ������
//
static char *Sa=Str;    // ������� �� ������� (dscan->scan -> ��� �������)
static char *scan()     //
{ char* Z;              // Sa - ����� ������ ��� scan, dscan � ��� �������
  if( Sa[0]==0 )Sa=Str; //
  while( Sa[0]<=' ' )if( Sa[0] )Sa++; else goto EoLn; Z=Sa;
  while( Sa[0]>' ' )Sa++;
                if( Sa[0] ){ Sa[0]=0; Sa++; } else Str[0]=0;
                return Z;
EoLn: Str[0]=0; return Str;
}
//   ������� ����� ������� �� ����� � ����������� ������������
//
static char *dscan_prt;

static int dscan( const char *fmt=0,... )
{ if( !fgets( Str,sizeof( Str ),Fi ) )return Str[0]=0; strcut( Sa=Str );
  if( *dscan_prt )printf( dscan_prt,Str );
//if( First )
//{ static int y=0;
//  if( ++y>=Tv.mY/Tv.Th )y=12; Tgo( 0,y ); Tprintf( "%s    ",Str );
//}
  if( !fmt )return strcut( Str ); else
  { va_list aV;                    va_start( aV,fmt );
    int cnt=vsscanf( Str,fmt,aV ); va_end( aV );
    return cnt;
} }
static bool IsDegree( char* Msg ){ return strpbrk( Msg,"\xF8:'\"" )!=NULL; }

Depth& Depth::read( char* PathName )
//
//     ���� ������ ��������������� �����
//          Name   ��� ����� ������ ( ��� INFO - �������� )
//          Typ    ��� ������: 0-Text; 1-Byte; 2-Integer...
//          Ny,Nx  ������ ����������� �������
//          Jy,Jx  ��������� ������ ������������ ��������
//          Ly,Ly  ����������� ������������ �������� ������
//          Nc     ���������� ������������ ����� � ����� .INF
{
 Point P;
 Real Meashure=0.0;
 int  Jy,Jx=-1,                 // �� �������� ����������� �������
      Ly,Lx,                    // �� ������������ �������������
      i,k=Nmp,nc=0;
 char Mp[11],Inv=0,             // Inv: 0x1 ��������� ����� ����������
     *sy,*sx;                   //      0x2 ������� ���������� �����
     *Str=0;
ReOpen:
  if( !PathName ){ (PathName=InfName)[0]=0; }
       PathName=strcpy( InfName,PathName ); // [0]<=' '?"*":PathName );
//  else
//  if( restore( PathName )>0 )
//  { if( !First )              // ������� ������������ ���������� �����
//    { if( Active&0x8000 )
//      if( Fs ){ fclose( Fs ); Fs=NULL; }
//      Active&=~0x8000;
//    } return *this;
//  }
  if( PathName[0]=='*' )Fi=0; else
  { fext( PathName=strcpy( InfName,PathName ),"inf" ); Fi=fopen( InfName,"rt" );
  }
  if( Fi==NULL )
  { Tfile( InfName,"��� ��������� ���������������� ����� (.inf)" );
    if( InfName[0]=='*' )exit( 0 );
    goto ReOpen;
  }
  dscan_prt=(char*)"\n%s";
  strcpy( InfName, W2D( InfName ) );
  Active&=~1; Name[0]=0;          // ������� ���������� ������
  dscan(); strcpy( Msg[0],Str );  // ��� ������
  dscan(); strcpy( Msg[1],Str );  // ��� ��������� � �������������� �������
  dscan(); strcpy( Name,  Str );  // ��� ����� ������
  dscan( "%d",&Type ); if( Type>4 || Type==3 )Break( "Valid Type 0,1,2 or 4" );
  dscan( "%d",&i );    if( i!=2 )Break( "?Bad File" );
  dscan( "%d%d",&Ny,&Nx ); //
  dscan();                       // ������ ������� ������� ��������� ����
  sy=scan(); Jy=0x7FFFFFFF;      //
  sx=scan(); Ly=atoi( scan() );  // if( Ly<=0 || Ly>Ny )Ly=Ny;
             Lx=atoi( scan() );  // if( Lx<=0 || Lx>Nx )Lx=Nx;
  if( IsDegree( sy ) || IsDegree( sx ) ){ AtoD( sy,P.y ); AtoD( sx,P.x ); }
                                   else { Jy=atoi( sy ); Jx=atoi( sx ); }
  dscan( "%lf%lf%s",&Max,&Min,Mp );
  switch( *Mp|0x20 ){ case 'c': Meashure=0.01; break;
                      case 'd': Meashure=0.10; }
  dscan( "%lf%lf%s%lf",&Sy,&Sx,Mp,&Fe );
  dscan(); AtoD( AtoD( AtoD( Str,Cs ),Base.y ),Base.x );
                       Fe /= Radian;
                       Base/=Radian;
  if(*Mp=='\xF8')*Mp='d'; Ms=*Mp|0x20;
  if( Sy<0.0 ){ Inv|=2; Sy=-Sy; }              // ��� ����� �����-�������
  { Real s=scale(); Sy*=s; Sx*=s; }
          basis();               // ����� ����������� ����� ��� ���� ����������
  if( Jy==0x7FFFFFFF )
  { DtI( P ); Jy=(int)P.y; if( Jy<0 )Jy=0; // �������� ��������� �
              Jx=(int)P.x; if( Jx<0 )Jx=0; // ������� �� ������ �������
  }
#if 0
  if( Jy+Ly>Net.y )Ly=My-Jy; if( Ly<=0 )Jy=0,Ly=My; // ����������
  if( Jx+Lx>Net.x )Lx=Mx-Jx; if( Lx<=0 )Jx=0,Lx=Mx; // ��������� ������
#endif
  if( !Ly )Ly=Ny-Jy;
  if( !Lx )Lx=Nx-Jx;
  if( Jy+Ly>Ny )Ly=Ny-Jy;            // ���������� ���������
  if( Jx+Lx>Nx )Lx=Nx-Jx;            // ������ (������ ������)
//if( Ly<=0 )Jy=0,Ly=My;
//if( Lx<=0 )Jx=0,Lx=Mx;
  if( Max<Min  )Inv|=1;                    // ��������� ����� ��� ����������
  if( Jy || Jx || Ly!=Ny || Lx!=Nx ) // ��� ������ ����� �����������
      Max=Min=0;                           //
  P.y=Jy;                 //
  P.x=Jx; ItD( P );       // ����������� ������ ������ � ������ ��������
     Base.y=P.y/Radian;   //
     Base.x=P.x/Radian; basis();
//
//      ������ ������������ ������ � ������ ���������������
//              �������� ���������������� ��������.
//
  if( !(Active&as_Simula) ) // ������ ���� ��� ���������� �������������
  { dscan( "%d",&nc );
    if( nc>0 )
    { const int np=nc/64+1;             // �������� ����������� ������ < 64 ���
      M=(MARIO*)Allocate( (nc+=(k=Nmp))*sizeof( MARIO ),M );
      for( int kfm=k; kfm<nc && dscan(); ) // ������� ��������� ��� ����
      {     sy=scan();                     // ����� ������� ����������
            sx=scan(); dscan_prt="";       // � �������� ���������� ������
        if( sy[0] && strchr( "+0123456789-.",sy[0] ) )
        if( sx[0] && strchr( "+0123456789-.",sx[0] ) )
        {                              //
         MARIO& Mk=M[k];               // ��������� ��������� �����
          AtoD( sy,Mk.G.y ); ++kfm;    // ��� ����� ������������ �����
          AtoD( sx,Mk.G.x );           //
          if( IsDegree( sy ) || IsDegree( sx ) )DtI( Mk.G ); else
            { Mk.G.y-=Jy;                //
              Mk.G.x-=Jx;                //
            } i=-1;                      // �������� �� ��������� � �������
          if( Mk.G.y>0 && Mk.G.y<Ly-1 )  // ��������� ����������
          if( Mk.G.x>0 && Mk.G.x<Lx-1 )  // (��������� ����� - ��������)
          for( i=0; i<k; i++ )           // � ������ �� ������������
            { DtI( P=M[i].G );           // -- � 30% �� ���� �����.
              if( abs( P-Mk.G )<0.3 )break;
            }        //
          if( i==k ) // !!! ������ ����� ���������� ������ �������� ����� 'k'
          { ItD( Mk.G );                       // <240>+ �����������v���
            while( *Sa!=0 && *Sa<=' ' )Sa++;   // �������� ������ | '�'
            if( !Sa[0] || Sa[0]==';' )Mk.cp=strdup( "�" ); else
            { if( strchr( Sa,';' ) )strchr( Sa,';' )[0]=0;
              if( strcut( Sa )>20 )Sa[20]=0; strcut( Sa );
              Mk.cp=strdup( Sa );               // �� ��������� ������ ������
            } k++;                              // � ��������� � ������� ������
            if( !(k%np) )dscan_prt="\r%s ";     // ����������� ������� �������
      } } } Nmp=k;       dscan_prt="\n%s ";     // �� ��������� �������
      if( Nmp<nc )M=(MARIO*)Allocate( Nmp*sizeof( MARIO ),M );
  } } fclose( Fi );
//
//      ������ ��������� ������� �����
//
      open( 'r' );
  if( Fs==NULL )Break( "\r\n%s not open",Name );
  {
   float *R; Real Wd=0.0;
   int y,ky,
       x,kx=0; if( Inv&1 )Max=-Max,Min=-Min;
      allocate( Ly,Lx );
    R=(float*)Allocate( Type ? Type*Lx : Nx*sizeof( float ) );

    if( Inv&2 )Jy=Ny-Jy-Ly;
    if( !Type )ky=0;
         else  ky=Jy;
    for( y=0; y<Ly && ky<Ny; ky++ )
    { if( Type>0 )
      { fseeko64( Fs,Type*( (long long)Nx*ky+Jx ),SEEK_SET ); kx=0;
        fread( R,Lx,Type,Fs );
      } else
      for( x=0; x<Nx; x++ ){ fscanf( Fs,"%f",R+x ); kx=Jx; }
      if( ky>=Jy )
      { for( x=0; x<Lx && kx<Nx; kx++,x++ )
        { switch( Type )
          { case 0:
            case 4: Wd=R[kx];            break;
            case 2: Wd=((short*)R)[kx];  break;
            case 1: Wd=((unsigned char*)R)[kx];
          }
          if( Inv&1    )Wd=-Wd;
          if( Meashure )Wd*=Meashure;
          if( Max<Wd   )Max=Wd;
          if( Min>Wd   )Min=Wd; __Depth[y][x]=Wd;
        } ++y;
    } }
    //
    //  c ���������� ����������� ��������� ����������
    //
    Active|=1; is_Globus(); Map.Jx=Map.Jy=0.0;
    Allocate( 0,R );        Map.Ly=Map.My=Ny=Ly;
    close();                Map.Lx=Map.Mx=Nx=Lx;
    if( Inv&2 )for( y=0; y<Ny/2; y++ )          // �������� ������������ �����
    { R=__Depth[y]; __Depth[y]=__Depth[Ny-y-1]; __Depth[Ny-y-1]=R;
  } }
  if( !(Active&0x18) )Map.window(); // ����, ���� ����� �� �� ��������� ����
  //
  // ����� ��������� ��������� ����� ��� ���� ����������� ������
  //
  for( k=0; k<Nmp; k++ )find_coast( M[k] );
  First=false;          printf( "\n\n" );
  return *this;
}
