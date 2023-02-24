//
//      Tv_draw : Встроенная процедура дооформления рисунка
//
//#include "..\Matrix\Basis.h"
//#include "..\Graphics\Tv_graph.h"
//
static const
  char Hp[]="F1 Help  F2 Text  F3 Object <Enter>Fix <Ins>New <Del>Old <Tab>%s";
static point p,*V;    // Местоположение, адрес интерполируемой кривой
static Point          //
       Ry={ 0,-100 },Rx={ 100,0 }, // Базисные оси
       As={ 0.6,.6 },Qs={ .4,.4 }; // Эллипс и прямоугольник
static int            // 0 - Базисные оси;  1 - Эллипс
       Pic=0,         // 2 - Прямоугольник; 3 - Кривая линия
       Nv=0,          // Длина массива в памяти       ¦ Enter  - Fix
       Mv=0,          // Контрольное количество точек ¦ Insert - Add
       Kv=0,          // Номер подвижной точки        ¦ Delete - Sub
       Pv=0,          // Степень интерполяции         ¦ '+','-'  Change
       Cl=LIGHTBLUE,  // Цвет                         ¦ '`'Ў'0'  Power
       Tl=SOLID_LINE, // Сохраняемый тип линии        ¦ ' '      Shift
       Sh=0;          // Признаки SHIFT               L
static Basis Bas;     //
                      //
static point value( point *V, int N, float Ar, int Pw )
{
// Интерполяция по Лагранжу на равномерном интервале
//                 V  - вектор значений функции
//                 N  - текушая длина вектора V
//                 Ar - интерполяционный аргумент
//                 Pw - степень интерполяции
 float yo=0,xo=0,p;           //
 int   Nb,Nf,j=(int)Ar,k=N-1; // Поиск ближайшего левого индекса
  if( j>k )j=k; else          //
  if( j<0 )j=0;
  if( Pw>k )Pw=k;
  if( Pw<=0 )return V[j];
  if( (Nf=++j+Pw/2)>k )Nf=k,Nb=k-Pw; else
  if(  ( Nb=Nf-Pw )<0 )Nb=0,Nf=Pw;
  for( j=Nb;j<=Nf;j++ )
  { for( p=1,k=Nb; k<=Nf; k++ )if( j!=k )p*=(Ar-k)/(j-k);
    yo+=V[j].y*p;
    xo+=V[j].x*p;
  } point q; q.y=int(yo+.5); q.x=int(xo+.5); return q;
}                       //
static point R2p( const float& X, const float& Y ){
 Point Q; Q.x=X;
 point q; Q.y=Y; Q=Bas.Goext( Q ); q.y=(int)Q.y;
                                   q.x=(int)Q.x; return q;
}
static void Axis()
{ point r = R2p( 0,0 ),u;
  line( r,u=R2p( 0,0.8 ) ); needle( u,R2p( 0,1.1 ) );
  line( r,u=R2p( 0.8,0 ) ); needle( u,R2p( 1.1,0 ) );
}
static void Ellipse(){ const float dt = ( M_PI_2/12 );
 float angle=0;
  for( int k=0; k<=48; k++,angle+=dt )
  { point q=R2p( As.x*cos( angle ),As.y*sin( angle ) );
    if( k )lineto( q );
      else moveto( q );
} }
static void RectAngle()
{ point q; moveto( q=R2p(  Qs.x, Qs.y ) );
           lineto(   R2p( -Qs.x, Qs.y ) );
           lineto(   R2p( -Qs.x,-Qs.y ) );
           lineto(   R2p(  Qs.x,-Qs.y ) );
           lineto( q );
}                                 //
static int Not_First_Curve=FALSE; // Используется без выхода на рекурсию
                                  //
