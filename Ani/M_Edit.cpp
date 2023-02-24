//
//      Mario_Edit
//
#include <StdIO.h>
#include "Mario.h"

void m_Edit()
{
 static char bl[]="  ",yes[]="\tда ",no[]="\tнет";
 long  TD;
 Event _J=Tm;
 Real  _T=_J.T,_d=dT,_t=Tn;
 int k,ans=0,c=color( CYAN ),RD=Nm>0,m,d,y;
 Mlist Menu[]=
  { { 1, 0,"\t Параметры рядов и изображения" }
  , { 2, 5,"\tНачало %5d",&y },{ 0,2,"-%02d",&m },{ 0,2,"-%02d",&d }
  , { 0,11,", %3°",&_T }
  , { 1, 5,"\tДлина  %5ld",&TD },{ 0,11,"\t дней   %3°",&_t } //°
  , { 1,10,"\tШаг переинтерполяции %3°",&_d }                 //°
  , { 1, 2,"\tСтепень интерполяции %2u",&Pw }
  , { 2, 0,"\tРазметка шкал времени и функции" }                            //
  , { 1, 0,"\t  удаление разметки? " },{ 0,1 },{ 0,0,bl },{ 0,1 }     //10
  , { 1, 0,"\t        линии сетки? " },{ 0,1 },{ 0,0,bl },{ 0,1 }     //14
  , { 1, 0,"\tотносительная шкала? " },{ 0,1 },{ 0,0,bl },{ 0,1 }     //18
  , { 1, 0,"\t      разметка осей? " },{ 0,1 },{ 0,0,bl },{ 0,1 },{0} //22
  , { 1, 0,"\t            подписи? " },{ 0,1 },{ 0,0,bl },{ 0,1 }     //26
  };                                                                //
 Display T( Mlist( Menu ),1,RD?-1:0 );
  Thelp( RD?"F4 изменить  F5 сдвинуть":"" ); _J.UnPack( m,d,y ); TD=long(Tn/24);
                                             _t-=Real( TD )*24;
  do
  { Menu[11].Msg=Vv&0x08?yes:no;  Menu[13].Msg=Vv&0x80?yes:no;
    Menu[15].Msg=Vv&0x04?yes:no;  Menu[17].Msg=Vv&0x40?yes:no;
    Menu[19].Msg=Vv&0x02?yes:no;  Menu[21].Msg=Vv&0x20?yes:no;
    Menu[23].Msg=Vv&0x01?yes:no;  Menu[25].Msg=Vv&0x10?yes:no;
    Menu[28].Msg=Vv&0x100?no:yes; Menu[30].Msg=Vv&0x200?yes:no;
    switch( ans=T.Answer() )
    { case 11: Vv^=0x08;  break;  case 13: Vv^=0x80;  break;
      case 15: Vv^=0x04;  break;  case 17: Vv^=0x40;  break;
      case 19: Vv^=0x02;  break;  case 21: Vv^=0x20;  break;
      case 23: Vv^=0x01;  break;  case 25: Vv^=0x10;  break;
      case 28: Vv^=0x100; break;  case 30: Vv^=0x200; break;
    }
  } while( ans!=_Esc && ans!=0xF4 && ans!=0xF5 ); T.Back();
  Tn=_t+Real( TD )*24.0;
  Tm.Pack( m,d,y );
  Tm.T=_T;
  dT=_d;
  if( RD )
  { switch( ans )
    { case 0xF5:
       for( k=0; k<Nm; k++ )if( !Nb || Ms[k].flag&1 )Ms[k].JT += Tm-_J; break;
      case 0xF4: m_View(); Thelp( "?Все верно" );
          k=Tv_getc();
      if( k==_Enter || (k|0x20)=='y' )
      { for( k=0; k<Nm; k++ )
        { Mario &M=Ms[k];
          if( !Nb || M.flag&1 )
          { if( M.JT<Tm+Tn && M.JT+M.dt*M.N>Tm ){ double t; long l,m,n;
              if( Tm<M.JT )m=long( ( M.JT-Tm )/dT+0.5 ); else m=0;
                           n=long( Tn/dT+0.5 );
              if( (t=(Tm+Tn)-(M.JT+M.dt*M.N))>0 )n -= long( t/dT+0.5 );
              if( n-m>0 )
              { Mario Vs;
                Vs.allocate( unsigned( n-m ) );
                for( l=m; l<n; l++ )Vs[int(l-m)]=M( Tm+dT*l );
                M.free();
                M.V=Vs.V; Vs.V=0;
                M.N=Vs.N; Vs.N=0;
                M.dt=dT;
                M.JT=Tm+dT*m; if( M.flag&1 ){ M.flag&=~1; Nb--; }
          } } }
        } if( Nb>0 )m_Delete();
    } }   m_View();
  }      color( c );
}
static int Kx=0,s=1,N;    // Сохраняемое положение маркера и размерность
static Real St=0,Ex=0,*Z; // Шаг движения, порог и адрес массива

