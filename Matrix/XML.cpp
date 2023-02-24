//
//      XML.cpp                                          Ver 0.0  2008.08.26
//
//      ����� �������� ��� ������/������ ������ � �������
//            ������������ ����� �������� ������
//
//      <?xml Khram encoding="Windows-1251" lang="ru" ?>
//
#include <String.h>
#include <StdLib.h>
#include "..\Matrix\XML.h"
//
//      ��� �������� ��� ���������� ������ ����� � ��������� private,
//      �� ��� ����������� ������ ����� ���� �������� �����,
//      �.�. ����� ���� ����� � ����������� ������� ����
//      ������ � ������ �������� �����������������
//
static char inC=' ';                         // ��������� ����������� ������
//
//      �������� ������� ������ ������
//
char* inXML::Ctrs( int pos ) // ����� �� ������ �������� ��� ������� ������� ...
{ if( pos>=lst )str=(char*)realloc( str,lst=pos+1024 );
  if( !str )exit( 1 );
  return str;
}
//      ������ ����� ��������� msk �� ������� �����
//      -- � ������ ������ �� ����� ����� - ��� �����������,
//         �� ��� �� ��������� �������� ���� �� ����������
//
bool inXML::Pass( const char* msk ) //
{ if( File )                        // ... �� ������ ������, ����� ��� Close ...
  { int k=0,l=strlen( msk );        //
    while( !feof( File ) )
    { if( fgetc( File )==msk[k] )k++; else k=0;
      if( k==l )return true;
    } Close();
  } return false;
}
//      ������ ������� ������ �����, ��� ������� ��������� �����������
//      �������� � inC ��� ������������ ������� ��������� ������ �����
//
static char NS[]="";   // ������ ������ - �� �����, ������ �� ������ !!

char* inXML::Word()    //  �� ������ ������ str ��� ��������� 0 � NS
{  int i,k;            //
 bool w=false;         // ������� ������ ������������ ������� �����
  if( !File || !inFind )return NS; // ���� ����� ������, �� �� ������ ��������������
  for( i=k=0; !feof( File ); i++ )        // � ������ ������ ������ �����
  { inC=fgetc( File );                    //   ������ inC ��������� ������
    if( !w )                              //
    { if( inC<=' ' || inC==',' )continue; // ������� ������ ��������� ��������
                           else w=true;   // ������� ������ ������� �����
    }
    if( inC<=' ' || inC==',' )break;         // ������� ���������� �����
    str[k++]=inC;                            //
   (Ctrs( k+1 ))[k]=0;                       // ���������� �����
    if( !inFind && inC=='>' ){ k=0; break; } // ��� ����������� ������
    if( k==2 )                               //
    { if( !strcmp( str,"/>" ) ){ inFind=false; k=0; break; }
      if( !strcmp( str,"</" ) )inFind=false;
  } }
  if( k ){ if( str[k-1]=='>' )str[--k]=0; return str; } else return NS;
}
//
//      ������ ������� ���� ���� �� ������� �����������
//
char* inXML::String()              // �� ������ ������ str ��� ��������� 0 � NS
{ int i,k;                         //
 bool w=false,b=false;             // ������� ������ ������������ ������� �����
  if( !File || !inFind )return NS; // ���� ����� ������, �� �� ������ ��������������
  for( i=k=0; !feof( File ); i++ ) //
  { inC=fgetc( File );            // � ������ ������ ����� ������ inC ��������� ������
    if( !w && inC<=' ' )continue; // ������� ������ ��������� ��������
    else w=true;                  // ������� ������ ������� �����
    if( inC<=' ' && inC!='\n' )   // ����������� 1 ������ ����� �����
      { if( !b ){ inC=' '; b=true; } else continue; } else b=false;
    if( !inFind && inC=='>' )break; // ���������� ����� ��� ����������� ������
    if( k>0 )
    { if( inC=='>' )if( str[k-1]=='/' ){ inFind=false; str[--k]=0; break; }
      if( inC=='/' )if( str[k-1]=='<' ){ inFind=false; str[k-1]=0; break; } //??
    }                    //
    if( inC=='>' )break; // ���������� ������ ��� ����������� ��� ���������� �������
                         // ! !? !? !? !? - ��� ���� ���������, �.�. ����� ��������
    str[k++]=inC;
   (Ctrs( k+1 ))[k]=0;
  }
  if( k )for( k=strlen( str ); k>0; k-- )
          if( str[k-1]>=' ' )break; else str[--k]=0;
  if( k )return str; else return NS;
}
//
//      ������ ����� ������/����� ����� ������ �� ������ �������� �����
//
char* inXML::Next()
{ Start:            //
  if( File )        // ... �� ������ ������, ����� ��� Close ...
  if( Pass( "<" ) ) // ����� ����������� ������ � ����� ������ ������� �� ��� �����
  { inFind=true;    //
    if( Word()[0] )                       // ����� ������� ����� ���������
    { if( !strcmp( str,"!--" ) )          // ������ ������� ���� ������������
      { Pass( "-->" ); inFind=false; goto Start; } // ���� ��� �� ����� �����
      inFind=true;
      return str;
    }
//  if( str[0]!='/' )
//  { inFind=true; return str;
//  } else        // ���� �� ������� ��� ����� �� ���������� ����� ������,
//  { goto Start; // - ������, ������ �������� �� ����� ���������� �����,
//  }             // ������� �������������� �������� �� ���������� ����������
  }
  return NULL;
}
//
//    � ������ ��������� �������� ����� ����������� ��������� Title
//      ����� �������� ������������ � �������� ���������
//
char* inXML::Open( const char *FileName )
{ if( File )Close();
  if( (File=fopen( FileName,"rb" ) )==NULL )return NULL;
  (str=(char*)malloc( lst=2040 ))[0]=0;     // ����������� ��������� ����
  strlwr( fgets( str,6,File ) );            // � �������� �������� �� ����� 2�
  if( strncmp( str,"<?xml",6 ) )goto out0;
  inFind=true;     // ��������������� ���������� �� ����� ���������� � �����
  if( Word()[0] )  // ����������� ������� � ���������� ���������� ���������
  if( Pass( "?>" ) ){ Title=strdup( str ); return Title; }
out0: Close(); return NULL;
}
void inXML::Close()
{ if( File  )fclose( File );   File=NULL;
  if( Title )free( Title );   Title=NULL;  inFind=false;
  if( str   )free( str );       str=NULL;     lst=0;
}
//
//      ����������� ����� ���� � � ���� ����������� ���� ������ ���������
//      - ��� �������� ��������� ���������� ��������
//
static int lsp( const char *param ) //����� ������������ ������� ���������
{ if( param )if( param[0] )return strlen( param ); return 0;
}
static char *ps2( const char *param ) //
{ int l=lsp( param );                 // ������ ��������� ���������� ���������
  if( l>0 )                           //
  { static char *str=NULL; static int ms=0;
    if( l+3>=ms )str=(char*)realloc( str,ms=l+3 );
    sprintf( str," %s ",param ); return str;
  } return (char*)"";                        //
}                                     // ������� �����, ���� - ��� �������
static char *ps1( const char *param ) //
{ char *w=ps2( param ); if( w[0] )w[strlen( w )-1]=0; return w;
}
bool outXML::Create( const char *FileName, //
                     const char *Key,      // ���� ��� ����������� ���� ������
                     const char *CodeXML ) // ��� ������ ����� ����,
{ if( !File )                              //     �.�. ������������ ��������
  if( (File=fopen( FileName,"wt" ) )!=NULL ) //   ������ ����� ������� ��������
  { fprintf( File,"<?xml %s%s ?>",ps2( Key ),CodeXML );
    return true;
  } return false;
}
#include <StdArg.h>
#define str_Fmt    \
  char str[256]=""; \
  if( lsp( Fmt ) ){ va_list V; va_start( V,Fmt ); vsprintf( str,Fmt,V ); va_end( V ); }
