/**
        Выбока данных из XML файла с материалами регулярных
               гидрометеорологических наблюдений
                                                 Sakh-Xml, май,2008 - июнь,2009
**/
#include "Sakhalin.h"
#include "..\Matrix\XML.h"
#include "..\Matrix\Function.h"

#define NextEcoWord   word = Eco.Word();   if( *word ) //if( word )if( word[0] )
#define NextEcoString word = Eco.String(); if( *word ) //if( word )if( word[0] )
                           //
static inXML Eco;          // доступ к файлу XML
                           //  - расширяемого языка описания данных
static char str[MAX_PATH]; // рабочая текстовая строка ограниченной длины
                           //
struct Meteo_Point         //
{ Event  DataTime;         // точка - указание отсчета времени наблюдения
  float  Min,Mid,Max;      //  Минимальная, контрольная и максимальная величины
};                         //    для зарегистрированных экологических данных
struct Meteo_Series: Item //
{ short PntNum,           // длина считанного ряда метеоданных
        Mask;             // маска типа отсчетов времени в данной записи
                          // 0х00 - данные приведены к среднегодовым значениям
                          // 0х01 - среднемесячные
                          // 0х02 - среднесуточные данные
                          // 0x03 - точное указание даты времени наблюдений
  Meteo_Point *M;         // собственно массив (один ряд) метеоданных
  char *dataKey,          // ключ доступа к справочнику с описанием данных
       *dataName,         // словесное определение экологического параметра
       *dataMeas;         // физическая размерность контролируемой величины
  float Pdk;               // предельно допустимая концентрация
  Meteo_Series( char* );  // здесь должна быть выделена память и считаны данные
  void Draw();            //
  void Extrem();          //
};
//      снова, снова и снова нарушение принципов объектно-ориентированного
//      программирования (дабы не переделывать вечно всю програмулю целиком)
//
      bool PdkModeView=true;  // изображение в масштабе ПДК или - как есть
static int   nMax=0,          // наиболее длинный ряд наблюдений
             sMax=0, sDrw=0,  // общее количество и текущий эко-параметр
             xMask=0;         // маска наиболее точного указания отсчета времени
static Event xMin(0),xMax(0); // временной интервал текущего изображения
static float  yMin=0, yMax=0,  // размах абсолютных значений функции
             yScale=1.0;      // масштаб для изображения графика по ПДК
