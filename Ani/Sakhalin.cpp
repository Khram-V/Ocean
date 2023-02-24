/**
 *  �������������-���������������� ������� � �������������� �����������
 *  ���������� � ����������� ����������� ���������� ����� � �������
 *  ������������ ����� �������� ������ XML
 *
 *   (�)������� �������������� ��������
 *   (c)1996-2009 �, �.��������, ����������� ��������������� �����������,
 *         ����������� �������������� ������������� � ������� ������������
 *   (�)�.��������, ��������������� ��������������� ����������� �����������
 *
 *   <<  ������� ������ ���������  >>
 *                                              --enable-auto-import
 *                                              -static-libstdc++
                                                -Wl,-enable-auto-import
**/
static const char *HelpStrings[] =
    {  "",
       "        ������� �������������� ��������",
       "             ����������� ���������",
       "          ������ ������� ������������",
       "",
       " Sakhalin 3.2.1�,",
       "        ����������� ���������-�������������� ������� ",
       "",
       "                ���������� �������, �����, ������� �",
       "                   ���������-������������ ����������",
       "                    � ������� �������������� ������",
       "",
       " <L-btn>, (Enter) - ���������� � ����������� �������",
       " <LL-btn>,(Space) - �������� ������������ ����������",
//     "  <Shift>+(Space) - � ������������ �� ��������������",
       " <R-btn � �����>  - ������������� ���������������",
       " <L-btn � �����>  - ����� ����� �� ��������� �������",
       "",
       " <Ctrl+L-btn> - ����� ����� �� ������� ��� ���������",
//     " <ScrollLock> - �������� ������ ���������� �����",
       "",
//     "             <F2> - ������ (����������) ����������",
       "             <F3> - ���������� ���� ������� ������",
//     "       <Shift-F3> - ��������� ���� ��������� ������",
       "             <F7> - ������ ���� �������� ��������",
       "         (Escape) - ������� � ���������� ��������",
       "    (F10),<alt-X> - ���������� ���������",
       "",
       "                                       (c)2010.09.08",
       "",0
    };
#include "Sakhalin.h"
#include "..\Graphics\_Small_font.c++"
//
//    ��������� ����������
//
enum{ idFileOpen=256,      // ������ 255 ������� �� ������ Design-CAD-�
      idExit,              // ������ ������ �� ���������
      idNoZoom,            // ������ �������������� ��������� ��������
      idSelMapLayer,       // �������� ��������
      idSelObjLayer,       // �������� ��������� ����������
      idMainMap,           // --//-- �������� � ��������� �������
      idPrevMap,           // --//-- �������� � �����������
      idObjBrowse,         // --//-- - ����� ������ ���������� ����������
      idPDKabs,            // ������������ �������� � ��� - �������
      idHtml,              // �������� ��� ������ �������� � Html
      __FullMap,           // ������������� ������� ������
      __PictMap,           // ������������� ��������� ��������
      __WinList,           // ���� ������ ������� ��� ��������
      __WinText            // --//-- ������� ��������� ����������
    };                                  //
const  unsigned PicLayers=0x4000,       // 16� - ������� ���������� �������
                TxtLayers=0x2000,       //  8� - ������� ���������� ����������
                TxtBrowse=0x8000;       // 32� - ������� ������������ � ������
static unsigned LayersType=PicLayers;   //

Map  StartMap;               /// ����� ������ ����� ��������� ������� MainName
Map* ActiveMap=NULL;         /// ����� �������, ������������� �� ������
static Map* RefMap=NULL;     /// ����� ������� �� �������� ������
static PanelWin  *Win_Texts =0, /// ������ ������ � ������� ���������
                 *Win_Layers=0; /// ������ - ��� ������ �����
static BitButton *But_LD = 0,    //  �������� ��� ������ ����.�����
                 *But_LT = 0;    //  --//-- ����� ��������������� �����
                                 //
extern char __ExitBtn[],__MapLayerBtn[],__CheckBtn[],
                        __TxtLayerBtn[],__UncheckBtn[];
