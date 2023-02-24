/*
  Hbound.c                                                           /90.11.03/

  ��������� ���������� ������� �� ��������� �������� � ������� ��������� ������
  ������������ �� ������� ������ � ����������� ��������� ������������ (�=vgH)
       Tstep,T - ��� � ������ ������� [���]
*/
#include <Time.h>
#include "..\Ani\Tide.h"
#include "..\Ani\LongWave.h"
#include "..\Matrix\Function.h"

static Function Freq;                              // ��������� �������
static Real RandomWaves( Real T, Real Wt,Real Wk ) // ������ � ������������
{ static Real Tmin=0,Tmax=0;    // ���-������ �������� � ����������� ������
  const int WInt=24;            // ��������������� ����� ���������������� �����
    Wt /= 2.0;                  // ���������� -> � ������ ������
    if( T<Tmin )Tmin=T;         //  ����� ���-�� ��������� �������� �� ��������
    if( T>=Tmax )               //   ������ � ���������� �������� �������
    { int k=0,l=Freq.length;    //
       if( Tmin>Wt )            // ��� �������� ������ �������� ��������
       for( k=0; k<Freq.length; k++ )if( Tmin<=Freq.X( k ) )break;
       if( k>3 )                        // ���� ����� ���� ������� ����� ������
       for( l=0,k-=(++k&1)+2; k<Freq.length; k++,l++ )
         { Freq.Y( l )=1-((l&1)<<1);    // ����� ��������� +1:-1
           Freq.X( l )=Freq.X( k );     // ������ l - ������ ��� �������
         } k=l;
       if( l+WInt>Freq.length )Freq.set( l+WInt ); // ����� ������ ���� ������
       if( !l )
         { for( ; k<4; k++ )            // ��� ������ ����� ��� �������� ����
           { Freq.X(k)=k*Wt/2; Freq.Y(k)=((Real[]){0,1,0,-1})[k];
           } srand( time( 0 ) );        // ��������� ����� ����� ��������
         }                              //
       for( ; k<Freq.length; k++ )
         { Freq.X( k ) = Freq.X( k-1 ) + Wt + Wt*Wk*Real( rand() )/RAND_MAX;
           Freq.Y( k ) = 1-((k&1)<<1); // 1.0 - 2.0*( k%2 );
         }
       Freq.SpLine();
//     Freq.First();
       Tmin=Tmax=Freq.X( Freq.length-WInt/3 );
    }  return Freq.derivative( T );
}
#define Local( W ) l.y=y*Dy; l.x=x*Dx; l=W.Goint( l );

static Real cFe, EsY,EsX,   // ����������� ������ � ���������������� ����
                 EpY,EpX;   // ������� ����� �� ������� �� �������

