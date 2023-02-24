//
//      Программа обработки списка мореограмм           /91.02.15/
//
#include "..\Ani\Depth.h"
                                             //
void ViewPointMario( int C )                 // Быстрая рисовка Mario точек
{                                            //
 char *str,View_Mode=byte( (Active&6)>>1 );  // 1-Letter; 2-Number; 3-Comment
  if( View_Mode )                            //
  { int k,c=color( C ); Point Q; point q;
   static char symb=250,Fmt[]="%c"; Fmt[1]=View_Mode<2?'c':'d';
   Bfont S( _Small_font,Tv.mY<500?10:(Tv.mY>1000?20:Tv.mY/50 ) );
    hidevisualpage();
    for( k=0; k<Chart.Nmp; k++ )
    { Chart.DtI( Q=Chart.M[k].G ); q=Q; symb=Chart.M[k].cp[0];
      if( symb==250 || symb=='+' )Tv_cross( Q,-1 ); else
      { str=_Fmt( View_Mode>2?Chart.M[k].cp:Fmt,View_Mode>1?k+1:'A'+k ); color( WHITE );
        setlinestyle( SOLID_LINE,0,THICK_WIDTH ); S.Text( Center,q,str ); color( C );
        setlinestyle( SOLID_LINE,0,NORM_WIDTH );  S.Text( Center,q,str );
    } } showactivepage(); color( c );
} }
static void CROSS( int y, int x )
{ //int c=color( CYAN );
  Tv_cross( (Point){ x,y },12 );
  //color( c );
}                                  //
void Depth::find_coast( MARIO &m ) // здесь ведется поиск
{ const int fn=16;                 // ближайшей береговой точки
  int x,xo,xn,                     // - на воде в квадрате 33x33
      y,yo,yn;
  Point Q; Q=m.G; DtI( Q ); x=xo=xn=int( Q.x+0.5 );
  m.D=m.A=0.0;              y=yo=yn=int( Q.y+0.5 );
  if( unsigned( y )<unsigned( Ny ) )
  if( unsigned( x )<unsigned( Nx ) )
  if( __Depth[y][x]>0  )
  { if( (yo-=fn)<0 )yo=0; if( (yn+=fn)>Ny )yn=Ny;
    if( (xo-=fn)<0 )xo=0; if( (xn+=fn)>Nx )xn=Nx;
    for( y=yo; y<yn; y++ )
    for( x=xo; x<xn; x++ )if( __Depth[y][x]<=0 )
    { Real Ds,Cs;
      Q.x=x;
      Q.y=y; ItD( Q ); Sphere( m.G,Q,Cs,Ds );
      if( m.D==0.0 || m.D>Ds ){ m.D=Ds; m.A=Cs; }
  } }
}
void Depth::move_to_coast( MARIO &m, Real minDepth, int fn ) // ведется поиск
{ int x,xo,xn,                                     // ближайшей береговой точки
      y,yo,yn;                                     // на воде в квадрате fn*2+1
 Point Q; Q=m.G; DtI( Q ); x=xo=xn=int( Q.x+0.5 );
  m.D=m.A=0.0;             y=yo=yn=int( Q.y+0.5 );
  if( unsigned( y )<unsigned( Ny ) )
  if( unsigned( x )<unsigned( Nx ) )
  { int  xf,yf=-1;                      // если -1 - береговая точка не найдена
    Real Ds,Cs,Dist=0.0,
         Dpi=__Depth[y][x];
    if( (yo-=fn)<0 )yo=0; if( (yn+=fn)>Ny )yn=Ny;
    if( (xo-=fn)<0 )xo=0; if( (xn+=fn)>Nx )xn=Nx;
    if( Dpi > minDepth )                    // если точка находится в море,
    { for( y=yo; y<yn; y++ )                // то сначала переносим ее на берег
      for( x=xo; x<xn; x++ )if( __Depth[y][x]<=minDepth )
      { Q.x=x;                                             // поиск ближайшей
        Q.y=y; ItD( Q ); Sphere( m.G,Q,Cs,Ds );            // береговой точки
        if( Dist==0.0 || Dist>Ds ){ Dist=Ds; xf=x; yf=y; } // по дистанции
      }
      if( yf>=0 ){ Q.x=xf; Q.y=yf; ItD( Q ); m.G=Q; m.D=m.A=0; } // - на берегу
    } else xf=x,yf=y;
    if( yf>=0 )                    // если точка снесена или уже была на берегу
    { int Dst=0,Y=yf,X=xf; yf=-1;  // возвращаем ее в ближайшую морскую точку
      for( y=yo; y<yn; y++ )
      for( x=xo; x<xn; x++ )if( (Dpi=__Depth[y][x])>minDepth )
      { int D = max( Y>y?Y-y:y-Y, X>x?X-x:x-X );
        if( !Dst || D<Dst ){ Dst=D, xf=x; yf=y; } else     //
        { if( D==Dst )                                     // поиск ближайшей
          if( Dpi<__Depth[yf][xf] ){ Dst=D, xf=x; yf=y; } //  морской точки
        }                                                // по дальнему индексу
      }                                                  // наименьшей глубины,
      if( yf>=0 )                                        // и снова снос в море
      { Q.x=xf; Q.y=yf; ItD( Q ); m.G=Q; m.D=m.A=0; find_coast( m );
      }
  } }
}
int Depth::mario_utility( int& y, int& x, int Ls )
{                               // Мариографные утилиты
 static int K=-1;               // Текущий пункт наблюдения
        int k=0;                // Ls == 0 изображение всего списка
  if( Ls>=_Enter )              //     +-1 сдвиг на одну точку
  { MARIO *m;                   // == Enter выход на текущую точку
    Real r=0,R;                 // точки '+' или '·' - обрабатываются особо
    for( m=M; k<Nmp; m++,k++ )  //
    { int Y,X; Chart.D2I( m->G,Y,X );
      if( Ls==_Enter ){ R=hypot( Y-y,X-x ); if( !k || R<r ){ K=k; r=R; } } else
      if( Y==y && X==x )break;
    }
    if( k<Nmp )K=k;                                 // запомнить найденное
    if( Ls==_Ins )                                  // Занесение точки в список
    { Point P={ x,y }; static char cpt[24]; memset( cpt,0,sizeof(cpt) ); Ls=0;
      if( k==Nmp )
      { M=(MARIO*)Allocate( ++Nmp*sizeof(MARIO),M ); m=M+k; K=-1; cpt[0]=250;
        ItD( m->G=P );      // ((int*)m->c)[0]=k+'A';
        find_coast( *m );
      } else strncpy( cpt,m->cp,20 );
      color( BLACK );
      CROSS( y,x ); Tgo( 0,0 ); Tgets( cpt,-20 ); Tv_cross( P,0 );
      m->cp=strdup( cpt );
    } else                  //
    if( Ls==_Del )          // Удаление текущей точки
    { if( k<Nmp )           //
      { ViewPointMario( YELLOW ); free( m->cp );
        for( --Nmp; k<Nmp; m++,k++ )m[0]=m[1];
      } Ls=0; K=-1;
  } }
  if( Ls==0 )ViewPointMario(); else
  if( Nmp )
  { if( Ls!=_Enter )K+=Ls;
    if( K>=Nmp )K=Nmp-1;
    if( K<0 )K=0;
    Chart.D2I( M[K].G,y,x );
  } else K=-1; return K;
}
void Depth::sphere( Point P, Point Q, bool ndl )
{ int N;                // N - количество дроблений указателя
 Real Cs,Cz,Ds,Dr=0.0;  // Направление и расстояние
 Point p=P,q=Q;         //
  DtI( p ); if( ndl )Dr=hypot( Sx,Sy )*scale()*( Radian*0.060*Mile ); // Km
  DtI( q ); Ds=hypot( ( p.y-q.y )/Ny,( p.x-q.x )/Nx );
  if( Ds<.05 )N=1; else
  if( Ds<0.2 )N=6; else
  if( Ds<0.6 )N=12; else N=24;
  Sphere( Q,P,Cz,Ds ); setwritemode( XOR_PUT ); Dr += Ds/12;
  Sphere( P,Q,Cs,Ds ); moveto( p );
  while( N>0 )
  { if( N>1 ){ Ds/=N; P=Sphere( P,Cs,Ds ); Sphere( P,Q,Cs,Ds ); } else P=Q;
    DtI( p=P ); lineto( p ); --N;
  }
  if( ndl )
  { DtI( q=Q );
    DtI( p=Sphere( Q,Cz+90,Dr ) ); moveto( p ); lineto( q );
    DtI( q=Sphere( Q,Cz-90,Dr ) ); lineto( q );
  } setwritemode( COPY_PUT );
}
static int WIM;
void static WINDOW(){ color( CYAN ); Twindow( 0,1,16,WIM?5:3 ); }
                                //
