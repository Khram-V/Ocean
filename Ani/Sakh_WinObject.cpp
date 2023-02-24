#include "Sakhalin.h"
//
//      Конструкторы
//      1. Списки объектов внутри уровня расслоения
//      2. Текстовые списки объектов, окошки, строчк и подсказки
//
Item::~Item(){ if( Prev )Prev->Next=Next;
               if( Next )Next->Prev=Prev;
             }
//      Конструкторы списков для формирования тематических объектов
//
ItemsList::ItemsList( const char *LayerName )
         :   First( NULL ),   Last( NULL ),
            L_Prev( NULL ), L_Next( NULL ),
           iActive( true ), szName( NULL ), nM( 0 )
         { szName=AddName( LayerName );
         }
ItemsList::~ItemsList()
  { if( L_Prev )L_Prev->L_Next=L_Next;
    if( L_Next )L_Next->L_Prev=L_Prev;
    for( Item *K,*M=Last; M; ){ K=M->Prev; delete M; M=K; --nM; }
    if( szName )free( szName );
  }
//      Редактирование списков
char*                                   // Добавление названия
ItemsList::AddName( const char *lN )    // 0 - без изменений
  { if( lN )                            //
      { if( szName )free( szName ); szName=strdup( lN ); } return szName;
  }                                     //
Item*                                   // Встраивание нового
ItemsList::AddItem( Item *Obj )         // графического элемента
  { ++nM;                               //
    if( !First ){ First=Obj; Obj->Prev=Obj->Next=NULL; } else
    {   Obj->Next=NULL;
        Obj->Prev=Last;
             Last->Next=Obj;
    } return Last=Obj;
  }                                     //
ItemsList*                              // Открытие нового слоя
LayerList::AddLayer( ItemsList *It )    // внутри текущего рисунка
  { ++nL;                               //
    if( !L_First ){ L_First=Enum=It; It->L_Prev=It->L_Next=NULL; } else
    {  It->L_Next=NULL;
       It->L_Prev=L_Last;
           L_Last->L_Next=It;
    } return L_Last=It;
  }
//      Конструкторы информационных слоев первого уровня
//
//LayerList::LayerList(): nL( 0 ),L_First( NULL ),L_Last( NULL ),Enum( NULL ){}
//LayerList::~LayerList(){ Free(); } // еще аж без исполнения конструкторов
void LayerList::Free()
  { ItemsList *M,*L; for( L=L_First; L; ){ M=L->L_Next; delete L; L=M; --nL; }
               L_First=Enum=NULL;
  }
//  Простой список уровней объединяющих блоки (списки) объектов
//   - подсчет количества объектов в выделенных слоях метафайла
//
long LayerList::Number_of_Objects() // Общее количество элементов
  { long n=0;                       //
    for( ItemsList *L=L_First; L; L=L->L_Next )if( L->iActive )n+=L->nM;
    return n;
  }
ItemsList* LayerList::First()
  { Enum=L_First;
    while( Enum )if( Enum->iActive )break; else Enum=Enum->L_Next;
    return Enum;
  }
ItemsList* LayerList::Next()
  { if( Enum )Enum=Enum->L_Next;
    while( Enum )if( Enum->iActive )break; else Enum=Enum->L_Next;
    return Enum;
  }
Item* LayerList::GetActive( long num ) // - выбор по длинному номеру
  { Item      *M=NULL;      long SN=0; //
    ItemsList *OL=First();             //   графического элемента
    while( OL && num>=SN+OL->nM ){ SN+=OL->nM; OL=Next(); } SN=num-SN;
    if( OL )
    for( M=OL->First; M && SN; SN--,M=M->Next ); return M;
  }                                             //
void LayerList::SwitchLayerState( unsigned Id ) // Переключение по номеру
  {  unsigned k=0;
     ItemsList *L=L_First;
     for( ; k<=Id && L!=NULL; L=L->L_Next,k++ ) //     в списке слоев
      if( k==Id ){ L->iActive^=1; break; }      //
//     for( ItemsList *L=L_First; L; L=L->L_Next )
//     if( Id==L->Key )L->iActive^=1;
  }
//
//        Определители (статические) для списка основных графических окон
//
WinObject* WinObject::_First=NULL;       // Первый элемент в списке "окон"
//
//      Список оконных объектов формируется вставкой нового элемента
//      в голову основного списка, со смещением других элементов в хвост
//        - сохранение _Sci - по особому требованию
//
WinObject::WinObject( viewporttype &f, // Размерения окранного окошка
                          unsigned Id, // Тип присоединных данных
                          unsigned Tm  // Tребование реакции на мышку
                    ): _Sci( NULL ),_Id_( Id ),_Tm( Tm ),_Prev( NULL ),scr( f )
{ _Next=_First;
    if( _First )_First->_Prev=this; _First=this;
}
WinObject::~WinObject()   // Восстановление старого поля
{  Restore(); if( _Prev )_Prev->_Next=_Next; else _First=_Next;
              if( _Next )_Next->_Prev=_Prev;
}                                 //
WinObject*                        // Первый найденный элемент соответствует
WinObject::FindNew( unsigned Id ) // новейшему из вновь созданных
{ WinObject *W;                   //
     for( W=_First; W; W=W->_Next )if( W->_Id_==Id )break; return W;
}
void WinObject::Save(){ _Sci= ::Save( scr,_Sci ); }
void WinObject::Restore(){ ::Restore( _Sci,scr.left,scr.top ); _Sci=0; }