static double initTime()      //
{ double dT; if( xMask>2 )dT=1.0/60.0;  else       // ?? часы и минуты
             if( xMask>1 )dT=1.0;       else       // ?? сутки + час
             if( xMask>0 )dT=24.0*7.0;  else       // ?? месяцы +неделя
                          dT=24.0*30.0; return dT; // ?? годы + месяц
}
static double deltaTime() // Шкала времени с учетом того, что
{ double dT=xMax-xMin,    // все наблюдения окажутся одномоментными
         iT=initTime(); if( dT<iT )dT=iT*2.0; return dT;
}
static int PdkColor( float Lvl )
{ int c=LIGHTBLUE; if( Lvl>6.0 )c=LIGHTRED;   else
                   if( Lvl>2.0 )c=43;         else // ORANGE - оранжевый
                   if( Lvl>1.2 )c=44;         else // YELLOW - желтый
                   if( Lvl>0.8 )c=LIGHTGREEN; else
                   if( Lvl>0.1 )c=LIGHTCYAN;  color( c ); return c;
}
void Meteo_Series::Draw()
{ if( PntNum<=0 )return;
 double DW=deltaTime()/100.0, // масштаб аргумента по текущим экстремумам
        dw=0.25,sD,W;         //=400 - количество точек аргумента для графики
 int bc=bkcolor( WHITE );
  if( PdkModeView && Pdk>0.0 )sD=Pdk; else sD=1.0;
  if( nMax<2 )                // Если это всего лишь одноразовое наблюдение
  { int x = Tv_x( 100.0/(sMax+1.0) + 100.0*sDrw/(sMax+0.5) ),
        y = Tv_y( (W=M[0].Mid)/sD ),z;
        Tv.Height( (Tv_port.right-Tv_port.left)<640?8:16 );
        if( !Pdk )color( DARKGRAY ); else pattern( INTERLEAVE_FILL,PdkColor( W/Pdk ) );
        setlinestyle( SOLID_LINE,0,2 );
        Tv.Text( North_West,x,y-4,dataKey );
        bar3d( x-12,y,x,Tv_port.bottom,6,1 ); x+=3;
        setlinestyle( DOTTED_LINE,0,THICK_WIDTH );
        if( (W=M[0].Min)>0.0 )
        { z = Tv_y( W/sD );
          if( !Pdk )color( DARKGRAY ); else PdkColor( W/Pdk ); needle( x,y,x,z );
        }
        if( (W=M[0].Max)>0.0 )
        { z = Tv_y( W/sD );
         if( !Pdk )color( DARKGRAY ); else PdkColor( W/Pdk ); needle( x,y,x,z );
        }
        setlinestyle( SOLID_LINE,0,NORM_WIDTH );
        pattern( SOLID_FILL,WHITE ); // bc ??
        Tv.Height( 0 );
        sDrw++;
  } else
  {
   int k,N=PntNum;
   float w,wn;
   Function T( PntNum );
    for( k=0; k<N; k++ )T.X( k )=(M[k].DataTime-xMin )/DW; // -- аргумент
                      w=T.X(0);                      // начальная и
                     wn=T.X(N-1)+dw;                 // конечная точка графика
    //
    //  график минимумов
    //
    for( k=0; k<N; k++ )T.Y( k )=M[k].Min/sD; T.First(); w=T.X( 0 );
                                               moveto( w,T( w ) );
    for( w+=dw; w<wn; w+=dw )
    { float W=T( w ); if( !Pdk )color( DARKGRAY ); else PdkColor( W*sD/Pdk );
      lineto( w,W );
    }
    //  график максимумов
    //
    setlinestyle( DOTTED_LINE,0,NORM_WIDTH );
    for( k=0; k<N; k++ )T.Y( k )=M[k].Max/sD; T.First(); w=T.X( 0 );
                                              moveto( w,T( w ) );
    for( w+=dw; w<wn; w+=dw )
    { float W=T( w ); if( !Pdk )color( DARKGRAY ); else PdkColor( W*sD/Pdk );
      lineto( w,W );
    }
    // график средних значений
    //
    setlinestyle( SOLID_LINE,0,THICK_WIDTH );
    for( k=0; k<N; k++ )T.Y( k )=M[k].Mid/sD; T.First(); w=T.X( 0 );
                                              moveto( w,T( w ) );
    for( w+=dw; w<wn; w+=dw )
    { float W=T( w ); if( !Pdk )color( DARKGRAY ); else PdkColor( W*sD/Pdk );
      lineto( w,W );
    }
    setlinestyle( SOLID_LINE,0,NORM_WIDTH );
    color( DARKGRAY );
    for( k=0; k<N; k++ )circle( Tv_x( T.X(k) ),Tv_y( T.Y(k) ),3 );
  } pattern( bc );
}
/// во втором кривом блоке масштабируется будущее изображение графиков
//
void Meteo_Series::Extrem()
{ if( PdkModeView && Pdk>0.0 )yScale=Pdk; else yScale=1.0;
  if( nMax<PntNum )nMax=PntNum;           // максимальная длина ряда
  if( xMask<Mask  )xMask=Mask;            // наиболее точное указание времени
  if( PntNum>0 )++sMax;                   // количество рядов для графики
  for( int k=0; k<PntNum; k++ )           //
  { Meteo_Point& Mk=M[k];                               //
    if( yMax==yMin )                                    // установка дат
    { xMax=xMin=Mk.DataTime;                            // по первой точке
      yMin=min( Mk.Min/yScale, min( Mk.Mid/yScale,Mk.Max/yScale ) );
      yMax=max( Mk.Max/yScale, max( Mk.Mid/yScale,Mk.Min/yScale ) );
      if( yMax-yMin < 2.0e-6 )yMax=max( yMax,yMin )+1.0e-6,yMin-=1.0e-6;
      if( PdkModeView ){ if( yMax<1.6 )yMax=1.6; if( yMin>0.0 )yMin=0.0; }
    } else
    { xMin=min( xMin,Mk.DataTime );
      xMax=max( xMax,Mk.DataTime );
      yMin=min( yMin,min(Mk.Mid/yScale,min(Mk.Min/yScale,Mk.Max/yScale)));
      yMax=max( yMax,max(Mk.Mid/yScale,max(Mk.Min/yScale,Mk.Max/yScale)));
  } }
}
///  Графический файл:
///     Простая циклическая прорисовка всех слоев и объектов
//   1) Для картографических файлов DesignCAD в первую очередь наноcятся
//      закрашенные полигоны (MetaHatch), чтобы по ним, сверху, нанести
//      все другие контурные-векторные объекты и тексты
//   2) Для графиков экологических наблюдений сначала делается вызов,
//      где определяются текущие экстремумумы функций, затем
//      производтся разметка окна и далее - искусство рисования
//
void Map::Map_Show(){ Item *M; ItemsList *L; int k; float w;
  if( MapType )
  { //Read_GRD();
    TvClip();
    for( k=0; k<2; k++ )                          //
     for( L=mapLL.L_First; L; L=L->L_Next )       // Учитывается двойной
      if( L->iActive )                            // вызов программы,
       for( M=L->First; M; M=M->Next )        // где сначала рисуются
        if( k^(((Entity*)M)->EntityType==17) )    // только закрашенные области
                           ((Entity*)M)->Show();  //
    //
    //  простая расстановка пометок метатекстовых привязок на карте
    //
    for( ItemsList *T=txtLL.L_First; T; T=T->L_Next )if( T->iActive )
      for( Item *M=T->First; M; M=M->Next )((Map*)M)->Pts_Show();
    TvUnClip();
  } else
  { //            Двойное обращение для предварительного вычисления экстремумов
    //
   static Bfont B( _Simplex_font,18 ); static TvMode Save;
   int m,d,y;
    xMax=xMin=0; yMax=yMin=0.0; nMax=sMax=sDrw=xMask=0;          // обход
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive )       // виртуальных
    for( M=L->First; M; M=M->Next )((Meteo_Series*)M)->Extrem(); // слоев
    if( !nMax || !sMax )return;
    //
    //  после просмотра данных надо найти экстремуиы для поля F и _F
    //
    F.Jx=-1.0;   F.Lx=102.0;
    F.Jy=w=yMin; w=(F.Ly=yMax-yMin)*0.8;
    if( nMax==1 )F.Ly*=1.06;
//  if( !Small_Image ||
    if( _F.Ly==0.0 )_F=F; { Field Ft=_F; Tv_place( 0,&Ft ); }
    Save=Tv;
    TvClip();                 //
                              // название прорисовывемого графического объекта
                              //
    k=sprintf( str," %s",Title );       // Одно единственное измерение также
    if( nMax==1 )                       // может быть сделано в разное время
    { xMin.UnPack( m,d,y ); k+=sprintf( str+k," %04d",y );
               if( xMask>0 )k+=sprintf( str+k,"-%02d",m );
               if( xMask>1 )k+=sprintf( str+k,"-%02d",d );
      if( xMax>xMin )
      { xMax.UnPack( m,d,y ); k+=sprintf( str+k," - %04d",y );
                 if( xMask>0 )k+=sprintf( str+k,"-%02d",m );
                 if( xMask>1 )k+=sprintf( str+k,"-%02d",d );
    } }
    //  Горизонтальные линии ПДК и кратные
    //
    color( LIGHTGREEN ); if( PdkModeView )line( (float)0.0,1.0,100.0,1.0 );
    //
    ///       ... и вот только здесь начинается собственно рисование графиков
    //                                                  (и опять с накладками)
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive )
    for( M=L->First; M; M=M->Next )((Meteo_Series*)M)->Draw();
    //
    //                Векртикальные отметки по шкале времени
    //
    if( nMax>1 )    /// Если есть временной ряд, то он должен быть размечен
    {               // это фактически всегда -- т.е. пока только годы и месяцы
     double v,hdt=yMax-yMin,dT=deltaTime(),iT=initTime();
     Event Pt( xMin ); Pt-=iT; Pt.UnPack( m,d,y );

      for(;;)
      { Pt.Pack( ++m,1,y ); color( LIGHTGRAY );
        Pt.UnPack( m,d,y );
        moveto( v=100.0*(Pt-xMin)/dT,yMax );
        if( m==1 ){ color( BLUE ); w=yMax-hdt/16.0; } else
        if( m==7 ){ color( CYAN ); w=yMax-hdt/24.0; } else
                                   w=yMax-hdt/32.0; lineto( v,w );
        if( Pt>=xMax )break;
        if( m==1 )B.OutText( (Point){ v,w-hdt/12 },
                             (Point){ v+4,w },_Fmt( "%04d ",y ) );
      } w-=hdt/8;
    } color( DARKGRAY ); B.Text( 0.0,w,str );
    TvUnClip();
    Tv=Save;
    Print();
} }
/**    Конструктор-считыватель полного блока станционных
               наблюдений экологических параметров
              по программе федерального мониторинга
                 (без анализа полученных данных)
**/
Meteo_Series::Meteo_Series( char *Key ) // Read_XML запускает конструктор,
    : PntNum( 0 ),Mask( 0 ),M( NULL ),  // где формируются и считываются
      dataKey ( NULL ),                 // временных ряды данных мониторинга
      dataName( NULL ),
      dataMeas( NULL ), Pdk( 1.0 )
{ char *word;
    dataKey = strdup( Key );
    sscanf( Eco.Word(),"%g",&Pdk ); // считываение доп. информации из заголовка
    NextEcoWord   { dataMeas=strdup( word ); A2O( dataMeas ); }
    NextEcoString { dataName=strdup( word ); A2O( dataName ); }
  for(;;)                              //
  { word = Eco.Next();                 // простой контроль - с избытком
    if( !word )break;                  // ничего не найдено
    if( !word[0] )break;               // пустая строка
    if(  word[0]=='/' )break;          // конец текущего блока
    if( !strcmp( "Eco",word ) )break;  // новый блок
    if( word[0]>='0' && word[0]<='9' ) // должна быть дата
    { int m=0,d=0,y=0;                 // месяц, день, год
     char *w=NULL;                     // Формат - yyyy.mm.dd-hh:mm'ss"dd
     float h=0.0;                      //
      if( (w=strchr( word,'.' ))!=NULL )  // есть месяц года
      { if( Mask<1 )Mask=1; *w++=0; m=atoi( w );
        if( (w=strchr( w,'.' ))!=NULL )   // есть день месяца
        { if( Mask<2 )Mask=2; *w++=0; d=atoi( w );
          if( (w=strchr( w,'-' ))!=NULL ) // и еще есть часы, минуты и секунды
          { if( Mask<3 )Mask=3; *w++=0; AtoD( w,h );
      } } } y=atoi( word );
      //
      // здесь производится разбор и сохранение значений месяца, дня и времени
      //
      M=(Meteo_Point*)Allocate( (PntNum+1)*sizeof( Meteo_Point ),M );
      M[PntNum].DataTime = Event( julday( m>0?m:1,d>0?d:1,y ),h );              // printf( "\nEco:Word = %s = %d = %d.%02d.%02d:%s - Mask=%d",word,M[PntNum].DataTime.D,y,m,d,DtoA( h ),Mask );
      //
      //  !! здесь надо поставить разборку запятых для месяца, дня и времени
      //
      NextEcoWord M[PntNum].Mid=atof( word ); // здесь необходимо
      NextEcoWord M[PntNum].Min=atof( word ); // разделить поля с запятыми
      NextEcoWord M[PntNum].Max=atof( word );                                   // printf( "\nEco:Word = { %0.3g, %0.3g, %0.3g }",M[PntNum].Mid,M[PntNum].Min,M[PntNum].Max );
      PntNum++;
  } }                                                                           // printf( "\nEco: Exit - PntNum = %d\n",PntNum );
}
//   Чтение и формирование метафайла расслоенной географической карты,
//       сформированной с помощью чертежного комплекса DesignCAD
//
void Map::Read_XML( char *name )
{ char *Next;
//if( (FileType=GetFileType( name ))!=dtXmlData ) return; // Тройной
  if( strcmp( "Sakhalin-data",Eco.Open( name ) ) )return; // контроль
  if( strcmp( "Station",Eco.Next() ) )            return; // файла данных
  //
  //                         Заготовка для одного пустого графического объекта
  if( mapLL.nL )mapLL.Free();                        // расчистка старых слоев
  for( ;; )
  { Next=Eco.Next();
    if( !strcmp( "Data",Next ) )break; else        // чистая пробежка
    if( !Next )return;                             // до признака блока данных
  }
  //                Начало поиска и считываенния реальных экологических данных
  for(;;)
  { Next=Eco.Next();
    if( !Next )break; else            //
    if( !Next[0] )break; else         //  новый слой - числовой ряд наблюдений
    if( !strcmp( "Eco",Next ) )       //
    { ItemsList *L = new ItemsList( Next=Eco.Word() ); //,mapLL.nL );
      L->AddItem( new Meteo_Series( Next ) );
      if( L->nM>0 )mapLL.AddLayer( L );
  } } F.Jx=F.Jy=F.Lx=F.Ly=0.0; _F=F;  // очистка для последующего обновления
}
/**
        Собственно создание, отображение и управление изображением
**/
//      Переход к карте.
//      Если карта не считана, то открыть ее и занести в список,
//      запомнив при этом текущую карту в качестве предыдущей.
//      Затем отобразить карту
//      ~~~~~~~~~~~~~~~~~~~~~~
void Map::Read() // const char *name )          // Подготовка к возможному
{ char *p=strrchr( strcpy( str,DstName ),'.' ); // открытию сопутствующего
  if( strncmp( strlwr( p ),".xml",4 ) )         // тектового описания для
    { Read_DC2( DstName );                      // нового графического файла
      if( p )strcpy( p,".dc1" );               //
      else strcat( str,".dc1" );              // Замена расширения dc2(dw2)-dc1
      ReadText( str );
    } else
    { MapType=false;           // это спец-признак особого масштабирования
      Read_XML( DstName );     // данные гидрометеорологических наблюдений
      Digest=Read_Digest( DstName ); // и лежащего рядом с ней справочника
    }
  Previous=ActiveMap;
  if( mapLL.nL==0 )Error_Message( "Графика не загружена: %s",DstName );
}
///     Чистый текстовый объект для статического заполнеия
 //
