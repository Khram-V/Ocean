//
//      XML.cpp                                          Ver 0.0  2008.08.26
//
//      Пакет процедур для чтения/записи файлов в формате
//            расширенного языка разметки данных
//
//      <?xml Khram encoding="Windows-1251" lang="ru" ?>
//
#include <String.h>
#include <StdLib.h>
#include "..\Matrix\XML.h"
//
//      для рекурсии эти переменные должны войти в подраздел private,
//      но для отладочного режима пусть пока остаются здесь,
//      т.к. может быть лучше и эффективнее держать этот
//      модуль с полным запретом реентерабельности
//
static char inC=' ';                         // последний прочитанный символ
//
//      Контроль текущей длинвы строки
//
char* inXML::Ctrs( int pos ) // столь же пустой контроль для рабочей строчки ...
{ if( pos>=lst )str=(char*)realloc( str,lst=pos+1024 );
  if( !str )exit( 1 );
  return str;
}
//      Прямой поиск подстроки msk во входном файле
//      -- в случае наезда на конец файла - все блокируется,
//         но все же исходного контроля пока не проводится
//
bool inXML::Pass( const char* msk ) //
{ if( File )                        // ... на всякий случай, вдруг был Close ...
  { int k=0,l=strlen( msk );        //
    while( !feof( File ) )
    { if( fgetc( File )==msk[k] )k++; else k=0;
      if( k==l )return true;
    } Close();
  } return false;
}
//      Прямая выборка одного слова, при котором найденный разделитель
//      остается в inC для последующего анализа положения внутри блока
//
static char NS[]="";   // пустая строка - на выход, менять ее нельзя !!

char* inXML::Word()    //  на выходе строка str или константа 0 в NS
{  int i,k;            //
 bool w=false;         // признак начала посимвольной выборки слова
  if( !File || !inFind )return NS; // если стоит запрет, то он просто подтверждается
  for( i=k=0; !feof( File ); i++ )        // в начале поиска нового слова
  { inC=fgetc( File );                    //   старый inC считается чистым
    if( !w )                              //
    { if( inC<=' ' || inC==',' )continue; // перебор пустых начальных символов
                           else w=true;   // признак начала выборки слова
    }
    if( inC<=' ' || inC==',' )break;         // обычное завершение слова
    str[k++]=inC;                            //
   (Ctrs( k+1 ))[k]=0;                       // завершение блока
    if( !inFind && inC=='>' ){ k=0; break; } // без продолжения чтения
    if( k==2 )                               //
    { if( !strcmp( str,"/>" ) ){ inFind=false; k=0; break; }
      if( !strcmp( str,"</" ) )inFind=false;
  } }
  if( k ){ if( str[k-1]=='>' )str[--k]=0; return str; } else return NS;
}
//
//      Прямая выборка всех слов до первого разделителя
//
char* inXML::String()              // на выходе строка str или константа 0 в NS
{ int i,k;                         //
 bool w=false,b=false;             // признак начала посимвольной выборки слова
  if( !File || !inFind )return NS; // если стоит запрет, то он просто подтверждается
  for( i=k=0; !feof( File ); i++ ) //
  { inC=fgetc( File );            // в начале поиска слова старый inC считается чистым
    if( !w && inC<=' ' )continue; // перебор пустых начальных символов
    else w=true;                  // признак начала выборки слова
    if( inC<=' ' && inC!='\n' )   // сохраняется 1 символ конца строк
      { if( !b ){ inC=' '; b=true; } else continue; } else b=false;
    if( !inFind && inC=='>' )break; // завершение блока без продолжения чтения
    if( k>0 )
    { if( inC=='>' )if( str[k-1]=='/' ){ inFind=false; str[--k]=0; break; }
      if( inC=='/' )if( str[k-1]=='<' ){ inFind=false; str[k-1]=0; break; } //??
    }                    //
    if( inC=='>' )break; // завершение строки без последствий для дальнейшей выборкм
                         // ! !? !? !? !? - это надо продумать, т.к. хвост остается
    str[k++]=inC;
   (Ctrs( k+1 ))[k]=0;
  }
  if( k )for( k=strlen( str ); k>0; k-- )
          if( str[k-1]>=' ' )break; else str[--k]=0;
  if( k )return str; else return NS;
}
//
//      Прямой поиск начала/конца блока данных на выходе название блока
//
char* inXML::Next()
{ Start:            //
  if( File )        // ... на всякий случай, вдруг был Close ...
  if( Pass( "<" ) ) // найти открывающую скобку и взять первое стоящее за ней слово
  { inFind=true;    //
    if( Word()[0] )                       // здесь выбрано слово заголовка
    { if( !strcmp( str,"!--" ) )          // грубый пропуск всех комментариев
      { Pass( "-->" ); inFind=false; goto Start; } // если это не конец блока
      inFind=true;
      return str;
    }
//  if( str[0]!='/' )
//  { inFind=true; return str;
//  } else        // надо бы закрыть все блоки до найденного здесь уровня,
//  { goto Start; // - однако, просто перейдем на поиск следующего блока,
//  }             // оставив синтаксический контроль за вызывающей программой
  }
  return NULL;
}
//
//    В случае успешного открытия файла заполняется заголовок Title
//      адрес которого возвращается в качестве параметра
//
char* inXML::Open( const char *FileName )
{ if( File )Close();
  if( (File=fopen( FileName,"rb" ) )==NULL )return NULL;
  (str=(char*)malloc( lst=2040 ))[0]=0;     // открывается текстовый файл
  strlwr( fgets( str,6,File ) );            // с буферной строчкой не более 2к
  if( strncmp( str,"<?xml",6 ) )goto out0;
  inFind=true;     // устанавливается разрешение на поиск параметров и затем
  if( Word()[0] )  // проверяется наличие и правильное оформление заголовка
  if( Pass( "?>" ) ){ Title=strdup( str ); return Title; }
out0: Close(); return NULL;
}
void inXML::Close()
{ if( File  )fclose( File );   File=NULL;
  if( Title )free( Title );   Title=NULL;  inFind=false;
  if( str   )free( str );       str=NULL;     lst=0;
}
//
//      Открывается новый файл и в него вписывается одна строка заголовка
//      - все элементы структуры метеданных обнулены
//
static int lsp( const char *param ) //здесь определяется наличие параметра
{ if( param )if( param[0] )return strlen( param ); return 0;
}
static char *ps2( const char *param ) //
{ int l=lsp( param );                 // строка параметра окружается пробелами
  if( l>0 )                           //
  { static char *str=NULL; static int ms=0;
    if( l+3>=ms )str=(char*)realloc( str,ms=l+3 );
    sprintf( str," %s ",param ); return str;
  } return (char*)"";                        //
}                                     // изрядно грубо, зато - без фокусов
static char *ps1( const char *param ) //
{ char *w=ps2( param ); if( w[0] )w[strlen( w )-1]=0; return w;
}
bool outXML::Create( const char *FileName, //
                     const char *Key,      // Ключ для опознавания типа данных
                     const char *CodeXML ) // Это именно новый файл,
{ if( !File )                              //     т.е. пересоздание возможно
  if( (File=fopen( FileName,"wt" ) )!=NULL ) //   только после полного закрытия
  { fprintf( File,"<?xml %s%s ?>",ps2( Key ),CodeXML );
    return true;
  } return false;
}
#include <StdArg.h>
#define str_Fmt    \
  char str[256]=""; \
  if( lsp( Fmt ) ){ va_list V; va_start( V,Fmt ); vsprintf( str,Fmt,V ); va_end( V ); }
