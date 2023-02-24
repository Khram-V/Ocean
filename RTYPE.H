#ifndef __RTYPE_H
#define __RTYPE_H

#ifndef __CTYPE_H
#include <ctype.h>
#endif

#undef	isalnum(c)
#define isalnum(c) ((_ctype[(unsigned)(c)+1]&(_IS_DIG|_IS_UPP6|_IS_LOW))||\
	((unsigned)(c)>=0x80&&(unsigned)(c)<=0xaf)||\
	((unsigned)(c)>=0xe0&&(unsigned)(c)<=0xef)\
)
#undef	isalpha(c)
#define isalpha(c) ((_ctype[(unsigned)(c)+1]&(_IS_UPP|_IS_LOW))||\
	((unsigned)(c)>=0x80&&(unsigned)(c)<=0xaf)||\
	((unsigned)(c)>=0xe0&&(unsigned)(c)<=0xef)\
)
#undef	isgraph(c)
#define isgraph(c) ((unsigned)(c)>=0x21&&(unsigned)(c)!=0x7f&&(unsigned)(c)!=0xff)
#undef	islower(c)
#define islower(c) ((_ctype[(unsigned)(c)+1]&_IS_LOW)||\
	((unsigned)(c)>=0xa0&&(unsigned)(c)<=0xaf)||\
	((unsigned)(c)>=0xe0&&(unsigned)(c)<=0xef)\
)
#undef	isprint(c)
#define isprint(c) ((unsigned)(c)>=0x20&&(unsigned)(c)!=0x7f)
#undef	isupper(c)
#define isupper(c) ((_ctype[(unsigned)(c)+1]&_IS_UPP)||\
	((unsigned)(c)>=0x80&&(unsigned)(c)<=0x9f)\
)

#if !__STDC__
#undef	_toupper(c)
#define _toupper(c) ((isascii(c))?((c) + 'A' - 'a'):\
	(((unsigned)(c)<=0xaf)?((unsigned)(c) + '€' - ' '):\
	((unsigned)(c) + '' - 'à'))\
)

#undef	_tolower(c)
#define _tolower(c) ((isascii(c))?((c) + 'a' - 'A'):\
	(((unsigned)(c)<=0x8f)?((unsigned)(c) + ' ' - '€'):\
	((unsigned)(c) + 'à' - ''))\
)
#endif

#define tolower(__c) _rtolower(__c)
#define toupper(__c) _rtoupper(__c)

int _rtolower(int __c)
{
	return( isupper(__c) ? _tolower(__c) : (__c) );
}

int _rtoupper(int __c)
{
	return( islower(__c) ? _toupper(__c) : (__c) );
}

#ifdef __STRING_H

#define strupr(__s)  _rstrupr(__s)
#define strlwr(__s)  _rstrlwr(__s)

char *_rstrlwr(char *__s)
{
char *s;
	for(s=__s;*__s;__s++)
		*__s=tolower(*__s &0xff);
	return s;
}

char *_rstrupr(char *__s)
{
char *s;
	for(s=__s;*__s;__s++)
		*__s=toupper(*__s &0xff);
	return s;
}

#endif
#endif
