//
//    ��������� ��������� ����������� ����� � �������������� �����������
//
//      Chart.cpp                                       /91.08.17/
//      World                                           /93.01.20/
//      Ch_Spher.cpp     ������������� ����������� �������������
//
#include <Ctype.h>
#include "..\Graphics\Tv_Graph.h"
                                  //
#define g       9.8106            // Constants rounded for 21 decimals.
#define Mile 1852.24637937659918            // Sea Mile
#define Pi      3.14159265358979323846      // M_PI
#define Pd      6.28218530717958647692      //  Pi x 2
#define Ph      1.57079632679489661923      //  Pi / 2
#define iP      0.318309886183790671538     //   1 / Pi
#define Pr      0.0174532925199432955       //  Pi / 180
#define rP     57.2957795130823229          // 180 / Pi
//
//  ����� ����������� ����� ������������� ��������� � ����-����������
//                                          //
static Point Op={ 142.71388,46.88745 };     // 46�43'15" 142�52'50"
                                            //
int World_Map( int R, Field *W=0, Field *M=0 ); // ����������� ����� ����
                                                //
     // ���� R = 0 �� ������������ ����������� �����������
     //            ����� M, � W ��������� �� ���������� ���� -> 1
     // ���� R > 0 - ������������ ������ ��� ������ �������  -> 1
     // ���� R < 0 - W,M ����������� ����������� ����������  -> 0
//
//   ����� ������������� ��������
//
//   void Navigation();         // ������ ������� � ���������
//   void Fisheries();          // ����� ����������� ��������
     void SM_list( char* =0 );  // ������� ������������ �������
                                //
static void Moving( int Ans );  // ����������� � ����������
//static void One_Minute( Point& P ) // ��������� ����� ������
//{    P.y=floor( P.y*60 )/60;     //
//     P.x=floor( P.x*60 )/60;    //
//}                              //
static int Wmap=0;              // �������������� ���������
 const Real M1=0.01666667;      //
static Real St=M1;              // ��� ������������� �����������
static void WINOp()             //
{ Tgo( 1,1 ); color( BLUE ); Tprintf( "%8s\n",DtoA( Op.y,2 ) );
                             Tprintf( "%8s\n",DtoA( Op.x,2 ) ); }
static void WINDM(){ color( GREEN ); Twindow( 1,-1,8,2 ); WINOp(); }
static void WIND2(){ color( GREEN ); Twindow( 1,-4,8,4 ); }
int main()
{ const char *Name[]={ "World  ","   Geographical",
                              "Tools & Enviroment",0 },
             *Text[]={ "F1 ","This Help",
                       "F2 ","Move Point by Coordinates",
                       "F3 ","- by Distance & Direction",
                       "F4 ","Retrieve the Chart Region",
                       "F7 ","Tsunami/Seismic Catalogue",0 },
             *Plus[]={ "<+|->    ","Draw Small Circles" ,
                       "<0,1..9> ","Step Multiplier",0,
                     };
 int ans=0xFF;
  Tv_Graphics_Start();
  setwindowtitle( "World - ��������� �� ����� ����" );
  Tfile_Filter="Data Series in ASCII file (*.txt)\0*.txt\0"
               "All Files in Storage   (*.*)\0*.*\0";
  bkcolor( WHITE );
  { field T; T.Jx=-11*Tv.Tw+1; T.Lx=T.Ly=-6; T.wb=0;
             T.Jy=-4*Tv.Th-4; Tv_place( &T );
  }
  if( _ac_>1 )
  { Wmap=1; SM_list( _av_[1] ); } else Wmap=World_Map( Wmap );
    WINDM();
  do
  { WINOp();
    if( ans>=_Enter )Thelp
    ( "{%d ��} 1help  2go  3move  4edit  7seismic  <0,1..9>Step <Esc>Quit",coreleft()>>20 );
    color( LIGHTRED );
    if( ans==0xF1 )ans=Help( Name,Text,Plus ); //
        else       ans=Tv_pause( &Op,12,St );  // One_Minute( Op );
    switch( ans )                              //
    { case _Enter:{ Field Plc; World_Map( -1,&Plc ); // ������� � ����-����������
                    Op.y=Plc.Jy+Plc.Ly*.8322;
                    Op.x=Plc.Jx+Plc.Lx*.2333; }
      case 0xF2:
      case 0xF3:          Moving( ans );                   break;
      case 0xF4: Tback(); Wmap=World_Map( Wmap ); WINDM(); break;
      case 0xF7: Tback(); SM_list();              WINDM(); break;
     default:
      if( isdigit( ans ) ){ if( ans<'2' )St=M1; else St*=ans-'0'; }
      else if( ans>' ' )ans=0xF1;
    }
  } while( ans!=_Esc );
}
//      Point's - ���������� � ��������
//      Course  - ����������� --//--
//      Dist    - ���������� � ����������
//      Arc     - ���������� ������ �����
//
//   ������ ����� ��������� ��� �������� �� �������� �����
//          �� �������� ����� �� ������� � ����������
//
static Point Sphere( Point Old_Pt, Real Course, Real Dist );
static Point Circle( Point Old_Pt, Real Course, Real Dist, Real Arc=0,int=24 );
//
//    ���������� � ����������� ����� ����� �������
//
static void Sphere( Point Opoint, Point Npoint, Real& Course, Real& Dist );
static Real Ds=0,Cs=-45,An=0;          //
static Point Np={ 142.71388,46.88745 }; // 46�43'15" 142�52'50" ��-���.
                                        //
