//
//      B_geo.c                                                   /91.05.11/
//      Процедуры градусно-часовых преобразований
//
#include <Math.h>
template <class real> real Angle( real A )
       { A=fmod( A,360.0 ); return A>180.0?A-360.0:( A<-180.0?A+360.0:A );
       }                                   //
template double Angle <double> ( double ); // Контроль направления
template float Angle <float> ( float );    //
//
//    Формирование текстовой строки F=0:{-123°46'57"89} 1-deg,2+min,3+sec,-hnd
//
#define put2hnd *s++=d/10+'0',*s++=d%10+'0';      // -- под сотки из цифирек --
inline char *put10( char *s, int d )              // -- составная рекурсия   --
       { int t=d%10; if( d>=10 )s=put10( s,d/10 ); *s++ =t+'0'; return s;
       }
char *DtoA( double D, int F, char c )                                  // с='°'
{ const double rnd[]={ 0.4999999,8.3333333e-3,1.3888888e-4 };
  static char S[16]; char *s=S;
  int d,f,l=0;                                  // -- счетчик заполненных полей
    if( (D=Angle( D ))<0.0 ){ D=-D; *s++='-'; } else *s++=' ';
    if( !F )F=-3; f=F;
    if( F>0 )D+=rnd[F-1]; else{ F=-F; D+=rnd[F-1]/100.0; } d=D; D-=d;
                        if( d||(!l&&F<2 ) ){ s=put10( s,d ); *s++=c; ++l;}
    if( F>1 ){ d=D*=60; if( d||(!l&&F<3 ) ){ put2hnd *s++='\''; ++l; } D-=d; }
    if( F>2 ){ d=D*=60; if( d|| !l        ){ put2hnd *s++='"';  ++l; } D-=d; }
    if( f<0 ){ d=D*100; if( d )put2hnd } *s=0; return S;
}
//
//  Разбор текстовой строки с градусами со свертыванием нового числа
//                                       (и теперь в реентерабельном контексте)
static char* getright( char *s, double& D )
{ double d=1.0; while( *s>='0' && *s<='9' )D+=(*s++ -'0')/(d*=10); return s;
}
static char* getleft( char *s, double& D )
{ while( *s>='0' && *s<='9' ){ D*=10.0; D+= *s++ -'0'; }
  if( *s=='.' || *s==',' )s=getright( ++s,D ); return s;
}
char *AtoD( char *S, double& _D )           // на выходе следующее свободное
{ double D=0.0,m;    char *s,*rs=S;         //  - (никому не нужное) поле
  short n=0,sign=0;  while( *rs<=' ' )++rs; // простая выборка пустых символов
    if( *rs=='-' )sign=1;                   // на случай установки знака числа
    if( *rs=='+' || sign )++rs;             // .. считывание и подвижка символа
  Rept: rs=getleft( s=rs,m=0.0 );           // считываются цифры до разделителя
    switch( *rs++ )                         // с перескоком по градусным меткам
    { case 0xF8: case '°':
      case ':' : if( n<1 ){ n|=1,D+=m;      goto Rept; } break;
      case '\'': if( n<2 ){ n|=2,D+=m/60;   goto Rept; } break;
      case '"' : if( n<4 ){ n|=4,D+=m/3600; goto Rept; } break; // повторное
      default  : if( !n )D+=m; else{ rs=getright( s,m=0 );     // считывание
                                     if( n&4 )D+=m/3600; else // - и ничего не
                                     if( n&2 )D+=m/60; else D+=m; } // попишешь
    } if( sign )D=-D; _D=D; return rs;
}
char *AtoD( char *s,float &f ){ double d; char *w=AtoD( s,d ); f=d; return w; }
//
//      Исправление градусов (°) в часы (h)
//
char* DtoH( double H, int f ){ return DtoA( H,f,0xFC ); }
