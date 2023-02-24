//
//  Tv_case                                                          92.10.16
//
//  ��������� ������������ ������ ������ � ������ �� ����������� ��������
//
//    Ki - ����� �������� � ������ � �������� ����������� ����� �� ���������;
//  ������������ ������� ������ �� ������
//   <ret> ������� � ���������� ��������� � ������ �� ���������:
//         ���� � ���������� ���������� ������� ������,
//              �� ���������� ����� �� ����� ������,
//         ���� ���������� ���� � ����������,
//              �� ������������ ������ ������ ���������� �� ��������� ����.
//
#include "..\Graphics\Tv_graph.h"
//
//     ������� ������� ������
//
static void Mark( int ls, int y, int x, int da, int ic )
{ Tgo( x*ls+1,y );  color( ic?WHITE:Tv.BkColor );
  x=Tv.Tw*ls-4;     Tpoint.x++;
  y=Tv.Th+(da>0);   Tpoint.y+=y-2;
  moveto( Tpoint ); Tpoint.y-=y;
  lineto( Tpoint ); Tpoint.x+=x;
  lineto( Tpoint ); color( ic?(Tv.BkColor==BLACK?LIGHTGRAY:BLACK):Tv.BkColor );
                    Tpoint.y+=y;
  lineto( Tpoint ); Tpoint.x-=x;
  lineto( Tpoint );
}
Course Tcase(           //
 field& M,              // ���������� ������� �� ������ ��������
   int ls,              // ������������ ����� �������������� ������
 char*  Fl(int),        // ���������� ������� �� ������ � ������
 Course Hl(int),        // ���������� ��� �������� ���������� �������
                        // ���� �� ������ 0: ����������� ������� �������
                        //               -1: ����������� � � ����� ������
   int  N,              // ������ ����� ������
   int& Ki,             // ����� �������� �������� (-1 �� ��������)
   int  ret             //       1 ����� � �����������
){                      //       0 ������� ���� � �����
 char * S;              //      -1 ���� ��� �������������
static int _K=-11;      // ������������ ������ ��������� � ������
 int  C,L,Lp,i,k,l,y,x, // ������ ����� �������� ������
      Ls,a=0,K=0,       // ����� ����� ������ � ������� ������ �������
      Nb,Ne,            // ������ ������ � ��������� �� ������� �������
      Ly,Lx;            // ���������� ����� � ��������
 char s[12];            // �������� ������� ��� ����������� ������
 int  ans;              //
   C=Tv.Color;          //
  Ls=ls+2;   L=(Ly=M.Ly)*(Lx=M.Lx);
  l=L/2;     if( Ki>=0 )K=Ki; else K=_K; if( K>=N )K=N-1; if( K<0 )K=0;
  Nb=K-l;    if( Nb<0  )Nb=0;
  Ne=Nb+L-1; if( Ne>=N )
             { Ne=N-1; Nb=N-L; if( Nb<0 )Nb=0,L=N,Ly=(L+Lx-1)/Lx; }
 if( ret>=0 ){ color( LIGHTGRAY ); Twindow( M.Jx,M.Jy,M.Lx*Ls,Ly,M.wb );
             }
ReView: color( C );  k=Nb;
  for( x=0; x<Lx; x++ )
  for( y=0; y<Ly && k<=Ne; y++,k++ )
  { S=Fl( k ); Tgo( x*Ls+2,y+1 );
    for( ans=1,i=0; i<Ls-2; i++ )
    { if( ans )ans=S[i]; Tputc( ans?ans:' ' ); } color( C );
  }
  Lp=Lx>1?Ly*((Lx-1)/3+1):Ly-1;
  for( ;; )
  { Mark( Ls,y=(K-Nb)%Ly+1,x=(K-Nb)/Ly,a,1 ); Tgo( x+a+1,y );
    if(  a )ans=Tgetc(); else
    if( Hl )ans=Hl( K ); else
    do ans=Tv_getc(); while( ans>=_MouseMoved && ans<=_MouseReleased );
    Mark( Ls,y,x,a,0 ); l=0;       // ������� �������� ����
                        k=K;       // ������� �������� �������
    if( ans==0xFF )ans=South,k=-1; // ������� ������� �������� ������
    if( !ans )goto ReView;         // - �� �� � ����������� ������������
    if( ans<0xF1 && ans>=' ' )     //
    { int dir=0;                   // ������ �������� ������
//    ans=ucod( ans );             //
      scpy( s,S=Fl( K ),12 );      // <<ucpy>>
      if( ans>s[a] )dir=1;  else
      if( ans<s[a] )dir=-1; else ++a;
      if( dir && a<12 )
      while( (dir>0 && K<N-1) || (dir<0 && K>0) )
      { S=Fl( K+dir );
        for( i=0; i<a; i++ )if( S[i]!=s[i] )goto Leave; // <<ucod>>
        if( (i=(ans-S[a]))*dir<0 )break; K+=dir;        // <<ucod>>
        if( !i ){ ++a; break; }
      }
    Leave: if( k!=K )
      {    if( K<=Nb ){ l=-1; if( Nb>0   )Nb=K+Ly/2; }
           if( K>=Ne ){ l=1;  if( Ne<N-1 )Ne=K-Ly/2; }
    } } else
    { switch( ans )
      { case North     :        if( K>0  )--K;              break;
        case North_West: K=0;   if( Nb>0 )Nb=1;     else    break;
        case North_East: --l;
        case       West: K-=K-Ly<Nb?Lp:Ly; if( K<0 )K=0;    break;
        case South     :        if( K<N-1 )++K;             break;
        case South_West: K=N-1; if( Ne<N-1 )Ne=N-2; else    break;
        case South_East: ++l;
        case       East: K+=K+Ly>Ne?Lp:Ly; if( K>=N )K=N-1; break;
       default:
        if( !ret )Tback(); Ki=K; color( C ); return Course( ans );
      } a=0;
    }
    if( k!=K )
    { if( l<0 )
      { if( Nb>0 ){ Nb-=Lp; if( Nb<0 )Nb=0; Ne=Nb+L-1; goto ReView; } }
      else
      if( l>0 )
      { if( Ne<N-1 ){ Ne+=Lp; if(Ne>=N)Ne=N-1; Nb=Ne-L+1; goto ReView; } }
      else
      if( K<Nb || K>Ne )
      { if( K<Nb ){ Nb-=Lp; if( Nb<0  )Nb=0;   Ne=Nb+L-1; k=-1; }else
        if( K>Ne ){ Ne+=Lp; if( Ne>=N )Ne=N-1; Nb=Ne-L+1; k=-1; }
      } if( k<0  )goto ReView;
  } }
}
Course Tcase(           //
 field& M,              // ���������� ������� �� ������ ��������
   int  ls,             // ������������ ����� �������������� ������
 char*  Fl( int ),      // ���������� ������� �� ������ � ������
   int  N,              // ������ ����� ������
   int& Ki,             // ����� �������� �������� (-1 �� ��������)
   int  ret             // ����� ���������� �����������
){                      //
  return Tcase( M,ls,Fl,0,N,Ki,ret );
}
