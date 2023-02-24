//
//      TideEdit.cpp
//      Блок программ ведения списков гармонических постоянных
//
#include "Tide.h"
#include "..\Graphics\Tv_graph.h"

extern char Months[12][10],          //
            Weeks[7][12];            // Mario
void Segment_Line( Real,int=CYAN ); //
                       //
static const char      // Приливная база данных
 *Hc[]={ "Sa","SSa",  "Mm", "Mf", "2Q1","SIG1",  "Q1", "RO1",  "O1",  "M1",
        "HI1","PI1",  "P1", "S1",  "K1","PSI1", "FI1","TET1",  "J1", "OO1",
        "2N2","MU2",  "N2","NU2", "M2",  "LD2",  "L2",  "T2",  "S2",  "R2",
         "K2", "M3",  "S3","MSf", "MP1", "SO1", "OQ2","MNS2", "OP2","MKS2",
       "MSN2","KJ2","2SM2","MO3", "SO3", "MK3", "SK3", "MN4",  "M4", "SN4",
        "MS4","MK4",  "S4","SK4","2MN6",  "M6","MSN6","2MS6","2MK6","2SM6",
       "MSK6", "M8",  "S5", "S6",  "S7",  "S8",  "S9"
       };                              //
static int   KT=0;                     // Номер текущей записи
static long  Js=0;                     // Фиксированная дата для графика
static char  Fnm[MAX_PATH]="Tide.tim", // Имя последнего считанного файла
             Msg[50]="";               // Всякая География
                        //
