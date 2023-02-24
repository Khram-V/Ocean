//
//  World_Map( int R, Field *W ,Field *M ) Изображение карты Мира
//
//  - если R = 0 то производится безусловное изображение
//               карты M, а W указывает на предыдущее поле -> 1
//  - если R > 0 - организуется диалог для выбора региона  -> 1
//  - если R < 0 - W,M заполняются внутренними значениями  -> 0
//
#include <Ctype.h>
#include "..\Graphics\Tv_graph.h"
                                        //
fixed  World_Parameters=0;              // 0x0010  1: без координатной сетки
Course Help( char *Name[], char** Text, char** Plus );
                                        //
static Field wF={ 130,42,  45,20 },     // Текущее видимое поле
             mP={ 100,10, 152,70 };     // и отметка новых границ
                                        //
static Real Point2Map( Real LA )
{ Real La=Angle( LA );
  if( La<wF.Jx       ){ if( La+360<wF.Jx+wF.Lx )La+=360; }else
  if( La>wF.Jx+wF.Lx ){ if( La-360>wF.Jx       )La-=360; }return La;
}
static void Rect( Field Mp )
{
 int b=0; color( CYAN ); setwritemode( XOR_PUT );
                 Mp.Jx=Point2Map( Mp.Jx );
                 Mp.Ly+=Mp.Jy;
                 Mp.Lx+=Mp.Jx;
 Repeat: moveto( Mp.Jx,Mp.Jy );
         lineto( Mp.Jx,Mp.Ly );
         lineto( Mp.Lx,Mp.Ly );
         lineto( Mp.Lx,Mp.Jy );
         lineto( Mp.Jx,Mp.Jy );
  if( b^=1 )
  { if( Mp.Lx>wF.Jx+wF.Lx )
    if((Mp.Lx-=360)>wF.Jx ){ Mp.Jx-=360; goto Repeat; }
  } setwritemode( COPY_PUT );
}
static void WINDOW(){ Twindow( 0,-1,12,3 ); }
static void Axis_b()
{
 Bfont S( _Small_font,9+Tv.mX/213 );
 const int Light=GREEN,Dark=LIGHTGRAY;
 Field W=wF;
 Real Q,f,l,Jf=W.Jy,Lf=Jf+W.Ly, Jf_b,
         dl,Jl=W.Jx,Ll=Jl+W.Lx, Jl_b;
 int kb,ks=(Tv_getk()&SHIFT)!=0;
  color( Dark ); Tv_place( 0,&W ); clear();
  Tv_rect();
  if( World_Parameters&0x10 )ks^=1;
//if( ks )
  rectangle( Tv_port.left-4,Tv_port.top-4,Tv_port.right+4,Tv_port.bottom+4 );
  if( wF.Lx>180 )l=30,dl=5; else
  if( wF.Lx>80  )l=20,dl=5; else l=Tv_step( wF.Lx ),dl=l/5;
  if( ( kb=int( Jl/l ) )>=0 )++kb;
  for(;;kb++ )
  { if((Q=kb*l)>=Ll )break;      Jf_b=Tv_Y( Tv_y( Jf )+6 );
    color( Light );
    S.Text( South,Q,Jf_b,DtoA( Q ) );
    if( !ks )
    { if( kb )color( Dark ); line( Q,Jf,Q,Lf );
      if(!kb )color( Dark );
  } }
  if( wF.Ly>90  )f=20; else
  if( wF.Ly>40  )f=10; else f=Tv_step( wF.Ly );
  if( ( kb=int( Jf/f ) )>=0 )++kb;
  for(;;kb++ )
  { if((Q=kb*f)>=Lf )break;       Jl_b=Tv_X( Tv_x( Jl )-6 );
    color( Light );
    S.Text( West,Jl_b,Q,DtoA( Q ) );
    if( !ks )
    { if( kb )color( Dark ); line( Jl,Q,Ll,Q );
      if(!kb )color( Dark );
  } }         color( Dark );
  for( l=ceil( Jl/dl )*dl; l+dl<Ll; l+=dl+dl )
  {   moveto ( kb=Tv_x( l ),Tv_port.bottom-2 ); linerel( 0, 4 );
      linerel( ks=Tv_x( l+dl )-kb,0 );          linerel( 0,-4 );
      moveto ( kb,Tv_port.top+2 );              linerel( 0,-4 );
      linerel( ks,0 );                          linerel( 0, 4 );
  }   dl=f/5;
  for( l=ceil( Jf/dl )*dl; l+dl<Lf; l+=dl+dl )
  {  moveto ( Tv_port.left+2,kb=Tv_y( l ) );    linerel( -4,0 );
     linerel( 0,ks=Tv_y( l+dl )-kb );           linerel(  4,0 );
     moveto ( Tv_port.right-2,kb );             linerel(  4,0 );
     linerel( 0,ks );                           linerel( -4,0 );
  }                                        //
}                                          // Если Ret=0, то это
                                           // приводит к безусловному
