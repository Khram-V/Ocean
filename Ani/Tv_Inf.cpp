//
//     �������� ��������������� � ��������������� ����
//
//     �� ������ �������� ����������� �����-�-�-�-���� ������ � �� ������
//     ������������� ��������� �������� � ����� ������������.
//     ��� ������� ���� �������� ������������� ������ ��������� �����������,
//     ������� ������� 320�200. ��� ����� ����� ������ ������ ����������
//     -bgi=_ ��� /bgi=_ � ������, ���:
//          1-(320x200),   2-(640x400),    3-(640-480),
//          4-(800x600),   5-(1024x768) �  6-(1280x1024).
//          7-(1600x1200), 8-(1920x1440)
//     �������� ��������� ��������������� ���� ������� ������� �� ����������
//     ����� �� ������, �� ������� ����������� ������� ����.
//
//     ��������� ������ ����� ����������, � ������� ���������� ������� ��������
//     � �������������� �����, �����, ����� ��������� ����������� ������������
//     ������ ����������� �����. ������ ������ �������� �� ������������ �������
//     (��������������� �������), ����������� �������� �� ��������� �����������
//     ��������������� ����.
//
//     �� �������� <������� ������/�����> ��������������� ����������� ���������,
//     ������� ������������ �������� ����� ������. <�������/�����/����> ��������
//     ������ �������, ��� ����������� �� ������������� �����������.
//     ������� <Enter> �������������� ������� �����������.
//     ����� �� ��������� �� <Escape>.
//
//     F1 - ������� � ������������ ���
//     <shift>F1 - ������ �������
//     F2 - ������ ������ � ���� � �������� .inf, .grd
//     <Enter> - ������� ��������
//     <Space> - ��������� �� �������
//     <Insert> - ���������� ����� ������
//     <shift-Ins> - �������� � �������������� ����������������
//     leftBtn+move - ���������� ����� ��� �������� �����
//     -- leftBtn+move - ��������� ����� �� ����� �����
//     -- leftBtn - ���������� ������� ��������� �����
//
#include <StdIO.h>
#include "Tv.h"

#define _Title "Inf-Grd - Seafloor Bathymetry & Topography"
#define _H_Msg "Sp\\Cr\\Ins \\ BkSp-������ F1-��� F2-������ F5-pvt F10-�������� "
                                        //
#define Mile 1852.24637937659918        // Sea Mile
#define Pd      6.28218530717958647692  //  Pi x 2
#define Ph      1.57079632679489661923  //  Pi / 2
#define Pr      0.0174532925199432955   //  Pi / 180
#define rP     57.2957795130823229      // 180 / Pi
#define Eps     1.0e-3                  //
#define _BkColor 31                     //_AxColor 13 // LIGHTGRAY // 29
//
//      ���������� ���� ��������� ��������������� ����
//
static bool VAX=false;       // �������� ��������� �� ������������� �������
static char Inf[2][80],      // ��� ������ ������������
            Fname[MAX_PATH], //  ��� ����� ������
            Str[MAX_PATH+2], // -----------------------
            mPr='G';         //
static int  Typ=0,           // 0:Text 1:Byte 2:Integer
            Fmt=0,           // 0:Var  1:Line 2:Field 3:Space - ���������
            Ny,Nx,           // ����������� ��������� ������� Field's Dimension
            jY,jX,lY,lX,     // ������� ������� �������� ����������
            Nm=0;            // ���������� ������������ ������� � ������
static Real Dmx=1.0,         // ���� � ������� �������
            Fe,Cs=0.0,       // Meridian's Course [ �-degree ]
            Skew=120,        // ����������� �������� (������ �������)
            Wmin,Wmax;       // ���������� ����������� � Topography
static float *A,*B;          // 2 ������ ��� ������� ������� �� ������ �������
static Point Origin,         // ���������� ������ ������� ����
             Step,           // ���� ����������� ����������
       Az={ -.8660254,-.5 }; // ���� ��������� � �������������� ��
static Basis  Geo;           // ����� �������� � ������������
static Mario *Mar=NULL;      // ������ ������������ �����/��������
//
//     ��� ��������� ����� �������� � ��������� � ����������� ������
//
static int  Mx=0,My=0,Mv=0;  // ����������� � ����� �����-��������
static HIMG Scr=0;           // ������ ��������� �������
static float** Data=NULL;
//
//    ���������� ���������������� ���������� ������ (���� �����-��! - �������)
//
static FILE *F=NULL;    // �������� ���� � �����������
static long long Beg=0, // ��������� �������� ��� ���������� ������
            kX=0,kY=0;  // ������� ������� ������� � ������ (� �� ������)
                        //
static void swab4( float *A )
{ char tmp,*a=(char*)A; tmp=a[3]; a[3]=a[0]; a[0]=tmp;
                        tmp=a[2]; a[2]=a[1]; a[1]=tmp; *A=check( *A );
}
static void ReWind()
{ rewind( F ); if( Beg )fseeko64( F,Beg,SEEK_SET ); kX=kY=0;
}
static void Set( int y, int x )          // ����� �� ����������� �������� jY,jX
{ long long M; y=Fmt?y:Ny-y-1;           // �����-������ (�����-��-�����), ����
            M=((long long)y)*Nx+x;
  if( Typ )fseeko64( F,Beg+M*Typ,SEEK_SET ); //  1,2,4 � 0 � ��������� �� kY,kX
  else for( long long K=kY*Nx+kX; !feof( F ) && K<M; K++ )fscanf( F,"%*g" );
  kY=y;
  kX=x;                            // ���������� � ����� M �� ���� ������
}
static void Get( float *d, int  leng )
{ if( !Typ )while( --leng>=0 )             // ����� - ��� ������� ��� long long
  { fscanf( F,"%g",d++ ); kX++; if( kX>=Nx ){ kX=0; kY++; } } else
  { int i; fread( d,Typ,leng,F );
    if( VAX ){ if( Typ==2 )swab( (char*)d,(char*)d,leng<<1 ); else
               if( Typ==4 )for( i=0; i<leng; i++ )swab4( d+i ); }
    if( Typ==1 )for( i=leng-1; i>=0; i-- )d[i]=((char*)d)[i]; else
    if( Typ==2 )for( i=leng-1; i>=0; i-- )d[i]=((short*)d)[i];
  }
}
static void GetLine( int y, float* d )       /// �������� ������ ����� ������ !
{                Set( jY+y,0 );               // � ������ �������� ��������: Jy
  if( jX+lX>=Nx )Get( d+(Nx-jX-1),jX+lX-Nx ); //  ��� ���������� ������������
  else if( jX<0 )Get( d-jX,jX+lX+1 );         //   � ������ ������� ���������
                 Set( jY+y,jX<0?jX+Nx-1:jX );
                 Get( d,jX<0?-jX:min( lX,Nx-jX ) );
}
static void Put( float dat )                    // ��������� ����� ������������
{  if( Typ==4 )fwrite( &dat,4,1,F );                       else
   if( Typ==2 ){ short i=(short)dat; fwrite( &i,2,1,F ); } else
   if( Typ==1 ){ char c=(char)dat; fputc( c,F ); }
}
static Window Map;
static void aBasis();                      // ���������� ��������������� ������
static void Axis( bool isMar=false, int Cl=29 );
inline int  MlDv( int a, int b, int c ){ return (a*b)/c; }
inline Real Merkatory( Real Fi ){ return log( tan( (Fi+Ph)/2.0 ) ); }
inline Real Geography( Real Rm ){ return 2.0*atan( exp( Rm ) )-Ph;  }
                                //
