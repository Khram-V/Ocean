/**
 *   ��������� ����������� � ������ � ����� ��������� ������,
 *    �� ������ ����� ����� �� ��������� ���������� ��������
**/
//     n>0 - ������ ������������ ����� (n) ����, ��� ���� ������ �����������
//           ��������� � ������ ����� ������ (0) �� ������������;
//     n=0 - ������������� � n=-32767;
//     n<0 - ����������� �� �������� strcpy � ����������� �������� �����,
//           �� ��� ���� ����� ���� �������� �� ����� ��� (n+1) ����.
//
//     !(W) - �������� ����������� ����������� ��� ����������
//                  �������������� ���������� �� ������������
//
#include <StdLib.h>
#include <String.h>
#include "..\Type.h"

char* scpy( char* D, const char* _S, int n )
{ char* S=(char*)_S;
  if( n<=0 )       // n<0  - ����������� �� n �������� � �������� ����� ������
  { if( n<0 )n=-n; // n==0 - ������� ����������� ��������� ������
        else n=0x7FFF;
    while( n>0 ){ if( (*D=*S)==0 )break; ++D,++S; --n; } *D=0;
  } else                //
  { bool bl=false;      // n>0 - ����������� ������ n � ���������� ���������
    while( n>0 )        //     � ��� ������� ������� ����� ������
    { if( !bl ){ if( (*D=*S)==0 )bl=true; ++S; }
      if( bl )*D=' '; ++D; --n;         //  �� ������ ����� ����������
  } } return D;                         //  �� ��������� �������� ���� (n<=0)
}                                       //     ��� ������������ �����  (n>0)
char* scat( char* D, const char* S, int n )
{ while( *D!=0 )++D; return scpy( D,S,n );
}
///  ����� ����� ����� ��� ���� ������������� ������� � � ���������� ����������
//
static char* FindName( char* S, int l )
{ while( l>0 ){ if( S[-1]=='\\' || S[-1]==':' )break; --l; --S; } return S;
}                             //
char* fname( const char *_S ) // ������ ����� ������ ����� �����
{ char* S=(char*)_S;          //
  int k=0; while( S[0] )k++,S++; return FindName( S,k );
}                             //
char* sname( char *S )        // �� ��, �� � ���������� ����������
{ int l=0;                    //
  while( S[0] && S[0]!='.' ){ ++l; ++S; } S[0]=0;
  return FindName( S,l );
}
int strcut( char* S, int len )  // ��������� �������� ��������
{                               // !! �������� ���� ( S+len+1 )=0
 char *s=(char*)S+len;          //
  if( !len )while( *s>0 )++len,++s; else *s=0;
            while( len>0 && *--s<=' ' )*s=0,--len; return len;
}
///    �������������� ������ ���������� ����� �����
//
char* fext( char* Name, const char* Ext )
{ if( Ext )
  { int k;
    for( k=strcut( Name )-1; k>=0; k-- )
    { if( Name[k]=='\\' ){ k=-1; break; } else
      if( Name[k]=='.' )break;
    } if( k<0 )k=strlen( Name ); Name[k]='.'; scpy( Name+k+1,Ext );
  } return Name;
}
///   �������������� � ������ ��������� ���������
//
byte W2D( byte c )
{ const byte _Dos[]                                                                  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ={0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7, 0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, // 30: ---�+��� ����---� - 128
   0xC0,0xC1,0xC2,0xC3,0xC4,0xF9,0xC6,0xC7, 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, // 40: L+T+-�� L�T�=+� - 144
   0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7, 0xF0,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, // 50: �TTLL-�+ �----��- - 160
   0xF8,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xFA, 0xF1,0xFC,0xF7,0xFB,0xFC,0xFD,0xFE,0xFF, // 70: �+������ 幷v���� - 176
   0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, // 00: �������� �������� - 192
   0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97, 0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, // 10: �������� �������� - 208
   0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7, 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, // 20: �������� �������� - 224
   0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7, 0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF};// 60: �������� �������� - 240
  return c&0x80?_Dos[c&0x7F]:c;                                                      //                       - 256
}
char* W2D( const char *str )
{ static char *_s=NULL;
    if( _s )free( _s ); _s=strdup( str );
    if( _s )for( char *s=_s; *s; s++ )*s=W2D( *s ); return _s;
//  if( _s ){ char *s=_s; while( *s )*s++ =W2D( *s ); } return _s;
}
#if 0
//#ifndef __MINMAX_DEFINED
//#define __MINMAX_DEFINED
char* ucpy( char*, const char*,int=0 ); // n<0 - n-���� ���� ��� strncpy � �������� �����
char* lcpy( char*, const char*,int=0 ); // �� ������ ����� �������� ��� ������������ �����
char  lcod( char );                     // �������� �������
char  ucod( char );                     // ��� ��������� - ���������
//id   rcod( char*,int=0 );             // Russian Selecting in ASCII-7
//
//   ������ �������� � ��������� � ������� ������ � ������� �������,
//   ���� � ������ ������� ���� �� ���� ������� �����
//
void rcod( char* S, int n )
{ //   char English[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
 //nst char Russess[]="��������������������������"; char b;
 const char Russess[]="��������������������������"; char b;
 int i=0,k;
  ucpy( S,S,n ); if( n<0  )n=-n; else
                 if( n==0 )n=0x7FFF;
  for( k=0; k<n; k++ )
  { if( (b=ucod(S[k]))==0 )break; if( b>='�' &&  b<='�' )++i; } n=k;
  if( i!=0 )
  for( k=0; k<n; k++ ){ i=S[k]-'A'; if( i>=0 && i<26 )S[k]=Russess[i]; }
}
//  ����������� � ��������� � ���������/�������� ����� ��������� ���������
//
char* ucpy( char* D, const char* _S, int n )
{ char* S=scpy( D,_S,n ); while( D!=S )*D++=ucod( D[0] ); return D;
}
char* lcpy( char* D, const char* _S, int n )
{ char* S=scpy( D,_S,n ); while( D!=S )*D++=lcod( D[0] ); return D;
}
//   �������� ������� - ���������, �.�. ������� ����������
//
char lcod( char C )
{ if( (C>='A' && C<='Z')                    //
   || (C>=128 && C<=143) )C+=32; else       //  C>='�' && C<='�'
  if(  C>=144 && C<=159  )C+=80; return C;  //  C>='�' && C<='�'
}                                           //
char ucod( char C )
{ if( (C>='a' && C<='z')                    //
   || (C>=160 && C<=175) )C-=32; else       // C>='�' && C<='�'
  if(  C>=224 && C<=239  )C-=80; return C;  // C>='�' && C<='�'
}                                           //
#endif