static void Inform( int *P )
{
 static Mlist Menu[] =
 { { 0,0,"\t<PgU/PgD> перебор рядов"      }
 , { 1,0,"\t<Enter> изменить значение"    }
 , { 1,0,"\t<Space> поиск по порогу"      }
 , { 1,0,"\t<BkSp> интерполяция точки"    }
 , { 1,0,"\t<1..0> множитель шага"        }
 , { 1,5,"%5.3lf",&Ex },{ 0,0,"\t порог"  }
 , { 1,5,"%3u" /*P*/ },{ 0,0,"\t степень" }
 };                                 Menu[7].dat=P;
 int c=color( CYAN ); Tmenu( Mlist( Menu ) ); color( c );
}
static Real ch1()      // Замена 1 точки интерполяцией 1 порядка
{ if( !Kx )  return 2*Z[1]-Z[2];   Real *z=Z+Kx;
  if(Kx==N-1)return 2*z[-1]-z[-2];
             return ( z[1]+z[-1] )/2;
}
static Real ch3()      // Замена 1 точки интерполяцией 3 порядка
{
 static Real A[]={ -1,4,-6,4,-1 };
 Real *z,w=0;
 int k=Kx,n=N-3;
  if( k>n )k-=n-2; else if( k>2 )k=2;
  for( z=Z+Kx-k,n=0; n<5; n++,z++ )if( k!=n )w+=*z*A[n]; w/=-A[k];
 return w;
}
void m_Show(){ if( Nm<=0 )return;  //
 bool lw=false;
 int A,c=color( GREEN );           //
 viewporttype V,v; getviewsettings( &V ); v=V;
 Tv.Height( 8 ); Thelp( " F1 инф. <Enter> ред. <BkSp> сред." );
 Tv.Height( 0 );
 Point P;
 Event X = Tm+dT*Kx;
  do
  { Mario &M=Ms[Km]; Z=M.V; N=M.N;
    if( St==0 )St=M.dt;
    if( X<Tm  )X=Tm+Tn; else if( X>Tm+Tn )X=Tm; Kx=int( (X-M.JT)/M.dt+0.01 );
    if( Kx<0  ){ Kx=0,   X=M.JT;  }else
    if( Kx>=N ){ Kx=N-1; X=M.JT+M.dt*Kx; }
    P.x=(X-Tm)+Tm.T;
    P.y=M.Level+Z[Kx];
    color( MAGENTA ); Tv_cross( P,6 );
    if( !lw )
    { Twindow( -36,0,13,2,0 ); v=Tport; v.right=v.left-2; v.left=0; lw=true;
      Tv_store( v.left,v.top,v.right,v.bottom );
    } color( DARKGRAY );
    Tgo( 1,1 ); Tprintf( fabs(Z[Kx])<1.0e-3?"%13.6lG\n":"F=%11.7lf\n",Z[Kx] );
                Tprintf( "%5d \xF1%.0lf\n",Kx,St/M.dt );
    { const int LW=64;                  // Длина маленького окошка
     static field Vf={ 0,-10,0,0,0 };
     static Field VF={ -LW,-1,2*LW,1 };
     int j=Kx-LW; if( j<0  )j=0;
     int m=Kx+LW; if( m>=N )m=N-1;
      for( VF.Jy=VF.Ly=Z[j],A=j+1; A<=m; A++ )
      { if( VF.Ly<Z[A] )VF.Ly=Z[A]; else
        if( VF.Jy>Z[A] )VF.Jy=Z[A];
      }     VF.Ly-=VF.Jy;
      if( VF.Ly<1.e-6 ){ VF.Ly+=2.0e-6; VF.Jy-=1.0e-6; }
      Vf.Ly=-v.top;
      Vf.Lx=-Tv_port.right+v.right;   color( LIGHTGRAY );
      Tv_place( &Vf,&VF );            color( LIGHTRED );
//    TvClip();
      A=( v.right-v.left )/2; line( A,Tv_port.top+6,A,Tv_port.top+20 );
      for( color( GREEN ),moveto( Real(j-Kx),Z[j] ),A=j+1; A<=m; A++ )
                          lineto( Real(A-Kx),Z[A] );
//    TvUnClip();
      Tv_place( 0,&VF );
      Tv_place( &Vf );
    }
    switch( A=Tv_getc() )
    { case West : s=-1,X-=St; break; case North_West: X=Tm;         break;
      case East :  s=1,X+=St; break; case South_West: X=Tm+(Tn-dT); break;
      case North: s=-1,X-=St*6; break;
      case South:  s=1,X+=St*6; break;
      case _BkSp:
      case _Enter:
      { color( DARKGRAY );
        if( Kx>0   )line( P.x-M.dt,M.Level+Z[Kx-1],P.x,P.y );
        if( Kx<N-1 )line( P.x,P.y,P.x+M.dt,M.Level+Z[Kx+1] );
        color( GREEN );
        if( A==_BkSp  )Z[Kx]=Pw>1?ch3():ch1(); else // BackSpace
        if( A==_Enter )
        { char Msg[25]; sprintf( Msg,"%13.6lg",Z[Kx] );
          Tgo(1,1); if( Tgets( Msg,13,3 )==_Enter )sscanf( Msg,"%lg",Z+Kx );
        } P.y=M.Level+Z[Kx];
        color( M.Typ==3?CYAN:GREEN );
        if( Kx>0   )line( P.x-M.dt,M.Level+Z[Kx-1],P.x,P.y );
        if( Kx<N-1 )line( P.x,P.y,P.x+M.dt,M.Level+Z[Kx+1] );
      } break;                  //
      case ' ':                 // Поиск по порогу
      { Real w,W,Exc;          //
        if( Ex>0 )Exc=Ex; else{ Exc=Tv_step( M.Max-M.Min ); Ex=-Exc; }
        color( BLUE );
        while( !Tv_getw() )
        { w=Z[Kx]; Kx+=s;
          if( Kx<0  ){ s=1;  X=M.JT;        break; }
          if( Kx>=N ){ s=-1; X=M.JT+M.dt*N; break; } X=M.JT+Kx*M.dt;
          if( X<Tm    ){ s=1;  X=Tm;    break; }
          if( X>Tm+Tn ){ s=-1; X=Tm+Tn; break; }
              P.x=(X-Tm)+Tm.T;
              P.y=M.Level+( W=Z[Kx] ); Tv_cross( P,4 );
          if( fabs( W-w )>Exc )break;
      } } break;
      case 0xF1: break;                 //
     default:                           // Варианты со сменой ряда
      if( lw ){ Tv_restore(); Tback(); }lw=false;
      switch( A )
      { case North_East: m_Mark( 1 );  St=0; break;
        case South_East: m_Mark( -1 ); St=0; break;
        default:
         if( A>='0' && A<='9' ){ A-='0'; St=!A?dT:A<2?M.dt:St*A; } else
         if( A!=_Esc )A=0xF1;
    } }
    if( A==0xF1 )Inform( &Pw );
  } while( A!=_Esc ); color( c ); Tv_cross( P,0 );
}
