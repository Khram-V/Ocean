#include <Math.h>
#include "..\Ship\Hull.h"
#include "..\Graphics\Tf_topo.h"
//
//    ��������� � ���������� ��������������� ��������
//      ��� ������������ ��������� ������� (K-HULL)
//
       Hull  Kh;        // ���������� ������
static Real  Lh=2.0,    // ��������� ����� �������
             Bh=1.0,    //   � ��� ������� ������
             Ts=0.75,   // ����������� �������� ������
             Ns=0.6,    // ������� �������� ��������� (1/Ns)
             Na=1.6,    // ������� �������� ��������� �������
             Fs=0.0,    // ������ ���������
             Ls=0.1,    // ������������� ����� ������
             La=0.15,   // ����� ��������� �������
             Cx=0.5,    // ���������� ��������� �������������� ����������
             Px=3,      // ������� ������� �������������� ����������
             Pz=6,      // ������� ������� ����������
             Tz=0.5,    // ����������� ����������� ������
             Ps=1.2,    // ��������� �������� �������: P � ]0�-[
             Pa=1.2,    //    P>10  �����������
                        // 1 <P< 5  ��������� ����������
                        //    P� 1  ����� �����
                        //    P<.5  ����� ����� �����
         Ks=0.8,Kp=1;   // ������ �����: K>1 - ������; K<1 - �����
                        //
static int Hull_Type=2; // 0-�������������; 1-�������������� ������
                        // 2-��������
static Mlist            //
       Menu_a[]=
        { { 1,0,"\t         �������� �������" }
        , { 2,4,"\t       ������ ��������� Fs [0\xB6\x30.5] = %4.2lf",&Fs }
        , { 1,4,"\t     ����������� ������ Ts [0.5\xB6\x31] = %4.2lf",&Ts }
        , { 1,4,"\t  ����� �������� ������ Ls [0\xB6\x30.5[ = %4.2lf",&Ls }
        , { 1,4,"\t       ������� �������� Ns ]1\xB6\xBA\x32\xBA] = %4.2lf",&Ns }
        , { 1,4,"\t����� ��������� ������� La [0\xB6\x30.5[ = %4.2lf",&La }
        , { 1,4,"\t       �������� ������� Na ]1\xB6\xBA\x32\xBA] = %4.2lf",&Na }
        , { 1,6,"\t      ����� ������� Length       = %6.2lf",  &Lwl }
        , { 1,6,"\t      ������       Breadth       = %6.2lf",  &Breadth }
//      , { 1,6,"\t      ������ �����   Depth       = %6.2lf",  &Height  }
        , { 1,6,"\t      ������       Draught       = %6.2lf",  &Draught } },
       Menu_s[]=
        { { 1,0,"\t��������� �������� �������: P\xBA]0\xB6-[" }
        , { 1,0,"\t    P>10  �����������"               }
        , { 1,0,"\t 1 <P< 5  ��������� ����������"      }
        , { 1,0,"\t    P\xBA 1  ����� �����"               }
        , { 1,0,"\t    P<.5  ����� ����� �����"         }
        , { 1,4,"\t    P- stem  = %4.1lf",&Ps            }
        , { 1,4,"\t    P-astern = %4.1lf",&Pa            }
        , { 1,4,"\t ����������� = %4.2lf",&Tz },{ 0,0,"\t \xBA ]0\xB6\x31]" }
        , { 1,4,"\tH����� ����� = %4.2lf",&Ks },{ 0,0,"\t ������>1>�����" } },
       Menu_p[]=
        { { 1,0,"\t   ������ ����������� ��"  }
        , { 1,0,"\t     ��������� ��������"   }
        , { 2,4,"\t���������  Pz = %4.1lf",&Pz }
        , { 1,4,"\t���������� P� = %4.1lf",&Px }
        , { 1,4,"\t���������� �� = %4.2lf",&Cx },{ 0,0,"\t \xBA ]0\xB6\x31]" }
        , { 1,4,"\t������ �����  = %4.2lf",&Kp },{ 0,0,"\t ������>1>�����" }
        };                                  //
static Real m_Stern( Real _z );             // ������������
static Real m_Stem ( Real _z );             // ����������
static Real m_Hull ( Real _z, Real _x );    // ������
                                            //
