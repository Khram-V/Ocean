//
//      Mario.cpp                                       /91.04.01/
//      ��������� ����������                            /93.01.10/
//
#include "..\Ani\Mario.h"
//
//      ���� �������� ������ �������� ����������
//
Series_Array Ms; // ���������� ������ ���������� � �� �����������
Event  Tm;       // ������,
double dT=1,     //  ��� �
       Tn=72;    //  ����� �� ������� [���]
int    Nb=0,     // ����� �����
       Nx=0,     //  � �������
       Pw=1;     // ������� ���������� ������������
fixed  Vv=0x235; // ��������� ������������ (������: 0�275)
                 //   X0 - ������������ �
                 //   0X - �������������� �����
                 //    8 - 1-������ �������
                 //    4 - 1-����� �� ������� ��������
                 //    2 - 0-����������, 1-������������� ��������
                 //    1 - 1-������� �������� ��������
                 // 01xx - 0-������� �� ��� �������
                 // 02xx - 1-������� �������� �����
                 // 1xxx - 1-MEM; 0-FFT
                 // 2xxx - 1-������; 0-�������
                 // Delete    - ���������� � �������� ����
                 // Up/Down   - ����� �����������
Bfont Tw;        // Home/End  - ��������� �����/������ �������
                 // Left/Right- ����������� ���������� ���
char             // PgUp/PgDn - �������� ���� �����/������
Months[12][10] = //
//{ "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" },
  { "������","�������","����","������","���","����","����","������","��������","�������","������","�������" },
Weeks[7][12] =
//{ "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday" };
  { "�����������","�������","�����","�������","�������","�������","�����������" };

static char UIC[]="(c)\0""1991-2016 \0�.�.��������, �������, ������";
static unsigned long Mmem=0;

