//
//   Window - ����������� �������� ��� ����� � ���� � ����������
//
#include "Window.h"

Field Window::Focus()
   { field t=f; Tv_place( &t );
     Field T=F; Tv_place( 0,&T ); b=Tv_port; return T;
   }
Course Window::Console( int Info( int,int,bool ) ) // Info=>0 - ����������
{ Course Ans;                                      //  true - c ������� �������
  for( ;; )                                        //  false - �������� �������
  {   Ans=Tv_getc();                               //   ret=-1 - ��������������
  for_Repeat:
    if( Ans<_MouseMoved || Ans>_MouseReleased )return Ans; // ����� ��� �����
    Tv_GetCursor( x,y );                                  // == ����� �������
    if( Info( x,y,Ans==_MousePressed ) ) //&& Tv_MouseState&_MouseLeftPress ) )
    { int X,ix,lx=b.right-b.left,                       // ������� � ���� �����
          Y,iy,ly=b.bottom-b.top;
      if( Ans==_MousePressed )           //&& Tv_MouseState&_MouseLeftPress )
      { ReScale: _x=x,_y=y; ix=iy=0;                    // ����� �������
        while( (Ans=Tv_getc())!=_MouseReleased )        // ��� ���������������
        { Tv_rect_x( _x,_y,x,y );                       //
          Tv_GetCursor( x,y ); X=x,Y=y;
          if( !ix ){ if( x>b.right )ix=1,x-=lx; else if( x<b.left )ix=-1,x+=lx; }
              else   if( ix<0 && x>=b.right || ix>0 && x<=b.left )x+=ix*lx,ix=0;
          if( !iy ){ if( y>b.bottom )iy=1,y-=ly; else if( y<b.top )iy=-1,y+=ly; }
              else   if( iy<0 && y>=b.bottom || iy>0 && y<=b.top )y+=iy*ly,iy=0;
          if( ix>0 && x>_x )x=_x-1; else if( ix<0 && x<=_x )x=_x+1;
          if( iy>0 && y>_y )y=_y-1; else if( iy<0 && y<=_y )y=_y+1;
          if( x!=X || y!=Y )Tv_SetCursor( x,y );        // Check( x,y );
          Tv_rect_x( _x,_y,x=minmax( _x-lx,x+ix*lx,_x+lx ),
                           y=minmax( _y-ly,y+iy*ly,_y+ly ) );
          Info( x,y,true );                             // ����� �� ������
        }
        if( _x==x && _y==y )return _MousePressed;       // ���� ��� ����������
//      if( _y>y ){ Y=y; y=_y; _y=Y; }                  //  ����� ������� ���
//      if( _x>x ){ X=x; x=_x; _x=X; }                  //   �������� ����� ���
  ReSelect:
        while( (Ans=Tv_getc())!=_MousePressed )
        { if( Ans<_MouseMoved || Ans>_MouseReleased )break; // � ������
          Tv_GetCursor( X,Y );         // Check( X,Y ); // �������� ���������
                  Info( X,Y,false );                    //  ������� �����
        }
        Tv_rect_x( _x,_y,x,y );                         // - ��������� �����
        if( Ans<_MouseMoved || Ans>_MouseReleased )return Ans; //   �� �����
        if( ix>0 && X<_x || ix<0 && X>_x )X += ix*lx;   // ���������� �������
        if( iy>0 && Y<_y || iy<0 && Y>_y )Y += iy*ly;   //  �� ������� ������
        if( ( (X-x)^(X-_x) )<0
         && ( (Y-y)^(Y-_y) )<0 )           //&& Tv_MouseState&_MouseLeftPress )
        { Ans=Tv_getc();
          if( Ans==_MouseMoved )                        // ����������� �����
          {
//          Tv_GetCursor( _X,_Y );                      // Info( 0,0,false );
//          Tv_rect_x( _x+=(_X-X),_y+=(_Y-Y),x+=(_X-X),y+=(_Y-Y) );
            Tv_rect_x( _x,_y,x,y );                     // �������� ����� �����
            Tv_SetCursor( X=_x,Y=_y );
            Tv_CursorDisable();
            while( (Ans=Tv_getc())==_MouseMoved )
            { Tv_rect_x( _x,_y,x,y );                   // ������� ������ �����
              Tv_GetCursor( _x,_y ); x+=_x-X,y+=_y-Y;   // ������������ ������
              Info( X=_x,Y=_y,true );                   // ����� �� �����
              if( !ix )
              { if( _x<b.left  )_x+=lx,x+=lx,ix=1;  else if( x>b.right )ix=1;
                if( _x>b.right )_x-=lx,x-=lx,ix=-1; else if( x<b.left )ix=-1;
              } else if( ix==1  ){ if( _x>b.right )ix=0,_x-=lx,x-=lx; else if( x<b.right )ix=0; }
                else if( ix==-1 ){ if( _x<b.left  )ix=0,_x+=lx,x+=lx; else if( x>b.left  )ix=0; }
              if( !iy )
              { if( _y<b.top    )_y+=ly,y+=ly,iy=1;  else if( y>b.bottom )iy=1;
                if( _y>b.bottom )_y-=ly,y-=ly,iy=-1; else if( y<b.top    )iy=-1;
              } else if( iy==1  ){ if( _y>b.bottom )iy=0,_y-=ly,y-=ly; else if( y<b.bottom )iy=0; }
                else if( iy==-1 ){ if( _y<b.top    )iy=0,_y+=ly,y+=ly; else if( y>b.top    )iy=0; }
              if( _x!=X || _y!=Y )Tv_SetCursor( X=_x,Y=_y );
                                  Tv_rect_x( _x,_y,x,y ); // ����� ������ �����
            }                     Tv_CursorEnable();
            goto ReSelect;
          } else
          { if( Ans!=_MouseReleased ){ x=X; y=Y; goto ReScale; }
            return (Course)-1;    //! -1 ����� � �������� �� ������������������
          }
        } goto for_Repeat;
      }
    } else
    if( Ans==_MousePressed )                                  // ����� ��� ����
    { if( (Ans=Tv_getc())==_MouseReleased )return _MousePressed;
      goto for_Repeat;
    }
  }
}
void Button::Place( int xt, int yt )
{ Tv.Height( 8 );
  if( xt<0 ){ f.Lx=-xt-Tv.mX; f.Jx=xt+Tv.StrLen( _sign )+4; } // ������ �����
      else  { f.Jx=-xt; f.Lx=Tv.StrLen( _sign )-Tv.mX;
            } f.Ly=-yt; f.Jy=yt+10-Tv.mY;
              Tv_place( &f ); b=Tv_port;       // ���������� ����� �������� �
              Tv_place( &f ); Tv.Height( 0 );  // �������������� ��������� ����
}
void Button::Initial( const char* sign, int xt,int yt )
   { if( _sign )free( _sign ); _sign=strdup( sign ); Place( xt,yt );
   }
void Button::Draw()
{ if( _sign ){ Tv.Height( 8 ); int c=color( BLUE );
               Tv.Text( South_East,b.left,b.top,_sign );
               rectangle( b.left,b.top,b.right,b.bottom );
               Tv.Height( 0 ); color( c ); }
}
//void Window::Check( int &X, int &Y )
//   { bool Ans=false;                              //
//     if( Y<b.top    ){ Ans=true; Y=b.top; } else  // ��������� ����� �
//     if( Y>b.bottom ){ Ans=true; Y=b.bottom; }    //  �������� ��������� ����
//     if( X<b.left   ){ Ans=true; X=b.left; } else //
//     if( X>b.right  ){ Ans=true; X=b.right; }
//     if( Ans )Tv_SetCursor( X,Y );
//   }
