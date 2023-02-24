//
//      Инициализация списка карт и открытие главной карты
//              Блок процедур из раздела Sakhalin.cpp,
//              использующий его локальные данные и процедуры
//
static void XOR_rect( viewporttype &r )
{ int c=color( LIGHTGREEN^WHITE );
  setwritemode( XOR_PUT  ); rectangle( r.left,r.top,r.right,r.bottom );
  setwritemode( COPY_PUT );
  color( c );
}
static void XOR_small()                 //
{  int c=color( LIGHTBLUE^WHITE );      // Рамка в маленьком окошке
 Field &M=FullMap->MVi->_F;             //
  PictMap->SetView( false );            // установка массштабов
  setwritemode( XOR_PUT );
  rectangle( Tv_x(M.Jx),Tv_y(M.Jy+M.Ly),Tv_x(M.Jx+M.Lx),Tv_y(M.Jy) );
  setwritemode( COPY_PUT );
  color( c );
}
static void Restore_Small_Picture()
{ if( FullMap->MVi!=PictMap->MVi )
    { PictMap->MVi=FullMap->MVi; PictMap->Draw(); XOR_small();
      FullMap->SetView( true );
}   }
//
// Отображение карты и инициализация всех связанных с ней экранных объектов
//
void Map::Display( bool Full ) // =1 безусловное заполнение всех рисунков
{
  if( Full ){ RefMap=0; FullMap->MVi=this; FullMap->Draw();
            }           PictMap->MVi=this; PictMap->Draw();
  if( Full )         //
  { ActiveMap=this;  // Простановка кнопочек под выбор отображаемых слоев
    InitMetaLayers( LayersType );
    XOR_small();
  } Status_Line->Print( 1,"%4ld Мб",coreleft()>>20 );
}
//    Логика выборки и прорисовки катинки на маленьком или большом экране
//      ( по необходимости с дочитыванием собственно данных из файла )
//
void Map::Find()
{ bool isMap=false;

  if( DstName==NULL )return;
  if( *DstName==0 )return;
  Status_Line->Print( 0,"%s: \"%s\"",(isMap=mapLL.nL>0)?"Выбор":"Чтение",DstName );
  if( !isMap )Read();
  if(  mapLL.nL<=0 )return;
  if( isMap )Display(); else
  { if( iDispType&dtpChart ){ F.Jx=P.x,F.Jy=P.y,F.Lx=S.x,F.Ly=S.y; _F=F; } //  _F=F=Frame(); }
    Display( 0 );
} }
//      Выделение блока увеличения на основной карте и увеличение карты
//
static bool ZoomOnFull( int _iM )
{
 int Key;
 MapView &MZ = (_iM==__FullMap)?*FullMap:*PictMap;
  if( Tv_MouseState!=_MouseRightPress )return 0; XOR_small();
  if( _iM==__FullMap )FullMap->SetView( true );
             else     PictMap->SetView( false );
 viewporttype w,&W=MZ.Win; w.right=w.left=Tv_MouseX();
                           w.bottom=w.top=Tv_MouseY(); RefMap=0;
  for(;;)
  { XOR_rect( w ); Key=Tv_getc();
    XOR_rect( w );
    if( Key==_MouseReleased )break;
    if( Key==_MouseMoved ){  w.right =Tv_MouseX();
                             w.bottom=Tv_MouseY();
      if( w.right<W.left    )w.right=W.left;
      if( w.right>W.right   )w.right=W.right;
      if( w.bottom<W.top    )w.bottom=W.top;
      if( w.bottom>W.bottom )w.bottom=W.bottom;
      Tv_SetCursor( w.right,w.bottom );
  } }
  if( w.top!=w.bottom || w.left!=w.right )
  {                     //
   Field &_M=MZ.MVi->_F, // Проверки, проверки...
          &M=MZ.MVi->F;  //
    if( w.right==w.left )++w.right; else
    if( w.right<w.left ){ Key=w.left; w.left=w.right; w.right=Key; }
    if( w.bottom==w.top )++w.bottom; else
    if( w.bottom<w.top ){ Key=w.top; w.top=w.bottom; w.bottom=Key; }
    _M.Jx=Tv_X( w.left   );      if( _M.Jx<M.Jx )_M.Jx=M.Jx;
    _M.Jy=Tv_Y( w.bottom );      if( _M.Jy<M.Jy )_M.Jy=M.Jy;
    _M.Lx=Tv_X( w.right )-_M.Jx; if( _M.Lx>M.Lx )_M.Lx=M.Lx;
    _M.Ly=Tv_Y( w.top   )-_M.Jy; if( _M.Ly>M.Ly )_M.Ly=M.Ly;
    FullMap->SetView( true );
   viewporttype &W=FullMap->Win;
    _M.Jx=Tv_X( W.left   );      if( _M.Jx<M.Jx )_M.Jx=M.Jx;
    _M.Jy=Tv_Y( W.bottom );      if( _M.Jy<M.Jy )_M.Jy=M.Jy;
    _M.Lx=Tv_X( W.right )-_M.Jx; if( _M.Lx>M.Lx )_M.Lx=M.Lx;
    _M.Ly=Tv_Y( W.top   )-_M.Jy; if( _M.Ly>M.Ly )_M.Ly=M.Ly;
   float Z;
    Z = Tv_X( W.right ); if( Z>(M.Jx+M.Lx) )_M.Jx=(M.Jx+M.Lx)-_M.Lx;
    Z = Tv_Y( W.top   ); if( Z>(M.Jy+M.Ly) )_M.Jy=(M.Jy+M.Ly)-_M.Ly;
    FullMap->Draw();     XOR_small();
    return true;
  } return false;
}
//      Перемещение рамки просмотра карты на уменьшенной карте
//              и переход к новому изображению
//
static void ZoomOnSmall()
{
 int x,y,xx,yy,Key;
 viewporttype &W=PictMap->Win,w;
  PictMap->SetView( false );
  if( Tv_MouseState!=_MouseLeftPress )return;
 Field &_M=FullMap->MVi->_F;
      x=Tv_MouseX(); w.left  =Tv_x( _M.Jx ); w.right=Tv_x( _M.Jx+_M.Lx );
      y=Tv_MouseY(); w.bottom=Tv_y( _M.Jy ); w.top  =Tv_y( _M.Jy+_M.Ly );
  if( x<w.left || x>w.right  )return;
  if( y<w.top  || y>w.bottom )return;
  XOR_small();
  for( ;; )
  { XOR_rect( w ); Key=Tv_getc();
    XOR_rect( w );
    if( Key==_MouseReleased )break;
    if( Key==_MouseMoved )
    {     x=( xx=Tv_MouseX() )-x;
          y=( yy=Tv_MouseY() )-y;
      if( x+w.right<W.right && x+w.left>W.left )
        { w.left+=x; w.right+=x; x=xx; } else x=xx-x;
      if( y+w.bottom<W.bottom && y+w.top>W.top )
        { w.top+=y; w.bottom+=y; y=yy; } else y=yy-y; Tv_SetCursor( x,y );
  } } _M.Jx=Tv_X( w.left );   _M.Lx=Tv_X( w.right )-_M.Jx;
      _M.Jy=Tv_Y( w.bottom ); _M.Ly=Tv_Y( w.top )-_M.Jy;
      FullMap->Draw();
      XOR_small();
}
///   Вывод краткой информации по объекту в окошке внизу экрана
//
static void ObjectInfo()            /// Программа оформляет краткое описание
{ //if( RefMap!=IDT )               /// графического объекта в специально для
  if( RefMap!=NULL )                ///      этого выделенном окне
  { int i,_sP=0;                    /// А также рисует звездочку и др.бантики
   viewporttype &W=Win_Texts->Win;  //
   point p={ W.left+3,W.top+1 };
   //
   //  Подготовка шрифта и начало вывода краткого описания
   //
   static int lH=0,fH=0;
   static char **_d=NULL;
   static Bfont *T;
    if( fH==0 ){ if( Tv.mY>800  )lH=4;        else
                 if( Tv.mY>600  )lH=2;        else
                 if( Tv.mY<=350 )lH=-5, fH=1; else
                 if( Tv.mY<=480 )lH=-2; fH+=(W.bottom-W.top)/6;
                 T = new Bfont( _Small_font,fH-lH );
               }
    if( _d!=RefMap->Comment )Win_Texts->Draw();    // Перетирка старого
    if( !RefMap->Comment )return; color( BLUE );   //
    if( RefMap->Title )Tv.Text( p,RefMap->Title ); // информационного окна
    if( _d!=RefMap->Comment )                      // с учетом быстроменяющейся
    { _d=RefMap->Comment;                          // титульной строчки
      color( BLACK ); p.y+=fH+1;
      if( _d )for( i=0; _d[i] && i<5; i++,p.y+=fH )T->Text( p,_d[i] );
      //
      // Проверка информационных отметок и растрового окошка
      //
      if( RefMap->Contain )
      { i=(W.bottom-W.top)/3; color( YELLOW ); Geo_Page( W.right-i,W.bottom-i,i );
      }
//    if( RefMap->MapType )
      if( RefMap->DstName )
      if( RefMap!=&StartMap )
      { static float a=0.0;
        i=(W.bottom-W.top)/3;
        color( LIGHTCYAN ); Geo_Star( W.right-i,W.bottom-i,i,a );
        a+=M_PI_4/6.0;
        if( RefMap->Small_Image!=NULL )
          { PictMap->MVi=RefMap; PictMap->Draw(); _sP=1; }
      }
      if( !_sP )Restore_Small_Picture();
    }
  } //IDT=RefMap; // это сильно устарело, однако пусть пока будет тоже
}
///  Координаты мышки и подвод стрелки к ближайшему объекту
 //      Здесь отслеживается местоположение мышки и
 //      задаются контекстные подсказки, кнопочки и пр.
 //
