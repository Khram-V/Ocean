//
//      List.cpp                        1993.05.15
//      Программа прорисовки сейсмологических событий
//              на карте батиметрии
//      Формат текстового списка:
/*
YYYY MM DD hh mm ss.s ±FF.ff ±LLL.ll DDD M.s II.i HHH.hh
---------- ---------- ------ ------- --- --- ---- ------
      Дата            Широта Долгота Глубина      Максимальная амплитуда
           Гринвичское                   Магнитуда          цунами
           Время                             Интенсивность


*/
#include <CType.h>
#include "..\Graphics\Tv_graph.h"
#define Ang( x ) (x>=0?x:x+360)
                                //
static const Real Mit=-1.0e3;   // Контрольный минимум Интенсивности
                                //
const char *Months[]=
  { "January","February","March","April","May","June","July",
    "August","September","October","November","December"
  };
static char FileName[MAX_PATH]="Ani.lts",
                 Msg[MAX_PATH]; // Статическая информационная строчка
static FILE *F=0;           //
static long *L=0;           // Список меток текущего текстового файла
static int   N=0;           // Количество записей
static Field Fm;            // Границы текущего изображения
static Point P,Q;           // Географические координаты
static long J1,J2;          // Интервал времени в Юлианских днях
static int Year,Month,Day,  // Дата и
           Hour,Minute;     //  время текущего события
static Real Hg=0,  Hg1,Hg2, // Глубина очага
            Ms=0,  Ms1,Ms2, // Магнитуда
            It=Mit,It1,It2, // Интенсивность цунами по Соловьеву
            Am=0.0,         // Зарегистрированная амплитута
            Sec=0.0,        // Секунды считанные из файла
            Yasp;           //
static fillsettingstype FL; //
static point C[]={ {  0,1000 },{ 383, 924 },{ 707, 707 },{ 924, 383 }
                 , {  1000,0 },{ 924,-383 },{ 707,-707 },{ 383,-924 }
                 , { 0,-1000 },{-383,-924 },{-707,-707 },{-924,-383 }
                 , { -1000,0 },{-924, 383 },{-707, 707 },{-383, 924 }
                 , {  0,1000 } };
 int World_Map( int R, Field *W, Field *M );

Course Help( const char *Name[], const char** Text, const char** Plus );

