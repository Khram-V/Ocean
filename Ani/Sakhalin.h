#ifndef  __Sakhalin_hxx__
#define  __Sakhalin_hxx__

#include "..\Graphics\Tv_Graph.h"
//
//     ����� �������� ������� ������-�����-������
// ___________________________________________________________________________
//
enum{ dtpCross=1,dtpCircle=2,dtpChart=4,dtpDCFile=8 };   // ��� ����� �� �����
//
//              ������������ ��������� ���� �������, �����, ������ ��� �������
//
struct Item     // ����� ������������ ��������� � ������ ������� ������ ������
{ Item *Prev,*Next;          // ��������� �������� � ������
  Item(): Prev( 0 ),Next( 0 ){}
 ~Item();                        // ���������� �� ����������������� ������
};
//     �����������/�������������� ������� ������������ � ����� (������)
//
struct ItemsList
{ Item      *First, *Last;    // ��������� �������� ������
  ItemsList *L_Prev, *L_Next; //
  fixed iActive;              // ���� ��������� � ���� ����
  char *szName;               // �������������� ������
  long nM;                    // ������� ����� ������ �� Item
       ItemsList( const char *LayerName=NULL ); //, fixed Identify );
      ~ItemsList();
  Item* AddItem( Item* );
  char* AddName( const char *name );
};
//     ���������� ������� ����������� �� ������ �������
//     ��������� ����������� ��� �������������� ��������,
//     ���������� � ������������ ����, ��������� � �������� ������������
//
struct LayerList                     // ���� �����
{ int nL;                            // ����� �����
  ItemsList *L_First, *L_Last,       // ������ �����
            *Enum;                   // ������� ����������� ����
  LayerList(): nL( 0 ),L_First( NULL ),L_Last( NULL ),Enum( NULL ){}
 ~LayerList(){ Free(); }             // ��� �� ��� ���������� �������������
  ItemsList *AddLayer( ItemsList* ); //
  ItemsList *First();                // ����� ������� �� �������� �������
  ItemsList * Next();                //  � ����������� ������ � �����������
  Item *GetActive( long );           // ����� ������� �� ������ ����� ��������
  void  SwitchLayerState  ( unsigned K   );
  long  Number_of_Objects();             //
  void  Free();                          // ���������� ������, ������� ������
};
//      �������� �������� ��������� � ���������� �������
//
struct Map: Item        //
{                       // ���� ������� ����������� ��������,
 LayerList mapLL,       // ������ ����� ��� ���������� �����
           txtLL;       // � ��������� ������
 bool  MapType;         // 1 - ������� �������������� �����, ����� 0 - �������
 HIMG  Small_Image;     // ��������� ����� ����������� �� ���������� ������
 Field _F,F;            // ������� � ������ ���� �� ������� ������
       Map *Previous;   // ����� - �������� (��� ��������)
       Map();           // ���������� ������ � ������ ����
       Map( const char* ); // ����� ����������� ��������� �� ����� XML
  void Read();             // ���� ����������� ��������� ��� ��������� �����
//    ~Map();
  void Find();                  // ����� ��� ������ ������ �����
  void Display( bool=true );    // 1-�������, ����� ���������
  void Map_Show();              // � �������������� ��������� ������
  void Pts_Show();              // ����� ������ ������������� ������� �� �����
Map* FindMouse( const Point& ); // ����� ������� �� ���������� ������
Map* GetActive( int K ){ return (Map*)txtLL.GetActive( K ); }
private:
  void ReadText( char* ); // ������ ���������� �������������-������������� �����
  void Read_DC1();        // ������ ����� ������� ������ ��������������� �����
  void Read_DC2( char* ); // ������ ����������������� �����
  void Read_XML( char* ); // ������ ������� �� ����� Xml
  void Print();           // ������ ������� eco-������� � ���� .html
//
//      ���� ������������� ��������
//
 void DigestComment( const char* Key ); // ���������� ������� �� �����������
 void *Digest;   // ����� �������� ����������� � �� ��� ������������� ������
 float iMark;    // ������ ����� ������� �� ������
 Point P,S;      // ����� �������� �/��� ������ �������� (����� ������� ������)
public:          //
 fixed iColor,   // ���� ������� �� ��������
     iDispType;  // ��� ����������� ������� ( 1-�������, 2-��������...)
 char *DstName,  // ������ �� ��� ����� ��� ��������� ��������
      *Title,    // ����� �������� �������
     **Comment,  // ������� ����������������� �����������
     **Contain;  // ����� ���������� �������� �������
 void TextWindowView();
 operator Point(){ return ( iDispType&dtpChart ) ? P+S/2.0:P; }
};
//      ����� �������� ������� ������� ���������
//      ����������� ��������� � .DC2,.DW2 ������
//
struct Entity: Item                  //
{ short EntityType,                  // ������������� ������� ��� Design-CAD-�
        PntNum,LineType,EntityColor; // ���������� ������� �����, ���� � ��.
  float LineWidth,PatternScale;      // �������� ������� �������
  Point *PL;                         // ������ �����
 virtual void Show()=0;              // ����������, ������������ �����
          Entity();
         ~Entity();
};
// ____________    ���� �������� ��������� � ����� ���������
                // _______________________________________________
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
enum{ ptInnerRaise=1, ptInnerLower=2, ptInnerNone=0, // ����� ���������
      ptOuterRaise=4, ptOuterLower=8, ptOuterNone=0  // ������ � ������
    };
