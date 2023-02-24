//
//      Dp_Edit.cpp                                   /91.02.09/
//      Комплекс программ для географических преобразований
//              батиметрии и (или) расчетных областей
//
#include "..\Ani\Depth.h"
//
//      Реализуемый набор команд
//      Esc  Выход из программы
//      F1   Информационное окно
//      F2   Мореографные посты и прорисовка изобат
//      F3   Считывание новой батиметрической карты
//      F4   Ручная установка параметров преобразования
//      F5   XOR изображение новой сетки
//      F9   Запись текущей рамки в качестве исходной
//      F10  Восстановление исходных параметров карты
//      Cr   Переключение смещение/сжатие
//    'dmk   Размерность шага и соответственно тип проекции
// =====================
static Depth    Dcur,  // Батиметрия текущего изображения (исходная карта)
                Dsrc;  // Исходная сохраняемая батиметрия
static int SY=1,SX=1,  // SY,SX Знак ориентации оси
           Rclr=WHITE; // Цвет для рамки
static byte W_y=false; // Признак активности информационного окна
                  // 0x0001 Готовность батиметрии
                  // 0x0006 Empty, Letters, Numbers, Identify (4) для Mario
                  // 0x0020 Произведен сдвиг карты в F4 (сброс при считывании)
                  // 0x0018 Был считан новый файл в F4  (сброс с перерисовкой)
                  //    - 8 = 0 - Произведена перерисовка внутри программы F4
                  // 0x0100 Признак замыкания параллелей
                  // 0x1000 Признак активизации численного моделирования
                  // 0x8000 Процесс восстановления сохраненного расчета
static void setWin()   //
          { W_y^=1; if( W_y&1 )Twindow( 0,1,16,6 ); else Tback(); }
static void sWIN(){ if( W_y==1 || W_y==2 ){ W_y^=2; setWin(); } }
                                          //
