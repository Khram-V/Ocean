//
//   Window - ����������� �������� ��� ����� � ���� � ����������
//
#include "Tv_Graph.h"

struct Window                        //
{ Field F; field f;                  // �������� ������� ������������ ���������
  viewporttype b;                    // Tv_port - �������� ���� Window
  int _x,_y,x,y;                     // =( l,t,r,b ) ����� ��� ������� ������
  Window(): F( (Field){ 0,0,1,1 } ),f( (field){ 0,0,0,0,0 } )  // ����������� �
       { _x=b.left,_y=b.top,x=b.right,y=b.bottom; Focus(); }   // �������������
  Field Focus();                                               //
  bool Is( int X,int Y ){ return X>=b.left&&X<=b.right&&Y<=b.bottom&&Y>=b.top; }
  Course Console( int Fl( int,int,bool ) );     // -1 -������� �� ����������� ?
};
//
//     ���������� ��������� ������������ �� ��������
//
class Button: public Window                             // Window ����� �������
{ char *_sign;                                          // ..�������� � �������
public:Button(): Window(),_sign( NULL ){}               // ������ �����������
      ~Button(){ if( _sign )free( _sign ); }            // + ����������
  void Initial( const char* sign, int xt,int yt );      // ��������� �������
  void Place( int xt,int yt );                          // � ���������������
  void Draw();
};
