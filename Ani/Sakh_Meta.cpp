///
///  ���� �������� ��� ��������� ����� ������ ����������� �������� ����������
///
#include "Sakhalin.h"
#include "..\Matrix\Function.h"

static FILE *_F_=NULL;                                // ������� ����
static char inMsgStr[MAX_PATH*2];                     // � ��� ��������� �����
//    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    ����������, ������������ ��� ������ ����� XML, DC2, DW2, DC1
//
                      // ������� ��� ������� ���������� ������������ ���������
static enum DataType  // ��� ������ ���������� �� ���������� ����� �����
       { dtNoData,    // -- ���� �� �������
         dtDC_Ascii,  // ������� DesignCAD � �������� ������� DC2
         dtDC_Binary, // ������� DesignCAD � �������� ������� DW2
         dtTextData   // ����-��������� �������� ���������������� ��������
       } FileType;    //
//
//     ����������� ���� ������������ ����� �� ��� ���������� �����
//
static DataType GetFileType( const char *name ){ char *ext;
  if( (ext=strrchr( name,'.' ) )!=NULL )
  { static char e[4]="dc2"; strlwr( (char*)memcpy( e,++ext,3 ) );
    if( !strcmp( e,"dc1" ) )return dtTextData;
    if( !strcmp( e,"dc2" ) )return dtDC_Ascii;
    if( !strcmp( e,"dw2" ) )return dtDC_Binary;
  } Error_Message( "?�������� ��� �����: %s",name ); return dtNoData;
}
//
///  ������ ����� ������ �� �����
//      ��� ��������� - �������� ��� ����� ����� ������, � ����� � ��� ������
//      ��� ���������� - �������� ������ � ���������� ������� �������� ������
//          ���������������, ��� ���� ������, ���
//          ��������� � ������� ������ "\r\n" ������������
//
static char* inputString()
{ int l;
  if( FileType==dtDC_Binary )
    { fread( inMsgStr,1,l=get2(_F_),_F_ ); inMsgStr[l]=0; return inMsgStr;
    }
  for( l=0; !feof( _F_); )
    { char ch=getc( _F_ );
      if( ch=='\r' ){ if( (ch=getc( _F_ ))!='\n' )ungetc( ch,_F_ ); break; }
      if( ch=='\n' ){ if( (ch=getc( _F_ ))!='\r' )ungetc( ch,_F_ ); break; }
      if( (l+1)<int( sizeof( inMsgStr ) ) )
      { if( ch=='\t' )do inMsgStr[l++]=' '; while( l%8 );
                    else inMsgStr[l++]=ch;
    } }   inMsgStr[l]=0;
  strcut( inMsgStr );
   return inMsgStr;
}
///     ������ ��������������� ������, ���������� �� ���������� �����
//
static void readStrData( const char *Fmt,... )
{ fgets( inMsgStr,sizeof( inMsgStr ),_F_ );
  if( Fmt ){ va_list a; va_start( a,Fmt ); vsscanf( inMsgStr,Fmt,a ); }
}
#include "Sakh_DesignCAD.cpp"
//
//      ���� �������� �� ������ ��������� ����-������ Sakh-MET
//      �� ��������� ��������������� � ������� ��������������
//      �������� � ������ ���������� �������������� ��������
//
//      ����������� ������ ���������� ����� ������
//
#define TDPrefix  '.'           // ������� �������
#define TDComma   ','           // ������� ������ �����������
#define TDLayer   "StartLayer"  // ������ ����
#define TDPoint   "Point"       // ����������� �����
#define TDEco     "Eco"         // ����������� ����������

enum{ tdCommaID,tdContourID,tdPointID=2048,tdLayerID=4096,tdEcoID=8192 };

