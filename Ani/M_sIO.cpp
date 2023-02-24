//
//  S_IO.cpp                            Ver 3.2  91.05.09
//      Второй вариант построения объекта-вектора,
//      реализуются процедуры ввода/вывода
//
#include "Mario.h"
                                  //
static char Iden[MAX_PATH]="",    // Текстовый идентификатор \ 162 для Impex
            Name[MAX_PATH]="*.*"; // Имя файла               /
static int   Typ=0, // 0-Text; 1-Byte; 2-Integer; 4-Float
             Mod=0; // 0-Read; 1-Write; 2-Added;  3-Update
static const
       char *Mode[4]={ " Read "," Write"," Added","Update" },
            *Type[4]={ " Text "," Byte "," Fixed"," Float" };
static int  J=0,K=1,              // Длина в байтах, отступ и шаг
            L=0;                  // Длина выборки
static FILE *Fs;                  // Единый (временный) указатель файла
       HIMG SImage;

static viewporttype v,vi;

static void View   ( Mario& );
static void Dread  ( Mario& );
static void Dwrite ( Mario& );
static void Example( Mario& );
#undef Nm

Mario::operator char*(){ return Iden; }

void iMENU( Mario& S )
{ View( S );
  for( int ans=1,c=color( CYAN );; )
  { Display T( 0,0,1,-2 );
    for( L=S.N;; )
    { if( S.N<=0 )Mod=0; if( Mod==3 && Typ==0 )Typ=3;
      { Mlist Menu[]=             //
        { { 0,41,     0,Iden    } // Идентификатор
        , { 1,41,     0,Name    } // Имя файла
        , { 1,S.N?6:0,Mode[Mod] } // Режим чтения/записи
        , { 0,6,      Type[Typ] } // Тип данных
        , { 0,6," set %-4ld",&J } // Смещение до первой точки
        , { 0,3, " by %-3ld",&K } // Шаг считывания
        , { 0,5," plus %-5d",&L } // Количество вводимых точек
        };                        //
        T.M=(mlist*)Menu;
        T.Nm=7;
        Thelp( "<%d> F1 Example  F2 I/O  F9 Clear",S.N );
        ans=T.Answer( ans );
      }
      if( ans==1 ){ Tfile( Name );        }else
      if( ans==2 ){ Mod++; Mod&=3; L=S.N; }else
      if( ans==3 ){ Typ--; Typ&=3;        }else break;
    }
    switch( ans )
    { case 0xF1: Example( S );                               break;
      case 0xF2: if( !Mod||Mod==3 )Dread(S); else Dwrite(S); break;
      case 0xF9: S.allocate( L=0 ); View( S );               break;
      case _Esc: color( c ); return;
  } }
}
Mario& Mario::_IO_( const char *Fname, char mode )
{ getviewsettings( &v );
  vi=v;
  vi.left=Tv.Tw*40+3;
  vi.top=vi.bottom-Tv.Th*6-1;
  SImage=Tv_store( vi.left,vi.top,vi.right,vi.bottom );
  if( mode=='r' )Mod=0; else
  if( mode=='w' )Mod=1; else
  if( mode!='s' ){ if( Mod>1 )Mod = 3-Mod; }
  if( Fname!=NULL )
  if( Fname[0]>' ' )strcpy( Name,Fname ); iMENU( *this );
                   Ghelp(); Tv_restore(); return *this;
}

