//
//                                      Подраздел модуля Sakh-MET, Feb-23,98
//
//      Описание текущего объекта DesignCad-6.0
//
static int rEntity, rPntNum,rLType,rColor;
static Real rPScale=1.0,rLWidth=1.0,glScale=1.0;
//
// Чтение null-terminated строки из двоичного файла
//
static char* readDC_BinaryString()
{ for( int i=0; ; i++ )if( ( inMsgStr[i]=fgetc(_F_) )==0 )break;
  return inMsgStr;
}
static float DCget(){ float w; fread( &w,1,4,_F_ ); return w; }
//
//      Конструктор-считыватель базового объекта метафайла от DesignCad-a.
//
Entity::Entity()
      : EntityType  ( rEntity ),
        PntNum      ( rPntNum ),
        LineType    ( rLType  ),
        EntityColor ( rColor  ),
        LineWidth   ( rLWidth ),
        PatternScale( rPScale )
{ int i,j;
  PL=(Point*)Allocate( sizeof( Point )*PntNum );
  if( FileType==dtDC_Ascii )
  { for( i=0; i<PntNum; i++ )
    { readStrData( "%lf%lf", &(PL[i].x),&(PL[i].y) ); PL[i]/=glScale;
  } } else
  if( FileType==dtDC_Binary )
  { for( i=0; i<PntNum; i++ )PL[i].x = DCget();
    for( i=0; i<PntNum; i++ )PL[i].y = DCget();
    for( i=0; i<PntNum; i++ )PL[i]/=glScale;
  }
  if( rEntity==4 )
  { for( i=j=1; i<PntNum; i++ )
    { Real l=abs( PL[i]-PL[j-1] ); if( l>0.0 )PL[j++]=PL[i]; } PntNum = j;
} }
Entity::~Entity(){ Allocate( 0,PL ); }

