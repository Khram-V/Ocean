//
//      Tv.cpp                                                   /91.03.25/
//       - сборник примеров для визуализации и конвертации графики и геобаз
//
#include "Tv.h"
#include <CType.h>
                              //
Button All;                   // Кнопочка для масштабируемых картинок
static FILE *Fs=0;            //   (тоже надо, конечно - же)
static char *Adr,*Pic,*INM,Line[]={ _Esc,'J',24,_Esc,'*',6,0,0 };
static viewporttype V;        //
static int  Tbuf[4],          // Typ  Признак данных в памяти
            TY,tY,Y,          // -1 - Первое вхождение с _argv[1]
            TX,tX,X,ans=-1,   // 00 - Нет данных в памяти
            Typ=-1;           //  1 - .TV
                              //  2 - .CHR, 3-.VCT, 4-.VFN
//long long Tfile( char* );   //  5 - .DC2, 6-.DW2, 7-.BLN
//atic void Printer();        // 10 - .PCX
                              // 11 - .TV в старом упакованном формате
static void Getc()
{ static int St=3;
  int H,Sz=TRUE,c=color( CYAN );
Begin:
  tY=V.bottom-V.top+1;
  tX=V.right-V.left+1; H=ans<=_Enter;
  pattern( BLACK );
  if( !H&&ans!=' ' )
  Ghelp( "%s%s 3read  \"%s[%dx%d]\"",Typ==1?"1screen":"",
                                     Typ==11?" 2print":"",INM,X,Y );
  if( H )Tv_rect_x( V.left-1,V.top-1,V.right+1,V.bottom+1 );
  setviewport( V.left,V.top,V.right,V.bottom,1 ); ans=Tv_getc();
  setviewport( 0,0,TX,TY,1 );
  if( H )Tv_rect_x( V.left-1,V.top-1,V.right+1,V.bottom+1 ); Ghelp();
  if( ans==_Enter ){ Sz=!Sz; goto Begin; }else
  if( ans<_Enter )
  { H=Tv_getk()&SHIFT?!Sz:Sz;
    if( ans&North ){ V.top  -=St; if( H )V.bottom-=St; }
    if( ans&South ){ V.top  +=St; if( H )V.bottom+=St; }
    if( ans&East  ){ V.right+=St; if( H )V.left  +=St; }
    if( ans&West  ){ V.right-=St; if( H )V.left  -=St; }
    if( V.top<0  )V.top=0;  if( V.bottom>TY )V.bottom=TY;
    if( V.left<0 )V.left=0; if( V.right>TX ) V.right=TX; goto Begin;
  } else if( ans==_Esc )
  { if( tY!=TY+1 || tX!=TX+1 )
    { V.bottom=tY=TY; tY++; V.top=0;
      V.right =tX=TX; tX++; V.left=0; goto Begin;
    }
  } else if( isdigit( ans ) ){ ans-='0'; St=ans<2?1:St*ans; goto Begin; }
  color( c );
}
void Net_CDF ( char *Name );
void Inf_View( char *Name );
void GRD_View( char *Name );
void DCM_View( char *Name, const int Type );
void FNT_View( char *Name, const int Type );
void PCX_View( char *Name );

