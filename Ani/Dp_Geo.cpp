//
//      Dp_Geo.cpp                                                   /91.01.18/
//      Комплекс формализованных географических операций
//
#include "..\Ani\Depth.h"

void Depth::allocate( int Y, int X )
{ __Depth=(float**)Allocate( Y,size_t( X )*sizeof( float ),__Depth );
}
void Depth::DtI( Point& W ) // Перевод системы счисления из географии
{    bool Rec=true;         // к равномерной индексной сетке, при этом
     Point P,Q=W;           // делается одна попытка исправления через +360°
Try: D2I( Q );              //
  if( Q.x<0.0 || Q.x>=Nx || Q.y<0.0 || Q.y>=Ny )   //! это всё надо проверить ?
  { if( Rec )
    { P=Q; Q=W; if( Q.x<0.0 )Q.x+=360.0; else Q.x-=360.0; Rec=false; goto Try;
    } W=P;
  } else W=Q;
}
Depth& Depth::basis( bool Lat )           // - true  <- широта без изменений
{ double Y=cos( Cs/Radian ),              // - false -> с переходом к меркатору
         X=sin( Cs/Radian );
  if( !isMercator )X/=cos( Base.y ); else if( !Lat )Base.y=Merkatory( Base.y );
  Geo.Euler( Base,90.0-atan2( Y,X )*Radian,(Point){ Sx,Sy } );
  return *this;
}                                             //
double Depth::scale()                         // Коэффициент для перехода к rad
{ Real Sc=(Real)1.0/Radian; isMercator=false; //   в соответствии с символом Ms
  switch( Ms ){                               //
    case 'k' : Sc*=1000.0;  case 'm': Sc/=Mile*cos( Fe ); isMercator=true;
    case '\'': Sc/=60.0;    case 'd': default:; } return Sc;
}

static const double Km2R = 1000.0/Mile/60.0/Radian;

void Sphere( Point Op, Point Np, Real& _Cs, Real& _Ds )
{
//      Расстояние и направление между двумя точками
//            Cs[°]  направление из точки Op на Np
//            Ds[km] расстояние между точками
 Real Cs=_Cs,Ds=_Ds,W,S;
  Op.y=Ph-Op.y/Radian; W=Angle( Np.x-Op.x )/Radian;
  Np.y=Ph-Np.y/Radian;
  Ds=acos( S=cos( Op.y )*cos( Np.y )+sin( Np.y )*sin( Op.y )*cos( W ) );
  Cs=acos( ( cos( Np.y )-cos( Op.y )*S )/sin( Op.y )/sin( Ds ) );
  Ds/=Km2R;
  Cs*=Radian;  if( W<0 )Cs=-Cs; _Cs=Cs; _Ds=Ds;
}
Point Sphere( Point Op, Real Cs, Real Ds ) // Cs[°], Ds[km]
{
//      Расчет новых координат при движении по большому кругу
//          из заданной точки по азимуту и расстоянию.
//              Область определения функции:    Fi ]-90:90[
//                                              La [0:360°]
 Real Y,X; Op.x/=Radian;
           Op.y=Ph-Op.y/Radian;
  if( ( Ds*=Km2R )<0 )Ds=-Ds,Cs-=180.0;
        Cs=Angle( Cs )/Radian;
  Y=acos( X=cos( Ds )*cos( Op.y )+sin( Op.y )*sin( Ds )*cos( Cs ) );
  X=( cos( Ds )-cos( Op.y )*X )/sin( Op.y )/sin( Y );
  if( X>=1.0 )X=0.0; else
  if( X<=-1.0 )X=Pi; else X=acos( X );        //! ! ! ? ? ?
  Ds=fmod( Ds,Pd );       if( Ds>Pi  )X=Pd-X;
  Op.y=( Ph-Y )*Radian;   if( Cs<0.0 )X=-X;
  Op.x=fmod( ( Op.x+X )*Radian,360.0 );
 return Op;
}
//
//      Изображение малого круга на сферической Земле
//
void Geo_Circle( _Point Centre,_Real Radius )
{ for( Real A=0.0; A<363.1; A+=6.0 )
  { Point P=Sphere( Centre,A,Radius ); Chart.DtI( P );
    if( A<4.0 )moveto( P ); else lineto( P );
} }
#if 0
Is_Chart_Sphere()
{ if( Chart.Cs==0.0 )
  { Point P={ 0,0 };          Chart.ItD( P );
    Point O={ 2,0 };          Chart.ItD( O );
    Point Q={ Chart.Nx-1,0 }; Chart.ItD( Q );
    if( fabs( Angle( P.x-Q.x ) )<fabs( Angle( O.x-P.x ) ) )return 1;
  } return 0;
}
#endif
//
//    Сплайн интерполяция на равномерном разбиении аргумента
//
#define YY( _k ) ( ((Real*)(((byte*)Y)+((_k)*Offs)))[0] )

Real ReLine( Real *Y, int N, Real Ar, int Offs )
{ if( N<=0 )return 0.0;                 //
  if( N==1 )return Y[0];                // Узлы, последовательность целых чисел
  { const int k=minmax( 0,int( Ar ),N-2 );
    const Real b=Ar-k;
    if( b==0 )return YY(k);
    if( N==2 )return YY(k) + ( YY(k+1)-YY(k) )*b;
    {                                   //
     int i,l=0,n=N-1;                   // Выборка укороченного списка точек
     Real p,_2[12],u[12];               //
      if( N>12 ){ if( (l=k-6)<0 )l=0; n=l+11;
                  if( n>=N-1 ){ n=N-1; l=n-11; } }
     _2[0]=_2[n-l]=u[0]=0;              //
      for( i=1; i<n-l; i++ )            // Предварительная подготовка сплайна
      { _2[i]=-0.5/( p=_2[i-1]/2+2 );   //
         u[i]=( ( YY( l+i+1 )-YY( l+i )*2+YY( l+i-1 ) )*3-u[i-1]/2 )/p;
      }
      for( i=n-l-1; i>=0; i-- )_2[i] = _2[i]*_2[i+1]+u[i]; p=1-b;
      return p*YY( k )                  //
           + b*YY( k+1 )                // Собственно расчет значения функции
           + ( ( p*p-1 )*p*_2[k-l]      //
             + ( b*b-1 )*b*_2[k-l+1] )/6.0;
} } }
#undef YY
