//
//      Mario.Filtration
//      Комплекс функций для преобразования мариограмм
//
#include "Mario.h"
                        //
static Real *Af=NULL;  // Коэффициенты фильтрационного окна
static int    wp=0;     // Длина этого окна
                        //
static void filtr( int Mode, int Wp )
{ if( Wp>1 )
  { int k; void KIZERS( int,Real* ); Real w=0;
    if( Wp!=wp )Af=(Real*)Allocate( (wp=Wp)*sizeof(Real),Af );
    if( Mode==2 )KIZERS( wp,Af ); else
    for( k=0; k<wp; k++ )Af[k]=Mode ? 1:( 2*k<wp?k+1:wp-k );
    for( k=0; k<wp; k++ )w+=Af[k];
    for( k=0; k<wp; k++ )Af[k]/=w;
  }
}
static Real filtr( Real* AA, int I, int N )
{
 Real *A=Af,W=0; if( wp<2 )return AA[I];
 int i=I-wp/2,
     n=i+wp;
  if( n>=N )n=N-1; else
  if( i<0 ){ A-=i; i=0; } else N=0; AA+=i; i-=I; n-=I; I=i;
  while( i++<n )W += *A++ * *AA++;
  if( N )
  { Real w; for( w=0; I<n; I++ )w += *--A; W/=w;
  } return W;
}
 const Real g=9.8106;  // M/c¤
static Real Wo,Wk,     // Время подхода волны и коэффициент усиления
            Rls=0.96,  // Коэффициент для 2pMoWiF
            Dis=360,   // Разрядность цифрующей полосы
            Ds=36,     // Порог для попытки состыковки
            Db=1,      // Глубина в береговой точке
            Dp=60,     // Глубина в морской точке
            Dl=6000;   // Расстояние между берегом и мариографом
static int  ak,ans,fs,rs,ks; // индексы и - Перерасчеты для колодца мореографа
static
 const char *Fms[]={ "\t Треугольный","\t Прямоугольный","\tКайзер-Бессель" },
            *Rms[]={ "\tГрин-2","\tГрин-3","\tГрин-4", "\tКайстренко" };

#define MM( a,b ) if( i<=a )B=b; else
static void Help( int i )
{ const char* B;
             MM( 2,"F = FxR + F[-1]x(1-R)"   )
             MM( 5,"Осредняющий фильтр"      )
             MM( 10,""                       )
             MM( 11,"Метод Kuniake Abe & Co" )
             MM( 12,"Параметры мореографа"   )
             MM( 14,"Порог поиска / Шкала"   )
  B="";      Wo=Dl/(sqrt( g*Dp )+(rs==3?0.0:sqrt( g*Db )))/1.8; // Время
  if( rs==1 )Wk=pow( Dp/Db,0.3333333 );                         // ^3
  else{      Wk=sqrt( Dp/(rs<3?Db:1) );                         // ^2
   if( rs>0 )Wk=sqrt( Wk );                                     // ^4
  }                                                             //
 Thelp( i>5 && i<11?"%s%sЗадержка=%s Усиление=%.3lg":"%s%s",
                 ak?"F1 новый ряд  F2 - оба  ":"",B,DtoH(Wo,2),Wk );
}
#undef  MM
#define Video_X( _T ) ( _T.T+double( _T.D-Tm.D )*24.0 )