static void TideWrite() // Запись на диск списка данных по Мореографам
{ int k,l;              //
  FILE *F;
  static char mod[]="x+b";
    Thelp( "%79s","<Enter>новый или <Tab>добавка к файлу" ); Tgo( 0,0 );
    switch( Wgets( fext( Fnm,"tim" ),-48 ) )
    { case _Tab  : *mod='r'; break;
      case _Enter: *mod='w'; break;
     default     : return;
    }
Open:
  if( (F=fopen( fext( Fnm,"tim" ),mod ))==0 )
  { if( *mod=='r' ){ *mod='w'; goto Open; } }else
  { if( *mod=='r' )
    { fread( Msg,1,6,F );
      if( Mario_Title( Msg ) )fseek( F,0,SEEK_END ); else goto abend;
    } else
    { fwrite( FTitle,1,6,F );
      put2( int(Time_Zone*10),  F );
      put2( int(Tide_Map.Jy*10),F );
      put2( int(Tide_Map.Jx*10),F );
      put2( int(Tide_Map.Ly*10),F );
      put2( int(Tide_Map.Lx*10),F );
    }
    for( k=0; k<Number_Tides; k++ )
    { Tide &M=List_of_Tide[k];
        put2( M.Len,F );
      fwrite( M.Name,1,20,F );  //
        putw( M.Day,F );        //-0     Юлианская дата
        putf( M.Fi, F );        //+4     Географические координаты в градусах
        putf( M.La, F );        //+8
        putf( M.Co, F );        //+12    Средний уровень моря (ТНГ)
        putf( M.Ch, F );        //+16    Критический уровень моря
        putf( M.Cs, F );        //+20    Коэффициент пересчета на высоту цунами
        put2( M.sW, F );        //+24=26 Слово признаков - старая запись 16 бит
      if( (l=List_of_Tide[k].Len)>0 )
        { fwrite( M.H,1,l,F );
          for( int i=0; i<=l*2; i++ )putf( M.C[i],F );
        }
      else putw( 0,F );         // это просто старая ошибка
    } abend: fclose( F );
  }
}                                       //
static void Graph( int C, long J )      // [1] - Max(-1)
{ int  i,j,k,M,*L=0;                    // [0] - Min(-2)
 Real **S=0,*s,St=1,H=0,v,w;            //
 Tide *Mt;
 point p;
 Bfont B( _Small_font,(Tv.mY-Tv_port.bottom)/3 );

  for( k=M=0,Mt=List_of_Tide; k<Number_Tides; Mt++,k++ )
  if( Mt->Len>0 && (Mt->sW&0x8000)==0
   && Mt->Fi>Tide_Plc.Jy && Mt->Fi<Tide_Plc.Jy+Tide_Plc.Ly
   && Mt->La>Tide_Plc.Jx && Mt->La<Tide_Plc.Jx+Tide_Plc.Lx
  ){
    S=(Real**)Allocate( M+1,sizeof(Real)*75,S );
    L= (int*) Allocate( (M+1)*sizeof(int),L );
    L[M]=k;
    s=S[M]+2;
    Current_Tide=List_of_Tide+k;
    Current_Tide->TideSeries( s,73,0.3333,J,0 );
    if( C )
    for( v=Mt->Ch,w=List_of_Tide->Cs,j=0; j<73; j++ )s[j]=w*(v-s[j]);
                                          s[-2]=s[-1]=s[0];
    for( j=1; j<73; j++ )               //
    { if( s[j]<s[-2] )s[-2]=s[j]; else  // Minima
      if( s[j]>s[-1] )s[-1]=s[j];       // Maxima
    }                                   //
    if( s[-1]-s[-2]<1 )s[-1]=s[-2]+1; w=Tv_step( s[-1]-s[-2] );
    if( St<w )St=w;
       ++M;
  } if( !M )goto Leave;
  for( k=0;k<M;k++ )
  { s=S[k]; s[0]=floor( s[0]/St )*St;
            s[1]=ceil( s[1]/St )*St; H+=s[1]-s[0];
  } color( CYAN );
    clear();
  { Field F={ 0,0,24,0 }; F.Ly=H; Tv_place( 0,&F ); Tv_rect();
  }
  setlinestyle( USERBIT_LINE,0x1111,NORM_WIDTH );
  Tv.Height( Tv.Th/2 );
  for( p.y=Tv_y( 0 )+5,k=1; k<24; k++ )
  {    p.x=Tv_x( k ); color( LIGHTCYAN ); line( p.x,0,p.x,p.y );
       if( !(k%2) )
       {  color( CYAN ); Tv.Text( South,p,_Fmt( "%d",k ) ); }
  }
  color( LIGHTBLUE );
  for( w=St; w<H-St/2; w+=St )line( Real(0),w,Real(24),w );
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  color( RED ); p.x=3*Tv.mX/4;
                p.y+= Tv.Th*2;
  B.Text( South_East,p,W2D ( _Fmt( C?"K· ( H-Tide )":"Tide" )));
  B.Height( Tv.Th+2 );
  w=0;
  for( k=0; k<M; k++ )
  { j=L[k]; s=S[k]+2; w-=s[-2]; ((int*)Msg)[0]=' ';
    i=1;
    v=List_of_Tide[j].Cs;
    if( C )
    { if( v!=1 )i+=sprintf( Msg+1,W2D( "%lg · { " ),v );
                   sprintf( Msg+i,"%lg - ",List_of_Tide[j].Ch );
    } scat( Msg,List_of_Tide[j].Name,-20 );    strcut( Msg );
    if( C && v!=1 )scat( Msg," }" );           color( LIGHTBLUE );
    Tv.Text( East,Real(0),s[-2]+w+St/2,Msg ); color( CYAN );
   char fmt[]="%lg_";
    for( v=s[-2]; v<s[-1]-St/2; v+=St )
    { B.Text( North_West,Real(0),v+w,_Fmt( fmt,v ));
      color( CYAN  ); fmt[2]=' ';
    }
    color( GREEN );
    setlinestyle( SOLID_LINE,0,2 ); //THICK_WIDTH );
    for( i=0; i<73; i++ )if( !i )moveto( Real(i)/3,s[i]+w );
                            else lineto( Real(i)/3,s[i]+w ); w+=s[-1];
    setlinestyle( SOLID_LINE,0,NORM_WIDTH );
  } Leave: Tv.Height( 0 );
            Allocate( 0,0,S );
            Allocate( 0,L );
}
static char* Wnum()
{ static char Msg[6]; int i,j;
  for( i=j=0; i<Current_Tide->Len; i++ )if( (Current_Tide->H[i]&0x80)==0 )++j;
  sprintf( Msg,"%2d/%-2d",j,i );
 return Msg;
}
static char* Pname( int K )
{ char *s;    Current_Tide=List_of_Tide+K;
  s=scpy( Msg,Current_Tide->Name,20 );
  s+=sprintf( s,"%c%s ",Current_Tide->sW&0x8000?'<':' ',Wnum() );
  if( Current_Tide->Day!=0 )
  { int m,d,y;
    julday( Current_Tide->Day,m,d,y ); sprintf( s,"for %u:%02u:%02u",y,m,d );
  } else
  if( Current_Tide->Len>0 )
  { int k=KT; KT=K;
    Real h;              s+=sprintf( s,"tide =%4.0lf",h=Current_Tide->TidePoint() );
    if( Current_Tide->Ch!=0 )sprintf( s,"\\%-4.0lf",Current_Tide->Cs*(Current_Tide->Ch-h) );
                    else     sprintf( s,"  cm" );
    KT=k;
  } return Msg;
}                               //
static void Texts_Report()      // Сохранение в текстовом формате
{ int k,l,d,m,y;                //
  FILE *F;
      Tgo( 0,0 );
  if( Wgets( fext( Fnm,"lst" ),-79 )==_Enter )
  if( (F=Topen( fext( Fnm,"lst" ),"w" ))!=0 )for( k=0; k<Number_Tides; k++ )
  { Tide& M=List_of_Tide[k];
    scpy( Msg,M.Name,-20 );
    julday( (M.Day!=0?M.Day:julday()),m,d,y );
    fprintf( F,_Fmt( "%-20s\n",Msg ) );
    fprintf( F,_Fmt( "%5d-%s-%02d  (%ld)\n",y,Months[m-1],d,M.Day ) );
    fprintf( F,W2D( _Fmt( "%13s    широта\n",DtoA( M.Fi,3,'°' ) ) ) );
    fprintf( F,W2D( _Fmt( "%13s долгота\n"
               "%12lg    средний уровень моря над ТНГ\n"
               "%12lg    наивысший возможная высота\n"
               "%12lg    барический коэффициент\n"
               "%12ld    количество приливных гармоник\n",
               DtoA( M.La,3,'°' ),M.Co,M.Ch,M.Cs,M.Len ) )
    );
    for( l=0; l<M.Len; l++ )
    fprintf( F,W2D( _Fmt( " %2d %4s %4lgcм %5lg°\n",
        M.H[l]&0x7F,Hc[(M.H[l]&0x7F)-1],M.C[l*2+1],M.C[l*2+2] ) ));
    if(k<Number_Tides-1)fprintf( F,"\n" );
        else  fclose( F );
} }
static int Help()
{ static int d,m,y; int a=0;
  static Mlist Menu[]=
  { { 1,0,"\t  Морские  П Р И Л И В Ы"  }
  , { 2,8,"\t?Часовой пояс %2°",&Time_Zone }
  , { 1,5,"\t?Дата %5d",&y},{0,2,":%02u",&m},{0,2,":%02u",&d},{ 0,1 }
  , { 2,0,"\tF2 Сохранить новый архив"  }
  , { 1,0,"\tF3 Взять данные с диска"   }
  , { 1,0,"\tF4 Мореографы на карте"    }
  , { 1,0,"\tF5 Прилив на текущий день" }
  , { 1,0,"\tF6 Запись таблиц гармоник" }
  , { 1,0,"\tF10 - Сортировка -"        }
  , { 1,0,"\t<Tab> Преключение режима"  }
  , { 1,0,"\t<Enter>   Редактирование"  }
  , { 1,0,"\t<Ins/Del> - для текущей"   }
  , { 1,0,"\t<BkSp>    - отметка ряда"  }
  , { 1,0,"\t<Space>  Остановка часов"  }
//, { 2,0,"\t     (с)95-06, В.Храмушин" }
  };
 Display T( Mlist( Menu ) );
  if( Js )julday( Js,m,d,y ); color( DARKGRAY );
  for( ;; )
  { if( Js!=0 ){ Menu[5].Msg="\t замер";   Js=julday( m,d,y );       }
         else  { Menu[5].Msg="\t сегодня"; julday( julday(),m,d,y ); }
    if( a==_Esc || a>0xF0 )break;
    if((a=T.Answer())==5){ if( Js!=0 )Js=0; else Js=julday( m,d,y ); }
  } return a;
}
static int Tide_sort_function( const void *a, const void *b )
{ return strncmp( (char *)a+sizeof(short),(char *)b+sizeof(short),20 );
}                       //
int TideSetup()         // Установка мариографных данных
{                       //
 static char Hlp[]="F1 Inform  F3 Read  F4 Map %s%s",Msg[40];
 static field Fc={ 0,-1,1,12,0 };
 point p;              //
 long  J,JJ=0;         // Текушие Дата
 Real  ZZ=0,TT=0,      //  и данные для запоминания при белом стирании
       T,z;            //  и Время
 int   Wmap=0,         // Wmap принудительное рисование
       Mod=1, Vm=1,    // Режим графики прилива или (1)кр.уровня
       ans=Number_Tides>0?0:_Tab,
       k,t,d,m,y;
 static bool _Arg=true; if( _Arg )_Arg=_ac_>1;
  for( Js=0;; )
  { Thelp( Hlp, "<Space>Задержка таймера ",
          !Number_Tides?"<Insert>Новый пункт":"<Tab>Список пунктов" );
    J=julday();
    T=onetime();
    if( _Arg ){ TideFileRead( _av_[1] ); ans=0xF4; _Arg=false; _ac_=0; }
    else
    //   Блок слежения за таймером, и управление изображением
    {
     int s,c,GrD=Wmap<0 && Js!=0;
     long jg;
     Bfont B( _Small_font,c=(Tv.mY-Tv_port.bottom)/3 );
      julday( jg=( GrD?Js:J ),m,d,y ); z=-1;
      p.x=0;
      p.y=Tv_port.bottom+c+2; color( BLACK );
          sprintf( Msg,"%5d.%s.%02d",y,Months[m-1],d );
          B.Text( South_East,p,Msg );
      p.x=B.StrLen( Msg )+22; color( BLUE );
      B.Text( South_East,p,strcpy( Msg,Weeks[int(jg%7)] ) );
      p.x+=B.StrLen( Msg )+4;
      p.y+=c;  color( CYAN );
      sprintf( Msg,Time_Zone==0?"(GMT) ":"(%s) ",DtoA( Time_Zone,-1,0xFC )+1 );
      B.Height( B.Th-2 );
      B.Text( North_East,p,Msg );
      p.x+=Tv.StrLen( Msg )+Tv.Tw;
      if( !Number_Tides || ans==_Tab )
      { k=t=int( fmod( T*60,60 ) );
        c=-1;                 //
        do                    // Блок опроса клавиатуры с таймером
        { color( BLUE );      //
          if( GrD )ans=' '; else
          { T=onetime(); t=(int)fmod( T*60,60 ); s=(int)fmod( T*3600,60 );
            if( s!=c )
            { //pattern( BLACK );
              //bar( p.x,p.y-Tv.Th,2*Tv.mX/3,p.y );
              Tv.Text( North_East,p,_Fmt( "%-12s",DtoA( T,3,0xFC )+1 ) ); c=s;
              if( Wmap<0 && z>=0 )Segment_Line( z );
              if( t!=k && (Wmap>0 || J!=julday()) )
              { J=julday(); ans=_Tab; goto Leave; }
              else
              { Sleep( 250 ); ans=Tv_getw();
              }
              if( Wmap<0 )Segment_Line( z=T );
            } if( ans ){  Segment_Line( z ); z=-1; }
          }
          if( ans==' ' ){ Tv_bell(); Ghelp(); ans=Tv_getc(); if( !GrD )ans=0; }
        } while( !ans );
        if( !Number_Tides || ans>0xF0 )goto Leave; // переход к выбору команды
      } t=(int)T;
      color( BLUE );
//    bar( p.x,p.y-Tv.Th,p.x+Tv.Tw*8,p.y );
      Tv.Text( North_East,p,_Fmt( "%02d\xFC%02.0lf'",t,(T-t)*60 ));
      color( DARKGRAY );
      Thelp( Hlp,"<Tab>Active ","<Enter>Edit <Ins|Del>City <BkSp>Mark" );
      ans=Tcase( Fc,42,Pname,Number_Tides,KT,Vm ); Vm=-1;
      if( ans==_Esc )
      { Thelp( ">Вы уверены?" );
        Course a=Tv_getc(); if( (a|0x20)!='y' && a!=_Enter )ans=_Tab;
      }
    Leave:
      if( ans==0xF1 )
      if((ans=Help())<0xF2 )ans=Wmap<0?0xFF:_Tab;
      p.y-=Tv.Th+15; color( DARKGRAY );
    }
//      Собственно командный процессор
//
    switch( ans )
    { case _Esc: Current_Tide=0; ans=TideDefault( KT );
        if( Vm<0 )Tback(); Ghelp(); return ans;
      case _Ins:
        List_of_Tide=(Tide*)Allocate( (++Number_Tides)*sizeof(Tide),List_of_Tide );
        for( k=Number_Tides-1; k>KT; k-- )List_of_Tide[k]=List_of_Tide[k-1];
        if( KT<Number_Tides-1 )++KT;      //
        List_of_Tide[KT].H=0;             // Добавляем
        List_of_Tide[KT].C=0;             // следующую запись
        if( KT>0 )                        // и дублируем ее
        { k=List_of_Tide[KT].Len;         // из предыдущей
          List_of_Tide[KT].allocate( k ); //
          memcpy( List_of_Tide[KT].H,List_of_Tide[KT-1].H,k );
          memcpy( List_of_Tide[KT].C,List_of_Tide[KT-1].C,(k*2+1)*sizeof( Real ) );
        }
//      List_of_Tide[Number_Tides-1].Cs=1;
        List_of_Tide[KT].Edit();
      case _Tab :                          Vm=0; break;
      case 0xF3: TideFileRead( "*.tim" ); Vm=0; break;
      case 0xF4:
      { int World_Map( int,Field*,Field* ); Vm=1; Tback();
        Wmap=World_Map( Wmap,&Tide_Map,&Tide_Plc );
        if( Wmap>0 )JJ=0; Js=0; ans=_Tab;        // Wmap=1 - Карта на экране
      } break;                                  //
     default: if( Number_Tides<=0 )Wmap=0; else // ? если есть
      switch( ans )                             // ! что править
      { case 0xF2: TideWrite(); break;          //
        case 0xF5: Mod^=1;
        case 0xFF: Vm=0; ans=_Tab; Wmap=-1; break;
        case 0xF6: Texts_Report();          break;
        case 0xFA: Vm=0;
             qsort( (void*)List_of_Tide,Number_Tides,sizeof(Tide),Tide_sort_function );
             break;
        case _BkSp :List_of_Tide[KT].sW^=0x8000; break; // Признак пассивности
        case _Del  :List_of_Tide[KT].allocate( 0 );     //
                    for( k=KT; k<Number_Tides-1; k++ )List_of_Tide[k]=List_of_Tide[k+1];  Vm=0;
                    List_of_Tide=(Tide*)Allocate( (--Number_Tides)*sizeof( Tide ),List_of_Tide ); break;
        case _Enter:List_of_Tide[KT].Edit(); break;
    } }
    if( !Vm || Wmap ){ Vm=1; Tback();
      if( Wmap<0 )Graph( Mod,Js?Js:J ); else
      if( Wmap>0 )
      { for( k=KT,KT=0; KT<Number_Tides; KT++ )
        { Tide& M=List_of_Tide[KT];
          if( (M.sW&0x8000)==0
            && M.Fi>Tide_Plc.Jy && M.Fi<Tide_Plc.Jy+Tide_Plc.Ly
            && M.La>Tide_Plc.Jx && M.La<Tide_Plc.Jx+Tide_Plc.Lx )
          { if( JJ )
            { z=Time_Zone; Time_Zone=ZZ; M.Draw( false,JJ,TT ); Time_Zone=z; }
            M.Draw( true,J,T );
          }
        } KT=k; ZZ=Time_Zone; JJ=J; TT=T;
  } } }
}
inline char* Hname( int k ){ return (char*)Hc[k]; }

