//
//      Комплекс графических программ класса Depth           /90.08.20/
//
#include "..\Ani\Depth.h"
//
//      Построение географической сетки                      /91.02.06/
//      Пропорциональное равноугольное
//      масштабирование изображения батиметрических карт     /07.03.11/
//
static Real STEP( Real Dst )
{ Dst=Tv_step( Dst ); if( Dst>30 )Dst=floor( Dst/30 )*30; return Dst;
}
static void bound( int k, Point P, point &s, point &z )
{ s=z=P;
  switch( k )
  { case 1: s.y-=3; z.y=Map.b.bottom+1; break;
    case 2: s.x-=3; z.x=Map.b.right +1; break;
    case 3: s.y+=3; z.y=Map.b.top   -1; break;
    case 4: s.x+=3; z.x=Map.b.left  -1;
} }
void Screen::axis( Depth &Dp, int Cl )
{
 Bfont RP( _Small_font,Tv.Th>10?Tv.Th-2:Tv.Th+2 );
 point z={ 0,0 },s,r[4],p,q;
 int cl=color( 112 ),k,m,kp,qp=6;
  pattern( EMPTY_FILL,WHITE );
  //
  // Data Title & Size of Image Field Calculating
  //
  Focus();                         // собственно активизация поля разгула мышки
  Tv.Height( 10 );                 // мелкий шрифт 10
  p.x=b.left;  p.y=b.bottom;
  q.x=b.right; q.y=b.top;       bar( 0,0,q.x,q.y     ); z.x=( q.x+p.x )/2;
  Tv.Text( South,z,Dp.Msg[0] ); bar( 0,q.y,p.x,p.y   ); z.y+=9;
  Tv.Text( South,z,Dp.Msg[1] ); bar( 0,p.y,q.x,Tv.mY ); color( Cl );
  Tv_rect();
  rectangle( b.left-3,b.top-3,b.right+3,b.bottom+3 ); //
  { Point St[7]={{0,-3},{1,2},{0,1},{-1,2},{0,-3},{0,-4.25},{0,3}};  // стрелка
    color( GREEN ); setlinestyle( SOLID_LINE,0,THICK_WIDTH );       // на север
    if( Dp.is_Globus() )circle( b.left/2,b.bottom-2,9 );      // и глобусу
    for( m=0; m<2; m++ )
    { for( k=0; k<7; k++ )
      { if( !m )St[k]*=polar( 6.0,-Dp.Cs/Radian );
        z.y=b.bottom+int( St[k].y );
        z.x=b.left/2+int( St[k].x );
        if( k==6 ){ color( !m?YELLOW:LIGHTRED ); RP.Text( Center,z.x+2,z.y,"S" ); } else
        if( k==5 ){ color( !m?YELLOW:CYAN );     RP.Text( Center,z.x+1,z.y,"N" ); } else
        if( !k )moveto( z ); else { color( !m?YELLOW:LIGHTBLUE ); lineto( z ); }
      } setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  } }   color( DARKGRAY );
  RP.Text( South_East,b.left-12,b.bottom+RP.Th+4,sname( Dp.InfName ) );
  color( BLUE );
  RP.Text( South_East,b.left-18,b.bottom+RP.Th*2+4,
    _Fmt( "%s: { %dx%d } %s",Dp.isMercator?Mercator:Geograph,My,Mx,
      Dp.Type==0?"Text": Dp.Type==1?"Byte":Dp.Type==2?"Integer":"Float" ) );
//
//     Разметка рамки карты и географических осей
//
 Point P[4]={ {Jx,Jy},{Jx+Lx,Jy},{Jx+Lx,Jy+Ly},{Jx,Jy+Ly} }, // шкала на
       Q,Pmin,Pmax,Pst,R[2],Pw;                              // внешней рамке
  Tv.Height( 0 );
  color( Cl );                    //
  for( k=3; k>=0; k-- )           // Координатные   [3] -<3>- [2]
  { Q=P[k]; Dp.ItD( Q );          // экстремумы     <4>   X   <2>
    if( k==3 )Pmin=Pmax=Q; else   //                [0] -<1>- [1]
    { if( Q.y>Pmax.y )Pmax.y=Q.y; else if( Q.y<Pmin.y )Pmin.y=Q.y;
      if( Q.x>Pmax.x )Pmax.x=Q.x; else if( Q.x<Pmin.x )Pmin.x=Q.x;
  } } Pst.y=STEP( Pmax.y-Pmin.y )/qp; // Шаг графической сетки
      Pst.x=STEP( Pmax.x-Pmin.x )/qp; //
 Real W,w;
 const Course Dir[]={ Center,South,East,North,West };
 const bool Crs=fabs( Dp.Cs )<=45.0 || fabs( Dp.Cs )>135.0;
//
//      L A T I T U D E
//
  kp=int( Pmin.y/Pst.y+1 );
  for( k=0; k<4; k++ )r[k].x=-1;
  for( W=Pst.y*kp; W<Pmax.y; W+=Pst.y,kp++ )
  { Dp.D2I( Pw=(Point){ Pmin.x-Pst.x/qp,W } );
    Dp.D2I( Q=(Point){ Pmax.x+Pst.x/qp,W } );
    for( m=0,k=1; k<=4; k++ )if( m<2 )
    if( !Cross( Pw,Q,P[k-1],P[k&3],R[m] ) )
    { if( (Crs && !(k&1)) || (!Crs && (k&1)) )
      { bound( k,R[m],s,z );
        if( r[k-1].x>-1 ){ line( r[k-1],z ); r[k-1].x=-1; } else r[k-1]=z;
        if( !(k&2) )
        if( !(kp%qp) )
        { if( k==4 )s.x-=10; else s.y+=10;
          color( BLUE ); RP.Text( Dir[k],s.x-2,s.y-2,DtoA( W,3,0xF8 ) );
          color( Cl );
        } line( z,s );
      }   m++;
    } if( kp%qp==0 && m==2 ){ color( Cl|8 ); line( R[0],R[1] ); color( Cl ); }
  }
//
//      L O N G I T U D E
//
  kp=int( Pmin.x/Pst.x+1 );
  for( k=0; k<4; k++ )r[k].x=-1;
  for( W=Pst.x*kp; W<Pmax.x; W+=Pst.x,kp++ )                //
  { Dp.D2I( Pw=(Point){ W,Pmin.y-min( 4.0,w=Pst.y/qp ) } ); // С учетом того,
    Dp.D2I(  Q=(Point){ W,Pmax.y+min( 4.0,w ) } );          // что меркаторская
    for( m=0,k=1; k<=4; k++ )if( m<2 )                      // широта не должна
    if( !Cross( Pw,Q,P[k-1],P[k&3],R[m] ) )                 // превышать 85°
    { if( (Crs && (k&1)) || (!Crs && !(k&1)) )              //
      { bound( k,R[m],s,z );
        if( r[k-1].x>-1 ){ line( r[k-1],z ); r[k-1].x=-1; } else r[k-1]=z;
        if( !(k&2) )
        if( !(kp%qp) )
        { if( k==4 )s.x-=10; else s.y+=10;
          color( BLUE ); RP.Text( Dir[k],s.x-2,s.y-2,DtoA( W,3,0xF8 ) );
          color( Cl );
        } line( z,s );
      }   m++;
    } if( !(kp%qp) && m==2 ){ color( Cl|8 ); line( R[0],R[1] ); color( Cl ); }
  }   color( cl ); Tv_isKey();
}
#include "..\Graphics\Tf_Topo.cpp"
//
//      Dp_Show.cpp                                                  /91.02.27/
//      Программа отрисовки оперативного файла батиметрии
//
static const struct{ Real D; int c; }
       Level[] = { { -2999.99,255 }, { -999.99,225 }, { -499.99,216 }
                 , {   -99.99,212 }, {   20.01,111 }, {   50.01,105 }
                 , {   100.01,100 }, {  200.01,90  }, {  500.01,80  }
                 , {  1000.01,70  }, { 2000.01,60  }, { 5000.01,50  }
                 , {  8000.01,30  }, {    0,   16  } };

