#include <StdIO.h>
#include <String.h>

int main( int ac, char **av )
{
   if( ac>1 )
   { FILE *I,*O;
     if( (I=fopen( av[1],"rb" ))!=0 )
     { int i,j,k;
       for( i=0;;i++ )
        if( av[1][i]==0 || av[1][i]=='.' ){ av[1][i]=0; break; }
       strcat( av[1],".c++" );
       if( (O=fopen( av[1],"wt"))!=0 )
       { unsigned char c,chr[1000000];
           fread( chr,128,1,I );
         k=fread( chr,1,1000000,I );
         c=chr[0];
         for( i=0; i<15; i++ )chr[i]=chr[i+1]; chr[15]=c;
         for( i=0; i<k; i++ )
         {
           if( !(i%32) )fprintf( O,"\n  " );
           fprintf( O,"0x%02X,",chr[i] );
         }
       }
     }
   }
}

 