static void ItD( Point& W )     // �������� �������� � ���.����������
{ W=Geo.Goext( W );             //
  if( mPr=='M' )W.y=Geography( W.y ); W/=Pr;
}
static void DtI( Point& W )     // ������� ������� ��������� �� ���������
{ //bool Rec=true;              // � ����������� ��������� �����, ��� ����
  Point Q=W; //R;               // �������� ���� ������� ����������� ����� 360�
/*Retry:*/
        Q*=Pr;
  if( mPr=='M' )Q.y=Merkatory( Q.y ); W=Geo.Goint( Q );
//  if( Q.x<0.0 || Q.x>=Nx
//   || Q.y<0.0 || Q.y>=Ny )
//  { if( Rec )
//    { R=Q; Q=W; if( Q.x<=0.0 )Q.x+=360; else Q.x-=360; Rec=false; goto Retry;
//    } else W=R;
//  }   else W=Q;
}
static int Info( int x,int y,bool Rect )
{       int is_mouse=0;         // =0 - ���������� ������ ����� � ������
 static int k=0; Point P;       // ���������� � ������� ��������� �������
  if( k>0 )                     //
  { Str[k]=0; while( k>0 )Str[--k]=' '; Tv.Text( North_West,Tv.mX,Tv.mY,Str );
  }
  if( Map.Is( x,y ) )
  { float W=Data[minmax( 0,My-y-Map.b.top+4,My-1 )] //! +4 - ???
                [minmax( 0,x-Map.b.left-1,Mx-1 )]; // �������� ����� � �������
    x=int( (P.x=Tv_X( x ))+0.5 ); color( BLUE );
    y=int( (P.y=Tv_Y( y ))+0.5 ); ItD( P );
    Tv_cross( (Point){ x,y },!Rect?16:0 );
    k =sprintf( Str,  W2D( " <%c> %s, " ),Rect?'*':'-',DtoA( P.y,3 ) );
    k+=sprintf( Str+k,W2D( "%s [ %d,%d ] == %-8.1f -- %x " ),
                                   DtoA( P.x,3 ),y,x,W,*((unsigned int*)&W) );
    Tv.Height( 8 ); Tv.Text( North_West,Tv.mX,Tv.mY,Str ); // Tv.Height( 0 );
                        is_mouse=1;             //
  } color( 63 ); return is_mouse;               // ���� ��� ��������� �����
}                                               //
static int Mouse_Console()                      // ������ � �������
{ Tv.Height( 8 );          Thelp( _H_Msg );     // ������� �����
  Tv.Height( 0 );          All.Draw();          //
 int Ans=Map.Console( Info ); Ghelp();
  if( Ans==_MousePressed )
  { int x,y;
    Tv_GetCursor( x,y );
    if( All.Is( x,y ) ){ jX=jY=0; lX=Nx; lY=Ny; Ans=-2; }
  } else
  if( Ans==-1 )
  { if( Map.x<Map._x )jX=Map._x,Map._x=Map.x,Map.x=jX; // ���������� ��
    if( Map.y<Map._y )jY=Map._y,Map._y=Map.y,Map.y=jY; // ��������� �����������
  //if( Map._x<0 )Map._x+=Nx,Map.x+=Nx;                // ����� ������
    jX=int( Tv_X( Map._x )-0.5 );
    jY=max( 0,int( Tv_Y( Map.y  )-0.5 ) );
    lX=minmax( 8,int( Tv_X( Map.x  )+1.5 )-jX,Nx );
    lY=minmax( 6,int( Tv_Y( Map._y )+1.5 )-jY,Ny ); if( jY+lY>Ny )jY=Ny-lY;
    Ans=-2;
  } return Ans;
}
//
//  ���� ������� ����� ������� � ����������� ������
//   ��� ���������� ��� ��������� ������ � ��������� ��������
//
#define RePlace( A,B ) { float *Z=B; B=A; A=Z; }
                                //
static void ReadDataImage()     // ������������ ������� ������ �� ������ ������
{                               //
 Real X,Y,V,W;
 int x,x1,x2,y,y1,y2,iy;
  Axis();                           /// ����� ����� ������ ������������ ����
  x=Tv_port.right-Tv_port.left+1;
  y=Tv_port.bottom-Tv_port.top+1;
  iy=imagesize( Tv_port.left,Tv_port.top,Tv_port.right,Tv_port.bottom );
  if( Mx!=x || My!=y || Mv!=iy )
  { Scr = Allocate( Mv=iy,Scr );
    Data=(float**)Allocate( My=y,(Mx=x)*sizeof( float ),Data );
    memset( Scr,0xE0,Mv );       // 224 = 0xE0 - ��� ���� �����
   ((short*)Scr)[0]=Mx;          // ����������� ����������
   ((short*)Scr)[1]=My;          // ������� bitmap [ MxL ]
  }                              //
  ReWind();                      // ��������� �������� ��� ������������� ������
  for( y=0; y<My; y++ )          //
  { if( y )Y=( lY-1 )*Real( y )/( My-1 ); else
      { Y=0.0; y1=iy=0; y2=1; GetLine( 0,B ); }        // Set( 0 ); Get( B ); }
    if( y<My-1 )
    { y2 = int( ( lY-1 )*Real( y+1 )/( My-1 ) );
      if( y2<=Y )y2 = int( Y+1.0 );
      if( iy<y2 ){ RePlace( A,B ) y1=iy; GetLine( iy=y2,B ); } // Set( iy=y2 ); Get( B ); }
    } V = (Y-y1)/(y2-y1);
    for( x=0; x<Mx; x++ )
    { x1 = int( X = (lX-1)*Real( x )/( Mx-1 ) );
      x2 = (lX<=Mx) ? x1+1 : (lX-1)*Real( x+1 )/( Mx-1 );
      if( x2>=lX )x2=lX-1;                                     // �� ��
      if( x1>=x2 )x1=x2-1; W = (X-x1)/(x2-x1);                 // ������ ������
//      W=Data[Fmt?y:My-y-1][x]
        W=Data[y][x] = ( A[x1]*(1-W) + A[x2]*W )*(1-V)         // ����������
                     + ( B[x1]*(1-W) + B[x2]*W )*V;            // ������������
      if( !(x|y) )Wmin=Wmax=W; else                            // ++
      if( Wmin>W )Wmin=W; else                                 // �����
      if( Wmax<W )Wmax=W;                                      // �����������
    } if( !(y%100) ){ Ghelp( "Read: Y=%d ",y ); Tv_getw(); }
  }                   Ghelp();
}
//      ������� ���������� ������� �����,
//      � ��� ����� � ������� ��������� ����������
//
static RGB_colors sea[224];     // ��� ������ 32-� ������
                                //
