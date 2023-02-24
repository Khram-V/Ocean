/**
 *  Информационно-картографическая система с использованием расслоенных
 *  метафайлов и результатов мониторинга окружающей среды в формате
 *  расширяемого языка описания данных XML
 *
 *   (с)Русское географическое общество
 *   (c)1996-2009 г, В.Храмушин, Сахалинский государственный университет,
 *         лаборатория вычислительной гидромеханики и морских исследований
 *   (с)В.Савельев, Дальневосточный государственный технический университет
 *
 *   <<  Главный модуль программы  >>
 *                                              --enable-auto-import
 *                                              -static-libstdc++
                                                -Wl,-enable-auto-import
**/
static const char *HelpStrings[] =
    {  "",
       "        Русское географическое общество",
       "             Сахалинское отделение",
       "          Секция морских исследований",
       "",
       " Sakhalin 3.2.1ж,",
       "        графическая справочно-информационная система ",
       "",
       "                объединяет чертежи, карты, рисунки и",
       "                   справочно-тематическую информацию",
       "                    с помошью гипертекстовых связей",
       "",
       " <L-btn>, (Enter) - считывание и изображение графики",
       " <LL-btn>,(Space) - просмотр описательной информации",
//     "  <Shift>+(Space) - с возможностью ее редактирования",
       " <R-btn и сдвиг>  - прямоугольник масштабирования",
       " <L-btn и сдвиг>  - сдвиг рамки на маленьком рисунке",
       "",
       " <Ctrl+L-btn> - новая точка на ломаной для дистанции",
//     " <ScrollLock> - фиксация режима добавления точек",
       "",
//     "             <F2> - запись (обновление) информации",
       "             <F3> - считывание всех видимых ссылок",
//     "       <Shift-F3> - пересмотр всех доступных ссылок",
       "             <F7> - список всех активных объектов",
       "         (Escape) - возврат к предыдущей картинке",
       "    (F10),<alt-X> - завершение программы",
       "",
       "                                       (c)2010.09.08",
       "",0
    };
#include "Sakhalin.h"
#include "..\Graphics\_Small_font.c++"
//
//    Интерфейс управления
//
enum{ idFileOpen=256,      // первые 255 отметок за слоями Design-CAD-а
      idExit,              // кнопка выхода из программы
      idNoZoom,            // кнопка восстановления исходного масштаба
      idSelMapLayer,       // кнопочка картинки
      idSelObjLayer,       // кнопочка текстовых расслоений
      idMainMap,           // --//-- перехода к исходному рисунку
      idPrevMap,           // --//-- возврата к предыдущему
      idObjBrowse,         // --//-- - вызов списка заголовков метатекста
      idPDKabs,            // переключение масштаба к ПДК - абсолют
      idHtml,              // кнопочка для вызова таблички в Html
      __FullMap,           // идентификатор полного экрана
      __PictMap,           // иеднтификатор маленькой картинки
      __WinList,           // окно списка уровней для картинки
      __WinText            // --//-- краткой текстовой информации
    };                                  //
const  unsigned PicLayers=0x4000,       // 16к - признак расслоения рисунка
                TxtLayers=0x2000,       //  8к - признак расслоения метатекста
                TxtBrowse=0x8000;       // 32к - признак переключения к списку
static unsigned LayersType=PicLayers;   //

Map  StartMap;               /// Здесь всегда адрес исходного рисунка MainName
Map* ActiveMap=NULL;         /// Адрес рисунка, изображенного на экране
static Map* RefMap=NULL;     /// Адрес рисунка по активной ссылке
static PanelWin  *Win_Texts =0, /// Нижняя панель с кратким описанием
                 *Win_Layers=0; /// правая - для списка слоев
static BitButton *But_LD = 0,    //  Кнопочка для списка граф.слоев
                 *But_LT = 0;    //  --//-- слоев гипертекстового файла
                                 //