Real TWave::DiffHeigh( Real C, const int y, const int x )
{ int  Yn,
     Xw, Xe, jy,jx,
       Ys;
 Real Deh,Dhy,Dhx,s; Point l;
//
//    ���������� ������������� ������� �� ��������� ������� �� ������� �������
//
  Yn=Xw=Xe=Ys=0;
  Deh=Dhy=Dhx=0.0;
//
//    N-S:E-W
//
  if( y>0    ){ if( Dp[y-1][x]>DZ )Ys=-1; }  // ����������� �������
  if( y<My-1 ){ if( Dp[y+1][x]>DZ )Yn=1;  }  // ������������� ���������
  if( x>0    ){ if( Dp[y][x-1]>DZ )Xw=-1; }  // ����������������� �������
  if( x<Mx-1 ){ if( Dp[y][x+1]>DZ )Xe=1;  }
//
//    Dh/Dr
//
  if( ( jy=Yn+Ys )==0 ) if( !y || (y==My-1) )return 0.0; // ������� �����
  if( ( jx=Xe+Xw )==0 ) if( !x || (x==Mx-1) )return 0.0; // ����������� ??
                        //
                        // ����������������� ������� ���������
                        //
  if( Yn>Ys )Dhy = ( Hg[Yn+y][x]-Hg[Ys+y][x] )/Dy/( Yn-Ys );
  if( Xe>Xw )Dhx = ( Hg[y][Xe+x]-Hg[y][Xw+x] )/Dx/( Xe-Xw );
                        //
  if( WaveAs&0x004 )    // ���� ���������������� ����������
  {                     //
    if( Yn>Ys )Dhy -= ( Hf[Yn+y][x]-Hf[Ys+y][x] )/Dy/( Yn-Ys );
    if( Xe>Xw )Dhx -= ( Hf[y][Xe+x]-Hf[y][Xw+x] )/Dx/( Xe-Xw );
  }                     //
  if( WaveAs&0x800 )    // ���� ������� ����������� ����������
  {                     //
    if( Yn>Ys )Dhy += ( Fa[Yn+y][x]-Fa[Ys+y][x] )/Dy/( Yn-Ys );
    if( Xe>Xw )Dhx += ( Fa[y][Xe+x]-Fa[y][Xw+x] )/Dx/( Xe-Xw );
  }                                     //
  if( WaveAs&0x300 )C *= cFe/_Fc[y][x]; // ���� �����������
  if( WaveAs&0x002 )                    //
  {                                     // ��������� ����� ������������
   Local( Ve );                         //   ����� �������������� �����
        l.y-=(Tlaps-V0)*C/Vs.y;         //
    if( l.y<1.0 && l.y>1.0-2.0*Vk )     //
    { s = Va * C/Vs.y*Ph * sin( l.y*Ph )*( cos( l.x*Ph ) );
          Dhy+=EsY*s/C;
          Dhx+=EsX*s/C; Deh += Tstep*s;
  } }                                   //
  if( WaveAs&0x001 )                    // �������������� �������� �����,
  {                                     //   ������������ �� ��������� ��������
   Local( We );                         //   � ������� ������� �� �������
    if( Tk<0 )                          //   � ����������� � ������������
    { if( (l.y=l.y/C-(Tlaps-W0))<0 )    // ����� �������
      { if( WaveAs&0x8 )                //   ��� ���������� ������� ����� [���]
        { s = Wa*RandomWaves( -l.y,Wt,Wk ); Dhy+=EpY*s/C;
                                            Dhx+=EpX*s/C; Deh += Tstep*s;
        } else                          // ��� ����������� �� ������� ���������
        if( (l.y/=Wt) > -Wk )           // � ���������� �����-�������
        { s = Pd*Wa/Wt * cos( Pd*l.y ); Dhy+=EpY*s/C;
                                        Dhx+=EpX*s/C; Deh += Tstep*s;
      } }
    } else                              //
    { l.y/=C;                           // ���� ������� ��������� ����
      if( l.y-Tlaps<=0.0 )              // ������������ �� ��������� �������
      {                                 //
       Tide &TT=List_of_Tide[Tk];
       Real Td[2];
        TT.TideSeries( Td,2,Tstep/3600.0,Instant.D,Instant.T-l.y/3600.0-Tstep/7200.0 );
        s=( Td[1]-Td[0] )/100.0; Dhy+=EpY*s/Tstep/C;
                                 Dhx+=EpX*s/Tstep/C; Deh+=s;
  } } }
//
//      ������ �������� � ����� ���������
//
  s=( Dhy*jy + Dhx*jx );
  if( !jy || !jx ){ if( s<0 )C=-C; s=hypot( Dhy,Dhx ); }
 return Deh + C*Tstep*s;
}
void TWave::HBound()
{
//      ����� �� ��������� ��� �������������� ��������� �������
//
 const bool Globus=Active&as_Globus?true:false;  // ������ ��������� ����������
 int  by=0,bx=0,y=0,x=1;
 Real Bd,Dd,Bh,B1=Hg[0][1],Deh=0.0;
  MarioTide();
  EsY=cos( Bh=(Vc-Chart.Cs)/Radian ); EsX=sin( Bh ); cFe=cos( Chart.Fe );
  EpY=cos( Bh=(Wc-Chart.Cs)/Radian ); EpX=sin( Bh ); Bh=Bd=0.0; //
  for( ;; )                                                     //  x<---x
  {     Dd=Dp[y][x];                                            //  |    |
    if( Dd>DZ )Deh=DiffHeigh( sqrt( Dd*9.8106 ),y,x );          //  V    |
    if( Bd>DZ )Hg[by][bx]+=Bh; by=y; Bd=Dd;                     //  o--->x
                               bx=x; Bh=Deh;                    //
    if( !x ){ if( Globus && y>0 )y=0; else y--;
              if( !y )Dd=Hg[0][1],Hg[0][1]=B1,B1=Dd; else
              if( y<0 ){ Hg[0][1]=B1; break; }   } else
    if( y==My-1 )--x;                              else
    if( x==Mx-1 ){ if( Globus )y=My-1; else y++; } else
    if( !y )++x;
  } if( Bd>DZ )Hg[by][bx]+=Bh;
}
//
//      ��������� ��������� ���������� ���������� � ������������ ������
//
#define AdC   if( unsigned( Y )<unsigned( My ) \
               && unsigned( X )<unsigned( Mx ) )if( Dp[Y][X]>DZ )
