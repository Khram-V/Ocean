//
//      ����������� ���������� ������� � ������� ����������� ����
//
//                               ������ 1.2 �� 13 ����� 1993 ����
//                                      ������� ��������, �������
#include "..\Graphics\Tv_graph.h"

extern Real  DCy,DCx;           // ������ ������� ������
extern Field F;                 // �������� �� �� ���� �� ��� 3/2

static Point *C=NULL;           // ��������� ������ ��� ��������� �������
static int Mc=0,Nc=0,Kc=0;      // ���������� �������� � ����������� �����

void Curve_Analytic( int=-1 );  // ��������� (�����) ������������� �����
//
//      ��������� ��������� �������� �� ������� ���������
//              ��� ���������� ���������� ��������������

static Real  Dl=1;      // ��������� ��� ��������� ��������������
static int   Kl=0,kl=0, // ����� ����� � ������ ��������� �������
                  nc=0; // ���������� ��������� ������� �����
static Point P;

Real Curve_Normal( const Real _D ){     // D - ������������ ����� �������
 Real l,L=0; Kl=kl=nc=0; Dl=_D;         // - ����� ��������� ������ �������
  if( Nc>0 )                            // - �� ������ ������ ����� �������
  { P=C[Nc-1];
    for( int k=0; k<Nc; k++ )           // ������ ���� �������� �� ����� ���-��
    { if( (l=abs(C[k]-P))<Dl*1e-4 )     // ���������� ������� �������� ��������
      { --Nc; for( int j=k; j<Nc; j++ )C[j]=C[j+1]; continue; }
      L+=l; P=C[k];
    }
  } return L;
}
int Curve_Normal( Point& R, Point& N )
{                                       // R - ����� ������������ �������
 static Point D;                        // N - ������� � ������� �� �������
  if( Nc<=0 )return 0;
  if( kl>=nc )                          // �� ������ ������� ����������
  {    kl=0;                            //    ������ �������
    if( Kl>=Nc )
    { P=C[Nc-1]; return Kl=nc=0;        // ���������� �������� �� ������ �����
    } nc=int( abs( D=C[Kl]-P )/Dl+1 );
    if( Kl>0 )P=C[Kl-1]; Kl++;  D/=Real( nc );
  } N.x=-D.y;
    N.y= D.x;
  R = P+D/2; P+=D; kl++; return TRUE;
}
//      ������� ���������� ����� �������
//
void Curve_View()
{ color( YELLOW );
  if( Nc>0 ){ int k=0; for( moveto( C[Nc-1] ); k<Nc; k++ )lineto( C[k] ); }
  color( LIGHTGRAY );
 Point R,N; Curve_Normal( Dl );
     while( Curve_Normal( R,N ) )line( R,R+dir( N )/(6/Dl) );
}
//      ��������� ������ � ��������� �����
//
static void Corner( int AColor=YELLOW )
{ if( Nc>1 )
  { if( Nc==2 )line( C[0],C[1] );
    else{              moveto( C[Kc+1>=Nc?0:Kc+1] );
      color( AColor ); lineto( C[Kc] );
      color( YELLOW ); lineto( C[Kc-1<0?Nc-1:Kc-1] );
  } }
}
//   ������� �������� �������������� ������������ �����
//      � ������������� ���������� �������
//
int Curve_Access( Point A )
{ if( A.y<F.Jy || A.y>F.Jy+F.Ly || A.x<F.Jx || A.x>F.Jx+F.Lx )return false;
  { Real S=0;
    Point P,Q=C[Nc-1]-A;
    for( int k=0; k<Nc; k++ )
    { P=C[k]-A; S+=atan2( P.y*Q.x-P.x*Q.y,P.y*Q.y+P.x*Q.x ); Q=P;
    } return abs( S )>M_PI_2;
} }
//      �������� ��������� ��������� ������� �����
//
static void Field_Check( Field& F )
{ F.Jy=min(-F.Ly,F.Jy ),F.Ly=max(-F.Jy,F.Ly );      // �������� � ���������
  F.Jx=min(-F.Lx,F.Jx ),F.Lx=max(-F.Jx,F.Lx );      // ������������� ���������
  F.Jy*=1.25; F.Ly*=1.25; F.Ly-=F.Jy;               // ������������ �����������
  F.Jx*=1.5;  F.Lx*=1.5;  F.Lx-=F.Jx;
 Real LS=Real( Tv_port.bottom-Tv_port.top )/(Tv_port.right-Tv_port.left);
  if( LS>F.Ly/F.Lx ){ Real W=F.Ly; F.Ly=LS*F.Lx; F.Jy-=(F.Ly-W)/2; }
             else   { Real W=F.Lx; F.Lx=F.Ly/LS; F.Jx-=(F.Lx-W)/2; }
}
void Curve_Show()
{ Point P={ 25,50 },Q={ -3,-3 };
  int ans=0xFF,alt=0,k,cl=color( YELLOW );
//Tv_mouset( Insert,Tab,Delete );              // ����������� "��������" ������
//
//      ���������� ��������������� ��� ������������
//      �������������� ���������� ���������
//
  setwritemode( XOR_PUT );
  if( Nc>0 )
  { for( k=0; k<Nc; k++ )
    { if( k==0   )moveto( C[0] ); else
      if( k<Nc-1 )lineto( C[k] );
    } P=C[k-1]; Kc=Nc-1;
  }
  Corner( WHITE );
  //!      ������ ����� ������� - ��� ����������� ���������
  //
  color( LIGHTCYAN ); Twindow( 0,-5,10,2 );
  color( WHITE );     Tprintf( " Y:\n" ); Tprintf( " X:" );

  for( bool first=true;; first=false )    // �������� ������� ������� ���������
  { if( ans>_Enter )
      Ghelp( "F1 Info  F4 Analytic  <Ins|Del|Tab>Curve Points" );
    color( YELLOW );
    Tgo( 4,1 ); Tprintf( "%6.2lf\n",P.y*F.Ly+F.Jy );
    Tgo( 4,2 ); Tprintf( "%6.2lf\n",P.x*F.Lx+F.Jx );
    Real St=alt&CTRL? 0.1
          : alt&ALT ? 4:1;
    Corner( WHITE ); ans=Tv_pause( &P,8,St );
    Corner( WHITE ); alt=Tv_getk();
    //
    //   ��������� � ��������� ������� ������������� �����
    //
    if( ans==0xF4 ){ Ghelp(); Curve_Analytic(); goto Leave; }
    if( first && ans!=_Esc ){ Nc=Kc=0; continue; }
    if( ans<_Enter )
    { if( Nc>0 )
      { if( (alt&SHIFT)==0 )C[Kc]=P; else
        { Corner(); Curve_View(); P-=C[Kc]; for( k=0; k<Nc; k++ )C[k]+=P;
          Curve_View(); Corner(); P =C[Kc];
        }
      } continue;
    }
    switch( ans )
    { case _Esc: Corner(); goto Leave;
      case 0xF1: Corner();
      { Ghelp();                          color( CYAN );
        Twindow( 0,1,17,8 ); Tln();       color( LIGHTCYAN );
        Tprintf( "<Insert>New Point\n" );
        Tprintf( "<Delete>Point Off\n" );
        Tprintf( "<Tab>Point Select\n" ); color( WHITE );
        Tprintf( " Arrows +\n" );         color( LIGHTGRAY );
        Tprintf( " <SHIFT> Moving\n" );
        Tprintf( "  <CTRL> Slowly\n" );
        Tprintf( "   <ALT> Fastly\n" ); Tv_getc(); Tback();
      } Corner(); break;
      //
      //   ������� ����� ����� � �������� ����� �������
      //
      case _Ins:
        if( abs( P.y-Q.y )>=1
         || abs( P.x-Q.x )>=1 )
        { if( Nc>=Mc )C=(Point*)Allocate( (Mc=(Nc+24))*sizeof(Point),C );
          if( !Nc ){ C[0]=C[1]=Q=P; Kc=1; Nc=2; } else
          { int r=Kc+1; if( r>=Nc )r=0;   P+=P+C[r],P/=3;
            int l=Kc-1; if( l<0 )l=Nc-1; line( C[l],C[Kc] ); ++Kc;
             for( k=Nc; k>=Kc; k-- )C[k]=C[k-1];  C[Kc]=Q=P; ++Nc;
        } } break;
      //
      //   ������� ����� �������
      //
      case '-': alt|=SHIFT;
      case '+':
      case _Tab: Corner(); Kc+=alt&SHIFT?-1:1;
                 if( Kc<0 )Kc=Nc-1; else if( Kc>=Nc )Kc=0;
                 Corner(); Q=P=C[Kc]; break;
      //
      //   ���������� ��������� ����� �� �������
      //
      case _Del:
        if( Nc==2 ){ Mc=Nc=0; Allocate( 0,C ); C=0; }else
        if( Nc>2 )
        { for( k=Kc; k<Nc-1; k++ )C[k]=C[k+1];
                --Nc;
          if( Kc>=Nc )Kc=0; line( C[Kc],C[Kc+1<Nc?Kc+1:0] ); Q=P=C[Kc];
        }
    }
  }
Leave:
  Tback(); setwritemode( COPY_PUT ); color( cl );
  if( Nc>0 )
  for( k=0; k<Nc; k++ )                          // ������� � �������� ��������
  { if( !k )F.Jy=F.Ly=C[0].y,F.Lx=F.Jx=C[0].x; else // � ������ �����������
    { if( F.Jy>C[k].y )F.Jy=C[k].y; else if( F.Ly<C[k].y )F.Ly=C[k].y;
      if( F.Jx>C[k].x )F.Jx=C[k].x; else if( F.Lx<C[k].x )F.Lx=C[k].x;
  } }
  Field_Check( F ); Curve_Normal( Dl );
}
//
//      ��������� ������������� ��������
//
void Curve_Analytic( int inType )
{
 Mlist Menu[]
 ={ { 1,0,"\t  ����� ������� �����" }
  , { 2,0,"\t������ : " },{ 0,1 }
  , { 2,4,"\t������ Y= %4.2lf",&DCy },{ 0,4,", X= %4.2lf",&DCx }
  };
 Display T( Mlist( Menu ),1,-1 );
 static fixed Typ=0;                              // ��� �������������� �������
 const char *Obj[]={ "\t������","\t����������","\t���������",
                             "\t�������","\t�����","\t�����" };
 int ans,k; color( CYAN );
      Nc=0;
  if( Mc<24 )C=(Point*)Allocate( (Mc=24)*sizeof(Point),C );
  if( inType>=0 )Typ=inType;
  for( ans=k=0;; )
  { Thelp( "F4 ���� ������  <Escape>�����" );
    if( ans<sizeof(Menu)/sizeof(Mlist) )
    { if( Typ==5 )                              //  ����� � ��������� ���������
      { const Point Home[]=
         { {-.8,-.5},{-1,.3},{-.3,1},{0,.8},{.3,1},{1,.3},
           {.8,-.5},{.6,-.6},{.5,-.6},{.4,-.5},{.4,-.1},{.3,0},
           {0,.1},{-.3,0},{-.4,-.1},{-.4,-.5},{-.5,-.6},{-.6,-.6}
         }; Nc=18;
        for( k=0; k<Nc; k++ )C[k].y=Home[k].y*DCy,C[k].x=Home[k].x*DCx;
      }
      else if( Typ>2 )                 //  ���������� �� ������ ���������������
      { const Point Rect[]=
         { {-1,1},{1,1},{1,-1},{-1,-1},{-1,-.3},{1,-.3},{1,.3},{-1,.3}
         }; Nc=8;
        for( k=0; k<Nc; k++ )C[k].y=Rect[k].y*DCy,C[k].x=Rect[k].x*DCx;
        if( Typ==3 )Nc=4;
      } else
      { //  ���������� �� ������ �������� �� �������
        //
        for( k=0; k<24; k++ )
        { C[k].y= DCy*sin( M_PI*double( k )/12.0 );
          C[k].x=-DCx*cos( M_PI*double( k )/12.0 );
        }           Nc=24;
        if( Typ==1 )Nc=13; else
        if( Typ==2 )
        { C[0].x=C[1].x=C[11].x=C[12].x=C[13].x=C[23].x=0;
          C[1].y=C[11].y=C[2].y;
          C[13].y=C[23].y=C[22].y;
        }
      } F.Jy=-DCy; F.Ly=DCy;
        F.Jx=-DCx; F.Lx=DCx; Field_Check( F );
    }
    // ����������� �������� ����� �������������� ����
    //
    if( inType<0 )
    { Menu[2].Msg=Obj[Typ];     Curve_View();
      setwritemode( COPY_PUT ); color( CYAN ); ans=T.Answer();
      setwritemode( XOR_PUT );
      switch( ans )
      { case 2: case 0xF4: Typ++; if( Typ>5 )Typ=0; ans=2; break;
        case _Esc: return;
      } Curve_View();
    } else break;
  }
}
