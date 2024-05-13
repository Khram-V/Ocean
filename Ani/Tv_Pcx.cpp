#include "..\Graphics\Tv_graph.h"
//
//      �������� ������ PCX � ������������ ����������
//      - ������ ����� � ������ 256-������� PCX ��������.
//
//      � ��������� ��������� ��������� ������ � ������ PCX ������.
//      ���������� �� "����������", ������ �� ������� 'P' ��� <alt-P>.
//    ( ������������ ������ �������, �.�. ���������� ������ �� �� ���� )
//
typedef struct
 {     char     manuf;        // Always =10 for Paintbrush
       char     hard;         // Version information
       char     encod;        // Run-length encoding (=1)
       char     bitpx;        // Bits per pixel
       short    x1,y1,x2,y2;  // Picture dimensions (incl)
       fixed    hres;         // Display horiz resolution
       fixed    vres;         // Display vert  resolution
       char     clrma[16][3]; // Pallete
       char     vmode;        // (ignored)
       char     nplanes;      // Number of planes (ver 2.5=0)
       fixed    bplin;        // Bytes per line
       fixed    palinfo;      // Palette Info (1=col, 2=gray)
//     fixed    shres;        // Scanner resolution
//     fixed    svres;        //
//     fixed    xtra[27];     // Extra space (filler)
//     fixed    rsv;          // ������ ����� ������ = 128 ����
 }     pcxheader; // __attribute__ ((packed));
static pcxheader
       PCX={ 10,5,1,1,0,0,639,479,640,480, {{ 0 }}, 0,4,80,1 }; //,0,0,{ 0 } };
//
//     ��������� ��� ������ PCX �����������
//       � �������������� ������� �� �������� Borland Graphics
//
void PCX_View( char *Name )
{
 FILE *Fs;
  if( (Fs=fopen( Name,"rb"))==0 )Break("\7\n?File %s not open\n",Name );
          fread( &PCX,1,sizeof( pcxheader ),Fs );
  if( PCX.manuf!=10 )Break( "\7\n?File %s incorrect (%d)\n",Name,PCX.manuf );
  {
   unsigned char C=0,D=0,px,*B,*V;
   short i,j,X,Xf,width=PCX.x2-PCX.x1,
         ans,y,Y,height=PCX.y2-PCX.y1;
    Tv_Graphics_Start( -1,width,height );
    setwindowtitle( "Tv - PaintBrush.pcx pictures" );
    bkcolor( BLUE );         //
    clear();                 // � ��������� ��� ����� �������
    if( PCX.hard>=2 )        //             ��� ������ ������
    { static char B[1024];   //
      for( i=0; i<16; i++ ){ B[i*4] = PCX.clrma[i][2];
                             B[i*4+1]=PCX.clrma[i][1];
                             B[i*4+2]=PCX.clrma[i][0]; }
      if( PCX.hard==5 && PCX.bitpx==8 )                     //
      { fseek( Fs,-769,SEEK_END );                          //  ������ �������
        if( fgetc( Fs )==12 )                               //
        for( i=0; i<256; i++ )B[i*4+2]=fgetc( Fs ),
                              B[i*4+1]=fgetc( Fs ),
                              B[i*4] = fgetc( Fs );
      } Set_Palette( 0,i,(RGB_palette)B );
    }
//
//  ���� ������������� PCX ����� � BGRI �����-����� ��� �������
//
    X =Tv.mX+1; if( width<X )X=width;   int x1=0,xN=X;
    Y =Tv.mY+1; if( height<Y )Y=height; int y1=0,yN=Y;
    X =PCX.bitpx*X/8 + 1;                        // ����� ��� �������� ������
    Xf=PCX.bitpx*width/8 + 1;                    // ����� ������ � PCX �����
//
//  ���� ����������, ���������� � ����������� ��������
//
ReRead: ans=0;
    hidevisualpage(); fseek( Fs,128,SEEK_SET ); B=(byte*)Allocate( width+8 );
    for( y=0; y<=height; y++,memset( B,0,width+8 ) )
    { for( px=1,j=PCX.nplanes-1; j>=0; j--,px<<=1 )
      for( V=B,i=0; i<PCX.bplin; )
      if( C-- )                                   //
      { if( y>=y1 )                               // ������ � �������� �������
        if( i>=x1 && i<xN )                       //
        { if( PCX.bitpx==8 ){ V[0]=D; V++; } else
          if( PCX.bitpx==1 )
          { char d=D;
            for( int k=0; k<8; k++ ){ if( d&0200 )V[0]|=px; d<<=1; V++; }
        } } ++i;
      } else
      { if( ((D=fgetc( Fs ))&0300)==0300 ){ C=D&077; D=fgetc( Fs ); } else C=1;
      }
      if( y>=y1 && y<yN )for( i=0; i<width; i++  )putpixel( i-x1,y-y1,B[i] );
      if( y>=yN-1 || feof( Fs ) )break;
      if( (ans=Tv_getw())!=0 )break;
    } Allocate( 0,B ); showactivepage();
//
//      �������� �����������, ������� ����� ���� � ������ ����
//
    if( !ans )ans=Tv_getc();
    if( X<width || Y<height )
    { switch( ans )
      { case North_West: x1=y1=0;               break;
        case North_East: x1=Xf-X; y1=0;         break;  // � ����� ��������
        case South_East: x1=Xf-X; y1=height-Y;  break;  // �������� ��������
        case South_West: x1=0;    y1=height-Y;  break;
        case South: y1+=Y/4;                    break;
        case North: y1-=Y/4;  if( y1<0 )y1=0;   break;
        case West : x1-=Y/16; if( x1<0 )x1=0;   break;
        case East : x1+=Y/16;                   break;
      }
      if( ans!=_Esc )
      { xN=x1+X; if( xN>Xf ){ xN=Xf; x1=Xf-X; }
        yN=y1+Y; if( yN>height ){ yN=height; y1=height-Y; } goto ReRead;
      }
    } fclose( Fs );
  }
}
