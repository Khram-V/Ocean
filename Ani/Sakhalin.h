#ifndef  __Sakhalin_hxx__
#define  __Sakhalin_hxx__

#include "..\Graphics\Tv_Graph.h"
//
//     Общее описание понятия объект-карта-график
// ___________________________________________________________________________
//
enum{ dtpCross=1,dtpCircle=2,dtpChart=4,dtpDCFile=8 };   // Тип метки на карте
//
//              Элементарные примитивы типа графика, линии, кривой или надписи
//
struct Item     // Самые элементарные примитивы с самого нижнего уровня списка
{ Item *Prev,*Next;          // Ближайшие элементы в списке
  Item(): Prev( 0 ),Next( 0 ){}
 ~Item();                        // Исключение из последовательного списка
};
//     Графические/информационные объекты группируются в блоки (уровни)
//
struct ItemsList
{ Item      *First, *Last;    // Граничные элементы списка
  ItemsList *L_Prev, *L_Next; //
  fixed iActive;              // Ключ состояния и типа слоя
  char *szName;               // Идентификаторы списка
  long nM;                    // Текущая длина списка по Item
       ItemsList( const char *LayerName=NULL ); //, fixed Identify );
      ~ItemsList();
  Item* AddItem( Item* );
  char* AddName( const char *name );
};
//     Собственно геобаза формируется из блоков больших
//     полценных графических или информационных объектов,
//     собираемых в тематические слои, связанные с экранным изображением
//
struct LayerList                     // Слои карты
{ int nL;                            // Число слоев
  ItemsList *L_First, *L_Last,       // Список слоев
            *Enum;                   // Текущий графический слой
  LayerList(): nL( 0 ),L_First( NULL ),L_Last( NULL ),Enum( NULL ){}
 ~LayerList(){ Free(); }             // еще аж без исполнения конструкторов
  ItemsList *AddLayer( ItemsList* ); //
  ItemsList *First();                // Поиск первого из активных уровней
  ItemsList * Next();                //  и продолжение поиска к последующим
  Item *GetActive( long );           // Выбор объекта по номеру среди активных
  void  SwitchLayerState  ( unsigned K   );
  long  Number_of_Objects();             //
  void  Free();                          // Освободить память, занятую слоями
};
//      Описание объектов метафайла с текстовыми данными
//
struct Map: Item        //
{                       // Блок сложных графических объектов,
 LayerList mapLL,       // список слоев для собственно карты
           txtLL;       // и текстовых данных
 bool  MapType;         // 1 - признак географической карты, иначе 0 - рисунок
 HIMG  Small_Image;     // Растровая копия изображения из маленького окошка
 Field _F,F;            // Текущее и полное поле на большом экране
       Map *Previous;   // карта - источник (для возврата)
       Map();           // совершенно чистый и пустой лист
       Map( const char* ); // здесь считываются описатели из файла XML
  void Read();             // этот конструктор открывает все связанные файлы
//    ~Map();
  void Find();                  // Поиск или запрос нового листа
  void Display( bool=true );    // 1-большой, иначе маленький
  void Map_Show();              // с относительными размерами окошка
  void Pts_Show();              // здесь просто расставляются пометки на карте
Map* FindMouse( const Point& ); // поиск объекта по текстовому списку
Map* GetActive( int K ){ return (Map*)txtLL.GetActive( K ); }
private:
  void ReadText( char* ); // Чтение текстового информационно-описательного файла
  void Read_DC1();        // Чтение одной текущей записи информационного файла
  void Read_DC2( char* ); // Чтение картографического файла
  void Read_XML( char* ); // Чтение графики из файла Xml
  void Print();           // запись таблицы eco-графика в файл .html
//
//      Блок метатекстовых описаний
//
 void DigestComment( const char* Key ); // заполнение текстов из справочника
 void *Digest;   // Адрес внешнего справочника в ОП для экологических данных
 float iMark;    // Размер метки объекта на экране
 Point P,S;      // Точка привязки и/или размер картинки (точки связаны рамкой)
public:          //
 fixed iColor,   // Цвет отметки на картинке
     iDispType;  // Вид изображения объекта ( 1-крестик, 2-кружочек...)
 char *DstName,  // Ссылка на имя файла для связанной картинки
      *Title,    // Текст названия объекта
     **Comment,  // Краткое быстрообновляемое определение
     **Contain;  // Текст подробного описания объекта
 void TextWindowView();
 operator Point(){ return ( iDispType&dtpChart ) ? P+S/2.0:P; }
};
//      Общее описание понятия объекта метафайла
//      Привязанное конкретно к .DC2,.DW2 файлам
//
struct Entity: Item                  //
{ short EntityType,                  // Идентификатор объекта для Design-CAD-а
        PntNum,LineType,EntityColor; // Количество опорных точек, цвет и пр.
  float LineWidth,PatternScale;      // Перечень свойств объекта
  Point *PL;                         // Список точек
 virtual void Show()=0;              // Прорисовка, активируемая снизу
          Entity();
         ~Entity();
};
// ____________    Блок объектов связанных с окном терминала
                // _______________________________________________
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
enum{ ptInnerRaise=1, ptInnerLower=2, ptInnerNone=0, // Флаги состояния
      ptOuterRaise=4, ptOuterLower=8, ptOuterNone=0  // кнопок и окошек
    };
#define Mouse_Sector( w )                                           \
  return unsigned( Tv_MouseX()-w.left )<=unsigned( w.right-w.left ) \
      && unsigned( Tv_MouseY()-w.top  )<=unsigned( w.bottom-w.top )
                           //
