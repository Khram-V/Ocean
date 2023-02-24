/**
        ������ ������ �� XML ����� � ����������� ����������
               ���������������������� ����������
                                                 Sakh-Xml, ���,2008 - ����,2009
**/
#include "Sakhalin.h"
#include "..\Matrix\XML.h"
#include "..\Matrix\Function.h"

#define NextEcoWord   word = Eco.Word();   if( *word ) //if( word )if( word[0] )
#define NextEcoString word = Eco.String(); if( *word ) //if( word )if( word[0] )
                           //
static inXML Eco;          // ������ � ����� XML
                           //  - ������������ ����� �������� ������
static char str[MAX_PATH]; // ������� ��������� ������ ������������ �����
                           //
struct Meteo_Point         //
{ Event  DataTime;         // ����� - �������� ������� ������� ����������
  float  Min,Mid,Max;      //  �����������, ����������� � ������������ ��������
};                         //    ��� ������������������ ������������� ������
struct Meteo_Series: Item //
{ short PntNum,           // ����� ���������� ���� �����������
        Mask;             // ����� ���� �������� ������� � ������ ������
                          // 0�00 - ������ ��������� � ������������� ���������
                          // 0�01 - ��������������
                          // 0�02 - �������������� ������
                          // 0x03 - ������ �������� ���� ������� ����������
  Meteo_Point *M;         // ���������� ������ (���� ���) �����������
  char *dataKey,          // ���� ������� � ����������� � ��������� ������
       *dataName,         // ��������� ����������� �������������� ���������
       *dataMeas;         // ���������� ����������� �������������� ��������
  float Pdk;               // ��������� ���������� ������������
  Meteo_Series( char* );  // ����� ������ ���� �������� ������ � ������� ������
  void Draw();            //
  void Extrem();          //
};
//      �����, ����� � ����� ��������� ��������� ��������-����������������
//      ���������������� (���� �� ������������ ����� ��� ���������� �������)
//
      bool PdkModeView=true;  // ����������� � �������� ��� ��� - ��� ����
static int   nMax=0,          // �������� ������� ��� ����������
             sMax=0, sDrw=0,  // ����� ���������� � ������� ���-��������
             xMask=0;         // ����� �������� ������� �������� ������� �������
static Event xMin(0),xMax(0); // ��������� �������� �������� �����������
static float  yMin=0, yMax=0,  // ������ ���������� �������� �������
             yScale=1.0;      // ������� ��� ����������� ������� �� ���
