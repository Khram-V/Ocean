//
//      Long_Wave_Initial_Surfaces.cpp                               /90.11.08/
//      .---------------------------------------------------------------.
//      ¦                                                               ¦
//      ¦  Комплекс процедур для интерактивного построения начальных    ¦
//      ¦     условий и соответственного заполнения расчетных полей     ¦
//      ¦  Реализуется задание начальных условий в виде геометрического ¦
//      ¦      очага произвольной формы или путем определения           ¦
//      ¦      периода входящей волны, а также в виде частотного        ¦
//      ¦      а-ля белого шума                                                         ¦
//      +---------------------------------------------------------------+
//
#include <String.h>
#include "..\Ani\LongWave.h"

#define MSG( i ) Menu[i].Msg
                               //
static bool LorP=false;        // На первом входе - пространственный размер,
                               //     по "истине" - фиксируется период
static short WTYPE=3;          //   0 - синусоидальный пакет
                               //   1 - простой эллиптический очаг
                               //   2 - аля-"овоидный" подъем дна Поплавского
                               //   3 - новый очаг на склоне морского дна
static Point Grad={ 0,0 };     // относительный градиент наклона морского дна
static Real Mdp=-20.0,         // Mag=8.0 - магнитуда и глубина землетрясений
            Ppl=40.0;          // -- новое чудо - "в перьях" от А.Поплавского
