#ifndef __TF_Topo_Hxx__
#define __TF_Topo_Hxx__
//
//   ������� ���������� ������� ����� � ������� ��������� ����������
//   Amax(-) - ����� �������; Amin(+) - ������ �������
//
void Color_Paint( float **_D,int y,int x,int _y,int _x );
//
//      Tv_topo - �������� ��������� ��������� ����� ������          /91.03.01/
//        Cmd = true  - �� �������� ������
//              false - � �������������� ����������� ������
//
template <class real>
    void Tf_Topo( real **_D,int y,int x,int _y,int _x,bool Cmd=false );
//
//       ���������� ����� ������ � ������ Topo
//          return - ���������� ����� � ������
//
     int Tf_List( Real fl, const int cl ); // cl - ����
     int Tf_List( int NL=0 );              // ��������� ���������� �����
//
//      ����������� ����� �������� ��� ��������� � ������
//
template <typename real>
    void Tf_Line( real **_D,int y,int x,int _y,int _x,Real fl, int clr );
//
//      ��������� ���������� ������� ��������
//
//id Tf_Show( float **_D,int y,int x,int _y,int _x,void(*axis)(bool)=Tv_axis );
void Tf_Color
 ( const int Cplus=LIGHTBLUE,const int Cnull=YELLOW,const int Cminus=GREEN );
#endif
