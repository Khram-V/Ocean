//
//   Window - ����������� �������� ��� ����� � ���� � ����������
//
#include "Tv_Graph.h"

struct Window                        //
{ Field F; field f;                  // �������� ������� ������������ ���������
  viewporttype b;                    // Tv_port - �������� ���� Window
  int _x,_y,x,y;                     // =( l,t,r,b ) ����� ��� ������� ������
  Window();                          // ����������� � �������������
  Field Focus(); bool Is( int X,int Y );
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
