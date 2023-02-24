//
//	������� ��������� ����������� � ������������� ����
//
#include <StdIO.h>
#include <String.h>
#define Block 4096
#if 1
int main( int Ac, char **Av )
{ if( Ac>0 )
  { printf( "%s <> %s ",Av[0],Av[1] );
    FILE *Fi,*Fo;
    if( (Fi=fopen( Av[1],"rb"  ))!=NULL
    &&  (Fo=fopen( Av[1],"r+b" ))!=NULL )
    { size_t Len;
      char Buff[Block];
        fseek( Fi,4,SEEK_SET );
	fseek( Fo,4,SEEK_SET );
	while( (Len=fread( Buff,1,Block,Fi ))>1 )
        { swab( Buff,Buff,Len&0xFFFE );
	  fwrite( Buff,1,Len,Fo );
	  if( Len!=Block ) break;
	}    printf( "OK" );
    } else printf( "Shut" ); fclose( Fi ); fclose( Fo );
  }        printf( "\n" );
}
#else
#include <Dir.h>
#include <Dos.h>
#include <StdLib.h>


void main( int Ac, char **Av )
{
 int k,done;
 struct ffblk F;
  for( k=1; k<Ac; k++ ){
   char	Path[MAXPATH],Drive[MAXDRIVE],Dir[MAXDIR],File[MAXFILE],Ext[MAXEXT];
    fnsplit( Av[k],Drive,Dir,File,Ext );
    printf( ">%d: %s\n",k,Av[k] );
    done=findfirst( Av[k],&F,0 );
    while( !done ){
      done=(F.ff_attrib&FA_RDONLY)==0;
      printf( "%12s %8ld Read %s ",F.ff_name,F.ff_fsize,
					done?"Write":"Only" );
      if( done ){
	FILE *Fi,*Fo;
	fnmerge( Path,Drive,Dir,"","" ); strcat( Path,F.ff_name );
	if( (Fi=fopen( Path,"rb"  ))!=NULL
	&&  (Fo=fopen( Path,"r+b" ))!=NULL ){
	 int  Len;
	 char Buff[Block];
	  fseek( Fi,4,SEEK_SET );
	  fseek( Fo,4,SEEK_SET );
	  while( (Len=fread( Buff,1,Block,Fi ))>1 ){
	    swab( Buff,Buff,Len&0xFFFE );
	    fwrite( Buff,1,Len,Fo );
	    if( Len!=Block ) break;
	  }   printf( "OK" );
	}else printf( "Shut" ); fclose( Fi ); fclose( Fo );
      }       printf( "\n" );   done=findnext( &F );
    }
  }
}
#endif
