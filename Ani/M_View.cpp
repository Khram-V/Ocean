//
//      Mario.View
//      Программа рисования нескольких графиков одновременно
//
#include "Mario.h"

Real static Extrem( Mario& M, Real& Mn, Event& tm, Real& tn )
{
 const Real EPS=1e-12;
 Event T=M.JT;
 Real W,*S,w,Max,Min;            S=M.V; W=Max=Min=0.0;
  if( T<tm+tn && tm<T+M.dt*M.N ){        W=Max=Min=S[0];
    for( int k=1; k<M.N; k++ )
    { ++S; W+=w=S[0]; if( Max<w )Max=w; else if( Min>w )Min=w;
    } W/=M.N;
    if( Max-Min<EPS )Max=Min+EPS;
  } Mn=Min;
/*if( M.Typ!=3 )*/{ M.Mid=W;
                    M.Min=Min;
                  } M.Max=Max; return Max-Min;
}
Real static Extrem( Mario& W, Real& Mn, double dY )
{
 Real Min= ceil( Mn/dY    )*dY; if(    Mn<Min )Min-=dY;
 Real Max=floor( W.Max/dY )*dY; if( W.Max>Max )Max+=dY;
  dY=(Max-Min)-(W.Max-Mn );
/*if( W.Typ!=3 )*/ W.Min=Min; Mn=Min;
                   W.Max=Max; return dY;
}
static int Y0,M0,D0; // Дата и размер векторного шрифта
static char Id[21];
void static Jprint( point &p, int Y, int M, int D )
{ int m,d,y,k=0;
  julday( julday( M>0 || D>0?M0+M:1,D>0?D0+D:1,Y0+Y ),m,d,y );
  if( D )
    { M|=M0!=m;
      M|=(Y|=Y0!=y);
      if( d>1 ){ if( D<5 || d<30 )k+=sprintf( Id,"%d",d ); }else M=1;
    }
  if( M )
    { Y|=Y0!=y;
      if( m>1 ){ scpy( Id+k,Months[m-1],-3 ); k+=3; } else Y=1;
    }
  color( CYAN );
  if( Y )k+=sprintf( Id+k,"%d",y );
  if( k )Tv.Text( South_East,p,Id ); Y0=y; M0=m; D0=d;
}
static char* _g( double f, double d=1.0 )
{ sprintf( Id,"%0.3lg",fabs( f/d )>1e-3?f:0.0 ); return Id;
}
void    Segment_Line( Real,int );
void static Horiz_Line( Real x1, Real x2, Real y )
{ color( BLUE );
  setlinestyle( USERBIT_LINE,0x1111,NORM_WIDTH ),line(x1,y,x2,y);
  setlinestyle( SOLID_LINE,0,NORM_WIDTH ); color( CYAN );
}
// +--------------------------------------------------------------------------+
// ¦  Программа заполняет таблицы визуализации и прорисовывает числовые ряды  ¦
// +--------------------------------------------------------------------------+
//
void m_View(){ clear(); if( Nm<=0 )return;
 Point P;
 point q,p;
 Real *Min; Min=(Real*)Allocate( Nm*sizeof( Real ) );
 double dY,w,t;
 int i,k,c,cl=color( CYAN ); Tm.Check();
  {
   Field F;   F.Jx=Tm.T; F.Lx=Tn; F.Jy=F.Ly=0.0;
    for( i=0; i<Nm; i++ )F.Ly+=Extrem( Ms[i],Min[i],Tm,Tn );
    dY=Tv_step( F.Ly )/5;
    if( (Vv&0x20)==0 )
    for( i=0; i<Nm; i++ )F.Ly+=Extrem( Ms[i],Min[i],dY );
    Tv_place( 0,&F );
    Tv_rect();
  }
  Ghelp( "<Enter>+Name <Space>-List only" ); color( BLUE );
  Tv.Height( Tv.Th/2 );
  Tm.UnPack( M0,D0,Y0 );
  if( (Vv&0x100)==0 )
  { p.x=6,p.y=Tv_port.bottom+Tv.Th+4;
    Tv.Text( South_East,p,_Fmt( "%d-%s-%02d %s %s",Y0,Months[M0-1],D0,
             fabs(Tm.T)<1e-4?"-":DtoH( Tm.T,3 ),Weeks[Tm.D%7] ) );
  } P.x=Tm.T;
  if( Vv&0x80 )goto Without_Y;        // обход фоновой разметки осей функции
  color( CYAN );
  if( Vv&0x20 )                       // 0-абсолютная, 1-относительная разметка
  { for( i=1; i<=20; i++ )                      // Вертикальная шкала
    { P.y=i*dY/2; moveto( p=P );                // в относительных отсчетах
      p.x+=i%10==0?8:i%5==0?6:3; lineto( p );
      if( i==10 ){ p.x-=11; Tv.Text( West,p,_Fmt( "%0.3lg",dY*5 ) ); }
    }
    for( i=0,w=0; i<Nm; i++ )
    { Mario &M=Ms[i];
      w-=Min[i];   P.x=Tm.T;
                   P.y=M.Mid+w; M.Level=w;
      if( Vv&0x40 )Horiz_Line( Tm.T,Tm.T+Tn,P.y );
      if( M.Max>M.Min )
      if( Vv&0x10 )
      { if( fabs( M.Mid )>1e-3 )
        { p=P; p.x-=10; Tw.Text(       West,p,_g( M.Mid ) ); }
               P.x+=Tn;
          p=P; p.x-=3;
               p.y-=2;  Tw.Text( North_West,p,_g( M.Max ) );
               p.y+=4;  Tw.Text( South_West,p,_g( M.Min ) );
      } w+=M.Max;                               //
    }                                           // Вертикальная шкала с
  } else                                        // равномерной разбивкой
  for( i=0,w=0,P=Tm.T; i<Nm; i++ )              //
  { Mario &M=Ms[i]; w-=Min[i]; M.Level=w;
    if( !i && (Vv&0x10) )
    { color( BLUE );
      Tv.Text( South_West,P,_Fmt( "\x7F \xF1%0.3lg ",dY*1.0005 ) );
      color( CYAN );
    }
    for( t=Min[i]+dY; t<M.Max+dY/2; t+=dY )
    if( i==Nm-1 && t>M.Max-dY/2 )break; else
    { P.y=w+t;
      if( Vv&0x40 )Horiz_Line( Tm.T,Tm.T+Tn,P.y ); // сетка на главных отметках
      if( Vv&0x10 )                               // 1-снятие числовых подписей
      { p=P; line( p.x,p.y,p.x+3,p.y );
        if( M.Max-Min[i] < dY*1.5 )
        { p.y=Tv_y( w+M.Mid ); p.x-=8;
          Tw.Text( West,p,_Fmt( "%s>",_g( M.Mid,dY ) ));
        } else
        { if( t<M.Max-dY/2 ){ p.x-=10; Tw.Text( West,p,_g( t,dY ) ); } }
    } } w+=M.Max;
  }
Without_Y:
  if( Vv&0x08 )goto Without_X;                  //
  color( CYAN ); P.y=0; k=6;                    // Горизонтальная ось - точки
  if( Vv&0x02 )                                 //
  { if( (k=c=(int)Tv_step( Tn/dT ))<=1 )k=c=1; else if( c<10 )k=1; else k=c/5;
    for( i=k; i<=int( (Tn-dT)/dT ); i+=k )
    { P.x = floor(Tm.T/dT+1)*dT + i*dT;
      moveto( p=P );
      if( i%c )
      { p.y-=2; lineto( p ); }else{ if( Vv&4 )Segment_Line( P.x,BLUE );
        p.y-=6; lineto( p );
        if( Vv&0x01 ){ p.y+=10; Tw.Text( South,p,_Fmt( "%u",i ) ); }
    } }                                         //
  } else                                        // Горизонтальная ось - часы
  if( (t=Tv_step( Tn ))<24.1 )                  //  k - вторичное дробление
  { int D,d; c=1;                               //
    if( Tn<0.018 )c=3600; else if( Tn<1.1 )c=60;
    if( c>1 )
    { if( (t=Tv_step( Tn*c )/c)<3e-4 )k=5; } else
    { if( t>12 )t=12; else
      if( t>6  )t=6;  else
      if( t>3  )t=3;  else
      if( t>1  )t=1;
    } t/=k; P.x=floor(Tm.T/t+1)*t; D=int( P.x/24 );
    for( ; P.x<Tm.T+Tn; P.x+=t )
    { moveto( p=P );
      if( int(P.x/t+0.45)%k )
      { p.y-=2; lineto( p ); }else{ if( Vv&4 )Segment_Line( P.x,BLUE );
        p.y-=6; lineto( p );                      //
        if( Vv&0x01 )                             // Подпись оси времени
        { p.y+=9; d=int( P.x/24 ); color( CYAN ); //
          if( D==d )Tw.Text(South,p,DtoH(P.x-Real(D)*24,Tn<0.009?-3:c>1?3:2));
          else{ p.y++; Jprint( p,0,0,d-D ); D=d; }
    } } }
  } else
  { int  y,m,d,b,ld;   long j,J=Tm.D,Jd2=J+long(Tn/24);
         y=m=d=b=ld=0;                          //
    if( (t/=24)<32 ){ k=5; d=(int)t;            // Один месяц
      if( d>15 )d=15;    else                   //
      if( d>10 )d=10;    else
      if( d>5  )d=5;     else
      if( d>2  )d=2,k=6; else d=1,k=8;  D0=((D0-1)/d)*d; if( !D0 )D0=1;
    } else                                      //
    if( (t/=30)<13 ){    m=(int)t;              // Один год
      if( m>6 )m=6; else                        //
      if( m>3 )m=3; else m=1;           M0=((M0-1)/m)*m+1; D0=1;
    } else
    { y=(int)Tv_step( Real(Jd2-Tm.D)/360 );
      if( y>4 )k=5; else y=1;           Y0=(Y0/y)*y; D0=M0=1;
    }
    J=julday( M0,D0,Y0 ); P.x=dY=double(J-Tm.D)*24;
    for( ;; )
    { j=julday( m+M0,d+D0,y+Y0 ); ld=d; dY+=double( d?d:j-J )*24;
      t=(dY-P.x)/k;             //
      if( d>1){ int mm,dd,yy;   // Переход дня через месяц
        julday( J=j,mm,dd,yy ); //
        if( mm!=M0 ){ mm=M0+1; dd=1; }
            else    { dd = (dd/d)*d; }
        j=julday( mm,dd,Y0 ); ld+=int(j-J); dY+=double(j-J)*24;
      }
      for( ;P.x<dY+t/2; P.x+=t ){ if( P.x>Tm.T+Tn )goto Without_X;
        if( !b )if( P.x>Tm.T )b=1;
        if( b ){ moveto( p=P );
          if( P.x<dY-t/2 )
          { p.y-=2; lineto( p ); }else{ if(Vv&4)Segment_Line(P.x,BLUE);
            p.y-=6; lineto( p ); b=2;
            if( Vv&0x01 ){ p.y+=11; Jprint( p,y,m,ld ); }
        } }
      } P.x=dY; J=j; if( b<2 )julday( j,M0,D0,Y0 );
  } }
Without_X:                                 //
 int mode=0;                               //! собственно рисование графиков
  for( Nb=k=0,w=0; k<Nm; k++ )             //!         начинается здесь
  { int i,j,c;                             //
   Mario &M=Ms[k]; w-=Min[k]; color( GREEN );
   Event T=M.JT;
    if( (j=Tv_getw())!=0 ){ if( j==_Enter || j==' ' )mode=j; else mode=0; }
    if( mode!=' ' )
    if( T<Tm+Tn && Tm<T+M.dt*M.N )
    { if( T<Tm )T += M.dt*(i=int( (Tm-T)/M.dt )); else i=0;
      { double t=T-Tm;
        for( j=c=0; i<M.N && t<=Tn; j++,i++,t+=M.dt )
        { P.x=Tm.T+t; P.y=M.V[i];
          if( M.Typ==3 )
          { if( P.y<M.Min ){ if( !c )c=GREEN; P.y=M.Mid; }
                        else if( !c )c=CYAN;
          } P.y+=w;
          if( !j )moveto( q=p=P ); else
          if( q!=(p=P) )
          { if( c==CYAN  ){ color( c ); c=0; } lineto( q=p );
            if( c==GREEN ){ color( c ); c=0; }
    } } } }
    if( M.Min || M.Max )
    { if( mode || Vv&0x200 )
      { strncpy( Id,M.Title,Mario_Title_Length ); Id[strcut(Id)]=0; color(BLUE);
//      scpy( Id,M.Title,Mario_Title_Length )[0]=0; strcut( Id ); color( BLUE );
        if( Id[0]!=250 || Id[1]!=0 )         // имя точка = 250 - опускается
        { p.x=Tv_x( Tm.T )+8;
          p.y=Tv_y( M.Mid+w )+3; Tv.Text( East,p,Id );
        }
      } //M.Cross();
    } if( M.flag&1 )Nb++; w+=M.Max;
  }     Tv.Height( 0 );
  Nx=0; Ghelp(); Allocate( 0,Min ); m_Mark(); color( cl );
}
void Mario::Cross()
{ point p,q; int c=color( flag&1?BLACK:getbkcolor() );
  p.x = Tv_x( Tm.T );            q.x=p.x-10;
  p.y = Tv_y( Mid+Level );       q.y=p.y-4;
  moveto( p ); lineto( q );      q.y+=8;
               lineto( q ); lineto( p ); color( c );
}
void static mark( Real y )
{ int c=color( LIGHTMAGENTA );
 point p; p.x=Tv_x( Tm.T )-5; p.y=Tv_y( y );
  setwritemode( XOR_PUT );
  if( (Vv&0x20)==0 || (Vv&0x10)==0 )
  { point q=p; q.x-=45; needle( q,p ); } else
  { p.x+=8;    moveto( p );     p.y-=Tw.Th/2;
    p.x-=13;   lineto( p );
    p.x =0;    lineto( p );
  } setwritemode( COPY_PUT ); color( c );
}                              //
void m_Mark( int St )          // ё1  Перемещение маркера
{ static int sW=0;             // ' ' Временное стирание информации
         int c=color( GREEN ); //  0  Восстановление после стирания
  {                            //
   Mario &M=Ms[Km];
    if( abs( St )==1 )
    { mark( M.Mid+M.Level ); Km+=St;
      if( Km<0 )Km=Nm-1; else
      if( Km>=Nm )Km=0;
    } else
    { if( sW )Tback();
      if( St==' ' )sW=0; else{ sW=1; Twindow( 0,0,36,2,0 ); }
  } }
  {
   Mario &M=Ms[Km];
    if( Nm>1 )mark( M.Mid+M.Level );
    if( sW ){ int m,d,y;
      M.JT.UnPack( m,d,y );
      scpy( Id,M.Title,Mario_Title_Length )[0]=0;
      Tgo( 1,1 );                              color( BLUE );
      Tprintf( "%20s",Id );                    color( MAGENTA );
      Tprintf( "%8s", DtoA( M.Latitude,2 ) );
      Tprintf( "%s\n",DtoA( M.Longitude,2 ) ); color( BLUE );
      Tprintf( "%5d-%02d-%02u ",y,m,d );       color( GREEN );
      Tprintf( DtoH( M.JT.T,2 ) );             color( CYAN );
      Tprintf( " + %d\xFA%s\n", M.N,DtoH( M.dt,-3 )+1 ); // '·'
  } } color( c );
}
