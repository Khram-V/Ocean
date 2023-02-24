//
//   Curve.Cpp				Ver 3.2  /89.01.27-93.03.22/
//   Несколько программ анализа "Point" векторов
//
#include "..\Matrix\Curve.Hxx"

Curve:: Curve( fixed S ){ _C=0; _2=0; N=0; if( S>0 )allocate( S ); }
Curve::~Curve() 	{ free(); }

Curve& Curve::allocate( fixed Sz )
{ if(_2 )_2=(float*)Allocate( 0,_2 );
	 _C=(Point*)Allocate( (N=Sz)*sizeof( Point ),_C ); return *this;
}
Curve& Curve::free()
{ if( _2 )_2=(float*)Allocate( 0,_2 );
  if( _C )_C=(Point*)Allocate( N=0,_C ); return *this;
}
//Point& Curve::operator[]( int Ind )
//{ if( fixed( Ind )>=N )Break( "?Curve.%d[%d] off range",N,Ind );
//  return _C[Ind];
//}
Curve& Curve::operator += ( Point& P )
{ Point *C=_C;
  for( int k=0; k<N; k++ )*C++ += P; return *this;
}
#define  _X( k ) ( _C[(k)].x )
				 //
Curve::find( float Ar )		 // Двоичный поиск ближайшего левого
{ int i=0,k,n=N-1,d=_X(1)>_X(0); //  индекса для заданного аргумента
  while( n-i>1 )		 //   с учетом знака его прироста
  {   k=(n+i)/2;		 //
    if( d ){ if( Ar<_X(k) )n=k; else i=k; }
      else { if( Ar>_X(k) )n=k; else i=k; }
  } return i;
}
float Curve::value( float Ar, int Pw ) // Интерполяция по Лагранжу
{			       //
 int i=find( Ar ),Nb,Nf,n=N-1; // Поиск ближайшего левого индекса
  if( !Pw  )return _C[i].y;
  if( Pw>n )Pw=n;	       //
  if( (Nf=i+(Pw/2)+1)>n )Nf=n,Nb=n-Pw; else
  if( (Nb=Nf-Pw)<0      )Nb=0,Nf=Pw;
 float yo=0,po,pi; i=Nb;
  do
  { n=Nb; po=pi=1;
    do if(   i!=n  ){ pi *=_X(i)-_X(n); po*=Ar-_X(n); }
    while( ++n<=Nf ); yo +=_C[i].y*po/pi;
  } while( ++i<=Nf ); return yo;
}
#define X( k ) ( P[k].x )
#define Y( k ) ( P[k].y )
float Curve::operator()( float Ar )
{  int k;
 Point *P=_C+( k=find( Ar ) ); float h=P[1].x-P[0].x, b=( Ar-P[0].x )/h;
  if( _2 )
  { float a=1-b; return a*P[0].y
		      + b*P[1].y
		      + ( (a*a-1)*a*_2[k]
			+ (b*b-1)*b*_2[k+1] )*(h*h)/6.0;
  } return P[0].y + b*(P[1].y-P[0].y);
}
void Curve::SpLine( float yp1, float ypn )
{ if( N<3 ){ if( _2 )Allocate( 0,_2 ); _2=0; return; }
 int   i,k,n=N-1;
 float p,qn,sig,un,*u;
 Point *P=_C;	       _2=(float*)Allocate( N*sizeof( float ),_2 );
			u=(float*)Allocate( n*sizeof( float ) );
  if( yp1>.99e30 )_2[0]=u[0]=0.0; else
    { _2[0]=-0.5;
      p=X(1)-X(0); u[0]=3*( (Y(1)-Y(0))/p - yp1 )/p;
    }      	     P++;
  for( i=1; i<n; i++,P++ )
  { sig=( X(0)-X(-1) )/( X(1)-X(-1) ); p=sig*_2[i-1]+2;
   _2[i]=( sig-1 )/p;
    u[i]=( Y(1)-Y(0)  )/( X(1)-X(0) )
	-( Y(0)-Y(-1) )/( X(0)-X(-1) );
    u[i]=(6.0*u[i]/(X(1)-X(-1))-sig*u[i-1])/p;
  }
  if( ypn>.99e30 )qn=un=0.0; else
    { qn=0.5;
      p=X(0)-X(-1); un=3*( ypn-( Y(0)-Y(-1) )/p )/p;
    }
			 _2[n]=( un-qn*u[n-1] )/( qn*_2[n-1]+1.0 );
  for( k=n-1; k>=0; k-- )_2[k]=_2[k]*_2[k+1]+u[k];
  Allocate( 0,u );
}
#undef X
#undef Y
//
//	Подготовка экстремумов функции и аргумента
//
void Curve::Extreme( Field& Fm )
{ for( int i=0; i<N; i++ )
  { Point P=_C[i];
    if( i )
    { if( Fm.Jy>P.y )Fm.Jy=P.y; else if( Fm.Ly<P.y )Fm.Ly=P.y;
      if( Fm.Jx>P.x )Fm.Jx=P.x; else if( Fm.Lx<P.x )Fm.Lx=P.x;
    } else
    { Fm.Lx=( Fm.Jx=P.x )+1.0e-12;
      Fm.Ly=( Fm.Jy=P.y )+1.0e-12;
  } } Fm.Ly-=Fm.Jy;
      Fm.Lx-=Fm.Jx;
}
//
//	Поверхность построенная на кривых параллельного аргумента
//
Surface:: Surface( fixed _n )
{	 _S=0; M=0; _sp=0; dF1=dFN=1e32; if( _n )allocate( _n );
}
Surface::~Surface(){ free(); }
Surface& Surface::allocate( fixed Sz )
{ _A.allocate( M=Sz );
  _S=(Curve*)Allocate( M*sizeof( Curve ),_S ); _sp=0; return *this;
}
Surface& Surface::free()
{        _sp=0;		     _A.free();
  for( int i=0; i<M; i++ )_S[i].free(); Allocate( 0,_S ); _S=0;  M=0;
  return *this;
}
//Curve& Surface::operator[]( int Ind )
//{ if( fixed( Ind )>=M )Break( "?Surface.%d[%d] off range",M,Ind );
//  return _S[Ind];
//}
float Surface::operator()( float X, float Y )
{
 int k;
 static float _X=0.0;
  if( (_sp&2)==0 || X!=_X )
  { for( k=0; k<M; k++ )_A[k].y=_S[k]( X ); _X=X; _sp|=2;
    if( _sp&1 )_A.SpLine( dF1,dFN );
  } return _A( Y );
}
void Surface::SpLine	// Активизация Сплайн-интерполяции
	   ( float y1,	// Производная в начале и в
	     float yN	// конце поверхности (по нормали к кривым)
	   )		//
{  _sp=1; for( int i=0; i<M; i++ )_S[i].SpLine(); dF1=y1; dFN=yN;
}
void Surface::Extreme( float &Xmin, float &Xmax,  // Аргумент кривых
		       float &Ymin, float &Ymax,  // Ширина полосы
		       float &Zmin, float &Zmax ) // Значения функций
{
 int i,j;
  for( i=0; i<M; i++ )
  { Curve &L=_S[i];
    Point P=_A[i];
    if( !i ){ Ymax=( Ymin=P.x )+1.0e-12; }
       else { if( Ymin>P.x )Ymin=P.x; else if( Ymax<P.x )Ymax=P.x; }
    for( j=0; j<L.N; j++ )
    { P=L[j];
      if( i || j )
      { if( Zmin>P.y )Zmin=P.y; else if( Zmax<P.y )Zmax=P.y;
	if( Xmin>P.x )Xmin=P.x; else if( Xmax<P.x )Xmax=P.x;
      } else
      { Zmax=( Zmin=P.y )+1.0e-12;
	Xmax=( Xmin=P.x )+1.0e-12;
    } }
} }

