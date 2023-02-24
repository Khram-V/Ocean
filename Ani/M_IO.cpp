/*
        Mario.IO
        Программы прямого ввода данных

    Новый формат
     Mario\xFA+short Nm - Количество мореографных рядов в short формате
     | Mar\xFA + int Nm --- (0)-независимая последовательность
        long   Jd - Юлианская дата и
        double Tm -  время начала графического изображения на экране ЭВМ
        double dT - Шаг дискретизации в часах
        double Tn - Длительность изображаемого процесса
        fixed  Vv - Битовая маска настройки параметров изображения
        byte   Pw - Степень переинтерполяции графиков/перезаписи (0)-сплайн-функция
        byte  Key - контрольный байт, должен быть равен строго 123
        long   N   - длина ряда или (-1) для независимой последовательности
    Итого: общий заголовок составляет ровно 80 байт.
        string Ident     - 20-ограниченная строка с названием числового ряда
        long   Jday      - Юлианская дата
        double  To       -  и время начала процесса
        double  dT       - дискретность
        double Lon,Lat   - географические координаты в градусах
        float  Depth    - глубина моря
        float  Bearing  - направление
        float  Distance -  и дистанция до берега
                         -- итого 80 байт
              (0.0) - к каждой величине будет добавляться значение аргумента: F(x)={F,x}

     !! ввести ряды с неравномерным шагом
     !! - координатные ряды
     !! - ряды скорости и направления
     !! для формата Ani надо добавить глубину моря, расстояние и направление до берега,
        и все это связать в тройки чисел - уровень моря, скорости течений и их направления
     !! ввести связные рады для представления скорости течений, и трехмерной скорости

*/
#include "Mario.h"
//#include "..\Matrix\XML.h"
//
//    Вариант Mario№          Вариант Mario°
//      short Nm                Количество мореограмм в файле
//      char  Ident[20]         Текстовый идентификатор записи
//      float Lon,Lat           Географические координаты в градусах
//      fixed Year
//      byte  Day,Mon           long  Jday
//            mn,hr,hn,sc       float Time
//      float dt    (min)       float dt (hour)
//      S[Nm].N      длина всех рядов соответствует длине файла
//
//    Вариант Mario¤
//      long  Jd - Юлианская дата и
//      float Tm - Время начала изображения
//      float dT - Шаг дискретизации в часах
//      float Tn - Длительность изображаемого процесса
//      byte  Pw - Степень интерполяции на шаг изображения
//      byte ... - резерв
//      fixed Vv - Битовая маска изображения
//      г short S[].N   - длина ряда
//      ¦ char  Ident[20] Текстовый идентификатор записи
//      ¦ float Lon,Lat - Географические координаты в градусах
//  44\г- long  Jday    - Юлианская дата и
//  42/L¬ float To      - Время начала процесса (час)
//      ¦ float dt      - дискретность          (час)
//      ¦ byte  Fmt     - 1 byte;   2 int; else float
//      L byte  Typ     - 1 Series; 2 Curve; 3 Tide...
//      Далее:
//      Typ=1(S)
//          Fmt=0   - поток данных в плавающем формате
//             =2,1 - Числа float: Min & Mult, затем поток
//                      данных в байтовом или целом формате,
//                 Dat= Min + Mult*File.
//      Typ=3(T) - специальный формат для приливного ряда
//          Fmt=2,1 - int,byte
//                      и дополнительный заголовок:
//              int Min Минимум ряда
//              int Mid Средний многолетний уровень моря
//              int [2] резерв из 2-х слов
//                      далее поток данных в соответствии с Fmt.
//                      пропуски отмечаются в файле как 0xFF,
//                      что отмечается в данных признаком Min-1.
//               Tide = ( Min+File )/100.
//    _ac_ и _av_ после первого считывания должны обнуляться
// ~~~~~~~~~~~~~~~~~~~~ //
struct Mario_Old        //
{      char  Ident[20]; //+¬ Название
       float Lon,Lat;   //|¦ Географические координаты [deg]
       long  Jday;      // ¦ Юлианская дата
       float To,        // ¦ Начальное время в часах и
             dt;        // -- шаг дискретизации во времени
       byte  Fmt,       // | 42 - 1 byte;   2 int; else float
             Typ;       // L=-  - 1 Series; 2 Curve; 3 Tide...
  void select( Mario& s )
  { memmove( s.Title,Ident,20 ); s.JT.T=To;   s.Latitude=Lat;  s.Fmt=Fmt==4?0:Fmt;
                        s.dt=dt; s.JT.D=Jday; s.Longitude=Lon; s.Typ=Typ;
} };
static const char
            AcEr[]="? Ошибка записи",
            OpEr[]="? Файл не открыт",
            Tt92[]="Mario\xFC", // № Файл блочной структуры данных
            Tt93[]="Mario\xF8", // ° -//- однодневка образов из памяти
            Tmar[]="Mario\xFD", // ¤ Система с линейным хранением данных
            Tt10[]="Mario\xFA", // записи программы Ani - 2010 г.
            Tt16[]="Mar\xFA",   //   -//- программы Ani - 2016 г.
            Tm10[]="Mario\xFB", // плюс новые записи Mario от 12.09.2010 г.