static int Set_Stem()
{ int i,j;
  Ns=1/Ns; Tmenu( Mlist( Menu_a ),1,-1 );
  Ns=1/Ns;
  scpy( Kh.Name,
   W2D( string(Hull_Type?"�������-��������� ������":"�������������� ������ �������") ) );
  if( Breadth<0.1 )Breadth=0.1;
  if( Draught>Breadth )Draught=Breadth;
  if( Lwl<Breadth*3 )Lwl=Breadth*3;
//-------
  Lh*=Lwl/m_Stem( Draught ); //-m_Astern( Draught ) );
  Xm =Lwl/2;
  Height=Draught*2.0;
  Xo  = m_Stern( 0 );
  Length=m_Stem( 0 );
  for( i=1; i<32; i++ )
  { Do=i*Height/32;
    if(    Xo>m_Stern( Do ) )Xo=m_Stern( Do );
    if( Length<m_Stem( Do ) )Length=m_Stem( Do );
  } Length-=Xo;
    Bwl=0.0;
  for( i=0; i<32; i++ )
  { Do=i*Draught/32;
    if( Bwl<m_Hull( Do,Xm ) )Bwl=m_Hull( Do,Xm );
  } Bh *= Breadth/Bwl/2;
    Bwl=m_Hull( Draught,Xm )*2;
    Do=0.0;
 //
 // ������������ ������������ �������������� ������� � �������� ����
 //
 const int Nx=24,Nz=36;
 Real Dx,Dz,x,z;        Dx=Length/( Nx+1 );
 static Frame *Deck=NULL;
 if( !Deck ){ (Deck=(Frame*)calloc(sizeof(Frame),1))->allocate(5); } // ��� new
 { Real *X=Deck->z,*Z=Deck->y; Z[0]=Height*0.9,X[0]=Xo;
                               Z[1]=Height*0.9,X[1]=Xo+Dx/2;
                               Z[2]=Height*0.8,X[2]=Length/2;
                               Z[3]=Height,    X[3]=Length-Dx;
                               Z[4]=Height,    X[4]=Length; Deck->SpLine();
 } Kh.Simple_Hull( Nx,Nz );
  for( i=0; i<Nx; i++ )                         // pow( Nz-1,0.648054273 );
  { Kh.F[i].X=x=Xo+Dx*(i+1); Dz=(*Deck)( x )/pow( Nz-1,1.543080635 );
    for( j=0; j<Nz; j++ )
    { z=Dz*pow( j,1.543080635 );
                   Kh.F[i].z[j]=z;
                   Kh.F[i].y[j]=m_Hull( z,x );
      if( i==Nx-1 )Kh.Stx[j].x=Kh.Sty[j].x=z,
                   Kh.Stx[j].y=m_Stem( z );
      else if( !i )Kh.Asx[j].x=Kh.Asy[j].x=z,
                   Kh.Asx[j].y=m_Stern( z );
  } } Hull_Type=2; Kh.Init(); return 0;
}
int Set_Sinus()
{ Hull_Type=1; Tmenu( Mlist( Menu_s ),1,-1 ); return Set_Stem();
}
int Set_Power()
{ Hull_Type=0; Tmenu( Mlist( Menu_p ),1,-1 ); if( Cx>1 )Cx=1; return Set_Stem();
}
static Real m_Stem( Real z )
{ extern Real K_Stem( Real ); if( Hull_Type==2 )return Kh.Stx( z );
  { Real m = Ts*Ns/(1-Ts),
         l = pow( Ts,m )*pow( 1-Ts,Ns );
    z = 1-z/Draught;
    return Lh *
     ( 1 + ( 1-(Fs*(z+1))/2 )           // ������ ���������
         * ( 1+Ls*( z<0?
                    z*z*(1-Ls):
                    pow( fabs(z),m )
                 *  pow( 1-z,Ns )/l) )/( 1+Ls ) )/2;
} }
static Real m_Stern( Real z )
{ extern Real K_Stern( Real ); if( Hull_Type==2 )return Kh.Asx( z );
  z = 1-z/Draught;
  return Lh*( 1 + sin( tanh( z*Na )*2.25 )*( z+2 )*La/3-1 )/2;
}
static Real m_Hull( Real z, Real x )
{ if( Hull_Type==2 )return Kh( x,z );
  Real A,S,Z;
   if( (S= m_Stem( z ))<x )return 0;
   if( (A=m_Stern( z ))>x )return 0;
   x = ( 2*x-S-A )/( S-A );
   z = 1-z/Draught;
   Z = z*Tz;
  return
  Hull_Type
  ? Bh * pow( 1-z,1/(2+8/(Pa+Ps)) )   // ������ ��������
       / ( 1+1/Pa+1/Ps )/2            // ���������� �� ����������
       * ( 1+cos( M_PI*x ) )          // �������� ����������
       * ( 1+pow( Ps*( 1-x ),-Z-1 )   // ������� ������ �����
            +pow( Pa*( 1+x ),+Z-1 ) ) // �������� ���������
       * ( 1+(1-Ks)*z )               // ����������� ������ ������
  : Bh * ( 1-pow( fabs( x ),Px ) )    // ��������� ����������
       * ( cos( x*M_PI_2*Cx ) )       // � �� ���������� ���������
       * ( 1-(z>0?pow( z,Pz ):0) )    // ��������� ��������
       * ( 1+(1-Kp)*z );         //====           +
}                               //        -+     -+-  o
                               //         -+-   --+/--=�  �
