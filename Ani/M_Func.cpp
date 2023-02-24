//
//      Mario.Function
//      Комплекс функций для преобразования мариограмм
//
#include "Mario.h"
#define  Msize ( sizeof( Menu )/sizeof( Mlist ) )

void Function()
{
 int k,Nd,c=color( CYAN ),z;
 Real t,*Dst,*Src;
 fixed ans=0;
 Mario &M=Ms[Km]; z=M.Min>1e-6;
  if( M.flag&1 )
  {
   static Real Mul=1,Add=0;
   Mlist Menu[]=
    { { 1, 0, "\tПростые операции" }
    , { 2, 5, " F +"     },{ 0,6,"%5.3lf",&Add },{ 0,5,  " - F" }
    , { 1, 5, " F x"     },{ 0,6,"%5.3lf",&Mul },{ 0,5*z," / F" }
    , { 1, 8, " F -Mid"  },{ 0,8," F -F(o)" }
    , { 1,8*z," Log    " },{ 0,8," Exp  "  }
    , { 1,8*z," Log10  " },{ 0,8," Exp10"  }
    };
   Display T( Mlist( Menu ) );
    do ans=T.Answer( ans ); while( ans==2 || ans==5 ); T.Back();
    if( ans<Msize )
    { for( Nd=0;; )
      { while( Nd<Nm )if( Ms[Nd].flag&1 )break; else ++Nd;
                      if( Nd>=Nm )break;
       Mario &S=Ms[Nd];
        for( t=(Dst=S.V)[0],k=0; k<S.N; k++,Dst++ )
        { if( S.Typ==3 )
          if( *Dst<S.Min )*Dst=S.Mid;
          switch( ans )
          { case  1:*Dst+=Add;          break;
            case  4:*Dst*=Mul;          break;
            case  3:*Dst= Add- *Dst;    break;
            case  6:*Dst= Mul/ *Dst;    break;
            case  7:*Dst-=S.Mid;        break;
            case  8:*Dst-=t;            break;
            case  9:*Dst= log( *Dst );  break;
            case 10:*Dst= exp( *Dst );  break;
            case 11:*Dst=log10( *Dst ); break;
            case 12:*Dst=pow( 10.0,*Dst );
          }
        } if( S.Typ==3 )S.Typ=1,S.Fmt=2; ++Nd;
      }   m_View();
    }
  } else
  { Mlist Menu[] = { { 1,0,"\t Применить" }
    , { 2,5," F +" },{ 0,0,"\tблок" },{ 0,5,  " - F " }
    , { 1,5," F *" },{ 0,0,"\tблок" },{ 0,5*z," / F " }
    };
    Thelp( "Операции с текущим рядом для группы помеченных записей" );
    ans=Tmenu( Mlist( Menu ),1,-1 );
    if( ans<Msize )
    { for( Nd=0; Nd<Nm; Nd++ )
      if( Ms[Nd].flag&1 )
      { Event t=M.JT;
        Mario Sm,&S=Ms[Nd]; Sm.allocate( M.N ); Dst=Sm.V; Src=M.V;
        for( k=0; k<M.N; ++k )
        { Real W=S( t ),
               V=Src[0];
          if( M.Typ==3 )if( V<M.Min )V=M.Mid;
          if( S.Typ==3 )if( W<S.Min )W=S.Mid;
          switch( ans )
          { case 1: *Dst = W + V; break;
            case 3: *Dst = W - V; break;
            case 4: *Dst = W * V; break;
            case 6: *Dst = W / V;
          } t+=M.dt; ++Dst,++Src;
        }
        S.free(); S=M; S.V=Sm.V; Sm.V=0;
                       S.N=Sm.N; Sm.N=0;
        if( S.Typ==3 ) S.Typ=1, S.Fmt=2;
      } m_View();
  } } color( c );
}