//          Txml[]="<?xml",     // xml - простая текстовая запись данных
            Ttid[]="~Tide\xF8"; // ° Идентификатор приливного файла
static char FName[MAX_PATH]="~Series.mar",
             Msg [MAX_PATH]="";
                       //
static int Ver=0,Lk=1, // Количество пропусков при считывании длинных рядов
                 LR=0; // Признак информационного чтения/записи без данных
                       // 0 - полное считывание файла по обычным правилам
                       // 1 - считывание заголовков
static FILE *Fs=0,     // Файлы исходных и
            *Fz=0;     // выходных данных

static void Title_write( Mario &M ) /// !! простая запись структуры не проходит
{  putw( M.N,Fz );                      // Размерность оперативного массива   4
   fwrite( &(M.Title),1,Mario_Title_Length,Fz ); //+¬ Название               20  24
   fwrite( &(M.Longitude),1,sizeof(double),Fz );  //--                        8
   fwrite( &(M.Latitude),1,sizeof(double),Fz );   // Координаты [deg]         8  40
   fwrite( &(M.JT.D),1,sizeof(long),  Fz );       // ¦ Юлианская дата,        4  44
   fwrite( &(M.JT.T),1,sizeof(double),Fz );       // ¦ время в часах          8  52
   fwrite( &(M.dt),1,sizeof(double),Fz );         // шаг дискретизации [час]  8  60
   putf( M.Depth,Fz );                            //++ глубина моря           4  64
   putf( M.Distance,Fz );                         //++ отстояние берега [км]  4  68
   putf( M.Bearing,Fz );                          //++ пеленг на берег  [°]   4  72
   put2( M.flag,Fz );                             // Флаги операций           2  74
   putc( M.Typ,Fz );     // 1 Series; 2 Curve; 3 Tide...                      1  75
   putc( M.Fmt,Fz );     // 0-float; 1-byte; 2-int; 3-байт; 4-word else float 1  76
   putw( 0,Fz );         // всего лишь только одно слово в резерве            4  80
}

static void Title_read( Mario &M, int N )
{  M.N=N;                                // Размерность оперативного массива  4
   fread( &(M.Title),1,Mario_Title_Length,Fs );   //+¬ Название              20  24
   fread( &(M.Longitude),1,sizeof(double),Fs );   //--                        8
   fread( &(M.Latitude),1,sizeof(double),Fs );    // Координаты [deg]         8  40
   fread( &(M.JT.D),1,sizeof(long),  Fs );        // ¦ Юлианская дата,        4  44
   fread( &(M.JT.T),1,sizeof(double),Fs );        // ¦ время в часах          8  52
   fread( &(M.dt),1,sizeof(double),Fs );          // шаг дискретизации [час]  8  60
   M.Depth   =getf( Fs );                         //++ глубина моря           4  64
   M.Distance=getf( Fs );                         //++ отстояние берега [км]  4  68
   M.Bearing =getf( Fs );                         //++ пеленг на берег  [°]   4  72
   M.flag=get2( Fs );                             // Флаги операций           2  74
   M.Typ=getc( Fs );     // 1 Series; 2 Curve; 3 Tide...                      1  75
   M.Fmt=getc( Fs );     // 0-float; 1-byte; 2-int; 3-байт; 4-word else float 1  76
   N=getw( Fs );         // в резерве должен быть ноль                        4  80
}
static void E_pas( const char F[] )
       { Thelp( "?\"%s\" %s",FName,F ); Tv_bell(); Tv_getc();
       }
