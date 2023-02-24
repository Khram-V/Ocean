//
//!     ���� ��������� ����������
//
//      ��� ������ ������ �������� ������ �� ����������� ����� �����
//      .mar � ������������� �������� ������� ��� ������������ �����.
//       dT - ��������� ��� ��������������� ��������,
//            ������ ������ ������� � ������������� ����� �����
//            ������� �������� ����� � ����� 1 ������ ��������� �����.
//
#include <Time.h>
#include "..\Ani\LongWave.h"
#include "..\Ani\Tide.h"
                                    //
static FILE *Mf=0,*L=0;             // �������� ����� ���������� � ������
static double T1=0;                 // ����������� ����� ��� ������ ����������
static float *oM=0;                 // ������ ������� �������: { H, Vx, Vy }
static int dy,dm,dd,It=0;           // ������� ���� ������������ �������
                                    //
static char MarioName[MAX_PATH]=""; // ����� ��� �������� ������
                                    //
void TWave::Print_the_Time()
{
//   ������� ���� � ����� ��������� ������
//
 if( It )return; It=1;
  Instant.UnPack( dm,dd,dy );
  fprintf( L,"\n%s %d ",W2D( _Week[unsigned(Instant.D%7)] ),dy );
  fprintf( L,"%s %02d (%ld Julian day)\nThe Time is%s",
           W2D( _Month[dm-1] ),dd,Instant.D,DtoA( Instant.T,3,':' ) );
  if( !Kt )fprintf( L," at Starting\n" ); else
  { long J=int( Kt*Tstep/86400.0 );
    double t=Kt*Tstep/3600.0-J*24;
           fprintf( L,", elapsed" );
    if( J )fprintf( L," %ldd ",J );
           fprintf( L,"%s for a %ld steps and %.0lf records\n",
           DtoA( t,-3,':' ),Kt,Tstep*Kt/dT );
} }
void TWave::WaveIn_state( fixed Mode=0 )
{                   //! ������������ ���������� ��������������� �����
                    // 0xFF - ��������� ��������� � ������ �����, �� ��� ������
                    // 0x01 - ������ ������� ����� ���������� �����
                    // 0x02 - ������ ����� ������������� �����
                    // 0x04 - ������ ��������� �������
                    // 0x08 - ������ ��������� � �������������� �������
                    // 0x10 - ������ ���������� ������������ �������
                    // 0x80 - �������� ����� ( ���������� ������������ )
                    //
 static fpos_t Fpt=-1; // ��������� ����������� ������� � Mif
//
//      ������������ ��������� ��������������� �����
//
      L=0;             // ���� ��������������� ��������
  if( Mode==0xFF )
  { if( ( L=Topen( fext( MarioName,"mif" ),"a+" ) )!=0 )
    { time_t t=time(0); fprintf
      ( L,"\n<< ANI >>  Ocean Wave Simulation, %s~~~~~~~~~",ctime( &t ) );
      Fpt=ftell( L );
    }
  } else if( Fpt>=0 )L=fopen( fext( MarioName,"mif" ),"r+" );

  if( !L )return; It=0; fsetpos( L,&Fpt );
//
//  ��������� ��������������� ��������
//
 static fixed W=0;
  if( Mode==0xFF || ((W&0x700)!=(WaveAs&0x700)) )
  { Print_the_Time(); W=WaveAs;
    fprintf( L,"\nUse the %s Equations in Flow variables",
               W&0x100 ? "Spherical Earth" : "Simple" );
    fprintf( L,"\n     %s Coriolis Forces",W&0x200?"Including":"No" );
    fprintf( L,W&0x400
             ? "\n     Viscosity Friction Coefficient is %lg\n"
             : "\n     No Fluid Viscosity\n",Kvs );
  }
//  ������� ��������� ������� ��������� �� ����������
//
  if( Mode&0x02 )
  { Point P={ 0,0 }; Real mB=max( DZ,1.0 ); Chart.ItD( P );
    Print_the_Time();
    fprintf( L,"\nFile: %s"
               "\n~~~~: %s"
               "\nName: %s"
               "\n~~~~: %s\n\n",
                 Chart.InfName, Chart.Name, Chart.Msg[0], Chart.Msg[1] );
    fprintf( L,"Frame Direction is %-11s\n",
                                 !Chart.Cs?"North":DtoA( Chart.Cs ) );
    fprintf( L,"Latitude of Scale: %-11s  Size [%dx%d]\n",
                                      DtoA( Chart.Fe*Radian ),My,Mx );
    fprintf(L,"  Basis Latitude : %-11s    dY %lgkm\n",DtoA(P.y),Dy/1000);
    fprintf(L,"       Longitude : %-11s    dX %lgkm\n",DtoA(P.x),Dx/1000);
    fprintf(L,"  Compute/Record : %s",                DtoA(Tstep/3600,-3,':' ));
    fprintf(L," /%s Time Step\n",                     DtoA(dT/3600,-3,':' ));
    fprintf(L,"Wave 6pts Period : %s\n",
                                 DtoA( max( Dy,Dx )/sqrt( mB*9.8106 )/600,3,':' ) );
    fprintf(L," Depth Limitation:  %lg m\n",mB );
  }
//  ����������� ��������� �������
//
  if( Mode&4 )
  { Print_the_Time();
    if( Wa && WaveAs&1 )
    { fprintf(L,"\nExternal Waves Generation"
                "\nPeriod:%s\tAmplitute %.2lfm with %lg Half of Waves"
                " in the Group",            DtoA( Wt/3600,3,':' ),Wa,Wk );
      fprintf(L,"\n~~~~~~\t\tDirection%s",  DtoA( Wc ) );
      fprintf(L,"\n\t  Initial Time is%s",  DtoA( W0/3600,3,':' ) );
      fprintf(L,           ", Duration%s\n",DtoA( (Tlaps-W0)/3600,3,':' ) );
    } else
    fprintf(L,"\nNot Defined a Boundary Conditions by Period of Waves\n" );
  }
//  ����������� ��������� � �������������� ��������� �������
//
  if( Mode&8 )
  { Print_the_Time();
    if( !Va )
    fprintf( L,"\nNot Inspected Initial Condition or Geometry Sources\n" ); else
    { Point P={ Vo.x/Dx,Vo.y/Dy };
      int y=minmax( 0,int( P.y ),My-1 ),
          x=minmax( 0,int( P.x ),Mx-1 ); Chart.ItD( P );
      fprintf( L,"\nSource in Latitude %-10s   Place{%0.1lf,%-0.1lf}pts,"
                 " on Depth=%gm & Slope %0.2lf of Seafloor",
                            DtoA( P.y ),Vo.y/Dy,Vo.x/Dx,Chart[y][x],Glide );
      fprintf( L,"\n~~~~~~   Longitude%-11s   Size [ %4gx%-4g ]km",
                                  DtoA( P.x ),Vs.y/500,Vs.x/500 );
      fprintf( L,"\n  Period in Source%-11s   Height=%lgm",
                                  DtoA( Vt/3600.0,-3,':' ),Va );
      fprintf( L,"\n   Initial Time is%s",DtoA( V0/3600,3,':' ) );
      fprintf( L,          ", Duration%s",DtoA( (Tlaps-V0)/3600,3,':' ) );
      if( Mag>0.0 )
      fprintf( L,"\n  Magnitude of Earthquake: %0.1lf",Mag );
      fprintf( L,"\n This is %s",
        WaveAs&2?"Progressive Wave Group":"Initial Sea Rising, Modulated" );
      fprintf( L," with %lg Half of Waves\n",Vk );
    }
  }
//  ������ �������� � ������������� ������������ �������
//
  if( Mode&0x10 )
  { Print_the_Time();
    if( Na<1 )fprintf( L,"\nAtmosphere Cyclone not Defined\n" ); else
    { fprintf( L,"\nAtmosphere Cyclone on %d Basis Points:\n"
      ".. Day & Time  Geo.Coordinetes Size[km] P[wat'cm] Wind[m/c] Stress\n",Na );
      for( int k=0; k<Na; k++ )
      { int Day=int( Ma[k].T/86400.0 );
        fprintf(L,"%2d,%3d%7s",k+1,Day,DtoA(Ma[k].T/3600-Real(Day)*24,2,':'));
        fprintf(L," %7s",                            DtoA( Ma[k].G.y,2 ) );
        fprintf(L,"%8s  %8.2lf %9.1lf %9.1lf %6lg\n",DtoA( Ma[k].G.x,2 ),
                         Ma[k].R/500,Ma[k].H*100,Ma[k].W,Ma[k].Avs );
      }
    }
  }
//  ������� ������ � ������������ ������ ����������
//
  Fpt=ftell( L );
  if( Mode!=0xFF && Nmp>0 )
  { int k,m;
   MARIO *M;
    if( (Mode&0x02)==0 )Print_the_Time();
    fprintf( L,"\n Latitude  Longitude %4d Marigraph Stations  Depth Dir/Dist"
               "    Initial\\Extremum  +Max/-Min  Speed Course"
               "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m]~~~[\xF8]"
               "/[km]~~~[h:m's\"]~~~~~~~~~~~~[m]~~~~~~~~[m/s]~~[\xF8]",Nmp );
    for( k=0,M=Mg; k<Nmp; M++,k++ )
    { int y,x;
      Real DC;
      Point P=M->G; Chart.D2I( P,y,x ); DC=1.0;
      fprintf( L,"\n%-10s",     DtoA( M->G.y,3 ) );
      fprintf( L,"%-12s %-20s ",DtoA( M->G.x,3 ),M->cp );
      if( y>=0 && y<My && x>=0 && x<Mx )
      { DC=Chart[y][x];
        fprintf( L,fabs( DC )>1000.0 || !( DC-int(DC) )?"%6.0lf":"%6.1lf",DC );
        if( M->D>0.0 )fprintf( L,"%6s/%-5.1f",DtoA( M->A,1 ),M->D );
               else  fprintf( L,DC>DZ?" -open sea- ":" -coastal-  " );
      }        else fprintf( L,"alien             " );
      m=wTm[k]==0.0;
      fprintf( L,"%10s",  m?"calm ":DtoA( wTm[k],3,':' ) );
      fprintf( L,"%10s",m?""     :DtoA( mTm[k],3,':' ) );
      if( mxH[k]-mnH[k]>=0.01 || mFl[k]>=0.01 )
      fprintf( L," %4.2f/%-4.2f %6.2f %5s",mxH[k],1e-3-mnH[k],mFl[k]/DC,DtoA( aFl[k],1 ) );
    } fputc( '\n',L );
  }                                                                //
  if( Mode==0x01 )Fpt=ftell( L );                                  // Signature
  { time_t t=time( 0 ); fprintf                                    //
       ( L,"\n\n(c)86-17, V.Khramushin, Sakhalin, Russia, %s\n",ctime( &t ) );
  }
  if( Mode==0x80 )
  { Allocate( 0,mTm );
    Allocate( 0,wTm );
    Allocate( 0,aFl ); if( Mf ){ fclose( Mf ); Mf=0; } Fpt=-1;
    Allocate( 0,mFl );  //
    Allocate( 0,mnH );  // ������ ��������� ����������� ������,
    Allocate( 0,mxH );  // ���������� ������������ ������ ��������� Tsunami
    Allocate( 0,oM );   //
  } fclose( L );
}                       //
static int MAR=0,mk=0;  // ������� ����������
static double Mk=0;     // ����� ���������� ���������� �����������
                        //