void WinObject::Clear( int c )
{    pattern( SOLID_FILL,c ); bar( scr.left,scr.top,scr.right,scr.bottom );
}
///
///      Здесь выполняется прокрутка списка точек метафайла
///
static void TextXY( int xx, int yy, char *s )
  { if( xx<0 )xx+=Tv.mX; else if( xx )--xx;
    if( yy<0 )yy+=Tv.mY; else if( yy )--yy; Tv.Text( xx,yy,s );
  }
//void PanelWin::TextXY( int x, int y, char *T )
//  {          ::TextXY( Win.left+x*Tv.Tw+1,Win.top+y*Tv.Th+1,T );
//  }
ScrollList::ScrollList( viewporttype &f, long n, char* (*iF)(int) )
           : WinObject( f ),Num( int(n) ),Cur( 0 ),First( 0 ),ItemFun( iF )
  { Height=min( Num,(scr.bottom-scr.top-2)/Tv.Th );
    MaxLen=         ( scr.right-scr.left )/Tv.Tw;
    ShowList();
  }
static void ScrollString( int l,int t,int r,int b, char *s, int c )
  { pattern( SOLID_FILL,c ); bar( l,t,r,b ); c=bkcolor( c ); TextXY( l+1,t+2,s );
                                               bkcolor( c );
  }
void ScrollList::ShowSelect( int f )
  { if( Num )
    { int cur=Cur-First; f=f?CYAN:LIGHTGRAY;
      color( BLACK );
      ScrollString( scr.left+1, scr.top+cur*Tv.Th+2,
                    scr.right-1,scr.top+(cur+1)*Tv.Th,ItemFun( Cur ),f );
  } }
void ScrollList::SetCurrent( int c, int f )
  { if( Num )
    { if( f<0 )f=First;
      if( c>=Num )c=Num-1;             if( c<0 )c=0;
      if( f>=Num-Height )f=Num-Height; if( f<0 )f=0;
      if( c>=f+Height   )f=c-Height+1; if( c<f )f=c;         //
      if( f!=First ){ Cur=c; First=f; ShowList(); } else     // перелистывание
      if( c!=Cur ){ ShowSelect( 0 ); Cur=c; ShowSelect( 1 ); }
  } }
void ScrollList::ShowList()
{ Clear( LIGHTGRAY );
  if( Num )
  { int i,C=color( BLACK ),
         bc=bkcolor( LIGHTGRAY );
    for( i=0; i<Height && First+i<Num; i++ )
        TextXY( scr.left+2,scr.top+i*Tv.Th+4,ItemFun( First+i ) );
    ShowSelect( 1 );
    color( C ),bkcolor( bc );
} }
Course ScrollList::Run()
{ Course key;
  int cur=Cur,first=First;
  switch( key=WinObject::Action() )
  { case _Left : cur-=3;
    case _Up   : cur--;                          break;
    case _Right: cur+=3;
    case _Down : cur++;                          break;
    case _PgUp : cur-=Height-1; first-=Height-1; break;
    case _PgDn : cur+=Height-1; first+=Height-1; break;
    case _Home : first=cur=0;                    break;
    case _End  : cur=Num-1;  first=cur-Height-1; break;
    case _MousePressed:
      if( MouseThere() )
      { cur=First+( Tv_MouseY()-scr.top-1 )/Tv.Th;
        if( cur!=Cur )SetCurrent( cur ); else
        { if( Tv_MouseState==_MouseRightPress )key=_Space;
          if( Tv_MouseState==_MouseLeftPress  )key=_Enter;
      } }
    default:                 return key;
  } SetCurrent( cur,first ); return key;
}
///       Несколько процедур для оформления информационной строки
 //
Comment_String *Status_Line=0; // Базовая строка активных комментариев
                               //
Comment_String::Comment_String( viewporttype &f, int n,... )
              : WinObject( f )
{ va_list _ptr;
  va_start( _ptr,n );
  for( num=0; num<n; num++ )
  { off[num].xb= va_arg( _ptr,int )*Tv.Tw + scr.left+5;
    off[num].w = va_arg( _ptr,int );
  } va_end( _ptr ); Save(); Draw();
}
Comment_String::~Comment_String(){ Restore(); }