void Depth::mario_control()     // Собственно обработчик команд
{                               //
 const char *Name[]={ "MARIO  ","Мореографные точки и посты  ",
                                 "(расстановка и описание); ",
                                 " + корректировка глубин ",0 },
             *Text[]={ " F1  ","Включение окна описания данных",
                       " F2  ","Процедура прорисовки изобат",
                       " F3  ","Выбор формы надписей",0 },
             *Plus[]={ "<BackSpace> ","Коректировка глубины",
                       "<Enter>   ","Переход к ближайшей точке",
                       "< +,- >   ","Перебор точек по списку",
                       "<Insert>  ","Установка нового поста",
                       "<Delete>  ","Снятие точки или привязки",
                       "<Shift+Enter> ","Взаимозамена отметок",
                       "<Sh+стрелки>  ","Отметки между точками",
                       "<0,1..9> ","    Множители шага курсора",0,
                     };
 int ans = '-';
 static int Y=1,X=1,st=1; // Координаты и шаг маркера
 Point P,Q={ X,Y };       // Закрепляемая и просто точка на карте
 int c=color( CYAN ),     //
     K=-1,Kf=false,       // Признак закрепления точки, если KЄ0
     WIND=false;          // Окно информации
     WIM=false;           //  и признак его расширения
  ViewPointMario();       // Изображение всех точек
  do                      //
  { if( ans>=_Enter )
    switch( ans )
    { case _Enter: case _Ins: case '+':
                   case _Del: case '-':
                 K=mario_utility( Y,X,ans=='+'?1:ans=='-'?-1:ans );
                 if( K>=0 ){ Kf=1; Q=P=M[K].G; DtI( Q ); }
                      else { P=Q; ItD( P ); K=Kf=0; } break;
      case 0xF1: if( WIND^=1 )WINDOW(); else Tback(); break;
      case 0xF2: if( WIND )Tback(); Map.show( __Depth );
                 if( WIND )WINDOW(); break;
      case 0xF3: //ans=color( WHITE ); ViewPointMario(); color( ans );
                 ViewPointMario( WHITE );
                 if( (Active&6)==6 )Active&=~6; else Active+=2;
                 ViewPointMario( BLACK ); ans=0; break;
     default:
      if( unsigned( ans-'0' )<10u ){ ans-='0'; st=ans<2?1:st*ans; }else
      if( ans>' ' )ans='H';
    }
    if( WIND || ans==_BkSp )
    { float &D=__Depth[Y][X]; Point q=Q; ItD( q ); sprintf( Str,"%1.5g",D );
      if( !WIND )WINDOW();
      if( Tv_getk()&SHIFT )
      { if( !WIM ){ WIM=TRUE; Tback(); WINDOW(); }
       Real Cs,Ds,Cz;
        if( P==q )Cs=Ds=Cz=0.0; else{ Sphere( q,P,Cz,Ds );
                                      Sphere( P,q,Cs,Ds ); }
        Tgo( 1,4 ); color( CYAN );
        Tprintf( "dst %6.2lfkm\n",Ds );
        Tprintf( "%6.1lf>\xF8<%.1lf\n",Cs,Cz );
      } else if( WIM ){ WIM=FALSE; Tback(); WINDOW(); }
      Tgo( 1,1 );                            color( BLUE );
      Tprintf( "%s[%d,%d]\n",Str,Y,X  );     color( GREEN );
      Tprintf( "F: %8s\n",DtoA( q.y ) );
      Tprintf( "L:%8s\n", DtoA( q.x ) );
      if( ans==_BkSp )
      { color( RED ); CROSS( Y,X ); Tgo( 1,1 );
        if( Tgets( Str,6,0 )==_Enter )sscanf( Str,"%f",&D ); Tv_cross( Q,0 );
      } if( !WIND )Tback();
    }
    if( !ans || ans>=_Enter )
    Thelp( ans==' '?0:"<%d> F1 Inform  F2 Depth  F3 Names %s<Ins Del><+->Выбор"
        " <1..0>Шаг <H>elp",Nmp,WIND?"<BkSp>Глубина ":"<Shift+>Перенос " );
    if( ans=='H' ){ CROSS( Y,X ); ans=Help( Name,Text,Plus ); Tv_cross( Q,0 ); }
    //
    // Обработка команды с информированием о расстоянии между точками
    //
    else if( Tv_getk()&SHIFT && ans<=_Enter )
    { if( K>=0 )
      { Point q=Q; ItD( q );
        color( Kf?LIGHTRED:LIGHTGRAY ); sphere( P,q,2 );
        color( RED );             ans=Tv_pause( &Q,2,st );
        color( Kf?LIGHTRED:LIGHTGRAY ); sphere( P,q,2 );
        if( ans==_Enter )
        { if( Kf )
          { ItD( P=Q=(Point){ X,Y } );
            DtI( Q=M[K].G ); ViewPointMario( LIGHTGRAY );
            M[K].G=P;        ViewPointMario();
          } else { q=Q; DtI( Q=P );
                        ItD( P=q ); } ans=0;
        }
      } if( K<0 ){ K=Kf=0; P=Q; ItD( P ); }
    }
    //    Последовательная обработка команды в контексте дополнительных условий
    else
    { if( (ans=Tv_getw())==_MousePressed && Tv_MouseState&_MouseRightPress )
      if( (ans=Map.Console( false ))==-2 )
      { Map.axis();          // здесь формируется собственно графическое поле
        Map.view( Chart );   // затем выполняется простая прорисовка на готовом
        ViewPointMario();
      } color( LIGHTMAGENTA ); ans=Tv_pause( &Q,12,st );
    }
    Y=int( Q.y+0.5 );
    X=int( Q.x+0.5 );
  } while( ans!=_Esc ); if( WIND )Tback(); Ghelp(); color( c ); ans=0;
}