static void __Full_Data_File_Name( const char *Name )
{ char *s,*z=strdup( Name );
  if( !(s=strrchr( strcpy( inMsgStr,StartMap.DstName ),'\\' )) )s=inMsgStr; else s++;
  strcpy( s,z ); free( z ); //strupr( inMsgStr );
}
void Map::Read_DC1()                               //
{                                                  // ������ ��������� � ������
 char *p,Lat[16],Lon[16],ch;                       //
 int k=0;
  readStrData( "%s%s%d%f%f%s",Lon,Lat,&iDispType,&(S.x),&(S.y),inMsgStr+MAX_PATH );
  AtoD( Lon,P.x );
  AtoD( Lat,P.y );
  if( (iDispType&dtpDCFile) && inMsgStr[MAX_PATH] ) //
  {    __Full_Data_File_Name( inMsgStr+MAX_PATH );  // ������� ����� ���
         DstName=strdup( inMsgStr );                // ���������� �����
  } else DstName=0;                                 //
  if(!(iDispType&dtpChart) )iColor=int( S.x+0.1 ),iMark=S.y,S.x=S.y; else
  if( (iDispType&(dtpCircle|dtpCross))!=0 )iMark=int( abs( S )/2 );
  //  dtpCross=1,dtpCircle=2,dtpChart=4,dtpDCFile=8 - ��� ����� �� �����
  //
  //  ������ �������� ������� �� ���������� ����� .dc1
  //
  if( *(p=inputString()) )Title=strdup( p );
  ch=getc( _F_ );
  k=0;
  while( ch!=TDComma && ch!=TDPrefix && !feof( _F_ ) )
  { ungetc( ch,_F_ );
    (Comment=(char**)Allocate( sizeof(char*)*(k+2),Comment ))[k++]=strdup( inputString() );
    ch=getc( _F_ );
  } k=0;
  if( ch==TDComma )do
  { ungetc( ch,_F_ );
    (Contain=(char**)Allocate( sizeof(char*)*(k+2),Contain ))[k++]=strdup( inputString() );
  } while( ( ch=getc( _F_ ) )!=TDPrefix && !feof( _F_ ) );
  if( !feof(_F_) )ungetc( TDPrefix,_F_ );
}
///     ����������� ������� �������������� �������� � �������������� �����
 //
static unsigned _RType=0x8421; //x4924
static point _p={ 0,0 },_s={ 0,0 };
static void Geo_Rect()
{ setlinestyle( USERBIT_LINE,_RType,NORM_WIDTH ); rectangle( _p.x,_s.y,_s.x,_p.y );
  setlinestyle( SOLID_LINE,0,NORM_WIDTH );
}
void Map::Pts_Show()
{ color ( iColor );
  if( iDispType&dtpChart ){ _p=P; _s=P+S; Geo_Rect(); } else
    { if( iDispType&dtpCross  )Cross ( P,iMark );
      if( iDispType&dtpCircle )Circle( P,iMark ); }
}
void CrossAt( Map *T )          //
{                               // ��������� �������
 static float _a=0;             //
 static  int _st=0;             //
 int c=color( GREEN^WHITE );    // ������ ������ �������
  setwritemode( XOR_PUT );      //
  if( _st==2 )Geo_Rect();
  if( _st==1 )Geo_Star( _p.x,_p.y,12,_a ); _st=0; _RType=0x8421;
  if( T )
  { if( T->iDispType&dtpChart )
    {  static unsigned _RT=0xF0F;     // ??????
      _RT<<=2; _RType=( _RT|=_RT>>8 ); T->Pts_Show(); _st=2;
    } else
    { point p=Point( *T );
      if( _p==p )_a+=M_PI_4/12.0; else _p=p; Geo_Star( p.x,p.y,12,_a ); _st=1;
  } } setwritemode( COPY_PUT ); color( c );
}
//  ����� ���� � ��������� ��������� �������
//
#define Add( Str ) (Txt=(char**)Allocate( N+2,0,Txt ))[N]=strdup( Str ); N++