static int ANSWER( bool S, Real Step )
{ Tgo( 1,1 );                        color( S?CYAN:GREEN );
  Tprintf( "%8s\n",DtoA( Np.y,2 ) );
  Tprintf( "%8s\n",DtoA( Np.x,2 ) ); color( S?GREEN:CYAN );
  Tprintf( "%6.0lfkm\n",Ds );
  Tprintf( "%8s",DtoA( Cs,2 ) );     color( LIGHTRED );
 int ans=Tv_pause( &Np,4,Step );     //if( S )One_Minute( Np );
 return ans;
}
static void Moving( int ans ){ if( ans==_Enter ){ Np=Op; return; }
//
//     ������� ������������� �����
//
 const char *Name[]={ "Movie  "," Course and Distance",
                                 "on the Spherical Earth",0 },
             *Text[]={ "F1 ","This Help",
                       "F2 ","Move Point by Coordinates",
                       "F3 ","- by Distance & Direction",
                       "F4 ","Retrieve the Chart Region",0 },
             *Plus[]={ "<+|->    ","Draw Small Circles" ,
                       "<0,1..9> ","Step Multiplier",0,
                     };
  const Real Km=1;
 static Real Sr=1,Sd=Km;
    int S=TRUE,tk=-1;
        WIND2();
Repeat:
  switch( ans )
  { case 0xF1: Help( Name,Text,Plus ); break;
    case 0xF2: S=TRUE;  break;
    case 0xF3: S=FALSE; break;
    case 0xF4: Tback();                         //
               Tback(); Wmap=World_Map( Wmap ); // Wmap=1 ����� �� ������
               WINDM();                         //
               WIND2(); break;
    case  '+': case '=': An+=Sr,tk=1; break;
    case  '-': case '_': An-=Sr,tk=1; break;
    case _Esc: Tback(); return;
  }
  if( S && tk>=0 )Sphere( Op,Np,Cs,Ds ); else
  if( S )tk=0;
  if( ans>0xF0 )
  Ghelp( "1help  2go  3move  4edit  <+->circle <0,1..9>Step" );

  Np=Circle( Op,Cs,Ds,An ); ans=ANSWER( S,St );
     Circle( Op,Cs,Ds,An );
  if( ans<_Enter )
  { if( !S )
    { if( ans&West  )Cs+=Sr,tk=1; else if( ans&East  )Cs-=Sr,tk=1;
      if( ans&North )Ds+=Sd,tk=0; else if( ans&South )Ds-=Sd,tk=0;
    }
  } else
  if( ans<='9' && ans>='0' )
  {   if( ans<'2' ){ if( tk )Sr=1; else Sd=Km,St=M1; }
    else{ ans-='0';  if( tk )Sr*=ans; else
                     if( !S )Sd*=ans; else St*=ans; }
  } goto Repeat;
}
//
//      �������������� ������� �� ����������� �����
//              ������  : -90� � 90�
//              ������� :   0� � 360� (���������� �� ����� �����)
//              Op,Np   : ������� � ����������� �����
//              Ds [km] : ���������� ����� �������
//              Cs [ �] : ���� �������� �� �������� �����
//              An [ �] : ����-���� ������� ������ ����������
//
#include <Math.h>
#include "..\Graphics\Tv_graph.h"
#define Mile 1852.24637937659918        // The Sea Mile in meters
#define Pi      3.14159265358979323846  // M_PI
#define Pd      6.28218530717958647692  //  Pi x 2
#define Ph      1.57079632679489661923  //  Pi / 2
#define Pr      0.0174532925199432955   //  Pi / 180
#define rP     57.2957795130823229      // 180 / Pi