int main()
{
 long Mmem=coreleft();
 field fM={ -80,-64,0,0,0 };   // ����, ���������� ��� ������� ���������
  Tv_Graphics_Start();
  setwindowtitle( "Mario - �������������� ������������� � ������� ������������" );
  Tw.Font( _Small_font,Tw.Th=9+Tv.mX/213 );
  fM.Jy=-5*Tw.Th;
  Tv_place( &fM );
 int k=Tv.mX/Tv.Tw;
  Tgo( k-50,2 ); color( YELLOW );    Tprintf( UIC );
                 color( LIGHTCYAN ); Tprintf( UIC+4 );
                 color( LIGHTGRAY ); Tprintf( W2D( UIC+15 ) );
  Tgo( k-50,3 ); color( YELLOW );    Tprintf( UIC );
                 color( LIGHTGRAY ); Tprintf( W2D( "2010 ����������� ��������������� �����������" ) );
  Tgo( k-42,4 );                     Tprintf( W2D( "�������������� � 2010615847" ) );
  Tgo( k-48,6 ); color( YELLOW );    Tprintf( "Use> Mario FileName" );
                 color( WHITE  );    Tprintf( ".<mar|tim>"     );
                 color( LIGHTGRAY ); Tprintf( " /beg:len m1 m2 ... mN" );
                 color( CYAN );
  Tgo( k-44,7 ); Tprintf( W2D( "/beg:len - ������ � ����� ������" ) );
  Tgo( k-44,8 ); Tprintf( W2D( "m1... mN - ������ ����������� �������" ) );
                 bkcolor( WHITE );
  for( k=0; k<12; k++ )CharToOem( Months[k],Months[k] );
  for( k=0; k<7; k++ )CharToOem( Weeks[k],Weeks[k] );
  Tm.Now();
  Mario_main();
}
void MinMax( Real* V, int N, Real &Min, Real &Max )
{ for( int i=0; i<N; i++,V++ )
  if( i==0 )Min=Max=*V; else
  { if( Min>*V )Min=*V; else
    if( Max<*V )Max=*V;
  } if( Max-Min<1e-6 )Max=Min+1e-6;
}
static void T( const char *s ){ Tln(); Tprintf( W2D( s ) ); }
static int Help()
{
 unsigned long M=coreleft(); int c=color( CYAN ),b=bkcolor( BLUE );
  Twindow( 2,-2,38,24,2 );                        color( YELLOW );
  T( "  << ������ ������������ ������� >>\n"   ); color( WHITE );
  T( "  L  ������"                             );
  T( " R/W ������/������ ����� �������"        );
  T( " I/O ����������/������� ����� �����"     );
  T( " XYZ ������ ��� <Ani><Grid><Mesosaur>\n" ); color( YELLOW );
  T( "  K �������������� �����"                ); color( WHITE );
  T( "  T ������������� ��������� ��������"    ); color( LIGHTCYAN );
  T( "  F ��� ������ �������������� �������"   );
  T( "  S ��������� ����������� � �������"     );
  T( "  E ��������� ���������� ������ ����"    );
  T( "  C ����������� ������ ��������� ����"   );
  T( " M/D �������/�������� ������ �������\n"  ); color( LIGHTGREEN );
  T( " <Enter> �������������� �������� ����"   ); color( LIGHTCYAN );
  T( " <Up-Down-Ins> ����� � ������� �����"    );
  T( " <Home-End-Del> ����� � ������ ������"   );
  T( " <Left-Right> ����������� ������ ����"   );
  T( " <PgUp-PgDn> ����� ����������� ����"     );
  T( " <0|1> ��������� ���� �� ����|������"    );
  T( " <2:9> ���������� ����\n"                ); color( LIGHTGRAY );
  Tpoint.y+=3;
  Tv.Height( 10 );
  Tprintf( W2D( " ������������: %lu + %lu �� ������" ),(Mmem-M)>>20,M>>20 );
  Tv.Height( 0 );
 int Ans=Tv_getc(); Tback(); bkcolor( b ); color( c ); return Ans;
}
void Series_Array::allocate( unsigned s )
{ static bool First=true;
   if( n<=0 )k=0;
       Array=(Mario**)Allocate( (n=s),sizeof( Mario ),Array );
   if( n )
   { if( First ){ First=false; Array[0][0].JT=Tm; Array[0][0].dt=dT; }
     if( k>=n )k=n-1;
   }
}
void Segment_Line( Real T, int c=CYAN )
{ Point P;   P.x=T, P.y=0; fixed l=c==BLUE?0x1111:0xCCCC;
  point p,q; p=q=P; p.y=1;         c=color( c );
   setlinestyle( USERBIT_LINE,l,NORM_WIDTH );
   if( l==0xCCCC )setwritemode( XOR_PUT  ); line( p,q );
   if( l==0xCCCC )setwritemode( COPY_PUT ); color( c );
   setlinestyle( SOLID_LINE,0,NORM_WIDTH );
}                                           //
#define RR { if( TM+(TN+=ST)>W )TN=W-TM; }  // ������ ������� ������
#define RL { if((TN-=ST)<ST )TN=ST;      }  // ������ ������� �����
#define LL { if((TM-ST)<Tm.T)TN=TM+TN-Tm.T,TM=Tm.T; else TM-=ST,TN+=ST; }
#define LR { if((TN-ST)<ST  )TM+=TN-ST,TN=ST; else TM+=ST,TN-=ST; }

