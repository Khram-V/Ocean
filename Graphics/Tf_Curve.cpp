//
//	Curve.cpp				       /90.10.10-99.03.31/
//	��������� ������������ ������ �������� ��� ������ ��������� ������
//
//	-- ���������� ������� ���������� ��� ����������
//	   �� �������� ������� ��� ������������ ����� (Lf)
//
//
#include <StdIO.h>
#include "..\Graphics\Tv_Graph.h"

static FILE *Fs;	    //
static int T=2,		    // ��� ������
       Nx,		    // ����� �������� ������ � ����� ��� � ������
       Jy=0,Jx=0,Ly=0,Lx=0; // ������ ��� �������
//
// ------------------ ���������� �������� --------------------------------
//
//     ���������� � ������������ ���� ����� � ����������� ������
//
static float* NextLine( float Level, int y )
{  int W,k,x;
 float V,*R;
 static float **Height; // ��� �������������� ������ � ������
  if( !y )
  { if( Jy )
    { if( !T )for( k=0; k<Jy; k++ )
	      for( x=0; x<Nx; x++ )fscanf( Fs,"%f",&V );
       else fseek( Fs,Nx*long( T )*Jy,SEEK_SET );
    } Height=(float**)Allocate( 2,Nx*sizeof(float) );
  }   else if( y<0 ){ Allocate( 0,0,Height ); return NULL; }
  R = Height[y&1];
  for( k=x=0; k<Nx; k++ )
  { if(  !T  )fscanf( Fs,"%f",&V ); else
    if( T<4 ){ fread( &W,T,1,Fs ); V=T==1?byte( W ):W; } else
	       fread( &V,4,1,Fs );
    if( y>=0 && k>=Jx && x<Lx )R[x++]=V-Level-1e-6;
  } return R;
}
//struct __ListLine: public IsoLine
//{ int M;			// ����� ������ ����� � ������ �������
//  __ListLine(){ N=0; L=NULL; T=NULL; Z=0.0; Y=X=NULL; M=0; }
//};		  //
struct sLine	  // ���������� ����������� ��� ���������� �����
{		  //
 byte Type;	  // 0-��������� ������; 2-��������� �����
 int  Length,	  // ����� �����
      MaxLength;  //   (� ������ �������)
 float *Y,*X;	  // ���������� ��������� ����� (�� 16 ���.�����)
 sLine *Lc;	  // ����� ����������� �����
  void Allocate( fixed len )
     { Y=(float*)::Allocate( len*sizeof( float ),Y ); MaxLength=len;
       X=(float*)::Allocate( len*sizeof( float ),X );
     }
  sLine(){ Lc=NULL; Y=X=NULL; Length=MaxLength=0; Type=0; Allocate( 4 ); }
 ~sLine(){ if( MaxLength>0 )Allocate( 0 ); }
  sLine* Rev();
  sLine* Add( float _Y, float _X )
  { if( Length>=MaxLength )Allocate( Length+36 );
      Y[Length]=_Y;
      X[Length]=_X; ++Length; return this;
} };
sLine* sLine::Rev()
{ float Z;
  for( int k=0,n=Length-1; k<n; k++,n-- )Z=X[k],X[k]=X[n],X[n]=Z,
					 Z=Y[k],Y[k]=Y[n],Y[n]=Z;
  return this;
}				    //
static int Ln=0,Lm=0,Lf=0;   	    // ���������� ������� ����� (�� ������.)
static sLine **LineList=NULL,**_Dn, // ������ ����� � ����� ������� �����
	     *Up=0,*Left,*Right;    // �������� ������ ��� ����� �����
				    //
