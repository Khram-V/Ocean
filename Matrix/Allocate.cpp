/**
                                              Ver 3.1 /1991.01.19 - 2008.11.09/

        ����� ����� �������� ��������������� ��������
**/
//      ��� ������������� ������ ������������� ���� �� ������� ����,
//          ��� ������������ ������������ �� ������� 16 ����, �����,
//          � ��������� ����� ����� ��������� ����� ����� ���������
//          ������� � ������, � ��� ������ - ���������� �����, ��������������.
//      ���� ��� ������ Allocate ����� ������� ����������� �������,
//          �� ���������� ������ ������������ ����� ���������� ������
//      ���� ����� ������� - ���������� ����� ������,
//          ����� ���������� ����������������� ������, ��� �������
//          ��������� �������� �����������, ����������� - ���������
//      ���� ��� ������������� ���������� ������� � �������� ����� �����
//          ����������� 0, �� �������� ������������� ������ � ����������
//          ����� ������ � ������ ����� �� ������������,
//          �.�  ��� �������� �������� � ���������� ���������� ���������.
//
#include <StdIO.h>
#include <StdLib.h>
                                             //
#define Word 4u                              // ������������ �� ������� 4 ����
#define Align (sizeof( size_t )*Word)        // ��� = 16 ����, ���  - 64 ����
                                             //
static inline void* ALset( size_t *A, const size_t L )
       { *A++ =0; *A++ =0; *A++ =0; *A++ =L; return A;
       }                                           //
size_t Allocate( void* A )                         // �� ������ ����� � ������
{  if( A )return ((size_t*)A)[-1]; else return 0;  //     ��� ����������
}                                                  //     ����� �������
void *Allocate( size_t s, void *A )                 // �������� ������ � ������
{ size_t S=0;                                       //  ++ 4 ����� ������������
  if( s || A )                                      // ����� ��� �������� �����
  { if( !s ){ free( ((size_t*)A)-Word ); A=NULL; } else // ������������� ������                                      //
    { if( !A )A=malloc( s+Align ); else   // ����� ������ ��� �����������������
      { S = ((size_t*)A)[-1];             //  ���������� �������� ����� �������
        if( S!=s )A=realloc( ((size_t*)A)-Word,s+Align ); else return A;
      }
      if( !A ){ printf( "\n\7 No memory %d",s ); exit( 1 ); } else
      { A=ALset( (size_t*)A,s );               // ��������� �����, ������ �����
        while( S<s )((char*)A)[S++]=0;         // ������� � ��������� ������
    } }
  } return A;
}
//     y -     ���������� ����� � ������� (������)
//      y=0 -  ������ �������� �� �������������, � ���� ������� ���������
//     s -     ����� ������ � ������
//      s=0 -  ����� ������ �� ����������, � ������ �� ����������
//
void **Allocate( size_t y, size_t s, void *A )
{ size_t Y=0;
  if( y || A )                              // ��� ��������� ������� ��� ������
  { if( A )                                 // -- ���� ������ ��� ����, ��
    { Y=((size_t*)A)[-1];                   // -- ��� ������ � ���������� �����
      while( Y>y )Allocate( 0,((void**)A)[--Y] );  // ������������ ������ �����
      if( !Y ){ free( ((size_t*)A)-Word ); A=NULL; } } // � ������ ������������
    if( y )                                            // ���������� � ��������
    { if( A!=NULL )((size_t*)A)[-1] *= sizeof( A );    // ��������� ������� �
     ((size_t*)(A=Allocate( y*sizeof( A ),A )))[-1]=y; // �����������������
    }                                                  // ���������� �������
    if( s!=0 )                                         // ����� - ��� ��� ����
    for( Y=0; Y<y; Y++ )((void**)A)[Y]=Allocate( s,((void**)A)[Y] );
  } return (void**)A;
}