Map::Map()
   : mapLL(),txtLL(),MapType( true ), Small_Image( NULL ),Digest( NULL ),
     iMark( 1.0 ),iColor( GREEN^WHITE ),iDispType( 0 ),DstName( NULL ),
     Title( NULL ),Comment( NULL ),Contain( NULL )
{    F.Jx=F.Jy=0.0;
     F.Lx=F.Ly=1.0; _F=F; P=S=0.1;
}
#if 0
Map::~Map()            // !!! здесь надо все снова и снова перепроверить
{ if( &StartMap!=this && EntityType!=-222 ){ char **W;
    if(  Title  )free( Title );
    if( DstName )free( DstName );
    if( Comment ){ W=Comment; while( *W )free( *W++ ); Allocate( 0,Comment ); }
    if( Contain ){ W=Contain; while( *W )free( *W++ ); Allocate( 0,Contain ); }
    if( Small_Image )Tv_FreeImage( Small_Image );
    mapLL.Free();
    txtLL.Free();
} }
#endif
///    Считывание чисто описательной информациии из станционных бюллетеней
 //
Map::Map( const char* XML )
   : mapLL(),txtLL(),MapType( false ), Small_Image( NULL ),Digest( NULL ),
     iMark( 0.02 ),iColor( LIGHTRED ),iDispType( dtpCircle ),
     DstName( NULL ),Title( NULL ),Comment( NULL ),Contain( NULL )
{    P=S=0.1;
     F.Jx=F.Jy=0.0;
     F.Lx=F.Ly=1.0; _F=F;
  //
  //          Чтение текстовых описателей из файла XML в память
  //
 char *Next=Eco.Open( XML ),*word; //int k;
  if( !Next )goto Exto;                           // Тройной
  if( strcmp( "Sakhalin-data",Next ) )goto Exto;  // контроль
  if( strcmp( "Station",Eco.Next() ) )goto Exto;  // файла данных
  Title=strdup( A2O( Eco.String() ) ); strcut( Title );
  DstName=strdup( XML );
  //
  //          Начало поиска и считываенния описателей для экологических данных
  for( ;; )
  { Next=Eco.Next();
    if( !Next )goto Exto; else                 // считывание краткого описания
    if( !strcmp( "Data",Next ) )break; else    // с разбиением на строки
    if( !strcmp( "Digest",Next ) ){ NextEcoString Comment=String2TextLn( word ); } else
    if( !strcmp( "Description",Next ) ){ NextEcoString Contain=String2TextLn( word ); } else
    if( !strcmp( "Point",Next ) )AtoD( Eco.Word(),P.x ),
                                 AtoD( Eco.Word(),P.y );
  } Exto: Eco.Close();
}
//
///     Динамический перебор всех точек для интерактивного доступа к данным
//
//      Надо быть очень внимательным к кривым вызовам, которые при исключении
//      активных слоев могут и совсем не состояться
//
                                 //