//  __________________________________________________
//          ����� ���������
//
static void Sakhalin_Help()
{ static viewporttype hF={ 0,0,Tv.mX-1,Tv.mY-Tv.Th-11,1 };
  Inform_Window( (char**)HelpStrings,hF,YELLOW,BLUE,false );
}
//     ����������� ���� � ������������ ��������
//
static struct MapView: PanelWin
{ Field F; Map *MVi;                    // �������� �������� ������������ ����
  MapView( viewporttype &f, unsigned id )
  : PanelWin( f,id,1,2,ptInnerLower|ptOuterRaise,1,1,WHITE ),MVi( NULL ){}
 void Draw();                           // 1 - ������; 0 - ���� ���� (!+Dto+!)
 void SetView( bool );                  //
}    *FullMap=NULL,                     // �������
     *PictMap=NULL;                     // � ��������� ��������
//
//  �������� �������� - ������ ������� ����� �� ������ "�������" �����.
//      ��� ��� ��� ���� ��������� � ��� ������,
//      �� ���������� ��� ��������� � �� ������.
//
static int CheckNum=0;  // ���������� ������ �� ������ �������
static WinObject*       //
       CheckFirst=NULL; // ������ ������� � ������ ��������
                        //
static void DeleteLayerChecks()
{ for( ; CheckFirst && CheckNum>0; CheckNum-- )
    { WinObject *W=CheckFirst->Previous(); delete CheckFirst; CheckFirst=W;
    } CheckFirst=NULL;
      CheckNum=0;
  if( But_LD ){ delete But_LD; But_LD=NULL; }
  if( But_LT ){ delete But_LT; But_LT=NULL; } Win_Layers->Draw(); // ���������
}
//      ����������� ���� ��������� � ��� ���������� ���� ���� ������
//      ��������� ������ ������� � ���������� � ������ "�������" �����
//
static void InitMetaLayers( const unsigned Type )
{ if( Type&TxtBrowse )return;           //
    else DeleteLayerChecks();           // �������� �� ����������� �������
 WinObject *Z; LayersType=Type;         //
 LayerList &M=(Type==PicLayers) ? ActiveMap->mapLL
                                : ActiveMap->txtLL;
 viewporttype &W=Win_Layers->Win,
             Bt={ W.left+6,W.top+30,W.left+26,W.top+50,1 },
             BF={ PcX(70)+11,PcY(34)+7,0,0,1 };
  But_LD=new BitButton( BF,__MapLayerBtn,idSelMapLayer ); BF.left=But_LD->scr.right+Tv.Th-6;
  But_LT=new BitButton( BF,__TxtLayerBtn,idSelObjLayer );
  if( Type==PicLayers ){ But_LD->State=true; But_LD->Draw(); } else
                       { But_LT->State=true; But_LT->Draw(); }
 unsigned Key=0;
 ItemsList *L=M.L_First;
  for( ; L; L=L->L_Next,Key++ )
  { Z = new CheckButton( Bt,__CheckBtn,__UncheckBtn,
                        !L->szName?string( "---" ):L->szName,
                         Key|Type, L->iActive );
//                       L->Key|Type, L->iActive );
    if( !CheckNum )CheckFirst=Z; ++CheckNum;
    if( (Bt.bottom+=22)>=W.bottom )break;
         Bt.top+=22;
  }
}
#include "Sakh_Map.cpp"
//
///   ���������� ������������ ����������� �� ������ ���
///    1) ��� ��������� �������� ������������� ���������� ���������� ����,
///       ������� ��� ��������� ����������� ������ ������������ �� �����.
///    2) ����������� ����� ��������� �� ����������� ������,
///       ����� ��������� � �������� (���� ��� �������� ��� �����������)
//
void MapView::SetView( bool W )
{ F=W?MVi->_F:MVi->F; Map_Tv_place( F,Win,MVi->MapType ); // ��� � Sakh_Drawing
}
void MapView::Draw()                    //
{   PanelWin::Draw();                   // ������ ��������� ������������ ����
//if( MVi->MapType )
   if( this==PictMap )
    if( MVi->Small_Image )
      { Tv_PutImage( Win.left,Win.top,MVi->Small_Image ); return;
      }
  SetView( this!=PictMap );
//if( this==FullMap )
  if( MVi->MapType )Read_GRD( MVi->DstName,F ); /// ���� ��� ������������
                    MVi->Map_Show();            /// ��������� � ��������� �����
//if( MVi->MapType )
   if( this==PictMap )
    if( !MVi->Small_Image )
     MVi->Small_Image=Tv_GetImage( Win.left,Win.top,Win.right,Win.bottom );
}
/**
 *   << ������� ���������, ����������� ��� ���������� ������� >>
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 **/
const char *Title="����������� ����� � ������ ���������� �� ���������� ���������� �����";
static void Initial_Files_and_Windows()
{ int i;
 char *MainName=(char*)"Sakhalin.dc2";
  for( i=0; i<int( sizeof( HelpStrings )/sizeof( *HelpStrings ) )-1; i++ )
    { if( i==12 )i=sizeof( HelpStrings )/sizeof( *HelpStrings )-5;
      printf( "%s\n",W2D( HelpStrings[i] ) );
    }                      //
  Tv_Graphics_Start();     // ���������� ������� ������ � ��������� ��������
  setwindowtitle( Title ); //
// ( "����������� ����� � ������ ���������� �� ���������� ���������� �����" );
  Tv_Start_Mouse();
 const int WType=ptInnerLower|ptOuterRaise;
 static viewporttype                                  //
     CF={       0,Tv.mY-Tv.Th-12,Tv.mX,Tv.mY   ,1 },  // �����������
//   FF={       0,Tv.Th+3,    PcX(70),PcY(80)  ,1 },  //
//   PF={ PcX(70)+1,Tv.Th+3,  Tv.mX,  PcY(36)-1,1 },  //
     FF={       0,0,          PcX(70),PcY(80)  ,1 },  // ��� �����
     PF={ PcX(70)+1,0,        Tv.mX,  PcY(34)-1,1 },  // ��� ������
     TF={         0,PcY(80)+1,PcX(70),Tv.mY-Tv.Th-11,1 },
     LF={ PcX(70)+1,PcY(34),  Tv.mX,  Tv.mY-Tv.Th-11,1 };
//   EF={ Tv.mX-Tv.Tw-8,0,0,0 };
//   pattern( SOLID_FILL,BLUE ); bar( 0,0,Tv.mX+1,Tv.Th+2 );
//   color( YELLOW );     Tv.Text( South,Tv.mX/2,0,"������� � ������" );
//   color( BLACK  );        line( 1,Tv.Th+2,Tv.mX+1,Tv.Th+2 );

  Status_Line=new Comment_String ( CF,3, 0,53, 54,8, 63,15 );
  FullMap    =new MapView ( FF,__FullMap ); //,2,WType,1,1,WHITE );
  PictMap    =new MapView ( PF,__PictMap ); //,2,WType,1,1,WHITE );
  Win_Layers =new PanelWin( LF,__WinList,0,2,WType,1 );
  Win_Texts  =new PanelWin( TF,__WinText,0,2,WType,1 );
  //          new BitButton( EF,"�.",idExit );          //  ^- ������� �����
  //
  Tv_SetCursor( 4*Tv.mX/5,Tv.mY/3 );
  for( i=1; i<_ac_; i++ )
   if( *_av_[i]!='-' && *_av_[i]!='/' ){ MainName=_av_[i]; break; }
  if( _ac_>1 )MainName=_av_[1];
  StartMap.DstName=MainName;
  StartMap.Read(); // strdup( MainName ) ); // ���������� ������ �����
  if( !(StartMap.mapLL.nL) )exit( 1 );      // �������� ���������� �������
  StartMap.Title=(char*)""; //"\"Sakhalin\" - Geography System";
  { static char* Comm[]=
    { (char*)"         Russian Geographical Society",
      (char*)"           Sakhalin State University",
      (char*)"Coputational Fluid Mechanics and Marine Researches lab.",(char*)"",MainName,0
    }; StartMap.Comment=Comm;
  }
  StartMap.Display();                      // � ���������� ���������
  RefMap=&StartMap;
  ObjectInfo();
}
static void ReRead_for_All_Pictures( Map &Mc )
{ if( Mc.MapType )
  for( ItemsList *L=Mc.txtLL.L_First; L; L=L->L_Next )if( L->iActive )
  { for( Item *M=L->First; M; M=M->Next )
    { Map *T=(Map*)M;
      if( T )T->Find(); //if( T->DstName )Map::Find( T,0 );
  } }
}                               //
Course WinObject::Action()      // ��� ������ ����������� Getc, ���������
{       Ctrl_Line( 1 );         // ����������, ������ ��� ������� �����
 Course Key=Tv_getc();          // ���������� ������������� �������� �������
        Ctrl_Line( 0 );         //
  if( Key==_MouseMoved )return _MouseMoved;
  if( Key==_MousePressed )
  { for( WinObject *W=_First; W; W=W->_Next )
     if( W->_Tm )
      if( W->MouseThere() )return Course( W->_Id_ );
  } return Key;
}
///    �������� ��������� ��� ���������� ����� ������ ����������
///         ������������ �� ������ (��� ���� ��������)
 //      _______________________________________________
 //      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static bool Map_Interpreter( int& Key )
{
  if( PictMap->MouseThere() )PictMap->SetView( 0 );
                       else  FullMap->SetView( 1 );
  switch( Key )
  { case _MouseMoved: PutLocation(); break; // ����������� ��������� �� ������
    case _Esc     :if( Ctrl_Del( ) ) break;
    case idPrevMap:if( ActiveMap )
                   if( ActiveMap->Previous )
                     { ActiveMap->Previous->Display(); RefMap=0; } break;
    case idMainMap: StartMap.Display(); RefMap=0; break;
    case __PictMap:if( Ctrl_Add() )break;
                   if( Tv_MouseState==_MouseRightPress )
                     { if( ZoomOnFull( __PictMap ) )RefMap=0; break;
                     }     ZoomOnSmall();           RefMap=0; break;
    case __FullMap:if( Ctrl_Add() )break;
                   if( Tv_MouseState==_MouseRightPress )
                     if( ZoomOnFull( __FullMap ) )RefMap=0;
                   if( Tv_MouseState&_MouseLeftPress )
    case _Enter:     { if( RefMap )RefMap->Find();
                     } else
                   if( Tv_MouseState&_MouseLeftDblClick )
    case _Space:   if( RefMap )RefMap->TextWindowView(); break;
    case idHtml:   system( StartHtml ); break;
    case idPDKabs: PdkModeView^=true;      // ������������ ������ ��� - �������
                   ActiveMap->_F.Ly=0;     // ������ ������ ���������������
                   FullMap->Draw();
//                 PictMap->Draw();
                   PutLocation(); break;             //
    case idNoZoom: XOR_small();                      // �������
                   FullMap->MVi->_F=FullMap->MVi->F; // � ��������� ��������
                   FullMap->Draw();                  //
                   XOR_small(); break;               //
    case 0xF1: Sakhalin_Help(); break;               // ������ ���� ��������
    case 0xF3: RefMap=0;                             // � ������������ ��
//       if( Tv_getk()&SHIFT )                       // ��������� �����������
//       for( Map *M=&Map::FirstMap; M; M=M->Next )
//       { if( M->MapType )ReRead_for_All_Pictures( *M );
//       }  else
         ReRead_for_All_Pictures( *ActiveMap );
         Restore_Small_Picture(); break;
    case idExit: Key=0xFA; break;
   default: return true;
  }
  if( Ctrl )Status_Line->Print
           ( 1,"%5.1f%s",Ctrl_Distance(),ActiveMap->MapType?"��":"��" );
  return false;
}
//
//  ������� �������� ������� �� ������� ��� ����������� ������ ��������� ������
//
static char* GetItemText( int n ){ return ActiveMap->GetActive( n )->Title; }
//
//  ������ ������ �������� � ���������� ����� � ������ �� �������
//
static void BrowseObjects( int &_KeyExt )
{
 int Key; static int k=0; Map *D=NULL,*T=NULL;
  for(;;)
  { LayerList &MT=ActiveMap->txtLL; DeleteLayerChecks();
    long BrowseNum=MT.Number_of_Objects();
    ScrollList SL( Win_Layers->Win,BrowseNum,GetItemText );
               SL.SetCurrent( k );
    if( RefMap )RefMap=0;
    do
    { if( !BrowseNum )Key=SL.Run(); else
        { T=ActiveMap->GetActive( k=SL.GetCurrent() );
          if( RefMap!=T ){ RefMap=T; ObjectInfo(); RefMap=0; }
          if( !FullMap->MouseThere() )
            { CrossAt( T ); Key=SL.Run(); CrossAt( T ); } else
            {               Key=SL.Run();               }
        }
      if( Key!=_MouseMoved )D=RefMap,RefMap=T; Map_Interpreter( Key );
      if( Key!=_MouseMoved )RefMap=D; else
      if( FullMap->MouseThere() )
        { if( RefMap  )
          { for( int i=0; i<BrowseNum; i++ )
            if( ActiveMap->GetActive( i )==RefMap ) // ��������� �����
              { SL.SetCurrent( k=i ); break; }      // - � ������������
          }
        }
      switch( Key )
      { case idObjBrowse:
        case 0xF7:
        case 0xFA: goto Exit;
      }
    } while( MT.L_First==ActiveMap->txtLL.L_First );
  }
Exit: InitMetaLayers( LayersType&=~TxtBrowse ); _KeyExt=Key;
}
int main()
{ //
 ///    ���������� � ������ ��������� ���������� ������ � ���������� ��������
  //    ______________________________________________________________________
  //    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 int Key;
  Tfile_Filter=(char*)"������� ���� � ������� Sakhalin (*.dc1)\0*.dc1\0"
                  "����� � ������� DesignCAD (*.dc2,dw2)\0*.dc2;*.dw2\0"
                  "������ ����� (*.*)\0*.*\0";
  Initial_Files_and_Windows();
  do
  { if( Map_Interpreter( Key=WinObject::Action() ) )
    switch( Key )
    { case idSelMapLayer:
           if( LayersType!=PicLayers )InitMetaLayers( PicLayers ); break;
      case idSelObjLayer:
           if( LayersType!=TxtLayers )InitMetaLayers( TxtLayers ); break;
      case 0xF7:                               //
      case idObjBrowse: LayersType|=TxtBrowse; // ������� � ������ ��������
           if( ActiveMap->txtLL.Number_of_Objects()>1 )BrowseObjects( Key );
           break;
     default:
      if( Key & (PicLayers|TxtLayers) )         // �������� ����� �����
      { if( Key&PicLayers )
          { ActiveMap->mapLL.SwitchLayerState( (~PicLayers) & Key );
            if( !ActiveMap->MapType )ActiveMap->_F.Ly=0.0;
            FullMap->Draw();
          } else
        if( Key&TxtLayers )
            ActiveMap->txtLL.SwitchLayerState( (~TxtLayers) & Key );
            RefMap=0;
        ((CheckButton*)WinObject::FindNew( Key ))->Press();
    } }
  } while( Key!=0xFA );
}

/**    ����������� ���������
 ++    ������ ��������� ��� ��������� ������������� ������
 ++    ������ � ������ �� ��������, ������ ���� �������������
 --    ��� ������ F7 ����� ����� F1 - ��������� ��������� ������
 --    ���� ��������� ���� "�������" �������� ��� �����-��������
**/