class WinObject            // Обобщенный экранный объект (окошко)
{          HIMG   _Sci;    // Растровый образ графики в малом окне
           fixed  _Id_,    // Идентификатор связанных с окном данных
                  _Tm;     // Признак трассировки указателем мышки
static WinObject* _First;  // Первый элемент в стековом списке
       WinObject* _Next,   // Следующий
                * _Prev;   //  и предыдущий указатели элементов списка
protected:
   void Save();                                 // Сохранение и востановление
   void Restore();                              // подстилающего изображения
   void Clear( int );                           // затирка цветом фона
   bool MouseThere(){ Mouse_Sector( scr ); }    // для реакции на мышку
public: WinObject( viewporttype&, unsigned Id=0xFFFF, unsigned Tm=0 );
        WinObject* Previous(){ return _Prev; }
       ~WinObject();
       viewporttype scr;              // Размерения экрана в пикселах
static Course Action();               // Ожидание указаний от мыши и клавиатуры
static WinObject* FindNew( unsigned ); // Всегда ищется новейший элемент
};
//     Интерфейс упраления главным графическим элементом
//
class PanelWin: WinObject
{ short PanelFlags,Ledge,Border,InsideColor;
public: viewporttype Win;
           // согласно флажку fl (3-ий параметр):
           // 1 - проценты
           // 0 - x1,y1 - проценты,  x2,y2 - ширина и высота в символах
           // 2 - физич. экранные координаты ( необходимого пространства )
 PanelWin( viewporttype&, fixed id,          // окно и его идентификатор
                          fixed tm,          // признак активности мышки
                          int fl=1, int pf=ptOuterLower,
                          int bw=0, int w=0, int ic=LIGHTGRAY );
  bool MouseThere(){ Mouse_Sector( Win ); }
  void Draw();
};
#undef Mouse_Sector
//
//     Нижняя строка коментариев
//
struct Comment_String: WinObject //
{ struct{ int xb,w; }off[3];     // модуль Sakh_CMN.cpp
  int num;                       //
  Comment_String( viewporttype &f,int,... );
 ~Comment_String();
   void Print( int at, const char* str,... );
   void Draw();
};
//    Просмотр списка названий и выбор графических объектов
//
class ScrollList: WinObject             //
{ int Height,Num,Cur,First,MaxLen;      // Работа со списком станций
  char* (*ItemFun)( int );              //     в правом поле
  void ShowSelect( int );               //
  void ShowList();
public: ScrollList( viewporttype&,long,char*(*)(int) );
 Course Run();
   void SetCurrent( int,int=-1 );
    int GetCurrent(){ return Cur; }
};
class BitButton: public WinObject
{ char* Image;
public: BitButton( viewporttype& _f, const char* _mark, int _id, bool _st=false );
  bool State;                          // признак нажатой: 1 или отжатой кнопки
  void Draw();
};
class CheckButton: public WinObject
{ bool Stat;
 char *Im1,*Im2,*_Msg;
public: CheckButton( viewporttype &f, char *im1, char *im2, char *msg, int id, bool state=0 );
       ~CheckButton();
        void Draw(); void Press();
};
//   Простые процедуры прямого действия для программы Сахалин
//
void Inform_Window( char**,viewporttype&,int c=BLUE, int b=LIGHTCYAN, bool OEM=true );
void Error_Message( const char Msg[],... );
void CrossAt( Map* );        // Особо-попарного вызова отметка на экране
 int Key_Answer();           // Требование безусловного ответа (drw)
                             //
void Panel( int x1,int y1,int x2,int y2,       // fl=1 - Raizing,
            int fl,int bw,int w, int _color ); // fl=2 - Lowering
void* Read_Digest( const char* FileName );
char** String2TextLn( char* str );    // , int width=72 ); -- спрятано в Xmd
//
//     DRAWing tools
//
int PcX( int px ); //{ return ((Tv.mX+1)*px)/100; } // из процентов ширины
int PcY( int py ); //{ return ((Tv.mY+1)*py)/100; } //  и высоты экрана
                                   //
HIMG Save( viewporttype&,HIMG=0 ); // Сохраниение и
void Restore( HIMG, int x, int y); //  восстановление фона
                                   //
void Map_Tv_place( Field &F, const viewporttype &pl, bool MapType ); // Drawing
void Read_GRD( const char*, const Field& );    // Чтение батиметрического файла
void FillPoly( int N, Point *P, int Pattern, int Color );
void LineWide( int color, float width );       // с учетом масштаба экрана
void Cross ( Point &, float Sz );              // --//--
void Circle( Point P, float R  );              // Центр и радиус
void Circle( Point P, Point Q  );              // Центр и точка
void Circle( Point P, Point,Point );           // Центр точка и радиус для дуги
void Geo_Star( int x, int y, int r, float a ); // Картинка с картушкой
void Geo_Page( int x, int y, int size );       // Картинка с книжкой
                                    //
extern Map StartMap,                // самая первая карта в общем списке
          *ActiveMap;               // активная большая или маленькая карта
extern Comment_String *Status_Line; // Строка комментариев
extern char* StartHtml;            // Заготовка имени для таблички Html
extern bool PdkModeView;          // изображение в масштабе ПДК или - как есть
                                 //
inline char* A2O( char *s ){ CharToOemBuff( s,s,strlen( s ) ); return s; }
inline char* O2A( char *s ){ OemToCharBuff( s,s,strlen( s ) ); return s; }
#endif