void static View( Mario& S )
{ if( S.N>0 )
  { setviewport( vi.left,vi.top,vi.right,vi.bottom,1 );
   int k=vi.right-vi.left+1,
       l=min( k,S.N ),
       c=color( CYAN );
   Real *W=S,Min,Max;             MinMax( W,l,Min,Max );
   static Field F={ 0,0,0,0 };     F.Jy=Min,F.Lx=l-1,F.Ly=Max-Min;
   static field f={ 0,-1,-1,0,0 }; Tv_place( &f,&F ); color( GREEN );
   Point  P;
    P.x=0;
    P.y=*W;
    for( moveto( P ),P.x=1; P.x<l; P.x++ ){ P.y=*++W; lineto( P ); }
    Tv.Height( Tv.Th/2 );
    P.x--;   color( LIGHTCYAN );
    P.y=Max; Tv.Text( South_West,P,_Fmt( "%.2lg",Max ) );
    P.y=Min; Tv.Text( North_West,P,_Fmt( "%.2lg",Min ) );
    setviewport( v.left,v.top,v.right,v.bottom,1 ); color( c );
    Tv.Height( 0 );
    Tv_place( 0,&F );
    Tv_place( &f );
  } else Tv_PutImage( vi.left,vi.top,SImage,COPY_PUT );
}
void static Example( Mario& S )
{
 static Real A=1,K=12,Rn=0; scpy( Iden,"E x a m p l e" );
 static Mlist Menu[]=                //
        { { 1,0,Iden               } // Название функции
        , { 2,4,  " F = %.1lf",&A  } // Амплитуда
        , { 0,4,"* sin( %.1lf",&K  } // Частота
        , { 0,5,   "*PI*k/%d",&L   } // Количество вводимых точек
        , { 0,4,   ") ё %.1lf",&Rn } // Добавленный шум
        , { 1 }                      //
        };
  if( S.N )L=S.N; else
  if( L<2 )L=120;
  if( Tmenu( Mlist( Menu ) )<5 ){ S.allocate( L );
   Real *W=S;
    for( int k=0; k<L; k++ )
     *W++ += A*sin( K*k*M_PI/L )+ (Rn*rand())/RAND_MAX-Rn/2;
    View( S );
  } else L=S.N;
}
void static Fput( fixed Typ, Real w )
{ switch( Typ )
  { case 1: putc( (int)w,Fs ); break;
    case 2: put2( (int)w,Fs ); break;
    case 4: putf( w,Fs );
} }
void static Dread( Mario& S )
{
 char* Key=(char*)( Mod?"r+b": Typ?"rb":"rt" );
 Real *W,w;
 int  N,n,l,Tp=Typ<3?Typ:4;
 long I;
  if( (Fs=Topen( Name,Key ) )!=NULL )
  { if( !Mod )
    { W=S.allocate( N=L>0?L:120 ); Iden[0]=0;
      for( I=0,n=0;; I++ )
      { if( Tp )l=fread( &w,Tp,1,Fs ); else{ l=fscanf( Fs,"%lf",&w );
          if( l<=0 )
          { if( I==0 )
            { rewind( Fs ); fgets( Iden,80,Fs ); strcut( Iden );
              l=fscanf( Fs,"%lf",&w );
            } else break;
        } }
        if( l<=0 )break;                    // Конец файла или данных
        if( L ){ if( n>=L )break; } else    // Ограничитель по длине
        if( n>=N )W=S.allocate( N+=n/3 )+n; // Выделение нового блока
        if( I>=J && (I-J)%K==0 )            // Начало и чередование
          { n++; *W++ = (Real)( Tp==1?*(byte*)&w :Tp==2?*(int*)&w:w ); }
      }
      if( n!=S.N )S.allocate( n ); L=S.N;   // Реальная длина на входе
    }else{ fseek( Fs,long(J)*Tp,SEEK_SET );
      for( n=0,W=S; n<L && !ferror( Fs ); n++ )
      { Fput( Tp,*W++ ); if( K>1 )fseek( Fs,long(K-1)*Tp,SEEK_CUR ); }
    } fclose( Fs ); Fs=NULL; View( S );
} }
void static Dwrite( Mario& S )
{
 char Key[]={ Mod==2?'a':'w','+',Typ?'b':'t',0 };
 fixed Tp=Typ<3?Typ:4;
  if( (Fs=Topen( Name,Key ))!=NULL ){
   int k,kv;
   Real *W=S;
    if( !Typ && Iden[0]>' ' )fprintf( Fs,"%s\n",Iden );
    if( !L )L=(S.N-J)/K;
    for( kv=J,k=0,W+=J; k<L && kv<S.N;  kv+=K,k++,W+=K )
     if( Tp )Fput( Tp,*W ); else fprintf( Fs,"%lg\n",*W );
    fclose( Fs ); Fs=NULL;
  }
}