extern char __ExitBtn[],__MapLayerBtn[],__CheckBtn[],
                        __TxtLayerBtn[],__UncheckBtn[];
//  __________________________________________________
//          Вывод подсказки
//
static void Sakhalin_Help()
{ static viewporttype hF={ 0,0,Tv.mX-1,Tv.mY-Tv.Th-11,1 };
  Inform_Window( (char**)HelpStrings,hF,YELLOW,BLUE,false );
}
//     Графическое окно с изображением картинки
//
static struct MapView: PanelWin
{ Field F; Map *MVi;                    // реальная разметка графического поля
  MapView( viewporttype &f, unsigned id )
  : PanelWin( f,id,1,2,ptInnerLower|ptOuterRaise,1,1,WHITE ),MVi( NULL ){}
 void Draw();                           // 1 - сектор; 0 - весь лист (!+Dto+!)
 void SetView( bool );                  //
}    *FullMap=NULL,                     // Большая
     *PictMap=NULL;                     // и маленькая картинки
//
//  Удаление объектов - кнопок выборки слоев из списка "горячих" точек.
//      Так как при этом удаляется и сам объект,
//      то происходит его стрирание и на экране.
//
static int CheckNum=0;  // Количество кнопок из списка уровней
static WinObject*       //
       CheckFirst=NULL; // Первый элемент в списке кнопочек
                        //
