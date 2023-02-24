//
//  S_IO.cpp				Ver 3.2  91.05.09
//	Второй вариант построения объекта-вектора,
//      реализуются процедуры ввода/вывода
//
#include <StdIO.h>
#include <StdLib.h>
#include "..\Matrix\Series.Hxx"
#include "..\Graphics\Tv_graph.Hxx"
				//
long Tfile( int,int,string );	// Программа поиска имени
				//
static Act=FALSE;	// Признак сохраненного изображения
static FILE *Fs;	// Единый (временный) указатель файла
static struct viewporttype v,vi;
static struct Sio{
 char  Ident[81],	// Текстовый идентификатор
       Name[81];	// Имя файла
 long  Fl,J,K;		// Длина в байтах, отступ и шаг
 fixed L;		// Длина выборки
 byte  Typ,		// 0-Text; 1-Byte;  2-Integer; 4-Float
       Mod;		// 0-Read; 1-Write; 2-Added;   3-Update

 void MENU   ( Series& );
 void View   ( Series& );
 void Dread  ( Series& );
 void Dwrite ( Series& );
 void Example( Series& );

} io={ "","*.*",0,0,1,0,0,0 };

string Series::IOName(){ return io.Ident; }

Series& Series::IO( string Fname, char mode ){
  if( mode=='r' )io.Mod=0; else
  if( mode=='w' )io.Mod=1; else
  if( mode!='s' )if( io.Mod>1 )io.Mod=3-io.Mod;
  if( Fname && Fname[0]>' ' )scpy( io.Name,Fname );
  Ghelp(); io.MENU( *this );
  Ghelp();
 return *this;
}
void Sio::MENU( Series& S )
{ getviewsettings( &v );
  vi=v; vi.left=Tv.Tw*40+3; vi.top=vi.bottom-Tv.Th*6-1;
  View( S );
  for( int ans=1,c=color( CYAN );; )
  { {
     Display T( 0,0,1,-2 );
      for( L=S.N;; )
      { if( S.N<=0 )Mod=0; if( Mod==3 && Typ==0 )Typ=3;
       static char *Mode[]={ " Read "," Write"," Added","Update" },
		   *Type[]={ " Text "," Byte "," Fixed"," Float" };
       Mlist Menu[]=
	{ { 0,39,0,Ident	}	// Идентификатор
	, { 1,39,0,Name	        }	// Имя файла
	, { 1,S.N?6:0,Mode[Mod] }	// Режим чтения/записи
	, { 0,6,      Type[Typ] }	// Тип данных
	, { 0,4," set %-4ld",&J }	// Смещение до первой точки
	, { 0,3, " by %-3ld",&K }	// Шаг считывания
	, { 0,5," plus %-5d",&L }	// Количество вводимых точек
	};
	T.M=(mlist*)Menu;
	T.Nm=7;
	Ghelp( "<%d> F1 Example  F2 I/O  F9 Clear",S.N );
	ans=T.Answer( ans );
	if( ans==1 )Tfile( 1,-6,Name ); else
	if( ans==2 )Mod++,Mod&=3,L=S.N; else
	if( ans==3 )Typ--,Typ&=3;       else break;
    } }
    switch( ans )
    { case 0xF1: Example( S );				     break;
      case 0xF2: if( !Mod||Mod==3 )Dread(S); else Dwrite(S); break;
      case 0xF9: S.allocate( L=0 ); View( S );		     break;
      case Esc : if( Act )Tv_restore(),color(c); Act=FALSE; return;
    }
  }
}
void Sio::View( Series& S )
{
  if( !S.N ){ if( Act )Tv_restore(); Act=FALSE; return; }
  if( !Act )Tv_store( vi.left,vi.top,vi.right,vi.bottom );
 setviewport( vi.left,vi.top,vi.right,vi.bottom,1 );
 int k=vi.right-vi.left+1,l=min( k,S.N ),c=color( CYAN );
 float *W=S,Min,Max; Min=Max=*W++;
  for( k=1; k<l; W++,k++ )if( Min>*W )Min=*W; else if( Max<*W )Max=*W;
  if( Max-Min<1e-6 )Max+=1e6,Min-=1e-6;
 static Field F={ 0,0,0,0 };   F.Jy=Min,F.Lx=l-1,F.Ly=Max-Min;
 static field f={ 0,-1,-1,0,0 }; Tv_place( &f,&F ); W=S; color( GREEN );
 Bfont T( 0,Tv.Th/2 );
 Point P; P.x=0;
	  P.y=*W;
  for( moveto( P ),P.x=1; P.x<l; P.x++ ){ P.y=*++W; lineto( P ); }
  P.x--;   color( LIGHTCYAN );
  P.y=Max; T.Text( South_West,P,_Fmt( "%.2g",Max ) );
  P.y=Min; T.Text( North_West,P,_Fmt( "%.2g",Min ) );
  Tv_place( 0,&F );
  setviewport( v.left,v.top,v.right,v.bottom,1 ); color( c ); Act=TRUE;
  Tv_place( &f );
}
void Sio::Example( Series& S )
{
 static float A=0.1,K=12,Rn=1; scpy( Ident,"E x a m p l e" );
 static Mlist Menu[]=			//
	{ { 1,0,Ident		  }	// Hазвание функции
	, { 2,4,  " F = %.1f",&A  }	// Амплитуда
	, { 0,4,"* sin( %.1f",&K  }	// Частота
	, { 0,5,   "*PI*k/%d",&L  }	// Количество вводимых точек
	, { 0,4,   ") ё %.1f",&Rn }	// Добавленный шум
	, { 1 }				//
	};
  if( S.N )L=S.N; else
  if( L<2 )L=360;
  if( Tmenu( Mlist( Menu ) )<5 ){ S.allocate( L );
   float *W=S;
    for( int k=0; k<L; k++ )
     *W++ += A*sin( K*k*M_PI/L )+ (Rn*rand())/RAND_MAX-Rn/2;
    View( S );
  } else L=S.N;
}
static void Fput( fixed Typ, float w )
{ switch( Typ )
  { case 1: putc( (int)w,Fs ); break;
    case 2: put2( (int)w,Fs ); break;
    case 4:fwrite( &w,4,1,Fs );
  }
}
void Sio::Dread( Series& S )
{
 char *Key=Mod?"r+b": Typ?"rb":"rt";
 float *W,w;
 int  N,n,l,Tp=Typ<3?Typ:4;
 long I;
  if( (Fl=Tfile( 1,-1,Name ))>0 )
  if( (Fs=fopen( Name,Key ) )!=NULL )
  { if( !Mod )
    { W=S.allocate( N=L>0?L:120 ); Ident[0]=0;
      for( I=0,n=0;; I++ )
      {	if( Tp )l=fread( &w,Tp,1,Fs ); else{ l=fscanf( Fs,"%f",&w );
	  if( l<=0 )
	  { if( I==0 )
	    { rewind( Fs ); fgets( Ident,80,Fs ); strcut( Ident );
	      l=fscanf( Fs,"%f",&w );
	    }else break;
	} }
	if( l<=0 )break;		    // Конец файла или данных
	if( L ){ if( n>=L )break; } else    // Ограничитель по длине
	if( n>=N )W=S.allocate( N+=n/3 )+n; // Выделение нового блока
	if( I>=J && (I-J)%K==0 )	    // Начало и чередование
	{ n++; *W++=Tp==1?*(byte*)&w :Tp==2?*(int*)&w:w; }
      }
      if( n!=S.N )S.allocate( n ); L=S.N;   // Реальная длина на входе
    }else{ fseek( Fs,J*Tp,SEEK_SET );
      for( n=0,W=S; n<L && !ferror( Fs ); n++ )
      {	Fput( Tp,*W++ ); if( K>1 )fseek( Fs,(K-1)*Tp,SEEK_CUR ); }
    } fclose( Fs ); Fs=NULL; View( S );
  }
}
void Sio::Dwrite( Series& S )
{
 char Key[]={ Mod==2?'a':'w','+',Typ?'b':'t',0 };
 fixed Tp=Typ<3?Typ:4;
  if( (Fs=fopen( Name,Key ))!=NULL ){
   fixed k,kv;
   float *W=S;
    if( !Typ && Ident[0]>' ' )fprintf( Fs,"%s\n",Ident );
    if( !L )L=(fixed)((S.N-J)/K);
    for( kv=(fixed)J,k=0,W+=(fixed)J; k<L && kv<S.N;
	kv+=(fixed)K,k++,W+=(fixed)K )
    if( Tp )Fput( Tp,*W ); else fprintf( Fs,"%g\n",*W );
    fclose( Fs ); Fs=NULL;
  }
}
