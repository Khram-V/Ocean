//
//      Display/Tmenu                                   \91.06.18\.
//      ѕакет диалога с терминалом с помощью таблиц запросов
//        Y,X   - координаты левого верхнего угла дл€ окна запросов
//        Mlist - список параметров дл€ запросов на терминал
//        Nm    - количество запросов в списке Mlist
//       <PgUp> - переход в начало меню
//       <PgDn> - переход в конец меню
//       return - номер последнего активного запроса
//
#include "..\Graphics\Tv_graph.h"
//
//  Mlist - —писок параметров одного запроса на терминал
//      INPUT                           ACTIVE
//  Y   :skip string                    :номер сроки
//  X   :!0 - готовность                :номер позиции дл€ сообщени€
//  lf  : 0 - запрос не производитс€    :длина входного сообщени€
//  S   : ~ стро€щийс€ формат чтени€ данных
//  Msg :NULL - чистое входное поле     :выходной формат
//  dat :NULL и lf<>0 - меню-запрос     :адрес измен€емого объекта
//
Display::Display( Mlist *__M, int n, int x, int y )
{ X=x; M=(mlist*)__M; Nm=n; Hl=0; Y=y; K=0; Active=false;
}
Display::Display( void HL(int), Mlist *__M, int n, int x, int y )
{ X=x; M=(mlist*)__M; Nm=n; Hl=HL; Y=y; K=0; Active=false;
}
#define setC { fc=Tv.FColor; c=color( DARKGRAY ); bc=bkcolor( LIGHTCYAN ); }
#define retC { color( c ); pattern( fc ); bkcolor( bc ); }
#define _s( S ) ( *(S)!='\t'?(S):W2D( (S)+1 ) )

static mlist *Ls;                               //
static char St[244]="";                         // ”троенна€ рабоча€ строка
static int Mestr()                              // –азбор текстовой строчки
{                                               //
 char *S=(char*)(Ls->Msg); void *dat=Ls->dat; int lc=0,k=0,l=Ls->lf; Ls->S='s';
  if( !S )*St=0; else
  { lc=strlen( strcpy( St,_s( S ) ) ); while( k<lc && St[k]!='%' )k++;
  }
  if( dat!=NULL )                               //
  { if( k==lc )strcat( St,_s( (char*)dat ) );   // Here is S==NULL
    else                                        //
    { char ch=St[lc-1];
      short lh=St[lc-2];                        //
      if( ch=='∞' || ch==0xF8 )                 // Degrees
      { lh-='0'; Ls->S=0xF8;                    //  '∞';
        if( fixed( lh )>3 )lh=0; else if( St[k+1]=='-' )lh=-lh;
        strcpy( St+k,DtoA( *(double*)dat,lh,0xF8 ) );
      } else                                    // and another formats
      { ch|=0x20;                               // tolower( S )
        lh=lh=='l';                             //
        Ls->S=lh ? ch|0x80:ch;                  // (long)|(double)
        if( ch>'d' && ch<'h' )                  //
        { if( lh )          sprintf( St,_s( S ),*(double*)dat );
             else           sprintf( St,_s( S ),*(float*)dat );
        } else if( ch!='s' )sprintf( St,_s( S ),*(long*)dat );
          else              sprintf( St,_s( S ),dat );
    } }
  } else if( l ){ if( l<k )l=k; k=0; }
       Ls -> lf=l;                  St[l+=k]=0;
  for( lc=strlen( St ); lc<l; lc++ )St[lc]=' '; return k;
}
inline int Message()
{ int k=Mestr(); Tgo( Ls->X,Ls->Y );   Tpoint.y+=2; Tprintf( St );
                 Tgo( Ls->X+k,Ls->Y ); Tpoint.y+=2; return k;
}
int Display::Answer( int _K )
{ const int IO=81,            //  81-161 - строка обмена с терминалом
            BF=162;           // 162-242 - сохранение дл€ регистрации изменений
 static int lf=0;             //       Ќачальна€ инициализаци€ списка сообщений
 int  ls,Lx=0,Ly=1,bc,hc,fc,c,k,kw=_K>=0 && _K<Nm;
 bool key=( M->X==0 );
  setC
  for( Ls=M,k=ls=0; k<Nm; Ls++,k++ )    //
  { if( key )Ls->Y+=Ly;                 // новый номер строки
    if( Ls->Y!=Ly )ls=0,Ly=Ls->Y;       // поиск новой строки
    if( key )Ls->X=ls+1;                // позици€ начала сообщени€
    ls += Mestr();                      // нова€ длина строки
    ls += Ls->lf;                       //  + длина входного пол€ +1
    if( Lx<ls )Lx=ls;                   // выбор ширины окна      -1
  }                                     //
  key |= !Active | !kw;                 // заполнение окна сообщений
  if( !Active )Twindow( X,Y,Lx,Ly,1 );  //
  if( key )for( Ls=M,lf=k=0; k<Nm; Ls++,k++ ){ Message(); lf+=Ls->lf; }
 int ans=0; k=1; kw=kw?_K:K;
  if( !lf ){ Tv_bell(); Tv_getc(); }
  else do
  { if( kw<0 )kw=Nm-1; else if( kw>=Nm )kw=0;
        K=kw;
        Ls=M+kw;
    if( Ls->lf && Hl ){ retC Hl( kw ); setC } ls=Message();
    if( Ls->lf )
    { if( Ls->dat )ans=Tgets( strcpy( St+IO,strcpy( St+BF,St+ls ) ),Ls->lf );
      else
      { int l=Tpoint.x-2, r=l+Tv.Tw*strlen( St+ls )+2,
            t=Tpoint.y-2, b=t+Tv.Th+1;
        Tv_rect_UpXOR( l,t,r,b );
        do ans=Tv_getc(); while( ans>=_MouseMoved && ans<=_MouseReleased );
        Tv_rect_UpXOR( l,t,r,b );
      }
      if( ans==_Tab )ans=Tv_getk()&SHIFT?West:East;
      if( ans==_Enter ){;} else
      if( ans>_Enter  ){ if( (kw=ans)==_Esc )break;
                         if( kw>' ' && kw<0xF0 )kw=' '; ans=_Enter; } else
      if( ans==North_East ){ k=+1; kw=0;    } else
      if( ans==South_East ){ k=-1; kw=Nm-1; } else
      if( ans& South_East ){ k=+1; kw++;    } else
      if( ans& North_West ){ k=-1; kw--;    }
      if( Ls->dat!=NULL )
      if( strcmp( St+IO,St+BF ) )
      { if( Ls->S=='∞' || Ls->S==0xF8 )AtoD( St+IO,*(double*)Ls->dat ); else
        { static char f[4]="%  "; int k=1;                   // — учетом %lx
          if( Ls->S&0x80 ){ ++k; f[1]='l'; } else f[2]=0;
              f[k]=Ls->S&~0x80;
          if( f[k]=='s' )strcpy( (char*)Ls->dat,St+IO );
                else     sscanf( St+IO,f,Ls->dat );
      } } Message();
    } else kw+=k;
  } while( ans!=_Enter ); Active=TRUE; retC return kw;
}
void Display::Back(){ if( Active )Tback(); Active=false; }
//
//      Tmenu   - јвтономна€ процедура диалога с терминалом  /90.10.03/
//
int Tmenu( Mlist *m, int n, int x, int y, int ans )
{
  Display T( m,n,x,y ); return T.Answer( ans );
}
int Tmenu( void Hl(int), Mlist *m, int n, int x, int y, int ans )
{
  Display T( Hl,m,n,x,y ); return T.Answer( ans );
}
Display::~Display(){ Back(); }
