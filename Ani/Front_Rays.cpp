//
//      Пакет процедур из TIME_HEX.cpp          95-may-01
//      Расчет лучей по полю времен пробега волны
//
static bool Angle_Ray( int j, int i, int J, int I )
{ j+=Y; i+=X;
  J+=Y; I+=X;
  if( ubnd(j,Ny) || ubnd(i,Nx) || ubnd(J,Ny) || ubnd(I,Nx) )return false;
  if( Chart[j][i]==0 || Chart[J][I]==0                     )return false;
  D = atan2( _D[J][I]-T, _D[j][i]-T );                      return true;
}
//     Заполнение матрицы направлений для нормалей к фронту
//
static void Time_Ray()
{ float V,W,Az[8];       // Mассивы для сохранения азимутов
 int y=Y,x=X,k,n;        // Счетчик принятых к осреднению азимутов
                         //
  if( !_R )_R=(float**)Allocate( Ny,Nx*sizeof(float) );

  for( Y=0; Y<Ny; Thelp( "Wait please [Y=%d]",++Y ) )
  for( X=0; X<Nx; X++ )if( Chart[Y][X]>0 ){ T=_D[Y][X]; n=0;
    if( Angle_Ray( 1, 0, 0, 1 ) )Az[n++]=D;
    if( Angle_Ray( 0, 1,-1, 0 ) )Az[n++]=D+Ph;
    if( Angle_Ray(-1, 0, 0,-1 ) )Az[n++]=D+Pi;
    if( Angle_Ray( 0,-1, 1, 0 ) )Az[n++]=D-Ph;
    if( Angle_Ray( 1, 1,-1, 1 ) )Az[n++]=D+Ph*0.5;
    if( Angle_Ray(-1, 1,-1,-1 ) )Az[n++]=D+Ph*1.5;
    if( Angle_Ray(-1,-1, 1,-1 ) )Az[n++]=D-Ph*1.5;
    if( Angle_Ray( 1,-1, 1, 1 ) )Az[n++]=D-Ph*0.5;
    for( V=0,k=0; k<n; k++ )
    { W=fmod( Az[k]+Pd,Pd );
      if( k )if( fabs( V/(k+1)-W )>Pi ){ if( W>V )W-=Pd; else W+=Pd; }
      V+=W;
    }_R[Y][X]=V/n;
  } Y=y,X=x;
}
//     Построение луча по матрице направлений
//
#define _Trace_ 0
static int Current_Ray( int& L, Real Pc=0, bool fr=false )
{                         //
 Point R = { X,Y };       // Py,Px - координаты стартовой точки
 Real  DY,dY=0,DX,dX=0,   // Смещения метода коррекции
       A,a,Angle;         // Angle - контрольное направление
 int M,l,y,x;             // M - допустимое число крутых поворотов
  moveto( R );            //
#if _Trace_
  FILE *Tr=NULL; Point S; Chart.ItD( S=R );
  if( L>0 && Tv_getk()&SHIFT && (Tv.Color==LIGHTMAGENTA && !fr || Tv.Color==MAGENTA && fr) )
  { color( BLUE );
    sprintf( Msg,"Tr(%d,%d)%1.0f,%1.0f,%1.0f.txt",Y,X,Chart[Y][X],_D[Y][X],_R[Y][X]*Radian );
    Tr=fopen( Msg,"at" );
    if( Tr ){ color( BLUE ); fprintf( Tr,";  широта   долгота глубина время направление - луч%+1.0lf град.\n"
              " %8.4f %9.4f %5.0f %7.2f %6.1lf\n",Pc*Radian,S.y,S.x,Chart[Y][X],_D[Y][X],(_R[Y][X]+Pc)*Radian );
  } }
#endif
  for( M=l=0,A=Angle=_R[Y][X]+Pc; M<Nbr; )
  { R.y += DY=Rst*cos( Angle );
    R.x += DX=Rst*sin( Angle );
    if( l ){ R.y+=(DY-dY)/2;
             R.x+=(DX-dX)/2;
    } dY=DY; y=int( R.y+0.5 ); if( ubnd( y,Ny ) )break;
      dX=DX; x=int( R.x+0.5 ); if( ubnd( x,Nx ) )break; ++l; lineto( R ); // R.x,R.y );
                               if( Chart[y][x]<=0.0 )break;
    if( fr )if( cos( Angle-A )<Rsr )break;
    a=Angle; if( cos((Angle=_R[y][x]+Pc)-a)<0 )++M;
    if( !L ){ if( Tv_getw() ){ M=-1; break; }
              if( l>=0x7FFF )break;
    }    else if( l>=L )break;
#if _Trace_
    if( Tr )
    { Chart.ItD( S=R );
      fprintf( Tr," %8.4lf %9.4lf %5.0lf %7.2lf %6.1lf\n",S.y,S.x,Chart( R ),Value( _D,R.x,R.y,Nx,Ny ),Angle*Radian );
    }
#endif
  } L=l;
#if _Trace_
  if( Tr )fclose( Tr );
#endif
  return M>=0;
}