void Mario_main()
{                                   //
 double TM=0,TN=12,ST=1,W=0;        // ���������� �������
 int    Cmd,Side=-1,Act=0,k;        // ������� �����������
 static byte C_L[]={ CYAN,DARKGRAY,LIGHTGRAY },
             C_R[]={ CYAN,LIGHTGRAY,DARKGRAY };
        Cmd=_ac_>2?'r'
           :_ac_>1?'l':0xF1;
Repeat:
  switch( Cmd|0x20 )                      //
  { case 's': m_Edit(); Act=0;     break; // ���������� �������
    case 'i': m_InPut();           break; // ��� ������ � ������
    case 'l': m_List(); if(_ac_>1 )m_Tide(); break;
    case 'r': m_Read(); if(_ac_<1 )break;
    case 't': m_Tide();         break;
   default: if( Nm<=0 ){ Act=0; break; } W=Tm.T+Tn;
   Mario &M=Ms[Km];
    switch( Cmd )
    { case _Tab : Vv^=0x1000;  break; //
      case _Del : Act=0;       break; // ������� ����������������
      case North_West: Side=0; break; // � ������ ������
      case South_West: Side=1; break; //
      case       West: if( Side!=0 )RL else LL break;
      case       East: if( Side<=0 )LR else RR break;
      case North_East: TM-=ST; if( TM<Tm.T )TM=W-TN; Side=-1;  break;
      case South_East: TM+=ST; if( TM+TN>W )TM=Tm.T; Side=-1;  break;
      case 0xF4: Tm=M.JT; dT=M.dt; Tn=dT*M.N; Act=0; m_View(); break;
      case 0xF2: Tm.T=TM,Tn=TN;               Act=0; m_View(); break;
      case 0xF3:{ long J=Tm.D; m_ReView(); TM+=double( J-Tm.D )*24; }
                       W=TM+TN;         break;
     default:
      switch( Cmd|0x20 )
      { case 'k': m_Karta();            break; // ��� ����������� - � ���������
        case 'c': m_Copy();   m_View(); break;
        case 'm': m_Move();   m_View(); break;
        case 'd': m_Delete(); m_View(); break;
        case 'e': M.Initial();          break;
        case 'o': m_OutPut();           break;
        case 'x': m_Write();            break;
        case 'z': m_Write( 1 );         break;
        case 'w': m_Write( 2 );         break;
        case 'f':!Nb?Filtration():Function(); break;
        case 'y': if( Side>=0 )
        { static char Name[43]="~Surface.dat"; FILE *F; Event T=Tm+(TM-Tm.T);
          if( Side>0 )T+=TN;
          Thelp( "?��� ��������� �����" ); Tgo( 0,-1 ); color( BLUE );
          if( Wgets( Name,-42 )==_Enter )
          if( (F=fopen( fext( Name,"dat" ),"wt" ))!=0 )
          { for( k=0; k<Nm; k++ )
            { Mario &Mw=Ms[k];
              if( !Nb || M.flag&1 )
              fprintf( F,"%lg %lg %lg\n",Mw.Longitude,Mw.Latitude,Mw( T ) );
            } fclose( F ); F=NULL;
        } } break;
       default:
        if( Cmd>='0' && Cmd<='9' )
        { if( Cmd=='0' )ST=M.dt; else
          if( Cmd=='1' )ST=dT; else ST*=(Cmd-'0');
          if( ST>Tn )ST=Tn/3;
          if( ST>TN )TM+=(TN-ST)/2,TN=ST;
        } else Cmd=0xF1;
    } }
  }             //
  if( Nm>0 )    // ���������� ������ ���������� ����� ���������� ���������
  { if( !Act )  //                        ��� ������� ������� � ����������
    { TM=Tm.T,TN=Tn,W=TM+TN,Side=-1,Act|=4;
      ST=Tn/( Tv_port.right-Tv_port.left );
    }                                   //             !! ���������� �����
    if( TM<Tm.T )TN+=TM-Tm.T,TM=Tm.T;   // ��������� � ��������
    if( TM+TN>W )TN=W-TM;               // �������� �� ������� ��������
    if( ST>TN   )ST=TN/3;               //
    if( TM>Tm.T ){ Segment_Line( TM   ,C_L[Side+1] ); Act|=1; }
    if( TM+TN<W ){ Segment_Line( TM+TN,C_R[Side+1] ); Act|=2; }
  }
  for( ;; )
  { static int Tif=0,TiLx=36;   //
    if( Act&3 )                 // ����������� ��� �������� ��������
    { Mario &Mv=Ms[Km];         // ������ � �����������
      color( DARKGRAY );        // �������������� ��������� ��������
      if( Nm>0 && Cmd<_Enter )   //
      { if( !Tif ){ Twindow( -37,-1,TiLx=Tn>48?36:25,1,0 ); Tif=1; }
       double Tf=Side<1?TM:TN+TM;
       long jd=long( Tf/24 ); Tgo( 1,1 );
       int  l=0;
        if( TiLx>25 )
        { int m,d,y;                color( BLUE );
          julday( Tm.D+jd,m,d,y ); l=Tprintf( " %d-%02d-%02d",y,m,d );
        }                         l+=Tprintf( " %s",DtoH( Tf-jd*24,3 ) );
        if( Side<0 )
        {                           color( DARKGRAY );
                                    l+=Tprintf( " + "     );
          if( (jd=long( TN/24) )>0 )l+=Tprintf( "%dd,",jd );
                                    l+=Tprintf( DtoH( TN-jd*24,3 ) );
        } else
        { Event T( Tm.D,Tf );
          if( T>Mv.JT && T<Mv.JT+Mv.dt*Mv.N )
          { color( DARKGRAY ); Tprintf( " [%d]:",int( (T-Mv.JT)/Mv.dt ) );
            color( BLUE );     Tprintf( "%5.3lg",Mv( T ) );
        } }                    Tprintf( "\n" );
    } }                         //
//  else                        // ����� ����������� ������ �������� ����
//  { Ms[Km].Spectrum( Vv );    //
//  }
    // ----------------------------------------------------� +==========
    // �   ���������� ��������� �� �������� � ����������   � �
    // L===================================================- �
    if( Cmd==' '&&Nm>0 ){ Ghelp(); m_Mark( ' ' ); Cmd=Tv_getc(); m_Mark(); }
    else
    { if( Cmd>=_Enter )Thelp( "F2 ����  F3 ���  F4 ����" );
      if( Nm<=0 || Cmd==0xF1 )Cmd=Help();
                         else Cmd=Tv_getc();
    }
    if( Tif )
    if( Cmd>=_Enter || Cmd==North || Cmd==South ){ Tback(); Tif=0; }
    if( Cmd==_Esc )
    { Thelp( ">�� �������?" );
      Cmd=Tv_getc();
      if( Nm<=0 || (Cmd|0x20)=='y' || Cmd==_Enter ){ Tv_close(); return; }
      Cmd=_Esc; continue;
    }
    if( Nm>0 )switch( Cmd )
    { case North : if( Nm>1 )m_Mark( 1 );    continue;
      case South : if( Nm>1 )m_Mark(-1 );    continue;
      case _Ins  : if((Ms[Km].flag^=1)!=0 )Nb++; else Nb--;
                       Ms[Km].Cross();       continue;
      case _Enter: m_Show();                 continue;
      default    : if( Cmd>0xF4 || Cmd==' ' )continue;
    } break;
  }
  if( Nm>0 ){ if( Act&1 )Segment_Line( TM   ,C_L[Side+1] );
              if( Act&2 )Segment_Line( TM+TN,C_R[Side+1] );
            }
  goto Repeat;
}
#if 0
//
// ��� ��������� �� Numerical Recipes
//
//void static memcof( Real *data, int n, int m, Real &pm, Real *cof );
Real static evlmem( Real fdt, const Real *cof, int m, Real pm );

