//
//      Mario.Import-Export
//      Процедуры ввода и редактирования инородных данных
//
#include "Mario.h"

#define IOffs 162               // Смещение до типа данных в S_IO

void Mario::Initial()
{                               //
 static int M,D,Y;              // Уточнение характеристик числового ряда
 static Real _JT,_dt,Lon,Lat;   //
 Mlist Menu[]=
 { { 1,0,"\t Параметры записи" },{ 2,Mario_Title_Length-1,0,Title }
 , { 1,4,"\tДата   %4u",&Y },{ 0,2,":%02u",&M },{ 0,2,":%02u",&D }
 , { 1,13,"\tВремя %-3°",&_JT }
 , { 1,10,"\tИнтервал %-3°",&_dt }
 , { 1,11,"\tШирота  %-3°",&Lat }
 , { 1,11,"\tДолгота %-3°",&Lon }
 , { 1,6, "\tГлубина моря %5.1lf",&Depth }
 , { 1,7, "\tДальность и %6.1lf",&Distance }
 , { 1,5, "\tкурс на берег %-1°",&Bearing }
 , { 1,0, "\tФормат данных: "},{ 0,4 }
 };
 int ans=sizeof( Menu )/sizeof( Mlist ); if( Typ==3 )ans-=2;
 Display T( Menu,ans,1,-1 );
  _dt=dt; Lon=Longitude; Ghelp( "%.3lg<%u>%.3lg",Min,N,Max );
          Lat=Latitude;  color( DARKGRAY );
  do
  { Menu[13].Msg=(char*)(!Fmt?"Real":Fmt==1?"Byte":Fmt==2?"Int":Fmt==3?"3byt":"Word");
    _JT=JT.T; Lon=Longitude;
    _dt=dt;   Lat=Latitude;
    julday( JT.D,M,D,Y ); ans=T.Answer();
    JT=Event( julday( M,D,Y ),double( _JT ) ).Check();
    if( ans==13 )
    if( ++Fmt>4 )Fmt=0;
  } while( ans!=_Esc ); Longitude=Lon; dt=_dt;
                          Latitude=Lat;  Ghelp();
}
void m_InPut()
{ Mario Mar; Mar<<="";
  if( Mar.N>0 )
  { Ms.allocate( Nm+1 );
    if( Nm>1 ){ for( int k=Nm-1; k>Km; k-- )Ms[k]=Ms[k-1]; Km++; }
    else Ms[Km=0]=Mar;
   Mario &M=Ms[Km];
    if( *(char*)Mar>' ' )scpy( M.Title,Mar,Mario_Title_Length ); else
    if( *(char*)M  >' ' )scpy( M.Title,M,Mario_Title_Length ); else
    if( M.Title[0]<' ' )
    scat(M.Title,sname((char*)M+81),20-sprintf(M.Title,"\x11%u\x10",Km));
    M.V=Mar.V;
    M.N=Mar.N;
    M.Fmt=((char*)Mar)[IOffs];
    M.Initial(); if( Nm==1 )Tm=M.JT,dT=M.dt,Tn=dT*M.N; M.Typ=1;
    m_View();
  }
}
void m_ReView( int dr )
{  int k;
 Event T0,TN;
       T0=Ms[0].JT;
       TN=T0+(Ms[0].dt*Ms[0].N);
  for( k=1; k<Nm; k++ )
  { Mario &M=Ms[k];
    if( T0>M.JT )T0=M.JT;
    if( TN<M.JT+(M.dt*M.N) )TN=M.JT+(M.dt*M.N);
    if( dT>M.dt )dT=M.dt;
  }
  if( dr || T0<Tm || TN>Tm+Tn ){ Tm=T0; Tn=TN-T0; m_View(); }
}
void m_OutPut()
{                       //
 Mario &M=Ms[Km];       // Вывод годового приливного ряда
  if( M.Typ==3 )        //
  {
   Real Dat; int i,Start,Cur=0,Month,Day,Year; char S[81];
   FILE *F;
    { int ans,c=color( CYAN );
      scpy ( S,M.Title,-20 );
      Ghelp( "?Tidal File Name:" ); Tgo(-17,0 ); ans=Wgets( S,-25 );
      Ghelp(); color( c );
      if( ans==_Enter )
      if( (F=fopen( S,"wt" ))!=0 )goto OK_to_write; Tv_bell(); return;
    }
OK_to_write:
    M.JT.UnPack( Month,Day,Year );
    Start=int( M.JT.D*24L+long( M.JT.T+0.5 )-julday( 1,1,Year )*24L );
    scpy( S,M.Title,-20 );
    sscanf( S,"%s",S );
    fprintf( F," %s %04d %.0lf %.5lg %0.6lg\n",S,Year,M.Mid*100,M.Latitude,M.Longitude );
    for( int m=0; m<12; m++ )
    {
     int l=int( julday( m+2,1,Year )-julday( m+1,1,Year ) );
      fprintf( F,"\n" );
      for( int d=0; d<l; d++ )
      { for(   i=0; i<2; i++ )
        { for( int h=0; h<12; h++ )
          { if( Cur>=Start
             && Cur<Start+M.N && (Dat=M.V[Cur-Start])>M.Min
            ) sprintf( S+(h*4),"%4.0lf",Dat*100.0 ); else
              sprintf( S+(h*4),"    " ); ++Cur;
          }
          if( !d && !i )sprintf( S+48,"\t%04d.%02d.01",Year,m+1 );
                         strcut( S );
                        fprintf( F,"%s\n",S );
      } }          //
    } fclose( F ); // Вывод на диск в форматах обычного ряда Series
  } else           //
  { ((char*)M)[IOffs]=M.Fmt<=4?M.Fmt:0;
    scpy( M,M.Title,-20 ); m_Mark( ' ' ); M>>=""; m_Mark();
    if( !M.N )m_Delete(),m_View();
  }
}
void m_Copy() // Копирование ряда
{
 int k; Ms.allocate( ++Nm );
 Mario &M=Ms[++Km];
  for( k=Nm-1; k>=Km; k-- )Ms[k]=Ms[k-1]; k=M.N; M.N=0; M.allocate( k );
 Real *Src=Ms[Km-1].V;
 Real *Dst=M.V; while( --k>=0 )*Dst++ = *Src++; M.flag&=~1;
}
void m_Move()
{
 int Nd=Ms[Km].flag&1 || !Nb ? 0:Km;
  do
  { int k;
    for( k=0; k<Nm; k++ )
    if( Ms[k].flag&1 )
      { Ms[Km=k].flag&=~1; Nb--; break; }
    if( Km!=Nd ){ Mario Mk=Ms[Km];
      if( Km<Nd ){ for( k=Km; k<Nd; k++ )Ms[k]=Ms[k+1]; }
         else    { for( k=Km; k>Nd; k-- )Ms[k]=Ms[k-1]; }
      Ms[Nd]=Mk;
    } Km=Nd;
  } while( Nb>0 );
}
void m_Delete()
{ do
  { int k=Km;
    if( Nb )
    { for( k=0; k<Nm; k++ )if( Ms[k].flag&1 ){ Nb--; Km=k; break; }
      if( k>=Nm )break;
    }
    Ms[k].free();
    for( --Nm; k<Nm; k++ )Ms[k]=Ms[k+1]; Ms.allocate( Nm );
  }while( Nb>0 );
}