//
//      Здесь рекурсивно включается новый уровень
//      имя которого заносится в конец последовательного списка
//
int outXML::New( const char *Id,        //
                 const char *Fmt, ... ) // Собственно здесь пока все очень простенько
{ if( File )                            //
  { str_Fmt
    if( mL<=Level )Ident=(char**)realloc( Ident,(mL+=256)*sizeof(*Ident) );
    for( int i=0; i<=Level*2; i++ )fputc( !i?'\n':' ',File );
    fprintf( File,"<%s%s>",Ident[Level]=strdup( Id ),ps1( str ) );
    return ++Level;
  } return -1;
}
//
//      Здесь рекурсивно включается новый уровень
//      имя которого заносится в конец последовательного списка
//
int outXML::One( const char *Id,        //
                 const char *Fmt, ... ) // Новый уровень просто записывается
{ if( File )                            // с новыми параметрами и - закрывается
  { str_Fmt                             //
    for( int i=0; i<=Level*2; i++ )fputc( !i?'\n':' ',File );
    fprintf( File,"<%s%s />",Id,ps1( str ) );
    return Level;
  } return -1;
}
//
//  Запись в файл метаданных одной полной строки по формату
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
//  Уровень вложенности опускается на единицу,
//    о чем делается пометка в выходном файле
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
//  Закрываются все уровни сверху, включая указанный.
//    Если же соответствия при поиске не найдено,
//    то закрываются все уровни до нулевого,
//    а сам файл метаданных остается активным
//    (т.е. спуск можно сделать по пустому запросу)
//
int outXML::Back( const char *Id )
{ int cmp=0;
  if( File )for(;;)
  { if( Level>0 ){ cmp=strcmp( Id,Ident[Level-1] ); Back(); }
    if( !cmp || !Level )return Level;
  } return -1;
}
//
//   Фактическое закрытие текущего файла метаданных
//   после которого все можно начинать с самого начала
//
void outXML::Close()
{ if( File ){ Back( "" ); fputc( '\n',File ); fclose( File ); File=NULL; }
}
