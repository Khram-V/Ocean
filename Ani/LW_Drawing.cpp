//
//     Long_Waves_Draw.cpp                     /90.11.03-07.02.16/
//
//     Комплекс процедур для изображения промежуточных результатов.
//              Реализован вывод штрихованных полей уровня моря, с разделением
//              областей изолиниями. Дополнительные режимы позволяют получать
//              изображения горизонтальных профилей волнового поля и векторного
//              поля скорости течения или потоков.
//
#include "LongWave.h"
#include "..\Graphics\Tf_Topo.h"

static bool Met_mark=false;

void TWave::EGraph( bool draw )          //  Регистрация нового шага во времени
{ if( !draw )
  { if( !(VideoM&2) && Kt>0 )  // двойной барьер от сложений пустых перерисовок
    { if( VideoM&1 )
      { static int sT=-1; if( int( Instant.T/St )!=sT ){ sT=Instant.T/St; goto Begin; }
      } else              if( Kt%int( St+0.5 )==0 )goto Begin;
    } goto Finish;
  }
Begin:                        //  Собственно начало тяжелой процедуры рисования
{
 const int gStep[]={ 6,10,15,20,30,40,50,60,80,100,120,150,
                     200,250,300,400,500,600,800,1000,2000 };
 const unsigned P[][2]={ { 0x01000000L,0x10000000L }     // 2
                       , { 0x11000000L,0x11000000L }     // 4
                       , { 0x11004400L,0x11004400L }     // 6
                       , { 0x5500AA00L,0x5500AA00L } };  // 8
 Bfont RP( _Small_font,12 );
 Real  Q[4],Dh,d,h,Hmax,Hmx=0,Hmin,Hmn=0,Umax=0;
 point p,q,r,v;
 Field Old=Map.Focus();
 int k,y,x,c=color( YELLOW ),cy,
     Th=Tv.Th+2,l=(Th-2)*2+2,  // Единица масштабирования
     Tw=Tv.Tw,  L=Tw*16;       // Отступ для шкалы
     Met_mark=false;           // Установка режима мультипликации
     VideoM|=2;                // Установка признака начала обновления картинки
     hidevisualpage();
     cleardevice();
  for( y=k=0; y<My; y++ )      // Цикл расчета экстремумов
  for(   x=0; x<Mx; x++ )      //         для поля уровня
  if( Dp[y][x]>DZ )            // <= Hg[y][x]=1e24; else // контроль касания
  { h=Hg[y][x];                // == // Здесь переполняются пустые ячейки
    if( !k ){ Hmx=Hmn=h; k=1; } else //  для исключения пересечений
    { if( Hmx<h )Hmx=h; else         //  береговых участков
      if( Hmn>h )Hmn=h;              //
  } } Hmax=Hmx,Hmin=Hmn;
  if( WaveAs&0x1000 )            //
  { if( VideoM&0x2000 )          // Поиск экстремумов на мореографных постах
    for( k=0; k<Nmp; k++ )       //
    { h=mxH[k]; if( Hmax<h )Hmax=h;
      h=mnH[k]; if( Hmin>h )Hmin=h;
  } }
  for( h=max(-Hmin,Hmax),x=0; x<int( sizeof(gStep)/sizeof(gStep[0]) ); x++ )
  {       Dh=Real( gStep[x] )/100.0;
    if( h<Dh*3.0 )break;
  }                                      //
  for( x=0,h=Dh; x<4; x++,h+=Dh )Q[x]=h; // Вектор раскраски
//
//  1++. Свето-теневая раскраска побережья и моря
//
  if( (VideoM&0x8) && (h=Hmax-Hmin)>1.0e-4 )
  { Shadow( min( Real( 12.0 ),h ) ); // Tf_Line ( Hg,Ny,Nx,0.0,111 );
  } else
//
// 2. Безусловное изображение побережья, - по требованию
//
  { const Real Lvl[]={-5000.01,-2000.01,-1000.01,-500.01,
                      -200.01,-100.01,-50.01,-20.01,-10.01 };
    const int  Clr[]={ 252,232,225,220,216,212,210,208,LIGHTGREEN }; Tf_List();
    for( k=0; k<9; k++ )
     if( Chart.Min<=Lvl[k] )Tf_List( Lvl[k],Clr[k] );
    if( DZ>0.0 )Tf_List( DZ,LIGHTGRAY ); // Изображение контура берега и зоны
    Tf_Topo<float>( Chart,Map.Jy,Map.Jx,Map.Ly,Map.Lx,true ); // отсечения на
  }                                                           // мелководье
  setwritemode( XOR_PUT );  setlinestyle( SOLID_LINE,0,THICK_WIDTH );
  Tf_Line<float>( Chart,Map.Jy,Map.Jx,Map.Ly,Map.Lx,1.0e-12,YELLOW^WHITE );
  setwritemode( COPY_PUT ); setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  Tf_Line<float>( Chart,Map.Jy,Map.Jx,Map.Ly,Map.Lx,0.01,DARKGRAY );
  Map.axis( Chart,96 );
//Chart.view();
//
//  3.  Блок построения масштабной шкалы
//
  Tv.Height( Tv.Th/2 );
    y=Tv_port.bottom+Th+6; k=r.y=y+18; // +l;
  r.x=Tv_port.right-L;          //
  p=q=r; q.x+=6;                // Инициировано для подписи
         p.x-=6; h=Dh;          //  о типе закраски уровней
  line( r.x,y,r.x,k-1 );        // Отметка нуля
  for( x=0; x<4; x++,h+=Dh )    //
  { if( h-Dh<=Hmax ){ p.x-=l;                            color( LIGHTBLUE );
      rectangle( p.x,y,p.x+l,k ); setfillpattern( (byte*)(P[x]),LIGHTBLUE );
        bar( p.x,y,p.x+l-1,k-1 ); }
    if( Dh-h>=Hmin ){ q.x+=l;                            color( GREEN );
      rectangle( q.x-l,y,q.x,k ); setfillpattern( (byte*)(P[x]),GREEN );
        bar( q.x-l+1,y,q.x,k-1 ); }
  }
//
//  3.2. Подпись экстремумов и размера одной ячейки
//
  p.y=q.y=r.y-7*Th/10; color( CYAN );

  p.x--; if( Hmx<Hmax )Tv.Text( South_West,p,_Fmt( "%3.1lf",Hmx )); // p.y-=Th;
  q.x++; if( Hmn>Hmin )Tv.Text( South_East,q,_Fmt( "%3.1lf",Hmn )); // q.y-=Th;
  color( LIGHTGRAY );
  p.x--; if( Hmax>Dh/10  )Tv.Text(North_West,p,_Fmt("%3.1lf",Hmax));
  q.x++; if( Hmin<Dh/-10 )Tv.Text(North_East,q,_Fmt("%3.1lf",Hmin)); r.y-=7*Th/10;
                          Tv.Text(North,r,     _Fmt("%lg",   Dh ));  r.y+=7*Th/10;
//
//  3.3. Отметка текущих амплитудных экстремумов на шкале
//
                                         color( CYAN );
  moveto( x=int( r.x-Hmx*l/Dh ),r.y );   lineto( x,r.y-3 );
  lineto( x=int( r.x-Hmn*l/Dh ),r.y-3 ); lineto( x,r.y );
//
//  4.  Блок рисования профилей и расчета векторных экстремумов
//
//setviewport( v.x=Tv_port.left,v.y=Tv_port.top,Tv_port.right,Tv_port.bottom,1 );
//setviewport( Tv_port.left,Tv_port.top,Tv_port.right,Tv_port.bottom,1 );
  TvClip();
  if( Sp>0 )                    //
  {                             // Образуется область отсечения
   int *Lw=NULL,Nw=0;           // Здесь подключена верхняя строка Uy
    if( VideoM&0x40 )           //
    { if( !Lw || Nw<Mx )Lw=(int*)Allocate( (Nw=Mx)*sizeof( int ),Lw );
      cy=Tv_y( 0 ); for( x=0; x<Mx; x++ )Lw[x]=cy;
    }
    for( y=0; y<My; y+=Sp )
    { q.y=Tv_y( y ); //-v.y;    //
      for( k=x=0; x<Mx; x++ )   // для пересчета знака уровня вместе с p
      { int w=0;                //
        q.x=Tv_x( x ); //-v.x;
        if( Dp[y][x]>DZ )w=int( Hg[y][x]*( l*3 )/Dh );
        cy=q.y-w/3;
        if( VideoM&0x40 ){ if( cy>Lw[x] ){ cy=Lw[x]; w=0; } else Lw[x]=cy; }
        if( !w )
        { if( k )lineto( q.x,cy ); else moveto( q.x,cy ); k=0; } else
        {    k=1;                       //
          if( !x )moveto( q.x,cy );     // Линия уровня моря
          else                          //
          { color( w<0?GREEN:LIGHTBLUE ); lineto( q.x,cy ); }
  } } } }
//
//  5.  Блок полей течений или потоков
//
  if( Fx>0 )
  { for( y=Fy; y<My-1; y+=Fy )  //
    for( x=Fx; x<Mx-1; x+=Fx )  // Расчет экстремумов потока
    if( ( d=Dp[y][x] )>DZ )     //
    { h=hypot( (Uy[y][x]+Uy[y+1][x])/2,(Ux[y][x]+Ux[y][x+1])/2 );
      if( !(VideoM&0x20) )h/=d;
      if( Umax<h )Umax=h;
    }
    if( Umax>0 )
    { Hmx=Tv_step( Umax ); Hmx*=Umax/Hmx>6 ? 6.0:3.0; color( CYAN );
      for( cy=Fy*2,y=(( My-2 )/cy)*cy; y<My-1; )
      { p.y=Tv_y( y ); //-v.y;
        for( x=Fx; x<Mx-1; x+=Fx )if( (d=Dp[y][x])>DZ )
        { if( VideoM&0x20 )d=1;
          p.x = Tv_x( x ); //int( Tv_x( x )-v.x );
          q=p;
          d=l/d/Hmx;
          q.y-=int( ( Uy[y][x]+Uy[y+1][x] )*d );
          q.x+=int( ( Ux[y][x]+Ux[y][x+1] )*d ); needle( p,q );
        } if( y%cy )y+=cy; else{ y-=cy; if( y<Fy )y=Fy; }
  } } }
  TvUnClip();
  if( Fx>0 && Umax>0 )
      Tv.Text( South,r.x+6,r.y+2,
        _Fmt( VideoM&0x20 ? "Flow by %.1lf of %.1lf m\xFD/c"
                         : "Drift by %.2lf of %.1lf m/c",Hmx/2,Umax ) );
//
//  6.  Режим окаймления одинаковых уровней моря
//
  if( VideoM&0x4 )
  { Tf_List();
    for( k=-4; k<=4; k++ )
    { if( k==0 )++k;
      h=Q[ abs( k )-1 ];
      Tf_List( k<0?-h:h,k<0?GREEN:LIGHTBLUE );
    } Tf_List( 1.0e-3,LIGHTGRAY );
      Tf_Topo<double>( Hg,Map.Jy,Map.Jx,Map.Ly,Map.Lx,true );
  }
//
//  7.  Отметка экстремальных величин в мореографных пунктах
//
  pattern( EMPTY_FILL,BLACK );
  if( VideoM&0x6000 )
  for( k=0; k<Nmp; k++ )
  { Point P=Mg[k].G; Chart.DtI( P ); char symb=Mg[k].cp[0];
    r.y=int( P.y+0.5 );
    r.x=int( P.x+0.5 );
    if( r.x<0 || r.y<0 || r.x>=Mx || r.y>=My )continue;
    RP.Height( 12 );
    p=P; P.y=0;
    if( Dp[r.y][r.x]>DZ )
    if( Kt && WaveAs&0x1000 )
    {   r.y=int( P.y+0.5 );
        r.x=int( P.x+0.5 ); q.x=p.x;
      if( VideoM&0x4000 )P.y=wTm[k];
      if( (P.x=mxH[k])>0.1 )
      { if( VideoM&0x2000 )
        if( (y=int( P.x*( l )/Dh ))>5 )color( RED ),
                                       bar3d( p.x-1,p.y-y,p.x+1,p.y,1,1 );
        if( symb!=250 && symb!='+' )
        if( VideoM&0x4000 )
        { char Msg[60];
          int c=sprintf( Msg,"%.1lf",P.x );             // Поправка на глубину
          if( Dv>DZ && Dv>0.01 )
          { c+=sprintf( Msg+c,"\\%.1lf",P.x*sqrt( Dp[r.y][r.x]*DZ/Dv ) );
          }
          if( P.y!=0.0 )sprintf( Msg+c,",%s",DtoA( mTm[k],3,':' ) );
          q.y=p.y-Tv.Th/2; color( LIGHTBLUE );
          RP.Text( North,q,Msg );
      } }
      if( VideoM&0x2000 )
      if( (P.x=mnH[k])<-0.1 )
      { if( (y=int( P.x*( l )/Dh ))<-5 )color( RED ),
                                        bar3d( p.x-1,p.y,p.x+1,p.y-y,1,0 );
        if( symb!=250 && symb!='+' )
        if( VideoM&0x4000 )
        { q.y=p.y+(Tv.Th-2)/2; color( GREEN );
          RP.Text( South,q,_Fmt( "%2.1lf",P.x ) );
      } } color( LIGHTRED ); line( p.x-2,p.y,p.x+2,p.y );
    }
    if( symb==250 || symb=='+' ){ color( LIGHTGRAY ); Tv_cross( p,-1 ); } else
    { char *s=_Fmt( P.y?"%s%s":"%s",((Active&6)==6?Mg[k].cp:""),DtoA(P.y,3,':') );
      RP.Height( 14 );
      color( 255 ); setlinestyle( SOLID_LINE,0,THICK_WIDTH ); RP.Text( Center,p,s );
      color( BLUE ); setlinestyle( SOLID_LINE,0,NORM_WIDTH ); RP.Text( Center,p,s );
  } } color( c );
  // ViewPointMario();
  Tv_place( 0,&Old ); Tv.Height( 0 ); // Восстановление метрики обычного шрифта
  showactivepage();
}
Finish: Print_Time();  // и печать текущего времени процесса
}     //
      // WaveIs:        1 ¦ Задан период входящей волны
      //                2 ¦ Отслеживается протяженность граничной волны
      //                4 ¦ Учет геопотенциала
      //             1.00 ¦ Spherical Earth    \ .
      //             2.00 ¦ Coriolis  Forces    \ Дополнительные
      //             4.00 ¦ Viscosity Friction  / элементы вычислений
      //             8.00 ¦ Atmosphere Cyclone /
      //            10.00 - Признак запуска процесса записи мореограмм
      // VideoM:        1 - Время на экране в абсолютных отсчетах
      //                2 - Изображение только что обновлено
      //                4 - Вывод (или нет) изолиний уровня
      //                8 - свето-теневая закраска уровней моря
      //               xC = 8+4 - Свето-теневая раскраска + контуры уровней
      //            ---10 - Берег изображается контуром или закраской
     //                20 - Вывод потоков течения, иначе скоростей
    //                 40 - 3d-style или обычные профили
   //                  80 - Включен режим мультипликации EGA монитора
  //                20.00 - Прорисовка уровней в Mario-точках
 //                 40.00 - Подпись времен и экстремальных величин