#include "Tide.h"

void m_Tide()
{ int i,v=0;
 static Field F,f={ 0,0,1,1 };
  if( Nm>0 )m_Mark( ' ' ); F=f; Tv_place( 0,&F );
  if( TideSetup()>0 )
  { if( Nm>0 )m_Copy(); else
    { Tm.T=0; dT=0.25; Nb=Km=0; Tn=72;
      Tm.D=julday()-1; m_Edit();
      if( Tn/dT>3 )
      { Ms.allocate( 1 );
        Ms[0].allocate( int( Tn/dT+0.5 ) );
        Ms[0].JT=Tm;
        Ms[0].dt=dT;
        Ms[0].Fmt=0;
        Ms[0].Typ=1;
    } }
    if( Nm>0 )
    { Mario &M=Ms[Km];
      if( Current_Tide->TideSeries( M.V,M.N,M.dt,M.JT.D,M.JT.T )>=0 )
      {   TideMario( M.Title,M.Longitude,M.Latitude,M.Mid ); ++v; M.Mid/=100;
       Real *V=M.V;
        for( i=0; i<M.N; i++,V++ )V[0]/=100.0;
        MinMax( M.V,M.N,M.Min,M.Max );                 M.Min-=0.005;
        if( M.Mid<M.Min )M.Mid=(M.Max+M.Min)/2.0;
      } else m_Delete();
    }
  } Tv_place( 0,&F );
  if( Nm>0 ){ if( v || memcmp( &F,&f,16 ) )m_View(),m_Mark(); m_Mark(); }
}