static void PutLocation()
{
 const char *s[3]={ "F1 подсказка","<R-btn> рамка","F10 выход" };
 static int i0=0;
 static Map *pM=NULL; if( pM!=PictMap->MVi ){ i0=0; pM=PictMap->MVi; }
 Map *cM=ActiveMap;               //
  if( FullMap->MouseThere() )     // Мышка обнаружена в большом окне!
  {   FullMap->SetView( true );   // Вывод координат
   float X = Tv_X( Tv_MouseX() ), //
         Y = Tv_Y( Tv_MouseY() );
    if( i0!=1 )
    { int c1=0,c2=0; if( RefMap )c1=RefMap->Contain!=NULL,
                                 c2=RefMap->DstName!=NULL; i0=1;
      Status_Line->Print( 0,"%s, %s, %s",
                      c1  ? "<Space> Текст"   :s[0],
                      c2  ? "<Enter> Рисунок" :s[1],
             cM->Previous ? "<Escape> Возврат":s[2] );
    } Status_Line->Print( 2,"(%.2f;%.2f)",X,Y );
    //
    // Поиск и установка указателя на ближайший объект
    //
    RefMap=FullMap->MVi->FindMouse( (Point){ X,Y } ); // поиск в активном списке
    ObjectInfo();
  } else
  { if( RefMap )
    { //if( !ActiveMap->MapType )
                                { RefMap=FullMap->MVi; ObjectInfo(); }
      RefMap=0;
      i0=2;
    } Restore_Small_Picture();
    if( PictMap->MouseThere() )
    { PictMap->SetView( false );
      if( i0!=1 )
      { Status_Line->Print( 0,"%s, %s, <L-btn> Сдвиг",s[0],s[1] ); i0=1;
      } Status_Line->Print( 2,"(%.2f;%.2f)",Tv_X( Tv_MouseX() ),
                                            Tv_Y( Tv_MouseY() ) );
    } else {
     static viewporttype CW={ 0,Tv.mY-Tv.Th-7,20,Tv.mY,1 };
     static BitButton *SC=NULL,*EX=NULL,*PR=NULL,*PM=NULL,*NL=NULL,*HL=NULL,*PS=NULL,*PH=NULL;
     static int Scale=0,ObNum=0,First=0,Prevs=0,Pscal=0;
     static bool w=false;
      Status_Line->Print( 2,"" );
                 w=(LayersType&TxtBrowse)!=0 || cM->txtLL.Number_of_Objects()>1;
      if( ObNum!=w ){ i0=3; ObNum=w; } w=memcmp(&FullMap->MVi->_F,&FullMap->MVi->F,sizeof(Field))!=0;
      if( Scale!=w ){ i0=3; Scale=w; } w=cM!=&StartMap;
      if( First!=w ){ i0=3; First=w; } if( cM->Previous )w=cM->Previous!=&StartMap; else w=0;
      if( Prevs!=w ){ i0=3; Prevs=w; } w = !cM->MapType;
      if( Pscal!=w ){ i0=3; Pscal=w; }
      if( i0 )Status_Line->Print( 0,"" );
      if( i0==3 || !EX )
      { CW.left=8;
        if( SC ){ delete SC; SC=0; } // кнопка востановления исходного масштаба
        if( PR ){ delete PR; PR=0; } // --//-- возврата к предыдущему
        if( PM ){ delete PM; PM=0; } // --//-- перехода к исходному рисунку
        if( PS ){ delete PS; PS=0; } // переключатель режима ПДК - абсолют
        if( PH ){ delete PH; PH=0; } // кнопочка для Start Sakhalin-table.html
        if( NL ){ delete NL; NL=0; } // вызов списка заголовков метатекста
        if( !HL )HL=new BitButton( CW,"?.",0xF1 ); else HL->Draw(); CW.left=HL->scr.right+4;
        if( !EX )EX=new BitButton( CW,__ExitBtn,idExit ); else EX->Draw();
                                                                    CW.left=EX->scr.right+4;
        if( Scale ){ SC=new BitButton( CW,"1:1",        idNoZoom ); CW.left=SC->scr.right+4; }
        if( First ){ PM=new BitButton( CW,"Исходный",  idMainMap ); CW.left=PM->scr.right+4; }
        if( Prevs ){ PR=new BitButton( CW,"Предыдущий",idPrevMap ); CW.left=PR->scr.right+4; }
        if( Pscal ){ PS=new BitButton( CW,"<ПДК>", idPDKabs, PdkModeView ); CW.left=PS->scr.right+4;
                     PH=new BitButton( CW,"Html",  idHtml ); CW.left=PH->scr.right+4; }
        if( ObNum )  NL=new BitButton( CW,"Список",idObjBrowse );
      } else if( i0 )
      { if( HL )HL->Draw(); if( EX )EX->Draw(); if( SC )SC->Draw();
        if( PS ){ PS->State=PdkModeView; PS->Draw(); PH->Draw(); }
        if( PM )PM->Draw(); if( PR )PR->Draw(); if( NL )NL->Draw();
      } i0=0;
if( PS ){ PS->State=PdkModeView; PS->Draw(); } /// !!! ??? !!!
    }
  }
}
///      Маленький комплекс процедур для операций по измерению растояний
///                Включается LMouse при нажатом Ctrl
//
static Point *PCtrl=NULL;
static int Ctrl=0,CtrlView=0;               //
static int Ctrl_Add()                       // Новая измерительная нить
{ if( Tv_getk()&(CTRL|ScrollLock) )         //
  { (PCtrl=(Point*)Allocate( sizeof( Point )*(Ctrl+1),PCtrl ))[Ctrl]
      = (Point){ Tv_X( Tv_MouseX() ),Tv_Y( Tv_MouseY() ) }; return ++Ctrl;
  } return 0;
}
static bool Ctrl_Del()
{ if( Ctrl ){ Ctrl=0; Allocate( 0,PCtrl ); PCtrl=NULL; return true; } return false;
}
static float Ctrl_Distance()
{ if( Ctrl )
  { float Dsts=0.0,Fi; int i;
    Point Psts;
    for( i=1; i<=Ctrl; i++ )
    { if( i<Ctrl )Psts=PCtrl[i];
             else Psts=(Point){ Tv_X(Tv_MouseX()),Tv_Y(Tv_MouseY()) };
      if( ActiveMap->MapType )Fi=M_PI*(Psts.y+PCtrl[i-1].y)/360; // широта
          Psts -= PCtrl[i-1];
      if( ActiveMap->MapType ){ Psts *= 60.0*1.8526; Psts.x *= cos( Fi ); }
          Dsts += abs( Psts );
    }     return Dsts;
  }       return 0.0;
}                                               //
static void Ctrl_Line( const int v )            // 0 - стереть; 1 - рисовать
{ if( CtrlView^v )                              //
  { int c=color( LIGHTRED^WHITE );              //
   static int q=0;                              // Измерительная нить
   static Point P,Q; setwritemode( XOR_PUT );   //
    if( v ){ P.x=Tv_X( Tv_MouseX() ); P.y=Tv_Y( Tv_MouseY() ); }
    if( Ctrl )
      { int i; for( i=1; i<Ctrl; i++ )line( PCtrl[i-1],PCtrl[i] );
                                      line( PCtrl[i-1],P ); }
    if( FullMap->MouseThere() || q )            //
      { color(LIGHTCYAN^WHITE);                 // Стрелка и отметка объекта
        if( v && RefMap )                       //
          { Q=Point( *RefMap ); q=1; needle( P,Q ); CrossAt( RefMap ); } else
          {                 if( q ){ needle( P,Q ); CrossAt( NULL ); } q=0;
      }   }
    setwritemode( COPY_PUT ); CtrlView^=1;
    color( c );
} }