static Course Hhelp( int k )
{ Thelp( "Wave[%d]: %s = %s",k+1,Hc[k],DtoA( 360.0/Tidal_Quad[k],3,'ч' ) );
  return Tv_getc();
}
static char* Wname( int k )
{ int H=Current_Tide->H[k];
  sprintf( Msg,W2D( "%2d %4s%c%7.2lfсм %7.2lf°" ),
      H&0x7F,Hc[(H&0x7F)-1],H&0x80?'<':' ',Current_Tide->C[k*2+1]+0.001,
                                           Current_Tide->C[k*2+2]+0.001 );
 return Msg;
}
static void WavEdit( Tide &W )
{ const int Hw=15;
  const char Digit[]=".+-0123456789";
 static field Fc={ 51,-1,4,17,0 };
 int i,j=6,k1=0,k=0,kn=Hw-1,ans=0; color( BLUE );
  for( ;; )
  { if( ans<=0 )
    { if( W.Len>Hw )
      { if( k<k1 ){ k1-=Hw-2; if( k1<0      )k1=0; kn=k1+Hw-1; }else
        if( k>kn ){ kn+=Hw-2; if( kn>=W.Len )kn=W.Len-1; k1=kn-Hw+1; }
      } else
      { kn=W.Len-1; k1=0; }                 //
      if( k<k1 )k=k1; else if( k>kn )k=kn;  // в пустом списке
      if( !ans )                            //   k=kn=-1
      { i=color( GREEN );
        Twindow( 23,-1,26,kn-k1+3,3 ); Tpoint.y+=8;
        Tprintf( W2D( "  Волна   Размах   Фаза " ) );
        color( i );
      } Tgo( 1,2 );
      for( i=k1; i<=kn; i++ ){ Tln(); Tprintf( Wname( i ) ); }
    }
    if( !W.Len )ans=_Ins; else
    { Tgo( j,k-k1+3 ); Thelp
      ( "[%d/%d]=%s <Ins>Добавление -<Enter>Выбор -<Space>Отметка -<Del>Удаление -<Esc>Выход",
         k+1,W.Len,DtoA( 360.0/Tidal_Quad[((Current_Tide->H[k])&0x7F)-1],3,'ч' )
      ); ans=Tgetc();
    }
    if( ans<_Enter )
    { switch( ans )
      { case North     : --k;     break;
        case South     : ++k;     break;
        case North_East: k-=Hw-2; break;
        case South_East: k+=Hw-2; break;
        case North_West: k=0;     break;
        case South_West: k=W.Len-1; break;
        case East: if( j==1  )j=6;  else
                   if( j==6  )j=11; else
                   if( j==11 )j=20; else j=1; break;
        case West: if( j==1  )j=20; else
                   if( j==20 )j=11; else
                   if( j==11 )j=6;  else j=1; break;
      }
      if( k<0 )k=0; else
      if( k>=W.Len )k=W.Len-1;
      if( k<k1 || k>kn )ans=-1;
    } else
    switch( ans )
    {
     case _Ins: W.allocate( W.Len+1 );
      if( W.Len==1 ){ W.H[0]=1; for( i=0;i<3;i++ )W.C[i]=0; }else
      for( i=W.Len-1; i>k; i-- )
      { W.H[i]=W.H[i-1]; W.C[i*2+1]=W.C[i*2-1];
                         W.C[i*2+2]=W.C[i*2];
      } Tback(); ans=0; break;
     case _Del:
      for( i=k; i<W.Len-1; i++ )
      { W.H[i]=W.H[i+1]; W.C[i*2+1]=W.C[i*2+3];
                         W.C[i*2+2]=W.C[i*2+4];
      } W.allocate( W.Len-1 );
      if( kn>=W.Len ){ --kn; --k1; --k; if( k1<0 )k1=0; } Tback(); ans=0;
      if( W.Len )break; else goto Leave;
     case _Esc: Tback();    goto Leave;
     default:  if( ans>'z' )break;
      if( ans==' ' )W.H[k]^=0x80; else
      { Real &Cw=W.C[k*2+(j==11?1:2)];
        i=( W.H[k]&0x7F );
        if( j==6 )
        { --i; if( Tcase( Fc,5,Hname,Hhelp,67,i )!=_Esc )W.H[k]=i+1;
        } else
        if( j==1 )
        { sprintf( Msg,"%2d",i ); i=0;
          if( strchr( Digit+3,ans ) ){ Msg[0]=ans; i=1; }
          Tgo( 1,k-k1+3 );
          if( Tgets( Msg,-2,i )!=_Esc )
          { sscanf( Msg,"%d",&i ); if( i>0 && i<=67 )W.H[k]=i; }
        } else
        { sprintf( Msg," %7.2lf",Cw+0.0045 );
          if( strchr( Digit,ans ) )Msg[j==11?3:2]=ans;
          Tgo( j==11?8:18,k-k1+3 );
          if( Tgets( Msg,-8,j==11?4:3 )!=_Esc )sscanf( Msg,"%lf",&Cw );
        }
      } Tgo( 1,k-k1+3 ); Tprintf( Wname( k ) );
  } }   Leave:;
}
#define _clr( clr ) color( Cl?clr:b )
                                                //