static void Palette_Paint()     // ��������� �������
{ static bool wLx=false;        // ��������� �������� �������
//static RGB_colors *grn=sea+72,*gry=sea+152;
  if( !wLx )                    //  -- ��������� ������������ ��������
  { int k; wLx=true;
//  sea = (RGB_colors*)Allocate( 224*sizeof( RGB_colors ) );
    for( k=0; k<72; k++ )sea[k].r=8*k/5,sea[k].g=20*k/6,sea[k].b=64+8*k/3;
    for( k=0; k<80; k++ )sea[k+72].g=k<20?k*4+172:(k<60?255-(k-20)*4:92+(k-60)*8),
                         sea[k+72].r=k<20?64*k/5 :(k<60?255-(k-20)*2:172+(k-60)*4),
                         sea[k+72].b=k<60?0:(k-60)*12;
    for( k=0; k<72; k++ )sea[k+152].r=sea[k+152].g=sea[k+152].b=255-10*k/3;
  } Set_Palette(  32,72, sea+152 ); // ����� ������� ��� ��������� �����
    Set_Palette( 104,72, sea );     // ������� ������ ����
    Set_Palette( 176,80, sea+72 );  // ������� ������� ��� ����������� ����
}
static int Topography_Inquiry( int ans=0 )
{ if( !ans )ans=Tv_getw();      // ������� ������ ������� �
  if( ans )                     // ���������, ���� ������� �������
  { const Point P = { 0.98078528,0.195090322 };      // 1 ���� - 11,25 ��������
    int b=0,g=Tv.mY,d=g-Tv_port.bottom-5,x,y;        //
     pattern( SOLID_FILL,_BkColor );      bar( b,g-d,b+d,g ); d/=2; b+=d+2;
     x=int( -d*Az.y ); setcolor( CYAN );  circle( b,g-d,d/4 );
     y=int(  d*Az.x ); setcolor( GREEN ); needle( b-x,g-d+y,b+x,g-d-y );
    switch( ans )
    { case North: Skew/=1.12; break; case East: Az/=P; break;
      case South: Skew*=1.12; break; case West: Az*=P; break;
      case _BkSp: if( Typ )               //
      { Tv_GetCursor( x,y );              // �������������� �������� ������
        if( Map.Is( x,y ) )               //
        { float W=Data[minmax( 0,My-y-Map.b.top+4,My-1 )]  // +4 - ??? � -
                      [minmax( 0,x-Map.b.left-1,Mx-1 )];   // ������ ���������
          x=int( Tv_X( x )+0.5 ); color( BLUE );
          y=int( Tv_Y( y )+0.5 ); Tgo( 1,0 );
          Tprintf( "Data[%d,%d] = ",y,x ); sprintf( Str,"%-10g \\",W );
          if( Tgets( Str,12 )==_Enter )
          { sscanf( Str,"%g",&W ); Set( y,x ); Put( W ); ans=-2;
  } } } } } return ans;
}
//  ���������� �������������� ����� � ���������� ���� ����� � ������� ��������
//
static void Topography_Paint(){
#pragma omp parallel for
  for( int y=0; y<My; y++ ) // ������� ����������� � ��������������� � ��������
  for( int x=0; x<Mx; x++ ) // ������ ��������� �����. ������� ���������������
  { Real U=Dmx*Data[y][x];  // �� ������� � ���������� ���������� ������� ���
    RGB_colors C=sea[minmax( 0,U>0.0?71-int(U*0.0072):72-int(U*0.0256),151 )];
    U=tanh( ((Data[y][min(Mx-1,x+1)]-Data[y][max(+0,x-1)])*Az.y
           + (Data[max(0,y-1)][x]-Data[min(My-1,y+1)][x])*Az.x)*Dmx*Skew/5e4 );
    C.b+=U*(U>0?255-C.b:C.b); // ������������ ������� ������ ������� �� 10000 �
    C.g+=U*(U>0?255-C.g:C.g); //   � ���������� ������������ � ������� ������
    C.r+=U*(U>0?255-C.r:C.r); ((unsigned*)Scr)[y*Mx+x+1]=C.i; // (� ����������)
  } Tv_PutImage( Tv_port.left+1,Tv_port.top+1,Scr );          // Axis();
}
//
//   ���������� ���������� ����������� �����
//
static void Topography_Shadow()   // � ���������� ���� ����� � ������� ��������
{ Point St=Dmx*My*Step*111138.0/Pr/lY;
#pragma omp parallel for
  for( int y=0; y<My; y++ )
  for( int x=0; x<Mx; x++ )
  { Real U = Dmx * Data[y][x],                   // ��������������� �� �������
         gY = ( Data[min( My-1,y+1 )][x]-Data[max( 0,y-1 )][x] )/St.y,
         gX = ( Data[y][min( Mx-1,x+1 )]-Data[y][max( 0,x-1 )] )/St.x,
          W = hypot( gY,gX );
    if( W<1.0e-6 )W=36; else                   //
        W = 36.0 * ( 1.0-(gY*Az.x-gX*Az.y)/W   // - �����������
                 * tanh( W*Skew ) );           // - ������ * sin(atan(W*Skew))
    ((unsigned*)Scr)[y*Mx+x+1] = sea[minmax( 0,int(U<=0?224-W:W),223 )].i;
  } Tv_PutImage( Tv_port.left+1,Tv_port.top+1,Scr );
}
//
//     ���������� ������� �������������� ����� - ���������
//
static int Topography()
{ Real Smax=0,Smin; int Key;
  for( int ans=0;; ans++ )
  { if( ans%2 ){ Smin=72.0/Wmax; if( Wmin<-1e-6 )Smax=-5184.0/Wmin; }
          else { Smax=1.0,                  // 1.125, // 0.72*(0.75^2)/0.48
                 Smin=0.0072;               // ������� � 10000-��� �����������
                 if( ans )Dmx=-Dmx; Real W=Wmax; Wmax=-Wmin; Wmin=-W;
               }
#pragma omp parallel for
    for( int y=0; y<My; y++ )                // ���������� ����� ������ ������
    for( int x=0; x<Mx; x++ )                //  ��������������� �� �������
    { int d; Real W=Dmx*Data[y][x];          //   � ����� �����������
    if( !ans ){ if( !(x|y) )Wmin=Wmax=W; else            //
                if( Wmin>W )Wmin=W; else                 // ����� �����������
                if( Wmax<W )Wmax=W; }                    //
      if( W==0.0 )d=72; else
      if( W>0.0 )d=71-W*Smin; else d=72+sqrt( -W*Smax );
      ((unsigned*)Scr)[y*Mx+x+1] = sea[ minmax( 0,d,151 ) ].i;
    } Tv_PutImage( Tv_port.left+1,Tv_port.top+1,Scr );
      if( (Key=Mouse_Console())!=_Space )return Key;
} }
//
//      ���������� ��������
//
static void TopoLine( float *Lvl, int *C, int Cl )
{
#pragma omp parallel for
  for( int y=0; y<My; y++ )
  for( int x=0; x<Mx; x++ )
  { Real U=Dmx*Data[y][x],xU=Dmx*Data[y][min( Mx-1,x+1 )],
                          yU=Dmx*Data[min( My-1,y+1 )][x],
                          yx=Dmx*Data[min( My-1,y+1 )][min( Mx-1,x+1 )];
    for( int c=0; c<Cl; c++ )
    { Real Lc=Lvl[c],xx=(yx-Lc + xU-Lc)*(yU-Lc + U-Lc), // xx=(U-Lc)*(yx-Lc),
                     yy=(yU-Lc + yx-Lc)*(U-Lc + xU-Lc); // yy=(xU-Lc)*(yU-Lc);
      if( (xx<0 || yy<0) || ( !(xx*yy) && (xx || yy) ) )
                         ((unsigned*)Scr)[y*Mx+x+1]=Default_Palettes[C[c]].i;
  } } Tv_PutImage( Tv_port.left+1,Tv_port.top+1,Scr );
}
static int Topography_Line()
{
 static float L[]={-5000,-2000,-1000,-200,-100,-20,-10,5,20,100,200,1000,2000,5000,0    },
              M[]={-7000,-3000, -500,-300, -50, -5, -1,1,10, 50,300, 500,3000,7000,1e-3 };
 static int   C[]={ WHITE,BROWN,YELLOW,GREEN,GREEN,LIGHTGREEN,LIGHTGREEN,
                     CYAN,LIGHTBLUE,LIGHTBLUE,BLUE,BLUE,BLUE,BLUE,BLACK };
 int i,kb=Tv_getk()&SHIFT;
  for( int ans=0;;ans++ )
  { if( Wmin*Wmax<=0.0 && !kb )
    { float *LC=ans?M:L;
      int k=0,n=0;
      for( i=0; i<sizeof( C )/sizeof( int ); i++ )
      { if( LC[i]<Wmin )k++;   else
        if( LC[i]>Wmax )break; else n++;
      } TopoLine( LC+k,C+k,n );
    } else
    { float L[12]; int c,C[12];
      for( c=0; c<12; c++ ){ L[c]=Wmin+(float(c)+(ans?1:.5))*(Wmax-Wmin)/12;
                             C[c]=L[c]<0.0?172:LIGHTCYAN;
                           } TopoLine( L,C,12 );
    } if( ans>0 ){ i=0; break; }
      if( (i=Mouse_Console())!=_Ins )break;
  }   return i;
}
///    +----------------------------------------------------+
///    �  �������� ��������� ���������� �������� � �������� �
///    +----------------------------------------------------+
static void Write();
static void PVT();