static double initTime()      //
{ double dT; if( xMask>2 )dT=1.0/60.0;  else       // ?? ���� � ������
             if( xMask>1 )dT=1.0;       else       // ?? ����� + ���
             if( xMask>0 )dT=24.0*7.0;  else       // ?? ������ +������
                          dT=24.0*30.0; return dT; // ?? ���� + �����
}
static double deltaTime() // ����� ������� � ������ ����, ���
{ double dT=xMax-xMin,    // ��� ���������� �������� ��������������
         iT=initTime(); if( dT<iT )dT=iT*2.0; return dT;
}
static int PdkColor( float Lvl )
{ int c=LIGHTBLUE; if( Lvl>6.0 )c=LIGHTRED;   else
                   if( Lvl>2.0 )c=43;         else // ORANGE - ���������
                   if( Lvl>1.2 )c=44;         else // YELLOW - ������
                   if( Lvl>0.8 )c=LIGHTGREEN; else
                   if( Lvl>0.1 )c=LIGHTCYAN;  color( c ); return c;
}
void Meteo_Series::Draw()
{ if( PntNum<=0 )return;
 double DW=deltaTime()/100.0, // ������� ��������� �� ������� �����������
        dw=0.25,sD,W;         //=400 - ���������� ����� ��������� ��� �������
 int bc=bkcolor( WHITE );
  if( PdkModeView && Pdk>0.0 )sD=Pdk; else sD=1.0;
  if( nMax<2 )                // ���� ��� ����� ���� ����������� ����������
  { int x = Tv_x( 100.0/(sMax+1.0) + 100.0*sDrw/(sMax+0.5) ),
        y = Tv_y( (W=M[0].Mid)/sD ),z;
        Tv.Height( (Tv_port.right-Tv_port.left)<640?8:16 );
        if( !Pdk )color( DARKGRAY ); else pattern( INTERLEAVE_FILL,PdkColor( W/Pdk ) );
        setlinestyle( SOLID_LINE,0,2 );
        Tv.Text( North_West,x,y-4,dataKey );
        bar3d( x-12,y,x,Tv_port.bottom,6,1 ); x+=3;
        setlinestyle( DOTTED_LINE,0,THICK_WIDTH );
        if( (W=M[0].Min)>0.0 )
        { z = Tv_y( W/sD );
          if( !Pdk )color( DARKGRAY ); else PdkColor( W/Pdk ); needle( x,y,x,z );
        }
        if( (W=M[0].Max)>0.0 )
        { z = Tv_y( W/sD );
         if( !Pdk )color( DARKGRAY ); else PdkColor( W/Pdk ); needle( x,y,x,z );
        }
        setlinestyle( SOLID_LINE,0,NORM_WIDTH );
        pattern( SOLID_FILL,WHITE ); // bc ??
        Tv.Height( 0 );
        sDrw++;
  } else
  {
   int k,N=PntNum;
   float w,wn;
   Function T( PntNum );
    for( k=0; k<N; k++ )T.X( k )=(M[k].DataTime-xMin )/DW; // -- ��������
                      w=T.X(0);                      // ��������� �
                     wn=T.X(N-1)+dw;                 // �������� ����� �������
    //
    //  ������ ���������
    //
    for( k=0; k<N; k++ )T.Y( k )=M[k].Min/sD; T.First(); w=T.X( 0 );
                                               moveto( w,T( w ) );
    for( w+=dw; w<wn; w+=dw )
    { float W=T( w ); if( !Pdk )color( DARKGRAY ); else PdkColor( W*sD/Pdk );
      lineto( w,W );
    }
    //  ������ ����������
    //
    setlinestyle( DOTTED_LINE,0,NORM_WIDTH );
    for( k=0; k<N; k++ )T.Y( k )=M[k].Max/sD; T.First(); w=T.X( 0 );
                                              moveto( w,T( w ) );
    for( w+=dw; w<wn; w+=dw )
    { float W=T( w ); if( !Pdk )color( DARKGRAY ); else PdkColor( W*sD/Pdk );
      lineto( w,W );
    }
    // ������ ������� ��������
    //
    setlinestyle( SOLID_LINE,0,THICK_WIDTH );
    for( k=0; k<N; k++ )T.Y( k )=M[k].Mid/sD; T.First(); w=T.X( 0 );
                                              moveto( w,T( w ) );
    for( w+=dw; w<wn; w+=dw )
    { float W=T( w ); if( !Pdk )color( DARKGRAY ); else PdkColor( W*sD/Pdk );
      lineto( w,W );
    }
    setlinestyle( SOLID_LINE,0,NORM_WIDTH );
    color( DARKGRAY );
    for( k=0; k<N; k++ )circle( Tv_x( T.X(k) ),Tv_y( T.Y(k) ),3 );
  } pattern( bc );
}
/// �� ������ ������ ����� �������������� ������� ����������� ��������
//
void Meteo_Series::Extrem()
{ if( PdkModeView && Pdk>0.0 )yScale=Pdk; else yScale=1.0;
  if( nMax<PntNum )nMax=PntNum;           // ������������ ����� ����
  if( xMask<Mask  )xMask=Mask;            // �������� ������ �������� �������
  if( PntNum>0 )++sMax;                   // ���������� ����� ��� �������
  for( int k=0; k<PntNum; k++ )           //
  { Meteo_Point& Mk=M[k];                               //
    if( yMax==yMin )                                    // ��������� ���
    { xMax=xMin=Mk.DataTime;                            // �� ������ �����
      yMin=min( Mk.Min/yScale, min( Mk.Mid/yScale,Mk.Max/yScale ) );
      yMax=max( Mk.Max/yScale, max( Mk.Mid/yScale,Mk.Min/yScale ) );
      if( yMax-yMin < 2.0e-6 )yMax=max( yMax,yMin )+1.0e-6,yMin-=1.0e-6;
      if( PdkModeView ){ if( yMax<1.6 )yMax=1.6; if( yMin>0.0 )yMin=0.0; }
    } else
    { xMin=min( xMin,Mk.DataTime );
      xMax=max( xMax,Mk.DataTime );
      yMin=min( yMin,min(Mk.Mid/yScale,min(Mk.Min/yScale,Mk.Max/yScale)));
      yMax=max( yMax,max(Mk.Mid/yScale,max(Mk.Min/yScale,Mk.Max/yScale)));
  } }
}
///  ����������� ����:
///     ������� ����������� ���������� ���� ����� � ��������
//   1) ��� ���������������� ������ DesignCAD � ������ ������� ����c����
//      ����������� �������� (MetaHatch), ����� �� ���, ������, �������
//      ��� ������ ���������-��������� ������� � ������
//   2) ��� �������� ������������� ���������� ������� �������� �����,
//      ��� ������������ ������� ������������ �������, �����
//      ����������� �������� ���� � ����� - ��������� ���������
//
void Map::Map_Show(){ Item *M; ItemsList *L; int k; float w;
  if( MapType )
  { //Read_GRD();
    TvClip();
    for( k=0; k<2; k++ )                          //
     for( L=mapLL.L_First; L; L=L->L_Next )       // ����������� �������
      if( L->iActive )                            // ����� ���������,
       for( M=L->First; M; M=M->Next )        // ��� ������� ��������
        if( k^(((Entity*)M)->EntityType==17) )    // ������ ����������� �������
                           ((Entity*)M)->Show();  //
    //
    //  ������� ����������� ������� ������������� �������� �� �����
    //
    for( ItemsList *T=txtLL.L_First; T; T=T->L_Next )if( T->iActive )
      for( Item *M=T->First; M; M=M->Next )((Map*)M)->Pts_Show();
    TvUnClip();
  } else
  { //            ������� ��������� ��� ���������������� ���������� �����������
    //
   static Bfont B( _Simplex_font,18 ); static TvMode Save;
   int m,d,y;
    xMax=xMin=0; yMax=yMin=0.0; nMax=sMax=sDrw=xMask=0;          // �����
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive )       // �����������
    for( M=L->First; M; M=M->Next )((Meteo_Series*)M)->Extrem(); // �����
    if( !nMax || !sMax )return;
    //
    //  ����� ��������� ������ ���� ����� ���������� ��� ���� F � _F
    //
    F.Jx=-1.0;   F.Lx=102.0;
    F.Jy=w=yMin; w=(F.Ly=yMax-yMin)*0.8;
    if( nMax==1 )F.Ly*=1.06;
//  if( !Small_Image ||
    if( _F.Ly==0.0 )_F=F; { Field Ft=_F; Tv_place( 0,&Ft ); }
    Save=Tv;
    TvClip();                 //
                              // �������� ��������������� ������������ �������
                              //
    k=sprintf( str," %s",Title );       // ���� ������������ ��������� �����
    if( nMax==1 )                       // ����� ���� ������� � ������ �����
    { xMin.UnPack( m,d,y ); k+=sprintf( str+k," %04d",y );
               if( xMask>0 )k+=sprintf( str+k,"-%02d",m );
               if( xMask>1 )k+=sprintf( str+k,"-%02d",d );
      if( xMax>xMin )
      { xMax.UnPack( m,d,y ); k+=sprintf( str+k," - %04d",y );
                 if( xMask>0 )k+=sprintf( str+k,"-%02d",m );
                 if( xMask>1 )k+=sprintf( str+k,"-%02d",d );
    } }
    //  �������������� ����� ��� � �������
    //
    color( LIGHTGREEN ); if( PdkModeView )line( (float)0.0,1.0,100.0,1.0 );
    //
    ///       ... � ��� ������ ����� ���������� ���������� ��������� ��������
    //                                                  (� ����� � ����������)
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive )
    for( M=L->First; M; M=M->Next )((Meteo_Series*)M)->Draw();
    //
    //                ������������� ������� �� ����� �������
    //
    if( nMax>1 )    /// ���� ���� ��������� ���, �� �� ������ ���� ��������
    {               // ��� ���������� ������ -- �.�. ���� ������ ���� � ������
     double v,hdt=yMax-yMin,dT=deltaTime(),iT=initTime();
     Event Pt( xMin ); Pt-=iT; Pt.UnPack( m,d,y );

      for(;;)
      { Pt.Pack( ++m,1,y ); color( LIGHTGRAY );
        Pt.UnPack( m,d,y );
        moveto( v=100.0*(Pt-xMin)/dT,yMax );
        if( m==1 ){ color( BLUE ); w=yMax-hdt/16.0; } else
        if( m==7 ){ color( CYAN ); w=yMax-hdt/24.0; } else
                                   w=yMax-hdt/32.0; lineto( v,w );
        if( Pt>=xMax )break;
        if( m==1 )B.OutText( (Point){ v,w-hdt/12 },
                             (Point){ v+4,w },_Fmt( "%04d ",y ) );
      } w-=hdt/8;
    } color( DARKGRAY ); B.Text( 0.0,w,str );
    TvUnClip();
    Tv=Save;
    Print();
} }
/**    �����������-����������� ������� ����� �����������
               ���������� ������������� ����������
              �� ��������� ������������ �����������
                 (��� ������� ���������� ������)
**/
Meteo_Series::Meteo_Series( char *Key ) // Read_XML ��������� �����������,
    : PntNum( 0 ),Mask( 0 ),M( NULL ),  // ��� ����������� � �����������
      dataKey ( NULL ),                 // ��������� ���� ������ �����������
      dataName( NULL ),
      dataMeas( NULL ), Pdk( 1.0 )
{ char *word;
    dataKey = strdup( Key );
    sscanf( Eco.Word(),"%g",&Pdk ); // ����������� ���. ���������� �� ���������
    NextEcoWord   { dataMeas=strdup( word ); A2O( dataMeas ); }
    NextEcoString { dataName=strdup( word ); A2O( dataName ); }
  for(;;)                              //
  { word = Eco.Next();                 // ������� �������� - � ��������
    if( !word )break;                  // ������ �� �������
    if( !word[0] )break;               // ������ ������
    if(  word[0]=='/' )break;          // ����� �������� �����
    if( !strcmp( "Eco",word ) )break;  // ����� ����
    if( word[0]>='0' && word[0]<='9' ) // ������ ���� ����
    { int m=0,d=0,y=0;                 // �����, ����, ���
     char *w=NULL;                     // ������ - yyyy.mm.dd-hh:mm'ss"dd
     float h=0.0;                      //
      if( (w=strchr( word,'.' ))!=NULL )  // ���� ����� ����
      { if( Mask<1 )Mask=1; *w++=0; m=atoi( w );
        if( (w=strchr( w,'.' ))!=NULL )   // ���� ���� ������
        { if( Mask<2 )Mask=2; *w++=0; d=atoi( w );
          if( (w=strchr( w,'-' ))!=NULL ) // � ��� ���� ����, ������ � �������
          { if( Mask<3 )Mask=3; *w++=0; AtoD( w,h );
      } } } y=atoi( word );
      //
      // ����� ������������ ������ � ���������� �������� ������, ��� � �������
      //
      M=(Meteo_Point*)Allocate( (PntNum+1)*sizeof( Meteo_Point ),M );
      M[PntNum].DataTime = Event( julday( m>0?m:1,d>0?d:1,y ),h );              // printf( "\nEco:Word = %s = %d = %d.%02d.%02d:%s - Mask=%d",word,M[PntNum].DataTime.D,y,m,d,DtoA( h ),Mask );
      //
      //  !! ����� ���� ��������� �������� ������� ��� ������, ��� � �������
      //
      NextEcoWord M[PntNum].Mid=atof( word ); // ����� ����������
      NextEcoWord M[PntNum].Min=atof( word ); // ��������� ���� � ��������
      NextEcoWord M[PntNum].Max=atof( word );                                   // printf( "\nEco:Word = { %0.3g, %0.3g, %0.3g }",M[PntNum].Mid,M[PntNum].Min,M[PntNum].Max );
      PntNum++;
  } }                                                                           // printf( "\nEco: Exit - PntNum = %d\n",PntNum );
}
//   ������ � ������������ ��������� ����������� �������������� �����,
//       �������������� � ������� ���������� ��������� DesignCAD
//
void Map::Read_XML( char *name )
{ char *Next;
//if( (FileType=GetFileType( name ))!=dtXmlData ) return; // �������
  if( strcmp( "Sakhalin-data",Eco.Open( name ) ) )return; // ��������
  if( strcmp( "Station",Eco.Next() ) )            return; // ����� ������
  //
  //                         ��������� ��� ������ ������� ������������ �������
  if( mapLL.nL )mapLL.Free();                        // ��������� ������ �����
  for( ;; )
  { Next=Eco.Next();
    if( !strcmp( "Data",Next ) )break; else        // ������ ��������
    if( !Next )return;                             // �� �������� ����� ������
  }
  //                ������ ������ � ������������ �������� ������������� ������
  for(;;)
  { Next=Eco.Next();
    if( !Next )break; else            //
    if( !Next[0] )break; else         //  ����� ���� - �������� ��� ����������
    if( !strcmp( "Eco",Next ) )       //
    { ItemsList *L = new ItemsList( Next=Eco.Word() ); //,mapLL.nL );
      L->AddItem( new Meteo_Series( Next ) );
      if( L->nM>0 )mapLL.AddLayer( L );
  } } F.Jx=F.Jy=F.Lx=F.Ly=0.0; _F=F;  // ������� ��� ������������ ����������
}
/**
        ���������� ��������, ����������� � ���������� ������������
**/
//      ������� � �����.
//      ���� ����� �� �������, �� ������� �� � ������� � ������,
//      �������� ��� ���� ������� ����� � �������� ����������.
//      ����� ���������� �����
//      ~~~~~~~~~~~~~~~~~~~~~~
void Map::Read() // const char *name )          // ���������� � ����������
{ char *p=strrchr( strcpy( str,DstName ),'.' ); // �������� ��������������
  if( strncmp( strlwr( p ),".xml",4 ) )         // ��������� �������� ���
    { Read_DC2( DstName );                      // ������ ������������ �����
      if( p )strcpy( p,".dc1" );               //
      else strcat( str,".dc1" );              // ������ ���������� dc2(dw2)-dc1
      ReadText( str );
    } else
    { MapType=false;           // ��� ����-������� ������� ���������������
      Read_XML( DstName );     // ������ ���������������������� ����������
      Digest=Read_Digest( DstName ); // � �������� ����� � ��� �����������
    }
  Previous=ActiveMap;
  if( mapLL.nL==0 )Error_Message( "������� �� ���������: %s",DstName );
}
///     ������ ��������� ������ ��� ������������ ���������
 //
