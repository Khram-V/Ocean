//
//   Комплекс графических программ класса Depth                   /90.08.20/
//
#include "..\Graphics\Tv_Graph.h"
//
//   Простая прорисовка цветной карты с теневой разметкой градиентов
//     Amax(-) - белые вершины; Amin(+) - черные глубины
//
static union RGBi{ unsigned i; struct{ char b,g,r,a; }; } sea[152]; //sea[72],grn[80];
static void Palette()    //
{ static bool wLx=false; // Настройка исходной палитры
  if( !wLx )             //  -- процедура отрабатывает единожды
  { int k; RGBi *grn=sea+72; wLx=true;
    for( k=0; k<72; k++ ){ sea[k].r=8*k/5; sea[k].g=20*k/6; sea[k].b=64+8*k/3; }
    for( k=0; k<80; k++ ){ grn[k].g=k<20?k*4+172:(k<60?255-(k-20)*4:92+(k-60)*8);
                           grn[k].r=k<20?64*k/5:(k<60?255-(k-20)*2:172+(k-60)*4);
                           grn[k].b=k<60?0:(k-60)*12; }
} }
void Color_Paint( float **_D, int Jy,int Jx, int Ly,int Lx )
{                            // поле должно быть настроено на { 0,0,Nx-1,Ny-1 }
 int  M,L;                   // объем графического образа в оперативной памяти
 Real X,Y;
 HIMG Scr=Allocate( L=imagesize
                   ( Tv_port.left,Tv_port.top,Tv_port.right,Tv_port.bottom ) );
  Palette();                     // установка статической палитры в массиве sea
  memset( Scr,0xE0,L );                          // 224 = 0xE0 - три бита слева
  ((short*)Scr)[0]=L=Tv_port.right-Tv_port.left; // размерность растрового
  ((short*)Scr)[1]=M=Tv_port.bottom-Tv_port.top; // массива bitmap [ MxL ]
//
//      Прорисовка раскрашенной географической карты
//
#pragma omp parallel for private( X,Y )
  for( int y=0; y<M; y++ )
  { int y1 = int( Y = (Ly-1)*Real( y )/( M-1 ) );
    int y2 = (Ly<=M) ? y1+1 : (Ly-1)*Real( y+1 )/( M-1 );
    if( y2>=Ly )y2=Ly-1;       // то,
    if( y1>y2 )y1=y2-1;        //   на всякий случай
   float *A = _D[Jy+y1]+Jx,    //
         *B = _D[Jy+y2]+Jx;    // адреса двух смежных по экрану строк в массиве
   Real   V = Y-y1;            //
    for( int x=0; x<L; x++ )
    { int x1 = int( X = (Lx-1)*Real( x )/( L-1 ) ),d;
      int x2 = (Lx<=L) ? x1+1 : (Lx-1)*Real( x+1 )/( L-1 );
      if( x2>=Lx )x2=Lx-1;       // то тоже
      if( x1>x2 )x1=x2-1;        // - на всякий случай
     Real W = X-x1,              //
          U = (A[x1]*(W-1) - A[x2]*W)*(1-V)  //
            + (B[x1]*(W-1) - B[x2]*W)*V;     // масштабирование по палитре
      if( U<=0.0 )d=71+int( U*0.0072 ); else // масштаб к 10000-ным экстремумам
                  d=72+int( U*0.0256 );      // sqrt( U ) // 0.72*(0.75^2)/0.48
      U = tanh( ( ( A[x1]-A[x2] )*( 1-V ) + ( B[x1]-B[x2] )*V  // это тени
                - ( A[x1]-B[x1] )*( 1-W ) - ( A[x2]-B[x2] )*W ) / 600 );
     RGBi C = sea[ minmax( 0,d,151 ) ];               // max( 0,min( 151,d ) )
      C.b += U * ( U>0 ? 255-C.b : C.b );             // попиксельный перебор
      C.g += U * ( U>0 ? 255-C.g : C.g );             // освещенности с тенями
      C.r += U * ( U>0 ? 255-C.r : C.r );             //       ( и призраками )
      ((unsigned*)Scr)[y*L+x+1] = C.i;                //
    }
  } Tv_PutImage( Tv_port.left,Tv_port.top,Scr );
    Allocate( 0,Scr );
}