#define AdM       AdC { Hm+=Hg[Y][X]; ++n; }
#define AdS       AdC Hg[Y][X] = Hw;
#define AdH( mH ) AdC Hg[Y][X] = Hw + mH*Hm;
//
//    �������� Tide.W - 0x8000 - 1: ��������� ��� ��������� �����
//                      0x4000 - 1: ����� ��������� �� �������
//                      0x0002 - 0: ��������� ����� ��� �� � �������
//                      0x0001 - 1: ���������� ������� �� �����
//             Tide.Cs - ������������ ��� ����������� ������ ����
//
void TWave::MarioTide()
{ if( Tk<0 )
  for( int k=0; k<Number_Tides; k++ )if( (List_of_Tide[k].sW&0xC000)==0 )
  { int Y,X;
   Tide &Td=List_of_Tide[k];
   Point P={ Td.La,Td.Fi };
   Real  Hm=0.0,                // ������� �������
         Hw=Td.TidePoint( Instant.D,Instant.T+Tstep/3600.0 )/100.0;
    Chart.D2I( P,Y,X );               //
    if( ( Td.sW&2 )==0 )        // �������� �� ������ �������� ������
    if( Td.TidePoint( Instant.D,Instant.T )*Hw>0 )continue;
                                //
    if( Td.sW&1 )               // ������ �������� ������ ��� ����� �� ������
    {         int n=0;          //
      Y++;        AdM           // North
            X--;  AdM           // North West
      Y--;        AdM           //       West           2  1  8
      Y--;        AdM           // South West            \   /
            X++;  AdM           // South                3 <+> 7
            X++;  AdM           // South East            /   \.
      Y++;        AdM           //       East           4  5  6
      Y++;        AdM           // North East
      Y++,  X-=2; AdM           // North West t'North
      Y--,  X--;  AdM           // North West t'West     1 � 8
      Y-=2;       AdM           // South West t'West    2 㦬 7
      Y--,  X++;  AdM           // South West t'South   --���--
            X+=2; AdM           // South East t'South   3 LT- 6
      Y++,  X++;  AdM           // South East t'East     4 � 5
      Y+=2;       AdM           // North East t'East
      Y++,  X--;  AdM           // North East t'North

      Y-=2; X--;  if( n )Hm/=n; else continue;
    } else        //
    Hm=Hg[Y][X];  // ��� ����� ��� ����
                  //
    if( Td.sW&2 )Hm=( Hm-Td.Cs )*0.8; else Hm=0;
        Td.sW|=2;
        Td.Cs = Hw + Hm;
               AdS       // Center
    Y++;       AdS       // North
         X--;  AdH( .1 ) // North West
    Y--;       AdS       //       West           2  1  8
    Y--;       AdH( .1 ) // South West            \   /
         X++;  AdS       // South                3 <+> 7
         X++;  AdH( .1 ) // South East            /   \.
    Y++;       AdS       //       East           4  5  6
    Y++;       AdH( .1 ) // North East
    Y++, X-=2; AdH( .6 ) // North West t'North
    Y--, X--;  AdH( .6 ) // North West t'West     1 � 8
    Y-=2;      AdH( .6 ) // South West t'West    2 㦬 7
    Y--, X++;  AdH( .6 ) // South West t'South   --���--
         X+=2; AdH( .6 ) // South East t'South   3 LT- 6
    Y++, X++;  AdH( .6 ) // South East t'East     4 � 5
    Y+=2;      AdH( .6 ) // North East t'East
    Y++, X--;  AdH( .6 ) // North East t'North
  }                      //
}
