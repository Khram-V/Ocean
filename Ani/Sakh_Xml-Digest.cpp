/**
     Выбока данных из XML файла справочника с описанием
        данных гидрометеорологических наблюдений
                                              Sakh-Xml-digest, Апрель, 2009
**/
#include "Sakhalin.h"
#include "..\Matrix\XML.h"

#define NextEcoWord   word = Digest.Word();   if( *word ) //if( word )if( word[0] )
#define NextEcoString word = Digest.String(); if( *word ) //if( word )if( word[0] )
                           //
static inXML Digest;       // доступ к файлу XML
                           // - расширяемого языка описания данных
struct Digest_Element      //
{ char *Key,    // ключ для поиска элемента справочника
       *Meas,   // физическая размерность данных
       *Name,   // краткое текстовое определние данных
       *Title,  // расширенное название
      **Short,  // краткое описание для быстрой справки
      **Full;   // полное текстовое описание экологического параметра
  float Pdk;     // контрольное значение предельно допустимой концентрации
  bool Active;  // изначальный признак визуализации данных на экране ЭВМ
};
struct Digest_List
{ Digest_List* Next;        // Следующий справочник
  char *FileName;           // Полное имя файла
  int N;                    // текущее количество элементов в списке
  Digest_Element *Eco_List; // собственно массив описателей
  Digest_List(): Next( NULL ),FileName( NULL ),N( 0 ),Eco_List( NULL ) {}
};
static Digest_List *First = NULL; // динамический список открытых справочников
//
//  При любом раскладе, на выходе из процедуры поиска/чтения
//    оказывается пустая или заполненная справочная таблица
//    !!! все другие операции со справочником ведутся исключительно
//    и только по выбранному здесь контектному списку
//
void* Read_Digest( const char* FileName )
{
 Digest_List *EL=First;
 char dName[MAX_PATH];           // рабочая текстовая строка ограниченной длины
                  // первым делом надо попытаться найти уже открытый справочник
  strcpy( fname( strcpy( dName,FileName ) ),"Digest.xml" );// копирование имени
  while( EL )
     if( !strcmp( EL->FileName,dName ) )return EL; else EL=EL->Next;
  //
  // если такого имени файла еще нет, то заводится новый список
  EL = new Digest_List;
 Digest_List &Current=*EL;             // контестная копия
  //
  // пытаемся открыть файл справочника
 char *word;
  if( !(word=Digest.Open( dName ) ) )return NULL;            // двойной
  if( strcmp( "Sakhalin-digest",word ) )goto Ext_from_Set; // контроль
  //
  // справочник найден, надо заполнять новый список экологических описаний
 Digest_Element Eco;
  for( ;; )                                // !!! плоховасто конечно !!!,
  { if( !(word=Digest.Next()) )break;      // здесь можно оказаться только
    if( strcmp( "Eco",word ) )continue;    // при первом или ошибочном проходе
  Find_New_Eco: // метка выхода на новый заголовок описателя внутри блока "Eco"
    //
    // на всякий случай, лучше обнулить текущие адреса текстовых полей
    Eco.Name=NULL,   // краткое текстовое определние данных
    Eco.Title=NULL,  // расширенное название
    Eco.Short=NULL,  // краткое описание для быстрой справки
    Eco.Full=NULL;   // полное текстовое описание экологического параметра
    NextEcoWord Eco.Key=strdup( word ); else continue; // нет ключа, нет ничего
    NextEcoWord sscanf( word,"%g",&(Eco.Pdk) ); else Eco.Pdk=0.0;
    NextEcoWord Eco.Meas=strdup( word ); else Eco.Meas=NULL;
    NextEcoWord Eco.Active=word[0]=='+'; else Eco.Active=true;
    for( ;; )
    { word=Digest.Next();         // здесь начинается выборка тектовых данных
      if( !word )break;
      if( !strcmp( "Eco",word ) ){ word[0]=1; break; } // чтоб не совсем пусто
      if( !strcmp( "Name",word ) ){ NextEcoString Eco.Name=strdup( word ); }
      if( !strcmp( "Title",word ) ){ NextEcoString Eco.Title=strdup( word ); }
      if( !strcmp( "Digest",word ) ){ NextEcoString Eco.Short=String2TextLn( word ); }
      if( !strcmp( "Description",word ) ){ NextEcoString Eco.Full=String2TextLn( word ); }
    }
    //
    // теперь надо пополнить текуший список описаний
    Current.Eco_List=(Digest_Element*)Allocate
             ( sizeof(Digest_Element)*(++Current.N),Current.Eco_List );
    Current.Eco_List[Current.N-1]=Eco;
    if( !word )break;
    if( word[0]==1 )goto Find_New_Eco;          // если новый блок уже начат
  }
Ext_from_Set:
  Digest.Close();                                                               // printf( " -- read %d records",Current.N );
  if( Current.N>0 )  // контрольное обновление списка
  { if( !First )First=EL; else
    { Digest_List *EP=First;
      while( EP->Next )EP=EP->Next; EP->Next=EL;
    } EL->FileName=strdup( dName );                     // дамп нового имени
    return EL;
  } delete EL; return NULL;
}
void Map::DigestComment( const char* Key ) // заполнение текстов из справочника
{ if( !Digest )return;
  Digest_List &Current = *((Digest_List*)Digest);
  for( int i=0; i<Current.N; i++ )                // перенос комментариев
  if( !strcmp( Key,Current.Eco_List[i].Key ) )    // если совпадает ключ данных
  {      // попробуем попростецки, пока, передать две строки для показа справки
    Comment=Current.Eco_List[i].Short;
    Contain=Current.Eco_List[i].Full;
  }
}
//
//      Вспомогательные процедуры трансформации текcтов
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