static void Rect()                        // рамка со сторонами ортодромий
{ const int ks=24;                        // и с изгибом по ks точек на сторону
 Point z={0,0},p,s;                       //
 int c=color( Rclr ); SY=Chart.Ny<0?-1:1;
                      SX=Chart.Nx<0?-1:1; setwritemode( XOR_PUT );
  Chart.ItD( p=z ); Dcur.DtI( p ); moveto( p );
  for( int i=0; i<4; i++ )
  { s=0; if( i%2 )s.y=Chart.Ny/(Real)ks;  // пробежка по периметру
            else  s.x=Chart.Nx/(Real)ks; if( i>1 )s=-s;
    for( int k=0; k<ks; k++ )
    { Chart.ItD( p=(z+=s) ); Dcur.DtI( p ); lineto( p );
  } } Chart.ItD( s=(Point){ Chart.Nx/12,Chart.Ny/6 } );
       Dcur.DtI( s ); needle( p,s ); setwritemode( COPY_PUT ); color( c );
}
static void IP( Point P )
{ point p; Dcur.DtI( P ); p=P; //putpixel( p.x,p.y,LIGHTGRAY );
                               line( p.x-1,p.y,p.x+1,p.y );
                               line( p.x,p.y-1,p.x,p.y+1 );
}
static void Rect( int clr ){ Rclr=clr; Rect(); Rclr=WHITE; }
static void PointView()
{ int y,x,c=color( WHITE ); Rect(); setwritemode( XOR_PUT );
  for( y=0; y<abs( Chart.Ny ); y+=Chart.Ny/48 )
  for( x=0; x<abs( Chart.Nx ); x+=Chart.Nx/64 )
  { Point P={ SX*x,SY*y }; Chart.ItD( P ); IP( P );
  } setwritemode( COPY_PUT ); Rect(); color( c );
}                                       //
static void ShiftMap()                  // Пересчет Chart на исходный Dsrc
{ Map.axis( Dcur,BLACK );               //
  for( int k=0; k<Chart.Nmp; k++ )Dsrc.DtI( Chart.M[k].G );
  Chart.Sy *= Chart.Ny/Real( Dsrc.Ny ); Chart.Ny=Dsrc.Ny;
  Chart.Sx *= Chart.Nx/Real( Dsrc.Nx ); Chart.Nx=Dsrc.Nx;
  Chart.basis( true );
  if( (Active&0x08)==0 )Dcur=Chart;
       Active|=0x20;    Dsrc=Chart;
  for( int k=0; k<Chart.Nmp; k++ )Chart.ItD( Chart.M[k].G ); Map.axis( Dcur );
}
//
//     Смена системы координат с пересчетом на граничные стороны
//
inline Real D85( _Real y ){ return minmax( -1.4835298642,y,1.4835298642 ); }
void Depth::projection( byte Sym )
{ Point X={ Nx-1,0 }; ItD( X ); X/=Radian; bool oMerc=isMercator;
  Point Y={ 0,Ny-1 }; ItD( Y ); Y/=Radian; Ms=Sym|0x20; scale();
  if( oMerc!=isMercator )
  { if( isMercator )                     // переход к меркаторской проекции
    { Y.y=Merkatory( D85( Y.y ) );       // с зацепкой за оси и отнесением
      X.y=Merkatory( D85( X.y ) );       // центра отсчета к началу координат
      Base.y=Merkatory( D85( Base.y ) ); Sy=abs( Y-Base )/(Ny-1);
                                         Sx=abs( X-Base )/(Nx-1); } else
    { Base.y=Geography( Base.y ); Sy=abs( Y-Base )/Ny;
                                  Sx=abs( X-Base )/Nx;
    } basis( true );
} }
//
//   Интерактивные запросы на ручное и автоматическое редактирование
//
void Depth::medit()
{ const char *Name[]={ "Chart ","  позиционирование батиметрии        ",
                                "с помощью описания ее параметров;",
                                "определение характеристик новой карты",0 },
             *Text[]={ " F2  ","Установка рамки по числовым данным",
                       " F5  ","XOR прорисовка новых узловых точек",
                       " F6  ","данные в меню определяют всю карту",
                       " F9  ","смещение карты к новым параметрам",0 },
             *Plus[]={ "     ","  Если команда <Enter> задана в окне:",
                       "\"Project\"   ","выбор: Mercatory или Geography",
                       "\"Step,Size\" ","перерасчет на текущую карту",0,
                     };
 Point  a;                                   // Координаты в градусах и минутах
 Real   dY,dX,Sc;                            //
 int LY,LX,A,ans=0; static char S[]="\xF8";  // "°"
  do{
   Mlist Menu[]
   ={ { 0,10,isMercator?Mercator:Geograph },{ 0,0," Projection" }    // 0,1
    , { 1,14,"Latitude  %3°",&a.y }                                  // 2
    , { 1,14,"Longitude %3°",&a.x }                                  // 3
    , { 1,14," Course   %-1°",&Cs }                                  // 4
    , { 1,14,"Scale Lat %2°", &Fe }                                  // 5
    , { 1,6, "Step  y=%.3lf", &dY },{ 0,6," x=%.3lf",&dX },{ 0,0,S } // 6,7,8
    , { 1,6, "Size Ny=%d",    &LY },{ 0,6," Nx=%d", &LX  }           // 9,10
    };
   Display T( Mlist( Menu ),1,1 );
    Thelp( "F1 Help  F2 New Frame  F5 Nodes  F6 Shift" );
    if( (S[0]=Ms)=='d' )S[0]='\xF8';                                // '°';
    a.y=a.x=0.0; ItD( a ); Sc=scale(); dY=Sy/Sc; LY=Ny;
                                       dX=Sx/Sc; LX=Nx;
    Rect(); Fe*=Radian; color( LIGHTGRAY );
    do
    { A=T.Answer( ans );
      if( A<' ' )ans=A; else
      if( A==0xF1 )Help( Name,Text,Plus );
      T.Back();
    } while( A==0xF1 ); Fe/=Radian; Rect(); dY*=Sc; dX*=Sc;
    switch( ans )
    { case 6: case 7:  LY = int( (Ny-1)*Sy/dY+1 );
                       LX = int( (Nx-1)*Sx/dX+1 ); break;
      case 9: case 10: dY = (dY*(Ny-1))/(LY-1);
                       dX = (dX*(Nx-1))/(LX-1);    break;
    }
    Ny=LY; Sy=dY; Base.y=a.y/Radian;
    Nx=LX; Sx=dX; Base.x=a.x/Radian; basis( false );
    switch( A )
    { case 0: projection( isMercator ? '\'':'m' ); break;
      case 0xF9: ShiftMap();                       break;
      case 0xF5: PointView();
    }
  } while( ans!=_Esc );
}
//        Собственно интерактивное и автоматическое (Au=true)
//          редактирование батиметрических массивов
//
bool Depth::edits( bool Au ) // на выходе признак реального редактирования
{ const
  char *Name[]={ "Chart ","    Редактирование характеристик      ",
                          "батиметрической карты, переопределение",
                          "    ее проекции с последующей",
                          "  переинтерполяцией исходных данных",0 },
       *Text[]={ " F1  ","Вызов/гашение информационного окна",
                 " F2  ","Мореографные посты и прорисовка изобат",
                 " F3  ","Считывание новой батиметрической карты",
                 " F4  ","Вызов списка с числовым описанием карты",
                 " F5  ","XOR прорисовка новых узловых точек",
                 " F6  ","Наложение считанной карты на исходную",
                 " F9  ","Восстановление характеристик батиметрии",
                 " F10 ","Установка характеристик целевой карты",0 },
       *Plus[]={ " <Enter> ","ключ подвижки и изменения размеров",
                 " <Shift+стрелки>   ","изменение режима <Enter>",
                 " <0,1..9>   ","умножители скорости перемещений",
                 " <R>otate  ","поворот рамки: R-вправо; r-влево",
                 " <D>egree <'>min & <K>ilo<M>etre  ","тип сетки",
                 " ","    где D,' для географии; K,M - Меркатора",
                 " Shift   ", "новая рамка задает текущую сетку",
                 " Turn    ", "вертикальный переворот данных",
                 " <Esc>  ",  "выход с подтверждением корректности",
                 " - <Enter>  ","подтверждение интерполяции карты",
                 " - <BkSp> ","выход с отказом от редактирования",
                 " - <Esc> ", "продолжение с отметкой новой рамки",0,
               };
 if( !(Active&1) )return false;
 if( Au && isMercator ){ Ms='m'; return false; }
 int  ans=0xF1,St=1;
 bool SIZE=true,L=false;
 Point P,A,B; A.x=A.y=0,B.x=Nx,B.y=Ny;
       W_y=false;  //
       Dcur=       // Батиметрия текущего изображения
       Dsrc=Chart; // Исходная сохраняемая батиметрия
  for( ;; )        //
  { if( ans>=_Enter )
    Thelp( ans==' '?0:"F1 Info  F2 Mario  F3 Read  F4 Edit"
                   "  F5 Nodes  F6 Update F10 UnDo <'dmk>Step <R>otate" );
    Rect();
    if( Au )ans='m'; else
    if( ans!='H' )ans=Tv_getc(); else ans=Help( Name,Text,Plus ); Rect();
    if( ans<_Enter )
    { L= Tv_getk()&SHIFT ? !SIZE:SIZE;
      if( ans&North ){ if( !L )A.y+=St; else B.y+=St; }
      if( ans&South ){ if( !L )A.y-=St; else B.y-=St; }
      if( ans&East  ){ if( !L )A.x+=St; else B.x+=St; }
      if( ans&West  ){ if( !L )A.x-=St; else B.x-=St; } L=true;
    } else switch( ans )
    { case 'R': ans=0; L=true; Cs++; break;
      case 'r': ans=0; L=true; Cs--; break;
      case 'm': case 'M': case 'k': case 'K':
      case 'D': case 'd': case '\'' : projection( byte( ans ) ); break;
      case 'S': case 's':
        if( (Tv_getc()|0x20)=='h' )
        if( (Tv_getc()|0x20)=='i' )
        if( (Tv_getc()|0x20)=='f' )
        if( (Tv_getc()|0x20)=='t' ){ sWIN(); ShiftMap();
                                     sWIN(); B.y=Ny,B.x=Nx; } break;
      case 'T': case 't':
        if( (Tv_getc()|0x20)=='u' )
        if( (Tv_getc()|0x20)=='r' )
        if( (Tv_getc()|0x20)=='n' )
        { float *Lin=(float*)Allocate( Nx*sizeof( float ) );
          for( int i=0; i<Ny/2; i++ )
          { memcpy( Lin,Dcur[i],         unsigned( Nx )*sizeof(float) );
            memcpy( Dcur[i],Dcur[Ny-i-1],unsigned( Nx )*sizeof(float) );
            memcpy( Dcur[Ny-i-1],Lin,    unsigned( Nx )*sizeof(float) );
          } Allocate( 0,Lin ); clear(); Map.axis( *this );
                                        Map.view(__Depth ); ViewPointMario();
        }  break;
      case _Enter: SIZE=!SIZE;
      case _Esc : break;
      case 0xF1: setWin(); break;
      case 0xF2:{ Depth Wd; Rect(); Wd=Chart; Chart=Dcur; Chart.mario_control();
                  Chart=Wd; Rect(); } break;
      case 0xF4: sWIN(); medit(); B.y=Ny;
                 sWIN(); L=true;  B.x=Nx; break;
      case 0xF5: sWIN(); PointView();
                 sWIN(); break;
      case 0xF6: if( Active&0x10 )
                 { Chart=Dcur;
                 #pragma omp parallel for
                   for( int y=0; y<Ny; y++ )   // Наложение новой считанной
                   { for( int x=0; x<Nx; x++ ) //          карты на текущую
                     { Point P={ SX*x,SY*y }; ItD( P ); Dsrc.DtI( P );
                       if( P.x>=0.0 && P.x<Dsrc.Nx
                        && P.y>=0.0 && P.y<Dsrc.Ny )
                       { float &ds=Dcur[y][x],d=Dsrc( P );
                         if( d>0 || ( !d && ds>0 ) )ds=d;
                       }
                     } Tv_isKey();            // исполнение прерываний Windows
                   } A.x=A.y=0; B.x=Nx,B.y=Ny; basis( true );
                   Dsrc.allocate( 0,0 );
                   Dsrc=Dcur; Active&=~0x18; clear(); Map.axis( *this );
                                                      Map.view(__Depth );
                 } break;
      case 0xF9: { float **Dc=__Depth;   Chart=Dcur; A=0;
                              __Depth=Dc;            B.y=Ny; B.x=Nx;
                 } break;
      case 0xF3: strcpy( sname( InfName ),"*.inf" ); //
                 if( Tfile( InfName )>0 )            // Выбор имени файла
                 { if( !(Active&0x10) )__Depth=NULL; // Блокировка
                   Active|=0x18;                     //   старого массива
                   Dsrc=read( InfName ); Rect();     // Новый файл
                   Active&=~0x20; Rect( LIGHTCYAN ); //   без сдвига
                 }                                   //
      case 0xFa: Chart=Dsrc; A=0; B.y=Ny; B.x=Nx; L=true;
                 if( (Active&0x1000) && ans==0xF3 && Ms!='m' )ans='m'; break;
      default  : if( unsigned( ans-'0' )<10u )St=ans<'2'?1:St*(ans-'0');
            else if( ans>' ' )ans='H';
    } ItD( A );
    if( L )
    { Base.y=A.y/Radian; Ny=int( B.y+0.5*SY );
      Base.x=A.x/Radian; Nx=int( B.x+0.5*SX );
      if( abs( Ny )<2 ){ SY=-SY; B.y=Ny=2*SY; } L=false;
      if( abs( Nx )<2 ){ SX=-SX; B.x=Nx=2*SX; } basis();
    }
    if( W_y )
    { Real Sc=scale(); Tgo(1,1); color( BLUE );
      Tprintf( isMercator?Mercator:Geograph ); Tln();
      Tprintf( " F %8s\n",        DtoA( A.y ) );
      Tprintf( " L%8s\n",         DtoA( A.x ) );
      Tprintf( "Cs%8s\n",         DtoA( Cs  ) );
      Tprintf( "dY%7.1lf%c+%3d\n",Sy/Sc,Ms,Ny );
      Tprintf( "dX%7.1lf%c+%3d\n",Sx/Sc,Ms,Nx );
    }   A=0.0;
    if( Active&as_Simula )projection( 'm' ); // если волна,то Меркатор в метрах
    if( ans==_Esc )
    { int a,b = (Active&0x08)!=0;
      Tv_bell();
      Thelp( "?<Enter>Exit  <BkSp>Quit %s "
               "<Tab>Marking only",b?" <Space>ReDraw ":"" );
      Rect(); a=Tv_getc(); Ghelp();
      Rect( a==_Tab?DARKGRAY:WHITE );
      if( b && a==' ' )
      { Depth Dd; if( W_y )setWin();
        Dd=Chart; Chart=Dsrc; clear(); Map.axis( *this ); Map.view(__Depth );
        Active&=~0x08;
                  Chart=Dcur; Dcur=Dsrc; Rect( LIGHTGRAY ); Chart=Dcur;
      } else
      { if( a==_Enter )ans='y',a=_BkSp;
        if( a==_BkSp )break;
      }                           //
    } if( Au ){ ans='y'; break; } // выход из автоматической смены проекции
  }                               //
  if( W_y )setWin(); St=0;        // Признак форматирования
  if( ans!='y' )Chart=Dsrc;       // Запрет ручной корректировки
//
//      Интерполяционный блок
//
  if( ans=='y' )                // Подтверждение интерполяции
  { Rect();                     //
    Ny=abs( Ny );
    Nx=abs( Nx ); __Depth=NULL; allocate( Ny,Nx );
    color( GREEN );
    Min=Max=0.0;
    Type=2;
    St = 6*(Nx+Ny)/(Tv.mX+Tv.mY); if( !St )St=1;
#pragma omp parallel for private( A,P )
    for( int y=0; y<Ny; y++ )
    { for( int x=0; x<Nx; x++ )
      { float &Dat=__Depth[y][x];
        P.y=SY*y;
        P.x=SX*x; ItD( P ); A=P; /*IP( P );*/
        if( !(y%St) )
        if( !(x%St) )
        { point p; Point Q=P; Dcur.DtI( Q ); p=Q; putpixel( p.x,p.y,LIGHTGRAY );
        }
        Dsrc.DtI( P );
        if( P.x<0.0 || P.x>Dsrc.Nx
         || P.y<0.0 || P.y>Dsrc.Ny ){ Dcur.DtI( A ); Dat=Dcur( A ); }
                              else                   Dat=Dsrc( P );
        if( Max<Dat )Max=Dat; else if( Min>Dat )Min=Dat;
      } Tv_isKey();                           // исполнение прерываний Windows
    }   basis( true );                        //
        St=1;        Dsrc.allocate( 0,0 );    // Закрытие текущей батиметрии
  } if( Active&0x10 )Dcur.allocate( 0,0 );    // Сброс исходного массива
//                                            //
//      Перебор мореографных точек
//
  if( !(Active&as_Simula) )                   // если не активен режим
  if( ans=='y' || (Active&0x10) )             // численного моделирования
  { int k,n,x,y;                              // то, ограничивается список
    for( n=k=0; k<Nmp; k++ )                  // мореографных постов наблюдения
    { MARIO &Ml=M[k]; Chart.D2I( Ml.G,y,x );  //
      if( y>=0 && y<Ny
       && x>=0 && x<Nx ){ M[n]=Ml; n++; }
    } M=(MARIO*)Allocate( (Nmp=n)*sizeof(MARIO),M );
    //
    // Поиск ближайших береговых точек для всех контрольных постов
    //
    for( k=0; k<Nmp; k++ )find_coast( M[k] );
  }
  if( ans=='y' )
    { Map.Ly=Map.My=Ny; Map.Jy=Map.Jx=0;           //
      Map.Lx=Map.Mx=Nx; Map.window(); }            //! пересоздание нового окна
  if( St || (Active&0x10) )                        //  с последующей
    { clear(); Map.axis( *this );                  //  полной перерисовкой
               Map.view(__Depth ); Active&=~0x10;  //
    } ViewPointMario();                            // - включая мореографы
  return ans=='y';
}
//
//     Простая билинейная интерполяция на равномерной сетке /88.01.07-91.01.23/
#if 0
static float __value( float *V, int N, float Ar )
{                        //  Интерполяция по Лагранжу на равномерном интервале
 int j,k;                //            V  - вектор значений функции
 float yo=0,p;           //            N  - текушая длина вектора V
  if( N==1 )return V[0]; //            Ar - интерполяционный аргумент
  for( j=0; j<N; j++ )   //            Pw - степень интерполяции
    { for( p=1.0,k=0; k<N; k++ )if( j!=k )p*=(Ar-k)/(j-k); yo+=V[j]*p;
    } return yo;
}                                      //
float Depth::value( Point P )          // здесь необходимы Эрмитовы полиномы,
{                                      //   не отпускающие градиенты вразнос
 int y=int( P.y-1.0 ),ny=4,            // Расчет индекса ближайшей точки
     x=int( P.x-1.0 ),nx=4,j;          //
 float _F[4];
  if( y<-2 || x<-2 || y>Ny || x>Nx )return 0.0;
  if( y<0 )y=0; else if( y>=Ny-4 ){ y=Ny-4; if( y<0 ){ y=0; ny=Ny; } }
  if( x<0 )x=0; else if( x>=Nx-4 ){ x=Nx-4; if( x<0 ){ x=0; nx=Nx; } }
  if( !__Depth[y+1][x+1]
   && !__Depth[y+1][x+2]
   && !__Depth[y+2][x+2]
   && !__Depth[y+2][x+1] )return 0.0;
  P.x-=x;
  P.y-=y;
  for( j=0; j<ny; j++ )_F[j]=__value( __Depth[y+j]+x,nx,P.x );
                      return __value( _F,ny,P.y );
}
#else
//float Depth::value( Point P )
//{                        //
// int y=P.y,             // Расчет индекса ближайшей точки
//     x=P.x;            //
// float **_F;
// double dy,dx,dy1,dx1;
//  if( y<-1 || x<-1 || y>Ny || x>Nx )return 0.0;
//  if( y<0 )y=0; else if( y>=Ny-1 )y=Ny-2;
//  if( x<0 )x=0; else if( x>=Nx-1 )x=Nx-2;
//  dy=P.y-y,dy1=1-dy,       //
//  dx=P.x-x,dx1=1-dx;       // Расчет разностей аргумента
// _F =__Depth+y;            //
// return ( dx1*( _F[0][x] ) + dx*( _F[0][x+1] ) )*dy1
//      + ( dx1*( _F[1][x] ) + dx*( _F[1][x+1] ) )*dy;
//}
#endif
//            и совсем устаревший интерполяционный блок,
//              ранее работавший при ограниченной оперативной памяти
#if 0
 int y,x;
 float d;                  //
  if( ans=='y'             // Подтверждение интерполяции
   || (Active&0x0010)!=0 ) //   и перечитанной батиметрии
  {                        //
   static char Temp[]="~Temp",Inf[]="inf",Flw[]="flw";
   unsigned long FreeMem = farcoreleft(),
                 NeedMem = long( Ny )*( sizeof( float* )
                           + long( Nx )*sizeof( float ) )+4096;
    Rect();      ShowMemory();
    Ny=abs( Ny );
    Nx=abs( Nx ); ans='y';
    __Depth=NULL;
    if( NeedMem<=FreeMem )allocate( Ny,Nx ); else open( Temp,Flw,"wb" );
    Thelp( "Bathimetry[%dx%d]",Ny,Nx );
    setwritemode( XOR_PUT );
    color( GREEN );
    Min=Max=0.0;
    Typ=2;
    for( y=0; y<Ny; y++ )
    for( x=0; x<Nx; x++ )
    { P.y=SY*y; P.x=SX*x; ItD( P ); IP( P );
      Di.DtI( P ); d=Di.value( P );
      if( __Depth )__Depth[y][x]=d; else fwrite( &d,sizeof( float ),1,Fs );
      if( Max<d )Max=d; else
      if( Min>d )Min=d;         //
    } Di.allocate( 0,0 );       // Закрытие старой батиметрии
    if( Fs )                    //
    { open( Temp,Inf,"wt" ); infout( Temp );
      open( Temp,Flw,"rb" ); allocate( Ny,Nx );
      for( y=0; y<Ny; y++ )fread( __Depth[y],sizeof( float ),Nx,Fs );
      close();
    } setwritemode( COPY_PUT ); basis( TRUE ); St=1;
  }
#endif