static void New_Read( bool newRead, int L, Mario& M )
{ if( Ver==10 )Title_read( M,L ); else
  { Mario_Old MO; fread( &MO,1,42,Fs ); MO.select( M );
  }
  if( newRead )
  { M.N=0;
    M.allocate( L );                    //
    if( M.Typ==3 )                      // Считывание приливной мореограммы
    { Real W;                           //
      int Min=get2( Fs ),i; M.Min=M.Max=Real( Min )/100.0-0.001;
      int Mid=get2( Fs ),j;       M.Mid=Real( Mid )/100.0;
             fseek( Fs,4L,SEEK_CUR );
      for( i=0; i<L; i++ )
      { if( M.Fmt==2 )j=(fixed)get2( Fs );
               else { j= (byte)getc( Fs ); if( j==0xFF )j=-1; }
        M.V[i]=W=Real( (j==-1)?Min-1:j+Min )/100.0;
        if( M.Max<W )M.Max=W;
      }                                 //
    } else                              // Считывание простых временных рядов
    if( M.Fmt>=1 && M.Fmt<=4 )          //
    { int i,w;
      Real Min,Apl,DApl;
           Min=getf( Fs );
      if( M.Fmt>2 )fread( &DApl,1,8,Fs ); else Apl=getf( Fs );
      for( i=0; i<L; i++ )
      switch( M.Fmt )
      { case 1: M.V[i]=(fixed)getc( Fs )*Apl+Min; break;
        case 2: M.V[i]=(fixed)get2( Fs )*Apl+Min; break;
        case 3: w=0; fread( &w,1,3,Fs ); M.V[i]=w*DApl+Min; break;
        case 4: M.V[i]=(unsigned)getw( Fs )*DApl+Min;
      }
    } else for( int i=0; i<L; i++ )M.V[i]=getf( Fs );
  } else
  { unsigned long oL=M.N=L;                            //
    switch( M.Fmt )                           // пустой пробег при построении
    { case 1: oL += 8;        break;          // списков мореограмм
      case 2: oL = oL*2 + 8;  break;          //
      case 3: oL = oL*3 + 12; break;
      case 4: oL = oL*4 + 12; break;
     default: oL*=4;
    } fseek( Fs,oL,SEEK_CUR );
  }
}
void m_Read()
{ long long Lf;
  Tfile_Filter=(char*)"MarioGraphic Data File (*.mar)\0*.mar\0"
                      "Ocean Tidal Data Base  (*.tim)\0*.tim\0"
                      "All Files in Storage   (*.*)\0*.*\0";
  if( _ac_>1 )strcpy( FName,_av_[1] );
       else   strcpy( fname( FName ),"*.mar" );
  if( ( Lf=Tfile( FName )-8 )>40 )
  { if( (Fs=fopen( FName,"rb" ))!=0 )
    {                 fread( Msg,1,4,Fs );
      if(             memcmp( Tt16,Msg,4 )==0 )Ver=3;  else
      {               fread( Msg+4,1,2,Fs );
        if(           memcmp( Tt93,Msg,6 )==0 )Ver=0;  else
        { if(         memcmp( Tt92,Msg,6 )==0 )Ver=1;  else
          { if(       memcmp( Tt10,Msg,6 )==0 )Ver=2;  else
            { if(     memcmp( Tm10,Msg,6 )==0 )Ver=10; else
              { if(   memcmp( Tmar,Msg,6 )==0 )Ver=12; else
//            { if(   memcmp( Txml,Msg,5 )==0 )Ver=64; else
                { if( memcmp( Ttid,Msg,6 )==0 )
                  { fclose( Fs ); Fs=0; _ac_=2; _av_[1]=FName; return;
                  } else { Ver=-1; E_pas( "Wrong file" ); }
      } } } } } }
     const int Max_IO_list=100;
#if 0
//
//  Еще одна разновидность синтаксических упражнений с данными на языке XML
//
      if( Ver==64 )           //
      { char *str;           // заголовок только для контроля
        inXML Xml;           // полагая, что это больше нигде особо не нужно
        fclose( Fs ); Fs=0;  // ~~~~~~~~~~~~~~~ //
        if( (str=Xml.Open( FName ))!=NULL )     // все проверено -
        if( !strcmp( strupr( str ),"MARIO" ) )  // - можно работать
        { Event  T0;       // Исходное время, задаваемое по умолчанию
          double d0=1;     // Общий шаг для всех числовых рядов [час]
          int k,n=0,m=-16, // Номер текущего временного ряда и количество мореограмм
              mk,*id=NULL; // Разности для прямого пересчета номеров считываемых рядов
          bool cD=false,cT=false,cd=false;
          if( (str=Xml.Next())!=NULL )
          if( !strcmp( str,"Titles" ) )
          while( (str=Xml.Next())!=NULL )
          { if( !strcmp( str,"Mario" ) )break; else
            if( !strcmp( str,"Date"  ) )
            { int m,d,y; T0.UnPack( m,d,y ); // на всякий случай ставится текущая дата
              sscanf( Xml.String(),"%d %d %d",&y,&m,&d );
              if( !cD ){ T0.Pack( m,d,y ); cD=true; } // общая для всех рядов дата
              if( n>0 )Ms[n-1].JT.Pack( m,d,y );
            } else
            if( !strcmp( str,"Time"  ) )
            { float t; AtoD( Xml.Word(),t );
              if( !cT ){ T0.T=t; cT=true; }
              if( n>0 )Ms[n-1].JT.T=t;
            } else
            if( !strcmp( str,"dT"  ) )
            { double dt=1; sscanf( Xml.Word(),"%lg",&dt );  dt/=3600.0;
              if( !cd ){ d0=dt; cd=true; }
              if( n>0 )Ms[n-1].dt=dt;
            } else                    // Здесь проводится распределение памяти
            if( !strcmp( str,"M"  ) ) // и надо бы еще поставить проверку на повторы
            { sscanf( Xml.Word(),"%d",&k );
              if( m<=n )
              { Ms.allocate( m=n+256 );
                id=(int*)Allocate( m*sizeof(int),id );
              } id[n]=k;
              Ms[n].Init();       // установка начальных параметров
              strcut( str=Xml.String() );
              strncpy( Ms[n].Title,W2D( str ),Mario_Title_Length );
              Ms[n].JT=T0;        // Предполагается, что координаты и
              Ms[n].dt=d0; ++n;   // другие описатели глубины и дистанции
            } else                // связываются только с конкретными
            if( !strcmp( str,"Place" ) ) // мореограммами
            { if( n>0 )sscanf( Xml.String(),"%lg %lg",&(Ms[n-1].Latitude),&(Ms[n-1].Longitude) );
            } else
            if( !strcmp( str,"Depth"  ) )
            { if( n>0 )sscanf( Xml.Word(),"%lg",&(Ms[n-1].Depth) );
            } else
            if( !strcmp( str,"Coast"  ) )
            { if( n>0 )sscanf( Xml.String(),"%lg %lg",&(Ms[n-1].Bearing),&(Ms[n-1].Distance) );
            }
          }
//
//      Завершено считывание информационного блока
//
          if( !strcmp( Xml.Word(),"fixed" ) )
          if( n>0 )                // какие-то данные в файле всяко долждны быть
          { Ms.allocate( n ); n=0; // пока же пробуем читать все подряд
            mk=-1;                 // признак нового блока
            while( (str=Xml.Next())!=NULL )               // перебор до конца файла
            if( !strcmp( str,"m"  ) )                     // опознавание точки "m"
            { sscanf( Xml.Word(),"%d",&m );               // считывание индекса ряда
              for( k=0; k<Nm; k++ )if( m==id[k] )         // цикл с поиском индекса
              { if( mk>=0 && k<=mk )n++; mk=k;            // переход к новому блоку
                if( Ms[k].N<=n )Ms[k].allocate( n+1000 ); // распределение памяти
                sscanf( Xml.Word(),"%lg",&(Ms[k][n]) );   // прямая выборка числа
                break;                                    // и выход из поиска
              }                                           //
            } for( k=0; k<Nm; k++ )Ms[k].allocate( n+1 ); // чтение закончено, расчистка,
          }   if( id )id=(int*)Allocate( 0,id );          // и список id больше не нужен
//
//      По завершении считывания данных
//      необходима настройка параметров визуализации
//      и уточнение основных экстремальных значений и средних величин
//        if( !kt ){ Tm=Ms[0].JT; dT=Ms[0].dt; Tn=Ms[0].dt*Ms[0].N; }
                   { Tm=T0; dT=d0; Tn=Ms[0].dt*Ms[0].N; }
//        m_ReView( 1 );
        }
      } else
#endif
//
//      Блок считывания данных в старом формате ANI
//       Mario FileName.mar /beg:len m1 m2 ... mN
//         /beg:len - starts & length of series
//          m1...mN - numbers of reading series
//
      if( Ver>=0 && Ver<=3 )       // блокированные форматы из моделирования
      { int i,k,l=0,b=0,kt,N,      // N  - Количество рядов в файле
            Ld,m=0,s[Max_IO_list]; // m  - количество рядов во входном списке
        Lk=1;                                 // Lk - шаг считывания
        if( Ver==3 ){ Ver=2; N=getw( Fs ); }  // N  - Количество рядов в файле
        else N=(fixed)get2( Fs );             // Ld - Длина ряда в файле
         Ld=Lf/N/4-(Ver<2?10:15);
        if( _ac_>2 )
        { for( i=2; i<_ac_; i++ ){ char *c=_av_[i];
           if( c[0]=='/' ){          sscanf( c+1,"%d",&b );
             if((c=strchr(c,':'))!=0)sscanf( c+1,"%d",&l );
           }
           else if( m<Max_IO_list ){ sscanf( c, "%d", &kt );
                if( kt>0 && kt<=N ){ s[m]=kt; m++; }
          } }
          if( !l )l=Ld-b; if( Ld>b+l )Ld=b+l;
        }            kt=Nm;
        Ms.allocate( Nm+(m?m:N) );
        if( Ver==2  )
        for( i=k=0; k<N; k++ )
        { char PointName[Mario_Title_Length]; long JulDay; double Din[2]; float Fin[5];
            fread( &PointName,1,Mario_Title_Length,Fs ); // последовательное
            fread( &JulDay,1,4,Fs );                     // считывание
            fread( &Din,2,8,Fs );                        // всех заголовков
            fread( &Fin,5,4,Fs );
          if( !m )i=k; else for( i=m-1; i>=0; i-- )if( s[i]-1==k )break;
          if( i>=0 )
          { Mario &S=Ms[kt+i];
            memcpy( S.Title,PointName,Mario_Title_Length );
            S.JT.D=JulDay;              // Юлианская дата
            S.JT.T      = Din[0];       // и начальное время нового ряда
            S.dt        = Din[1]*Lk;    // расчетный шаг
            S.Longitude = Fin[0];       S.Fmt = 0;
            S.Latitude  = Fin[1];       S.Typ = 1;
            S.Depth     = Fin[2];       // глубина моря
            S.Bearing   = Fin[3];       // направление
            S.Distance  = Fin[4];       // и дистанция до берега
            if( LR )S.N=Ld-b; else      //
                    S.allocate( Ld-b ); S.JT.T+=S.dt*b;
          }
        }
//
//      эти форматы уже никем не создаются, но могут быть ранее сохраненными
//
        else
        for( i=k=0; k<N; k++ )
        { Mario_Old MO; char* MS=(char*)(&MO); fread( &MO,1,40,Fs );
                    MO.Fmt=0;
                    MO.Typ=1;           //
          if( Ver==1 )                  // Выборка даты и времени
          { long &Jd= ((long*)MS)[7];   // из DOS-kого формата
           float &Ti=((float*)MS)[8];   //
            Jd=julday( MS[31],MS[30],((fixed*)(&Jd))[0] );
            Ti=float(MS[33])    + float(MS[32])/60.0
             + float(MS[35])/3600.0+float(MS[34])/360000.0;
           ((float*)(MS+36))[0]/=60.0;
          }
          if( Lk>1 )((float*)(MS+36))[0]*=float( Lk );
          if( !m )i=k; else for( i=m-1; i>=0; i-- )if( s[i]-1==k )break;
          if( i>=0 )
          { Mario &S=Ms[kt+i]; MO.select( S );
            if( LR )S.N=Ld-b; else
                    S.allocate( Ld-b ); S.JT.T+=S.dt*b;
          }
        }
//
//      после заголовков делается выборка собственно данных
//
        if( !kt ){ Tm=Ms[0].JT; dT=Ms[0].dt; Tn=Ms[0].dt*Ms[0].N; }
        if( LR<=0 )               //
        { for( l=0; l<Ld; l++ )   // Первое считывание данных в F3->Enter
          { for( k=0; k<N; k++  ) //
            { Real W=getf( Fs );
              if( l>=b )                       // if( ((l-b)%Lk)==0 )
              { //if( W==0xFFC00000L )W=0.0;   // Non Number
                if( !m )Ms[Nm-N+k].V[l-b]=W; else
                 for( i=0; i<m; i++ )
                  if( s[i]-1==k )Ms[Nm-m+i].V[l-b]=W;
            } }
            if( Lk>1 )fseek( Fs,unsigned( ( Lk-1 )*N )<<2,SEEK_CUR );
          } m_ReView( 1 );
        }
      } else
//
//    Новый (устаревший) формат записи для глобальной системы обработки данных
//
      if( Ver==12 || Ver==10 )
      { if( Nm==0 )            //
        { Tm.D=getw( Fs );     // Юлианская дата и
          Tm.T=getf( Fs );     // Время начала изображения
            dT=getf( Fs );     // Шаг дискретизации в часах
            Tn=getf( Fs );     // Протяженность изображения
            Pw=get2( Fs );     // Степень интерполяции
            Vv=get2( Fs );     // Битовая маска изображения
        } else                 //
        fseek( Fs,26L,SEEK_SET );
        for( ;; )
        { int L;
          if( Ver==10 )L=getw( Fs ); else L=get2( Fs );
          if( feof( Fs ) )break;
          Ms.allocate( Nm+1 );
          New_Read( LR<=0,L,Ms[Nm-1] );
        } if( !LR )m_View();
      }
      if( !LR || Ver<0 )
      if( Fs ){ fclose( Fs ); Fs=0; }
    } else E_pas( OpEr );
  } _ac_=0;
}
//
//      Организация записи данных в новых форматах
//
static void New_Write( Mario& M )
{
 int i,j;
 Real W,MX,MN;
  Title_write( M );                   // 80-байт описаний с 4-байтовым резервом
  //
  //    Смещение записи приливов для уменьшения динамического диапазона
  //
  if( M.Typ==3 )                        // Вариант записи приливного файла
  { MX=MN=M.Min*100;                    //  в обнинском формате
    for( i=j=0; i<M.N; i++ )            // Вычисление новых
    if( (W=( M.V[i]-M.Min )*100)!=0.0 ) // экстремумов для
    { if( j )                           // приливного ряда
      { if( MX<W )MX=W; else            //
        if( MN>W )MN=W;
      } else
      if( M.V[i]>M.Min ){ MX=MN=W; j=TRUE; }
    }
   int Min,Max; Max=int( MX+0.5 );              //
                Min=int( MN<0?MN-0.5:MN+0.5 );  // Запись данных с учетом
    if( Max-Min>254 )M.Fmt=2; else M.Fmt=1;     // новых экстремумов
      put2( Min,Fz );
      put2( int( ( M.Mid*100 )+( M.Mid<0?-0.5:0.5 ) ),Fz );
      put2( 0,Fz );
      put2( 0,Fz );
    for( i=0; i<M.N; i++ )
    { if( M.V[i]<M.Min )j=0xFFFF; else j=int( ( M.V[i]*100-Min )+0.5 );
      if( M.Fmt==2 )put2( fixed( j ),Fz );
              else  putc( byte( j ),Fz );
  } } else
  //
  //    Дискретная упаковка при записи обычных рядов
  //
  if( !M.Fmt )for( i=0; i<M.N; i++ )putf( M.V[i],Fz );                          // fwrite( M.V,4,M.N,Fz )
  else
  { MinMax( M.V,M.N,MN,MX ); putf( MN,Fz );                                     // fwrite( &MN,1,4,Fz )
    switch( M.Fmt )
    { case 1:{ W=(MX-MN)/254.0; putf( W,Fz );                                   // fwrite( &W,1,4,Fz );
               for( i=0; i<M.N; i++ )putc( (fixed)((M.V[i]-MN)/W+0.5),Fz );
             } break;
      case 2:{ W=(MX-MN)/65534.0; putf( W,Fz );                                 // fwrite( &W,1,4,Fz );
               for( i=0; i<M.N; i++ )put2( (fixed)((M.V[i]-MN)/W+0.5),Fz );
             } break;
      case 3:{ double DW=double(MX-MN)/16777214.0; fwrite( &DW,1,8,Fz );
               for( i=0; i<M.N; i++ )
               { j=(M.V[i]-MN)/DW+0.5; fwrite( &j,1,3,Fz ); }
             } break;
      case 4:{ double DW=double(MX-MN)/4294967294.0; fwrite( &DW,1,8,Fz );
               for( i=0; i<M.N; i++ )putw( (unsigned)((M.V[i]-MN)/DW+0.5),Fz );
             }
    }
  }
}
#define Sl if( Nb ){ while( ((Ms[k]).flag&1)==0 && k<Nm )k++; if( k>=Nm )break; }

