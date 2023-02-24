//
//      ������� �������������� ������������ ���� ��
//      ������������� ��������� ���� ������������ �����
//
//                              ������ 2.4 �� 11 ������ 1993 ����.
//                              ������� ��������, �������
#include "..\Graphics\Tv_graph.h"

const Real D2R=M_PI/180.0;
const int Blen=64000;       // ����� ������� ��������� ���������� �����

Real  DCy=12,DCx=12,        // ������ ������� ������
       dY=0.5,dX=0.5;       // ������������ � ��������������
Field F={ -15,-10,30,20 };  // ������ � ��� ���������� ����

static Real  dCy=0.2,       // ������������ �
             dCx=0.2,       //     �������������� ��� ������������ �������
             dL =0.2,       // ��� ������ ����� �������
             FD =32.0,      // ���� ����������� ������ � ��������
             Vm,Vd;         // ����������� ���������� ��� ���� ��������
static Point P,             // ������ ������� ������ ��� �����
             V,W,           // ����������� ������ ��������
             D,             // ������ ����������� �������� ������
             M;             // ��������� ������ ��� ����������.
static int Nb=0,            // ������� ����� ��� ���� ���������� �����
         Ny,y,Nx,x;         // ���������� ������� ���� ��������� ���������
                            // �������� ���������� �������� � ������������ ����
static byte FMod=3,         // �������������� ������ �� �������+���������������
                            // 0 - ����������� �� ������ ������� �� ��������
                            // 1 - ������ � ���������(�������������) ����������
                            // 2 - ������ � �������������(��������) ����������
            FVid=0,         // ���� ����������� ��������� � ������ �����������
                            // 1 - ���������� ������� �� ���������� �������
                            // 2 - ������� ������������� ������ ��������� �����
             **BF=NULL;     // ������ ��������� ��� ���������������� �����
static Point **EF=NULL,     // ������ ��� ���������� �������� ���������� ������
           Body[Blen],      // ���������� ����� ���������� ���������� ������
           Flow[Blen];      // ����������� ������ � ���� ������
void Curve_View();
void Curve_Show(); // Real&,Real&,Real&,Real& );
int  Curve_Access( Point );
Real Curve_Normal( Real D );            // D - ������������ ����� �������
                                        //   - �� ������ ������ ����� �������
int  Curve_Normal( Point& R,Point& N ); // R - ����� ������������ �������
                                        // N - ������� � ������� �� �������
