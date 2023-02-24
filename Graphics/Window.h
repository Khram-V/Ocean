//
//   Window - минимальна€ подборка дл€ мышки в окне с кнопочками
//
#include "Tv_Graph.h"

struct Window                        //
{ Field F; field f;                  // экранные границы действующего фрагмента
  viewporttype b;                    // Tv_port - разметка окна Window
  int _x,_y,x,y;                     // =( l,t,r,b ) рамка дл€ мышкина окошка
  Window(): F( (Field){ 0,0,1,1 } ),f( (field){ 0,0,0,0,0 } )  // конструктор с
       { _x=b.left,_y=b.top,x=b.right,y=b.bottom; Focus(); }   // приустановкой
  Field Focus();                                               //
  bool Is( int X,int Y ){ return X>=b.left&&X<=b.right&&Y<=b.bottom&&Y>=b.top; }
  Course Console( int Fl( int,int,bool ) );     // -1 -команда на перерисовку ?
};
//
//     ”прощенные процедуры реагировани€ на кнопочки
//
class Button: public Window                             // Window здесь излишне
{ char *_sign;                                          // ..кнопочка с текстом
public:Button(): Window(),_sign( NULL ){}               // пустой конструктор
      ~Button(){ if( _sign )free( _sign ); }            // + деструктор
  void Initial( const char* sign, int xt,int yt );      // установка надписи
  void Place( int xt,int yt );                          // с местоположением
  void Draw();
};