void m_Write( int Asc )
{
 long long Lf=0; int i,k,c=color( CYAN );
//
//      Блок интерактивного открытия файла
//
  Ghelp( "?Enter the File Name" );
  Tgo( 0,-1 );
  if( Wgets( fext( FName,Asc==1?"dat":"mar" ),-60 )!=_Enter )return;
  for( k=0; FName[k]; k++ )if( FName[k]=='.' ){ k=-1; break; }
  if( k>=0 )fext( FName,Asc==1?"dat":"mar" );
  if( ( Fz=fopen( FName,"r+b" ) )!=0 )
  { fread( Msg,1,6,Fz );
    i=memcmp( Tm10,Msg,6 );          //fseek( Fz,0L,SEEK_END ); Lf=ftell( Fz );
    Lf=Tfile( Fz );
    for( k=strlen(FName); k>0 && FName[k-1]!='\\'; k-- );
  F_ans: Thelp
    ( "?%s[%ld] File Exist? <Enter>New%s",FName+k,Lf,!i?" <Tab>Add":"" );
    switch( Tv_getc() )
    { case _Esc  : fclose( Fz ); Fz=0; return;
      case _Enter: fclose( Fz ); Fz=0; Lf=0; i=0;
                  rename( FName,fext( strcpy( Msg,FName ),"bak" ) );
      case _Tab: if( !i )break;
     default : goto F_ans;
    }
  } if( !Fz )Fz=fopen( FName,Asc==1?"wt":"wb" );
  //
  //     Организация записи данных на диск
  //
  if( Fz )
  { //Real w;                       //
    Event t,z=Tm+(Tn-dT/2);        // Запись в забытом формате z-"Мезозавр"
    if( Asc==1 )                   //
    { char Name[22]="";            // (что-то тут сложновасто с табуляторами)
      for( k=0; k<Nm; k++ )        //
      if( !Nb || (Ms[k].flag)&1 )
      { if( Name[0] )fprintf( Fz,"\t" ); else Name[0]='\"';
        strcut( strcpy( Name+1,Ms[k].Title ) );
        fprintf( Fz,"%s\"",Name ); //
      } fprintf( Fz,"\n" );        // запись интерполированных величин
      for( t=Tm; t<z; t+=dT )      // по отмеченным флажкам
      { Name[0]=0;                 //
        for( k=0; k<Nm; k++ )
        { if( !Nb || (Ms[k].flag)&1 )
          { if( Name[0] )fprintf( Fz,"\t" ); else Name[0]=1;
            fprintf( Fz,"%.6lg",(Ms[k])( t ) );
          } if( ferror( Fz ) ){ E_pas( AcEr ); break; }
        }   fprintf( Fz,"\n" );
    } }                           //
    else                          // Формат записи -x- блоками по Nm
    if( Asc==0 )                  // плавающих чисел программы "Ani"
    { fwrite( Tt16,1,4,Fz );      // -- Variant № 3 - 2016 год
      putw( Nb?Nb:Nm,Fz );        // скорректированный вариант записи данных
      for( k=0; k<Nm; k++ )       // блоками для быстрой спектральной обработки
      { Sl Mario &M=Ms[k];        //
        double Dout[]={ M.JT.T,M.dt };
        float  Fout[]={ M.Longitude,M.Latitude,M.Depth,M.Bearing,M.Distance };
          fwrite( M.Title,1,20,Fz );
          fwrite( &M.JT.D,1,4, Fz );
          fwrite( Dout,   2,8, Fz );
          fwrite( Fout,   5,4, Fz );
      }
      for( t=Tm; t<z; t+=dT )
      { for( k=0; k<Nm; k++ ){ Sl putf( Ms[k](t),Fz ); }                        // fwrite( &w,1,4,Fz ); }
        if( ferror( Fz ) ){ E_pas( AcEr ); break; }
      }
    } else                              //
    { if( !Lf ){ fwrite( Tm10,1,6,Fz ); // Mario V - старенькое
                 putw( Tm.D,Fz );       //+4 Юлианская дата и
                 putf( Tm.T,Fz );       //+4 Время начала изображения
                 putf( dT,Fz );         //+4 Шаг дискретизации в часах
                 putf( Tn,Fz );         //+4 Протяженность изображения
                 put2( Pw,Fz );         //+2 Степень интерполяции
                 put2( Vv,Fz );         //+2 Битовая маска изображения
      }                                 //
      for( k=0; k<Nm; k++ )
      { if( !LR ){ Sl }else
        { while( Ms[k].flag&0x10 && k<Nm )k++; if( k==Nm )break;
        } New_Write( Ms[k] );
      }
    } if( !LR )fclose( Fz );
  } else E_pas( OpEr ); /* Abn: */ Ghelp(); color( c );
}
//      Mario.List
//      Процедура обработки данных в виде списка
//
static int   Lf,        // Реальное количество записей в файле
             Sf=0;      // Количество отмеченных записей
