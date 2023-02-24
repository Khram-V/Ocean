//
//      ��������� ���������� ����������� ����� ��� �����������
//         �������� �������� ������������� �������� ������
//
#if 0
#include <StdIO.h>
#include <String.h>
#include "..\Ani\LongWave.h"
#include "..\ANI\Tide.h"
static FILE *F=NULL;

static void PutString( char* str ){ if( putc( char(strlen( str )),F )>0 )fputs( str,F ); }
static void GetString( char* str ){ int l=getc( F ); if( l>0 )fgets( str,l+1,F ); }

void TWave::save()
{
 int k;
  remove( "Ani.~hk" );
  rename( "Ani.chk","Ani.~hk" );
  if( (F=Chart.save( "Ani.chk" ))==NULL ){ Tv_bell(); return; }
  fwrite( "\r\n<<Wave>>\n\r",1,12,F );                  //
  fwrite( this,1,sizeof(TWave)-(13*sizeof(float*)),F ); // ���.���������
  if( Na>0 )fwrite( Ma,sizeof( METEO ),Na,F );  // ����������� ������
  put2( Number_Tides,F );                       // ���� ��������
  if( Number_Tides>0 )                          //
  { fwrite( &Time_Zone,sizeof( float ),1,F );
    fwrite( List_of_Tide,sizeof( Tide ),Number_Tides,F );
    for( k=0; k<Number_Tides; k++ )
    { Tide &M=List_of_Tide[k];
      if( M.L>0 )fwrite( M.H,1,M.L,F ); fwrite( M.C,1,M.L*8+4,F );
  } }
  for( k=0; k<Ny; k++ )
  { fwrite( Hg[k],sizeof( Real ),Nx,F );
    fwrite( Uy[k],sizeof( Real ),Nx,F );
    fwrite( Ux[k],sizeof( Real ),Nx,F );
  } fclose( F );
}