static void Show_Topography_data()    // ���������� � ������������� ����������
{ bool isPaint=true;                  // ����������������� ������ � �����������
   A=(float*)Allocate( Nx*sizeof(float) );
   B=(float*)Allocate( Nx*sizeof(float) ); ReadDataImage();
///
/// ������� ���� ������������� �������� ��� ����� ���������� �����
///
  for( int i=0xFA;; )                //
  { Topography_Inquiry( -1 );        // ��������� ������� �� ������� ��������
    switch( i )                      //
    { case _Enter: isPaint = !isPaint; goto Paint;
      case -2    : ReadDataImage();    goto Paint;
      case 0xFA  : Az.x=-0.8660254,Az.y=-0.5,Skew=120.0;
      case North : case East:                  //
      case South : case West:                  // ����� �� ����������� ��������
    Paint :                                    //
       if( isPaint )Topography_Paint();        // like 3D light ��������
              else  Topography_Shadow(); i=(i==0xFA)?0xF1:-1; continue;
      case _Ins  : i=Topography_Line(); continue;
      case _Space: i=Topography();      continue;        // ������� ����������
      case 0xF1  : Axis( true );        break;
      case 0xF2  : Write();             break;
      case 0xF5  : PVT();               break;
      case _Esc  : Allocate( 0,A );
                   Allocate( 0,B ); fclose( F ); return;
    } i=Topography_Inquiry( Mouse_Console() );
} }
// IsDegree( char* Msg ){ return strpbrk( Msg,"�:'\"" )!=NULL; }