static char* IName;     // Имя активного входного файла
                        //
static char* LName( int k )
{ if( k>Lf )--k; else if( k==Lf ){ color( YELLOW ); return IName; }
 Mario &M=Ms[k];
 int m,d,y;
  color( M.flag&0x10?(k<Lf?DARKGRAY:CYAN):(k<Lf?LIGHTGRAY:LIGHTCYAN) );
  sprintf( scpy( Msg+1,M.Title,20 ),"%5d.",M.N );
  Msg[0]=M.flag&0x10?'-':' ';
  Msg[26]=k<Lf?'.':'-';
  Msg[27]=M.Fmt==1?'b': M.Fmt==2?'i':'f'; M.JT.UnPack( m,d,y );
  Msg[28]=M.Typ==1?'S': M.Typ==3?'T':'?';
  sprintf( Msg+29,"%5u.%02u.%02u%7s ё",y,m,d,DtoH( M.JT.T,2 ) );
     scat( Msg,DtoH( M.dt,-3 ) );
 return Msg;
}
void m_List()
{
 static field Tf={ 0,0,1,16,0 }; static int Kf=0;
 int Tv=1,ans,k;        //
 char iName[MAX_PATH];  //
      Lf=Nm;            // Статический адрес текущего списка
  if( Kf>Lf )Kf=Lf-1;   //
  do
  { if( !Nm )ans=0xF3; else
    { int L=Nm;
      Ghelp( "{%d+%d}  F1 Info",Lf,Sf>0?Sf:L-Lf );
      color( LIGHTGRAY );
      if( L>Lf ){ L++; if( Kf>=Lf )++Kf; }
      ans=Tcase( Tf,57,LName,L,Kf,Tv ); Tv=-1;
      if( Kf>Lf )--Kf;
    }
  Rep:switch( ans )
    { case 0xF4: Ms[Kf].Initial(); break;
      case 0xF1:
      { unsigned Lm=coreleft()>>20;
        color( CYAN ); Twindow( 1,0,30,15 ); Tln(); color( BLUE );
        Tprintf( W2D( " Список заголовков\n" ) ); Tln(); color( DARKGRAY );
        Tprintf( W2D( "F1 - эта подсказка\n" ) );
        Tprintf( W2D( "F2 - сохранить в новом файле\n"  ) );
        Tprintf( W2D( "F3 - добавить данные из файла\n" ) );
        Tprintf( W2D( "F4 - параметры текущего ряда\n"  ) ); Tln(); color( GREEN );
        Tprintf( W2D( "<Ins> Отметить активным\n"       ) );
        Tprintf( W2D( "<Del> Метка на удаление\n"       ) );
        Tprintf( W2D( "<BkSp>  Закрыть список/файл\n"   ) );
        Tprintf( W2D( "<Enter> Взять отмеченные ряды\n" ) ); Tln(); color( CYAN );
        Tprintf( W2D( " %ld Мб памяти" ),Lm );
        ans=Tv_getc(); Tback();
        if( ans==_Esc ){ ans=0; continue; }goto Rep;
      }
      case _Ins:
      if( Ms[Kf].flag&0x10 || (Kf>=Lf && Sf==0) )
      { if( Kf>=Lf )
        { if( !Sf )for( k=Lf; k<Nm; k++ )Ms[k].flag|=0x10; ++Sf;
          if( Nm-Lf==Sf )Sf=0;
        } Ms[Kf].flag&=~0x10;
      } break;
      case _Del:
      if( (Ms[Kf].flag&0x10)==0 )
      { Ms[Kf].flag|=0x10;
        if( Kf>=Lf )
        { if( !Sf )Sf=Nm-Lf-1; else
          { --Sf; if( !Sf )for( k=Lf; k<Nm; k++ )Ms[k].flag&=~0x10; }
      } } break;        //
      case 0xF2:        // Сохранение рядов в новом
      { bool Rb;        //  и уже очень сильно устаревшем формате
        int N=Nm,j;
        LR=1; Nm=Lf; m_Write( 2 ); Nm=N; LR=0;
        if( Fz )
        { Mario M;
          for( k=Lf; k<Nm; k++ )
          { Mario &S=Ms[k]; Rb=(S.flag&0x10)==0;
            if( Ver==12 || Ver==10)
            { if( k==Lf )fseek( Fs,26L,SEEK_SET );
              if( Ver==10 )N=getw( Fs ); else N=get2( Fs );
              New_Read( Rb,N,M );                //
            } else                               // Очень сильно закручены
            { if( k==Lf )M.allocate( S.N );      // старые форматы + Ani среди
              if( Rb )                           // них - надо проверить !!!
              { fseek( Fs,8L+long(Nm-Lf)*(Ver==2?60:40)+long(k-Lf)*4L,SEEK_SET );
                for( j=0; j<S.N; j++ )
                { M.V[j]=getf( Fs );                                            // fread( M.V+j,1,4,Fs );
                  fseek( Fs,unsigned(Nm-Lf-1)<<2,SEEK_CUR );
            } } }
            if( Rb )
            { Real *V=M.V; memcpy( &M,&S,sizeof( Mario ) ); // раньше было 42.
                    M.V=V;
              if( S.Typ!=3 )
              if( S.Fmt>=1 && S.Fmt<=4 )MinMax( M.V,M.N,M.Min,M.Max );
              New_Write( M );
            }
          } if( M.N>0 )M.free();
            if( Fz ){ fclose( Fz ); Fz=NULL; }
        } break;
      }                          //
      case 0xF3:                 // Чтение нового или
      case _BkSp:                // отказ от прочитанного файла
      { if( Tv<1 ){ Tback(); Tv=1; }
        if( Nm>Lf )Ms.allocate( Lf );
        if( ans==0xF3 )         //
        { if( _ac_>1 )ans=0;    // Если вход при старте программы
          LR=1; m_Read(); LR=0; // Чтение заголовков
          if( _ac_>1 )return;   // Выход по чужому файлу
          scpy( iName,FName );  //
                IName=iName;
        } Sf=0;
        if( Nm<=0 )ans=_Esc;
        if( ans )break;
      }
      case _Enter:                      //
      { int N=0,j;                      //
        for( k=0; k<Lf; k++ )           // Расчистка удаляемых данных
         if( Ms[k].flag&0x10 )Ms[k].free();
        if( Fs )                        // Если файл уже открыт
        { Real W;
          fseek( Fs,Ver==12 || Ver==10 ? 26L : 8L+long( Nm-Lf )*(Ver==2?60:40),SEEK_SET );
          if( Ver==12 || Ver==10 )
          { for( k=Lf; k<Nm; k++ )
            { if( Ver==10 )j=getw( Fs ); else j=get2( Fs );
              New_Read( (Ms[k].flag&0x10)==0,j,Ms[k] );
            }
          }
          else                          //
          { for( k=Lf; k<Nm; k++ )      // Выбор характеристик ряда
            { Mario& M=Ms[k];           //
              if( ( M.flag&0x10 )==0 ){ N=M.N; M.N=0; M.allocate( N ); }
            }
            for(    j=0; j<N;  j++ )    //
            { for( k=Lf; k<Nm; k++ )    // Считывание данных в память
              { W=getf( Fs );           //
              //if( W==0xFFC00000L )W=0.0;                 // Non Number
                if( (Ms[k].flag&0x10)==0 )Ms[k].V[j]=W;
              } if( Lk>1 )fseek( Fs,unsigned( ( Lk-1 )*Nm )<<2,SEEK_CUR );
          } }                   //
        }                       // Упорядочение оставшихся рядов
        for( j=k=0; k<Nm; k++ ) //
         if( ( Ms[k].flag&0x10 )==0 ){ if( j!=k )Ms[j]=Ms[k]; ++j; }
        Sf=0;
        if( Tv<1 ){ Tback(); Tv=1; }
        if( Fs ){ fclose( Fs ); Fs=0; }
        Ms.allocate( Lf=j );    // MList=&(Ms[0]);
        m_View();               //
        if( !ans )ans=_Esc; break;
    } }
  } while( ans!=_Esc );
  Sf=0;
  if( Nm>Lf )Ms.allocate( Lf );
  if( Tv<1 )Tback();
}
//
//  -- устаревшие блоки 16-разрядных проверок
//
#if 0                               //
static long Request_Memory( int n ) // Вычисление объема требуемой памяти
{ long Ld=0;                        //
  for( int k=0; k<n; k++ )
  { if( k>=Lf )
    { if( (Ms[k].flag&0x10)==0 )Ld+=long( Ms[k].N )*4; }else
    { if( (Ms[k].flag&0x10)!=0 )Ld-=long( Ms[k].N )*4; }
  } return ( long( coreleft() )-Ld )/1024;
}
// long Ld=0;                   // Прогнозируемая длина данных в файле
//      Ld=Request_Memory( Nm );
//  if( Ld<30 ){ Tv_bell(); break; } // Если свободно более 30k памяти
#endif
