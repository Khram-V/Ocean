//
//      Dp_out.cpp                                /91.02.20 -
//                                               - 95.10.14/
//      Программа записи информационного и файла батиметрии
//
#include <String.h>
#include <StdArg.h>
#include "..\Ani\Depth.h"

static void dprint( const char Msg[], const char *fmt, ... )
{
 char str[30];
  va_list aV;
  va_start( aV,fmt ); vsprintf( str,fmt,aV ); va_end( aV );
  fprintf( Chart.Fs,"%-25s | %s\n",str,Msg );
}
void Depth::infout( const char* DataName )
{
//      Блок записи информационного файла
//              Name    Имя файла данных (для INFO - описания)
//              Type    Тип данных: 0-Text; 1-Byte; 2-Integer; 4-Float
//              Ny      Полная размерность матрицы
//              Jy        Начальный отсчет считываемого сегмента
//              Ly        Размерность считываемого сегмента данных
//              Nc      Количество мореографных точек в файле .INF
//
 static const char
        Msg4[]="Type        0:Text 1:Byte 2:Integer 4:Float",
        Msg5[]="Format      2:Field",
        Msg6[]="Ny Nx       Dimensions",
        Msg7[]="Jy Jx Ly Lx Base & Size",
        Msg8[]="Max Min     Bathimetry Extrema",
        Msg9[]="Sy Sx ' Fe  Grid Step, \xF8'km & Lat.of scale",
        Msga[]="Cs Fi\xF8'Lo\xF8' Course, Latitude & Longitude",
        Msgb[]="Mario";
 Real w,s=scale();
 char mY[12],mX[12];
 int  k,F,L;
 Point P; P.x=P.y=0; ItD( P ); strcpy( mY,DtoA( P.y,-2 ) );
                               strcpy( mX,DtoA( P.x,-2 ) );
 if( !DataName )goto X;
 fprintf( Fs,"%s\n",Msg[0]  );
 fprintf( Fs,"%s\n",Msg[1]  );
 fprintf( Fs,"%s\n",strupr( (char*)DataName ) );
  dprint( Msg4,"%6d",Type );
  dprint( Msg5,"     2" );
  dprint( Msg6,"%4d %d",Ny,Nx );                   //-( Active&0x100 ? 1:0 ) );
  dprint( Msg7,"   0 0 0 0"  );
  dprint( Msg8,"%6.5lg %1.5lg %s",Max,Min,"m" );
  dprint( Msg9,"%6.5lg %1.5lg %c %s",Sy/s,Sx/s,Ms,DtoA( Fe*Radian ) );
  dprint( Msga,"%s%s%s",DtoA( Cs,-1 ),mY,mX );
X:dprint( Msgb,"%6d",Nmp );
  for( k=0; k<Nmp; k++ )
  { MARIO &Ml=M[k];
    P=Ml.G;
    fprintf( Fs, "%9s ",         DtoA(P.y,-2) );
    fprintf( Fs,"%-10s %-20s ; ",DtoA(P.x,-2),Ml.cp );
    DtI( P,F,L );
    w = __Depth[F][L];
    fprintf( Fs,fabs( w )>1000.0 || !( w-int(w) )?"%6.0f":"%6.1lf",w );
    if( Ml.D>0.0 )fprintf( Fs,"%6s/%-5.1lf",DtoA( Ml.A,1 ),Ml.D );
            else  fprintf( Fs,w>0.0?" -open sea- ":" -coastal-  " );
    fprintf( Fs,"   [%3d/%-3d]\n",F,L );
  } close();
}
void Depth::write( const char* Path )
{
//      Запись информационного файла
//       с формированием новых имен и запросом на тип данных
//
 int Tp=Type,k,c=color( GREEN ); Ghelp(); //ofx=ofy=0;
 static char p[]={ 1,6,11,19,25 },        // ,34 },
        n[5][10]={ "Text","Byte","Integer","Float","Inf Only" }; //"Bathimetry"
  do
  { Tgo( 1,0 );
    for( k=0; k<6; k++ )Tprintf( "%s ",n[k] ); Tprintf( "\x1b\x09\x1a" );
    if( Tp<0 )Tp=4; else
    if( Tp>4 )Tp=0; color( LIGHTCYAN );
    Tgo( p[Tp],0 ); Tprintf( strupr( n[Tp]+1 )-1 ); strlwr( n[Tp]+1 );
        color( GREEN );
        k=Tv_getc();
    if( k==East )Tp++;
    if( k==West )Tp--;
  } while( k!=_Enter && k!=_Esc );
  Tgo( 1,0 ); Tprintf( "\n" );            //
  Tgo( 0,0 );                             // Традиционное сохранение
  if( k!=_Esc )                           // информационного и файла данных
  { if( Tp<4 )                            //
    { if( *Path>' ' )strcpy( InfName,Path );
      if( Tgets( InfName,-79 )==_Enter )  // Запрос общего имени
      { Type=Tp<3?Tp:4;                   // Тип данных
        strcut( InfName );
        sname( strcpy( Name,InfName ) );  // Копирование для имени данных
        write();                          // Запись данных
        open( InfName,"inf","wt" );       // Открытие информационного файла
        infout( sname( Name ) );          // Запись информационного файла
      }         //
    } else      // Корректировка только информационного файла
    if( Tp==4 ) //
    { FILE *Fo; Fo=fopen( open( InfName,"inf","w" ),"r" );
      if( Fs==0 || Fo==0 )Tv_bell(); else
      {        k=0;
        while( k<10 )if( fputc( fgetc( Fo ),Fs )=='\n' )k++;
        fclose( Fo );
        infout( 0 );
      }         //
//  } else      // Сохранение во внутреннем формате программы ANI
//  {           //
//    if( Tgets( fext( strcpy( Str,InfName ),"chk" ),-79 )==_Enter )
//    { FILE *F=save( Str ); if( F )fclose( F );
//    }
    }
  } color( c );
}
void Depth::write()
{
//     Запись числового файла батиметрии
//
  if( Active&1 )open( 'w' ); else Fs=NULL;
  if( Fs!=NULL )
  { int y,x;                                 //, Nxi=Nx-( Active&0x100 ? 1:0 );
    if( !Type )
    for(   y=0; y<Ny; y++ )
    { for( x=0; x<Nx; x++ )fprintf( Fs," %1.5g",__Depth[y][x] );
                           fputc( '\n',Fs );
    } else
    { float *R=(float*)Allocate( (unsigned)Nx*Type );
      for(   y=0; y<Ny; y++ )
      { for( x=0; x<Nx; x++ )
        if( Type==4 )R[x]=__Depth[y][x]; else
        if( Type==2 )((short*)R)[x]=short( __Depth[y][x] );
                else ((byte*)R)[x] = byte( __Depth[y][x] );
        fwrite( R,Type,Nx,Fs );
      } Allocate( 0,R );
    }   close();
  }
}