static void DeleteLayerChecks()
{ for( ; CheckFirst && CheckNum>0; CheckNum-- )
    { WinObject *W=CheckFirst->Previous(); delete CheckFirst; CheckFirst=W;
    } CheckFirst=NULL;
      CheckNum=0;
  if( But_LD ){ delete But_LD; But_LD=NULL; }
  if( But_LT ){ delete But_LT; But_LT=NULL; } Win_Layers->Draw(); // перетирка
}
//      Сканируются слои метафайла и для содержащих хоть один объект
//      создается кнопка выборки и помещается в список "горячих" точек
//
static void InitMetaLayers( const unsigned Type )
{ if( Type&TxtBrowse )return;           //
    else DeleteLayerChecks();           // Контроль за управлением списком
 WinObject *Z; LayersType=Type;         //
 LayerList &M=(Type==PicLayers) ? ActiveMap->mapLL
                                : ActiveMap->txtLL;
 viewporttype &W=Win_Layers->Win,
             Bt={ W.left+6,W.top+30,W.left+26,W.top+50,1 },
             BF={ PcX(70)+11,PcY(34)+7,0,0,1 };
  But_LD=new BitButton( BF,__MapLayerBtn,idSelMapLayer ); BF.left=But_LD->scr.right+Tv.Th-6;
  But_LT=new BitButton( BF,__TxtLayerBtn,idSelObjLayer );
  if( Type==PicLayers ){ But_LD->State=true; But_LD->Draw(); } else
                       { But_LT->State=true; But_LT->Draw(); }
 unsigned Key=0;
 ItemsList *L=M.L_First;
  for( ; L; L=L->L_Next,Key++ )
  { Z = new CheckButton( Bt,__CheckBtn,__UncheckBtn,
                        !L->szName?string( "---" ):L->szName,
                         Key|Type, L->iActive );
//                       L->Key|Type, L->iActive );
    if( !CheckNum )CheckFirst=Z; ++CheckNum;
    if( (Bt.bottom+=22)>=W.bottom )break;
         Bt.top+=22;
  }
}
#include "Sakh_Map.cpp"
//
///   Прорисовка графического изображения на экране ЭВМ
///    1) Для маленькой картинки предусмотрено сохраниеие растрового поля,
///       которое при повторном изображении просто сбрасывается на экран.
///    2) Графические файлы удаляются из оперативной памяти,
///       кроме исходного и текущего (пока это делается без кэширования)
//
void MapView::SetView( bool W )
{ F=W?MVi->_F:MVi->F; Map_Tv_place( F,Win,MVi->MapType ); // это в Sakh_Drawing
}
void MapView::Draw()                    //
{   PanelWin::Draw();                   // полная расчистка графического окна
//if( MVi->MapType )
   if( this==PictMap )
    if( MVi->Small_Image )
      { Tv_PutImage( Win.left,Win.top,MVi->Small_Image ); return;
      }
  SetView( this!=PictMap );
//if( this==FullMap )
  if( MVi->MapType )Read_GRD( MVi->DstName,F ); /// пока это единственное
                    MVi->Map_Show();            /// обращение к рисованию карты
//if( MVi->MapType )
   if( this==PictMap )
    if( !MVi->Small_Image )
     MVi->Small_Image=Tv_GetImage( Win.left,Win.top,Win.right,Win.bottom );
}
/**
 *   << Главная программа, запускаемая для подготовки графики >>
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 **/
const char *Title="Сахалинские карты и данные наблюдений за состоянием окружающей среды";
static void Initial_Files_and_Windows()
{ int i;
 char *MainName=(char*)"Sakhalin.dc2";
  for( i=0; i<int( sizeof( HelpStrings )/sizeof( *HelpStrings ) )-1; i++ )
    { if( i==12 )i=sizeof( HelpStrings )/sizeof( *HelpStrings )-5;
      printf( "%s\n",W2D( HelpStrings[i] ) );
    }                      //
  Tv_Graphics_Start();     // Оформление пустого экрана с исходными окошками
  setwindowtitle( Title ); //
// ( "Сахалинские карты и данные наблюдений за состоянием окружающей среды" );
  Tv_Start_Mouse();
 const int WType=ptInnerLower|ptOuterRaise;
 static viewporttype                                  //
     CF={       0,Tv.mY-Tv.Th-12,Tv.mX,Tv.mY   ,1 },  // Комментарий
//   FF={       0,Tv.Th+3,    PcX(70),PcY(80)  ,1 },  //
//   PF={ PcX(70)+1,Tv.Th+3,  Tv.mX,  PcY(36)-1,1 },  //
     FF={       0,0,          PcX(70),PcY(80)  ,1 },  // для карты
     PF={ PcX(70)+1,0,        Tv.mX,  PcY(34)-1,1 },  // для мелкой
     TF={         0,PcY(80)+1,PcX(70),Tv.mY-Tv.Th-11,1 },
     LF={ PcX(70)+1,PcY(34),  Tv.mX,  Tv.mY-Tv.Th-11,1 };
//   EF={ Tv.mX-Tv.Tw-8,0,0,0 };
//   pattern( SOLID_FILL,BLUE ); bar( 0,0,Tv.mX+1,Tv.Th+2 );
//   color( YELLOW );     Tv.Text( South,Tv.mX/2,0,"Сахалин и Курилы" );
//   color( BLACK  );        line( 1,Tv.Th+2,Tv.mX+1,Tv.Th+2 );

  Status_Line=new Comment_String ( CF,3, 0,53, 54,8, 63,15 );
  FullMap    =new MapView ( FF,__FullMap ); //,2,WType,1,1,WHITE );
  PictMap    =new MapView ( PF,__PictMap ); //,2,WType,1,1,WHITE );
  Win_Layers =new PanelWin( LF,__WinList,0,2,WType,1 );
  Win_Texts  =new PanelWin( TF,__WinText,0,2,WType,1 );
  //          new BitButton( EF,"Х.",idExit );          //  ^- толщины рамок
  //
  Tv_SetCursor( 4*Tv.mX/5,Tv.mY/3 );
  for( i=1; i<_ac_; i++ )
   if( *_av_[i]!='-' && *_av_[i]!='/' ){ MainName=_av_[i]; break; }
  if( _ac_>1 )MainName=_av_[1];
  StartMap.DstName=MainName;
  StartMap.Read(); // strdup( MainName ) ); // Отсутствие списка слоев
  if( !(StartMap.mapLL.nL) )exit( 1 );      // означает отсутствие графики
  StartMap.Title=(char*)""; //"\"Sakhalin\" - Geography System";
  { static char* Comm[]=
    { (char*)"         Russian Geographical Society",
      (char*)"           Sakhalin State University",
      (char*)"Coputational Fluid Mechanics and Marine Researches lab.",(char*)"",MainName,0
    }; StartMap.Comment=Comm;
  }
  StartMap.Display();                      // и завершение программы
  RefMap=&StartMap;
  ObjectInfo();
}
static void ReRead_for_All_Pictures( Map &Mc )
{ if( Mc.MapType )
  for( ItemsList *L=Mc.txtLL.L_First; L; L=L->L_Next )if( L->iActive )
  { for( Item *M=L->First; M; M=M->Next )
    { Map *T=(Map*)M;
      if( T )T->Find(); //if( T->DstName )Map::Find( T,0 );
  } }
}                               //
Course WinObject::Action()      // Это просто расширенный Getc, действует
{       Ctrl_Line( 1 );         // аналогично, только при нажатии мышки
 Course Key=Tv_getc();          // возвращает идентификатор нажатого объекта
        Ctrl_Line( 0 );         //
  if( Key==_MouseMoved )return _MouseMoved;
  if( Key==_MousePressed )
  { for( WinObject *W=_First; W; W=W->_Next )
     if( W->_Tm )
      if( W->MouseThere() )return Course( W->_Id_ );
  } return Key;
}
///    Основная программа для выполнения общих команд управления
///         изображением на экране (для всех картинок)
 //      _______________________________________________
 //      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static bool Map_Interpreter( int& Key )
{
  if( PictMap->MouseThere() )PictMap->SetView( 0 );
                       else  FullMap->SetView( 1 );
  switch( Key )
  { case _MouseMoved: PutLocation(); break; // непрерывная подглядка за мышкой
    case _Esc     :if( Ctrl_Del( ) ) break;
    case idPrevMap:if( ActiveMap )
                   if( ActiveMap->Previous )
                     { ActiveMap->Previous->Display(); RefMap=0; } break;
    case idMainMap: StartMap.Display(); RefMap=0; break;
    case __PictMap:if( Ctrl_Add() )break;
                   if( Tv_MouseState==_MouseRightPress )
                     { if( ZoomOnFull( __PictMap ) )RefMap=0; break;
                     }     ZoomOnSmall();           RefMap=0; break;
    case __FullMap:if( Ctrl_Add() )break;
                   if( Tv_MouseState==_MouseRightPress )
                     if( ZoomOnFull( __FullMap ) )RefMap=0;
                   if( Tv_MouseState&_MouseLeftPress )
    case _Enter:     { if( RefMap )RefMap->Find();
                     } else
                   if( Tv_MouseState&_MouseLeftDblClick )
    case _Space:   if( RefMap )RefMap->TextWindowView(); break;
    case idHtml:   system( StartHtml ); break;
    case idPDKabs: PdkModeView^=true;      // переключение режима ПДК - абсолют
                   ActiveMap->_F.Ly=0;     // запрос нового масштабирования
                   FullMap->Draw();
//                 PictMap->Draw();
                   PutLocation(); break;             //
    case idNoZoom: XOR_small();                      // Переход
                   FullMap->MVi->_F=FullMap->MVi->F; // к исходному масштабу
                   FullMap->Draw();                  //
                   XOR_small(); break;               //
    case 0xF1: Sakhalin_Help(); break;               // Чтение всех картинок
    case 0xF3: RefMap=0;                             // с запоминанием их
//       if( Tv_getk()&SHIFT )                       // растровых изображений
//       for( Map *M=&Map::FirstMap; M; M=M->Next )
//       { if( M->MapType )ReRead_for_All_Pictures( *M );
//       }  else
         ReRead_for_All_Pictures( *ActiveMap );
         Restore_Small_Picture(); break;
    case idExit: Key=0xFA; break;
   default: return true;
  }
  if( Ctrl )Status_Line->Print
           ( 1,"%5.1f%s",Ctrl_Distance(),ActiveMap->MapType?"км":"уе" );
  return false;
}
//
//  Возврат названия объекта по индексу при виртуальном вызове элементов списка
//
static char* GetItemText( int n ){ return ActiveMap->GetActive( n )->Title; }
//
//  Выдача списка объектов в выделенных слоях и работа со списком
//
static void BrowseObjects( int &_KeyExt )
{
 int Key; static int k=0; Map *D=NULL,*T=NULL;
  for(;;)
  { LayerList &MT=ActiveMap->txtLL; DeleteLayerChecks();
    long BrowseNum=MT.Number_of_Objects();
    ScrollList SL( Win_Layers->Win,BrowseNum,GetItemText );
               SL.SetCurrent( k );
    if( RefMap )RefMap=0;
    do
    { if( !BrowseNum )Key=SL.Run(); else
        { T=ActiveMap->GetActive( k=SL.GetCurrent() );
          if( RefMap!=T ){ RefMap=T; ObjectInfo(); RefMap=0; }
          if( !FullMap->MouseThere() )
            { CrossAt( T ); Key=SL.Run(); CrossAt( T ); } else
            {               Key=SL.Run();               }
        }
      if( Key!=_MouseMoved )D=RefMap,RefMap=T; Map_Interpreter( Key );
      if( Key!=_MouseMoved )RefMap=D; else
      if( FullMap->MouseThere() )
        { if( RefMap  )
          { for( int i=0; i<BrowseNum; i++ )
            if( ActiveMap->GetActive( i )==RefMap ) // повторный вызов
              { SL.SetCurrent( k=i ); break; }      // - и переключение
          }
        }
      switch( Key )
      { case idObjBrowse:
        case 0xF7:
        case 0xFA: goto Exit;
      }
    } while( MT.L_First==ActiveMap->txtLL.L_First );
  }
Exit: InitMetaLayers( LayersType&=~TxtBrowse ); _KeyExt=Key;
}
int main()
{ //
 ///    Подготовка и запуск процессов считывания данных и управления графикой
  //    ______________________________________________________________________
  //    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 int Key;
  Tfile_Filter=(char*)"Геобаза карт и текстов Sakhalin (*.dc1)\0*.dc1\0"
                  "Карты в формате DesignCAD (*.dc2,dw2)\0*.dc2;*.dw2\0"
                  "Другие файлы (*.*)\0*.*\0";
  Initial_Files_and_Windows();
  do
  { if( Map_Interpreter( Key=WinObject::Action() ) )
    switch( Key )
    { case idSelMapLayer:
           if( LayersType!=PicLayers )InitMetaLayers( PicLayers ); break;
      case idSelObjLayer:
           if( LayersType!=TxtLayers )InitMetaLayers( TxtLayers ); break;
      case 0xF7:                               //
      case idObjBrowse: LayersType|=TxtBrowse; // переход к списку объектов
           if( ActiveMap->txtLL.Number_of_Objects()>1 )BrowseObjects( Key );
           break;
     default:
      if( Key & (PicLayers|TxtLayers) )         // кнопочка слоев карты
      { if( Key&PicLayers )
          { ActiveMap->mapLL.SwitchLayerState( (~PicLayers) & Key );
            if( !ActiveMap->MapType )ActiveMap->_F.Ly=0.0;
            FullMap->Draw();
          } else
        if( Key&TxtLayers )
            ActiveMap->txtLL.SwitchLayerState( (~TxtLayers) & Key );
            RefMap=0;
        ((CheckButton*)WinObject::FindNew( Key ))->Press();
    } }
  } while( Key!=0xFA );
}

/**    Необходимые доработки
 ++    убрать звездочки при просмотре экологических данных
 ++    ошибка в ссылке на графиках, ссылку надо заблокировать
 --    при вызове F7 сразу после F1 - испорчена подсведка списка
 --    надо заполнить поле "объекты" легендой для метео-графиков
**/