void Comment_String::Draw()
{
 int i,rx,c=color( BLACK );
  Panel( scr.left,scr.top,scr.right,scr.bottom,ptOuterRaise,1,0,LIGHTGRAY );
  for( i=0; i<num; i++ )
  { rx=off[i].xb+off[i].w*Tv.Tw+4;
    Panel( off[i].xb,scr.top+3,rx,scr.bottom-3,ptOuterLower,1,0,-1 );
  } color( c );
}
void Comment_String::Print( int at, const char *_s,... )
{
 char buf[MAX_PATH];
 va_list ptr; va_start( ptr,_s ); vsprintf( buf,_s,ptr ); va_end( ptr );
  Panel( off[at].xb,scr.top+3,off[at].xb+off[at].w*Tv.Tw+4,
                    scr.bottom-3,ptOuterLower,1,0,LIGHTGRAY );
  if( !at )bar( 0,Tv.mY-2,Tv.mX,Tv.mY );
  bkcolor( LIGHTGRAY );
  color( BLACK );
  Tv.Text( off[at].xb+3,scr.top+7,W2D( buf ) );
}
///      Окно собщения о возникшей проблеме
//
void Error_Message( const char Msg[], ... )
{
 int l;
 viewporttype E;
 char ErrorTxt[120], *EFT[3]={ (char*)"О Ш И Б К А !",ErrorTxt,0 };
 va_list r; va_start( r,Msg ); l=vsprintf( ErrorTxt,Msg,r );
  E.left=E.top=0;
  E.right =Tv.Tw*l+20;
  E.bottom=Tv.mY-Tv.Th-16; Inform_Window( EFT,E,LIGHTRED,YELLOW,false );
}
//
///      Процедуры просмотра текстовой информации
//
static int StrLen( char *s )
{ int i,j,k,w=strlen( s );
   for( i=j=0; s[i]; i++,j++ )
    if( s[i]=='\t' ){ k=((j+8)/8)*8; w+=k-j-1; j=k; } return w;
}
void Inform_Window( char **SList, viewporttype &V, int _tc, int _bc, bool OEM )
{                       //
 HIMG _Sci=0;           // Программа сохраняет фоновую картинку и
 char str[250],*s;      // организует просмотр текста с прокруткой -- 81
 viewporttype scr=V;    //
 int x,Y=0,ans,N,       // Текущее смещение для текста и код запроса
     Width=0,           // Начальные позиции
  SymbolHeight=Tv.Th-2,                          // Высота одной строки
  yPage=( (scr.bottom-scr.top-8)/SymbolHeight ); // Длина страницы

  for( N=0; SList[N]; N++ )if( Width<(x=StrLen( SList[N] )) )Width=x;
  if( Width*Tv.Tw>Tv.mX-12 )Width=( Tv.mX-12 )/Tv.Tw;
  if( yPage>N )yPage=N;
  if( yPage<(scr.bottom-scr.top-8)/SymbolHeight )
             scr.top=scr.bottom-yPage*SymbolHeight-8;
  scr.left = scr.right-Width*Tv.Tw-12;
  scr.top  = scr.bottom-( ( scr.bottom-scr.top+SymbolHeight-9 )
                          / SymbolHeight )*SymbolHeight-8;
  _Sci=Save( scr );
  Panel( scr.left,scr.top,scr.right,scr.bottom,
           ptInnerLower|ptOuterRaise,2,1,_bc );
  for( int ySet=-1;; )
  { ans=0;
    if( Y!=ySet )
    {   ySet=Y; bkcolor( _bc );
                  color( _tc );
      for( int y=0; Y<N && y<yPage; Y++,y++ )
      { s=OEM?SList[Y]:W2D( SList[Y] );
        for( x=0; x<Width && s[0]; s++ )if( *s!='\t' )str[x++]=*s;
                                        else do{ str[x]=' '; x++; }while( x%8 );
        for( ; x<Width; x++ )str[x]=' '; str[Width]=0;
        Tv.Text( scr.left+8,scr.top+y*SymbolHeight+5,str );
            ans=Tv_getw();
        if( ans==_Up || ans==_Down )
        if( ySet && ySet<N-yPage )break; ans=0;
      } Y=ySet;
    }
    if( !ans )ans=Tv_getc();
    switch( ans )
    { case _Up  : Y--;        break;
      case _Down: Y++;        break;
      case _PgUp: Y-=yPage-2; break;
      case _PgDn: Y+=yPage-2; break;
      case _Home: Y=0;        break;
      case _End : Y=N-yPage;
      case _MouseMoved:
      case _MouseReleased: break;
     default: Restore( _Sci,scr.left,scr.top ); return;
    }
    if( Y<0 )Y=0; else
    if( Y>=N-yPage )Y=N-yPage;
} }