//
//      ��������� ���������� ����������
//              ��������� �������� ���� ������ ��� ����������
//              ����������� ����� ��� ������������� �������� ������
//
FILE *Depth::save( const char* PathName )
{
 char Name[MAX_PATH];
  fext( strcpy( Name,!PathName ? InfName:PathName ),"chk" );
  OemToCharBuff( Name,Name,strcut( Name )+1 );

//  if( !PathName )PathName=InfName;
//  if( (F=Topen( PathName,"wb","chk" ))==NULL )return NULL;

  if( (F=fopen( Name,"wb" ))==NULL )return NULL;
  fwrite( "ANI�chk�",1,8,F );        //
  PutString( Msg[0] );               // Comments
  PutString( Msg[1] );               //
  fwrite( &Base,1,sizeof(Point),F ); // Latitude & Longitude
  fwrite( &Step,1,sizeof(Point),F ); // Grid's Steps
  fwrite( &Fe,  1,sizeof(float),F ); // Average (middle) Latitude of chart
  fwrite( &Cs,  1,sizeof(float),F ); // Meridian's Course (�-degree)
  fwrite( &Geo, 1,sizeof(Basis),F ); // ����� �������� � ������������
                        //
  put2( Active,F );     // 1:Read; 2:Edit
  put2( Typ,F );        // 0:Text; 1:Byte; 2:Integer
  put2( Ny,F );         // Jy,Jx,Ly,Lx
  put2( Nx,F );         //   Field's Size & Sector
  putc( Ms,F );         // '-minute d-degree k-kilometre m-metre
  put2( (int)Max,F );   // Bathimetry
  put2( (int)Min,F );   //   Extrema
  put2( Nmp,F );         // Number of Marigrams
  fwrite( M,sizeof( MARIO ),Nmp,F );             // Marigrams
  for( int i=0; i<Ny; i++ )                     //
    fwrite( __Depth[i],sizeof(float),Nx,F );    // Depth Data
  return F;                                     //
}
//      ��������� �������������� ����������
//              ��� ������ ��������� � ��������� ( Ny=Nx=0 ) � ����
//              ������������ ������� ���������� ������������ �����,
//              �� ������������ Activ|=0x8000 � ����� ��������� �����
//              ���������� �� Chart.Fs.
//
int Depth::restore( char* PathName )
{
  int First=(Ny==0);
 char Msgs[24];
  if( (F=fopen( PathName,"rb") )==NULL )return 0; fread( Msgs,1,8,F );
  if( strncmp("ANI�chk�",Msgs,8 ) ){ fclose( F ); return 0; }
  GetString( Msg[0] ); strcpy( InfName,fext( PathName ) );
  GetString( Msg[1] ); strcpy( Name,InfName );
                                      //
  fread( &Base,1,sizeof( Point ),F ); // Latitude & Longitude
  fread( &Step,1,sizeof( Point ),F ); // Grid's Steps
  fread( &Fe,  1,sizeof( float ),F ); // Average (middle) Latitude of chart
  fread( &Cs,  1,sizeof( float ),F ); // Meridian's Course (�-degree)
  fread( &Geo, 1,sizeof( Basis ),F ); // ����� �������� � ������������
                                //
  Active=get2( F );             // 1:Read; 2:Edit
  Typ = get2( F );              // 0:Text; 1:Byte; 2:Integer
  Dat = 2;                      // 0:Var; 1:Line; 2:Field; 3:Space
  Ny  = get2( F );              // Jy,Jx,Ly,Lx
  Nx  = get2( F );              //   Field's Size & Sector
  Ms  = char( getc( F ) );      // '-minute d-degree k-kilometre m-metre
  Max = get2( F );              // Bathimetry
  Min = get2( F );              //   Extrema
  Nmp = get2( F );              // Number of Marigrams
  switch( Ms )                                       // ����� � ����������
  { case 'k' : case 'm': Map=(char*)Mercator; break; // ���� ����������������
    case '\'': case 'd': Map=(char*)Geograph; break; // ��������
   default   :           Map=(char*)UnKnown;         //
  }
   M =    (MARIO*)Allocate( Nmp*sizeof( MARIO ),M   );
 __Depth=(float**)Allocate( Ny,Nx*sizeof(float),__Depth );         //
  fread( M,sizeof( MARIO ),Nmp,F );                                 // Marigrams
  for( int i=0; i<Ny; i++ )fread( __Depth[i],sizeof(float),Nx,F ); // Depth Data
  if( First )                                                      //
  if( fread( Msgs,1,12,F )==12 )                 // �������� �� �����������
  if( strncmp( "\r\n<<Wave>>\n\r",Msgs,12 )==0 ) // �������������� ������
  {   Active|=0x8000;   //
      Fs=F;             // �������� ������ ��������� ����� ��� �����������
      return 1;         //
  } fclose( F ); return 1;
}
void TWave::save()
{
 int k;
  remove( "Ani.~hk" );
  rename( "Ani.chk","Ani.~hk" );
  if( (F=Chart.save( "Ani.chk" ))==NULL ){ Tv_bell(); return; }
  fwrite( "\r\n<<Wave>>\n\r",1,12,F );                  //
  fwrite( this,1,sizeof(TWave)-(13*sizeof(float*)),F ); // ���.���������
  if( Na>0 )fwrite( Ma,sizeof( METEO ),Na,F );  // ����������� ������
  put2( Number_Tides,F );                       // ���� ��������
  if( Number_Tides>0 )                          //
  { fwrite( &Time_Zone,sizeof( float ),1,F );
    fwrite( List_of_Tide,sizeof( Tide ),Number_Tides,F );
    for( k=0; k<Number_Tides; k++ )
    { Tide &M=List_of_Tide[k];
      if( M.L>0 )fwrite( M.H,1,M.L,F ); fwrite( M.C,1,M.L*8+4,F );
  } }
  for( k=0; k<Ny; k++ )
  { fwrite( Hg[k],sizeof( float ),Nx,F );
    fwrite( Uy[k],sizeof( float ),Nx,F );
    fwrite( Ux[k],sizeof( float ),Nx,F );
  } fclose( F );
}
int TWave::restore()
{
 const int Ftl=sizeof( TWave )-( 13*sizeof( float* ) );
 int k;                                           //
  if( (Chart.Active&0x8000)==0        )return 0;  // ���� 0, �� ������� ����
  if( (F=Chart.Fs)==NULL              )return 0;  // �� ������������� �����
         Chart.Fs=NULL;                           // ����������
  if( ferror( F )                     )return 0;  //
  if( (int)fread( this,1,Ftl,F )!=Ftl )return 0;  // �������������� ���������
      Mg=Chart.M;                                 //
  if( Na>0 )
  { Ma=(METEO*)Allocate( Na*sizeof( METEO ) );
    fread( Ma,sizeof( METEO ),Na,F );   // ����������� ������
  } else Ma=0;                          //
      Number_Tides=get2( F );           // ���� ��������
  if( Number_Tides>0 )                  //
  { List_of_Tide=(Tide*)Allocate( Number_Tides*sizeof( Tide ) );
    fread( &Time_Zone,sizeof( float ),1,F );
    fread( List_of_Tide,sizeof( Tide ),Number_Tides,F );
    for( k=0; k<Number_Tides; k++ )
    { Tide &M=List_of_Tide[k]; M.H=0;
                               M.C=0;  M.allocate( M.L );
      if( M.L>0 )fread( M.H,1,M.L,F ); fread( M.C,1,M.L*8+4,F );
  } }
  for( k=0; k<Ny; k++ )
  {    fread( Hg[k],sizeof( float ),Nx,F );
       fread( Uy[k],sizeof( float ),Nx,F );
   if( (int)fread( Ux[k],sizeof( float ),Nx,F )!=Nx )
       Break( "?Bad check file" );
  } fclose( F );
  if( (WaveIs&3)==0 && Na<=0 )T=0.1; // ����� �������� �������
  WaveIs&=~0x1000;             // ������ �������� �������� ������ ����������
  Kt=0;                        // ������� �������� ���������� ����������� �����
  return 1;                    //
}
#endif
