//
//	Curve.Hxx				  /91.04.12/
//	��᪮�쪮 �ணࠬ� ������� "Point" ����஢
//
#ifndef __CURVE_TYPE
#define __CURVE_TYPE
#include "..\Type.Hxx"

class Curve
{				//
 Point *_C;			// ���� ���ᨢ�
 float *_2;			// ���� �ந������� � 㧫�� �������
				//
public:				//
 fixed N;			// �����᭠� ����� �����
 Curve( fixed=0 );		// Constructor
~Curve();			// Destructor
				//
 Curve& allocate( fixed=0 );	// ���।������ �����
 Curve& free();			//
	operator Point* () { return _C;    } // ��אַ� ����� � ������
 Point& operator[]( int k ){ return _C[k]; } // �롮� �窨 �� �������
	find( float );		// ������ ���� ������ ������
 float  operator()( float );	// �������� ��� ������ ���௮����
 Curve& operator+=( Point& );	// ��ࠫ����쭮� ᬥ饭�� �鸞
 void SpLine( float Yx1=1e32,	// ��⨢����� ������-���௮��樨
	      float YxN=1e32	// �ந������� � ��砫� � � ���� ��१��
	    );			//
 void Linear(){ allocate( N ); }// ������ � �������� ���௮��樨
 void Extreme( Field &_Fm );	// �롮� ����६㬮� �� ��室�� �窠�
 float value( float,int=1 );	// �⥯����� ���௮���� �� ���࠭��
};				//
struct Surface			// ����� �ਢ��������� �����孮���
{      Curve _A,		// ��㬥��� ��� ��������� �㭪権
	    *_S;		// ���᮪ ᯫ������� (�������) �ਢ��
       fixed  M,		// ������⢮ ���௮������� �ਢ��
	    _sp;		// �ਧ��� ᯫ��� ���௮��樨
       float dF1,dFN;		// �ந������ �� �ࠩ��� �ਢ��
				//
 Surface( fixed=0 );		// Constructor
~Surface();			// Destructor
 Surface& allocate( fixed=0 );	// ���।������ �����
 Surface& free();		//
 Curve& operator[]( int k ){ return _S[k]; } // �롮� �ਢ�� �� �������
 float  operator()( float,float ); // �������� ��� ᯫ��� ���௮����
 void Linear(){ allocate( M ); }  // ������ � �������� ���௮��樨
 void SpLine( float Yx1=1e32,	 // ��⨢����� ������-���௮��樨
	      float YxN=1e32	// �ந������� � ��砫� � � ����
	    );			// �����孮�� (�� ��ଠ�� � �ਢ�)
 void Extreme( float &Xmin, float &Xmax,   // ��㬥�� �ਢ��
	       float &Ymin, float &Ymax,   // ��ਭ� ������
	       float &Zmin, float &Zmax ); // ���祭�� �㭪権
};
#endif