//
//      Собственно блок рисования топографических карт
//
static Point Az={ 0.5,-0.8660254 }; // Угол освещения в геометрической СК

void TWave::Shadow( Real Hmax )
{
 static int xL=0;
 static Real *C=NULL,
             *D=NULL;
  if( Map.Lx>xL ){ xL=Map.Lx; C=(Real*)Allocate( xL*sizeof( Real ),C );
                              D=(Real*)Allocate( xL*sizeof( Real ),D ); }
 int   y,Y,V,x,X,d,yY=-1;
 Real  W,kY,kV,kX,R = Pd*8.0*hypot( Dy,Dx )/Hmax;
 Point P;
//Map.Focus();
  Color_Paint( Chart,Map.Jy,Map.Jx,Map.Ly,Map.Lx );
  //
  //   Прорисовка затененной карты
  //
  for( y=Tv_port.bottom-1; y>=Tv_port.top; y-- )
  { V=minmax( Map.Jy,int( kV=Tv_Y( y )-0.5 ),Map.Jy+Map.Ly-2 ); kV-=V;
    if( yY!=V )for(;;)                        //
    { Real *Z=C; C=D; D=Z;                    // Подготовка двух векторов
      for( X=Map.Jx; X<Map.Jx+Map.Lx-1; X++ ) // со значениями яркости свечения
      {    P=0.0;                             //
        if( Dp[V][X]>DZ )
        { if( Dp[V+1][X]>DZ )P.y=( Hg[V+1][X]-Hg[V][X] )/Dy;
          if( Dp[V][X+1]>DZ )P.x=( Hg[V][X+1]-Hg[V][X] )/Dx;
        }
        D[X-Map.Jx]=48.0*( 1.0+tanh( ( P.x*Az.x+P.y*Az.y )*R ) );
      //D[X-Map.Jx]=48.0*( 1.0 + cos( arg( P/Az ) )   // - направление
      //                 * tanh( abs( P )*R ) );      // - наклон
      } if( yY<0 ){ yY=0; continue; } yY=V; break;
    }
    Y=minmax( Map.Jy,int( kY=V+kV+0.5 ),Map.Jy+Map.Ly-2 ); kY-=Y;
    for( x=Tv_port.left; x<Tv_port.right; x++ ) //,e++ )
    { byte ec;
      X=minmax( Map.Jx,int( kX=Tv_X( x ) ),Map.Jx+Map.Lx-2 ); kX-=X;
      W = ( ( Dp[Y][X+1]*kX + Dp[Y][X]*(1-kX) )*(1-kY)
          + ( Dp[Y+1][X+1]*kX+Dp[Y+1][X]*(1-kX) )*kY );
//    if( W<=0.0 )
//    { if( VideoM&0x10 )d=5; else // на берегу контуры или закраска
//      d=int( sqrt( -W )*0.50 ); if( d<0 )d=0; else if( d>47 )d=47;
//      ec=d+208;
//    } else
//    if( W<=DZ ){ ec=LIGHTGRAY; putpixel( x,y,ec ); }
//    else
      if( W>DZ )
      { ec=( ( Hg[Y][X+1]*kX+Hg[Y][X]*(1-kX) )*(1-kY)
           + ( Hg[Y+1][X+1]*kX+Hg[Y+1][X]*(1-kX) )*kY ) < 0.0 ?112:16;
        X=minmax( Map.Jx,int(kX=X+kX-0.5),Map.Jx+Map.Lx-3 ); kX-=X; X-=Map.Jx;
        X=minmax( 0,int( ( D[X]*(1-kX)+D[X+1]*kX)*kV             // 2-я степень
                       + ( C[X]*(1-kX)+C[X+1]*kX)*(1-kV) ),95 ); // сглаживания
        putpixel( x,y,ec+=X );
      }
    }
  }
//  for( int i=16; i<208; i++ )
//  for( int i=16; i<256; i++ )
//  { pattern( SOLID_FILL,i );
//    bar( 52+i*3,Tv_port.bottom-24,55+i*3,Tv_port.bottom );
//  }
}
void TWave::Print_Time()  // Процедура печати времени процесса
{                         //
 point pt={ 24,Tv.mY-2 }; //_port.bottom+( Tv.Th+4 )*2;
  if( VideoM&1 )
  { int m,d,y;            // if( Tlaps==0.0 )Instant.Now();                                                  // { Dat=julday(); Tim=onetime(); } julday( Dat,m,d,y );
    Instant.UnPack( m,d,y );
    Tv.Text( North_East,pt,
    _Fmt( "%d %s %04d,%s    ",d,W2D( _Month[m-1] ),y,DtoA(Instant.T,3,':') ) );
  } else
  { unsigned Day=unsigned( Tlaps/86400.0 ); char S[8];
    sprintf( S,Day>0?" %d\x7F":"",Day );
    Tv.Text( North_East,pt,
    _Fmt( "Time[%d]%s%s   ",Kt,S,DtoA( Tlaps/3600.0-Real( Day )*24.0,3,':' )));
  }                           //
  if( WaveAs&0x800 )          // Оперативная отметка местоположения циклона
  { static METEO bM;          //
    setwritemode( XOR_PUT ); color( LIGHTRED^WHITE );
    if( Met_mark )Meteo_Mark( bM ); Meteo_Mark( bM=aM ); Met_mark=true;
    setwritemode( COPY_PUT );
  }                           //
  if( WaveAs&4 )              // Прямое воздействие геопотенциала Луны и Солнца
  { Point P;                  //
    color( YELLOW ); P=Sun*Radian;  P.x=Angle( -P.x ); Chart.DtI( P ); Tv_cross( P,-6 );
    color( CYAN );   P=Moon*Radian; P.x=Angle( -P.x ); Chart.DtI( P ); Tv_cross( P,-6 );
  }                                 //
  if( Rt>0.0 )                      // Построение точек рестарта
  { static Real r=0.0;              //
           Real w=floor( onetime()/Rt ); if( w-r>1.0 ){ save(); r=w; }
  }
}

