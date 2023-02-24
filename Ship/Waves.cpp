#include "..\Graphics\Tv_graph.h"
#include "..\Graphics\Tf_Topo.h"
//
// 1++ ������� �������� ��������� � ����������� �� ������������� ���������
// 2++ ������� ������ �� ���� ���� ������� ������ � ��������� �� � Surfer-�
// 3++ ������� ������� ���� � �������� � ����� ��������
// 4++ �������� � �������� ������� ����������
//
const double            // D2R=M_PI/180.0,
       R2 =M_PI*2.0;    //
static bool GrId=true;  // ������� ��������� ��������� ����
                        //
static Point Gerstner   // �������� ��������� ����� (1756-1832)
( double L,             // ����� �����
  double F,             // �������� ���������� �� ����� ������� ����
  double Z=0.0,         // �������� ������� ����
  double A=1.0,         // ����������� ���������, ������������ ����.���������
  bool GrId=false       // ������� ��������� ���������
)                       // =====
{ Point P;              //
  A*=1.134*L/R2/2;      // ������ �������� - �������� *1.134/2
  F*=R2/L;              // ������ ���� � ��������
//Z += A*sin( F );      // ��������� ������ �������
  P.y=  Z     + A*exp( (Z+A*cos( F )-A)*R2/L )*cos( F );
  P.x= F*L/R2 - A*exp( (Z+A*cos( F )-A)*R2/L )*sin( F );
  if( GrId )
  { A/=1.286;
    F*=0.8888889;       // ������ ���� � ��������
    P.y += A*exp( (Z+A*cos( F )-A)*R2/L )*cos( F );
    P.x -= A*exp( (Z+A*cos( F )-A)*R2/L )*sin( F );
  } return P;
}                   //
static double Group // �������� ��������� ���� � �������� ������
( double Lw,        // ����� �����
  Point P,          // �������� ���������� �� ����� ������� ����
  double Zw   =0.0, // �������� ������� ����
  double Aw   =1.0, // ����������� ���������, ������������ ����.���������
  double Angle=0.0  // ����������� ��������������� ����
){
 Basis Bw;
 static Point Q={ 0,0 },R={ 1,1 };
 double A,W,V,c1,c2;
    R.y=0.666667/Aw;
    Bw.Euler( Q,Angle,R );
    P = Bw.Goint( P );
    A = Aw*Lw/R2;
    if( GrId )
    { c1 = cos( (P.x+P.y)*R2/Lw/17 ),
      c2 = cos( (P.y-P.x)*R2/Lw/17 ); if( c1*c2<0 )P.x+=Lw/2;
      A *= fabs( c1*c2 );
    }
    V = P.x*R2/Lw;
    W = P.x + A*exp( (Zw+A*cos( V )-A)*R2/Lw )*sin( V );
    P = Gerstner( Lw,W,Zw,A*R2/Lw,0 );
 return P.y;
}
static void Draw_Wave();     // ����� �������� �����
static void Draw_Profile0(); // ������������ ������
static void Draw_Profiles(); // ������� ���� �� ������������� �����������
static void Draw_TopoLine(); // ���������� �������� � �������� ����
static void Waves_Write();   // ������ ������� GRD ������
                                      //
static Field Basin={ 0,0,1000,1 };    // ������ ����� �������� � ��� ������
                                      //
static double Lw=60, Ls=100, Lc=160,  // ����� �����
             Aw=1.0, As=0.67,Ac=0.2,  // �������� ��������� �����
             Cw=10,  Cs=-10, Cc= 50,  // ����������� ��������������� �����
             Zmin=0.0, Zmax=0.0,      // ���������� ������������ ����
             Minw=0.0,Maxw=0.0,Mins=0.0,Maxs=0.0,Minc=0.0,Maxc=0.0,
           **Mm=NULL;                 // ������� ����� ���� � ����
static int   Lt=1920,Bt=1440,         // ����� � ������ ����������� �� ������
             MemField=0;   //         // ������� ���������� �������� �����
                          //==========