void Maximum_Entropy_Spectrum
( Real *_X,  int nD,   // ���� ������� ������
  Real *Spc, int nS,   // ������������ ������� �� ������
  Real &Max            // ��������� � ���������� ��������� �������
)                       //
{ int  j,k;
 Real *cof=(Real*)Allocate( nS*sizeof( Real ) ),
      *wk1=(Real*)Allocate( nD*sizeof( Real ) ),
      *wk2=(Real*)Allocate( nD*sizeof( Real ) ),
      *wkm=(Real*)Allocate( nS*sizeof( Real ) );
 Real pm,denom,num=0.0;
               Max=0.0;
//
// Given a real vector data[1..n], and given m, this routine returns
// a vector cof[1..m] with cof[j]=aj, and a scalar pm=b0, which are
// the coefficient for Maximum Entropy Method spectral estimation
//
  memcpy( wk1,_X,nD*sizeof( Real ) );
  memcpy( wk2,_X+1,sizeof( Real )*(nD-1) );
  for( k=0; k<nD; k++ )num+=sqr( _X[k] ); pm=num/nD;
  for( k=0;;k++ )
  { num=denom=0.0;
    for( j=0; j<nD-k-1; j++ )num+=wk1[j]*wk2[j],
                           denom+=norm( wk1[j],wk2[j] );
    cof[k]=2.0*num/denom;
    pm *= 1.0-sqr( cof[k] );
    if( k )for( j=0; j<=k-1; j++ )cof[j]=wkm[j]-cof[k]*wkm[k-j];
    if( k==nS-1 )break;
    memcpy( wkm,cof,sizeof( Real )*unsigned( k+1 ) );
    for( j=0; j<nD-k-2; j++ )
    { wk1[j]-=wkm[k]*wk2[j];
      wk2[j]=wk2[j+1]-wkm[k]*wk1[j+1];
    }
  } Allocate( 0,wkm );
    Allocate( 0,wk2 );
    Allocate( 0,wk1 );
  for( k=0; k<nS; k++ )                 // ������� ������ Spc
  { //nom=0.5*Real( k+1 )/Real( nS );
    denom=0.5+k*(1/Real(nS)-0.5)/(nS-1);
    Spc[k]=num=log( 1+evlmem( denom,cof,nS,pm ) ); if( Max<num )Max=num;
  } Allocate( 0,cof );
}
Real static evlmem( double fdt, const Real *cof, int m, double pm )
{
 double sumr,sumi,wr,wi,wpr,wpi,wtemp;
        sumr=wr=1.0; wpr=cos( 6.28318530717959L*fdt );
        sumi=wi=0.0; wpi=sin( 6.28318530717959L*fdt );
  for( int i=0; i<m; i++ )
  { wr=(wtemp=wr)*wpr-wi*wpi; sumr -= cof[i]*wr;
    wi=wi*wpr+wtemp*wpi;      sumi -= cof[i]*wi;
  } return pm/(sumr*sumr+sumi*sumi);
}
void FFT_Spectrum
( Real *_X,  int nD,   // ���� ������� ������
  Real *SX,  int nS,   // ������������ ������� �� ������ (^2)
  Real &Max            // ��������� � ���������� ��������� �������
);
void Time_Series::Spectrum( fixed mode )
{
 int k;
 static nS=1024;
 static Real *Spc=NULL;  // ����� �������� ���� Spc[nS]
 Real  MaxS=0;
 Field Fm={ 0,0,nS-1,0 };

  Spc=(Real*)Allocate( nS*sizeof( Real ) );

  if( mode&0x1000 )Maximum_Entropy_Spectrum( V,N,Spc,nS,MaxS );
           else                FFT_Spectrum( V,N,Spc,nS,MaxS );
  fM.Jx=fM.Jy=0;
  fM.Lx=Tport.left-3-Tv.mX;
  fM.Ly=-Tv_port.bottom-Tv.Th-2; Fm.Ly=MaxS;

  color( LIGHTGRAY  );
  Tv_place( &fM,&Fm );
  color( LIGHTRED   );
  for( k=0; k<nS; k++ )if( !k )moveto( Real( k ),Spc[k] );
                          else lineto( Real( k ),Spc[k] );

  color( MAGENTA );
  for( k=1; k<12; k++ )
  { line( Real( k*nS )/12,0.0,
          Real( k*nS )/12,MaxS/6 );
    Tw.Text( North_East,
          Real( k*nS )/12,0.0,DtoH( k*dt ) );
  }

  Tv_place( 0,&Fm );
  Tv_place( &fM   );
  Allocate( 0,Spc );
}