Tide& Tide::Draw( bool Cl, long JJ, Real TT ) // Cl=0 Стирание картинки
{ int  k,y,m=Ch!=0,                           //
       c,b=Tv.BkColor;
 char *si;
 Point P;  P.x=La,P.y=Fi;
 point p;  p=P;
 Real h,s[49],mn;
  Tv.Height( Tv.Th/2 );
  scpy( Msg,Name,-20 ); strcut( si=Msg );
  if( *Msg>='0' && *Msg<='9' ) // отсечение числового номера в имена мореографа
  { if( !(si=strchr( Msg,' ' )) )si=Msg; else
    while( *si<=' ' ){ if( *si==0 ){ si=strcpy( Msg,"\xFA" ); break; } si++; }
  }
  c=_clr( BROWN ); Tv.Text( South,p,si ); _clr( GREEN );
  if( Len>0 )
  { Current_Tide=this;
    Tv.Font( _Small_font,13 );
    p.y+=Tv.Th;
    Tv.Text( m?South_West:South,p,_Fmt( "%.0lf\\",h=TidePoint() ));
    if( m )
    { _clr( LIGHTMAGENTA ); p.y++;
      Tv.Height( 11 );
      Tv.Text( South_East,p,_Fmt( "%.0lf",h=Cs*(Ch-h) ));
    } _clr( GREEN );
    TideSeries( s,49,1,JJ-1,TT );
                             mn=s[0];
    for( k=1; k<49; k++ )if( mn>s[k] )mn=s[k];
    for( p.x-=48,k=0; k<49; k++,p.x+=2 )
    { y=int( p.y-8-(s[k]-mn)/4 );
      if( !k )moveto( p.x,y );
         else lineto( p.x,y );
    } _clr( LIGHTMAGENTA );
    p.x-=48; p.y-=4;                moveto( p );
    p.y-=int( m?4+Cs*(Ch-mn)/4:8 ); lineto( p );
    if( m ){ p.x-=2;                moveto( p );
             p.x+=4;                lineto( p );
    } Tv.Font( 0,0 );
  }   color( c );
  Tv.Height( 0 );
  return *this;
}
Tide& Tide::Edit()
{ int ans,d,m,y;
 long D=0;
 Mlist Menu[]=
  { { 1, 0,"\t Гармонический ряд" }
  , { 2,20,0,       Name        }
  , { 1,11,"\tШирота  %3°",&Fi    }
  , { 1,11,"\tДолгота %3°",&La  }
  , { 1, 0,"\tВремя"},{0,5,"%5d",&y},{0,2,":%02d",&m},{0,2,":%02d",&d},{0,4}
  , { 1,20,"\tСписок приливных волн" }
  , { 1, 6,"\tУровень ТНГ %6.2lf",&Co },{ 0,0,"\t см" }
  , { 1, 6,"\t+ наивысший %6.2lf",&Ch },{ 0,0,"\t см" }
  , { 1, 5,"\tусиление шельфа %5.2lf",&Cs }
  };
 Display T( Mlist( Menu ),1,-1 ); Current_Tide=this;
 FILE *F;
  if( !Name[0] )
  { sprintf( Name,"\x11 %d \x10",Number_Tides ); Fi=46.95,La=142.75;
    if( Number_Tides>1 )
    if( this!=List_of_Tide+KT )
    { Tide &Td=List_of_Tide[KT]; Fi=Td.Fi; La=Td.La; Day=Td.Day; }
  }
  for( ;; )                             //
  { static char Fn[80]="T~Wave.txt";    // Имя файла импорта-ехпорта
                                        //
    Thelp( "[%s]Волн  F2 Эскпорт  F3 Импорт",Wnum() );
    Menu[8].Msg=(char*)(Day?"\t есть":"\t нет");
          if( !Day )D=julday(); else D=Day;
                      julday( D,m,d,y );
    ans=T.Answer(); D=julday( m,d,y ); if( Day )Day=D;
    switch( ans )       //
    { case 8:           // Закрепление даты
      if( Day )Day=0;   //
          else Day=D; break;
      case 9:           // Обработка списка приливных волн
           WavEdit( *this );
           break;       //
//    case 0xF1:        \\ Информация о текущем состоянии и подсказка
//    {                 \/
//    }   break;        \/
      case 0xF2:        // Сохранение в текстовом формате
        Tgo( 0,0 );     //
        if( Wgets( Fn,-60 )==_Enter )
        if( (F=Topen( Fn,"w" ))!=0 )
        { fprintf( F," %2d %lg\n",Len,Co );
          for( int l=0; l<Len; l++ )
          fprintf( F," %2d %4lg %5lg%c",H[l]&0x7F,C[l*2+1],C[l*2+2],
                                      (l+1)%4 && l<Len-1?' ':'\n' );
          fclose( F );
        } break;        //
      case 0xF3:        // Считывание в формате LSM Рабиновича
      { int k,l;        //
        scpy( sname( Fn ),"*.*" );
        if( (F=Topen( Fn,"r" ))!=0 )
        { fscanf( F,"%d",&l );
          if( l>0 )
          { if( !Len ){ scpy(Name,sname(Fn)); Day=0; Fi=46.95,La=142.75; }
            k=Len; allocate( Len+l ); fscanf( F,"%lf",&Co );
            for( ; k<Len; k++ )
            { fscanf( F,"%d%lf%lf",&l,C+k*2+1,C+k*2+2 ); H[k]=l; }
          } fclose( F );
      } } break;
      case _Esc: if( Len )C[0]=Co; Ghelp(); return *this;
    }
  }
}