static void Ellipse( Basis& W, Real Va )
{ const Real da=Pi/24.0;       //  шаг угловых смещений по единичному кругу
 static int c,first=true;      //   пересохранение текущего цвета через стэк
  if( first )c=color( Va<-0.001? LIGHTGREEN: Va>0.001? LIGHTCYAN:DARKGRAY );
       else  c=color( c );
       moveto( W.Goext( (Point){-1.02,0 } ) ); setwritemode( XOR_PUT );
       lineto( W.Goext( (Point){-0.04,0 } ) );
  for( int k=0; k<=48; k++)lineto( W.Goext( (Point){ sin(da*k),cos(da*k)} ) );
       lineto( W.Goext( (Point){ 0.04,0 } ) );
       lineto( W.Goext( (Point){ 1.02,0 } ) ); setwritemode( COPY_PUT );
  if( first )c=color( c ); else color( c ); first=!first;
}
void TWave::Length_or_Period( bool Ang )     // установка тензорного базиса для
{ Point G,P={ Vo.x/Dx,Vo.y/Dy };
    int y=minmax( 0,(int)P.y,My-1 ), // построения
        x=minmax( 0,(int)P.x,Mx-1 ); // очага цунами
   G=Bottom_Gradient( P );
   if( !Ang )
   if( abs( G )!=0.0 )Vc=Chart.Cs-arg( G )*Radian-90.0;
   Chart.ItD( P );
  Real Dep=max( DZ,Dp[y][x] ),
       Vel=sqrt( 9.8106*Dep )*cos( Chart.Fe )/cos( P.y/Radian );
  if( LorP )Vs.y=Vt*Vel;
       else Vt=Vs.y/Vel; Ve.Euler( Vo,(Vc=Angle( Vc ))-Chart.Cs,Vs/2.0 );
  if( !Ang )                                          // уклон дна приводится к
  { P=Ve.Goext( (Point){ 0,1 } ); P.x/=Dx; P.y/=Dy;   // долям глубины от очага
    Glide = max( 0.0,1.0-max( 0.0,Chart( P ) )/Dep ); // в сторону мелководья
} }
int TWave::Picture()
{ int ans=0xF1; bool WIND=false;
  for( ;; )
  { switch( ans )
    { case 'R' :                 Vc++;              break;
      case 'r' :                 Vc--;              break;
      case 'X' :                          Vs.x+=Dx; break;
      case 'x' :             if( Vs.x>Dx )Vs.x-=Dx; break;
      case 'y' : LorP=false; if( Vs.y>Dy )Vs.y-=Dy; break;
      case 'Y' : LorP=false;              Vs.y+=Dy; break;
      case _Enter: LorP^=1;                         break;
      case _Esc: ans=0;
      case 0xF5:
      case 0xF6: goto _Ret;
      case 0xF7: ans=Seismic_Source(); break;
      case 0xF1: if( !(WIND^=true) )Tback(),Ghelp(); else
                   color( LIGHTGRAY ),Twindow( 1,1,25,7 );
     default: ans=_Enter;
    }
    Length_or_Period( ans>=' ' );              // перестройка параметров очага
    if( WIND )
    { Point P={ Vo.x/Dx,Vo.y/Dy }; color( BLUE );
      int y=minmax( 0,int( P.y+0.45 ),My-1 ),
          x=minmax( 0,int( P.x+0.45 ),Mx-1 ); Chart.ItD( P ); Tgo( 1,1 );
      Tprintf( W2D( " Параметры очага цунами \n" ) ); color( CYAN );
      Tprintf( W2D( " Высота волны : %.2lf\n" ),Va );
      Tprintf( W2D( " Глубина:уклон %.0f:%.2lf\n" ),Chart[y][x],Glide );
      Tprintf( W2D( " Размер %1.0lf х %-1.0lfкм\n"),Vs.y/1e3,Vs.x/1e3 );
      Tprintf( W2D( " Период волны%s:%.0lf°\n" ),DtoH( Vt/3600,2 ),Vc );
      Tprintf( W2D( " Место %s" ),               DtoA( P.y,2 ) );
      Tprintf( W2D( ",%-s\n" ),                  DtoA(P.x,2));
      Tprintf( W2D( " ...в узлах %1.1lf:%1.1lf\n" ),Vo.y/Dy,Vo.x/Dx  );
    }
    if( (ans==0 || ans>=_Enter) && strchr( "rxy",ans|0x20 )==0 )
    Thelp( ans==' '?0:"F1 Справка  F5 Добавить  F6 Новый  F7 Очаг <Enter>"
       "задан%s волны <Y|X|R>Размер и ориентация",LorP?" период":"а длина" );
    Ellipse( Ve,Va );                                      //
    do{ if( ans>0 && ans<3 )ans=0xF7; else                 // выход из менюшки
        if( ans<=0xF4 )ans=max( _Tab,Tv_pause( &Vo,12 ) ); // или по ее ключам
      } while( ans>=_MouseMoved && ans<=_MouseReleased );  //
    Ellipse( Ve,Va );
  } _Ret: if( WIND )Tback(); Ghelp(); return ans;
}                                       //
Point TWave::Bottom_Gradient( Point G ) // Расчет градиента по квадрату ms x ms
{ const int ms=5;                       //   - координаты в номерах узлов
  int x=minmax(0,int(G.x+0.5),Mx-1),xs=max(0,x-ms),xe=min(x+ms,Mx-1),
      y=minmax(0,int(G.y+0.5),My-1),ys=max(0,y-ms),ye=min(y+ms,My-1); G=0.0;
  if( xs<xe && ys<ye )                        //       ,-------.
  { for( y=ys; y<=ye; y++ )                   //       ¦ -x y+ ¦
    for( x=xs; x<=xe; x++ )                   //       ¦   X   ¦
    { if( y<ye )G.y += Dp[y+1][x]-Dp[y][x];   //       ¦ -y x+ ¦
      if( x<xe )G.x += Dp[y][x+1]-Dp[y][x];   //       '-------'
    }           G.x /= Dx*(xe-xs)*(ye-ys+1);  // Здесь используется коэффициент
                G.y /= Dy*(xe-xs+1)*(ye-ys);  //   пересчета длины и глубины
  } return G;                                 //
}
//
//     По магнитуде землетрясения вычисляются размеры очага и высота цунами
//
inline void
Macroseismic( Real Mag, Point &Vs, Real &Va )  // < магнитуда > размер + высота
{ LorP=false;                                  // пространственный размер очага
  Vs.y = 2000.0*pow( 10.0,Mag/3.0-1.0 );       // Tsunami Energy [Joules]
  Vs.x = Vs.y+1000.0*pow( 10.0,0.5*Mag-1.8 );  //   = pow( 10,2*Mag-1.5 )
  if( Mdp>0.0 )  //! здесь, теперь включается неказистое обновление Поплавского
  { const Real A=8.663,B=0.968,// b=4.356,c=0.5,d=1.8,Jmax=11,p=4.5,q=4.5,m=1.5
          Length=pow( 10.0,0.5*Mag-1.8 );   // длина сейсмического разлома [km]
    Ppl = pow( 10.0,Mag/3.0 - 1.4444 );     // ширина очага [км]
    if( Mdp>Ppl )                   // с пересчетом размерений, если полуширина
    { Ppl=Mdp;                      //  очага вдруг окажется меньше его глубины
      Vs.y = Ppl*2000.0;            //  и с приделыванием закруглений на концах
      Vs.x = Vs.y + Length*1000.0;  //      хребта/оврага сейсмического разлома
    } Va=sqrt( pow( 10.0,B*(1.5*Mag-4.5*log10( Ppl )+4.5 )-A ) );
  } else Va=2.0*sqrt( pow( 10.0,2.0*Mag-1.5)/((Pi*1025.0*9.8106)*Vs.x*Vs.y ) );
}
int TWave::Seismic_Source()    /// Варианты пространственных очагов цунами
{ if( Mag<0.0 )Mag=-Mag;       //! - в расчеты вводится магнитуда землетрясения
  for( int Ans=-1; ; )
  { Macroseismic( Mag,Vs,Va );  // Macroseismic Calculations for Tsunami Source
    Length_or_Period( false );  // установка тензорного базиса для очага цунами
    if( Ans!=-1 )return Ans;    // безусловный выход после уточнения параметров
    Mlist Menu[]=               // и это меню создается перед самым его вызовом
      { { 0,1,"\t Макросейсмический очаг  " }                           // 0
      , { 1,5,"\t    Магнитуда : %4.2lf",&Mag }                         // 1
      , { 1,5,"\t     под дном : %3.1lf",&Mdp },{ 0,0,"\t км" } };      // 2,3
    if( Mdp<=0 ){ MSG(2)="\t    (без глубины) "; Menu[2].dat=NULL; MSG(3)=0; }
    Thelp( "Уклон дна: %0.2lf\\%0.1lf - очаг строится в главном меню",Glide,Va );
    Ellipse( Ve,Va );  Ans=Tmenu( Mlist( Menu ),23,4 );
    Ellipse( Ve,Va );  Mag=minmax( 6.0,Mag,9.6 );         // минимум 6,0 до 9,6
    if( !Mdp )Mdp=-20; else if( Ans==2 )Mdp=-Mdp; if( Ans==' ' )Ans=2;
  }
}
const char* DownCount( Real T,Real cT ); // это отметки из меню времени для приливов
                      //