static IKA=1;

void    FFT( int,Real* );
void KIZERS( int,Real* );

static exp2( int p ){ return 1<<p; }
static log2( int n )
{ int k=0x8000,l=15; while( (k&n)==0 ){ l--; k>>=1; } return l;
}

void FFT_Spectrum
( Real *_X,  int nD,   // ���� ������� ������
  Real *SX,  int nS,   // ������������ ������� �� ������ (^2)
  Real &Max            // ��������� � ���������� ��������� �������
)
{
// -------------------------�
// �                        �
// � ������ ������� ������� �
// �                        �
// L-------------------------
 Real *AA,*X,delt;
 double RX;
 int l,k,K,N,MM,N8;
  N=(nS-1)*2;
  if( N<=0 || N>nD )N=nD;
  K=log2( N );          // ������� �������������� FFT
  N=exp2( K );          // ����� ���� FFT
  MM=N/2;               // ����� ���� ��� ��������� ����� ����
  N8=nD/MM;             // ���������� ��������� ����������
  nD=N8*MM;             // ��������� ����� ����
  delt=Real( N*2 )/N8; N8--;
  Max=0.0;

  X =(Real*)Allocate(  N*sizeof( Real ) );
  AA=(Real*)Allocate( (N+1)*sizeof( Real ) );
  RX=0.0;                         //
  for( k=0; k<nD; k++ )RX+=_X[k]; // ������� ������ �������� (�������)
  RX/=nD;                         //
                                  // ����������� ������������� ���� �������
  if( IKA )KIZERS( N+1,AA ); else // ���� ���� ������� �� ������������
  for( k=0; k<=N; k++ )AA[k]=1.0; // ������������ ������� ������� 1.0
  for( K=0; K<N8; K++ )
  { for( l=MM*K,k=0; k<N; k++ )X[k]=(_X[k+l]-RX)*AA[k];
    FFT( N,X );
    for( k=1; k<=MM; k++ )
    { // --------------�
      // � ����������� �
      // L--------------
      SX[k]+=(X[k]+X[nS-k])*sqrt( Real( k )/MM )/2;
    }
    // -----------------------------�
    // � ������� ����� ������������ �
    // L-----------------------------
    SX[0]=0; //+=fabs( X[0] );
  }                     //+=R*R;
  for( k=0; k<=MM; k++ )
  { RX=SX[k]*=delt;     // ���������� ������������
    if( Max<RX )Max=RX;
  }
  Allocate( 0,AA );
  Allocate( 0,X );
}
//========================= English ======================
  Twindow( 2,-2,28,25,4 );             color( YELLOW );
  T( "  <<Mariograms Treatment>>\n" ); color( WHITE );
  T( "   L  List of Records"        );
  T( "  R/W <Ani> special format"   );
  T( "  I/O for other data files"   );
  T( "  XYZ <Ani><Grid><Mesosaur>\n"); color( YELLOW );
  T( "  F Three - function groop"   ); color( LIGHTCYAN );
  T( "  T Tidal value collection"   );
  T( "  S visible configurations"   );
  T( "  E initial present series"   );
  T( "  C exact copy this series"   );
  T( "  M move over mark or down"   );
  T( "  D deleting market series\n" ); color( LIGHTGREEN );
  T( "  <Enter>  series editor"     ); color( LIGHTCYAN );
  T( "  <Up-Down-Ins>mark series"   );
  T( "  <Home-End-Del>  boundary"   );
  T( "  <Left-Right> bound shift"   );
  T( "  <PgUp-PgDn> window shift"   );
  T( "  <0|1> Series|Screen step"   );
  T( "  <2:9> Step multiplier\n"    ); color( LIGHTGRAY );
  Tln(); Tpoint.y-=4;
  Tprintf( "   Use: %lu + %luk memory",(Mmem-M)>>10,M>>10 );
#endif