static void Curve( int Sg )
{ if( !Mv )line( Tpoint,p ); else
  { float w=1e0/Sg,W=w;
    if( Not_First_Curve )p=V[Kv],Not_First_Curve=FALSE; else V[Kv]=p;
                Sg*=Mv-1;         moveto( V[0] );
    for( int j=0; j<Sg; W+=w,j++ )lineto( value( V,Mv,W,Pv ) );
} }
static void Building_Curve( int ans ) //       ¦ Enter  - Fix
{      //   Nv  Длина массива в памяти         ¦ Insert - Add
       //   My  Контрольное количество точек   ¦ Delete - Sub
       //   Ky  Номер подвижной точки          ¦ '+','-'  Change
       //   Pv  Степень интерполяции           ¦ '0'-'9'  Power
 point q; float w; int k=0;
  switch( ans )
  { case _Ins:
    if( Nv<=Mv )V=(point*)Allocate( sizeof(point)*(Nv+=12),V );
    if( !Mv )
    { V[1]=(V[0]=Tpoint)+(V[2]=p); V[1].x/=2;
                   Mv=3,Pv=2,Kv=1; V[1].y/=2;
    } else
    { if( Kv==Mv-1 )w=0.1+(float)Kv,Kv++;
      else if( Kv  )w=0.5+(float)Kv,Kv++; else w=-.1;
      q=value( V,Mv,w,Pv ); for( k=Mv; k>Kv; k-- )V[k]=V[k-1]; V[Kv]=q;
      Pv++;
      Mv++;
    } k=1; break;               // k - Признак корректировки
    case _BkSp: p=Tpoint;
    case _Del :if( Mv<=3 )Mv=0;
     else{ --Mv; for( k=Kv; k<Mv; k++ )V[k]=V[k+1]; if( Kv==Mv )--Kv; }
             k=1; break;
    case ' ':if( Mv>0 ){ q=p-V[Kv]; for( k=0; k<Mv; k++ )V[k]+=q; }
                 k=0;  Not_First_Curve=TRUE; break;
    case '=':
    case '+':if( Mv>2 ){ if( Kv<Mv-1 )Kv++; else Kv=0; k=1; }break;
    case '-':
    case '_':if( Mv>2 ){ if( Kv>0 )Kv--; else Kv=Mv-1; k=1; }break;
    case '`': Pv=Mv-1;
   default  :if( ans>='0'&&ans<='9' )Pv=ans-'0';
  }
  if( k ){ Not_First_Curve=TRUE; Tpoint=p; }
}
static const float Rels=1.02;

