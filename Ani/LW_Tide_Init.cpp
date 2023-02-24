//
//      ��������� ��������� ��������� ������� ������������ �����
//      ����� ������, ��� ���������� ������ ��� ������������� ��������
//
//      �������� Tide.W:
//               0x8000 - 1: ��������� ��� ��������� �����
//               0x4000 - 1: ����� ��������� �� �������
//               0x0002 - 0: ��������� ����� ��� �� � �������
//               0x0001 - 1: ���������� ������� �� �����
//
#include <String.h>
#include "..\Ani\LongWave.h"
#include "..\Ani\Tide.h"
//
//      ������ ������ �������� ���� �������� �����
//
const char* DownCount( Real T, Real cT )
{  if( cT==0 )return "\t  ����������� �����";
   if( cT!=T )return "\t������ �� ���������";
        else  return "\t ������ ����� �����";
}
void TWave::Period_Arrow()              //! ������� ��������� ������� �����
{
 const Point Q[]={ {0,-0.40},{0.12,-0.55},{0.04,0.24},{0.12,0.20},{0,0.66} };
 Real  C=(Real)Map.Ly/Map.Lx,
       S=C/cos( Chart.Fe );
 Field F=(Field){ -1,-S,2,S*2 };
 int i,j,c=color( WaveAs&1 ? LIGHTMAGENTA:DARKGRAY ); setwritemode( XOR_PUT );
  Tv_place( 0,&F );
  for( j=0; j<2; j++ )
  for( i=0; i<5; i++ )
  { Point P=Q[i];
    if( j )P.x=-P.x;
    if( C<1.0 )P*=C; else P/=C; P=We.Goext( P );
    if( !i )moveto( P ); else lineto( P );
  } Tv_place( 0,&F ); color( c ); setwritemode( COPY_PUT );
}
static void New_Tides_Data()
{
 int y,x,k=0,j=0;
 Point G;
  if ( TideFileRead( "Ani.tim" )>0 )
  while( k<Number_Tides )             //
  { G=TideGeo( k ); Chart.DtI( G );   // �������������� ����������
    y=int( G.y+0.5 ),                 //
    x=int( G.x+0.5 );                 // ������������ ���� ������ ����
    if( y>=0 && y<Chart.Ny            // � ������� ��������� ������� ����������
     && x>=0 && x<Chart.Nx )          //
    {                    List_of_Tide[k].sW=0;
      if( Chart[y][x]<0 )List_of_Tide[k].sW|=0x0001;
      if( k!=j )List_of_Tide[j]=List_of_Tide[k]; j++;
    } else      List_of_Tide[k].free();          k++;
  } if( k!=j )List_of_Tide
     =(Tide*)Allocate( (Number_Tides=j)*sizeof(Tide),List_of_Tide );
}
char* List_of_Tide_Gage( int k )
{
 static char Msg[42]="+ ";
 Tide &T=List_of_Tide[k];
  Msg[0]=T.sW&0x8000 ?'-' : T.sW&0x4000 ? '#' : T.sW&1 ?'o' : '+';
  scpy( Msg+2,T.Name,20 );
  return Msg;
}
static Course Ans_for_Tide_Gage( int k )
{                               //
 Course ans;                    // ��������� �������� � ������
 Point  G;                      // <Insert> ���������� �����
 Tide &T=List_of_Tide[k];       // <Delete|BkSp> ������ ��  �������
 int c=color( LIGHTCYAN );      // <Space> --//-- ������������
  Ghelp                         //
  ( "<Space>Switch <Esc>Simple <Enter>Tidal Wave of \"%s\" %s",
     T.Name,T.sW&0x8000?"out of Map" :T.sW&1?"on Shore":"in Open Sea"
  );
  if( !( T.sW&0x8000 ) ){ G.y=T.Fi;
                          G.x=T.La; Chart.DtI( G ); Tv_cross( G );
                        } ans=Tv_getc();
  if( !( T.sW&0x8000 ) )
  { Tv_cross( G,0 );
    switch( ans )
    { case _BkSp:
      case _Del : T.sW|= 0x4000; ans=Center; break;
      case  ' ' : T.sW^= 0x4000; ans=Center; break;
      case _Ins : T.sW&=~0x4000; ans=Center; default:;
  } } else if( ans==' ' )ans=South; color( c );
  return ans;
}
int TWave::Period()
{ const
  char *Name[]={ "Wave ","  ������� �������� ����������    ",
                         "�� ��������� �������� ���������",
                         "�������, ��������� �����������",
                         " ����������� � ��������������",
                         "     ��������� �����",0 },
       *Text[]={ "������������ ","�������� ������ � ����",
                 "���������� ���� ��� ���������� �� ����","",
                 "F3  ","������ �� �������� ��������� ����",
                 "F10 ","������ ������� ����������� �������",0 },
       *Plus[]={ "<Enter>"," � ������ ��������� ������ ���",
                 "<���>"," - ���� ��������� � ������� �����",0, },
       Com[]="F2 ������ F3 ����� F4 ����� F5 ����� F6 �����... F10 ������";
 Real WC,cT=W0,Wtl=Tlaps; // ?? ��������� float � double
 Mlist Menu[]={ { 0,20 }
              , { 1,8,"\t������ �����%-3�", &Wt }                    // 1
              , { 1,4,"\t���������  %4.2lf",&Wa },{ 0,0,"\t ����"  } // 2,3
              , { 1,4,"\t�����������%4.0lf",&WC },{ 0,0,"\t ����"  } // 4,5
              , { 1,4,0,                    &Wk },{ 0,0 }            // 6,7
              , { 1,20,0,0 } },                                      // 8
      MTide[]={ { 0,21 },{ 1,21 } // ".................... " } ��������[21]
              , { 1,4,"\t����������� %4.0lf",&WC },{ 0,0,"\t ����" } // 2,3
              , { 1,4,"\t���.��������� %4.2lf",&Ta }                 // 4
              , { 1,7,"\t������� ������%-2�",&To }                   // 5
              };
 Display Tm( Tk<0?Menu:MTide,Tk<0?9:6,1,1 );
 int ans=0; color( GREEN );            //
       WC=Wc;                          // ����������� ��� �������� ����� [����]
       Wo=0;                           // ���������� ������ �����        [����]
       Ws=(Point){ 1.0,1.0 };          // ������� ����� ����� ������     [����]
  if( !Wa )W0=Wtl;                     //
  for( ;; )
  { We.Euler( Wo,(Wc=WC=Angle( WC ))-Chart.Cs,Ws );
    MTide[0].Msg=
    Menu[0].Msg=(char*)( WaveAs&4 ? "\t���� � ������ ����":"\t������������� ���" );
    Menu[6].Msg=(char*)( WaveAs&8 ? "\t���������� %4.2lf" :"\t���������� %4.0lf" );
    Menu[7].Msg=(char*)( WaveAs&8 ? "\t ����" : "\t ����" );
    Wk = max( Wk,Real( !(WaveAs&8) ) );          // ���������� ������ �����
    if( Wa || (Tk>=0 && Ta!=0.0) )WaveAs|=1;     //
                             else WaveAs&=~1;    // ������������ ���������
    if( ans==_Esc || ans>0xF1 )break;            //! ����� ������ �����
    if( ans==0xF1 )Help( Name,Text,Plus );       //!
        Period_Arrow();
    if( Number_Tides>0 )
    { if( Tk<0 )Thelp( "%d/%d ������, %s",Number_Tides,Tk,Com );
         else  Ghelp( "%d/%d %s, %s",Number_Tides,Tk,List_of_Tide[Tk].Name,W2D(Com) );
    } else    Thelp( "��� �������, %s",Com );
              color( CYAN );
    if( Tk<0 )Menu[8].Msg=DownCount( Wtl,W0 );
        else  MTide[1].Msg=List_of_Tide[Tk].Name;            //
    if( Tk<0 )Wt/=3600; else To/=3600; ans=Tm.Answer( ans ); //! ������ ������
    if( Tk<0 )Wt*=3600; else To*=3600;                       //      ����� !!
                                           //
    if( Tk<0 )                             // ������ ������ � �������� �������
    { if( ans==8 )                         //
      { if( W0==Wtl )W0=0; else if( W0==0 && cT!=0 )W0=cT; else W0=Wtl;
    } }
    switch( ans )                     //
    { case 0:  WaveAs^=0x4; break;    //  ��������� ������������� ���� � ������
      case 6:  Wk+=(WaveAs^=0x8)?-1:1; Tm.Back(); break; // ��������� ���������� Vt+rand( Vk )
      case _Tab: if( Tk>=0 ){ Tk=-1; ans=0xF3; break; }  // ������ ������ �����
      case 0xFA: Allocate( Number_Tides=0,List_of_Tide ); List_of_Tide=NULL;
                 if( ans!=_Tab ){ ans=Tk<0?0:0xF3; Tk=-1; break; }              // if( Tk<0 )ans=0; else{ Tk=-1; ans=0xF3; } break; }
      case 0xF3: if( !Number_Tides )New_Tides_Data();
                 if( !Number_Tides ){ Tk=-1; break; }
                 if( !( VideoM&1 ) ){ VideoM|=1; St*=Tstep/3600.0; }
      for( ;; )                                  //
      { static field Lw={ 23,1,1,18 };           // ������ ������������ �������
        static int K,P=0;                        //
        K=Tcase( Lw,22,List_of_Tide_Gage,Ans_for_Tide_Gage,Number_Tides,P=Tk );
        if( K==_Tab || K==_Enter || Tk>=0 )
        { if( Tk!=P )List_of_Tide[Tk=P].sW&=~0x0002; ans=0xF3; break;
        }
        if( K==_Enter || K==_Esc ){ ans=0xF3; break; }
      }
    } Period_Arrow();
  }   WC =Angle( WC-Chart.Cs );
      Wo.y=fabs( WC )>90?My*Dy:0.0;
      Wo.x=      WC<=0.0?Mx*Dx:0.0; We.Euler( Wo,WC,Ws ); return ans;
}