#define LineStyle( color, style, width ) LineWide( color,width )
//
//    Набор ведущих, управляющих и вспомогательных процедур
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Сплайновые кривые
// Строится параметрическое определение кривой на плоскости с помощью двух
// однозначных функций CX,CY с единым параметром: - дробный номер узловой точки
//
class DC_Curve: public Entity
{ Function CX,CY; public: DC_Curve(); virtual void Show();
};
DC_Curve::DC_Curve(): Entity(), CX( rPntNum ),CY( rPntNum )
{ for( int i=0; i<rPntNum; i++ )
  { CY.Y(i)=PL[i].y; // А вот раньше здесь был еще и аргумент, исчислявшийся    if( i )CY[i].x=CX[i].x=L+=abs( PL[i]-PL[i-1] );
    CX.Y(i)=PL[i].x; // как дистанция между интерполируемыми точками            else CY[i].x=CX[i].x=0;
  } CY.SpLine();
    CX.SpLine();
}
void DC_Curve::Show()
{ LineStyle( EntityColor,LineType,LineWidth );    moveto( PL[0] );
  for( Real w=0.1; w<Real( PntNum )+0.05; w+=0.1 )lineto( (Real)CX( w ),(Real)CY( w ) );
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
}
//      Простая линия, составленная из прямых отрезков
//         (аналогичное параметрическое определение)
//
struct DC_Line: Entity{ virtual void Show(); };
void   DC_Line::Show()
{ int i,_n=PntNum;
  if( EntityType!=1 )_n-=2;
  LineStyle( EntityColor,LineType,LineWidth ); moveto( PL[0] );
  for( i=1; i<_n; i++ )
  if( LineType==-1 && (i&1)==0 )moveto( PL[i] );
                      else      lineto( PL[i] );
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
}
//      Метка на карте (пока отработана не полностью)
//
struct DC_Mark: Entity{ virtual void Show(); };
void   DC_Mark::Show()
{ int i;
  LineStyle( EntityColor,LineType,LineWidth );
  moveto( PL[0] );
  for( i=1; i<PntNum-2; i++ )lineto( PL[i] );
  Circle( PL[i-1]+PL[i],PL[i-1]+PL[i+1] );
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
}
//      Эллипс отработан для 3-х и 5-ти точек
//
struct DC_Ellipse: Entity{ virtual void Show(); };
void   DC_Ellipse::Show()
{  int k;
 Real dA=M_PI/24.0;
 Point R={ 1.0,0.0 };
 static Base B;
  LineStyle( EntityColor,LineType,LineWidth );
  if( PntNum<=5 )B.Axis( PL[1]-PL[0],PL[2]-PL[0] ); else
  {              B.Axis( PL[5]-PL[0],PL[6]-PL[0] );
                 R = dir( B.Goint( PL[1]-PL[0] ) );
    if( EntityType!=2 ){ dA=arg( B.Goint( PL[2]-PL[0] )/R );
                         if( dA>0.0 )dA-=M_PI*2.0; dA/=48.0; }
  }                    moveto( PL[0]+B.Goext( R ) );
  for( k=0; k<48; k++ )lineto( PL[0]+B.Goext( R*=polar( dA ) ) );
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
}
//      Окружности и секторы дуг
//
struct DC_Circle: Entity{ virtual void Show(); };
void   DC_Circle::Show()
{ LineStyle( EntityColor,LineType,LineWidth );
  if( PL[1]==PL[2]  )Circle( PL[0],PL[1] );       else
  if( EntityType==5 )Circle( PL[0],PL[2],PL[1] ); else
                     Circle( PL[0],PL[1],PL[2] );
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
}
//    Замкнутый полигон со штриховкой
//    Построение контура и определение способа закраски
//    (17-й объект быть нанесен на карту ранее всех других объектов)
//    !!! способы закраски еще не отработаны
//
class DC_Hatch: public  Entity
{ int pattern;  public: DC_Hatch();
  virtual void Show(){ FillPoly( PntNum,PL,pattern,EntityColor ); }
};
DC_Hatch::DC_Hatch(): Entity()
{ char *p=inputString();
  if( !strcmp( p,"GRASS" ) )pattern=CLOSE_DOT_FILL;
                   else     pattern=LTBKSLASH_FILL;
}
//      Построение объекта - текстовая строка
//
class DC_String: public Entity
{ char *Text;
public: DC_String(){ char *p=inputString(); if( *p )Text=strdup( p ); }
       ~DC_String(){ if( Text )free( Text ); }
  virtual void Show();
};
void DC_String::Show()
{ static Bfont Ft( _Simplex_font,0 ); color( EntityColor );
  if( EntityType!=22 )
  { if( PntNum<3 )Ft.OutText( PL[0],PL[1],Text ); else
    if( PntNum<4 )Ft.OutText( PL[0],PL[1],PL[2],Text ); else
                  Ft.OutText( PL[0],PL[1],PL[2],PL[3],Text );
  } else
  { int l=strlen( Text );
    if( l>0 )
    { static Base B; int k; Real dS,dA=abs( PL[3]-PL[0] );
      Point R,P={ 0,dA },
              Q={ dA,0 };
      B.Axis( Q,P );
      dA=arg( B.Goint( PL[2]-PL[0] )/B.Goint( PL[1]-PL[0] ) )/l;
      if( PL[3]==PL[2] )dA=-dA;
          R = B.Goint( PL[3]-PL[0] );
         dS = dA/fabs( Ft.StrLen( Text )/Real( l ) );
      for( k=0; k<l; k++ )
      { static char s[]=" "; s[0]=Text[k];
        Ft.OutText
         ( PL[0]+B.Goext( R ),
           PL[0]+B.Goext( R*polar( Ft.StrLen( s )*dS ) ),s );
        R*=polar( dA );
} } } }
//
//   Чтение и формирование метафайла расслоенной географической карты,
//      сформированной с помощью чертежного комплекса DesignCAD
//
void Map::Read_DC2( char *name )
{
 ItemsList *Layers=NULL;
 int i,kL=1;
      FileType=GetFileType( name );
  if( FileType==dtNoData )return;
  if( !(_F_=fopen( name,"rb" )) )return;
  if( FileType==dtDC_Ascii )readStrData( "%lf%lf%lf%lf",&F.Jx,&F.Jy,&F.Lx,&F.Ly ); else
  if( FileType==dtDC_Binary )
  { if(   get2( _F_ )==1000)
    {     getw( _F_ ); // - 4 байта
      if( get2( _F_ )==1200 )
      if( get2( _F_ )==16)
      { F.Jx=DCget(); F.Jy=DCget(); F.Lx=DCget(); F.Ly=DCget();
  } } }
  else goto Exit; // Здесь можно встроить .bln
  //
  //    Заготовка для одного пустого графического объекта
  //
   if( mapLL.nL )mapLL.Free();
    Layers =(ItemsList*)Allocate( sizeof( ItemsList )*256 );
   glScale = 1.0;
   for( i=0; i<256; i++ ) //
   { Layers[i].iActive=1; // Без переопределения установлется 1-й уровень
//   Layers[i].Key=i;     //
   } kL=1;
//
//   Выборка и расшифровка заголовка для одного графического примитива
//
  static char __Color[]=  // Простой пересчет (без воздействий на палитру)
   {/*WHITE*/ BLACK,RED,GREEN, BLUE,BROWN,CYAN,MAGENTA,DARKGRAY,LIGHTGRAY,
      LIGHTRED,LIGHTGREEN,LIGHTBLUE,YELLOW,LIGHTCYAN,LIGHTMAGENTA,BLACK
   };
DC_ReadItem:              // Метка цикла поэлементной выборки
  switch( FileType )      // графических примитивов
  {                       //
   case dtDC_Ascii:
      readStrData( "%d%d%lf%lf%d%d",
                 &rEntity,      // Собственно тип объекта
                 &rPntNum,      // Количество точек
                 &rPScale,      // Масштаб для закраски
                 &rLWidth,      // Толщина линии
                 &rLType,       // Тип линии    (0-8)
                 &rColor );     // Цвет объекта (1-16)
      rPScale/=glScale;         //
      rLWidth/=glScale;
      rColor=__Color[ (rColor-1)%15 ]; break;
   case dtDC_Binary:            //
      rEntity=get2( _F_ );      // Тип объекта
          i = get2( _F_ );      // Длина заголовка в байтах
      if( rEntity>=1300 ){ rPntNum=i; break; }
     static int EnS=2;
      if( i>0 )rPntNum=get2( _F_ ); else rPntNum=EnS; EnS=rPntNum;
      if( i>2 )rColor=__Color[ ((get2(_F_)&0x7F)-1)%15 ];
//    if( i>2 )rColor=__Color[ (abs( get2(_F_))-1)%15 ];
      if( i>4 )get2( _F_ );        //
      if( i>6 )get2( _F_ );        // RGB color
      if( i>8 )get2( _F_ );        // Solid ID (2D=0)
      if( i>10)get2( _F_ );        // Group Number
      if( i>12)rLType=get2( _F_ ); //
      if( i>14){ rLWidth=DCget(); i-=2; rLWidth/=glScale; }
      if( i>18){ rPScale=DCget(); i-=4; rPScale/=glScale; }
      while( --i>=16 )getc( _F_ );
    default:;
  }
  if( feof( _F_ ) )goto Exit;
//
//    Выборка описательных данных о представлении графического объекта
//
  switch( rEntity )             // Описание слоев
  {                             //
    case 23: for( i=0; i<rPntNum; i++ )Layers[i].AddName( inputString() ); break;
    case 21: kL=rPntNum;           break;
    case 32: rEntity=0; rPntNum=0; break;
    case 34:                    //
    case 33:                    // Линии после конвертации файлов AutoCAD-а
    case 31: rEntity=1;         //
    case  1: // Line
    case 24: // Arrow
    case 11: // Bezier
                Layers[kL].AddItem( new DC_Line() );    break;
    case  4: // Curve
                Layers[kL].AddItem( new DC_Curve() );   break;
    case 70: // Point Mark
                Layers[kL].AddItem( new DC_Mark() );    break;
    case  2: // Ellipse
    case  7: // Back Ellipse
                Layers[kL].AddItem( new DC_Ellipse() ); break;
    case  5: // Circular or Elliptical Arc
    case 16: // Circle,Circular,Arc
                Layers[kL].AddItem( new DC_Circle() );  break;
    case  3: // Text
    case 15: // Attribute
    case 22: // Text Arc
                Layers[kL].AddItem( new DC_String() );  break;
    case 17: // Hatch
                Layers[kL].AddItem( new DC_Hatch() );   break;
    case 1340:// Названия слоев в двоичном файле
      {       //
       int Bytes=rPntNum;
       long Off=ftell( _F_ );
        if( Bytes>255 )
        { rPntNum=get2( _F_ );
          for( i=0; i<=rPntNum; i++ )
          { char *p=readDC_BinaryString(); if( *p )Layers[i].AddName( p );
        } } fseek( _F_,Off+Bytes,SEEK_SET );
      }     break;                                 //
    case 20: if( FileType==dtDC_Ascii )rPntNum=14; // System parameters
    case 40:                                       //
   default:                                       //
    if( rEntity==41 && FileType==dtDC_Ascii )    // Масштаб в текстовом и
      { readStrData( "%lf",&glScale );          // в двоичном представлении
        F.Jx/=glScale,F.Jy/=glScale,            //
        F.Lx/=glScale,F.Ly/=glScale; --rPntNum;
      }
    if( rEntity==1309 && FileType==dtDC_Binary )
      { glScale=DCget();
        F.Jx/=glScale, F.Jy/=glScale,
        F.Lx/=glScale, F.Ly/=glScale; rPntNum-=4;
      }
    if( rEntity!=20
     && rEntity!=40
     && rEntity!=41
     && rEntity<1300 )Error_Message( "?Entity: %d.[%d]",rEntity,rPntNum );
    {
     Real x,y;
      if( FileType==dtDC_Ascii )
        for( i=0; i<rPntNum; i++ )readStrData( "%lf%lf",&x,&y );   else
      if( rEntity<1300 )
        for( i=0; i<rPntNum; i++ )fread( inMsgStr,1,8,_F_ ); else
        for( i=0; i<rPntNum; i++ )getc( _F_ );
  } }                                 //
  if( !feof( _F_ ) )goto DC_ReadItem; /// Цикл переходов к поэлементной
Exit:                                 ///     выборки всех графических
  fclose( _F_ );                      ///     примитивов до окончания данных
  FileType=dtTextData;                ///     в файле и переключение на
  if( Layers )                        ///     считывание информационного файла
  { for( i=0; i<256; i++ )            /// Перенос непустых слоев
     if( Layers[i].nM>0 )             ///     в связный список
       mapLL.AddLayer( (ItemsList*)
            memcpy( new ItemsList,Layers+i,sizeof( ItemsList ) ) );
    Allocate( 0,Layers );
  } _F=F;
}