static void Deform( Point& R, int C )
{ if( C&North )R.y*=Rels; if( C&East )R.x*=Rels;
  if( C&South )R.y/=Rels; if( C&West )R.x/=Rels;
}
static void Draw( int Wmod=1, int ans=0 ) // Wmod=0 - Закрепление
{                                       //      1 - Текущий объект
 point w=Tpoint;                        //      2 - С обработкой ANS
  setwritemode( Wmod?XOR_PUT:COPY_PUT );
  switch( Pic )
  { case 0: Axis();      break;
    case 1: Ellipse();   break;
    case 2: RectAngle(); break;
    case 3: Curve( Wmod?6:12 );
  } Tpoint=w; setwritemode( COPY_PUT ); // Закрепление рисунка
  if( !Wmod )
  { if( Pic==3 && (Sh&SHIFT)==0 )Mv=0; Tpoint=p;
  } else
  if( Wmod==2 )                         // Обработка команды
  { if( Pic==3 )Building_Curve( ans ); else
    if( ans<_Enter && (Sh&SHIFT)==0 )
    switch( Pic )
    { case 0:{  float S1=0.0174524; const float C1=0.9998477;
        switch( ans )
        { case North: Ry*=Rels; break; case East: Rx*=Rels; break;
          case South: Ry/=Rels; break; case West: Rx/=Rels; break;
         default:
          if( ans&East  )S1=-S1;
          if( ans&South )Rx.x=Rx.x*C1+Rx.y*S1,Rx.y=Rx.y*C1-Rx.x*S1;
                         Ry.x=Ry.x*C1+Ry.y*S1,Ry.y=Ry.y*C1-Ry.x*S1;
      } }                       break;
      case 1: Deform( As,ans ); break;
      case 2: Deform( Qs,ans ); break;
  } }
}
static void Init(){ Point Re;
  Re.x=p.x;
  Re.y=p.y; Bas.Axis( Re,Ry,Rx );
  setlinestyle( Tl&3,0,Tl&4?THICK_WIDTH:NORM_WIDTH ); Draw();
}
static byte *Svm;
static void Mpix( int k, int x, int y, int j ){ byte c=Cl;
  do
  { if( !k )Svm[0]=getpixel(x+j,y); else c=Svm[0]; putpixel( x+j,y,c ); ++Svm;
  } while( (j=-j)<0 );
}
static int Marker()
{ byte Save[60];             // 60 = 32+16+12
 point q=Tpoint;
 int A=0,k,j;
  for( k=0;;k++ ){ Svm=Save;
    for( j=1; j<=16;j++ )Mpix( k,p.x,p.y,  j );
    for( j=-3;j<=8; j++ )Mpix( k,p.x,p.y+j,0 ); if( k )break;
    for( j=1; j<=8; j++ )Mpix( 0,q.x,q.y-j,j ); A=Tv_getc(); Svm=Save+44;
    for( j=1; j<=8; j++ )Mpix( 1,q.x,q.y-j,j ); Sh=Tv_getk();
  } return A;
}
void Tv_draw()                 //
{ static int Level=-1,         // Контролируемый уровень рекурсии
             Mode=0;           // Текущий режим изображения
  if( Level>0 )return;         //
 int ans,c=color( Cl ),bc,k,Hl=0;
  if( Level<0 ){ Cl=(bc=Tv.BkColor)==BLACK ? YELLOW:BLACK; p.x=p.y=24; }
 point K=Tpoint;       Tpoint=p;
 linesettingstype lin; getlinesettings( &lin ); // Тип линии в главной программе
//pattern( WHITE );            //
  Level=1;
Repeat:
  if( Mode )Init(); ans=Marker();
  if( Mode )Draw( 2,ans );  if( Hl ){ Ghelp(); Hl=0; };
  if( ans<_Enter )              // Режим позиционирования и подписей
  { k=!Mode || Pic==3;          //
    if( k || Sh&SHIFT )
    { k=k && Sh&SHIFT?(Mode?12:3):1; if( Mode==0 )k *= Tv.Th+2;
      if( ans&North )p.y-=k;
      if( ans&South )p.y+=k; if( Mode==0 )k=(k*Tv.Tw)/(Tv.Th+2);
      if( ans&West  )p.x-=k;
      if( ans&East  )p.x+=k;
    }
  } else switch( ans )
  { case 0xF3:  if( Mode )++Pic&=3; else Mode=1;
                if( Mode && Pic==3 )Building_Curve( ' ' ); break;
    case 0xF2:  Mode=0; break;
    case 0xF1:{ point q=Tpoint;
         Ghelp( Hp,Mode?(Pic==3?"Line <+|->Change":"Style"):"Color" );
                Hl=1; Tpoint=q;
              } break;
    case _Ins: Tpoint=p; break;
    case _Del: p=Tpoint; break;
    case _Tab:
      if( !Mode || Sh&SHIFT )color( ++Cl&=15 ); else ++Tl&=7; break;
    case _Esc: color( c ); // pattern( bc );
         setlinestyle( lin.linestyle,lin.upattern,lin.thickness );
         p=Tpoint;
         Tpoint=K;
         Level=0;
         return;
   default:
    if( !Mode )
    switch( ans )
    { case _BkSp :Tpoint.x-=Tv.Tw; Tputc(' ');
                  Tpoint.x-=Tv.Tw;   break;
      case _Enter:Tpoint.x=p.x;
                  Tpoint.y+=Tv.Th+2; break;
     default: if( ans>=' ' && ans<0xF1 )Tputc( ans );
    }
    else if( ans==_Enter )Draw( 0 );
  } goto Repeat;
}