Map* Map::FindMouse( const Point &_P )
{ ItemsList *L;
  if( MapType )
  { float d,D=1e30;
    Map *M,*N=NULL;
    for( L=txtLL.L_First; L; L=L->L_Next )if( L->iActive )   // прямой перебор
    for( M=(Map*)L->First; M; M=(Map*)M->Next )          // по списку карт
    if( ( d = abs( (Point)*M - _P ) )<D ){ D=d; N=M; } return N;
  } else                          //
  {
   static Map UniTxt;             // статичный объект для динамичной информации
   Meteo_Series *mSt=NULL;        // Найденная станция для ближайшей точки
   Point mPts=_P,mQts,wP;         // Координаты мышки и ближайшей точки поиска
   Item *M;                       //
   float  mDst=0.0,scX=1.0;            // Минимальная дистанция и сжатие шкалы
    if( &UniTxt.mapLL != &mapLL ) // в качестве основных комментариев
      { UniTxt=*this;             // ставится исходная географическая карта
        UniTxt.DstName=NULL;      //
      }
    if( sMax && nMax )
    {
     double dT=deltaTime();       // !! это второе место перерасчета координат
     double WD=100.0/dT,wD,yS;
     int k=0,m,d,y;
     static Event Cur;
      scX = ( yMax-yMin )/100.0;
      if( nMax==1 )
      { for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) /// поиск здесь !!
        for( M=L->First; M; M=M->Next )
        { Meteo_Series& S=*(Meteo_Series*)M;
          if( S.PntNum<=0 )continue;
          yS=yScale;
          if( PdkModeView && S.Pdk>0.0 )yScale=S.Pdk; else yScale=1.0;
          wP.x = 100.0/( sMax+1.0 ) + 100.0*k/( sMax+0.5 );
          wP.y = S.M[0].Mid/yScale;
          wD = abs( mPts.x-wP.x );
          if( !k || wD<mDst ){ mSt=&S; mQts=wP; mDst=wD; yS=yScale; }
          yScale=yS;
          k++;
        } Cur=mSt->M[0].DataTime;
      } else
      if( nMax>1 )
      { for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) /// поиск здесь !!
        for( M=L->First; M; M=M->Next )
        { /// в первом кривом блоке выполнятся поиск ближайшей к мышке точки
          // ( косвенная подделка под кривизну объектно-сориентированного кода )
         Meteo_Series& S=*(Meteo_Series*)M;
          if( S.PntNum<=0 )continue;    //
          k=0;                          // k=0 - полный просмотр всего ряда
          yS=yScale;                    //
          if( PdkModeView && S.Pdk>0.0 )yScale=S.Pdk; else yScale=1.0;
          if( !mSt )
          { mSt=&S;
            mQts=(Point){ (S.M[0].DataTime-xMin)*WD,S.M[0].Mid/yScale }; k=1;
            mDst=hypot( (mPts.x-mQts.x)*scX,mPts.y-mQts.y );
          }
          for( ; k<S.PntNum; k++ )
          { wP=(Point){ (S.M[k].DataTime-xMin)*WD,S.M[k].Mid/yScale };
            wD=hypot( (mPts.x-wP.x)*scX,mPts.y-wP.y );
            if( wD<mDst ){ mSt=&S; mQts=wP; mDst=wD; yS=yScale; }
          } yScale=yS;
        } Cur = xMin + mPts.x / WD; //* (xMax-xMin)/100.0;
      }   Cur.UnPack( m,d,y );
      sprintf( str,W2D("%s: %s, ПДК=%g [%s], = (%0.4g) - %04d:%02d:%02d "),
       mSt->dataName,mSt->dataKey,mSt->Pdk,mSt->dataMeas,mPts.y*yScale,y,m,d );
      UniTxt.Title=str;        // текущие определения и величины эко-параметра
      UniTxt.P=mQts;
      UniTxt.DigestComment( mSt->dataKey );
    } return &UniTxt;
  }
}
//      Запись текущей таблички с данными на большом экране
//
static char* OA( const char *O )
{ static char *A=NULL; if( A )free( A ); return O2A( A=strdup( O ) );
}
char* StartHtml=NULL;   // Заготовка имени для таблички Html
void Map::Print()
{ if( MapType || !nMax || !sMax )return; else
  {
   FILE *html;
   char *scm;
   short Mask=0;
    if( !StartHtml )
    { strcpy( fname( strcpy( strcpy( str,"Start " )+6,StartMap.DstName ) ),"Sakhalin-table.html" ); //
      StartHtml=strdup( str );
    }
    if( (html=fopen( StartHtml+6,"wt" ))==NULL )return;      // нельзя, так нельзя
    scm=OA( Title );
    fprintf( html,
    "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\n"
    "<title>%s</title>\n</head>\n<body>\n<h2>%s</h2>\n",scm,scm );
   ItemsList *L;
   static Event Start,End;
   bool i=false;                               // i - признак первого прохода
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) // проход по слоям
    { Item *M;
      for( M=L->First; M; M=M->Next )
      { Meteo_Series& S=*(Meteo_Series*)M;
        if( S.Mask>Mask )Mask=S.Mask;
        for( int k=0; k<S.PntNum; k++ )
        { Meteo_Point& P=S.M[k];
          if( !i ){ Start=End=P.DataTime; i=true; } else
          if( End<P.DataTime )End=P.DataTime; else
          if( Start>P.DataTime )Start=P.DataTime;
    } } }
    //
    //  Найдены экстремумы - можно продолжить запись файла
    //
    { int m,d,y;
      Start.UnPack( m,d,y ); fprintf( html,"<p>Период наблюдения: %04d",y );
                 if( Mask>0 )fprintf( html,"-%02d",m );
                 if( Mask>1 )fprintf( html,"-%02d",d );
      End.UnPack( m,d,y );   fprintf( html," - %04d",y );
                 if( Mask>0 )fprintf( html,"-%02d",m );
                 if( Mask>1 )fprintf( html,"-%02d",d );
                             fprintf( html,"</p>\n<p>" );
    }
    if( !Comment )fprintf( html,"Без комментария" ); else
    for( int k=0; Comment[k]; k++ )fprintf( html,"%s<br>\n",OA( Comment[k] ) );
    fprintf( html,"</p>\n<table border=1 cellspacing=0 cellpadding=2>\n<tr>\n\t<td>Ключ</td>\n\t<td>Название</td>\n"
                  "\t<td>ПДК</td>\n\t<td>Среднее</td>\n\t<td>Минимум</td>\n\t<td>Максимум</td>\n</tr>\n" );
    //
    //  Теперь можно заполнить ячейки выходной таблички
    //
    for( L=mapLL.L_First; L; L=L->L_Next )if( L->iActive ) // проход по слоям
    { Item *M;
      for( M=L->First; M; M=M->Next )
      { Meteo_Series& S=*(Meteo_Series*)M;
        fprintf( html,"<tr>\n\t<td>%s</td>\n\t<td>%s</td>\n",S.dataKey,OA( S.dataName ) );
        fprintf( html,      "\t<td>%g %s</td>\n",S.Pdk,OA( S.dataMeas ) );
        i=false;
        float Aver=0,Amax=0,Amin=0;
        for( int k=0; k<S.PntNum; k++ )
        { Meteo_Point& P=S.M[k];
          if( !i ){ Aver=Amax=Amin=P.Mid; i=true; } else
          { Aver+=P.Mid;
            if( Amax<P.Mid )Amax=P.Mid; else
            if( Amin>P.Mid )Amin=P.Mid;
        } }
        fprintf( html,"\t<td>%0.4g</td>\n\t<td>%0.4g</td>\n\t<td>%0.4g</td>\n</tr>\n",Aver/(float)S.PntNum,Amin,Amax );
    } }
    fprintf( html,"</table>\n<p>" );
    if( !Contain )fprintf( html,"... без описания" ); else
      for( int k=0; Contain[k]; k++ )fprintf( html,"%s<br>\n",OA( Contain[k] ) );
    fprintf( html,"</p>\n</body>\n</html>\n" );
    fclose( html );
  }
}
#if 0
<html><head><meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
<title>Название станции</title>
</head>
<body>
<h2>Название станции</h2>
<p>Период наблюдения</p>
<p>Краткое описание</p>
<table border=1 cellspacing=0 cellpadding=2>
<tr>    <td>Ключ</td>
        <td>Название</td>
        <td>ПДК</td>
        <td>Среднее</td>
        <td>Минимум</td>
        <td>Максимум</td>
</tr>
<tr>    <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
</tr>
</table>
<p>Полное описание</p>
</body>
</html>
#endif
