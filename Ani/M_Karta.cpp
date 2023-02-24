//
//      Mario.Karta
//      Как-то надо справляться с бесчисленным множеством мореограмм
//        - вариант сбраса их на контурную карту с массштабированием
//
#include "Mario.h"

int World_Map( int,Field*,Field* );

static Field Map={ 139,42,21,11 }, // Map изменяется в World_Map
             Plc={ 139,42,21,11 }; // Plc текущее активное поле
static char str[Mario_Title_Length+2]="+";
//
//   при каждом вызове экстремальные отсчеты переопредлеляются заново
//
void m_Karta()
{                                         //
 static int  WinWide=6,                   // 180 Размеры индивидуального окошка
             WinHigh=48;                  // для каждого из графиков мореограмм
 static bool isName=true;                 // Названия пунктов (<space>)
 int    i,j,k,ans,Wmap=0;                 // Wmap принудительное рисование
 double Min,Max,t,V,W,wL;                 //
 field _f = { 4,6,95,93,0 }; Tv_place( &_f );
 Field _F = { 0,0,1.0,1.0 }; Tv_place( 0,&_F );
  setactivepage( 1 ); clear();            // Работа с графическим изображением
  for( k=0; k<Nm; k++ )                   // Поиск координатных размерностей
  { Mario &M=Ms[k];                       //
    if( !k )
    { Map.Jx=Map.Lx=M.Longitude;
      Map.Jy=Map.Ly=M.Latitude;
    } else
    { if( Map.Jx>M.Longitude )Map.Jx=M.Longitude;
      if( Map.Lx<M.Longitude )Map.Lx=M.Longitude;
      if( Map.Jy>M.Latitude  )Map.Jy=M.Latitude;
      if( Map.Ly<M.Latitude  )Map.Ly=M.Latitude;
  } }
  Map.Jx=int( Map.Jx )-1; Map.Lx=( int( Map.Lx+1 )+1 )-Map.Jx; // плюс градус в
  Map.Jy=int( Map.Jy )-1; Map.Ly=( int( Map.Ly+1 )+1 )-Map.Jy; // уширение поля

Repeat_Chart_from_World_Map:
  wL=0.0;                                     //
  setvisualpage( 0 );                         // -- в фоновом режиме
  W = cos( M_PI*( Map.Jy+Map.Ly/2.0 )/180 );  // Установка пропорциональности
  V = double( Tv_port.right-Tv_port.left )    // для географической карты
    / double( Tv_port.bottom-Tv_port.top );   // на середину приведенной широты
  if( V*(Map.Ly)/(Map.Lx)/W < 1.0 )           //
    { W=Map.Lx*W/V; Map.Jy=Map.Jy+(Map.Ly-W)/2; Map.Ly=W; } else
    { W=Map.Ly/W*V; Map.Jx=Map.Jx+(Map.Lx-W)/2; Map.Lx=W; }
                                           //
  Wmap=World_Map( Wmap=0,&Map,&Plc );      // Wmap=1 - если карта уже на экране
                                           //
  for( k=0; k<Nm; k++ )                    // размах колебаний
  { Mario &M=Ms[k];                        //
    if( M.Latitude>Map.Jy && M.Latitude<Map.Jy+Map.Ly )
    if( M.Longitude>Map.Jx && M.Longitude<Map.Jx+Map.Lx )
    if( wL<M.Max-M.Min )wL=M.Max-M.Min;
  }
  for( k=0; k<Nm; k++ )                // Собственно прорисовка данных
  { Mario &M=Ms[k];
    Field F = { Tm.T,0.0, Tn,wL };
    if( M.Latitude>Map.Jy && M.Latitude<Map.Jy+Map.Ly )
    if( M.Longitude>Map.Jx && M.Longitude<Map.Jx+Map.Lx )
    { Event T = M.JT;
      point a,b,p = { Tv_x( M.Longitude ),Tv_y( M.Latitude ) };
      //
      //  Разметка прямоугольника для будущего графика
      //
     field f;
      f.Ly=WinHigh-p.y;       f.Jy=p.y-Tv.mY; f.wb=0;
      f.Lx=WinWide-Tv.mX+p.x; f.Jx=-p.x;
      //
      //  Подготовка размерностей для виртуальной удвоенной полуплощадки
      //
      Tv_place( &f );    color( WinWide==360?CYAN:LIGHTBLUE );
      Tv_place( 0,&F );  // Tv_rect();
      if( T<Tm+Tn && Tm<T+M.dt*M.N )
      { if( T<Tm )T+=M.dt*(i=int( (Tm-T)/M.dt )); else i=0; t=T-Tm; Min=Max=0;
        for( j=0; i<M.N && t<=Tn; j++,i++,t+=M.dt )
        { Point P = { Tm.T+t,M[i]-M.Mid };
          if( Min>P.y )Min=P.y; else if( Max<P.y )Max=P.y;
          if( WinWide!=6 )
          { b=P; if( !j )moveto( a=b ); else if( a!=b )lineto( a=b );
          }
        }
        color( LIGHTRED );
        if( WinWide==6 )
        { bar3d( p.x-1,Tv_y( Max ),p.x+1,p.y,1,1 );
          bar3d( p.x-1,p.y,p.x+1,Tv_y( Min ),1,0 );
        } else line( p.x,Tv_y( Max ),p.x,Tv_y( Min ) );
      }
      Tv_place( &f );
      Tv_place( 0,&F );
      color( WinWide>6?RED:BLUE ); line( p.x-1,p.y,p.x+2,p.y );
      if( isName )
      { strcut( strncpy( str,M.Title,Mario_Title_Length ) );
        color( GREEN );  Tw.Text( South,p.x,p.y,str );
      }
    }
  }
ReAns:
  Thelp( "<Enter>карта <Space>имена <left/right>длина[%d] <up/down>высота{%d} <Esc>выход ",WinWide,WinHigh );
  setvisualpage( 1 );  ans = Tv_getc(); //Tv_revert( true );
  switch( ans )
  { case _Right: if( WinWide==360 )goto ReAns;
                 if( WinWide<360  )WinWide+=WinWide/3;
                 if( WinWide>360  )WinWide=360; break;
    case _Left:  if( WinWide==6   )goto ReAns;
                 if( WinWide>6    )WinWide-=WinWide/3;
                 if( WinWide<6    )WinWide=6;   break;
    case _Up:    if( WinHigh==240 )goto ReAns;
                 if( WinHigh<240  )WinHigh+=WinHigh/3;
                 if( WinHigh>240  )WinHigh=240; break;
    case _Down:  if( WinHigh==36  )goto ReAns;
                 if( WinHigh>36   )WinHigh-=WinHigh/3;
                 if( WinHigh<36   )WinHigh=36; break;
    case _Enter:
    { Real f=Map.Jy, ff=f+Map.Ly,
           l=Map.Jx, ll=l+Map.Lx; color( GREEN );
      Mlist Menu[]={ { 0,8,"      %2°",&ff }
                   , { 1,8,      "%2°",&l  }
                   , { 0,8,  "=+= %2°",&ll }
                   , { 1,8,"      %2°",&f  }
                   };
      Tmenu( Mlist(Menu),0 ); if( ll<l )ll+=360;
      Map.Jy=f; Map.Ly=ff-f;
      Map.Jx=l; Map.Lx=ll-l;
      Map.Jx=Angle( Map.Jx );
    } break;
    case _Space: isName ^= true;
    case _Esc:    break;
    default: goto ReAns;
  }
  if( ans!=_Esc )goto Repeat_Chart_from_World_Map;
  setactivepage( 0 );
  setvisualpage( 0 );
  Tv_place( 0,&_F );
  Tv_place( &_f );
}