#if 0

CONST	EPS=1.0E-12;
TYPE	VREAL=ARRAY[0..MAXINT] OF REAL;

//   NAPROG				 VER 3.2  /89.01.27/
//   Несколько программ стандартного обеспечения:
//     CALL SPQDS ( F",DX,N,FX,DFX );
//     CALL SPQDSM( F",X",N,FX,DFX );
//     CALL LAGRID( F",X",N,NP,XF,FX,DFX );
//     FX:= LAGRIN( F",X",N,NP,XF) );
//	    F"	 - Функция
//	    X"	 - Аргумент
//	    FX	 - Значение функции
//	    XF	 - Значение аргумента
//	    DX	 - Интервал аргумента
//	    I,N  - Индексный интервал вектора
//	    K	 - Степень интерполяционного полинома

TYPE	VREAL=ARRAY[0..MAXINT] OF REAL;

//   S P Q D S					 VER 1.0  /87.12.21/
//   Вычисление интеграла и производной трапециями с равным шагом
PROCEDURE SPQDS
( VAR Y:VREAL; DX:REAL; 	// Функция и шаг аргумента
      N:INTEGER;		// Количество точек
  VAR S,SM :REAL );		// Величина площади и момента
VAR	K:INTEGER;
	Y1,Y2,DS:REAL;
 BEGIN K:=1;
   S :=0.0; DX:=DX/2.0;
   SM:=0.0; Y1:=Y[0];
   WHILE K<N DO BEGIN
     Y2:=Y[K];	DS:=(Y2+Y1)*DX;
     SM:=SM+(K*2-N)*DX*DS+(Y2-Y1)*DX*DX/3.0; S:=S+DS;
     Y1:=Y2; K:=K+1;
   END;
 END (* S P Q D S *);