Map::Map()
   : mapLL(),txtLL(),MapType( true ), Small_Image( NULL ),Digest( NULL ),
     iMark( 1.0 ),iColor( GREEN^WHITE ),iDispType( 0 ),DstName( NULL ),
     Title( NULL ),Comment( NULL ),Contain( NULL )
{    F.Jx=F.Jy=0.0;
     F.Lx=F.Ly=1.0; _F=F; P=S=0.1;
}
#if 0
Map::~Map()            // !!! ����� ���� ��� ����� � ����� �������������
{ if( &StartMap!=this && EntityType!=-222 ){ char **W;
    if(  Title  )free( Title );
    if( DstName )free( DstName );
    if( Comment ){ W=Comment; while( *W )free( *W++ ); Allocate( 0,Comment ); }
    if( Contain ){ W=Contain; while( *W )free( *W++ ); Allocate( 0,Contain ); }
    if( Small_Image )Tv_FreeImage( Small_Image );
    mapLL.Free();
    txtLL.Free();
} }
#endif
///    ���������� ����� ������������ ����������� �� ����������� ����������
 //
Map::Map( const char* XML )
   : mapLL(),txtLL(),MapType( false ), Small_Image( NULL ),Digest( NULL ),
     iMark( 0.02 ),iColor( LIGHTRED ),iDispType( dtpCircle ),
     DstName( NULL ),Title( NULL ),Comment( NULL ),Contain( NULL )
{    P=S=0.1;
     F.Jx=F.Jy=0.0;
     F.Lx=F.Ly=1.0; _F=F;
  //
  //          ������ ��������� ���������� �� ����� XML � ������
  //
 char *Next=Eco.Open( XML ),*word; //int k;
  if( !Next )goto Exto;                           // �������
  if( strcmp( "Sakhalin-data",Next ) )goto Exto;  // ��������
  if( strcmp( "Station",Eco.Next() ) )goto Exto;  // ����� ������
  Title=strdup( A2O( Eco.String() ) ); strcut( Title );
  DstName=strdup( XML );
  //
  //          ������ ������ � ������������ ���������� ��� ������������� ������
  for( ;; )
  { Next=Eco.Next();
    if( !Next )goto Exto; else                 // ���������� �������� ��������
    if( !strcmp( "Data",Next ) )break; else    // � ���������� �� ������
    if( !strcmp( "Digest",Next ) ){ NextEcoString Comment=String2TextLn( word ); } else
    if( !strcmp( "Description",Next ) ){ NextEcoString Contain=String2TextLn( word ); } else
    if( !strcmp( "Point",Next ) )AtoD( Eco.Word(),P.x ),
                                 AtoD( Eco.Word(),P.y );
  } Exto: Eco.Close();
}
//
///     ������������ ������� ���� ����� ��� �������������� ������� � ������
//
//      ���� ���� ����� ������������ � ������ �������, ������� ��� ����������
//      �������� ����� ����� � ������ �� ����������
//
                                 //