//
//   Здесь производится выборка конкретных данных из строки в памяти
//
#if 0
static int Read()
{ if(  !fgets( Msg,132,F )               )return 0;
  if(  strcut( Msg )<36                  )return 0;
  if( strcspn( Msg,"\t+-01234567890. " ) )return 0;
  It=Mit; Hg=Ms=Am=0.0;
  sscanf( Msg,"%d%d%d%d%d%lf%lf%lf%lf%lf%lf%lf",
         &Year,&Month,&Day,&Hour,&Minute,&Sec,&P.y,&P.x,&Hg,&Ms,&It,&Am );
  P.x=Angle( P.x );
  if( It>9.89 )It=Mit;
  if( Ms>9.6 )Ms=9.6;
  return 1;
}
#else
static bool Read()
{ char *s; s=Msg; int l;
  if( !fgets( Msg,MAX_PATH,F ) )return false; while( *s && *s<=' ' )s++;
  if( !(*s) || (*s!='-' && !isdigit( *s )) )return false;
  sscanf( s,"%d",&Year ); while( *s && *s>' ' )s++;   // и далее все чисто
  if( (l=strcut( s ))<32 )return false;              // фиксированный формат
  s[3]=0; Month=max( atoi( s ),1 ); s+=4;           //
  s[2]=0;   Day=max( atoi( s ),1 ); s+=3;
  s[2]=0;   Hour=atoi( s ); s+=3;
  s[2]=0; Minute=atoi( s ); s+=3;
  s[4]=0;    Sec=atof( s ); s+=5;
  s[6]=0;    P.y=atof( s ); s+=7;
  s[7]=0;    P.x=atof( s ); s+=8; P.x=Angle( P.x ); It=Mit; Hg=Ms=Am=0.0;
  if( l>33 )
  { s[3]=0; Hg=atof( s ); s+=4;
    if( l>37  )
    { s[3]=0; Ms=atof( s ); s+=4; if( l>41 )sscanf( s,"%lg%lg",&It,&Am );
  } }
  if( It>9.89 )It=Mit;
  if( Ms>10 )Ms=10;
  if( l>33 )
  { s[3]=0; Hg=atof( s ); s+=4;
    if( l>37 )sscanf( s,"%lg%lg%lg",&Ms,&It,&Am ); if( Ms>9.6 )Ms=9.6;
  } if( It>9.89 )It=Mit; return true;
}
#endif
static void AnaLite( Real &X ){ if( X<Fm.Jx )X+=360; else
                                if( X>Fm.Lx )X-=360;
}
//  Настройка таблицы файловых ссылок
//                       First==FALSE - с учетом текущих границ
//                       First < 0    - с выборкой границ области
//                       First > 0    - с выборкой экстремумов данных
void static Analyse( const int First=false )
{ long Lm;                    //
   int n; n=N; N=0;           // Количество меток в оперативной памяти
 static Field fm={ 0,0,1,1 }; //
  rewind( F );
  Ghelp( "Wait Please" ); Tv_place( 0,&fm ); Fm=fm; Fm.Ly+=Fm.Jy;
                          Tv_place( 0,&fm );        Fm.Lx+=Fm.Jx;
  while( !feof( F ) )
  { Lm=ftell( F ); if( !Read() )continue;
    { long J;
      if( P.y<-90 || P.y>90 || P.x<-180 || P.x>180 )continue; //Sec=P.x;
          AnaLite( P.x );
      if( First>=0 )
      if( P.y<Fm.Jy || P.y>Fm.Ly || P.x<Fm.Jx || P.x>Fm.Lx )continue;
      J = julday( Month>0?Month:1,Day>0?Day:1,Year );
      if( First )
      { if( !N ){ J1=J2=J; Hg1=Hg2=Hg;
                           It1=It2=It;
                           Ms1=Ms2=Ms;
          if( First<0 ){ Fm.Jx=P.x-1; Fm.Lx=P.x+1;
                         Fm.Jy=P.y-1; Fm.Ly=P.y+1;
        } } else
        { if( It>Mit ){ if( It1>It )It1=It; else if( It2<It )It2=It; }
          if(  J1>J  )J1=J;   else if(  J2<J  )J2=J;
          if( Hg1>Hg )Hg1=Hg; else if( Hg2<Hg )Hg2=Hg;
          if( Ms1>Ms )Ms1=Ms; else if( Ms2<Ms )Ms2=Ms;
          if( First<0 )
          { if( P.y<Fm.Jy )Fm.Jy=P.y; else if( P.y>Fm.Ly )Fm.Ly=P.y;
            { Real Jx=P.x-Fm.Jx,
                   Lx=P.x-Fm.Lx;
              if( Jx>180 )Jx-=360; else if( Jx<-180 )Jx+=360;
              if( Lx>180 )Lx-=360; else if( Lx<-180 )Lx+=360;
              if( fabs( Jx )<fabs( Lx ) ){ if( Jx<0 )Fm.Jx+=Jx; }
                                   else  { if( Lx>0 )Fm.Lx+=Lx; }
        } } }
      } else
      { if(  J<J1  ||  J>J2  )continue;
        if( Hg<Hg1 || Hg>Hg2 )continue;
        if( Ms<Ms1 || Ms>Ms2 )continue;
        if( It<It1 || It>It2 )continue;
      }
    } if( N>=n )L=(long*)Allocate( (n=N+136)*sizeof(long),L ); L[N]=Lm; N++;
  }   if( N!=n )L=(long*)Allocate( N*sizeof(long),L );      rewind( F );
  if( First<0 )
  { Real ly=( Fm.Ly-=Fm.Jy )/12;  Fm.Jy-=ly; Fm.Ly+=ly+ly;
    Real lx=( Fm.Lx-=Fm.Jx )/12;  Fm.Jx-=lx; Fm.Lx+=lx+lx;
    if( Fm.Ly/Fm.Lx>1 ){ lx=Fm.Lx; Fm.Lx=Fm.Ly*2; Fm.Jx-=(Fm.Lx-lx)/2; }else
    if( Fm.Lx/Fm.Ly>3 ){ ly=Fm.Ly; Fm.Ly=Fm.Lx/2; Fm.Jy-=(Fm.Ly-ly)/2; }
        lx=Fm.Jx+Fm.Lx/2;
    if( lx<-180 )Fm.Jx+=360; else if( lx>180 )Fm.Jx-=360;
    if( Fm.Lx>360 ){ Fm.Jx+=(Fm.Lx/2)-180; Fm.Lx=360; }
    if( Fm.Jy<-90      )Fm.Jy=-90;
    if( Fm.Jy+Fm.Ly>90 )Fm.Ly=90-Fm.Jy;
        Fm.Jx=Angle( Fm.Jx ); World_Map( 0,&Fm,0 );
        Fm.Ly+=Fm.Jy;
        Fm.Lx+=Fm.Jx;
  }
}
void static Open()
{ Tfile_Filter = "Seismic Listing (*.lts)\0*.lts;*.txt\0"
                 "All Files (*.*)\0*.*\0";
  if( F ){ fclose( F ); strcpy( sname( FileName ),"*" ); }
  F=Topen( FileName,"rb","lts","Seismic and tsunami Catalogue (.lts) & <Shift+Enter> for new" );
  if( F )Analyse( Tv_getk()&SHIFT?1:-1 ); else
  if( L ){ Allocate( 0,L ); L=0; N=0; }
}
static int sort_func( const void *a, const void *b )
{ fseek( F,*((long*)a),SEEK_SET ); fgets( Msg,64,F );
  fseek( F,*((long*)b),SEEK_SET ); fgets( Msg+66,64,F );
  return strcmp( Msg,Msg+66 );
}
void static Sorting(){ Ghelp( "Sorting" ); qsort( (void*)L,N,4,sort_func ); }
void static Record( const int k )
{ fseek( F,L[k],SEEK_SET ); Read(); Q=P; AnaLite( P.x );
}
//   Программа для запросов интервалов выборки
//
void static Select()
{
 static int Y1,Y2,M1,M2,D1,D2;
 static Real G1,G2,I1,I2,f,ff,l,ll,H1,H2;
 static Mlist Menu[]=
  { { 1,0,"   Instant Boundaries" }
  , { 2,5,    "%5d",&Y1 },{ 0,2,"-%02d",&M1 },{ 0,2,"-%02d",&D1 }
  , { 0,4," .. %4d",&Y2 },{ 0,2,"-%02d",&M2 },{ 0,2,"-%02d",&D2 }
  , { 2,4," Depth %4.0lf",&H1},{0,4," .. %-4.0lf",&H2},{0,0," km    "}
  , { 1,4,"    Ms %4.1lf",&G1},{0,4," .. %-4.1lf",&G2}
  , { 1,4,"    It %4.1lf",&I1},{0,4," .. %-4.1lf",&I2}
  , { 2,8,"        %2°",&ff }
  , { 1,8,      "  %2°",&l  }
  , { 0,8,    "=+= %2°",&ll }
  , { 1,8,"        %2°",&f  } };
 int ans;
  julday( J1,M1,D1,Y1 ); H1=Hg1; G1=Ms1; I1=It1; color( DARKGRAY );
  julday( J2,M2,D2,Y2 ); H2=Hg2; G2=Ms2; I2=It2; Ghelp();
                        f=Fm.Jy; ff=Fm.Ly;
                        l=Fm.Jx; ll=Fm.Lx;
  do
  { color( CYAN ); ans=Tmenu( Mlist( Menu ),0,1 );
    if( ans<=17 )
    { J1=julday( M1,D1,Y1 ); Hg1=H1; Ms1=G1; It1=I1; Fm.Jy=f;
      J2=julday( M2,D2,Y2 ); Hg2=H2; Ms2=G2; It2=I2; Fm.Jx=l;
      Fm.Ly=ff-f;
      Fm.Lx=ll>l?ll-l:ll-l+360; World_Map( 0,&Fm,0 );
      Fm.Ly+=Fm.Jy;             Ghelp( "Wait Please" );
      Fm.Lx+=Fm.Jx;  Analyse(); Ghelp(); break;
  } } while( ans!=_Esc );
}
//
//      Программа рисования одного события
//
static void Item( const bool _xor=FALSE )
{
 Real D=Ms*Ms*Ms/4.0e4,d;
 point c[17],p=P;
 int i,cl;
  if( !_xor )D*=1.12; d=D;
  if( It>0 ){ D*=1.6-0.6/(It+1.0);
              d*=0.4+0.6/(It+1.0);
            }
  for( i=0; i<=16; i++ )
  { c[i].x=p.x+int( C[i].x*(i&1?d:D) );
    c[i].y=p.y+int( C[i].y*(i&1?d:D)*Yasp );
  }
  color( cl=It <  0.0 ? LIGHTGRAY:
            It <= 0.5 ? CYAN     :
            It <= 1.0 ? GREEN    :
            It <= 2.0 ? LIGHTBLUE:
            It <= 3.0 ? MAGENTA  : BROWN );
  if( !_xor )
  { setwritemode( XOR_PUT ); drawpoly( 17,(int*)c );
    setwritemode( COPY_PUT );
  } else
  { if( Hg>=0 )
    { pattern( SOLID_FILL,WHITE ); fillpoly( 17,(int*)c ); }
      pattern( Hg>300? LTSLASH_FILL:
               Hg>150?   HATCH_FILL:
               Hg>80 ?  XHATCH_FILL:
               Hg>30?CLOSE_DOT_FILL:
               Hg>0 ? WIDE_DOT_FILL:EMPTY_FILL,cl );
      fillpoly( 17,(int*)c );
      pattern( FL.pattern,FL.color );
  }
}
//      Прорисовка всех текущих событий
//
void static Draw(){ for( int i=0; i<N; i++ ){ Record( i ); Item( true ); } }
//
//   Сохранение нового списка в файл
//
void static Save()
{ int i,c=color( DARKGRAY );
  Ghelp( "?FileName:" );
  Tgo( -10,0 ); i=Wgets( FileName,-69,int( fname( FileName )-FileName ) );
  Ghelp();
  if( i==_Enter )
  { FILE *F;
    if( (F=Topen( FileName,"wt","lts" ))!=NULL )
    { for( i=0; i<N; i++ )
      { Record( i );
        fprintf( F,"%4d %2d %2d %2d %2d %4.1lf %6.2lf %7.2lf %3.0lf %3.1lf",
                 Year,Month,Day,Hour,Minute,Sec,P.y,Angle( P.x ),Hg,Ms );
        if( It==Mit ){ if( Am!=0.0 )fprintf( F,"    0" ); } else
        fprintf( F," %4.1lf",It );
        fprintf( F,Am!=0.0?" %6.2lf\n":"\n",Am );
      } fclose( F );
  } }   color( c );
}
//      Изображение легенды в левой части экрана
//
static void Inform()
{ int y,h;
  color( WHITE );  Twindow( 1,1,7,33 ); point p; h=Tv.Th+3; y=32;
  color( GREEN );
  p.x=38,p.y=26;   Tv.Text( North,p,"Depth" );
  p.x=40,p.y+=h*8; Tv.Text( North,p,"Ms"  );
         p.y+=h*7; Tv.Text( North,p,"Its" );
  color( LIGHTGRAY ); It=Mit;
                      Hg=0;
  Tv.Height( 8 );          // settextjustify( LEFT_TEXT,CENTER_TEXT );
  p.x=36,p.y=42;   Tv.Text( East,p,W2D( " 0км" ) );
         p.y+=h;   Tv.Text( East,p,"-33"  ); // 55
         p.y+=h;   Tv.Text( East,p,"-80"  ); // 68
         p.y+=h;   Tv.Text( East,p,"-150" ); // 81
         p.y+=h;   Tv.Text( East,p,"-300" ); // 94
         p.y+=h;   Tv.Text( East,p," ..." ); // 104
  pattern(     EMPTY_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(  WIDE_DOT_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern( CLOSE_DOT_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(     HATCH_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(    XHATCH_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern(   LTSLASH_FILL,CYAN ); bar( 6,y, 30, y+h ); y+=h;
  pattern( FL.pattern,FL.color ); rectangle( 5,34,31, ++y ); //114
  p.y+=h*3;                                                  //136
  Hg=50;                                                     //
  for( Ms=8.0; Ms>5.9; Ms-=0.5 )
  { p.x=16; P=p; Item( 1 ); p.x=40; Tv.Text( East,p,_Fmt( "%3.1lf",Ms ));
    p.y+=h;
  } p.y+=h+h; h+=h/3;                                        //
  Hg=0;                                                      // 206
  for( Ms=7.5,It=9.9; It>-0.9; It-- )                        //
  { if( It<0 )It=-0.01; else
    if( It<1 )It+=0.50;
    p.x=15; P=p; Item( 1 );
    p.x=40; Tv.Text( East,p,_Fmt( It>4?">3":It<0.4?"<0":"%3.1lf",It ) );
    p.y+=h;
    if( It>4 )It=4;
  } Tv.Height( 0 );
}
static char* InfList( int k )
{
 static char Msg[50],Blk[]="    * ";
  Record( k );
  k =sprintf( Msg,"%4d.%02d.%02d ",Year,Month,Day );
  k+=sprintf( Msg+k,Hour>0||Minute>0?"%2d\xFC%02d'":Blk,Hour,Minute );
  k+=sprintf( Msg+k," %4.0lf",Hg );
  k+=sprintf( Msg+k,   Ms>0.0?"  %3.1lf":Blk+1,Ms );
  k+=sprintf( Msg+k,It&&It>Mit?" %4.1lf":Blk+1,It );
  if( Am>0.0 )sprintf( Msg+k,  " %4.1lf",Am );
  Item();
  Item();
 return Msg;
}
static Course InfoLine( int k )
{
 Course ans; Point E; Record( k );
  Item();
  { color( WHITE ); Twindow( 1,-1,72,1 );
    color( CYAN  ); Tprintf( " <%d>  ",k+1 );
    color( BLACK ); Tprintf( "%4d-%s-%d",Year,W2D( Months[Month-1] ),Day );
    if( Minute>0 || Hour>0 )
    { color( CYAN );  Tprintf( " %02d\xFC%02d'",Hour,Minute );
    } color( BLUE );  Tprintf( "  Depth=%1.0lf, Ms=%3.1lf",Hg,Ms );
      color( BROWN );
       E=P;  if( It>Mit )Tprintf( "  I=%3.1lf",It );
             if( Am>0.0 )Tprintf( ", Am=%3.1lf",Am ); Tprintf( " \n" );
    ans=Tv_pause( &E,12,1 );                 //
    if( (ans|0x20)=='h' )ans=Course( 0xFF ); // Для выхода на инструкцию
    Tback();                                 //
  } Item(); return ans;
}
//      Внешний вход в главную программу
//
void SM_list( char* FName )
{
#if 1
 const char * Name[]={ "List   ","Каталог цунами",
                                 "и землетрясений",0 },
            * Text[]={ " F1 "," Легенда данных",
                       " F2 "," Запись данных в файл",
                       " F3 "," Выборка нового каталога",
                       " F4 "," Установка фрагмента",
                       " F5 "," Полная перерисовка",
                       " F6 "," Сортировка по времени",
                       " F7 "," Список событий",0 },
            * Plus[]={ "<Insert> ","отметка события",
                        "<198...> ","быстрый поиск",
                        " <Space> ","сброс подсказок",
                        "  < H >  ","окно помощи",0 };
#else
 const char * Name[]={ "List   ","Seismic and Tsunami",
                                  "     Catalogue",0 },
            * Text[]={ " F1 "," Legend for Active Data",
                        " F2 "," Save Catalogue to File",
                        " F3 "," Retrieve New Catalogue",
                        " F4 "," Data Segment Retrieval",
                        " F5 "," Draw All Events on Map",
                        " F6 "," Sorting for Lines Rise",
                        " F7 "," Display List Catalogue",0 },
            * Plus[]={ "<Insert> ","Mark a Given Point",
                        "<198...> ","Quick Records Find",
                        " <Space> ","Clean Inscriptions",
                        "  < H >  ","This Help List",0 };
#endif
 static int k=0;
 int ans='h',inf=0,ya,xa,ha=0; color( DARKGRAY );
                               getfillsettings( &FL  );
                               getaspectratio(&xa,&ya);
                               Yasp = Real( xa )/ya;
  if( FName )strcpy( FileName,FName );
  if( !N )ans=0xF3; else{ Analyse(); ans=0; }
  for( ;; )
  { if( N>0 && ans<=_Enter )
    switch( ans )
    { case North_West: k=0;   break;
      case South_West: k=N-1; break;
     default:
      if( ans&North_East ){ if( ++k>=N )k=0; } else
      if( ans&South_West )  if( --k<0 )k=N-1;
    } else
    { switch( ans )
      { case 0xF3: Open(); k=0; break;
        case 0xF1: if( inf^=1 )Inform(); else Tback(); break;
        case _Esc: if( inf&1 )Tback(); return;
        case ' ' : break;
       default:
        if( N>0 )switch( ans )
        { case _Ins  : Item( 1 ); break;
          case 0xF2  : Save();    break;
          case 0xF4  : Select();  break;
          case 0xF5  : Draw();    break;
          case 0xF6  : Sorting();
          case 0xF7  : break;
         default: ans='h';
        } else    ans='h';
      }            ha=0;
    }
    if( ans==' ' ){ Ghelp( "" ); ha=0; } else
    if( !ha ){ ha=1; Ghelp( "{%d} F1 Info %s F3 Open%s",
       N,N>0?" F2 Save ":" <H>elp",
         N>0?"  F4 Select  F5 Draw  F6 Sort  F7 List  <Ins>Item":"" );
    }
    if( N>0 && ans==0xF7 )
    { static field f={ 0,3,1,22,0 };
      color( CYAN );  Twindow( 0,1,39,1 );
      color( BLACK ); Tprintf( " Year.mm.dd  Time  Depth  Ms  Its  Amp" );
      color( DARKGRAY );
      ans = Tcase( f,37,InfList,InfoLine,N,k ); Tback();
      if( ans<' ' )ans=0;
    } else
    if( FName ){ FName=0; ans=0xF5; } else
    if( ans==' ' )ans=Tv_getc();      else
    if( (ans|0x20)=='h' )ans=Help( Name,Text,Plus ); else
                         ans=InfoLine( k );
  }
}