//   S P Q D S M				 VER 1.0  /87.05.01/
//   Вычисление интервала и оценка производной методом трапеций
PROCEDURE SPQDSM
( VAR Y,X  :VREAL;		// Функция и ее аргумент
      N    :INTEGER;		// Количество точек
  VAR S,SM :REAL );		// Величина площади и момента
VAR	K :INTEGER;
	X1,X2,Y1,Y2,DX,DS:REAL;
 BEGIN
   S :=0.0; X1:=X[0];
   SM:=0.0; Y1:=Y[0]; K:=1;
   WHILE K<N DO BEGIN
     X2:=X[K];	DX:=(X2-X1)/2.0;
     Y2:=Y[K];	DS:=(Y2+Y1)*DX;
     SM:=SM+DS*(X2+X1)/2.0+(Y2-Y1)*DX*DX/3.0; S:=S+DS;
     X1:=X2;
     Y1:=Y2; K:=K+1;
   END;
   SM:=SM-S*(X[N-1]+X[0])/2.0;
 END (* S P Q D S M *);

//   L A G R I D				 VER 0.1  /87.05.18/
//   Интерполяция и Дифференцирование Лагранжа
PROCEDURE LAGRID
( VAR Y,X:VREAL;	     // Табличная функция и ее аргумент
      N  :INTEGER;	     // Количество расчетных точек
      NP :INTEGER;	     // Степень интерполяционного полинома
      XS :REAL; 	     // Заданный аргумент
  VAR FX :REAL; 	     // Расчетное значение функции
  VAR DFX:REAL );	     // и ее производной
 VAR
   J,K,L    :INTEGER;
   NB,NF,IND:INTEGER;
   Y0,P0,P1 :REAL;
   Y1,S1,P2 :REAL;
 BEGIN
   N:=N-1;
   NB:=0; NF:=N;
   IF NP>N THEN NP:=N;
   IF NP>0 THEN BEGIN
     IND:=FINDID( X[0],N,XS ); // Поиск ближайшего элемента
     NF:=IND+( NP DIV 2 )+1;   // - правая граница аппроксимации
     NB:=NF-NP; 	       // - левая  -//-
     IF NB<0 THEN BEGIN NB:=0; NF:=NP; END;
     IF NF>N THEN BEGIN NF:=N; NB:=N-NP; END;
   END;

   Y0:=0.0;
   Y1:=0.0; J:=NB;
   REPEAT   K:=NB;
     S1:=0.0;
     P0:=1.0;
     P1:=1.0;
     REPEAT L:=NB;
       IF J<>K THEN BEGIN
	 P0:=P0*(XS-X[K]);	 // числитель
	 P1:=P1*(X[J]-X[K]);	 // знаменатель
	 P2:=1.0;		 // производная
	 REPEAT IF (L<>K) AND (L<>J) THEN P2:=P2*(XS-X[L]); L:=L+1
	 UNTIL L>NF;
       S1:=S1+P2;
       END; K:=K+1
     UNTIL K>NF;
     Y0:=Y0+Y[J]*P0/P1;
     Y1:=Y1+Y[J]*S1/P1;
	 J:=J+1;
   UNTIL J>NF;
   FX :=Y0;
   DFX:=Y1;
 END  (* LAGRID *);

//	F I N D E L				VER 2.0  /89.07.06/
//  Установка индексации массива-аргумента по заданному интервалу
//	CALL FINDEL( X",I,J,N,XS,DX );
//	  X"	- аргумент
//	  XS	- опорная точка поиска
//	  DX	- интервал аргумента
//	  I,N	- индексный интервал вектора
TYPE	VREAL=ARRAY[0..MAXINT] OF REAL;
PROCEDURE FINDEL
( VAR X    :VREAL;		// Исследуемый массив
  VAR I,N  :INTEGER;		// 'I'..'N' границы поиска
  VAR XS,DX:REAL );		// Аргумент и его интервал
 VAR
	XI,XN	:REAL;
	KI,KN,K :INTEGER;
 BEGIN
   KI:=I; K:=KI; KN:=N;
   XI:=XS-DX/2.0;
   XN:=XS+DX/2.0;
   WHILE K<=N DO BEGIN
	IF ABS( X[K]-XI )<ABS( X[KI]-XI ) THEN KI:=K;
	IF ABS( X[K]-XN )<ABS( X[KN]-XN ) THEN KN:=K; K:=K+1
   END;
   IF KI>KN THEN BEGIN K:=KI; KI:=KN; KN:=K END;
   I:=KI; XS:=( X[KN]+X[KI] )/2.0;
   N:=KN; DX:=	X[KN]-X[KI];
 END   (* F I N D E L *);
#endif