void Fletcher                // ����� ������� - C.A.J.Fletcher
( int   _N,                  // ���������� ����� ������� (��� ����������)
  Point *P,                  // ���������� ������� ����� ����������� ����
  Point _V,                  // ���������� �������� ����������� ������
  Point *V=0,                // ���������� ��������� �������� � ����������
  Point *Q=0                 //    � ��������� �������� ���������
);
Point Fletcher( Point );     // ������� ��������� �������� � ������������ �����
//
//      ���������� ��������� ����� � ��������
//      SM - 0x0001 - ������� ���������� ����� ����
//           0x0002 - ������ ��������� ����
//           0x0004 - ���������� ���� �������
//           0x0010 - ��������� ���� ������������ � ���������
//           0x0020 - ������������� ���� ��������
//           0x0100 - Y: ������� ���� �������
//                    N: ���� ��������� ���������
//
static void Draw( Real Scale, short SM=0x8002 )
{ { Field _f=F; Tv_place( 0,&_f ); }
  Ghelp(); clear(); Tv.Height( 8 );                         color( LIGHTBLUE );
  Tgo( 98,-3 ); Tprintf( W2D( "(c)1993-99, �.��������" ) ); color( CYAN );
  Tgo( 99,-2 ); Tprintf( W2D( "���.�������������� �������������" ) );
  Tgo( 99,-1 ); Tprintf( W2D( "����������� ��������������� �����������" ) );
  color( LIGHTGRAY ); Tv_axis( true ); Tv.Height( 0 );
  color( LIGHTCYAN ); Tv_rect();              // �������� ������������� �����
  if( FMod&1 || SM&2 || !FMod )               // ����������� ���������� �������
  { Curve_Normal( dL );  Curve_View();        // � ����� ���������� �����������
  }
  if( FMod&2 && SM>0 )                     // ���������� ������������� ��������
  { color( BLUE );                         // ���������� ������ ������� ������
    for( int k=0; k<Nb; k++ )
    { Point &Q=Body[k]; rectangle( Tv_x( Q.x-dCx/2 ),Tv_y( Q.y-dCy/2 ),
                                   Tv_x( Q.x+dCx/2 ),Tv_y( Q.y+dCy/2 ) );
    }
    color( LIGHTGRAY );                 // �������� ������� ������� �����������
    if( BF )                            // ��������� ������ ���������� �������
     for( P.y=F.Jy+dY,y=0; y<Ny; P.y+=dY,y++ )
     for( P.x=F.Jx+dX,x=0; x<Nx; P.x+=dX,x++ )if( BF[y][x] )Tv_cross( P,-2 );
  }
  color( SM&0x100?LIGHTGREEN:LIGHTCYAN );
  if( SM>=0 )                                 // ��� ������� �� �����
  for( P.y=F.Jy+dY,y=0; y<Ny; P.y+=dY,y++ )   // ������������ ����
  for( P.x=F.Jx+dX,x=0; x<Nx; P.x+=dX,x++ )   // � ����� EF[Ny][Nx]
  if( SM&4 || BF[y][x]==0 )
  { W=( SM&0x100 ? EF[y][x]/Vd-D:EF[y][x]/2 )*Scale; needle( P-W,P+W );
  } color( YELLOW );
  Scale=hypot( F.Ly,F.Lx )/12; needle( -D*Scale,D*Scale );
}
//      ����� ���������� �� �������� �������������
//             <Escape> - ����� � �������
//
static int Interrupt()
{ if( Tv_getw()==_Esc  ){ Thelp("?B ������(Y/N)"); int ans=Tv_getc()|0x20;
      Ghelp(); if( ans=='y' || ans==_Enter  )return 1; } return 0;
}
//    --------------------------------------------------�
//    � ������ ����������� � ���������� �� ������� ���� �
//    L--------------------------------------------------
static void Contour()
{ int j,k,l=1;
 Point P,Q,R,N,V;
 Real  L=Curve_Normal( dL );
  color( YELLOW ); Tgo( 1,1 ); Tprintf( "Lenght=%.1lf\n",L ); Vm=Vd=0;
  color( LIGHTRED );           Tprintf( " Wait Please\n" );
  color( CYAN );
  if( FMod==1 )                // ������ ����� �� �������
  { color( LIGHTMAGENTA );
    for( j=0; j<l; j++ )
    { for( V=0,k=0; Curve_Normal( R,N ); k++ )
      { if( k!=j )
        { if( k==j+1 )Q=R; R-=P; V-=M*N*R/norm( R )/2;
        } else if( !j )P=R;
      } if( !j )l=k; needle( P-V,P+V ); P=Q;
        if( Vd<(V*D).x )Vd=(V*D).x;
        if( Vm<abs( V ) )Vm=abs( V );
    } Tgo( 1,2 ); Tprintf( W2D( "���������:  %4lg\n" ),Vm );
                  Tprintf( W2D( "����������: %4lg\n" ),Vd );
  }
  if( !FMod )           // ��������������� ���������� ��� ������������� �������
  { Nb=0; while( Curve_Normal( R,N ) )Body[Nb++]=R;
    Tgo( 1,1 ); Tprintf( "Lenght[%d]=%.1lf\n",Nb,L );
    Fletcher( Nb,Body,-D,Body+Nb );
    for( k=0; k<Nb; k++ ){ Real M=abs( Body[Nb+k] ); if( Vm<M )Vm=M; } Vd=1;
  }
  if( FMod )Vm=Vd=0;                  // �����-�� ����� ������������ ����������
}
//      ----------------------------------------------------�
//      � ������ ��������� �������� ��������������� ������� �
//      L----------------------------------------------------
static Point Contour( Point& P )
{ Real A,B;
  Point V={ 0,0 },R,N;
         Curve_Normal( dL );
  while( Curve_Normal( R,N ) )
  { if( (A=norm( R-=P ))<(B=norm( N )/2) )A=B; V-=M*N*R/A/2;
  } return V;
}
//      -------------------------------------------------------�
//      � ������ ��������� �������� ��������������� �� ������� �
//      L-------------------------------------------------------
static Point Surface( Point& P )
{ Real Ds=dCy*dCx;
  Point V={ 0,0 },W,E; dCy/=2; dCx/=2;
  for( int k=0; k<Nb; k++ )
  { W=P-Body[k];
    E=FMod&1 ? Flow[k] : M;
    if( abs( W.y )>=dCy || abs( W.x )>=dCx )
//      V+=E*sqr( W )*Ds/pow( norm( W ),1.5 );
        V+=E*sqr( W )*Ds/sqr( norm( W ) );
    else { if( FVid&1 )BF[y][x]=1; V+=E*sqr( W )*16/Ds; }
  } dCx*=2; dCy*=2;
  return V;
}
//      -------------------------------------�
//      �  ������ � ���������� ���� �������  �
//      � � ������������ � ������������ METH �
//      L====================================-
static void Flow_Field()
{
//      ������ ��������� ��������� � �� �����������
//
 Point P,Q;
  BF =(byte** )Allocate( Ny=int( F.Ly/dY-0.5 ),Nx=int( F.Lx/dX-0.5 ),BF );
  EF =(Point**)Allocate( Ny,Nx*sizeof(Point),EF );
 int nY=0,mY=0;
 Real diagonal=hypot( dY,dX );
  if( FMod&2 )FVid|=1; Contour(); color( CYAN );
  if( FMod==3 )                                  // ������� �� "��������"
  { for( int k=0; k<Nb; k++ )
    { Flow[k]=conj( D-(Q=Contour( P=Body[k] )) )/M_PI; line( P,P+Q ); }
  }
  for( P=0;; )
  { if( fixed( y=Ny/2+mY )>=Ny ){ if( nY>=Ny )break; }else
    { ++nY;
           P.y=F.Jy+dY*(y+1);
      for( P.x=F.Jx+dX,x=0; x<Nx; P.x+=dX,x++ )
      {
        BF[y][x]=FVid&1?0:Curve_Access( P ); // 0 - �����, ���� ��� ������

        if( !FMod )V=Fletcher( P ); else
        { if( FMod&1 )V=Contour( P ); else V=0.0;
          if( FMod&2 )V+=Surface( P );
          if( !BF[y][x] ){ if( Vm<abs( V ))Vm=abs( V );
                           if( Vd<(V*D).x )Vd=( V*D ).x; }
        }
        EF[y][x]=V;     // if( Interrupt() )return;
        needle( P-dir( V )*diagonal/2,P+dir( V )*diagonal/2 );
      }
    } if( mY>0 )mY=-mY; else mY=1-mY;
  } FVid&=~1;
  color( YELLOW ); Tgo( 1,2 ); Tprintf( W2D( "���������:  %4lg\n" ),Vm );
  color( WHITE  );             Tprintf( W2D( "����������: %4lg\n" ),Vd );
}
//      +------------------------------------------+
//      � ������� ��������� ��� ��������� �������� �
//      �    ��������� ���� ������������ �����     �
//      -==========================================-
int main()
{ Mlist Menu[] =
  { { 1,0,"\t������� ��������� �������" }
  , { 1,4," Y: %4.2lf",&F.Jy},{0,4,"\t \xB6 %4.2lf",&F.Ly},{0,4,", dY %4.2lf",&dY }
  , { 1,4," X: %4.2lf",&F.Jx},{0,4,"\t \xB6 %4.2lf",&F.Lx},{0,4,", dX %4.2lf",&dX }
  , { 2,4,"\t����������� ������ %4.2lf",&FD  },{ 0,0,"\t�" }
  , { 2,4,"\t��� ����� �������: %4.2lf",&dL  }
  , { 1,4,"\t��������� ���� dY: %4.2lf",&dCy }
  , { 1,4,  "               dX: %4.2lf",&dCx }
  , { 2,0,"\t  �������� ��" },{ 0,10 }
  };
 static field Tr={ -32,-60,0,0,0 };
 int ans=0;
  Tv_Graphics_Start();                    // ��������� �������� �� 256 ������
  setwindowtitle( "Flow - fluidmechanics" );
  Tv_place( &Tr );
  M=conj( (D=(Point){ cos( FD*D2R ),sin( FD*D2R ) })/M_PI );
  void Curve_Analytic( int );
       Curve_Analytic( 2 );
//
//      ������������� ���������� ��������� �������
//
Start: // pattern( BLUE,LIGHTCYAN );
  {
//   static Real ymn=-6,ymx=6, // ������������ �
//               xmn=-6,xmx=6; //  �������������� ������ ����������� ������
   Display T( Mlist( Menu ),1,-1 ); Draw( 1.0 );
    do
    { Thelp( "F4 Figure  F5 Start  %sF10 Exit",EF?"F6 Show  ":"" );
      color( CYAN );
      Menu[13].Msg=(char*)( FMod==2?"\t �������"
                          : FMod==1?"\t �������"
                          : !FMod?"\t �������":"\t ��������" );
      ans=T.Answer();
      switch( ans )
      { case 0xF4: T.Back(); Draw( 1.0 ); Curve_Show(); // ymn,ymx,xmn,xmx );
        case 0xF5: break;
        case 0xF6: if( !EF )continue; goto Request;
        case 0xFa: return 0;
        case   13: (++FMod)&=3;
       default: if( ans>11 )continue;
      }
      M=conj( (D=(Point){ cos( FD*D2R ),sin( FD*D2R ) })/M_PI );
      Nb=0;
      T.Back();
      Draw( 1.0 );
      //
      // ���������� ���� ������������� ��������� ������ ��������� ������
      //
      color( BLUE );
      if( FMod&2 )
      for( P.y=int( F.Jy/dCy-1 )*dCy; P.y<F.Jy+F.Ly; P.y+=dCy )
      for( P.x=int( F.Jx/dCx-1 )*dCx; P.x<F.Jx+F.Lx; P.x+=dCx )
       if( Curve_Access( P ) )
       { rectangle( Tv_x( P.x-dCx/2 ),Tv_y( P.y-dCy/2 ),
                    Tv_x( P.x+dCx/2 ),Tv_y( P.y+dCy/2 ) );
         Body[Nb++]=P;
         if( Nb>=Blen )Break( "��� ������ ��� ������ ���������� �����" );
       }
    } while( ans!=0xF5 ); // ����� � ���������� �������� ��������
  }
//      ���������� ���������� ��������������� ������
//
  { color( LIGHTCYAN );
    Twindow( 5,-1,20,3 );
    if( FMod&2 )
    { color( LIGHTGRAY ); Tprintf( W2D( "����� : %3.1lf (%d)" ),dCy*dCx*Nb,Nb );
    }
  }
  Flow_Field();
//
//!     ����� ������������ �������� �������
Request:
 static Point VS,Vs,VV,Vv,VQ,VP={ 0.0,0.0 };
 static fixed VMode=1;               // ����� ����������� �������������� �����
 static Real VScale=1;               // �������������� ���������� ���������
        Real VSt=Tv_X(1)-Tv_X(0);
  for( ;; )
  { Thelp
    ( "F1 ��������� F2 ���������  F3 ���������  F4 � ����������  <Enter>� ������  <Esc>�����" );
    do
    { setwritemode( XOR_PUT ); VQ=VP;
      if( FMod&2 )
      { Vv=Surface( VP );     color( LIGHTGRAY );
        VV=dir(Vv/Vd-D)*VSt*64; line( VQ,VQ+VV );
        Vv=dir(Vv)*VSt*32;      line( VQ,VQ+Vv );
      }
      if( FMod&1 || !FMod )
      { if( FMod&1 )
        { Vs= Contour( VP ); color( LIGHTMAGENTA ); }else
        { Vs=Fletcher( VP ); color( WHITE );    }
        VS=dir( Vs/Vd-D)*VSt*64; line( VQ,VQ+VS );
        Vs=dir( Vs )*VSt*32;     line( VQ,VQ+Vs );
      }
      ans=Tv_pause( &VP,0,VSt*(Tv_getk()&SHIFT?12:1) );
      if( FMod&1 || !FMod )
      { color( FMod&1?LIGHTMAGENTA:WHITE); line( VQ,VQ+VS ),line( VQ,VQ+Vs ); }
      if( FMod&2 ){ color(LIGHTGRAY);      line( VQ,VQ+VV ),line( VQ,VQ+Vv ); }
      setwritemode( COPY_PUT );
    } while( ans<_Enter );
//
//  ��������� �������� � ���������� �����
//
  Rd: switch( ans )
    { case _Esc: Ghelp( "?Are you sure" );
        if( (Tv_getc()|0x20)!='y' )break; Tv_close(); return 0;
      case _Enter:Tback(); Ghelp(); goto Start;
      case 0xF1:  VMode &=~0x100;  goto Dr;
      case 0xF2:  VMode |= 0x100;  goto Dr;
      case 0xF4:
      { char Msg[20];
        Tgo( 12,3 ); sprintf( Msg," %-8lg",Vd ); color( WHITE );
        if( (ans=Tgets( Msg,-9,1 ))!=_Esc )
        { sscanf( Msg,"%lf",&Vd );
          Tgo( 12,3 ); Tprintf( " %-8lg",Vd ); ans=0xF2; goto Rd;
        }
      } break;
      case 0xF3: do
      { Mlist Menu[]=
        { { 1,0,"\t  ��������� �����������" }
        , { 2,26,(char*)((VMode&1)==0?"\t��� ������� �������� �����":"\t� �������� �������") }
        , { 1,26,(char*)((VMode&2)==0?"\t��� ����������� �������":"\t� �������� �������� ����") }
        , { 1,26,(char*)((VMode&4)==0?"\t��� ���������� �������":"\t� ����������� ��������") }
        , { 2,5,"\t  ���������: %5.3lf",&VScale }
        };
        Display T( Mlist( Menu ),1,-1 ); Ghelp(); color( CYAN );
        switch( ans=T.Answer( ans ) )
        { case 1: VMode^=1; break;
          case 2: VMode^=2; break;
          case 3: VMode^=4; break;
          case _Esc: goto Dr;
        }
      } while( ans<_Esc ); goto Rd;
    } continue;
Dr: color( BLACK ); Twindow( 5,-1,20,3 ); Draw( VScale,VMode ); Tback();
  } // for(;;)
}