void Map::TextWindowView()
{ if( !Contain )return; else
  { viewporttype V={ 0,PcY(36)-6,Tv.mX-2,Tv.mY-Tv.Th-8,1 };
    int N=0;
    static char **Txt=NULL; char **W;
    if( (W=Comment)!=NULL )
      { Add( " " ); while( *W ){ Add( *W ); W++; } Add( " " ); }
    W=Contain;
    while( *W )
    { if( **W!=TDComma ){ Add( *W ); } else
      if( sscanf( (*W)+1,"%s", inMsgStr+MAX_PATH )>0 ) // ����������� �� ������
      { __Full_Data_File_Name( inMsgStr+MAX_PATH );    // �������� ������ ���
        if( (_F_=fopen( inMsgStr,"rt" ))!=NULL ) // ���������� ������� �����
        { FileType=dtTextData;
          while( !feof(_F_) ){ Add( inputString() ); } fclose(_F_);
      } } W++;
    }
    if( Txt )
    { Inform_Window( W=Txt,V,BLACK ); while( *W ){ free( *W ); *W=NULL; ++W; }
    }
} }
//   ��������� ��������
//
void Map::ReadText( char *FileName )
{ int DataId=0,LNum=0; char *istr;
  if( (FileType=GetFileType( FileName ))!=dtTextData )return;
  if( !(_F_=fopen( FileName,"r" ) ) )return;    //
                                                // �������� ������
 ItemsList *Layer=new ItemsList; // ( 0,tdLayerID ); // ������ ����������
                                                //
  while( !feof( _F_ ) )
  { do{ if( !feof( _F_ ) )istr=inputString(); else goto Exit;
      } while( *istr!=TDPrefix );
    //  strlwr( istr );
    if( !strncmp( istr+1,TDLayer,sizeof(TDLayer)-1) )DataId=tdLayerID; else
    if( !strncmp( istr+1,TDPoint,sizeof(TDPoint)-1) )DataId=tdPointID; else
    if( !strncmp( istr+1,TDEco,3 )       ){ istr+=5; DataId=tdEcoID; } else
      { Error_Message( "? ����: %s",istr ); break;
      }
    if( feof( _F_ ) )goto Exit;
    switch( DataId )    //
    { case tdLayerID:   // �������� ���� (��������� ����)
       if( !LNum )  //
         { Layer->AddName( inputString() ); txtLL.AddLayer( Layer ); } else
           Layer=txtLL.AddLayer( new ItemsList(inputString()) ); ++LNum; break;
      case tdPointID:                       // �������� ����� (��������� ����)
         { Map *TW = new Map;
           TW->Read_DC1();
           Layer->AddItem( TW );
         } break;
      case tdEcoID:                  // -- ���-�� ����� ������������� iD>=8192
         while( *istr && *istr<=' ' )istr++;      // �������� ��� XML �����
         if( *istr )                              // �� ����� �������� ������
         { strcut( istr );
           __Full_Data_File_Name( O2A( istr ) );                                // printf( "\nMap::Xml - %s",W2D( inMsgStr ) );
           Map *TX = new Map( inMsgStr );       //,int(Layer->nM)+tdEcoID );
           if( TX->DstName )Layer->AddItem( TX );
           else{ delete TX; Error_Message( "? ������ XML: %s",inMsgStr ); }
         }
      default: break;
    }
  }                       //
Exit:                     // ���������� �� �������� ������ ������� ������ ����
  fclose( _F_ );          //
  if( !LNum ){ Layer->AddName( FileName ); txtLL.AddLayer( Layer ); }
  else
  { Item *M,*W,*X;
    for( ItemsList *L=txtLL.L_First; L; L=L->L_Next )
    for( M=L->First; M; M=M->Next )
    { for( W=(X=M)->Next; W; W=W->Next )
        if( strcmp( ((Map*)X)->Title,((Map*)W)->Title )>0 )X=W;
      if( X!=M )                                   //
      { if( M->Prev )M->Prev->Next=X;              // ������� �����
        if( X->Next )X->Next->Prev=M;              // ��������� ����������
        if( M->Next==X )                           //
          { X->Prev=M->Prev; M->Prev=X;            // ������� �����
            M->Next=X->Next; X->Next=M;            // �������������� �����
          } else                                   //
          { M->Next->Prev=X;                       // ����� ����
            X->Prev->Next=M;                       // ������� ���-�� ����
            W=M->Prev; M->Prev=X->Prev; X->Prev=W; // ������� ������
            W=M->Next; M->Next=X->Next; X->Next=W; // ���������� ������
          }                                        //
        if( M==L->First )L->First=X;               // ��� ������� ��������,
        if( M==L->Last )L->Last=X;                 // ������, �� ����� �����
        M=X;                                       // ��� ��������� �������
      }                                                                         // else printf( "%s \n",((Map*)M)->Title );
    }
  }
//  { Item *M,*W,*X;
//    for( ItemsList *L=txtLL.L_First; L; L=L->L_Next )
//    for( M=L->First; M; M=M->Next )
//    { for( W=(X=M)->Next; W; W=W->Next )
//        if( strcmp( ((Map*)X)->Title,((Map*)W)->Title )>0 )X=W;
//      if( X!=M )
//      { if( X->Prev )X->Prev->Next=X->Next; else L->First=X->Next; // ��������
//        if( X->Next )X->Next->Prev=X->Prev; else L->Last=X->Prev;  // ���������� ��������
//                                                        // ��������� �������
//        if( (X->Prev=M->Prev)!=NULL )X->Prev->Next=X;   // ������� �� �����
//        if( (X->Next=M->Next)!=NULL )X->Next->Prev=X;   // ��������� �������
//        L->Last->Next=M; M->Next=NULL;
//        M->Prev=L->Last; L->Last=M;                                           // printf( "%s <-- %s --   \n",((Map*)M)->Title,((Map*)X)->Title );
//        if( M==L->First )L->First=X;
//        M=X;
//      }                                                                       // else printf( "%s \n",((Map*)M)->Title );
//    }
//  }
}
