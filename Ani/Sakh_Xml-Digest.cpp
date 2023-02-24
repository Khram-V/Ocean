/**
     ������ ������ �� XML ����� ����������� � ���������
        ������ ���������������������� ����������
                                              Sakh-Xml-digest, ������, 2009
**/
#include "Sakhalin.h"
#include "..\Matrix\XML.h"

#define NextEcoWord   word = Digest.Word();   if( *word ) //if( word )if( word[0] )
#define NextEcoString word = Digest.String(); if( *word ) //if( word )if( word[0] )
                           //
static inXML Digest;       // ������ � ����� XML
                           // - ������������ ����� �������� ������
struct Digest_Element      //
{ char *Key,    // ���� ��� ������ �������� �����������
       *Meas,   // ���������� ����������� ������
       *Name,   // ������� ��������� ���������� ������
       *Title,  // ����������� ��������
      **Short,  // ������� �������� ��� ������� �������
      **Full;   // ������ ��������� �������� �������������� ���������
  float Pdk;     // ����������� �������� ��������� ���������� ������������
  bool Active;  // ����������� ������� ������������ ������ �� ������ ���
};
struct Digest_List
{ Digest_List* Next;        // ��������� ����������
  char *FileName;           // ������ ��� �����
  int N;                    // ������� ���������� ��������� � ������
  Digest_Element *Eco_List; // ���������� ������ ����������
  Digest_List(): Next( NULL ),FileName( NULL ),N( 0 ),Eco_List( NULL ) {}
};
static Digest_List *First = NULL; // ������������ ������ �������� ������������
//
//  ��� ����� ��������, �� ������ �� ��������� ������/������
//    ����������� ������ ��� ����������� ���������� �������
//    !!! ��� ������ �������� �� ������������ ������� �������������
//    � ������ �� ���������� ����� ����������� ������
//
void* Read_Digest( const char* FileName )
{
 Digest_List *EL=First;
 char dName[MAX_PATH];           // ������� ��������� ������ ������������ �����
                  // ������ ����� ���� ���������� ����� ��� �������� ����������
  strcpy( fname( strcpy( dName,FileName ) ),"Digest.xml" );// ����������� �����
  while( EL )
     if( !strcmp( EL->FileName,dName ) )return EL; else EL=EL->Next;
  //
  // ���� ������ ����� ����� ��� ���, �� ��������� ����� ������
  EL = new Digest_List;
 Digest_List &Current=*EL;             // ���������� �����
  //
  // �������� ������� ���� �����������
 char *word;
  if( !(word=Digest.Open( dName ) ) )return NULL;            // �������
  if( strcmp( "Sakhalin-digest",word ) )goto Ext_from_Set; // ��������
  //
  // ���������� ������, ���� ��������� ����� ������ ������������� ��������
 Digest_Element Eco;
  for( ;; )                                // !!! ���������� ������� !!!,
  { if( !(word=Digest.Next()) )break;      // ����� ����� ��������� ������
    if( strcmp( "Eco",word ) )continue;    // ��� ������ ��� ��������� �������
  Find_New_Eco: // ����� ������ �� ����� ��������� ��������� ������ ����� "Eco"
    //
    // �� ������ ������, ����� �������� ������� ������ ��������� �����
    Eco.Name=NULL,   // ������� ��������� ���������� ������
    Eco.Title=NULL,  // ����������� ��������
    Eco.Short=NULL,  // ������� �������� ��� ������� �������
    Eco.Full=NULL;   // ������ ��������� �������� �������������� ���������
    NextEcoWord Eco.Key=strdup( word ); else continue; // ��� �����, ��� ������
    NextEcoWord sscanf( word,"%g",&(Eco.Pdk) ); else Eco.Pdk=0.0;
    NextEcoWord Eco.Meas=strdup( word ); else Eco.Meas=NULL;
    NextEcoWord Eco.Active=word[0]=='+'; else Eco.Active=true;
    for( ;; )
    { word=Digest.Next();         // ����� ���������� ������� �������� ������
      if( !word )break;
      if( !strcmp( "Eco",word ) ){ word[0]=1; break; } // ���� �� ������ �����
      if( !strcmp( "Name",word ) ){ NextEcoString Eco.Name=strdup( word ); }
      if( !strcmp( "Title",word ) ){ NextEcoString Eco.Title=strdup( word ); }
      if( !strcmp( "Digest",word ) ){ NextEcoString Eco.Short=String2TextLn( word ); }
      if( !strcmp( "Description",word ) ){ NextEcoString Eco.Full=String2TextLn( word ); }
    }
    //
    // ������ ���� ��������� ������� ������ ��������
    Current.Eco_List=(Digest_Element*)Allocate
             ( sizeof(Digest_Element)*(++Current.N),Current.Eco_List );
    Current.Eco_List[Current.N-1]=Eco;
    if( !word )break;
    if( word[0]==1 )goto Find_New_Eco;          // ���� ����� ���� ��� �����
  }
Ext_from_Set:
  Digest.Close();                                                               // printf( " -- read %d records",Current.N );
  if( Current.N>0 )  // ����������� ���������� ������
  { if( !First )First=EL; else
    { Digest_List *EP=First;
      while( EP->Next )EP=EP->Next; EP->Next=EL;
    } EL->FileName=strdup( dName );                     // ���� ������ �����
    return EL;
  } delete EL; return NULL;
}
void Map::DigestComment( const char* Key ) // ���������� ������� �� �����������
{ if( !Digest )return;
  Digest_List &Current = *((Digest_List*)Digest);
  for( int i=0; i<Current.N; i++ )                // ������� ������������
  if( !strcmp( Key,Current.Eco_List[i].Key ) )    // ���� ��������� ���� ������
  {      // ��������� �����������, ����, �������� ��� ������ ��� ������ �������
    Comment=Current.Eco_List[i].Short;
    Contain=Current.Eco_List[i].Full;
  }
}
//
//      ��������������� ��������� ������������� ���c���
//
char** String2TextLn( char* str )
{ char **txt=NULL,*Next;
  if( strcut( str )>0 )for( int k=0;; k++ )
  { txt=(char**)Allocate( sizeof(char*)*(k+2),txt );
    if( (Next=strchr( str,'\n' ))!=0 )*Next=0;
    txt[k]=A2O( strdup( str ) );
    if( Next )str=++Next; else break;
  } return txt;
}
#if 0
static char** String2Text( char* str, int width=72 )
{ int i,j,k,l;
  char **txt=NULL;
    for( k=0;; k++ )
    { l=strlen( str );
      for( i=j=0; i<l; i++ )
      { if( str[i]<=' ' )j=i;
        if( i>width ){ str[i=j]=0; break; }
      }
      if( i>0 )
      { (txt=(char**)Allocate( k+2,0,txt ))[k]=strdup( W2D( str ) );
        if( i<l )++i; str+=i;
      } else break;
    }  return txt;
}
#endif