static void  AllocMem()  // ������������� ������
{ //if( Mm==NULL )
    Mm=(double**)Allocate( Bt,Lt*sizeof( double ),Mm );
}                       //
static void FieldSize() // ������������� ���������� ��������� �������
{ Field Br;             //
  Basin.Ly = Bt * Basin.Lx / Lt;
  Basin.Jy =-0.68*Basin.Ly;
  Basin.Jx =-0.32*Basin.Lx; Br=Basin; Tv_place( 0,&Br ); Tgo( 1,1 );
  setviewport( 0,0,Tv_port.left,Tv.mY,1 ); clear();
  color( LIGHTGREEN ); Tprintf( W2D( " �����\n" ) ); Tv.Height( 8 );
  color( LIGHTGRAY );  Tprintf( W2D( "%6.0lf�\n" ), Lw );
  if( Minw==Maxw )     Tprintf( W2D( "%6.2lf�\n" ), Aw*Lw/R2*0.75 );
            else     { Tprintf( W2D( "%6.2lf�\n" ), Maxw );
                       Tprintf( W2D( "%6.2lf�\n" ), Minw );
  color( LIGHTGREEN ); Tprintf( W2D( "%6.2lf�\n" ), Maxw-Minw );
  color( LIGHTGRAY );} Tprintf( W2D( "%6.2lf\"\n" ),sqrt( Lw*R2/9.8106 ) );
                       Tprintf( W2D( "%6.0lf�\n" ), Cw ); Tv.Height( 0 ); Tln();
  color( LIGHTCYAN );  Tprintf( W2D( "���� 1\n" )     ); Tv.Height( 8 );
  color( LIGHTGRAY );  Tprintf( W2D( "%6.0lf�\n" ), Ls );
  if( Mins==Maxs )     Tprintf( W2D( "%6.2lf�\n" ), As*Ls/R2*0.75 );
            else     { Tprintf( W2D( "%6.2lf�\n" ), Maxs );
                       Tprintf( W2D( "%6.2lf�\n" ), Mins );
  color( LIGHTCYAN );  Tprintf( W2D( "%6.2lf�\n" ), Maxs-Mins );
  color( LIGHTGRAY );} Tprintf( W2D( "%6.2lf\"\n" ),sqrt( Ls*R2/9.8106 ) );
                       Tprintf( W2D( "%6.0lf�\n" ), Cs ); Tv.Height( 0 ); Tln();
  color( CYAN );       Tprintf( W2D( " ���� 2\n" )     ); Tv.Height( 8 );
  color( LIGHTGRAY );  Tprintf( W2D( "%6.0lf�\n" ), Lc );
  if( Minc==Maxc )     Tprintf( W2D( "%6.2lf�\n" ), Ac*Lc/R2*0.75 );
            else     { Tprintf( W2D( "%6.2lf�\n" ), Maxc );
                       Tprintf( W2D( "%6.2lf�\n" ), Minc );
  color( CYAN );       Tprintf( W2D( "%6.2lf�\n" ), Maxc-Minc );
  color( LIGHTGRAY );} Tprintf( W2D( "%6.2lf\"\n" ),sqrt( Lc*R2/9.8106 ) );
                       Tprintf( W2D( "%6.0lf�\n" ), Cc ); Tln(); Tln();
  if( Zmin!=Zmax )
  { color( YELLOW );   Tprintf( W2D( "�������\n" ) );
    color( LIGHTGRAY); Tprintf( W2D( "%6.2lf�\n" ), Zmax );
                       Tprintf( W2D( "%6.2lf�\n" ), Zmin );
    color( YELLOW );   Tprintf( W2D( "%6.2lf�\n" ), Zmax-Zmin );
  }
  setviewport( 0,0,Tv.mX,Tv.mY,0 );
  Tv.Height( 0 );
  Tv_rect();
}
#define Exterm( A,Min,Max ){ if( Min>A )Min=A; else if( Max<A )Max=A; }
#define ExZero Minw=Maxw=Mins=Maxs=Minc=Maxc=Zmin=Zmax=0.0
//
//   � ������� ��������� ����������� ����� � ������������ �������
//
int main()
{
 static field Tr={ -64,-20,0,0,1 };
 static Mlist Menu[]
 ={ { 1,0,"\t  ��������� �������� ��������"    }
  , { 1,0,  "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~"    }
  , { 1,0,"\t             ����� ����-1 ����-2" }
  , { 1,5,"\t����� �����  %5.0lf",&Lw},{0,5,", %5.0lf",&Ls},{0,5,", %5.0lf",&Lc}
  , { 1,5,"\t����������� %5.0lf",&Cw},{0,5,", %5.0lf",&Cs},{0,5,", %5.0lf",&Cc}
  , { 1,5,"\t������ [1]   %5.2lf",&Aw},{0,5,", %5.2lf",&As},{0,5,", %5.2lf",&Ac}
  , { 1,6,"\t����� ���� ������� [�] %6.0lf",&Basin.Lx }
  , { 1,26 }
  };
 int ans=0xF3;
  Tv_Graphics_Start();
  setwindowtitle( "Waves - �������� ������� � ����������� ���������� ����" );
  Tv_place( &Tr );
  Bt=Tv_port.bottom-Tv_port.top+1;
  Lt=Tv_port.right-Tv_port.left+1; FieldSize();
  for(;;)
  { switch( ans )
    { case 0xF1: ExZero;
           do
           { if( ans==13 )GrId^=1;
             Menu[13].Msg=(char*)(GrId?"\t��������� ��������� �����":"\t������� ������������� �����");
           } while( (ans=Tmenu( Mlist( Menu ),1,-1 ))==13 );
                 MemField=0;
                 FieldSize();     continue;
      case 0xF2: Waves_Write();   break;
      case 0xF3: Draw_Wave();     break;
      case 0xF4: Draw_Profile0(); break;
      case 0xF5: Draw_Profiles(); break;
      case 0xF6: Draw_TopoLine(); break;
      case 0xFA: Tv_bar();        break;
      case _Esc: Ghelp( "?Are you sure:  y/n" ); ans=Tv_getc()|0x20;
                 Ghelp();
                 if( ans=='y' || ans==_Enter )return 0;
     default   : Tv_bell();
    }
    Thelp( "%d ��, F1-���������%s, F3-�����, F4-������, F5-�������%s, F10-�������",
        coreleft()>>20,MemField?", F2-������":"",MemField?", F6-��������":"" );
    ans=Tv_getc();
    if( ans!=0xF1 )Ghelp();
  }
}
//
// ����������� �������� �����
//
static void Draw_Profiles()
{  int j,k;
 Point P;
 double W,S,C;
  if( !MemField ){ Tv_bar(); ExZero; }

  color( LIGHTGRAY );  line( Basin.Jx,0.0,Basin.Jx+Basin.Lx,0.0 );
  AllocMem();          line( 0.0,Basin.Jy,0.0,Basin.Jy+Basin.Ly );

  for( k=0; k<Bt && Tv_getw()!=_Esc; k++ )
  for( j=0; j<Lt;                    j++ )
  { P.x=Basin.Jx+j*Basin.Lx/Lt;
    P.y=Basin.Jy+k*Basin.Ly/Bt;
    if( !MemField )
    {    W=Group( Lw,P,0,Aw,Cw ); Exterm( W,Minw,Maxw );
         S=Group( Ls,P,0,As,Cs ); Exterm( S,Mins,Maxs );
         C=Group( Lc,P,0,Ac,Cc ); Exterm( C,Minc,Maxc );
             Mm[k][j]=( W+=C+S ); Exterm( W,Zmin,Zmax );
    } else W=Mm[k][j];
    if( k%4==0 )
    { color( W<0.0?GREEN: (W>0.0?CYAN:WHITE) );
      if( P.x!=Basin.Jx )lineto( P.x,W+P.y );
                    else moveto( P.x,W+P.y );
    }
  } FieldSize();
    MemField=1;
}                           //
static void Draw_TopoLine() // ���������� �������� � �������� ����
{                           //
 int  k;
  if( MemField ){       Tf_List();
    for( k=6; k>0; k-- )Tf_List( k*Aw*Lw/R2/6,LIGHTCYAN ),
                        Tf_List( k*Aw*Lw/R2/-6,LIGHTGREEN );
                        Tf_List( 0.0,WHITE  );
                        Tf_Topo<double>( Mm,0,0,Bt,Lt,true );
} }
//
// ������� ���������� �������� ����
//
static void Draw_Profile0()
{                        //
 double W,S,C,Zw;        // �������, �� ������� ������� ���������
 Point P={ 0,0 };        //
  Tv_bar();  color( YELLOW );
  ExZero;    line( 0.0,Basin.Jy,0.0,0.0 );
  for( Zw=0; Zw>Basin.Jy && Tv_getw()!=_Esc; Zw-=Basin.Ly/Bt*4 )
  for( P.x=Basin.Jx; P.x<Basin.Jx+Basin.Lx; P.x+=Basin.Lx/Lt )
  { W = Group( Lw,P,Zw,Aw,Cw ); if( !Zw )Exterm( W,Minw,Maxw );
    S = Group( Ls,P,Zw,As,Cs ); if( !Zw )Exterm( S,Mins,Maxs );
    C = Group( Lc,P,Zw,Ac,Cc ); if( !Zw )Exterm( C,Minc,Maxc ); W+=S+C;
                                if( !Zw )Exterm( W,Zmin,Zmax );
    W-=2*Zw;
    color( fabs(W-Zw)/Aw<0.01?LIGHTBLUE:W<Zw?LIGHTGREEN:LIGHTCYAN );
    if( P.x!=Basin.Jx )lineto( P.x,W );
                  else moveto( P.x,W );
  } FieldSize();
}
static void Draw_Wave()
{                    //
 double Zw;          // �������, �� ������� ������� ���������
 Point P={ 0,0 },Q;  //
  ExZero;
  Tv_bar();
  color( YELLOW );   line( 0.0,Basin.Jy,0.0,0.0 );
  if( GrId )
  { color( DARKGRAY );
    for( P.x=Basin.Jx; P.x<Basin.Jx+Basin.Lx; P.x+=Basin.Lx/Lt )
    { Q=Gerstner( Lw*3.176472,P.x,0,Aw,0 );
          Exterm( Q.y/3.176472,Zmin,Zmax ); Q.y+=3*Aw*Lw/R2;
      if( P.x>Basin.Jx )lineto( Q );
                   else moveto( Q );
  } }
  for( P.x=Basin.Jx; P.x<Basin.Jx+Basin.Lx; P.x+=Basin.Lx/Lt*16 )
  for( Zw=0; Zw>Basin.Jy && Tv_getw()!=_Esc; Zw-=Basin.Ly/Bt )
  { Q = Gerstner( Lw*3,P.x,Zw,Aw,GrId );
    color( fabs(Q.y-Zw)/Aw<0.01?BLUE:Q.y<Zw?GREEN:CYAN );
    if( Zw )lineto( Q );
       else moveto( Q );
  }
  for( Zw=0; Zw>Basin.Jy && Tv_getw()!=_Esc; Zw-=Basin.Ly/Bt*16 )
  for( P.x=Basin.Jx; P.x<Basin.Jx+Basin.Lx; P.x+=Basin.Lx/Lt )
  { Q = Gerstner( Lw*3,P.x,Zw,Aw,GrId );
    color( fabs(Q.y-Zw)/Aw<0.01?LIGHTBLUE:Q.y<Zw?LIGHTGREEN:LIGHTCYAN );
    if( !Zw )Exterm( Q.y/3,Minw,Maxw );
    if( P.x>Basin.Jx )lineto( Q );
                 else moveto( Q );
  } FieldSize();
}
//
// ������ ������� GRD ������
//
static void Waves_Write()
{ int j,k;
 static int N=0;
 FILE *M,*W,*S,*C;
 Point P;
 char Nm[MAX_PATH];
  if( !MemField )return;     ++N;
  sprintf( Nm,"%02d_Sea.grd",  N ); if( !(M=fopen( Nm,"wt" )) )goto Fin;
  sprintf( Nm,"%02d_Wave.grd", N ); if( !(W=fopen( Nm,"wt" )) )goto Fin;
  sprintf( Nm,"%02d_Swell.grd",N ); if( !(S=fopen( Nm,"wt" )) )goto Fin;
  sprintf( Nm,"%02d_Swel2.grd",N ); if( !(C=fopen( Nm,"wt" )) )goto Fin;
  sprintf( Nm,"DSAA\n%d %d\n%.6lg %.6lg\n%.6lg %.6lg\n%.6lg %.6lg\n",
           Lt,Bt,Basin.Jx,Basin.Jx+Basin.Lx,
                 Basin.Jy,Basin.Jy+Basin.Ly,Zmin,Zmax );
  fputs( Nm,M );
  fputs( Nm,W );
  fputs( Nm,S );
  fputs( Nm,C );
  for( k=0; k<Bt; k++ )
  for( j=0; j<Lt; j++ )
  { P.x=Basin.Jx+j*Basin.Lx/Lt;
    P.y=Basin.Jy+k*Basin.Ly/Bt;
    fprintf( M,"%.5lg%s",Mm[k][j],             j==Lt-1?"\n":" " );
    fprintf( W,"%.5lg%s",Group( Lw,P,0,Aw,Cw ),j==Lt-1?"\n":" " );
    fprintf( S,"%.5lg%s",Group( Ls,P,0,As,Cs ),j==Lt-1?"\n":" " );
    fprintf( C,"%.5lg%s",Group( Lc,P,0,Ac,Cc ),j==Lt-1?"\n":" " );
    if( !j )Thelp( "������: %.1lf% ",100.0*double(k)/Bt );
  } Fin:; //fcloseall();
}
#if 0
-fno-exceptions
-ffast-math
-fno-trapping-math
-fno-enforce-eh-specs
-funsafe-math-optimizations
-mthreads
#endif // 0