static void Compress_Lines( const int lmax=0 )  //
{ int k,j;                                      // ������� ������ ��������,
  for( k=j=Lf; k<Ln; k++ )                      // � ������������� ����������
  if( LineList[k] )                             // �� ��������
  { if( LineList[k]->Length>lmax )	        //
    { if( k!=j )LineList[j]=LineList[k]; ++j;
    } else{ delete LineList[k]; LineList[k]=NULL; }
  } Ln=j;
}
static void Address_Lines( sLine *A, sLine *B )
{ int k;
  if( Up   ==B )Up   =A;
  if( Left ==B )Left =A;
  if( Right==B )Right=A;
  for( k=0; k<Lx-1; k++ )if( _Dn[k]==B )_Dn[k]=A;
  for( k=Lf; k<Ln;  k++ )if( LineList[k]->Lc==B )LineList[k]->Lc=A;
  B->Length=0; Compress_Lines();
}					 	//
static void Plus_Line( sLine* A, sLine* B )	// ����������� �����
{					 	//
 double st=hypot( B->Y[0]-A->Y[A->Length-1],B->X[0]-A->X[A->Length-1] );
  if( A==B )
  { if( st>0.0 && st<=M_SQRT2 )A->Add( A->Y[0],A->X[0] ); A->Lc=0;
  } else
  { for( int i=(st==0.0); i<B->Length; i++ )A->Add( B->Y[i],B->X[i] );
    A->Type += B->Type;
    Address_Lines( A,B );
} } 					 	//
static void Join_Line( sLine* A, sLine* B ) 	// ����������� �����
{ if( B->Lc )				 	//
    { Left=B->Lc; Plus_Line( A,B->Rev() ); if( Left )Plus_Line( A,Left );
    } else
  if( A->Lc )
    { Left=A->Lc; Plus_Line( B,A->Rev() ); if( Left )Plus_Line( B,Left );
    } else
  if( A->Type )Plus_Line( B,A->Rev() ); else
  if( B->Type )Plus_Line( A,B->Rev() );
}				    //
static sLine* New_Line( byte Tp=0 ) // 1 - ��������� �����, 0 - ����������
{			     	    //
  if( Ln>=Lm )LineList=(sLine**)Allocate( (Lm=Ln+12)*sizeof(sLine*),LineList );
	 LineList[Ln]=new sLine;
	 LineList[Ln]->Type=Tp; ++Ln;
  return LineList[Ln-1];
}
static void Check_Lines()
{ int k;
  for( k=Ln-1; k>=0; k-- 	)
  if( LineList[k] 		)
  if( LineList[k]->Type<2 	)
  if( LineList[k]->Length>0 	)
  if( LineList[k]->Lc           )
  if( LineList[k]->Lc->Type<2   )
  if( LineList[k]->Lc->Length>0 )
    { Plus_Line( LineList[k]->Rev(),LineList[k]->Lc ); } Compress_Lines( 1 );
}
IsoLine *Tf_curve
( float Level,
   void *_Fs,	    //
    int _T,	    // ��� ������
    int _Nx,	    // ����� �������� ������ � ����� ��� � ������
    int _Jy, int _Jx, int _Ly, int _Lx ) // ������ ��� �������
{
 fixed x,y;
  Fs=(FILE*)_Fs; T=_T; Nx=_Nx; Jy=_Jy; Jx=_Jx; Ly=_Ly; Lx=_Lx; rewind( Fs );
//
//			���������� ��������
 double Y,X;
 float *_D,*_1,*_2;	_D=_2=NextLine( Level,0 );
 fixed iUp, iRight;	_Dn=(sLine**)Allocate( (Lx-1)*sizeof( sLine* ) );
  for( x=0; x<Lx-1; x++,_2++ )
   if( _2[0]*_2[1]>0 )_Dn[x]=NULL;
		 else _Dn[x]=New_Line(1)->Add( 0,x+_2[0]/( _2[0]-_2[1] ) );
  for( y=0; y<Ly-1; y++ )
  {     _1=_D;
	_2=_D=(float*)NextLine( Level,y+1 );
    if( _1[0]*_2[0]>0 )Right=NULL;
		  else Right=New_Line(1)->Add( y+_1[0]/( _1[0]-_2[0] ),0 );
    for( x=0; x<Lx-1; _Dn[x++]=Up )
    { Left=Right;
      Up=Right=NULL;
      _1++;
      if( _2[0] || _2[1] )
      if( (iUp=((X=_2[0]*_2[1])<=0))!=0 )
      { if( X<0 )X=_2[0]/( _2[0]-_2[1] ); else if( !_2[0] )X=0; else X=1;
      }
      _2++;
      if( _1[0] || _2[0] )
      if( (iRight=((Y=_1[0]*_2[0])<=0))!=0 )
      { if( Y<0 )Y=_1[0]/( _1[0]-_2[0] ); else if( !_1[0] )Y=0; else Y=1;
      }
      if( !iUp && !iRight && Left && _Dn[x] )Join_Line( _Dn[x],Left ); else
      if( iUp && iRight && !Left && !_Dn[x] )
      { Up =  New_Line()->Add( y+1,x+X );
	Right=New_Line()->Add( y+Y,x+1 ); Right->Lc=Up; Up->Lc=Right;
      } else
//    if( iUp || iRight )
      {	if( iUp  && _Dn[x]             )Up   =_Dn[x]->Add( y+1,x+X );
	if( iRight && Left             )Right= Left ->Add( y+Y,x+1 );
	if( !Up && iUp && Left         )Up   = Left ->Add( y+1,x+X );
	if( !Right && iRight && _Dn[x] )Right=_Dn[x]->Add( y+Y,x+1 );
      }	if( Right && x==Lx-2 )Right->Type++;
	if(  Up  &&  y==Ly-2 )   Up->Type++;
    }   if( Tv_getw() )break;
  }	NextLine( 0,-1 ); // ������� ������
	Check_Lines();	  // � ������������� �������� ������
//
//	���������� ���������� ������ �������� � ������� IsoLine
//
 static IsoLine Ls;			       //
  Ls.N=Ln;      Allocate( 0,0,Ls.Y );  Ls.Y=0; // ������� �����, �������
  Ls.Z=Level;   Allocate( 0,0,Ls.X );  Ls.X=0; // ���������� �������������
  Ls.L=   (int*)Allocate( Ln*sizeof(int), Ls.L );
  Ls.T=  (byte*)Allocate( Ln*sizeof(byte),Ls.T );
  Ls.Y=(float**)Allocate( Ln,0,           Ls.Y );
  Ls.X=(float**)Allocate( Ln,0,           Ls.X );
  for( y=0; y<Ln; y++ )
  { Ls.T[y]=LineList[y]->Type;   LineList[y]->MaxLength=0;
    Ls.L[y]=LineList[y]->Length; LineList[y]->Length=0;
    Ls.Y[y]=LineList[y]->Y;	 LineList[y]->Y=0;
    Ls.X[y]=LineList[y]->X;	 LineList[y]->X=0; delete LineList[y];
  } Allocate( 0,LineList );      LineList=NULL; Ln=Lm=0;
    Allocate( 0,_Dn );
  return &Ls;
}