int TWave::Source()   //  Задание начальных условий в форме очага землетрясения
{                     //      или пространственного волнового пакета
 const char *Name[]={ "Source ","блок процедур определения источника",
                                " цунами или геометрического пакета",
                                "  волн (волн глубокой воды) через",
                                "   начальное возмущение свободной",
                                "    поверхности с помощью",
                                "     геометрических построений",0 },
            *Text[]={ "F2  ","вызов установки режимов моделирования",
                      "F3  ","переход к установке граничных условий",
                      "F4  ","визуальное позиционирование источника",
                      "F5  ","добавление нового источника возмущения",
                      "F6  ","расчистка поля и ввод нового источника",
                      "F7  ","задание эмпирического источника цунами",
                      "F10 ","обнуление всего расчетного поля",0 },
            *Plus[]={ "","    Если команда <Enter> задана в окне:",
                      "Период-Длина ","фиксация периода или размера",
                      "Синусоидальные группы ","входящие пакеты волн",
                      "  числом из - ","количество полуволн в пакете",
                      "Эллиптический очаг ","модулированный источник",
                      "  модуляция - ","количество наложенных волн",
                      "Статический очаг / бегущая волна - "," метод",
                      " ","   инициализации начального поля скорости",0, };
 int ans=1; Point Eg; Real cT=V0;
 Field F=(Field){ Map.Jx*Dx,Map.Jy*Dy,                     //
                  Map.Lx*Dx,Map.Ly*Dy }; Tv_place( 0,&F ); //! Метровая сетка !
  Vs*=2; Length_or_Period( false );                        //
  if( !Va )V0=Tlaps;
  for( ;; )
  { Mlist Menu[]=
    { { 0,13,"\tПериод %3°",&Vt }
    , { 1,8, "\tАмплитуда %5.2lf",&Va },{ 0,0,"\t м" }                 // 1
    , { 1,8, "\tНаправление %-1°",&Vc }                                // 3
    , { 1,11,"\tШирота   %-2°",&Eg.y }                                 // 4
    , { 1,12,"\tДолгота %-2°", &Eg.x }                                 // 5
    , { 1,6, "\tОчаг %6.1lf",&Vs.y },{ 0,6,"\xF9%-5.1lf",&Vs.x },{ 0,0,"\tкм" }
    , { 1,5, "\tМесто %5.1lf",&Vo.y },{ 0,6,":%-4.1lf",&Vo.x },{ 0,0,"\tуз" }
    , { 1,3,0,&Vk },{ 0,0,"\t волн" },{ 1,20 },{ 1,6 },{ 1,20 }        // 12-16
    };
   const int NList=sizeof( Menu )/sizeof( Mlist ); bool REGIME=true;
   Display Tm( Menu,NList,1,1 );
    MSG( 12 )=(char*)(!WTYPE?"\tЧислом из %2.0lf" :"\tМодуляция %2.0lf" );
    MSG( 14 )=(char*)(!WTYPE?"\t Синусоидные группы" :
                   (WTYPE==1?"\t Эллиптический очаг" :
                   (WTYPE==2?"\t Разлом Поплавского" :
                             "\t Цунами у побережья" ) ) );
    if( WTYPE==3 )
    { Menu[15]=(Mlist){ 1,6,"\t   уклон дна: %0.3lf",&Glide }; WaveAs&=~2;
    } else
    { Menu[15]=(Mlist){1,20,(WaveAs&2?"\t  Бегущая волна  ":"\t  Статический очаг" ),0};
    }
    if( WTYPE || !(WaveAs&2) ){ Tm.Nm=NList-1; MSG(16)=0; } else
                              { Tm.Nm=NList;   MSG(16)=DownCount( Tlaps,V0 ); }
    Ellipse( Ve,Va );
    Thelp( "F2 Настройка  F3 Период %sF7 Очаг  F9 Запуск  F0 Очистка",
        Va?"F4 Положение  F5 Добавить  F6 Новый ":"" );
    Vt/=3600.0; Vo.y/=Dy;
    Vs/=1000.0; Vo.x/=Dx; Eg=Vo; Chart.ItD( Eg );
    do
    { color( CYAN );  ans=Tm.Answer( ans );
      if( ans==0xF1 )Help( Name,Text,Plus ); else
      if( ans>0xF1 && ans!=0xF7 )Tm.Back();
    } while( ans==0xF1 );
    if( ans==9 || ans==10 ){ Eg=Vo; Chart.ItD( Eg ); }
                      else { Vo=Eg; Chart.DtI( Vo ); }
    Vt*=3600.0; Vo.y*=Dy;
    Vs*=1000.0; Vo.x*=Dx; Ellipse( Ve,Va ); Length_or_Period( true );
  Execution:
    switch( ans )
    { case    0: LorP=true;  break;
      case    6:
      case    7: LorP=false; break;
      case   14: if( ++WTYPE>3 )WTYPE=0; goto Tcnt;  // рядышком, через строчку
      case   15: WaveAs^=2;
           Tcnt: if( !WTYPE )
                 { if( WaveAs&2 ){ Menu[16].skip=11;
                                   Menu[16].lf=20; }
                 } Tm.Back(); break;
      case   16: if( V0==Tlaps )V0=0; else
                 if( V0==0 && cT!=0 )V0=cT; else V0=Tlaps; break;
      case 0xF4: if( Va )
                 if( (ans=Picture())!=0 )goto Execution; break;
      case 0xF5: REGIME=false;                      // с повтора или нет,
      case 0xF6: if( Va )                           // здесь строится совсем,
      { Real ey = cos( (Vc-Chart.Cs)/Radian ),      // или не очень, но новый
             ex = sin( (Vc-Chart.Cs)/Radian ),D;    // сейсмический очаг цунами
        Point SaveVs=Vs,
              SaveVo=Vo; LorP=false;
        if( WTYPE<2 || (WTYPE==3 && Glide==0.0 ) )  // == групповые структуры
        {                                           // или эллиптический очаг
        #pragma omp parallel for private( D )
          for( int y=0; y<My; y++ )
          for( int x=0; x<Mx; x++ )if( (D=Dp[y][x])>DZ )
          { Real d,w,h = Wave( WTYPE==0,Dy*y,Dx*x );
            if( REGIME )Hg[y][x]=h; else Hg[y][x]+=h;
            if( WaveAs&2 )                        // нулевая сторона Потоков
            { if( y>0 )if( (d=Dp[y-1][x])>DZ )    //
              { w=ey*Wave(WTYPE==0,Dy*(Real(y)-0.5),Dx*x)*sqrt(9.8106*(D+d)/2);
                if( REGIME )Uy[y][x]=w; else Uy[y][x]+=w; }
              if( x>0 )if( (d=Dp[y][x-1])>DZ )
              { w=ex*Wave(WTYPE==0,Dy*y,Dx*(Real(x)-0.5))*sqrt(9.8106*(D+d)/2);
                if( REGIME )Ux[y][x]=w; else Ux[y][x]+=w; }
          } }
        } else
        if( WTYPE==2 ) // Последний вариант очага цунами Сан Саныча Поплавского
        { Point SaveVs=Vs;
          Real Length=( Vs.x-Ppl*2.0 )/1000.0;  // кривовастая переустановка
          Vs.x=Vs.y=500.0;                      // основного тензорного базиса
          Length_or_Period( true );
        #pragma omp parallel for private( D )
          for( int y=0; y<My; y++ )
          for( int x=0; x<Mx; x++ )if( (D=Dp[y][x])>DZ )
          { Real d,h,w; Point P=Ve.Goint( (Point){ x*Dx,y*Dy } );
            if( fabs( P.x )<Length )w=P.y/2.0; else      // формулы Поплавского
            { if( P.x>0 )P.x-=Length;
                    else P.x+=Length; w=abs( P )/2.0;
            } h = Va * pow( hypot( w/Ppl,1.0 ),-4.356 );
            if( REGIME )Hg[y][x]=h; else Hg[y][x]+=h;      // без спецконтроля
            if( WaveAs&2 )                                 // узловых смещений
            { if( y>0 )if( (d=Dp[y-1][x])>DZ )
              { w=ey*h*sqrt( 9.8106*(D+d)/2 ); if( REGIME )Uy[y][x]=w;
                                                     else  Uy[y][x]+=w; }
              if( x>0 )if( (d=Dp[y][x-1])>DZ )
              { w=ex*h*sqrt( 9.8106*(D+d)/2 ); if( REGIME )Ux[y][x]=w;
                                                     else  Ux[y][x]+=w; }
          } }
        } else                        //! + цунами на склоне прибрежного шельфа
        { Vs.y*=2.0;  ex*=Glide/2;    // -- начинается с плавной отливной волны
                      ey*=Glide/2;    // направляющие косинусы с поправками
          Length_or_Period( true );
        #pragma omp parallel for private( D )
          for( int y=0; y<My; y++ )                        // прогрессивная
          for( int x=0; x<Mx; x++ )if( (D=Dp[y][x])>DZ )   // составляющая
          { Real d,w,h=Wave( false,Dy*y,Dx*x )*Glide/-2;   // новой волны
            if( REGIME )Hg[y][x]=h; else Hg[y][x]+=h;
            if( y>0 )if( (d=Dp[y-1][x])>DZ )    //
            { w=ey*Wave( false,Dy*(Real(y)-0.5),Dx*x)*sqrt( 9.8106*(D+d)/2 );
              if( REGIME )Uy[y][x]=w; else Uy[y][x]+=w; }
            if( x>0 )if( (d=Dp[y][x-1])>DZ )
            { w=ex*Wave( false,Dy*y,Dx*(Real(x)-0.5))*sqrt( 9.8106*(D+d)/2 );
              if( REGIME )Ux[y][x]=w; else Ux[y][x]+=w; }
          }                                   // сдвиг вниз до половины размера
          Vo=Ve.Goext( (Point){0,Glide/-2} ); // - координаты смещенного центра
          ey=(Va+Glide*Wave(false,Vo.y,Vo.x)/2)/Va; // поправка к новой высоте
          Vs.y/=M_SQRT2;                            // подстройка размера очага
          Length_or_Period( true );                 // установка нового тензора
        #pragma omp parallel for private( D )
          for( int y=0; y<My; y++ )                         // статическая фаза
          for( int x=0; x<Mx; x++ )if( (D=Dp[y][x])>DZ )    // большой волны
                   Hg[y][x]+=Wave( false,Dy*y,Dx*x )*ey;    // c корректировкой
        } EGraph( true );
          Vo=SaveVo,Vs=SaveVs;
          Length_or_Period( true );
      }   break;
      case 0xF7: ans=Seismic_Source(); Tm.Back(); goto Execution;
      case 0xFa: for( int y=0; y<My; y++ )            // Очистка всех
                 memset( Hg[y],0,Mx*sizeof( Real ) ), // массивов данных
                 memset( Uy[y],0,Mx*sizeof( Real ) ),
                 memset( Ux[y],0,Mx*sizeof( Real ) ); EGraph( true ); break;
      case _Esc: ans=0;
      case 0xF2: case 0xF3: case 0xF9: goto _Ret;
    }
  } _Ret: if( WTYPE )WaveAs&=~2; Ghelp(); Tv_place( 0,&F ); Vs/=2;
  return ans;
}
int TWave::Seismic_Meteo( const Event& Tsm, const Point &Geo, Real M, Real H )
{ int k,m=3;
 static char n[5][10]={ "Source: ","static"," or ","floating"," waves " };
  Ghelp(); Mag=M; Mdp=H;
  if( Ma ){ Allocate( 0,Ma ); Ma=NULL; Na=0; }         // очистка старого очага
  do
  { color( GREEN ); Tgo( 1,0 ); for( k=0; k<5; k++ )Tprintf( n[k] );
    color( CYAN ); Tprintf( "  \x1b\x09\x1a  <Esc>quit <Enter>accept" );
    color( BLUE ); Tgo( m==1?9:19,0 ); Tprintf( strupr( n[m] ) ); strlwr( n[m] );
    k=Tv_getc();
    if( k==East || k==West )m ^= 2;
    if( k==_Esc )return 0;
  } while( k!=_Enter ); Tgo( 1,0 ); Tprintf( "\n" );
//
//   по возврату из каталога полностью подготовлен новый очаг цунами
//
  LorP=false;                               // фиксируется длина волны
  Chart.DtI( Vo=Geo );                      // очаг идет в узловых отсчетах
  Vc=Chart.Cs-arg( Bottom_Gradient( Vo ) )*Radian-90.0;
  Macroseismic( Mag,Vs,Va );                // магнитудная оценка очага цунами
  Vo.y *= Dy;                               // очаг в дистанциях от начала
  Vo.x *= Dx;
  Length_or_Period( false );
  Instant=Tsm;
  if( m==1 )return 0xF4;                    //  к продолжению постановки
//
//   постановка динамически генерируемого очага цунами по падению давления
//
  Ma=(METEO*)Allocate( ( Na=5 )*sizeof( METEO ),Ma );
  for( int i=0; i<5; i++ )
  { METEO &M=Ma[i]; M.G=Geo;
                    M.Avs=M.W=0;
                    M.T=i*Vt/8.0;
//                  M.R=Vs.y;
//                  M.R=(i+1)*(i+1)*Vs.y/9;
//                  M.R=(i+2)*Vs.y/4.0;
//                  M.R= (i<=2?i:4-i)*Vs.y/2.0;
//                  M.R=(5-i)*Vs.y;
                    M.R=i*Vs.y/2.0;
//                  M.R=2.0*Vs.y*sin( M_PI*Real( i )/12.0 );
//                  M.R=Vs.y*sin( M_PI_4*Real( i ) );
                    M.H=-2.0*Va*sin( M_PI_4*Real( i ) );
//                  M.H=-2.0*Va*sqrt( sin( M_PI*Real( i )/4.0 ) );
//                  M.H=-0.5*Va * (i<3?i:4-i);
//                  M.H=-Va;
  } return 0;
}