int                                        // изображению карты <MP>,
World_Map( int Ret, Field *WF, Field* MP ) // WF Изображение на экране
{ const char                               // MP Отметка активного поля
       *Name[]={ "Region  ","   Geographical",
                       "Tools & Enviroment"  ,0 },
       *Text[]={ "F1   ","Band Information Window",
                 "F2   ","New Map, <Shift>grid out",
                 "F3   ","Specify Boundary Values",
                 "F4   ","Draw All World Picture",
                 "F10  ","Cleaning the Map Window",0 },
       *Plus[]={ "<0,1:9> ","Frame Step Multiplier",
                 "<Space> ","Set Frame to Image"  ,0 };
 static int ans=0xF2;
 const Real dF=1,dL=1;
 int  Mv=FALSE,WIND=FALSE,c=color( GREEN );
 Real f,l;
  if( Ret<0 ){ Ret=0; goto Break; } f=dF; l=dL;
  if( WF )wF=WF[0];
  if( MP )mP=MP[0]; { Field Tf=wF; Tv_place( 0,&Tf ); }
  if( !Ret )ans=0xFE;
  do
  { if( ans==_Enter )Mv^=1; else
    if(  ans<_Enter )
    { int Ev = Mv^( (Tv_getk()&SHIFT)!=0 );
      if( ans&North ){ if( Ev )mP.Jy+=f; else mP.Ly+=f; }
      if( ans&South ){ if( Ev )mP.Jy-=f; else mP.Ly-=f; }
      if( ans&East  ){ if( Ev )mP.Jx+=l; else mP.Lx+=l; }
      if( ans&West  ){ if( Ev )mP.Jx-=l; else mP.Lx-=l; }
          mP.Jx=Angle( mP.Jx );
      if( mP.Lx>360 )mP.Lx=360;
      if( mP.Jy<-90 ){ mP.Ly+=mP.Jy-90; mP.Jy=-90; }
      if( mP.Jy+mP.Ly>90 ){ if( mP.Jy>89 )mP.Jy=89; mP.Ly=90-mP.Jy; }
      if( mP.Ly<dF )mP.Ly=dF;
      if( mP.Lx<dL )mP.Lx=dL;
    }else
    { switch( ans )
      { case ' ' : mP=wF; break;
        case 0xF3: if( !WIND )break;
        case 0xF1: if( WIND^=1 )WINDOW(); else Tback(); break;
        case 0xFa: ans=0xF2; Ret=0; Ghelp(); clear(); goto Break;
        case 0xF4: wF.Jy=-90; wF.Ly=180;
                   wF.Jx=20;  wF.Lx=360; goto OxFE;
        case 0xF2: { Field W=wF; wF=mP; mP=W; }
        case 0xFE: OxFE:
         if( WIND )Tback(); Axis_b(); Map_View( wF );
         if( WIND )WINDOW(); ans='1';
         if( !Ret ){ Ret=1; goto Break; }
       default:
        if( isdigit( ans ) )
        { if( ( ans-='0' )<2 )f=dF,l=dL; else
          { f*=ans,l*=ans; if( f>=mP.Ly/3 )f=mP.Ly/3;
                           if( l>=mP.Lx/3 )l=mP.Lx/3;
          }
        } else ans='h';
      } Ghelp( "F1 Inf F2 Draw F3 Edit F4 all World F10 Clear"
               " <Space>Frame <0,1..9>Step <H>elp" );
    }
    if( WIND )
    { color( CYAN );                            Tgo( 5,1 );
      Tprintf( "%4s\n",DtoA( mP.Jy+mP.Ly,1 ) );
      Tprintf( "%5s +",DtoA( mP.Jx,      1 ) );
      Tprintf( "%-5s", DtoA( mP.Jx+mP.Lx,1 ) ); Tgo( 5,3 );
      Tprintf( "%4s",  DtoA( mP.Jy,      1 ) );
    }
    Rect( mP );
    if( ans!=0xF3 )
    { if( (ans|0x20)=='h' )ans=Help( Name,Text,Plus ); else
                           ans=Tv_getc(); Rect( mP );
    } else
    { Real f=mP.Jy, ff=f+mP.Ly,
           l=mP.Jx, ll=l+mP.Lx; color( GREEN );
      Mlist Menu[]={ { 0,8,"      %2°",&ff }
                   , { 1,8,      "%2°",&l  }
                   , { 0,8,  "=+= %2°",&ll }
                   , { 1,8,"      %2°",&f  }
                   };
      ans=Tmenu( Mlist(Menu),0 ); Rect( mP ); if( ll<=l )ll+=360;
      mP.Jy=f; mP.Ly=ff-f;
      mP.Jx=l; mP.Lx=ll-l;
      mP.Jx=Angle( mP.Jx );
      if( ans<_Enter )ans=0xF3; else if( ans==_Esc )ans=0;
    }
  } while( ans!=_Esc ); Break: if( WIND )Tback(); color( c );
                               if( WF )WF[0]=wF;
                               if( MP )MP[0]=mP; return Ret;
}
//
//  ===============================================
//
#define MulDiv( X,A,Az,B,C )                      \
     { w=A<Az;                                    \
        if( w )X=int( Az-A ); else X=int( A-Az ); \
               X*=B; X/=C;                        \
        if( w )X=-X;                              \
     }