int main( int Ac, char **Av )
{ int i,x,y;
 static char Name[]="\n>Tv \xABName\xBB"
     ".<inf|int>\t- Bathymetry Seafloor charts"
     "\n\t *.<grd>\t\t& GoldenSoft Topo fields"
     "\n\t *.<nc>\t\t- Atlas of World Surface Marine data"
     "\n\t *.<dc2|dw2|bln>\t- Design Cad & Simple line field"
     "\n\t *.<chr|vct|vfn>\t- Borland & Small American fonts"
     "\n\t *.<pcx|tv>\t- PaintBrush & Epson picture"
     "\n       \xAB-bgi=2\x97""8\xBB\t\t\t   \xA9""1991-2016 V.Khramushin";
//Tfile_Filter = "Bathimetry Data (*.inf)\0*.inf;*.int;*.dat;*.byt\0"
//               "Surfer Graphics data (*.grd)\0*.grd\0"
//               "All Files (*.*)\0*.*\0";
  if( Ac<2 )Break( Name );              //
  for( i=1; i<Ac; i++ )                 // Выборка последнего имени
  if( Av[i][0]!='-' && Av[i][0]!='/' )  // в списке параметров
  { char *S;                            //
    if( (S=strrchr( strcpy( Name,Av[i] ),'.' ))!=0 )
    { S[4]=0; strlwr( S+1 );
      if( !strcmp( S+1,"inf" )
       || !strcmp( S+1,"int" ) ){ Inf_View( Name );   return 0; }
      if( !strcmp( S+1,"grd" ) ){ GRD_View( Name );   return 0; }
      if( !strcmp( S+1,"pcx" ) ){ PCX_View( Name );   return 0; }
      if( !strcmp( S+1,"chr" ) ){ FNT_View( Name,2 ); return 0; }
      if( !strcmp( S+1,"vct" ) ){ FNT_View( Name,3 ); return 0; }
      if( !strcmp( S+1,"vfn" ) ){ FNT_View( Name,4 ); return 0; }
      if( !strcmp( S+1,"dc2" )
       || !strcmp( S+1,"dc3" ) ){ DCM_View( Name,5 ); return 0; }
      if( !strcmp( S+1,"dw2" ) ){ DCM_View( Name,6 ); return 0; }
      if( !strcmp( S+1,"bln" ) ){ DCM_View( Name,7 ); return 0; }
      if( !strcmp( S+1,"nc"  ) ){ Net_CDF( Name );    return 0; }
    } break;
  }
  Tv_Graphics_Start();         setwindowtitle( "Tv - графические прорисовки" );
  Tv_Start_Mouse();            getviewsettings( &V );
  TY=tY=V.bottom-V.top; ++tY;  INM=Name;
  TX=tX=V.right-V.left; ++tX;
  static const unsigned TVL=65000u;
  if( (Pic=(char*)Allocate( TVL ))==NULL ){ Tv_bell(); return 1; }
  do
  { size_t LF=0;
    if( ans>0 )Getc(); else ans=0xF3;
    switch( ans )
    { //case 0xF2: if( Typ==11 )Printer(); break;
      case 0xF3:        //
      { for( ;; )       // Ввод графического файла
        { if( Typ>=0 )strcpy( fname( Name ),"*.*" ); Typ=0;
          if( (LF=Tfile( strupr( Name ) ))<8 )break;
          //
          // чтение основного изображения в формате Epson-FX
          //
          if( (Fs=fopen( Name,"rb" ))!=0 )
          { fread( Tbuf,2,4,Fs );
            if( strncmp( Line,(char*)Tbuf,6 )==0 )
            { X=Tbuf[3];
              Y=int( LF/(X+9) )*8; Typ=1; break;
            } else              //
            { Y=Tbuf[0];        // Старый метод упаковки изображения
              X=Tbuf[1];        //
              if( Y>0 && Y<=480 && X>0 && X<=720 )Typ=11;
            } fclose( Fs ); Fs=0;
        } } if( Typ!=1 && Typ!=11 )break;
      }                 //
      case 0xF1:        // Вывод изображения в текущее окно терминала
      if( Typ==1 || Typ==11 )
      { if( (Fs=fopen( Name,"rb" ))!=0 )
        { fread( Tbuf,2,4,Fs );//
          if( Typ==11 )       // Старый вариант выбирается в память целиком
          { Adr=Pic;         //
            ((long*)Pic)[0]=((long*)Tbuf)[1];
            if( (LF-=4)>TVL )LF=TVL; y=Y; Y=(int)(LF*8/X);
            if( Y!=y )Tv_bell();
            fread( Pic+4,1,LF-4,Fs );
          }
         byte b,k=0,w=0;
          if( (Tv_getk()&SHIFT)==0 )bar( V.left,V.top,V.right,V.bottom );
          for( y=0; y<Y && Tv_getw()!=_Esc; y++ )
          { if( Typ==1 && (y%8)==0 ) //
            { int i,l,m,kx;          // Считывание одной печатной линии
              Adr=Pic;               //
              if( (l=((byte*)Tbuf)[2]/3)<8 )Y-=8-l; memset( Adr,0,X );
              for( kx=0; kx<X; kx++ )
               if( (b=getc( Fs ))!=0 )
                for( i=0,m=kx,b<<=8-l; i<l; i++,m+=X,b<<=1 )
                 if( b&0x80 )Adr[m>>3]|=0x80>>(m&7);
              getc( Fs );
              fread( Tbuf,2,4,Fs );     //
            }                           // Вывод на экран одной линии
            for( x=0; x<X; x++ )        //
            { if( k==0 ){ w=*Adr++; k=7; }else k--;
              if( w&0x80 )putpixel
                ( V.left+(X<=tX?x:(int)((long)x*tX/X)),
                  V.top +(Y<=tY?y:(int)((long)y*tY/Y)),LIGHTGRAY ); w<<=1;
      } } } }
      case  ' ': break;
      case _Esc: Ghelp( "?Are you sure" );
      { ans=Tv_getc(); ans = ans==_Enter || (ans|0x20)=='y' ? _Esc:0xFB; }
    }
  } while( ans!=_Esc );
}