static const Real Km2R=Pr*1000/Mile/60;

Point Sphere( Point Op, Real Cs, Real Ds )
{
//      ������ ����� ��������� ��� �������� �� �������� �����
//          �� �������� ����� �� ������� � ����������.
//              ������� ����������� �������:    Fi [-90:90]
//                                              La [0:360�]
 double Y,X;
  Op*=Pr;
  Op.y=Ph-Op.y;  if( (Ds*=Km2R)<0 )Ds=-Ds,Cs-=180;
                      Cs=Angle( Cs )*Pr;
  Y=acos( X=cos( Ds )*cos( Op.y ) + sin( Op.y )*sin( Ds )*cos( Cs ) );
  X=acos( ( cos( Ds )-cos( Op.y )*X )/sin( Op.y )/sin( Y ) );
  Ds=fmod( Ds,Pd );               if( Ds>Pi  )X=Pd-X;
  Op.y=( Ph-Y )*rP;               if( Cs<0   )X=-X;
  Op.x=fmod( ( Op.x+X )*rP,360 ); if( Op.x<0 )Op.x+=360;
 return Op;
}
void Sphere( Point Op, Point Np, Real& Cs, Real& Ds )
{
//      ���������� � ����������� ����� ����� �������
//
 double W,S;
  Op.y=Ph-Op.y*Pr; W=Angle(Np.x-Op.x)*Pr;
  Np.y=Ph-Np.y*Pr;
  Ds=acos( S=cos( Op.y )*cos( Np.y )+sin( Np.y )*sin( Op.y )*cos( W ) );
  Cs=acos( ( cos( Np.y )-cos( Op.y )*S )/sin( Op.y )/sin( Ds ) );
  Ds/=Km2R;
  Cs*=rP;  if( W<0 )Cs=-Cs;
}
static Real Lmin,Lmax;   // ���������� ������� ������� � ��������
static Point Q={ 0,0 };  // ��� ������������� �����
static int L2M=0;        // ������� ������ �����
static void LineToPoint( Point P )
{
//      ���������� ����� �� ������� ������� �����
//
  if( !L2M ){ L2M=1; moveto( P ); }else
  { if( P.x-Q.x>180 )P.x-=360; else
    if( Q.x-P.x>180 )P.x+=360; lineto( P );
    if( P.x<Lmin ){ P.x+=360; Q.x+=360; L2M=0; }else
    if( P.x>Lmax ){ P.x-=360; Q.x-=360; L2M=0; }
    if( !L2M ){ moveto( Q );
         L2M=1; lineto( P ); }
  } Q=P;
}
static Point Circle( Point Op, Real Cs, Real Ds, Real An, int dS )
{
//      �������� � XOR ������� ����� ����� ����� �������
//      � �� ���������� ���������� ������� ���� ������ �����
//
 Real D=Ds/dS;
 int k,l=dS/2,c=color( MAGENTA ); setwritemode( XOR_PUT );
  { static Field Fw={ 0,0,1,1 }; Tv_place( 0,&Fw ); Lmax=( Lmin=Fw.Jx )+Fw.Lx;
                                 Tv_place( 0,&Fw );
  }
  if( An!=0 )
  { Real A=An/dS; L2M=0;
    for( k=-l; k<=l; k++ )LineToPoint( Sphere( Op,Cs+A*k,Ds ) );
  }               L2M=0;  LineToPoint( Op );
  for(   k=1; k<=dS; k++ )LineToPoint( Sphere( Op,Cs,D*k ) );
  setwritemode( COPY_PUT );
  color( c );
 return Q;
}