#define Move( Y,X )                  \
{ int y,x,w; MulDiv( y,Y,JF,Ly,Vf )  \
             MulDiv( x,X,JL,Lx,Vl )moveto( Jx+(Kx=x),Jy-(Ky=y) ); }
#define Line( Y,X )                  \
{ int y,x,w; MulDiv( y,Y,JF,Ly,Vf )  \
             MulDiv( x,X,JL,Lx,Vl )  \
               if( Ky!=y || Kx!=x )lineto( Jx+(Kx=x),Jy-(Ky=y) ); }
                          //
void Map_View( Field wF ) // в градусах
{                         //
 Field W=wF;
 Real Jf=W.Jy,Lf=Jf+W.Ly,
      Jl=W.Jx,Ll=Jl+W.Lx; int kb; FILE *Fw;
 int Ver=0;
  { static char Wn[]="World.bin";
           char Wname[82];
    if( ( Fw=fopen( Wn,"rb" ) )==0 )
    { strcpy( sname(strcpy(Wname,_av_[0])),Wn ); Fw=fopen( Wname,"rb" );
    }
    if( Fw )
    { fread( Wname,1,6,Fw );
      if( !memcmp( Wname,"World",5 ) )
      { if( Wname[5]=='\xF8' )Ver=1;   // °
        if( Wname[5]=='\xFC' )Ver=2;   // №
      } if( !Ver ){ fclose( Fw ); Fw=0; }
  } }
  if( Fw )
  { long Fo,Fi,F,JF=int( Jf*360 ),LF=int( Lf*360 ),
         Lo,La,L,JL=int( Jl*180 ),LL=int( Ll*180 );
     int Vf=int( LF-JF ),
         Vl=int( LL-JL );
     int Jy=Tv_port.bottom,Ly=Jy-Tv_port.top,  Ky=0,vy,
         Jx=Tv_port.left,  Lx=Tv_port.right-Jx,Kx=0,v,b,N,k,K,nr,ns;
    if( Ver==1 )          //
    {     N=get2( Fw );   // Общее количество
      ns=nr=N-get2( Fw ); //
    } else
    { if( (kb=get2( Fw ))!=3 )Tv_bell();
      ns=get2( Fw );
      nr=get2( Fw ); N=nr+ns+get2( Fw ); nr=N-ns-nr; ns=N-ns;
    }
    color( DARKGRAY );       //
    for( kb=0; kb<N; kb++ )  // 360x180 = 64800
    {    K=get2( Fw );       //
      Fo=F=get2( Fw );
      Lo=L=get2( Fw );
      if( kb==nr )color( GREEN );
      if( kb==ns )color( BLUE ),setlinestyle( SOLID_LINE,0,2 ); //THICK_WIDTH );
      for( b=k=1; k<=K; k++ )
      { Fi=F;
        La=L;
        if( k==K )
        { if( kb<ns )break; F=Fo; L=Lo;
          if( L-La>32400L )L-=64800L; else
          if( La-L>32400L )L+=64800L;
        } else
        { F += (signed char)getc( Fw );
          L += (signed char)getc( Fw ); } vy = ( F>=JF && F<=LF );
                                          v=vy&( L>=JL && L<=LL );
Repeat: if( v&b ){ Move( Fi,La ) b=FALSE; }
        if( !b  ){ Line( F,L ) }
        if( !v  ){ b=TRUE;
          if( vy )
          { if( L<JL )La+=64800l,L+=64800l; else
            if( L>LL )La-=64800l,L-=64800l; else v=3;
            if( v<3 )
            if( L>=JL && L<=LL ){ v=3; goto Repeat; }
          }
        }
      }
    } fclose( Fw ); setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  } else Tv_bell();
}
