//
//  Spectral_S.cpp                             Ver 3.2  1990.12.23 - 2015.04.12
//
//  Первый пробный вариант построения объекта-вектора
//
#include "Spectral.h"
//
//      Простой объект для реализации числового ряда
//      с постоянным шагом аргумента, контроль физической
//      размерности аргумента здесь не поддерживается.
//
Real Series::operator()( Real x )
{ int k=(int)x;  if( k<0 )k=0; else if( k>N-2 )k=N-2;
  Real *v=_V+k;  if( (x-=k)==0.0 )return v[0]; return v[0]+x*(v[1]-v[0]);
}
Real Series::value( Real Ar, int Pw )   // Интерполяция по Лагранжу
{ Real yo,p; int Nb,Nf,j=(int)Ar,k=N-1; // Поиск ближайшего левого индекса
  if( j>k )j=k; else                    //
  if( j<0 )j=0;
  if( Pw>k )Pw=k;
  if( Pw<=0 )return _V[j];
  if( (Nf=++j+Pw/2)>k ){ Nf=k; Nb=k-Pw; } else
  if( (Nb=Nf-Pw)<0    ){ Nb=0; Nf=Pw;   }
  for( yo=0,j=Nb; j<=Nf; j++ )
  { for( p=1,k=Nb; k<=Nf; k++ )if( j!=k )p*=(Ar-k)/(j-k); yo+=_V[j]*p;
  } return yo;
}
void Vector::Axis( int p0, int pL )
{ if( pL<=0 || pL>=N  )pL=N-1;  End=pL;
  if( p0<=0 || p0>=pL )p0=0;  Start=p0;
}
static Sio io={ "","*.*",0,0,1,0,0,0,false };
//
//   Series                                    Ver 3.2  1990.12.23 - 2015.04.12
//   Второй вариант построения объекта-вектора
//   Реализуются процедуры ввода/вывода с графическим интерфейсом
//
void Sio::MENU( Series& S )
{ getviewsettings( &v );
  vi=v; vi.left=Tv.Tw*40+3; vi.top=vi.bottom-Tv.Th*6-1; SView( S );
  for( int ans=1,c=color( BLUE );; )
  { { Display T( 0,0,1,-2 );
      for( L=S.N;; )
      { if( S.N<=0 )Mod=0; if( Mod==3 && Typ==0 )Typ=3;
       const char *Mode[]={ " Read "," Write"," Added","Update" },
                   *Type[]={ " Text "," Byte "," Fixed"," Float" };
       Mlist Menu[]=              //
        { { 0,39,0,Ident        } // Идентификатор
        , { 1,39,0,Name         } // Имя файла
        , { 1,S.N?6:0,Mode[Mod] } // Режим чтения/записи
        , { 0,6,      Type[Typ] } // Тип данных
        , { 0,4," set %-4ld",&J } // Смещение до первой точки
        , { 0,3, " by %-3ld",&K } // Шаг считывания
        , { 0,5," plus %-5d",&L } // Количество вводимых точек
        };                        //
        T.M=(mlist*)Menu;
        T.Nm=7;
        Ghelp( "<%d> F1 Example  F2 I/O  F9 Clear",S.N ); ans=T.Answer( ans );
        Ghelp();
        if( ans==1 )Tfile( Name ); else
        if( ans==2 )Mod++,Mod&=3,L=S.N; else
        if( ans==3 )Typ--,Typ&=3;       else break;
    } }
    switch( ans )
    { case 0xF1: Example( S );                               break;
      case 0xF2: if( !Mod||Mod==3 )Dread(S); else Dwrite(S); break;
      case 0xF9: S.allocate( L=0 ); SView( S );              break;
      case _Esc: if( Act )Tv_restore(),color(c); Act=false; return;
  } }
}
void Sio::SView( Series& S )
{ if( !S.N ){ if( Act )Tv_restore(); Act=false; return; }
  if( !Act )Tv_store( vi.left,vi.top,vi.right,vi.bottom );
 setviewport( vi.left,vi.top,vi.right,vi.bottom,1 );
 int k=vi.right-vi.left+1,l=min( k,S.N ),c=color( BLUE );
 Real *W=S,Min,Max; Min=Max=*W++;
  for( k=1; k<l; W++,k++ )if( Min>*W )Min=*W; else if( Max<*W )Max=*W;
  if( Max-Min<1e-6 )Max+=1e6,Min-=1e-6;
 static Field F={ 0,0,0,0 };     F.Jy=Min,F.Lx=l-1,F.Ly=Max-Min;
 static field f={ 0,-1,-1,0,0 }; Tv_place( &f,&F ); W=S; color( GREEN );
 Bfont T( 0,Tv.Th/2 );
 Point P={ 0,W[0] };
  for( moveto( P ),P.x=1; P.x<l; P.x++ ){ P.y=*++W; lineto( P ); }
  P.x--;   color( LIGHTRED );
  P.y=Max; T.Text( South_West,P,_Fmt( "%.2lg",Max ) );
  P.y=Min; T.Text( North_West,P,_Fmt( "%.2lg",Min ) );
  Tv_place( 0,&F );
  setviewport( v.left,v.top,v.right,v.bottom,1 ); color( c ); Act=true;
  Tv_place( &f );
}
void Sio::Example( Series& S )
{ static Real A=0.1,K=12,Rn=1; scpy( Ident,"E x a m p l e" );
  Mlist Menu[]=
       { { 1,0,Ident             }       // Название функции
       , { 2,4,  " F = %.1lf",&A  }      // Амплитуда
       , { 0,4,"* sin( %.1lf",&K  }      // Частота
       , { 0,5,   "*PI*k/%d",&L  }       // Количество вводимых точек
       , { 0,4,   ") ё %.1lf",&Rn }      // Добавленный шум
       , { 1 } };                        //
  if( S.N )L=S.N; else
  if( L<2 )L=360;
  if( Tmenu( Mlist( Menu ) )<5 ){ S.allocate( L );
   Real *W=S;
    for( int k=0; k<L; k++ )
     *W++ += A*sin( K*k*M_PI/L )+ (Rn*rand())/RAND_MAX-Rn/2;
    SView( S );
  } else L=S.N;
}
void Sio::Fput( int Typ, Real w )
{ switch( Typ ){ case 1: putc( (int)w,Fs ); break;
                 case 2: put2( (int)w,Fs ); break;
                 case 4: putf( w,Fs );
}              }
void Sio::Dread( Series& S )
{
 char *Key=(char*)( Mod?"r+b": Typ?"rb":"rt" );
 Real *W,w;
 int  I,N,n,l,Tp=Typ<3?Typ:4;
  if( (Fl=Tfile( Name ))>0 )if( (Fs=fopen( Name,Key ) )!=NULL )
  { if( !Mod )
    { W=S.allocate( N=L>0?L:120 ); Ident[0]=0;
      for( I=0,n=0;; I++ )
      { if( Tp )l=fread( &w,Tp,1,Fs );
          else{ l=fscanf( Fs,"%lf",&w );
          if( l<=0 )
          { if( I==0 )
            { rewind( Fs ); fgets( Ident,80,Fs ); strcut( Ident );
              l=fscanf( Fs,"%lf",&w );
            } else break;
        } }
        if( l<=0 )break;                    // Конец файла или данных
        if( L ){ if( n>=L )break; } else    // Ограничитель по длине
        if( n>=N )W=S.allocate( N+=n/3 )+n; // Выделение нового блока
        if( I>=J && (I-J)%K==0 )            // Начало и чередование
        { n++; *W++=Tp==1?*(byte*)&w :Tp==2?*(int*)&w:w; }
      }
      if( n!=S.N )S.allocate( n ); L=S.N;   // Реальная длина на входе
    } else{ fseek( Fs,J*Tp,SEEK_SET );
      for( n=0,W=S; n<L && !ferror( Fs ); n++ )
      { Fput( Tp,*W++ ); if( K>1 )fseek( Fs,(K-1)*Tp,SEEK_CUR ); }
    } fclose( Fs ); SView( S );
  }
}
void Sio::Dwrite( Series& S )
{
 char Key[]={ Mod==2?'a':'w','+',Typ?'b':'t',0 };
 int Tp=Typ<3?Typ:4;
  if( (Fs=fopen( Name,Key ))!=NULL )
  { int k,kv;
   Real *W=S;
    if( !Typ && Ident[0]>' ' )fprintf( Fs,"%s\n",Ident );
    if( !L )L=(int)((S.N-J)/K);
    for( kv=(int)J,k=0,W+=(int)J; k<L && kv<S.N;
        kv+=(int)K,k++,W+=(int)K )
    if( Tp )Fput( Tp,*W ); else fprintf( Fs,"%lg\n",*W ); fclose( Fs );
} }
//      Реализуются процедуры ввода/вывода     Ver 3.2  1990.12.23 - 2015.04.12
//      и графического представления результатов
//
void Vector::Get( const char *Fname, char mode )
{ if( mode=='r' )io.Mod=0; else
  if( mode=='w' )io.Mod=1; else
  if( mode!='s' )if( io.Mod>1 )io.Mod=3-io.Mod;
  if( Fname && Fname[0]>' ' )scpy( io.Name,Fname );
  io.MENU( *this ); IdenS=strdup( io.Ident ); Start=0; End=N-1;
     View();
}
void Move( field& f )
{ int ans,c=color( DARKGRAY );
 bool SIZE=true; setwritemode( XOR_PUT ); field wf;
  if( f.Lx<=0 )f.Lx=100-f.Jx;
  do
  { wf=f; Tv_place( &wf,0 ); Tv_rect(); ans=Tv_getc(); Tv_rect();
    switch( ans )
    { case North: if( SIZE )f.Jy++; else f.Ly++; break;
      case South: if( SIZE )f.Jy--; else f.Ly--; break;
      case East : if( SIZE )f.Jx++; else f.Lx++; break;
      case West : if( SIZE )f.Jx--; else f.Lx--; break;
      case 0x0d :
      case 0xFa : SIZE=!SIZE;
  } } while( ans!=_Esc ); setwritemode( COPY_PUT ); color( c );
}