void Filtration()
{                       //
 const  int ML=14;      // Длина блока меню
 static int Wp=24;      //
 Real *Dst,*Src;          //
 int j,k; ak=ans=0;       // ak заполняется новыми фильтрованными величинами
 Event t;                 //
 HIMG VideoBuf; m_Copy(); // Выделяется и копируется новый ряд
 Mario &M=Ms[Km];         // для экспериментирования с фильтрами
 Point P={ Tm.T,M.Level+M.Max };
 point p,q;
       p=P; p.y=max( 0,p.y-40 ); P.x=Tm.T+Tn; P.y=M.Level+M.Min;
       q=P; q.y=min( int(Tv.mY),q.y+40 );
       VideoBuf=Tv_GetImage( p.x,p.y,q.x,q.y );
Begin: color( CYAN );
  { Mlist Menu[]=
    { { 1,0,"\t Преобразование записи" }                             //
    , { 2,4,"\tФильтр сдвигом 0< %4.2lf",&Rls },{ 0,0," <1"   }      // 1-2
    , { 1,14,Fms[fs] },{ 0,5," %5u",&Wp },      { 0,0,"\t точ." }    // 3-5
    , { 1, 3,Rms[rs] },{ 0,rs<3?3:0,(char*)(rs<3?" %3.1lf":""),&Db } // 6-7
    ,                  { 0,3,"\x1b%1.0lf",&Dp }                      // 8
    ,                  { 0,4,"\tм на %4.2lf",&Dl }                   // 9
    ,                  { 0,0,"\tкм" }                                // 10
    , { 1,1,(char*)(ks?"\tУровень моря":"\tМореограф") }             // 11
    , { 0,1,(char*)(ks?"\t на мореограф":"\t на уровень моря") }     // 12
    , { 1,4,"\tПерехлест шкалы %4.0lf",&Ds },{ 0,4,"/%.0lf",&Dis }   // 13-14
    };                                                               //
    Dp=M.Depth;
    Dl=M.Distance; ans=Tmenu( Help,Mlist( Menu ),1,-1,ans );
    M.Distance=Dl; Dl*=1000.0;
    M.Depth=Dp;
    switch( ans )
    { case  3: ++fs;  if( fs>2 )fs=0; goto Begin;
      case  6: ++rs;  if( rs>3 )rs=0; goto Begin;
      case 11: ++ks;  if( ks>1 )ks=0; goto Begin;
    } if( Db<1 )Db=1; if( Rls<0 )Rls=0; else if( Rls>1 )Rls=1;
      if( Dp<1 )Dp=1; if( Wp<2  )Wp=2;  else if( Wp>Ms[Km].N )Wp=Ms[Km].N;
  }                               //
  if( ak )                        // ak!=0 длина нового ряда - признак повтора
  { if( ans==_Esc )ans=ML+1; else  // с последующим удалением новой копии
    if( ans==0xF1 )Km--;    else  // -- удалением исходного ряда
    if( ans==0xF2 ){ if( ak!=Ms[Km].N )Ms[Km].allocate( ak ); } else
    if( ans>ML )goto Begin;       // развод на повторение
           else goto Start;       // или на исполнение
  }                               //
  if( ans>ML )
  { if( !ak || ans!=0xF2 ){ m_Delete(); --Km; } Tv_FreeImage( VideoBuf );
    if( ak )m_View(); return;     // завершающая перерисовка картинки
  }
Start:                                     //
  if( ak )Tv_PutImage( p.x,p.y,VideoBuf ); // восстановление испачканной картинки
  Dst=M.V;                                 //
  Src=Ms[Km-1].V;               //
  if( ans<13 )                  // Алгоритмы фильтрации и
  { color( MAGENTA );           // близбереговой трансформации
    ak=0; t=M.JT;               //
    if( ans==1 )ak++,t+=M.dt; else if( ans<6 )filtr( fs,Wp );
    for( k=0; ak<M.N; t+=M.dt,ak++,Dst++ )
    { if( ans==1 )
      { if( ak<M.N-1 )Dst[1]=Rls*Dst[0] + *++Src*(1-Rls); } //2pMoWiF
      else
      if( ans<6 )Dst[0]=filtr( Src,ak,M.N ); // Три простых фильтра
      else
      if( ans<11 )
      { if( rs<3 )Dst[0]=Wk*( t<M.JT+Wo?0:Ms[Km-1].value( t+(-Wo),Pw ) );
        else                            //
        { Dst[0]=(Src=Ms[Km-1].V)[0];   // Пересчет мариограммы на берег
          if( t-M.JT>Wo )               // Алгоритм Кайстренко
          { Real jt=0,rt,rw=0,d0=0,d1,WO=sqr( Wo/M.dt );
            for( j=0; j<M.N-1 && jt<(t-M.JT)-Wo; j++,Src++ )
            { rt=ak-j;
              d1=Src[1]-Src[0];
              rt=sqrt( sqr( rt )-WO )*( d1-d0 );
              Dst[0]+=( rt+rw )/2; rw=rt;
                                   d0=d1;
              jt+=M.dt;
            } Dst[0]+=rw*( (t-M.JT)-Wo-jt+M.dt )/M.dt;
          }                             //
        }                               // Алгоритм Kuniake Abe
      } else                            // для пересчета на мариограф
      { static Real D=1,               // - диаметр колодца
                    d=0.1,l=2;         // - диаметр и длина трубы
        Real dV,W=0; // F = fe + f*l/d + fo - коээффициент трения
        static int N=1;                 // - количество морских труб
        if( !ak )                       //
        { static Mlist Menu[]=
          { { 1,4,"\tДиаметр колодца %4.2lf",&D },{ 0,0,"\tм" }
          , { 1,4,"\tДиаметр трубы   %4.2lf",&d },{ 0,0,"\tм" }
          , { 1,4,"\tДлина трубы     %4.2lf",&l },{ 0,0,"\tм" }
          , { 1,4,"\tКоличество труб %4d",   &N }
          };
          if( Tmenu( Mlist( Menu ),1,-1 )==_Esc )break;
          W = N*(d*d/D/D)/sqrt( 1.5+0.013264*pow(4/d,0.3333)*l/d );
          W *= 4*g*W;
          if( ks>0 )W=-W;
        }
        dV = (Src[ak<N-1?1:0]-Src[ak>0?-1:0])/M.dt/3600.0;
        Dst[0]=Src[0]+dV*(dV<0?-dV:dV)/W; ++Src;
      }
      if( t-Tm>=0 && t-Tm<=Tn )
      { if( !k ){ moveto( Video_X( t ),M.Level+Dst[0] ); k=1; }
            else  lineto( Video_X( t ),M.Level+Dst[0] );
      }
      if( Tv_getw()==_Esc ){ Thelp( ">Стоп ? " );
        if( (Tv_getc()|0x20)=='y' )break; else k=0; Ghelp();
      }                 //
    }                   // Блок выравнивания ряда сформированоого
  } else                // на ограниченной (Dis) разрядной сетке
  { int cl;             //
    Real W,Lw=0,Lm=M.Level+Ms[Km-1].Min;
    color( BLACK ); line( Tm.T,Lm,Tm.T+Tn,Lm );
    for( t=M.JT+M.dt,ak=1; ak<M.N; ak++,t+=M.dt )
    { W=Src[1]-Src[0];                          //
      if( fabs( W )>=Ds ){ cl=BLACK;            // Остановка на пороге
        if( fabs( W+Dis )<=Ds )Lw+=Dis; else    //
        if( fabs( W-Dis )<=Ds )Lw-=Dis; else cl=MAGENTA;
      } else cl=GREEN;
      if( t-Tm>=0 && t-Tm<=Tn )
      { Real x1,x2;
        color( cl ); line( x1=Video_X( t )-M.dt,M.Level+Src[0],
                           x2=Video_X( t ),M.Level+Src[1] );
        if( cl==MAGENTA )line( x1,Lm,x2,Lm );
      } *++Dst = Lw+ *++Src;
    }           //
  } goto Begin; // здесь бесконечный цикл с выходом под контролем из строк меню
}               //