int TWave::Mario_Init() // ������� ������� ����������
{                       //
  if( Kt==0 || ( !MAR && WaveAs&0x1000 ) )
  {
   static Real t1,t2,tm; int k,l;
   Mlist Menu[]
   ={ { 1,38,0,MarioName }
    , { 1,4,"Event is %4d",&dy },{ 0,2,"-%02d",&dm },{ 0,2,"-%02d",&dd }
    , { 0,7,",%2�",&tm },        { 0,9," \x1E%-3�",&dT }
    , { 1,7,"      Time of Tide: %2�",&t1 }, { 0,7," <%2�",&t2 } };
    T1=Tlaps;
    MAR=mk=0;                                            //
    WaveAs&=~0x1000;                                     // ������� ����������
    if( Tlaps==0.0 )                                     //
      { Mk=onetime(); dT=Tstep; if( !Instant.D )Instant.Now();
      }
    Instant.UnPack( dm,dd,dy ); color( CYAN );
    for( k=5;; )
    { Thelp( "%78s","F9 Allow  F10 PutOff  <Escape>Cancel" );
      if( MarioName[0]<=' ' || MarioName[0]=='~' )
        { MarioName[0]='~';             // ������������ ������������� ��� �����
          strcpy( MarioName+1,sname( Chart.InfName ) );
        }
      if( Tk>=0 )                   //
      { int i; Real W;              // ����� ������� ������ ��� ��������� �����
        Tide &TT=List_of_Tide[Tk];  //
        t1=t2=Real( int( Instant.T*60.0+0.01 ) );      // � ��������� �� ������
        W=TT.TidePoint( Instant.D,Instant.T=t1/60.0 );
        for( i=0; i<720; i++ )
         if( TT.TidePoint( Instant.D,( --t1 )/60.0 )*W<=0.0 )break;
          t1/=60.0;
        for( i=0; i<720; i++,t2++ )
         if( TT.TidePoint( Instant.D,( t2+1.0 )/60.0 )*W<=0.0 )break;
          t2/=60.0;
      }
      dT/=3600.0; tm=Instant.T; k=Tmenu( Menu,Tk<0?6:8,0,-1,k );
      dT*=3600.0;
      if( dT<Tstep )dT=Tstep;
      Ghelp();
      if( k==6 )tm=t1;
      if( k==7 )tm=t2;
      Instant.T=tm;
      Instant.Pack( dm,dd,dy );
      Instant.UnPack( dm,dd,dy );
      if( k==_Esc )return -1;               // ������ ������
      if( k==0xFA )return 0;                // ����� ��� ������
      if( k==0xF9 && *MarioName>' ' )break; // ����� �� ������
    }                                       //
    Ghelp();
    OemToCharBuff( MarioName,MarioName,strcut( MarioName )+1 );
    if( (Mf=Topen( fext( MarioName,"mar" ),"wb" ))!=0 )
    {                    //
     MARIO *M;           // ������������ ��������� ����� ����������
     char cpt[22+122];   // "Mario�"
     static char Id[8]="Mar\xFA"; WaveAs|=0x1000; Active|=as_Simula;
        ((int*)Id)[1]=Nmp;        //! ============= format Ver.3 (2016)
//
//    ������������ ����� ����������
//
      time_t t=time(0);
      char *ct=ctime( &t ); ct[strlen( ct )-1]=0;
//
//    ������������ ������� ��������� ����� ����������
//
      fwrite( Id,1,8,Mf );                        // "Mar\x10" + Nmp ����������
//
//    ��������� � ����� ��������� XML
//
      for( M=Mg,k=0; k<Nmp; M++,k++ )
      { int x,y;
        Real D=0;
        Point P=M->G; Chart.D2I( P,y,x );
        if( unsigned( y )<unsigned( My )
         && unsigned( x )<unsigned( Mx ) )D=Chart[y][x];
//
//      ��������� ���������������� �������� �������
//
       double Dout[]={ Instant.T,dT/3600 }; // ����� ������ � ��� ����������
       float  Fout[]={ M->G.x,M->G.y,       // �������������� ������ � �������
                       D,M->A,M->D          // ������� ����, ������
                     };                     //       � ��������� �� �����
        l=strlen( strcpy( cpt,M->cp ) );
        if( l<20 )memset( cpt+l,' ',20-l );
        fwrite( cpt,       1,20,Mf ); // Identify
        fwrite( &Instant.D, 1,4,Mf ); // Julian day
        fwrite( &Dout,      2,8,Mf ); // ����� � ������������ ������
        fwrite( &Fout,      5,4,Mf ); // ����������, ������� � ��������� ������
      }                               // = ����� ����� 60 ����
//
//    ���������� ���������� � ������ ������������ ������ ������
//    ������������� ������ ��� �������� �������� ������������� ������
//
      MAR = BLUE;
      oM =(float*)Allocate( Nmp*sizeof( float ) ); // ���������� ������ ������
      mxH =(Real*)Allocate( Nmp*sizeof( Real ) ); // ������������� ������
      mnH =(Real*)Allocate( Nmp*sizeof( Real ) ); //   � ������ Mg [����]
      mFl =(Real*)Allocate( Nmp*sizeof( Real ) ); // ������������ �����
      aFl =(Real*)Allocate( Nmp*sizeof( Real ) ); //    � ��� �����������
      wTm =(Real*)Allocate( Nmp*sizeof( Real ) ); // ����� ������ �����������
      mTm =(Real*)Allocate( Nmp*sizeof( Real ) ); //    � ��������� �����
      WaveIn_state( 0xFF );
      return 1;
  } } return 0;
}
void TWave::Mariom()    //
{ if( Nmp>0     )       // ? ������� ����������
  if( MAR && Mf )       // ? ���� ������ � �� ������������
  {                     //
   int k,y,x,ly,lx;     //
   Real Fl,H,DH,wH,d,t; // ���������� ��� ����������� ����� ������������
// Point  wV;           //        ��������� ������ � �������� �� �������
   MARIO *M;            //
    for( k=0,M=Mg; k<Nmp; M++,k++ )
    { Point V=M->G; Chart.D2I( V,y,x );
      V=0;              // ������� ��� ��������
      DH=H=0;           // ������� ��� ������� � ������ ����
      if( unsigned( y )<unsigned( My ) && unsigned( x )<unsigned( Mx ) )
      if( (DH=Dp[y][x])>DZ )
      { H=Hg[y][x];
        if( wTm[k]==0 )if( fabs( H )>0.01 )wTm[k]=Tlaps/3600;
        if( mxH[k]<H ){ mxH[k]=H; mTm[k]=Tlaps/3600; }else
        if( mnH[k]>H )  mnH[k]=H;
        ly=lx=0;
        if( !y )y=1; else if( y==My-1 )y=My-2; else ly=1;
        if( !x )x=1; else if( x==Mx-1 )x=Mx-2; else lx=1;
        V.x=( Ux[y][x]+Ux[y][x+lx] )/DH/2,
        V.y=( Uy[y][x]+Uy[y+ly][x] )/DH/2;
        if( (Fl=abs( V ))>mFl[k] )
        { mFl[k]=Fl;
          aFl[k]=Angle( 90.0 - arg( V )*Radian + Chart.Cs );
        }                            //
        if( T1<=Tlaps )              // ? ����������� ����� ���������
        for( t=T1; t<=Tlaps; t+=dT ) //
        {  d=( Tlaps-t )/Tstep;      // ! ������������ �� ������ ������ �������
          wH=H-(H-oM[k])*d;          // ����� ������� ������ ����
//        wV=V-(V-oV[k])*d;          //       � ��������� �������
//        if( (Fl=abs( wV ))==0 )DH=0;
//         else DH=Angle( 90.0 - arg( wV )*Radian + Chart.Cs );
//        if( xml.One( "m","%d %lg %lg %3.2lf",k+1,wH,Fl,DH )<0 ){ MAR=0; break; }
        }
      } oM[k]=H; //oV[k]=V;
    }                            //
    if( T1<=Tlaps )                  // ? ����������� ����� ��������� + ������ �������
    { if( (int)fwrite( oM,sizeof(float),Nmp,Mf )<Nmp )MAR=0; else
      if( ++mk>3 )                     //
      if( int((Fl=onetime()-Mk)*60 ) ) // 1.min �������� ���
      { WaveIn_state();                //  ���������� ������
          mk=0;                        //
          Mk=Fl;
        if( Mf )fflush( Mf );
//        fclose( Mf );
//        Mf=fopen( fext( MarioName,"mar" ),"ab" );
      }
      color( MAR^=3 );           // ��������� ����� ������� �������� �������
      while( T1<=Tlaps )T1+=dT;
    }
  } // ready to write
}