Map* Map::FindMouse( const Point &_P )
{ ItemsList *L;
  if( MapType )
  { float d,D=1e30;
    Map *M,*N=NULL;
    for( L=txtLL.L_First; L; L=L->L_Next )if( L->iActive )   // ������ �������
    for( M=(Map*)L->First; M; M=(Map*)M->Next )          // �� ������ ����
    if( ( d = abs( (Point)*M - _P ) )<D ){ D=d; N=M; } return N;
  } else                          //
  {
   static Map UniTxt;             // ��������� ������ ��� ���������� ����������
   Meteo_Series *mSt=NULL;        // ��������� ������� ��� ��������� �����
   Point mPts=_P,mQts,wP;         // ���������� ����� � ��������� ����� ������
   Item *M;                       //
   float  mDst=0.0,scX=1.0;            // ����������� ��������� � ������ �����
    if( &UniTxt.mapLL != &mapLL ) // � �������� �������� ������������
      { UniTxt=*this;             // �������� �������� �������������� �����
        UniTxt.DstName=NULL;      //
      }
    if( sMax && nMax )
    {
     double dT=deltaTime();       // !! ��� ������ ����� ����������� ���������
     double WD=100.0/dT,wD,yS;
     int k=0,m,d,y;
     static Event Cur;
      scX = ( yMax-yMin )/100.0;
      if( nMax==1 )
      { for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) /// ����� ����� !!
        for( M=L->First; M; M=M->Next )
        { Meteo_Series& S=*(Meteo_Series*)M;
          if( S.PntNum<=0 )continue;
          yS=yScale;
          if( PdkModeView && S.Pdk>0.0 )yScale=S.Pdk; else yScale=1.0;
          wP.x = 100.0/( sMax+1.0 ) + 100.0*k/( sMax+0.5 );
          wP.y = S.M[0].Mid/yScale;
          wD = abs( mPts.x-wP.x );
          if( !k || wD<mDst ){ mSt=&S; mQts=wP; mDst=wD; yS=yScale; }
          yScale=yS;
          k++;
        } Cur=mSt->M[0].DataTime;
      } else
      if( nMax>1 )
      { for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) /// ����� ����� !!
        for( M=L->First; M; M=M->Next )
        { /// � ������ ������ ����� ���������� ����� ��������� � ����� �����
          // ( ��������� �������� ��� �������� ��������-����������������� ���� )
         Meteo_Series& S=*(Meteo_Series*)M;
          if( S.PntNum<=0 )continue;    //
          k=0;                          // k=0 - ������ �������� ����� ����
          yS=yScale;                    //
          if( PdkModeView && S.Pdk>0.0 )yScale=S.Pdk; else yScale=1.0;
          if( !mSt )
          { mSt=&S;
            mQts=(Point){ (S.M[0].DataTime-xMin)*WD,S.M[0].Mid/yScale }; k=1;
            mDst=hypot( (mPts.x-mQts.x)*scX,mPts.y-mQts.y );
          }
          for( ; k<S.PntNum; k++ )
          { wP=(Point){ (S.M[k].DataTime-xMin)*WD,S.M[k].Mid/yScale };
            wD=hypot( (mPts.x-wP.x)*scX,mPts.y-wP.y );
            if( wD<mDst ){ mSt=&S; mQts=wP; mDst=wD; yS=yScale; }
          } yScale=yS;
        } Cur = xMin + mPts.x / WD; //* (xMax-xMin)/100.0;
      }   Cur.UnPack( m,d,y );
      sprintf( str,W2D("%s: %s, ���=%g [%s], = (%0.4g) - %04d:%02d:%02d "),
       mSt->dataName,mSt->dataKey,mSt->Pdk,mSt->dataMeas,mPts.y*yScale,y,m,d );
      UniTxt.Title=str;        // ������� ����������� � �������� ���-���������
      UniTxt.P=mQts;
      UniTxt.DigestComment( mSt->dataKey );
    } return &UniTxt;
  }
}
//      ������ ������� �������� � ������� �� ������� ������
//
static char* OA( const char *O )
{ static char *A=NULL; if( A )free( A ); return O2A( A=strdup( O ) );
}
char* StartHtml=NULL;   // ��������� ����� ��� �������� Html
void Map::Print()
{ if( MapType || !nMax || !sMax )return; else
  {
   FILE *html;
   char *scm;
   short Mask=0;
    if( !StartHtml )
    { strcpy( fname( strcpy( strcpy( str,"Start " )+6,StartMap.DstName ) ),"Sakhalin-table.html" ); //
      StartHtml=strdup( str );
    }
    if( (html=fopen( StartHtml+6,"wt" ))==NULL )return;      // ������, ��� ������
    scm=OA( Title );
    fprintf( html,
    "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\n"
    "<title>%s</title>\n</head>\n<body>\n<h2>%s</h2>\n",scm,scm );
   ItemsList *L;
   static Event Start,End;
   bool i=false;                               // i - ������� ������� �������
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) // ������ �� �����
    { Item *M;
      for( M=L->First; M; M=M->Next )
      { Meteo_Series& S=*(Meteo_Series*)M;
        if( S.Mask>Mask )Mask=S.Mask;
        for( int k=0; k<S.PntNum; k++ )
        { Meteo_Point& P=S.M[k];
          if( !i ){ Start=End=P.DataTime; i=true; } else
          if( End<P.DataTime )End=P.DataTime; else
          if( Start>P.DataTime )Start=P.DataTime;
    } } }
    //
    //  ������� ���������� - ����� ���������� ������ �����
    //
    { int m,d,y;
      Start.UnPack( m,d,y ); fprintf( html,"<p>������ ����������: %04d",y );
                 if( Mask>0 )fprintf( html,"-%02d",m );
                 if( Mask>1 )fprintf( html,"-%02d",d );
      End.UnPack( m,d,y );   fprintf( html," - %04d",y );
                 if( Mask>0 )fprintf( html,"-%02d",m );
                 if( Mask>1 )fprintf( html,"-%02d",d );
                             fprintf( html,"</p>\n<p>" );
    }
    if( !Comment )fprintf( html,"��� �����������" ); else
    for( int k=0; Comment[k]; k++ )fprintf( html,"%s<br>\n",OA( Comment[k] ) );
    fprintf( html,"</p>\n<table border=1 cellspacing=0 cellpadding=2>\n<tr>\n\t<td>����</td>\n\t<td>��������</td>\n"
                  "\t<td>���</td>\n\t<td>�������</td>\n\t<td>�������</td>\n\t<td>��������</td>\n</tr>\n" );
    //
    //  ������ ����� ��������� ������ �������� ��������
    //
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) // ������ �� �����
    { Item *M;
      for( M=L->First; M; M=M->Next )
      { Meteo_Series& S=*(Meteo_Series*)M;
        fprintf( html,"<tr>\n\t<td>%s</td>\n\t<td>%s</td>\n",S.dataKey,OA( S.dataName ) );
        fprintf( html,      "\t<td>%g %s</td>\n",S.Pdk,OA( S.dataMeas ) );
        i=false;
        float Aver=0,Amax=0,Amin=0;
        for( int k=0; k<S.PntNum; k++ )
        { Meteo_Point& P=S.M[k];
          if( !i ){ Aver=Amax=Amin=P.Mid; i=true; } else
          { Aver+=P.Mid;
            if( Amax<P.Mid )Amax=P.Mid; else
            if( Amin>P.Mid )Amin=P.Mid;
        } }
        fprintf( html,"\t<td>%0.4g</td>\n\t<td>%0.4g</td>\n\t<td>%0.4g</td>\n</tr>\n",Aver/(float)S.PntNum,Amin,Amax );
    } }
    fprintf( html,"</table>\n<p>" );
    if( !Contain )fprintf( html,"... ��� ��������" ); else
      for( int k=0; Contain[k]; k++ )fprintf( html,"%s<br>\n",OA( Contain[k] ) );
    fprintf( html,"</p>\n</body>\n</html>\n" );
    fclose( html );
  }
}
#if 0
<html><head><meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
<title>�������� �������</title>
</head>
<body>
<h2>�������� �������</h2>
<p>������ ����������</p>
<p>������� ��������</p>
<table border=1 cellspacing=0 cellpadding=2>
<tr>    <td>����</td>
        <td>��������</td>
        <td>���</td>
        <td>�������</td>
        <td>�������</td>
        <td>��������</td>
</tr>
<tr>    <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
</tr>
</table>
<p>������ ��������</p>
</body>
</html>
#endif
