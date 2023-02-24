//
//      �������� ��������������� � ��������������� ����
//
//      �������� ��������� ��������������� ���� ������� ������� ��
//      ���������� ����� �� ������, �� ������� ����������� ������� ����.
//
//      ��������� ������ ����� ����������, � ������� ���������� �������
//      �������� � �������������� �����, �����, ����� ��������� �����������
//      ������������ ������ ����������� �����. ������ ������� ��������
//      �� ������������ ������� (������.�������), ��� ����������� ��������
//      �� ��������� ����������� ��������� � ���� ����.
//
#include "..\Graphics\Tv_Graph.h"
//
//      ���������� ���� ��������� ��������������� ����
//
static FILE *Grd=NULL;   // �������� ���� � �����������
static int  Typ=0,Ny,Nx; // 0:Text 1:Byte 2:Integer Field's Dimension
static long Beg=0;       // ��������� �������� ��� ���������� ������

static union RGBi{ unsigned i; struct{ char b,g,r,a; }; } sea[152]; //sea[72],grn[80];
static void Palette()    //
{ static bool wLx=false; // ��������� �������� �������
  if( !wLx )             //
  { int k; RGBi *grn=sea+72; wLx=true;
    for( k=0; k<72; k++ ){ sea[k].r=8*k/5; sea[k].g=20*k/6; sea[k].b=64+8*k/3; }
    for( k=0; k<80; k++ ){ grn[k].g=k<20?k*4+172:(k<60?255-(k-20)*4:92+(k-60)*8);
                           grn[k].r=k<20?64*k/5:(k<60?255-(k-20)*2:172+(k-60)*4);
                           grn[k].b=k<60?0:(k-60)*12; }
} }
//    ���������� ���������������� ���������� ������
//
static void Topography()
{ int    y2=0,L;
  bool   First=true;                             // ������ ������ � ������� �����
  float *A=(float*)Allocate( Nx*sizeof(float) ), // ��� �������� �������
        *B=(float*)Allocate( Nx*sizeof(float) ); // ��� ������ ����� ����������
  HIMG Scr=Allocate( L=imagesize( Tv_port.left,Tv_port.top,Tv_port.right,Tv_port.bottom ) );
  RGBi C;
   memset( Scr,224,L );
  ((short*)Scr)[0]=L=Tv_port.right-Tv_port.left;
  ((short*)Scr)[1]=Tv_port.bottom-Tv_port.top;
//
//     ���������� ������������ �������������� �����
//
  rewind( Grd );
  fseek( Grd,Beg,SEEK_SET );
  for( int y=Tv_port.bottom; y>=Tv_port.top; y-- )
  { float Y=Tv_Y( y );
    int y1=int( Y ),
        yi=(Tv_port.bottom-y)*L+1;      // �������� � ������������
    if( y1>Ny )break; else              // -- ������ ����������
    if( y1<0 )y=Tv_y( y1=0 );           // ++ ��� �������� ������
    if( y1>=y2 )                        // y1 ��������� �����
    { float *Z=B; B=A; A=Z;             // ������ ������� �������� �����
      while( y1>=y2 )                   // �� ���������� ����������� ������ �
      { if( Typ )fread( B,4,Nx,Grd ); else
        for( int x=0; x<Nx; x++ )fscanf( Grd,"%g",B+x ); ++y2;
    } }
    if( First ){ First=false; y2=y1; ++y; } else // y++ ���������� ������ �����
    for( int x=Tv_port.left; x<=Tv_port.right; x++ )
    { float X=Tv_X( x );
      int x1=int( X );
        if( x1<0 ){ x=Tv_x( x1=0 ); X=Tv_X( x ); } else
        if( x1>Nx )break;
      int x2=x1+1,d;
        if( x2>=Nx )x1=( x2=Nx-1 )-1;
      float W = X-x1,
            V = Y-y1,
            U = (A[x1]*(1-W) + A[x2]*W)*(1-V)
              + (B[x1]*(1-W) + B[x2]*W)*V;   // ��������������� �� �������
      if( U<=0.0 )d=71+int( U*0.0072 ); else // ������� � 10000-��� �����������
                  d=72+int( U*0.048 ); //sqrt( U ) // 1.125 // 0.72*(0.75^2)/0.48
      U = tanh( ( ( A[x2]-A[x1] )*( 1-V ) + ( B[x2]-B[x1] )*V  // ��� ����
                - ( B[x1]-A[x1] )*( 1-W ) - ( B[x2]-A[x2] )*W ) / 600 );
      C = sea[ max( 0,min( 151,d ) )];
      C.b += U * ( U>0 ? 255-C.b : C.b );        // ������������ �������
      C.g += U * ( U>0 ? 255-C.g : C.g );        // ������������ � ������
      C.r += U * ( U>0 ? 255-C.r : C.r );        //         ( � ���������� )
      ((unsigned*)Scr)[yi+x-Tv_port.left] = C.i; // putpixel( x,y,sea[d].i );
    }
  } Tv_PutImage( Tv_port.left,Tv_port.top,Scr );
    Allocate( 0,Scr );
    Allocate( 0,B );
    Allocate( 0,A );
}
#define C2( a,b ) a + ( b<<8 )

void Read_GRD( const char *Name, const Field &Ft )
{
 char *GRD_Name=fext( strdup( Name ),"grd" );
  if( (Grd=fopen( GRD_Name,"rb" ))!=NULL )
  { Field Fn;
    double w, BaseX,BaseY,    // ���������� ������ ������� ����
              StepX,StepY,    // ��� ��������� ����������
              Wmin,Wmax;      // ���������� ��������������� � Topography
    if( get2( Grd )==C2( 'D','S' ) )
    { Typ=get2( Grd );        // ������ ����� ��������� �����-�����
      if( Typ==C2( 'B','B' ) )Typ=4; else
      if( Typ==C2( 'A','A' ) )Typ=0; else Typ=-1;
    }
    if( Typ!=-1 )
    { if( Typ )                 //
      { Nx=get2( Grd );         // ������ ��������� Grd-�����
        Ny=get2( Grd );         //
        fread( &BaseX,8,1,Grd ); fread( &StepX,8,1,Grd );
        fread( &BaseY,8,1,Grd ); fread( &StepY,8,1,Grd );
        fread( &Wmin, 8,1,Grd ); fread( &Wmax, 8,1,Grd );
      } else fscanf( Grd,"%d%d%lf%lf%lf%lf%lf%lf",
                     &Nx,&Ny,&BaseX,&StepX,&BaseY,&StepY,&Wmin,&Wmax );
      Beg=ftell( Grd );
      w=(StepY-BaseY)/Ny; Fn.Jy=(Ft.Jy-BaseY)/w; Fn.Ly=(Ft.Jy+Ft.Ly-BaseY)/w-Fn.Jy;
      w=(StepX-BaseX)/Nx; Fn.Jx=(Ft.Jx-BaseX)/w; Fn.Lx=(Ft.Jx+Ft.Lx-BaseX)/w-Fn.Jx;
      /// -- ����� ��� ������ �������� ��������� � ��������� ���������
      //
      Palette();
      Tv_place( 0,&Fn ); Topography();
      Tv_place( 0,&Fn );
    } fclose( Grd );
  }   free( GRD_Name );
}
