//   +------------------------------------------------------------+
//   � ��������� ������� ����������� � ������������ ������������, �
//   �         ( �������� ������� � ��������������� )             �
//   *============================================================*
//                     1997, ������, �.��������, ����-���������
//                     2001, ������ - �����������, ������� - ��-210
//
#include "..\Ship\Hull.h"
                         //
const int _nZ=36,_nX=72, // ���������� ����� �� ���������� ������
          _nA=36;        // ���������� ����� ����������
const Real mA=180;       // ������������ ���� ���������� � ��������

struct Hydrostatic:Plaze
{ Real  Vol[_nZ],            // �������� ������
        zCV[_nZ],            // ��������� ������ ��������
        Swl[_nZ],            // ������� ����������
        Srf[_nZ],            // ��������� ����������� �������
        xCV[_nZ],            // �������� ������ ��������
        xCW[_nZ],            // �������� ������ ������� ����������
        rfX[_nZ],            // ���������� ���������������� ������
        Rfy[_nZ],            // ���������� ���������������� ������
        aV[_nA][_nZ];        // ������ ���������� �������������
  Point aC[_nA][_nZ];        // -- �������� � ������� ����� ������� ��������
  Hydrostatic(): Plaze( _nZ,_nX ){}
 ~Hydrostatic(){}
  void Initial();
  void Graphics();
  void Stability();
};
//
//   �---------------------------------------------------------�
//   �                                                         �
//   �  �������� ������ ��������� �������������� �������       �
//   �       ����� ������� �����, � ������������ �������       �
//   �                                                         �
//   �   Draw_Hull( k ) - ���������� �������� ������           �
//   �   Volume_and_Surface( Z ) - �������� ������             �
//   �                             � ������� �������           �
//   �                                                         �
//   L=========================================================-
//
void Hydrostatic::Initial()
{   int i,k,l;
 double r,R,S,sX,v,
        x,y,z;
  build( Height);
  for( k=0; k<_nZ; k++ )xCW[k]=Xm;
  for( k=0; k<_nZ; k++ )        //
  { R=r=sX=S=0.0;               // ������ ������ ��������������
    x=dX/2;                     // ��� ���������� ��������, �������,
    z=dZ*( double( k )+0.5 );    // �� �������� �������
    for( i=0; i<_nX; i++,x+=dX )// � ���������������� ��������
    { S+=( y=Y[k][i] );         //
      r+=y*y*y;                 // ���.61 � ��������� ������������� �-�-�
      R+=y*x*x;                 //
      sX+=y*x;
    }                           //
    Swl[k]=(S*=2*dX);           // ������� ����������
            sX*=2*dX;           // ������ ������� ����������
    if( k>0 )                   //
    { v=dZ*(S+Swl[k-1])/2;      //
      Vol[k]=Vol[k-1] + v;      // �������������
      zCV[k]=zCV[k-1] + v*z;    // ������ ��� ��������� ������ �������� zC
      xCV[k]=xCV[k-1] + sX*dZ;  // ������ ��� �������� ������ �������� xC
    }                           //
    if( !S )xCW[k]=Xm,Rfy[k]=rfX[k]=0; else
    { xCW[k]=sX/S+Xo;           // ����� ������� ����������
      Rfy[k]=(R*2*dX-sX*sX/S);   // /Vol[k]->���������� ���������������� ������
      rfX[k]= r*2*dX/3;          // /Vol[k]->���������� --//--
    }                           //
//  if( !S )continue;           //
//  xCW[k]=sX/S+Xo;             // ����� ������� ����������
//  Rfy[k]=(R*2*dX-sX*sX/S);    // /Vol[k]->���������� ���������������� ������
//  rfX[k]= r*2*dX/3;           // /Vol[k]->���������� --//--
  }                             //
  zCV[0]=xCW[0]; //Do;          //
  Srf[0]=S=Swl[0];              // ������� ��������� �����������
  for( l=k=1; k<_nZ; k++ )      //
  { if( l<0 || Swl[k] )
    { for( i=1; i<_nX; i++ )
      { y=Y[k][i];
        if( y>0 )S+=sqrt( 1+norm( Y[k-1][i]-y,Y[k][i-1]-y ) )*dX*dZ;
      } Srf[k]=S*2;
      zCV[k]=zCV[k]/Vol[k]+Do;
      xCV[k]=xCV[k]/Vol[k]+Xo;
      if( !Swl[k-1] && l>0 )
      for( l=k-1; l>=0; l-- )xCV[l]=xCV[k],xCW[l]=xCW[k],zCV[l]=Do; l=-1;
      if( !Swl[k] )xCW[k]=xCW[k-1];
  } }
}
static void MinMax( Real *F, int N, Real &Min, Real &Max, const int mx=0 )
{ int i; if( !mx )Min=Max=F[0];
  for( i=0; i<N; i++ )if( Min>F[i] )Min=F[i]; else
                      if( Max<F[i] )Max=F[i];
}
static void Graphic_for_Element
( Real *C,            // ���������� ��������������� ������ (NULL - ���� �� ���)
   int y,             // �������� ��� �� ���������
  Real I,             // ��������� ������ �����
  Real L,             // ����� ����� ���
  const int up,       // ����� �������� ������� ����� (-�����, +����, 0-���)
  const int sp,       // �������������� �����
  const char Label[], // ������� �� ����� � �� �������
  const int Scale=1    // ������� ������������������� (����������) �����
){                      //
 Field F; F.Jx=F.Jy=0.0;
 double x,dL;
 int i;
  if( Scale )
  { if( L==0.0 )MinMax( C,_nZ,I,L );
    dL=Tv_step( L-I );
    I=floor( I/dL )*dL;
    L=ceil( (L-I)/dL )*dL;
  } else dL=Tv_step( L ); F.Jx=I;
                          F.Lx=L;
                          F.Ly=_nZ-1; Tv_place( 0,&F );
  if( up )
  { line( Tv.mX/2,y,Tv.mX,y );
    for( x=int(I/dL+1)*dL; x<I+L+dL/2; x+=dL )
    { i=Tv_x( x );                      //
      line( i,y,i,y+up );               // �������� �����
      if( x>0 )                         //
      Tv.Text( up>0?South_West:North_West,i-1,y+up/abs(up),_Fmt( "%.9lg",x ) );
    } Tv.Text( up>0?South_East:North_East,Tv.mX/2,y+up/abs(up),Label );
  }
  if( C )
  { for( i=0; i<_nZ; i++ )if( i )lineto( C[i],(Real)i );
                            else moveto( C[i],(Real)i );
    Tv.Text( West,Tv_x( C[sp] )-3,Tv_y( sp ),Label );
  }
}
//
//  ���� ������� ��������� ������� � ��������� ����������� �������
//
void Hydrostatic::Graphics()
{ int i,k=_nZ/3;
  Real mn,mx;

  color( YELLOW );
  Graphic_for_Element( Vol,Tv.mY/2+30,0,Vol[_nZ-1],-4,k++,"V" );
  Graphic_for_Element( Srf,Tv.mY/2+30,0,Srf[_nZ-1], 4,k++,"S" );

  color( LIGHTGREEN );
  MinMax( xCW,_nZ,mn,mx );
  MinMax( xCV,_nZ,mn,mx,1 );                               //
  Graphic_for_Element(   0,Tv.mY/2+60,mn,mx,4,k,"xS,xC" ); // ��������
  Graphic_for_Element( xCW,Tv.mY/2+60,mn,mx,0,k++, "xC" ); // ��������
  Graphic_for_Element( xCV,Tv.mY/2+60,mn,mx,0,k+=2,"xS" ); // � �������
  Graphic_for_Element( Swl,Tv.mY/2+60,0,0, -4,k++,"Swl" ); //

  color( LIGHTMAGENTA );                                   //
  Graphic_for_Element( rfX,Tv.mY/2+90,0,0,-4,k++,"Jx" );   // �������
  Graphic_for_Element( Rfy,Tv.mY/2+90,0,0,+4,k++,"Jy" );   // ����������
                                                           //
  for( i=1; i<=_nZ; i++ )if( Vol[k] )rfX[i]/=Vol[i],
                                     Rfy[i]/=Vol[i]; Rfy[0]=Rfy[1];
  color( LIGHTRED );                                 rfX[0]=rfX[1];
  MinMax( rfX,_nZ,mn,mx );
  Graphic_for_Element( rfX,Tv.mY/2+120,0,min(mx,Breadth ),-4,k++,"r" );
  MinMax( Rfy,_nZ,mn,mx );
  Graphic_for_Element( Rfy,Tv.mY/2+120,0,min(mx,Length*2),+4,k++,"R" );

  color( WHITE );
  for( i=0; i<=_nZ; i++ )rfX[i]+=zCV[i];
  Graphic_for_Element(   0,Tv.mY/2,Do,Height,4,k++,"Z,zC,zM",0 );
  Graphic_for_Element( zCV,Tv.mY/2,Do,Height,0,k++,"zC"     ,0 );
  Graphic_for_Element( rfX,Tv.mY/2,Do,Height,0,k++,"zM"     ,0 );
  color( DARKGRAY ); line( Do,0.0,Height,Real( _nZ-1 ) );
}
//   ������ ���� ����������� ������������ �����
//
void Hydrostatic::Stability()
{                       //
 int i,                 // ������ ������ ���������
     j,                 // ������ ���� ����������
     k,                 // ������ ����������� ������ (*cos)
     l,                 // ������ ������ ����� �� ������� ���������
     m,                 // ������� ��������� ������� �� ��������� (+-1)
     n,                 // ������� ����������� ������ ����� �� ���������
     o;                 // ������� ��������� ����� � ������ ���������
 double Z,A,dA,         // ������� ���������
        V,dV;           // �������� ���������
 Point f0,f1,f2,a,b,c,  // ��� ������� � ��� ����������� �����
       M,dC;            // �������� �������� � ����� ������������ ��������
                        //
  A=dA=M_PI*mA/( _nA*180.0 );   // ��� �� ���� ����������
  TvClip();
  for( j=0; j<_nA; j++,A+=dA )
  for( k=0; k<_nZ; k++ )        //
  { V=0;                        // ������ ������������ ��������������
    M=0;                        //
    if( !k ){ Z=Breadth*sin( -A )/2; if( A>M_PI_2 )Z+=Height*cos( A ); }
       else { Z+=( fabs( Height*cos( A ) )+Breadth*sin( A ) )/(_nZ-1); }
                                //
    for( i=0; i<_nX; i++ )      // ������� ����������
    { f0.x=0.0;                 // ������ ����� ������ = 0
      f0.y=Y[0][i];             //
      f1=f0*=polar( A );
      if( Z-f1.x>=0 ){ a=c=f1; n=1; } else n=0;
           o=0;                 //
      for( l=m=1; l>=-1; )      // ������ ������ ���������
      { if( l<0 )f2=f0; else    //
        { f2.x=l*dZ;            //
          f2.y=m*Y[l][i];       // �������� ����� ������
          f2*=polar( A );       //        ������� �������
        }                       //
        if( l<0 || Z-f1.x>=0 || Z-f2.x>=0 )
        do
        { if( l<0 && Z-f1.x<0 && Z-f2.x<0 )b=c; else
          { b=f2;
            if( Z-f2.x<0 ){ b.x=Z; b.y=f1.y+(Z-f1.x)*(f2.y-f1.y)/(f2.x-f1.x); }
            if( Z-f1.x<0 )
            { if( !o ){ o=1; b.x=Z; b.y=f2.y+(Z-f2.x)*(f2.y-f1.y)/(f2.x-f1.x); }
                 else   o=0;
          } }
          if( !n ){ c=b; n=1; } else    //
          { dV=( a.y*b.x-b.y*a.x )/2;   // ������������ �������
            dC=( a+b )/3;               // ����� ������������ �������
            M+=dC*dV;                   //
            V+=dV;
          } a=b;
        } while( o );
        f1=f2;
        if( m>0 && l==_nZ-1 )m=-1; else l+=m; // �������� �� ������ ����
    } }                 //
    if( V>0 )           // ��������� � ��������
      aC[j][k]=M/V;     //  ����� ������� ��������
      aV[j][k]=V*dX;    // ������ ���������� �������������
    //
    // ��������������� ���������� ����������� � ����� ������ ���� ������
    //
    if( j>0 )
    { color( DARKGRAY ); line( A-dA,aC[j-1][k].x,A,aC[j][k].x );
      color( BLUE     ); line( A-dA,aC[j-1][k].y,A,aC[j][k].y );
  } } TvUnClip();
//
//      ��������� ��������� ���� �������� � ������������� �����������
//
  Swl[0]=0;            A=dA;
  for( j=0; j<_nA; j++,A+=dA )
  { for( k=1; k<_nZ; k++ )                      //
    { V=Vol[k];                                 // ����� ���������-������
      for( i=1; aV[j][i]<V && i<_nZ-1; )i++;    // �� ��������� �������
      Swl[k]=i-(aV[j][i]-V)/(aV[j][i]-aV[j][i-1]); // -��������� ����� ������
    }
    for( k=1; k<_nZ; k++ )                      //
    { i=int( Swl[k] );
      if( i<0 )i=0; else
      if( i>=_nZ-1 )i=_nZ-2;
      dV = Swl[k]-i;
      dC = dV*aC[j][i+1]+(1-dV)*aC[j][i];

//    dC.x=dV*aC[j][i+1].x+(1-dV)*aC[j][i].x;
//    dC.y=dV*aC[j][i+1].y+(1-dV)*aC[j][i].y;
                                                //
//    aV[j][k]=dC.y-(rfX[k]-Do-0.5)*sin( A );   // �� ���������� - ��������
      aV[j][k]=dC.y-(zCV[k]-Do)*sin( A );       // �� ������ ��������
//    aV[j][k]=dC.y-k*dZ*sin( A );              // �� ����������
    }                                           //
  }
}
//
//      ������� ���������� ���� ��� ��������� ����
//
static void Axis_Statics( Real A )
{ int k;
 Real dA,z;
  color( BLUE      ); line( Real( 1 ),Real( 0 ),Real( 1 ),Height-Draught/5 );
  color( LIGHTGRAY ); line( Real( 0 ),Real( 0 ),A,Real( 0 ) );
  dA=A/36;            line( Real( 0 ),Real( 0 ),Real( 0 ),Height-Draught/5 );
  for( k=0; k<=36; k++ )
  { color( LIGHTGRAY );
    line( Tv_x( k*dA ),Tv_y(0),Tv_x(k*dA),Tv_y(0)+(k&1?4:8) );
    color( WHITE );
    if( !(k%6) )Tv.Text( South,Tv_x(k*dA),Tv_y(0)+8,_Fmt( "%i",k*5 ) );
  }
  for( k=0,z=0.0; z<=Height-Draught/5; k++,z+=Draught/5 )
  { color( LIGHTGRAY );
    line( !(k%5)?3:6,Tv_y( z ),12,Tv_y( z ) );
    color( WHITE );
    if( k && !(k&1) )Tv.Text( North_East,0,Tv_y(z),_Fmt( "%.3lg",z ) );
  } Tv.Text( South_East,20,Tv.mY/2+12,W2D( "������������: L� [�] " ) );
    Tv.Text( South_East,32,Tv.mY/2+24,"Zg=Zc" );
}
void Hull_Statics()
{ Ghelp();
  setactivepage( 1 );   // ������ � ����������� ������������
  setvisualpage( 1 );   // ������ ��������� �������������� �������,
 int j,k;               // ����������� ������������ � ��������� �������
 Real A,dA,z,dz;
 Field _F;
 field f,_f;
  Hydrostatic LD;
  LD.Initial();
  clear();
 _F.Lx=Tv.mX*Height/Tv.mY;  _F.Ly=Height;
 _F.Jx=-_F.Lx/2;            _F.Jy=Do;
  f.Jx=0;  f.Lx=50;  f.wb=0;
  f.Jy=50; f.Ly=50; _f=f; Tv_place( &_f ); Tv_place( 0,&_F ); TvClip();
                                           Draw_Hull( 0 );  TvUnClip();
  f.Lx=0;  f.Jx=50; _f=f; Tv_place( &_f ); color( YELLOW );
  Tv.Text( North_West,Tv.mX/2-3,Tv.mY/2-1,W2D( "����" ) );
//
//      �������� ����
//
 _F.Jy=_F.Jx=Do;
 _F.Lx=_F.Ly=Height; Tv_place( 0,&_F );
  color( WHITE );    Tv.Text( North_West,Tv.mX-3,Tv.mY/2-1,W2D( "[�]" ) );
                     Tv.Text( North,Tv_x( Draught ),Tv.mY/2-1,W2D( "��" ) );
  k=Tv_x( 0 );       line( k,Tv.mY/2,k,0 );
//
//      ���������� ������ ��������� �������������� �������
//
  LD.Graphics();
//
//      ������ ���� ����������� ������������ �����
//
   A=M_PI*mA/180.0;
  _F.Jx=0.0;     _F.Lx=A;
  _F.Jy=-Height; _F.Ly=Height*2; f.Jx=-12,f.Jy=-40; f.wb=0;
                                 f.Lx=-Tv.mX/2-12;
                                 f.Ly=-Tv.mY/2;    Tv_place( 0,&_F );
                                                   Tv_place( &f );
  Axis_Statics( A );

  LD.Stability();

   pattern( Tv.BkColor );
  _F.Jx=0.0; _F.Lx=M_PI*mA/180.0; bar( 0,Tv.mY/2+2,Tv.mX/2-2,Tv.mY );
  _F.Jy=0.0; _F.Ly=Height;        Tv_place( 0,&_F );

  Axis_Statics( A );
  dA=A/_nA;
  dz=LD.dZ;
  z=Draught/5-Do;
  for( k=int( z/dz ); k<int(5*Height/Draught)*(Draught/5)/dz; k++ )
  { if( fabs( k*dz-z )<dz )
    {               color( fabs( k*dz-Draught+Do )<dz?YELLOW:
                     k*dz>Draught-Do?LIGHTCYAN:LIGHTGREEN );
      z+=Draught/5; setlinestyle( SOLID_LINE,0,NORM_WIDTH );
    } else          setlinestyle( DOTTED_LINE,0,NORM_WIDTH ),
                    color( k*LD.dZ-Do>Draught?CYAN:GREEN );
                         A=dA;  moveto( Real(0),Real(0) );
    for( j=0; j<_nA; j++,A+=dA )lineto( A,LD.aV[j][k] );
    if( k*dz>=Height-Draught/5 )break;
  }
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );

  Axis_Statics( M_PI*mA/180.0 );

  Thelp( "%ld �� ",coreleft()>>20 );
  while( Tv_getc()==_MouseMoved );

  Tv_place( 0,&_F );
  setactivepage( 0 );
  setvisualpage( 0 );
}
#if 0
if( k==Nz/2 )
//if( j==6 ) //&& j&1 )
{ color( Z-f2.x<0?LIGHTMAGENTA: (Z-f1.x<0?LIGHTBLUE:LIGHTRED ));
  line( (f1+Do)*polar( M_PI_2 ),(f2+Do)*polar( M_PI_2 ) ); //Tv_getc();
  color( j&1?LIGHTCYAN:YELLOW );
  line( (a+Do)*polar( M_PI_2 ),(b+Do)*polar( M_PI_2 ) ); //Tv_getc();
}

if( k==_nZ/2 )
{ Gheld( "Z[%i]=%lg, A[%i]=%lg, V=%lg, M={%lg,%lg}",k,Z,j,A,V*dX,aC[j][k].x,aC[j][k].y );
//  Tv_getc();
}
#endif