static const int LCC = sizeof( Level )/sizeof( Level[0] );
static void List()
{           Tf_Color( 111,16,208 ); //LIGHTGREEN );
            Tf_List( 0 );
  for( int k=0; k<LCC; k++ )Tf_List( Level[k].D,Level[k].c );
}
static int List( Real D )
{ int c=LCC-1;
  if( fabs( D )>=1.0 )
  for( c=LCC-2; c>0; c-- )if( D>Level[c-1].D ){ if( D<0 && c==4 )c=3; break; }
  return Level[c].c;
}
//      Программа управления выводом изолиний                        /91.03.01/
//
static int sort_function( const void *a, const void *b)
{        return *(Real*)b < *(Real*)a ? -1:1;
}
void Dp_Show( float **_D,int Jy,int Jx,int Ly,int Lx,int clr ) // (*axis)()
{
 const char *Name[]={ "Isoline  ","Прорисовка линий уровня",
                                  "для двумерных полей",0 },
             *Text[]={ "F2  ","Разметка географических осей",
                       "F3  ","Прорисовка по списку уровней",
                       "F4  ","Автоматический выбор уровней",
                       "F5  ","Автоматическая прорисовка",
                       "F9  ","Очистка поля изображения",
                       "F10 ","Очистка экрана и списка",0 },
             *Plus[]={ "<Цифры> ","Ручной ввод уровня",0, };
 if( !_D )return;
 int ans=1,lst=0, k,l,x,y, co=color( clr ); //, Ny=Jy+Ly,Nx=Jx+Lx;
 Real W,w,LF[Nlist],Srf,Cnt,Npt;
  for( ;; )
  { switch( ans )
    { case 0xF2: Map.axis();            break; /* (*axis)() */
      case 0xF9: Tv_bar();              break;
      case 0xFa: Tf_List( 0 ); clear(); break;
      case 0xF3:
      { char str[81]=""; w=0;
        color( clr );
        for( k=l=0; k<Nl; k++ )l+=sprintf( str+l," %1.5lg",w+=Lf[k] );
        Thelp( "[%d] <Enter>Перерисовка",Nl ); Tgo( 0,-1 );
        if( Tgets( str,-79 )!=_Enter )break;
        //
        // Подготовка списка изображения для Tf_Topo
        //
        Tf_List( l=x=0 );
        while( str[l] && x<Nlist && sscanf( str+l,"%lg",&w )>0 )
        { Tf_List( LF[x++]=w,_D==Chart?List( w ):clr );
          while( str[l]<=' ' )l++;
          while( str[l]>' ' )l++;
        } if( Nl<=0 )break; // из 0xF3
          Tf_Topo<float>( _D,Jy,Jx,Ly,Lx,true );
      } //
        //   Прорисовка списка площадей по заказу А.А.Поплавского
      { //
       Bfont Save( _Small_font,Tv.mY<500?11:(Tv.mY<1000?15:Tv.mY/66 ) );
       point p={ Map.b.left/2+2,Map.b.top+12 };
        lst=2;
        Cnt=100;
        Npt=0.0;
        for( y=Jy; y<Jy+Ly; y++ )
        for( x=Jx; x<Jx+Lx; x++ )if( Chart[y][x]>0.0 )Npt++; Npt/=100.0;
        qsort( (void*)LF,Nl,sizeof(Real),sort_function );
        l=Save.Th+2;
        for( k=Nl-1; k>=0; k-- )
        {    Srf=0.0;
          for( y=Jy; y<Jy+Ly; y++ )
          for( x=Jx; x<Jx+Lx; x++ )
          { //if( typ==2 )W=((int**)_D)[y][x]; else
            if( Chart[y][x]>0.0 )W=((float**)_D)[y][x]; else continue;
            if( W>LF[k] )Srf++;
          }    color( CYAN  );                     Srf/=Npt;
          Save.Text( South_West,p,_Fmt( "%4.1lf%%",Cnt-Srf) );
               color( DARKGRAY );                  Cnt=Srf; p.y+=l;
          Save.Text( South,p,_Fmt( "%1.0lf",LF[k] ) );
               color( CYAN );                               p.y+=l;
        } Save.Text( South_West,p,_Fmt( "%4.1lf%%",Cnt ) );
      }                                         break;
      case 0xF4: Map.view( _D );                break;
      case 0xF5: Color_Paint( _D,Jy,Jx,Ly,Lx );
                 Tf_Line<float>( _D,Jy,Jx,Ly,Lx,0.0,BLACK ); break;
     default:
      if( strchr( ".+-0123456789",ans )!=NULL )
      { char str[8]; ((int*)str)[0]=ans;
        Tgo( 0,0 ); color( BLUE );
        if( Tgets( str,-6,1 )==_Enter )
        { Real D=atof( str );
           if( _D==Chart )Tf_Line<float>( _D,Jy,Jx,Ly,Lx,D,List( D ) );
                    else  Tf_Line<float>( _D,Jy,Jx,Ly,Lx,D,clr );
    } } }
    if( lst )if( !--lst )Map.axis(); //bar( 0,0,Map.f.left-4,Map.f.bottom );
    if( ans==_Esc )break;
    Thelp( ans==' '?0:
       "___ F1-help F2-axis F3-list F4-auto F5-paint F9-clear F10-delete" );
    do if( ans==0xF1 )ans=Help( Name,Text,Plus ); else ans=Tv_getc();
    while( ans<_Esc );
    Thelp( "" );
  } color( co );
}
void Screen::show( float** D ){ List(); Dp_Show( D,Jy,Jx,Ly,Lx ); }
void Screen::view( float** D )
{     List(); hidevisualpage();           // рисование в скрытом буфере
      Tf_Topo<float>( D,Jy,Jx,Ly,Lx,Chart.Max>100 );
      Tf_Line<float>( D,Jy,Jx,Ly,Lx,0.0,BLACK ); showactivepage();
}
void Screen::window()                        // пропорциональное переустройство
{ int LX,LY;                                 // графического изображения
 Real YX=Ly*Chart.Sy/( Lx*Chart.Sx );        // только на видимый сектор
  if( YX>Real( LY=getmodeheight()+f.Jy+f.Ly ) // батиметрической карты
           / ( LX=getmodewidth()+f.Jx+f.Lx ) )LX=int( LY/YX+0.5 );
                                         else LY=int( LX*YX+0.5 );
   Tv_resize( LX-f.Jx-f.Lx,LY-f.Jy-f.Ly );
   Focus();          // графическая привязка после перестройки видимого сектора
}
//!
//!     Здесь концентрируются консольные запросы к клавиатуре с мышкой,
//!      собранные для единообразного сегментирования исходной карты
//!
inline void Info(){ printf( "\r\t\t\t\t\t\t\t\t\t"); }
static int Info( int x,int y, bool is )//{ return Map.Is( x,y ); }
{ if( !Map.Is( x,y ) ){ Info(); return false; }
  printf( "\r%s <=(%d,%d)",is?"true":"false",x,y );
  if( Map.Jx || Map.Jy || Map.Lx!=Map.Mx || Map.Ly!=Map.My )
  printf( "[%d,%d;%d,%d]",Map.Jx,Map.Jy,Map.Lx,Map.Ly ); Point P=(point){x,y};
  printf( "=> { %d,%d } = ",x=P.x,y=P.y );               Chart.ItD( P );
  printf( "%.1fm%s",Chart[max(0,y%Chart.Ny)][max(0,x%Chart.Nx)],DtoA( P.y,2 ) );
  printf( ",%s ",DtoA( P.x,2 ) ); return true;
}
int Screen::Console( bool hold )              /// необходим еще перехлест
{ int Ans,c=color( GREEN );                      // .. меридианов по глобусу !!
  static bool Segment=false; if( Segment )All.Draw();
   do
   { Focus(); color( LIGHTGRAY );
     if( ( Ans=Window::Console( Info ) )==_MousePressed )
     { if( Segment )                       // если активирована кнопочка на
       { int x,y; Tv_GetCursor( x,y );     // восстановление исходного масштаба
         if( All.Is( x,y ) ){ Jy=Jx=0; Lx=Mx; Ly=My; Ans=-2; }
       }
     } else                              // иначе переустанавливаются новые
     if( Ans==-1 )                      // географические размерения батиметрии
     { Jx=int( Tv_X( _x )-0.5 );                    if( Jx<0 )Jx=0;
       Jy=int( Tv_Y( y  )-0.5 );                    if( Jy<0 )Jy=0;
       Lx=minmax( 16,int( Tv_X( x  )+1.5 )-Jx,Mx ); if( Jx+Lx>Mx )Jx=Mx-Lx;
       Ly=minmax( 12,int( Tv_Y( _y )+1.5 )-Jy,My ); if( Jy+Ly>My )Jy=My-Ly;
       Ans=-2;
     }
   } while( Ans>=_MouseMoved && Ans<=_MouseReleased && hold );
   if( Ans==-2 )
   { window();             // после перестройки новая разметка осей обязательна
     Segment=Jx || Jy || Lx!=Mx || Ly!=My; All.Place( -b.left+8,b.top-4 );
   } color( c ); return Ans;
}
