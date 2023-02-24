#include "..\Graphics\TV_Graph.h"
//
//      Процедуры стандартной разметки осей                     /91.01.14/
//
#if 0
float Tv_step( float Di )        // Вычисление шага
{ float iPart=floor( Di=log10( Di ) );
        Di = exp( (Di-iPart)*M_LN10 );
 return pow( 10.0,iPart )*( Di>6.0 ?2.0 : Di>3.0 ?1.0 : Di>1.5 ?0.5:0.2 );
}
#endif
//      Разметка графического поля в соответствии с физическими экстремумами
//
void Tv_axis( bool lab )
{
 Field F,f={ 0,0,1,1 };
 Point P;
 point p;
 Bfont S( _Small_font,2*Tv.Th/3 );
  Tv_place( 0,&f ); F=f; Tv_place( 0,&f ); P.x=F.Jx; P.y=F.Jy; p=P;
  Tv_rect();
  p.y+=2; f.Jy=Tv_step( F.Ly ); F.Ly+=F.Jy;
  p.x-=2; f.Jx=Tv_step( F.Lx ); F.Lx+=F.Jx;
//axis-X
       P.y=Tv_Y( p.y );
  for( P.x=f.Jx*floor( F.Jx/f.Jx+1 ); P.x<F.Lx; P.x+=f.Jx )
  { line( P.x,F.Jy,P.x,F.Ly );
    if( lab )S.Text( South,point( P ),_Fmt( "%g",P.x ) );
  }
//axis-Y
       P.x=Tv_X( p.x+3 );
  for( P.y=f.Jy*floor( F.Jy/f.Jy+1 ); P.y<F.Ly; P.y+=f.Jy )
  { line( F.Jx,P.y,F.Lx,P.y );
    if( lab )S.Text( West,point( P ),_Fmt( "%g-",P.y ) );
  }
}