//
//      ����� ���������� ���������� ����� �������
//      ��� �������� ��������� � ����� ����������������� ������
//
int outXML::New( const char *Id,        //
                 const char *Fmt, ... ) // ���������� ����� ���� ��� ����� ����������
{ if( File )                            //
  { str_Fmt
    if( mL<=Level )Ident=(char**)realloc( Ident,(mL+=256)*sizeof(*Ident) );
    for( int i=0; i<=Level*2; i++ )fputc( !i?'\n':' ',File );
    fprintf( File,"<%s%s>",Ident[Level]=strdup( Id ),ps1( str ) );
    return ++Level;
  } return -1;
}
//
//      ����� ���������� ���������� ����� �������
//      ��� �������� ��������� � ����� ����������������� ������
//
int outXML::One( const char *Id,        //
                 const char *Fmt, ... ) // ����� ������� ������ ������������
{ if( File )                            // � ������ ����������� � - �����������
  { str_Fmt                             //
    for( int i=0; i<=Level*2; i++ )fputc( !i?'\n':' ',File );
    fprintf( File,"<%s%s />",Id,ps1( str ) );
    return Level;
  } return -1;
}
//
//  ������ � ���� ���������� ����� ������ ������ �� �������
//
int outXML::Put( const char *Fmt, ... )
{ if( File )
  { str_Fmt
    for( int i=0; i<=Level*2+1; i++ )fputc( !i?'\n':' ',File );
    fprintf( File,"%s",str );
    return Level;
  } return -1;
}
int outXML::Com( const char *Fmt, ... )
{ if( File )
  { str_Fmt fprintf( File,"\n <!-- %s -->",str ); return Level;
  } return -1;
}
//
//  ������� ����������� ���������� �� �������,
//    � ��� �������� ������� � �������� �����
//
int outXML::Back()
{ if( File )
  { if( Level>0 )
    { --Level; for( int i=0; i<=Level*2; i++ )fputc( !i?'\n':' ',File );
      fprintf( File, "</%s>",Ident[Level] ); free( Ident[Level] );
    } return Level;
  }   return -1;
}
//
//  ����������� ��� ������ ������, ������� ���������.
//    ���� �� ������������ ��� ������ �� �������,
//    �� ����������� ��� ������ �� ��������,
//    � ��� ���� ���������� �������� ��������
//    (�.�. ����� ����� ������� �� ������� �������)
//
int outXML::Back( const char *Id )
{ int cmp=0;
  if( File )for(;;)
  { if( Level>0 ){ cmp=strcmp( Id,Ident[Level-1] ); Back(); }
    if( !cmp || !Level )return Level;
  } return -1;
}
//
//   ����������� �������� �������� ����� ����������
//   ����� �������� ��� ����� �������� � ������ ������
//
void outXML::Close()
{ if( File ){ Back( "" ); fputc( '\n',File ); fclose( File ); File=NULL; }
}