void Inf_View( char *Name )
{
 char c,*S,*Z; if( ( F=fopen( fext( strcpy( Str,Name ),"inf" ),"rt") )==0 )
                     Break( "\7\n?File %s not open\n",Str );
//
//       ���������� � �������� ��������������� �����
//
  fgets( Str,MAX_PATH,F ); strcut( strncpy( Inf[0],Str,80 ) ); // ��������
  fgets( Str,MAX_PATH,F ); strcut( strncpy( Inf[1],Str,80 ) ); //  ����� ������
  fgets( Fname,MAX_PATH,F ); strcut( Fname );                  // ��� �����
  fgets( Str,MAX_PATH,F ); sscanf( Str,"%d",&Typ );            // ��� �����
  fgets( Str,MAX_PATH,F ); sscanf( Str,"%d",&Fmt );            // ������ = 2
  if( Fmt!=2 )Break( "\n? No Bathymetry data\n" );        //
  fgets( Str,MAX_PATH,F ); sscanf( Str,"%d%d",&Ny,&Nx ); // �����������
  fgets( Str,MAX_PATH,F ); jY=jX=0; lY=Ny; lX=Nx;       // ������ - ���� ������
  fgets( Str,MAX_PATH,F );                             //
  sscanf( Str,"%lf%lf%c",&Wmin,&Wmax,&c );
     c|=0x20; Dmx=Wmin<Wmax?-1.0:1.0;
  if( c=='c' )Dmx*=0.01; else
  if( c=='d' )Dmx*=0.1;
  fgets( Str,MAX_PATH,F ); sscanf( Str,"%lf%lf%s%lf",&Step.y,&Step.x,Str+80,&Fe );
  if( Step.y<0.0 ){ Step.y=-Step.y; Fmt=0; }
  if( !Step.y )Step.y=1.0;
  if( !Step.x )Step.x=Step.y;
      mPr='G';
    c=Str[80]|=0x20;
      Step*=Pr;
      Fe*=Pr;
  switch( c )
  { case 'k' : Step*=1000;  case 'm': Step/=Mile*cos( Fe ); mPr='M';
    case '\'': Step/=60.0;  case 'd': break;
  }
  fgets( Str,MAX_PATH,F ); AtoD( AtoD( AtoD( Str,Cs ),Origin.y ),Origin.x );
  Cs*=Pr;
  Origin*=Pr; aBasis(); jY=jX=0; lY=Ny; lX=Nx;
                                // aBs=Origin;
//Step/=Pr/111138.0;            // � ������
//
//     ������� ������������� ������
//
 int k;
  Nm=0;
  fgets( Str,MAX_PATH,F ); sscanf( Str,"%d",&Nm );
  if( Nm>0 )Mar=(Mario*)Allocate( Nm*sizeof( Mario ),Mar );
  for( k=0; k<Nm && fgets( Str,MAX_PATH,F ); k++ )
  { for( S=Str; *S && *S<=' '; S++  );
    if( !strcut( S ) || *S==';' ){ --k; continue; }
    S=AtoD( AtoD( S,Mar[k].G.y ),Mar[k].G.x );
    for( ; *S && *S<=' '; S++  )*S=0;              // �� ������ �������� ������
    if( !strcspn( Str,"�:\xF8\"'") )ItD( Mar[k].G ); // ��� ��������� � �����
    if( *S )
    { for( Z=S; *Z && *Z!=';'; Z++ ); *Z=0;
      if( strcut( S )>0 )Mar[k].cp=strdup( S );      // �������� ������
    }
  } Nm=k;
  fclose( F );
//
//      �������� ����� ������
//
  S=strcpy( Str,Name );                  // ����� ��������
  if( Fname[0]>' ' )                          // ������� ����� �����
  { OemToCharBuff( Fname,Fname,strcut( Fname )+1 );
    if( Fname[1]!=':' )                       // ���� �� ����� ���� � �����
    { if( (S=strrchr( Str,'\\'))!=NULL )S++;  // �� ������� �� ����� ����� .inf
      else S=Str;                             //
    } strcpy( S,Fname );
  } fext( strcpy( Fname,Str ), (char*)(Typ==1?"byt": Typ==2?"int":"dat") );
  F=fopen( Fname,!Typ?"rt":"r+b" );
  if( !F )Break( "\n?%s not found\n",Fname );
  Show_Topography_data();
}
///
///
///
static void Attribute_read( Real *A=NULL, Real *B=NULL )
{ int i,j,k,n,iD;
  color( LIGHTGRAY );
  Tprintf( W2D( "                      ���� ��������� : [%d]" ),n=l_read(F) );
  for( i=0; i<n; i++ )
  { Tln(); Tprintf( "%36s : ",s_read(F) );
    switch( iD=l_read(F) )
    { case NC_BYTE:
      case NC_CHAR:  Tprintf( s_read(F) ); break;
      case NC_SHORT: Tprintf( "%d~i~ ",k=l_read(F) ); for( j=0; j<k; j++ )Tprintf( "%d, ",i_read(F) ); if( k&1 )i_read(F); break;
      case NC_LONG:  Tprintf( "%d~l~ ",k=l_read(F) ); for( j=0; j<k; j++ )Tprintf( "%d, ",l_read(F) ); break;
      case NC_FLOAT: Tprintf( "%d~f~ ",k=l_read(F) ); for( j=0; j<k; j++ )Tprintf( "%g, ",f_read(F) ); break;
      case NC_DOUBLE:
      { Tprintf( "[%d] ",k=l_read(F) );
        if( i==n-1 && k==2 && A!=NULL )
        { color( LIGHTGREEN ); Tprintf( "%lg, ",*A=d_read(F) ); --Typ; // -=3;
                               Tprintf( "%lg, ",*B=d_read(F) );      // �������
        } else for( j=0; j<k; j++ )Tprintf( "%lg, ",d_read(F) );
      } break;
     default: Break( "������������ ��� �������� %d",iD );
  } }
}
void GRD_View( char *Name )
{
 union { short i; char c[2]; } D;
 int i,j,k,n; //,iD;                               // ������ ��������� ��������
  Typ=-1; Fmt=2; Dmx=-1.0;
  if( !(F=fopen( Name,"r+b" )) )Break( "\7\n?%s not open\n",Name );
  D.i=get2( F );
  if( D.c[0]=='D' && D.c[1]=='S' ) // ( get2( F )=='DS' )
  { D.i=get2( F ); mPr='G'; // ������ ����� ��������� ������-���� (����: Fmt=0)
    if( D.c[0]=='B' && D.c[1]=='B' )Typ=4; else
    if( D.c[0]=='A' && D.c[1]=='A' )Typ=0; else
        Break( "\7\n?%s isn't Surfer.grd",Name );
  } else
  if( D.c[0]=='C' && D.c[1]=='D' ) // ( get2( F )=='CD' )
  { D.i=get2( F );
    if( D.c[0]!='F' || D.c[1]!=1 )Break( "\7\n?%s isn't Topo.grd",Name );
    Tv_Graphics_Start();
    setwindowtitle( "Tv - NetCDF Bathymetry array from UCSD" );
    color( LIGHTCYAN );
    Tv.Height( 16 ); Tprintf( "%s %s ( %ld )\n",__argv[0],__argv[1],l_read( F ) );
    Tv.Height( 0 );
  //
  // ���������������� �������� ������� ������
  //
    if( l_read(F)==NC_DIMENSION )  // Dimensions_read()
    { color( WHITE );
      Tprintf( W2D( "����������� ���������������� ������� : [%d]\n" ),n=l_read(F) );
      if( n>=2 )
      { Tprintf( "%36s : ",s_read(F) ); Tprintf( "%d\n",Nx=l_read(F) );
        Tprintf( "%36s : ",s_read(F) ); Tprintf( "%d\n",Ny=l_read(F) );
        for( i=2; i<n; i++ )
        { Tprintf( "%36s : ",s_read(F) ); Tprintf( "%d\n",Nx=l_read(F) );
        }
        if( l_read(F)==NC_ATTRIBUTE )Attribute_read();
        if( l_read(F)==NC_VARIABLE  ) // Variable_read();
        { Tln(); color( YELLOW );
          Tprintf( W2D( "       ���������� ������� ���������� : [ %ld ]\n" ),n=l_read(F) );
          for( i=0; i<n; i++ )        // ��� Variable
          { color( YELLOW ); Tprintf( "%36s : ",s_read(F) ); Tprintf( "[%d]",k=l_read(F) );
            for( j=0; j<k; j++ )Tprintf( " %d",l_read(F) ); Tln();
            if( (k=l_read(F))!=NC_ATTRIBUTE )color( RED ),Tprintf( "%36s : %d",NC_Type_name[k],l_read(F) );
            else Attribute_read( !i ? &Origin.x:( i==1 ? &Origin.y:(i==2 ? &Wmin:NULL ) ),
                                 !i ? &Step.x : ( i==1 ? &Step.y : (i==2 ? &Wmax:NULL ) ) );
            Tpoint.x=Tv.mX/2; color( CYAN );
            Tprintf( "%s",NC_Type_name[j=l_read(F)] ); // ��� �����
            Tprintf( " / %7ld /",l_read(F) );         // ����� �������
            Tprintf( " %ld\n",Beg=l_read(F) );       // �������� � ������
            if( Typ==-4 )
            { if( j==3 || j==5 )
              { Typ=j-1; VAX=true; color( LIGHTCYAN ); Tln();
                Tv.Height( 16 ); Tprintf( W2D( "�������� ������� [n/y]" ) );
                j=Tv_getc();     if( (j|0x20)!='y' && j!=_Enter )exit( 1 );
                  Tv_close();    goto Graphics;
    } } } } } } Break( "?don't read UCSD NetCDF bathymetry" );
  }
  if( Typ==-1 )Break( "\7\n?%s isn't topo.grd",Name );
  { double wdl; VAX=false;
    if( Typ )
    { Nx=get2( F );
      Ny=get2( F );
      fread( &wdl,8,1,F ); Origin.x=wdl; fread( &wdl,8,1,F ); Step.x=wdl;
      fread( &wdl,8,1,F ); Origin.y=wdl; fread( &wdl,8,1,F ); Step.y=wdl;
      fread( &wdl,8,1,F );
      fread( &wdl,8,1,F );
    } else fscanf( F,"%d%d%lf%lf%lf%lf%lf%lf",
                 &Nx,&Ny,&Origin.x,&Step.x,&Origin.y,&Step.y,&Wmin,&Wmax );
  }
  Beg=ftello64( F );
Graphics:                                                   //
  Step.x=( Step.x-Origin.x )/(Nx-1); Cs=0.0;                // � ������
  Step.y=( Step.y-Origin.y )/(Ny-1); Origin*=Pr;            //
                                     Step*=Pr;
  aBasis();                       // Step/=Pr/111138.0;
  jY=jX=0; lY=Ny; lX=Nx;
  strcpy( Fname,Name );     Show_Topography_data();
}
//    �---------------------------------------------�
//    �  ���������� �������������� ����� /91.02.06/ �
//    �---------------------------------------------�
//
static void aBasis()
{ Real Y=cos( Cs ),X=sin( Cs );
  if( mPr=='G' )X/=cos( Origin.y ); else Origin.y=Merkatory( Origin.y );
  Geo.Euler( Origin,90-atan2( Y,X )/Pr,Step );
}
static void Cross( Point P, Real W )
{
 Course C=South;
 int c=color( BLUE ); point p=P;
  if( P.x-jX<Eps ){ p.x-=4; C=West; } else p.y+=4; Tv.Text( C,p,DtoA( W ) );
  color( c );
}
static void aLine( Point P, Real W )
{ static bool L=false;
  static Point Q;
  if( !L )Q=P; else
  { if( P.y<jY ){ P.x-=( P.y-jY )*( P.x-Q.x )/( P.y-Q.y ); P.y=jY; } else
    if( P.x<jX ){ P.y-=( P.x-jX )*( P.y-Q.y )/( P.x-Q.x ); P.x=jX; }
    if( P.x-jX<Eps || P.y-jY<Eps )Cross( P,W );
    if( Q.y<jY ){ Q.x-=( Q.y-jY )*( Q.x-P.x )/( Q.y-P.y ); Q.y=jY; } else
    if( Q.x<jX ){ Q.y-=( Q.x-jX )*( Q.y-P.y )/( Q.x-P.x ); Q.x=jX; }
    if( Q.x-jX<Eps || Q.y-jY<Eps )Cross( Q,W );
    line( P,Q );
  } L^=1;
}
static Real STEP( Real Dst )
{ Real Di=log10( Dst ),iPart=floor( Di );
       Di=exp( (Di-iPart)*M_LN10 );
       Dst=pow( 10.0,iPart )*( Di>6 ?2 : Di>3 ?1 : Di>1.5 ?0.5:0.2 );
  if( Dst>30 )Dst=floor( Dst/30 )*30; return Dst;
}
///
///     �������� ���� � ���������� �������� �������������� �����
///
static void Axis( bool isMar, int Cl )
{
 static int wLx=-1,wLy=-1; Point Q,Pmin,Pmax,Pst; int g,k,x,y,cl;
 Real W;                   Map.f=(field){ -44,-55,-5,0,1 };
  Tv.Height( 0 );          Map.F=(Field){ jX,jY,lX-1,lY-1 };
                           W=lY*Step.y/( lX*Step.x );
  if( W>Real( y=getmodeheight()+Map.f.Jy+Map.f.Ly )
          / ( x=getmodewidth()+Map.f.Jx+Map.f.Lx ) )x=int( y/W+0.5 );
                                               else y=int( x*W+0.5 );
  x -= Map.f.Jy+Map.f.Ly;               // ������ �������� ����������� ������
  y -= Map.f.Jx+Map.f.Lx;               // ���� ������ �������������� ���������
  if( wLx!=x || wLy!=y )
  { if( wLx<0 )Tv_Graphics_Start( -1,x,y ); // ��������� ����
         else  Tv_resize( x,y );            // ��� ����������������� ��������
    setwindowtitle( _Title );
    bkcolor( _BkColor );
    Tv_Start_Mouse();
    wLx=x,wLy=y;
    All.Initial( " 1:1 ",0,0 );     // � �������� ��� �������� � �������� �����
  }
  Map.Focus();
  Palette_Paint();
  color( DARKGRAY );                //
  pattern( SOLID_FILL,_BkColor );   // ������� ��������� �����
  bar( 0,0,Tv_port.left,Tv.mY  );   //
  bar( 0,Tv_port.bottom,Tv.mX,Tv.mY );
  Tv_rect();
   y=Tv_port.bottom;
   x=Tv.mY-y;
  Tv.Text( x,y+=20,       Inf[0] ); // ������� � �����
  Tv.Text( x,y+=4*Tv.Th/5,Inf[1] ); // � �������� �������
  for( k=0; k<256; k++ )            //
  { pattern( SOLID_FILL,k );
    bar( Tv.mX-4,Tv_port.bottom-MlDv( Tv_port.bottom,k,256 ),
    Tv.mX,  Tv_port.bottom-MlDv( Tv_port.bottom,k+1,256 ) );
  }
  pattern( SOLID_FILL,_BkColor );
  sprintf( Str,"%s[%dx%d]",sname( W2D( Fname ) ),lY,lX );
  color( DARKGRAY );  k=Tv_port.bottom+Tv.Th+15;
  Tv.Text( North_West,Tv.mX-9,k,Str );      // �������, �����������
  g=Tv.mY;                                  // ����������� �����������
  k=( g-Tv_port.bottom-5 )/2;               //
  x=int( -k*Az.y ); color( LIGHTCYAN );  circle( k+2,g-k,k/4 );
  y=int( k*Az.x );  color( LIGHTGREEN );
//if( !Fmt )y=-y;
                    needle( k+2-x,g-k+y,k+2+x,g-k-y );
  if( Tv_getk()&SHIFT )return;
  Tv.Font( _Small_font,15 );   //Tv.mY/59+3 );
  if( Cl )cl=color( Cl|0x18 );
  //
  //  Data Title & Size of Image Field Calculating
  //
 Point P[4]; //={ { jX,jY },{ lX+jX,jY },{ lX+jX,lY+jY },{ jX,lY+jY } };
  P[0].x=P[3].x=jX; P[2].x=P[1].x=lX+jX;
  P[0].y=P[1].y=jY; P[2].y=P[3].y=lY+jY;
  for( k=3; k>=0; k-- )            //
  { ItD( Q=P[k] );                 // ������������ ����������
    if( k==3 )Pmin=Pmax=Q; else    //
    { if( Q.y>Pmax.y )Pmax.y=Q.y; else if( Q.y<Pmin.y )Pmin.y=Q.y;
      if( Q.x>Pmax.x )Pmax.x=Q.x; else if( Q.x<Pmin.x )Pmin.x=Q.x;
  } } Pst.y=STEP( Pmax.y-Pmin.y ); //
      Pst.x=STEP( Pmax.x-Pmin.x ); // ��� ����������� �����
  //
  //                    L A T I T U D E
  //
  for( W=Pst.y*floor( Pmin.y/Pst.y+1 ); W<Pmax.y; W+=Pst.y )
  { Q.y=W; Q.x=Pmin.x+1.e-12; DtI( Q ); aLine( Q,W );
    Q.y=W; Q.x=Pmax.x-1.e-12; DtI( Q ); aLine( Q,W );
  }
  //                    L O N G I T U D E
  //
  for( W=Pst.x*floor( Pmin.x/Pst.x+1 ); W<Pmax.x; W+=Pst.x )
  { Q.x=W; Q.y=Pmin.y+1.e-12; DtI( Q ); aLine( Q,W );
    Q.x=W; Q.y=Pmax.y-1.e-12; DtI( Q ); aLine( Q,W );
  }
  //            �������� �������
  //
  if( isMar )
  { Tv.Font( _Small_font,15 ); hidevisualpage();
    for( k=0; k<Nm; k++ )
    { Q=Mar[k].G; DtI( Q );
      if( Q.x>=jX && Q.x<jX+lX && Q.y>=jY && Q.y<jY+lY )
      { color( 36 ); setlinestyle( 0,0,4 ); Tv.Text( Center,Q,Mar[k].cp );
        color( 28 ); setlinestyle( 0,0,2 ); Tv.Text( Center,Q,Mar[k].cp );
        color( 18 ); setlinestyle( 0,0,1 ); Tv.Text( Center,Q,Mar[k].cp );
    } }              showactivepage();
  } Tv.Font( 0,8 );  color( cl );
}
//    �---------------------------------------------�
//    �  ������ ������ � ������ ��������� ��������  �
//    �---------------------------------------------�
//
void dprint( FILE *Fs, const int _Ms, const char *fmt, ... )
{ const char
  *Msg[]={ "Type        0:Text 1:Byte 2:Integer 4:Float",
           "Format      2:Field",
           "Ny Nx       Dimensions",
           "Jy Jx Ly Lx Base & Size",
           "Max Min     Bathymetry Extrema",
           "Sy Sx ' Fe  Grid Step, �'km & Lat.of scale",
           "Cs Fi�'Lo�' Course, Latitude & Longitude",
           "Mario" };
  va_list aV;
  va_start( aV,fmt ); vsprintf( Str,fmt,aV ); va_end( aV );
  fprintf( Fs,"%-30s \xB3 %s\n",Str,W2D( Msg[_Ms] ) );
}
static void Write()
{
//     ������ ��������������� �����
//       � ������������� ����� ���� � �������� �� ��� ������
//
 FILE *Fs;
 long long y,x,Ffs=0;
 int  Tp=Typ,k,c=color( CYAN ); Tv.Font( 0,8 );
 static char p[4]={ 1,9,17,28 },*ext,
         n[4][12]={ "Grd-txt","Grd-bin","Bathymetry","Grid-pts" }; Ghelp();
  do
  { Tgo( 1,0 );
    for( k=0; k<4; k++ )Tprintf( "%s ",n[k] ); Tprintf( "\x1b\x09\x1a\n" );
    if( Tp<0 )Tp=3; else
    if( Tp>3 )Tp=0; color( BLUE );
    Tgo( p[Tp],0 ); Tprintf( strupr( n[Tp]+1 )-1 ); strlwr( n[Tp]+1 );
    k=Tv_getc();    color( CYAN );
    if( k==East )Tp++;
    if( k==West )Tp--;
  } while( k!=_Enter && k!=_Esc ); Tgo( 1,0 ); Tprintf( "\n" );
  if( k==_Enter )
  { Point Bs,Es; Bs.x=jX;            Es.x=jX+lX;
                 Bs.y=jY; ItD( Bs ); Es.y=jY+lY; ItD( Es );
       fext( Fname,ext=string(Tp<2?"grd":(Tp==2?"inf":"2Dp")) );
    do
    { color( RED  ); Tgo( 1,0 ); Tprintf( "%-29s","Name:" );
      color( BLUE ); Tgo( 7,0 ); k=Wgets( Fname,-64 );
                     Tgo( 1,0 ); Tprintf( "%29c",' ' );
      if( k==_Esc )return;
    }
    while( k!=_Enter );
    //
    // �������� ����� ��� ������ ����� ����������
    //
    if( (Fs=Topen( Fname,Tp==1?"w+b":"wt",ext ))!=NULL )
    { Thelp( "! ���� ������: %s",Fname );
      k=Tv_getk()&SHIFT;
      if( Tp<2 )
      { static struct{ char DS[4]; short X,Y; Real Xo,Xn,Yo,Yn,Zo,Zn; }
         w={ { 'D','S','B','B' } };
         w.X=lX; w.Zo=-Wmax; w.Xo=Bs.x; w.Yo=Bs.y;
         w.Y=lY; w.Zn=-Wmin; w.Xn=Es.x; w.Yn=Es.y;
        if( Tp ){ fwrite( &w,1,sizeof( w ),Fs ); Ffs=ftello64( Fs ); } else
                  fprintf( Fs,"DSAA\n%d %d\n%lg %lg\n%lg %lg\n%lg %lg\n ",
                                     lX,lY,w.Xo,w.Xn,w.Yo,w.Yn,w.Zo,w.Zn );
      } else
      if( Tp==2 )
      { Real d = fabs( Dmx );
        Point St; St=Step*( rP*60.0 );
        if( mPr!='G' )St*=1852.3*cos( Fe );
        strcpy( Str+116,DtoA( Bs.y,-3 ) );
        strcpy( Str+100,DtoA( Bs.x,-3 ) );
        fprintf( Fs,"%s\n%s\n\n",Inf[0],Inf[1] );
        dprint( Fs,0,"%2d",Typ );
        dprint( Fs,1," 2" );
        dprint( Fs,2,"%4d %d",lY,lX );
        dprint( Fs,3," 0 0 0 0" );
        dprint( Fs,4,"%6.6lg %1.6lg %s",Dmx<0?-Wmax:Wmax,
                                        Dmx<0?-Wmin:Wmin,d<0.09?"cm":(d<0.9?"dm":"m") );
        dprint( Fs,5,"%6.6lg %1.6lg %c %.0lf",
              ( Fmt || Tp || Typ )?St.y:-St.y,St.x,mPr=='G'?'\'':'m',Fe*rP );
        dprint( Fs,6,"%s %s %s",DtoA( Cs*rP,2 ),Str+116,Str+100 );
        dprint( Fs,7,"%6d",Nm );
        for( k=0; k<Nm; k++ )
        { fprintf( Fs,"%-13s ",DtoA( Mar[k].G.y,-3 ) ); char *s=Mar[k].cp;
          fprintf( Fs,"%-14s  %-s\n",DtoA( Mar[k].G.x,-3 ),s?s:"\xFA" );
        }
        fclose( Fs );
        if( !(Fs=Topen( Fname,!Typ?"wt":"w+b",Typ==1?"byt":(Typ==2?"int":"dat") )) )
          { Tv_bell(); return; }
      } else
      fprintf( Fs,"Lat,Lon,Dp\nQ 1 1 99 1 1 99 99 99\nS %lg %lg %lg %lg %lg\n",
                           Bs.x,Bs.y,Es.x-Bs.x,Es.y-Bs.y,(Es.y-Bs.y)/1e+6 );
      //
      // ���������� ������ ������
      //
      ReWind();
      for( y=0; y<lY && !feof( F ); y++ )
      { GetLine( y,B );                                  // Set( y ); Get( B );
        if( Tp==3 )
        { for( x=0; x<lX; x++ )
          { Bs.x=x+jX; Bs.y=y+jY; ItD( Bs );
            fprintf( Fs,"%lg\t%lg\t%g\n",Bs.x,Bs.y,B[x] );
        } } else
        if( !Tp || (!Typ && Tp==2) )
        { for( x=0; x<lX; x++ )fprintf( Fs," %g",B[x] ); fputs( "\n",Fs );
        } else                             //
        { k = ( Tp==1 ) ? 4:Typ;           // �������� ���� � ����������� �����
//        if( !Fmt )fseeko64( Fs,Ffs+(lY-y-1)*lX*k,SEEK_SET );
          if( k==4 )fwrite( B,((Tp==1)?4:Typ),lX,Fs ); else
          for( x=0; x<lX; x++ )
          { k = int( B[x] ); if( Typ==1 )fputc( k,Fs ); else put2( k,Fs );
      } } }
      Ghelp();
      fclose( Fs );
  } } color( c );
}
static void PVT()       //
{                       // ��������� ������� ���������� ��������
 Point P;               // � ������� *.PVT
 FILE *F;               //
 Tfile_Filter=(char*)( "PVT for Garmin units of G7to.exe (*.pvt)\0*.pvt\0"
                       "All Files (*.*)\0*.*\0" );
  strcpy( fname( Fname ),"*.pvt" );
  F=Topen( Fname,"rt","pvt" );
  for( int i=0; !feof( F ); i++ )      // "*.PVT for Garmin units of G7to.exe"
  { fgets( Str,sizeof( Str ),F );
    if( !strtok( Str,"," ) )break;
    for( int j=0; j<5; j++ )strtok( NULL,"," );
    sscanf( strtok( NULL,"," ),"%lf",&P.y ); color( BLACK );
    sscanf( strtok( NULL,"," ),"%lf",&P.x ); DtI( P ); Str[0]=0;
    if( i )lineto( P );
      else moveto( P );
  } fclose( F );
}
#if 0
static float Get()
{ float d;
  for( int J = Fmt ? jY : Ny-jY-lY; !feof( F ); )
  { if( !Typ )fscanf( F,"%g",&d ); else
    if( Typ==1 )d=(unsigned char)fgetc( F ); else
    if( Typ==2 )d=get2( F ); else fread( &d,4,1,F );
    if( kX>=jX && kX<jX+lX && kY>=J && kY<J+lY )break;
    if( ++kX==Nx ){ kY++; kX=0; }
  } if( ++kX==Nx ){ kY++; kX=0; } return d;
}
#endif