#define Mouse_Sector( w )                                           \
  return unsigned( Tv_MouseX()-w.left )<=unsigned( w.right-w.left ) \
      && unsigned( Tv_MouseY()-w.top  )<=unsigned( w.bottom-w.top )
                           //
class WinObject            // ���������� �������� ������ (������)
{          HIMG   _Sci;    // ��������� ����� ������� � ����� ����
           fixed  _Id_,    // ������������� ��������� � ����� ������
                  _Tm;     // ������� ����������� ���������� �����
static WinObject* _First;  // ������ ������� � �������� ������
       WinObject* _Next,   // ���������
                * _Prev;   //  � ���������� ��������� ��������� ������
protected:
   void Save();                                 // ���������� � �������������
   void Restore();                              // ������������� �����������
   void Clear( int );                           // ������� ������ ����
   bool MouseThere(){ Mouse_Sector( scr ); }    // ��� ������� �� �����
public: WinObject( viewporttype&, unsigned Id=0xFFFF, unsigned Tm=0 );
        WinObject* Previous(){ return _Prev; }
       ~WinObject();
       viewporttype scr;              // ���������� ������ � ��������
static Course Action();               // �������� �������� �� ���� � ����������
static WinObject* FindNew( unsigned ); // ������ ������ �������� �������
};
//     ��������� ��������� ������� ����������� ���������
//
class PanelWin: WinObject
{ short PanelFlags,Ledge,Border,InsideColor;
public: viewporttype Win;
           // �������� ������ fl (3-�� ��������):
           // 1 - ��������
           // 0 - x1,y1 - ��������,  x2,y2 - ������ � ������ � ��������
           // 2 - �����. �������� ���������� ( ������������ ������������ )
 PanelWin( viewporttype&, fixed id,          // ���� � ��� �������������
                          fixed tm,          // ������� ���������� �����
                          int fl=1, int pf=ptOuterLower,
                          int bw=0, int w=0, int ic=LIGHTGRAY );
  bool MouseThere(){ Mouse_Sector( Win ); }
  void Draw();
};
#undef Mouse_Sector
//
//     ������ ������ �����������
//
struct Comment_String: WinObject //
{ struct{ int xb,w; }off[3];     // ������ Sakh_CMN.cpp
  int num;                       //
  Comment_String( viewporttype &f,int,... );
 ~Comment_String();
   void Print( int at, const char* str,... );
   void Draw();
};
//    �������� ������ �������� � ����� ����������� ��������
//
class ScrollList: WinObject             //
{ int Height,Num,Cur,First,MaxLen;      // ������ �� ������� �������
  char* (*ItemFun)( int );              //     � ������ ����
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
  bool State;                          // ������� �������: 1 ��� ������� ������
  void Draw();
};
class CheckButton: public WinObject
{ bool Stat;
 char *Im1,*Im2,*_Msg;
public: CheckButton( viewporttype &f, char *im1, char *im2, char *msg, int id, bool state=0 );
       ~CheckButton();
        void Draw(); void Press();
};
//   ������� ��������� ������� �������� ��� ��������� �������
//
void Inform_Window( char**,viewporttype&,int c=BLUE, int b=LIGHTCYAN, bool OEM=true );
void Error_Message( const char Msg[],... );
void CrossAt( Map* );        // �����-��������� ������ ������� �� ������
 int Key_Answer();           // ���������� ������������ ������ (drw)
                             //
void Panel( int x1,int y1,int x2,int y2,       // fl=1 - Raizing,
            int fl,int bw,int w, int _color ); // fl=2 - Lowering
void* Read_Digest( const char* FileName );
char** String2TextLn( char* str );    // , int width=72 ); -- �������� � Xmd
//
//     DRAWing tools
//
int PcX( int px ); //{ return ((Tv.mX+1)*px)/100; } // �� ��������� ������
int PcY( int py ); //{ return ((Tv.mY+1)*py)/100; } //  � ������ ������
                                   //
HIMG Save( viewporttype&,HIMG=0 ); // ����������� �
void Restore( HIMG, int x, int y); //  �������������� ����
                                   //
void Map_Tv_place( Field &F, const viewporttype &pl, bool MapType ); // Drawing
void Read_GRD( const char*, const Field& );    // ������ ���������������� �����
void FillPoly( int N, Point *P, int Pattern, int Color );
void LineWide( int color, float width );       // � ������ �������� ������
void Cross ( Point &, float Sz );              // --//--
void Circle( Point P, float R  );              // ����� � ������
void Circle( Point P, Point Q  );              // ����� � �����
void Circle( Point P, Point,Point );           // ����� ����� � ������ ��� ����
void Geo_Star( int x, int y, int r, float a ); // �������� � ���������
void Geo_Page( int x, int y, int size );       // �������� � �������
                                    //
extern Map StartMap,                // ����� ������ ����� � ����� ������
          *ActiveMap;               // �������� ������� ��� ��������� �����
extern Comment_String *Status_Line; // ������ ������������
extern char* StartHtml;            // ��������� ����� ��� �������� Html
extern bool PdkModeView;          // ����������� � �������� ��� ��� - ��� ����
                                 //
inline char* A2O( char *s ){ CharToOemBuff( s,s,strlen( s ) ); return s; }
inline char* O2A( char *s ){ OemToCharBuff( s,s,strlen( s ) ); return s; }
#endif