void Building()               //       ___\� �--+-+--��L�\+/
{                            //        ----+-�---=======�=�===�
 field f; Real x,y,z;       //         --__________________v_/ ___
          int  k,n,m;      //   ~~~~~~���8                  /~~--
//                      __//~~~ ~~   ~~ L-------------------
//   �---------------------------------------------------------�
//   �                                                         �
//   �   �������� ���������� ��                                �
//   �                                                         �
//   L=========================================================-
  clear();
  f.Jx=9;  f.Lx=90; f.wb=0;
  f.Jy=30; f.Ly=33;
  wW.Set( f );
  wW.F.Jx=Xo;     wW.F.Jy= Breadth/2.0;
  wW.F.Lx=Length; wW.F.Ly=-Breadth/2.0;
  wW.Focus();              //
  for( k=0; k<Kh.Ns; k++ ) // ������� ��������������� ����������
  { x=Kh.F[k].X;           //
    color( CYAN );
    setlinestyle( k==Kh.Ms?SOLID_LINE:DOTTED_LINE,0,NORM_WIDTH );
    line( x,0.0,x,Breadth/2 );
    color( LIGHTGRAY ); setlinestyle( SOLID_LINE,0,NORM_WIDTH );
    Tv.Text( South,x,Breadth*.502,_Fmt( "%d",k ) );
  }
  color( CYAN );
  x=Kh.Asx( Draught );
  for( k=0,z=0; k<=5; z+=Breadth/10,k++ )
  { setlinestyle( k%5?DOTTED_LINE:SOLID_LINE,0,NORM_WIDTH );
    line( x,z,x+Lwl,z );
  }
  for( z=0.0; z<=Height; z+=Draught/5 )
  { color( z>Draught?LIGHTCYAN:LIGHTGREEN );
    for( x=Xo,moveto( x,Kh( x,z ) ); x<Length; x+=Length*0.001 )
              lineto( x,Kh( x,z ) );
  }
  color( YELLOW );                      //
  moveto( x=Kh.Asx[Kh.Asx.N-1].y,0.0 ); // ���������� ����� ������
  lineto( x,Kh.Asy[Kh.Asy.N-1].y );     //
          y=Kh.Stx[Kh.Stx.N-1].y;
  for( k=0; k<Kh.Ns; k++ )
  { z=Kh.F[k].X;
    if( z>x && z<y )lineto( z,Kh.F[k].y[Kh.F[k].N] );
  } lineto( y,Kh.Sty[Kh.Sty.N-1].y );
    lineto( y,0.0 );
     color( WHITE );
  Tv.Text( North_West,Tv_port.right-4,Tv_port.bottom-2,Kh.Name );
//
//   �---------------------------------------------------------�
//   �                                                         �
//   �   �������� ���                                          �
//   �                                                         �
//   L=========================================================-
//
  f.Jy=64; f.Ly=36;
  wM.Set( f );
  wM.F.Ly=Height; wM.F.Jy=Do;
  wM.F.Lx=Length; wM.F.Jx=Xo;
  wM.Focus();
  x=Kh.Asx( Draught );
  for( k=0,z=0.0; z<Height; z+=Draught/5,k++ )
  { color( CYAN ); setlinestyle( k%5?DOTTED_LINE:SOLID_LINE,0,NORM_WIDTH );
    line( x,z,x+Lwl,z );
    color( LIGHTGRAY );  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
    Tv.Text( West,Xo-Lwl/30,z,_Fmt( k!=5?"%d":"WL",k ) );
  }
  //
  // �������� ����������
  //
  color( CYAN );
  for( k=0; k<Kh.Ns; k++ )
  { setlinestyle( k==Kh.Ms?SOLID_LINE:DOTTED_LINE,0,NORM_WIDTH );
    x=Kh.F[k].X;  line( x,0.0,x,Height );
  } setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  //
  // ��������� � ��������� ���� ��� �������� ������
  //
  n=Tv_x( Kh.F[Kh.Ms].X );
  k=( int( Breadth*( Tv_y( Do )-Tv_y( Do+Height ) )/Height )+1 )/2;
  if( k*3>Tv.mX )k=Tv.mX/3;
  f.Jx=k-n;
  f.Lx=n+k-Tv.mX;
  wH.Set( f );
  wH.F.Lx=Breadth;    wH.F.Ly=Height;
  wH.F.Jx=-Breadth/2; wH.F.Jy=Do;     Tv_bar();
  //
  // �������
  //
  wM.Focus();
  {                     //
   Real **A,Z;         // - �������� ��� ��������
   int   z,x;           //
    A=(Real**)Allocate( 81,101*sizeof(Real) ); Tf_List( 0 );
    for( z=0; z<=80;  z++ )
    for( x=0; x<=100; x++ )A[z][x]=Kh( Xo+x*Length/100,Do+z*Height/80 );
    for( Z=Breadth/10; Z<Breadth*0.502; Z+=Breadth/10 )Tf_List( Z,LIGHTGRAY );
                                                 Tf_Topo( A,0,0,81,101,TRUE );
    Allocate( 0,0,A );
  }
  //
  // �������� �������: ������ - ������ - ����
  //
  color( YELLOW );
  m=Kh.Stx.N-1;
  n=Kh.Asx.N-1;           moveto( ~Kh.Asx[0] ); //
  for( k=1; k<=n;    k++ )lineto( ~Kh.Asx[k] ); // ������������
  for( k=0; k<Kh.Ns; k++ )
  { x=Kh.F[k].X;
    if( x>Kh.Asx[n].y && x<Kh.Stx[m].y )lineto( x,Kh.F[k].Z( 1.0 ) );
  }
  for( k=m; k>=0; k-- )lineto( ~Kh.Stx[k] );    // ����������
  for( k=Kh.Ns-1; k>=0; k-- )
  { x=Kh.F[k].X;
    if( x>Kh.Asx[0].y && x<Kh.Stx[0].y )lineto( x,Kh.F[k].z[0] );
  }                                     lineto( ~Kh.Asx[0] );
  //
  // ����� ��������� ��� �������� ������
  //
  wH.Focus();
  Draw_Hull();
}
//   �----------------------------------------------------------------�
//   �                                                                �
//   �   ���������� �������� ������ (��������� �������������� �������)�
//   �                                                                �
//   L================================================================-
//
void Draw_Hull( int id ) // �������� ������
{                        //
 int i,k; Real x,y,z;    // �����
  color( CYAN );         //
  for( k=0,z=0.0; z<=Height; k++,z+=Draught/5 )
  { setlinestyle( k%5?DOTTED_LINE:SOLID_LINE,0,NORM_WIDTH );
    line( -Breadth/2,z,Breadth/2,z );
  }
  for( k=0,y=-0.4*Breadth; k<=8; k++,y+=Breadth/10 )
  { setlinestyle( k==4?SOLID_LINE:USERBIT_LINE,0x5555,NORM_WIDTH );
    line( y,Do,y,Height );
  } setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  if( id )
  { color( WHITE ); Tv.Text( East,Breadth/2,Draught,W2D( " ��" ) );
                    Tv.Text( South,(Real)0.0,0.0,   W2D( "��" ) );
  }                     //
       i=0;             // ���������� �������� ������������� ����������
  for( k=0; k<Kh.Ns; k++ )
  { if( !i && k==Kh.Ms+1 ){ i=1; k=Kh.Ms; }
    for( x=0.0; x<=1.0; x+=0.001 )
    { Kh.F[k].YZ( x,y,z );
      color( z>Draught?LIGHTCYAN:LIGHTGREEN );
      if( !i )y=-y;
      if( !x )moveto( y,z ); else lineto( y,z );
  } }                        moveto( ~conj( Kh.Asy[0] ) ); color( WHITE );
  for( k=1; k<Kh.Asy.N; k++ )lineto( ~conj( Kh.Asy[k] ) ); color( LIGHTCYAN );
            i=Kh.Ns/2+1;        //
  for( k=0; k<Kh.Ns; k++ )      // ������ (� ������)
  {                             //
    if( i && k==i ){ k=i-1; i=0; lineto( y,z ); moveto( -y,z ); }
    y=Kh.F[k].y[Kh.F[k].N];
    z=Kh.F[k].z[Kh.F[k].N];
    if( !y && k==Kh.Ns-1 )lineto( ~Kh.Sty[Kh.Sty.N-1] );
    if( i )y=-y; lineto( y,z );
    if( !y && !k )lineto( ~conj( Kh.Asy[Kh.Asy.N-1] ) );
  }      k=Kh.Sty.N-1;
         i=0;
  for( ; k>=0; k-- ){ lineto(~Kh.Sty[k]); if(!i)color(i=WHITE); }
}
//   �---------------------------------------------------------�
//   �                                                         �
//   � ���������� ����������� �������� �������������� �������  �
//   �                                                         �
//   L=========================================================-
//
void Plaze::allocate( int z, int x )
{  Y=(Real**)Allocate( z,x*sizeof(Real),Y ); Nx=x; Nz=z;
  QV=(Real**)Allocate( z,x*sizeof(Real),QV);// ��������� �� ���������� ������
  Xa=(Real*)Allocate( z*sizeof(Real),Xa );  // �������� �����������
  Xs=(Real*)Allocate( z*sizeof(Real),Xs );  //   � ���������
  Ya=(Real*)Allocate( z*sizeof(Real),Ya );  // ��������� �� �����������
  Ys=(Real*)Allocate( z*sizeof(Real),Ys );  //   � ��������� (������)
  Wx=(Real*)Allocate( x*sizeof(Real),Wx );  // ������ ����������������
}                                           //
void Plaze::build( Real _z )
{ int  i,j;
  Real x,y,z,sx,sz;                     //
  if( Nx<3 || Nz<2 )allocate( 12,48 );  // ���� �� �������� ������
  dX=Length/(Nx-1);                     //
  dZ=(Depth=_z-Do)/(Nz-1);              //
  V=S=0.0;
  for(   z=Do,j=0; j<Nz; j++,z+=dZ )
  { for( x=Xo,i=0; i<Nx; i++,x+=dX )V+=Y[j][i]=Kh( x,z );
    Xa[j]=m_Stern( z );                 // ������������
    Xs[j]=m_Stem( z );                  // ����������
  }                                     //
       V*=dX*dZ*2.0;                    // �������������

  for( i=1; i<Nx; i++ )
  for( j=0; j<Nz; j++ )
  { y=Y[j][i];
    if( y>0.0 )
    { if( j<Nz-1 )              //
      if( (sz=Y[j+1][i])>0.0 )  // ��������� ����������� �������
      { sx=( Y[j][i-1]-y )/dX;  //
        sz=( sz-y )/dZ;
        S+=sqrt( 1+sx*sx+sz*sz )*dX*dZ; goto Lx;
      } S+=y*dX;                             Lx:;
  } }   S*=2.0;                 //
  for( j=0; j<Nz; j++ )         // �������������� ���������
  {                             //
    for( i=1; i<Nx-1; i++ )QV[j][i]=(Y[j][i+1]-Y[j][i-1] )*dZ/-2.0;
    QV[j][Nx-1]=QV[j][Nx-2]/2.0;
    QV[j][0]=QV[j][1]/2.0;
  }
}
