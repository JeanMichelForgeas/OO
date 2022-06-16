/*****************************************************************
 *
 *       Project:    OO
 *       Function:   Buffers processing
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1997 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_pool.h"
#include "oo_buf.h"
#include "oo_text.h"

#include "oo_buf_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static char *OOBuf_CopyO( char *buf, char *dest, ULONG start, ULONG len );
static char *OOBuf_CutO( char *buf, ULONG start, ULONG len );
static char *OOBuf_PasteO( char *src, ULONG len, char *buf, ULONG start, char pad );
static char *OOBuf_SetO( char *src, ULONG len, char *buf );
static char *OOBuf_ResizeO( char *buf, ULONG newsize, BOOL leave );
static void OOBuf_FreeO( void *buf );


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

ULONG OOBuf_GetDatas( OOBuf *buf, UWORD *pw )
{
    if (pw) *pw = P2O(buf)->ClientWord;
    return P2O(buf)->ClientLong;
}

void OOBuf_SetDatas( OOBuf *buf, UWORD uw, ULONG ul )
{
    P2O(buf)->ClientWord = uw;
    P2O(buf)->ClientLong = ul;
}

//-------------------------------------

static char *OOBuf_CopyO( char *buf, char *dest, ULONG start, ULONG len )
{
   ULONG max;

    if (!len || !buf) return buf;
    if (start >= P2O(buf)->CurSize) return NULL;
    max = P2O(buf)->CurSize - start;
    if (max < len) len = max;
    if (dest == NULL) { if ((dest = OOBuf_Alloc( P2O(buf)->Pool, len, P2O(buf)->BlkSize )) == NULL) return NULL; }
    if ((dest = OOBuf_ResizeO( dest, len, FALSE )) == NULL) return NULL;
    memmove( dest, buf + start, len );
    return dest;
}

OOBuf *OOBuf_Copy( OOBuf *buf, OOBuf **pdest, ULONG start, ULONG len )
{
  char *ptr;
    if (ptr = OOBuf_CopyO( buf, *pdest, start, len )) *pdest = ptr;
    return ptr;
}

OOBuf *OOBuf_Clone( OOBuf *src, OOBuf **pdest )
{
  char *ptr;
    if (ptr = OOBuf_CopyO( src, NULL, 0, MAXULONG )) *pdest = ptr;
    return ptr;
}

//-------------------------------------

static char *OOBuf_CutO( char *buf, ULONG start, ULONG len )
{
  ULONG max;

    if (!len || !buf) return buf;
    if (start >= P2O(buf)->CurSize) return buf; // Rien à couper
    max = P2O(buf)->CurSize - start;
    if (max < len) len = max;
    if (max = P2O(buf)->CurSize - start - len) // Ce qui reste au bout
        memmove( buf+start, buf+start+len, max ); // comble le vide sur place
    return OOBuf_ResizeO( buf, P2O(buf)->CurSize - len, FALSE );
}

OOBuf *OOBuf_Cut( OOBuf **pbuf, ULONG start, ULONG len )
{
  char *ptr;
    if (ptr = OOBuf_CutO( *pbuf, start, len )) *pbuf = ptr;
    return ptr;
}

//-------------------------------------

static char *OOBuf_PasteO( char *src, ULONG len, char *buf, ULONG start, char pad )
{
  ULONG newsize, oldsize;

    if (!buf || !src) return NULL;
    if (!len) return( buf );
    oldsize = P2O(buf)->CurSize;
    if (start == MAXULONG) start = oldsize;
    newsize = len + ((start > oldsize) ? start : oldsize);
    if ((buf = OOBuf_ResizeO( buf, newsize, TRUE )) == NULL) return NULL;
    if (start < oldsize)      // décale le bout
        memmove( buf+start+len, buf+start, oldsize - start );
	else if (start > oldsize)
		memset( buf+oldsize, pad, start - oldsize ); 
    memmove( buf+start, src, len );
    return buf;
}

OOBuf *OOBuf_Paste( char *src, ULONG len, OOBuf **pdest, ULONG start, char pad )
{
  char *ptr;
    if (ptr = OOBuf_PasteO( src, len, *pdest, start, pad )) *pdest = ptr;
    return ptr;
}

//-------------------------------------
// This function is documented as :
//	if src == NULL or len == 0, the buffer is set to null string

static char *OOBuf_SetO( char *src, ULONG len, char *buf )
{
    if (! buf) return NULL;
    P2O(buf)->CurSize = 0;
    buf[0] = 0;
    if (src && len) buf = OOBuf_PasteO( src, len, buf, 0, 0 );
    return buf;
}

OOBuf *OOBuf_Set( char *src, ULONG len, OOBuf **pdest )
{
  char *ptr;
	if (len == (ULONG)-1 && src != NULL) len = strlen( src );
    if (ptr = OOBuf_SetO( src, len, *pdest )) *pdest = ptr;
    return ptr;
}

//-------------------------------------
// This function is documented as being fast and do not reallocate the buffer

OOBuf *OOBuf_Truncate( OOBuf *buf, ULONG newsize )
{
    if (buf)
        if (newsize < P2O(buf)->CurSize)
            { P2O(buf)->CurSize = newsize; buf[newsize] = 0; }
	// The pointer is not reallocated, so is not modified
    return buf;
}

//-------------------------------------

static ULONG compute_new_size( ULONG size, ULONG blksize ) /* résultat toujours > size */
{
    return ((size + blksize) / blksize) * blksize;
}

OOBuf *OOBuf_Alloc( APTR pool, ULONG size, UWORD blksize )
{
  ULONG totsize;
  OOSBuf *ob;

    if (!blksize) blksize = 16;
    totsize = compute_new_size( size, blksize );
	OOASSERT( OOGlobalPool != NULL );
	if (pool == NULL) pool = OOGlobalPool;
    if ((ob = (OOSBuf*) OOPool_Alloc( pool, sizeof(OOSBuf)+totsize )) != NULL)
        {
        ob->Pool    = (OOPool*)pool;
        ob->TotSize = totsize;
        ob->CurSize = size;
        ob->BlkSize	= blksize;
        ob->ClientWord = 0;
        ob->ClientLong = 0;
        O2P(ob)[size] = 0;
        return O2P(ob);
        }
    return NULL;
}

static void OOBuf_FreeO( void *buf )
{
    if (buf)
		{
		OOSBuf *oobuf = P2O(buf);
        OOPool_Free( oobuf->Pool, oobuf );
		}
}

void OOBuf_Free( void **pmem )
{
    OOBuf_FreeO( *pmem );
    *pmem = NULL;
}

ULONG OOBuf_Length( OOBuf *buf )
{
    return P2O(buf)->CurSize;
}

//-------------------------------------

static char *OOBuf_ResizeO( char *buf, ULONG newsize, BOOL leave )
{
  ULONG totsize, copylen;
  OOSBuf *newbuf;

    if (! buf) return NULL;
    totsize = compute_new_size( newsize, P2O(buf)->BlkSize );
    if (leave == TRUE && (totsize <= P2O(buf)->TotSize)) goto INPLACE;
    if (totsize == P2O(buf)->TotSize) goto INPLACE;

	OOASSERT( OOGlobalPool != NULL );
    if ((newbuf = (OOSBuf*) OOPool_Alloc( P2O(buf)->Pool, sizeof(OOSBuf)+totsize )) == NULL)
        { if (newsize >= P2O(buf)->TotSize) return NULL; else goto INPLACE; } // if no memory, do like leave == TRUE
    newbuf->Pool      = P2O(buf)->Pool;
    newbuf->TotSize   = totsize;
    newbuf->CurSize   = newsize;
    newbuf->BlkSize   = P2O(buf)->BlkSize;
    newbuf->ClientWord = P2O(buf)->ClientWord;
    newbuf->ClientLong = P2O(buf)->ClientLong;

    copylen = MIN(newsize,P2O(buf)->CurSize);
    if (copylen) memmove( O2P(newbuf), buf, copylen );
    O2P(newbuf)[newsize] = 0;

    OOBuf_FreeO( buf );
    return O2P(newbuf);

  INPLACE:
    P2O(buf)->CurSize = newsize;
    buf[newsize] = 0;
    return buf;
}

OOBuf *OOBuf_Resize( OOBuf **pold, ULONG newsize, BOOL leave )
{
  char *ptr;
    if (ptr = OOBuf_ResizeO( *pold, newsize, leave )) *pold = ptr;
    return ptr;
}

//*******************************************************************************
//*******************************************************************************
//**************************							*************************
//**************************		OOBuf_Format		*************************
//**************************							*************************
//*******************************************************************************
//*******************************************************************************
//
// Cette fonction crée un buffer si *ubuf == NULL et laisse
// le buffer alloué. C'est au programmeur d'appeler la
// fonction OOBuf_Free() quand il n'en a plus besoin.
//

OOBuf *OOBuf_FormatV( OOBuf **ubuf, ULONG start, char *fmtstr, ... )
{
  va_list arglist;
  char *buf;

    va_start( arglist, fmtstr );
    buf = OOBuf_Format( ubuf, start, fmtstr, arglist );
    va_end( arglist );
    return buf;
}

OOBuf *OOBuf_FormatVI( OOBuf **ubuf, ULONG start, ULONG fmtid, ... )
{
  va_list arglist;
  char *buf;

    va_start( arglist, fmtid );
    buf = OOBuf_FormatI( ubuf, start, fmtid, arglist );
    va_end( arglist );
    return buf;
}

OOBuf *OOBuf_FormatI( OOBuf **ubuf, ULONG start, ULONG fmtid, va_list args )
{
  char *fmtstr;

	if ((fmtstr = (char*) OOStr_Get( fmtid )) == NULL)
		return NULL;
	return OOBuf_Format( ubuf, start, fmtstr, args );
}

#include <stdio.h>  /* Pour le proto de vsprintf() */

/////////////////////////////////////////////////////////////////////////////////
//===============================================================================
#ifdef AMIGA

OOBuf *OOBuf_Format( OOBuf **ubuf, ULONG start, char *fmtstr, va_list args )
{
  va_list arglist=args;
  ULONG genlen=0, len;
  char *fmt=fmtstr, *str, *buf=*ubuf, size;

  int           arg_int;
  long          arg_long;
  short         arg_short;
  char         *arg_charptr;
  double        arg_double;
  long double   arg_longdouble;

    if (! buf)
        {
        if (! (buf = OOBuf_Alloc( NULL, 0, 32 ))) return(0);
        *ubuf = buf;
        }

    if (P2O(buf)->CurSize < start)
        start = P2O(buf)->CurSize;

    /*------ Analyse des arguments pour déduire la longueur maximum de la chaîne résultante */
    while (TRUE)
        {
        /*------ Caractères normaux */
        str = fmt;
        for (fmt++; *fmt && *fmt != '%'; fmt++) ;
        genlen += fmt - str - 1;

        if (*fmt == 0) break;

        /*------ Décodage "%[flags][width][.precision][size]type" ------*/
        if (*fmt++ == '%')
            {
            /*--- flags ---*/
            switch (*fmt) { case '-': case '+': case ' ': case '#': case '0':
            fmt++; break; }
            genlen += 2; /* dans tous les cas un maximum de 2 pour le signe ou 0x */

            /*--- width ---*/
            if (*fmt == '*') { arg_int = va_arg(args,int); fmt++; }
            else while (*fmt >= '0' && *fmt <= '9') fmt++;

            /*--- precision ---*/
            if (*fmt == '.')
                {
                fmt++;
                if (*fmt == '*') { arg_int = va_arg(args,int), fmt++; }
                else while (*fmt >= '0' && *fmt <= '9') fmt++;
                }

            /*--- size ---*/
            size = 0;
            switch (*fmt) { case 'L': case 'l': case 'h':
            size = *fmt++; break; }

            /*--- type ---*/
            switch (*fmt++)
                {
                case '%':
                    len = 1;
                    break;

                case 'c':
                    arg_int = va_arg(args,int);
                    len = 1;
                    break;

                case 'e':
                case 'E':
                case 'f':
                case 'g':
                case 'G':
                    if (size == 'L') { len = 60; arg_longdouble = va_arg(args,long double); }
                    else { len = 30; arg_double = va_arg(args,double); }
                    break;

                case 'n':
                    arg_charptr = va_arg(args,char*);
                    len = 0;
                    break;

                case 's':
                    arg_charptr = va_arg(args,char*);
                    len = 1 + strlen( arg_charptr );
                    break;

                case 'p':
                case 'P':
                    arg_charptr = va_arg(args,char*);
                    len = 30;
                    break;

                case 'd':
                case 'i':
                case 'o':
                case 'u':
                case 'x':
                case 'X':
                    if (size == 'l') arg_long = va_arg(args,long);
                    else if (size == 'h') arg_short = va_arg(args,short);
                    else arg_int = va_arg(args,int);
                    len = 30;
                    break;

                default: /* on prend ce caractère à la boucle suivante */
                    fmt--;
                    len = 0;
                    break;
                }

            genlen += len;
            }

        }

    if (genlen)
        {
        ULONG newsize, oldsize;
        genlen++; /* Pour le zéro de fin */

        /*------ On fait la place dans le buffer pour sprintf() ------*/
        oldsize = P2O(buf)->CurSize;
        newsize = oldsize + genlen;
        if ((buf = OOBuf_Resize( ubuf, newsize, TRUE )) == NULL)
            return NULL;
        if (start < oldsize)      /* décale le bout */
            memmove( buf+start+genlen, buf+start, oldsize - start );

        /*------ On imprime et on enlève la place en trop ------*/
        len = vsprintf( buf+start, fmtstr, arglist );

        /*------ On fait un équivalent de OOBuf_Cut mais sans ré-allocation */
        if (len < genlen) /* Au moins de 1 pour le zéro de fin */
            {
            start = start + len;
            len = genlen - len;
            if (start < P2O(buf)->CurSize)
                {
                ULONG max = P2O(buf)->CurSize - start;
                if (max < len) len = max;
                if (max = P2O(buf)->CurSize - start - len) /* Ce qui reste au bout */
                    memmove( buf+start, buf+start+len, max ); /* comble le vide sur place */
                buf = OOBuf_Resize( ubuf, P2O(buf)->CurSize - len, TRUE );
                }
            }
        }

    return( buf );
}

#endif // AMIGA
//===============================================================================
/////////////////////////////////////////////////////////////////////////////////

//---

/////////////////////////////////////////////////////////////////////////////////
//===============================================================================
#ifdef _WIN32

#include <tchar.h>

#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

/*
#if defined(_68K_) || defined(_X86_)
	#define DOUBLE_ARG  { char doubleBits[sizeof(double)]; };
#else
	#define DOUBLE_ARG  double
#endif
*/
#define DOUBLE_ARG  double

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000

OOBuf *OOBuf_Format( OOBuf **ubuf, ULONG start, char *fmtstr, va_list args )
{
	va_list arglist = args;
	ULONG maxlen = 0, len;
	char *fmt = (char*)fmtstr;
	char *buf = (char*)*ubuf;

    if (buf == NULL)
        {
        if ((buf = (char*) OOBuf_Alloc( NULL, 0, 32 )) == NULL) 
			return NULL;
        *ubuf = (char*)buf;
        }

    if (P2O(buf)->CurSize < start)
        start = P2O(buf)->CurSize;

    //------ Analyse des arguments pour déduire la longueur maximum de la chaîne résultante

	// make a guess at the maximum length of the resulting string
	for (fmt=(char*)fmtstr; *fmt != '\0'; fmt=_tcsinc(fmt))
	{
		int itemlen = 0;
		int width = 0;
		int precision = 0;
		ULONG modifier = 0;

		// handle '%' character, but watch out for '%%'
		if (*fmt != '%' || *(fmt = _tcsinc(fmt)) == '%')
		{
			maxlen += _tclen(fmt);
			continue;
		}

		// handle '%' character with format
		for (; *fmt != '\0'; fmt = _tcsinc(fmt))
		{
			// check for valid flags
			if (*fmt == '#')
				maxlen += 2;   // for '0x'
			else if (*fmt == '*')
				width = va_arg(args,int);
			else if (*fmt == '-' || *fmt == '+' || *fmt == '0' || *fmt == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (width == 0)
		{
			// width indicated by
			width = _ttoi(fmt);
			for (; *fmt != '\0' && _istdigit(*fmt); fmt = _tcsinc(fmt))
				;
		}
		OOASSERT(width >= 0);

		if (*fmt == '.')
		{
			// skip past '.' separator (width.precision)
			fmt = _tcsinc(fmt);

			// get precision and skip it
			if (*fmt == '*')
			{
				precision = va_arg(args,int);
				fmt = _tcsinc(fmt);
			}
			else
			{
				precision = _ttoi(fmt);
				for (; *fmt != '\0' && _istdigit(*fmt); fmt = _tcsinc(fmt))
					;
			}
			OOASSERT(precision >= 0);
		}

		// should be on type modifier or specifier
		switch (*fmt)
		{
		// modifiers that affect size
		case 'h':
			modifier = FORCE_ANSI;
			fmt = _tcsinc(fmt);
			break;
		case 'l':
			modifier = FORCE_UNICODE;
			fmt = _tcsinc(fmt);
			break;

		// modifiers that do not affect size
		case 'F':
		case 'N':
		case 'L':
			fmt = _tcsinc(fmt);
			break;
		}

		// now should be on specifier
		switch (*fmt | modifier)
		{
		// single characters
		case 'c':
		case 'C':
			itemlen = 2;
			va_arg(args,TCHAR_ARG);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			itemlen = 2;
			va_arg(args,CHAR_ARG);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			itemlen = 2;
			va_arg(args,WCHAR_ARG);
			break;

		// strings
		case 's':
			itemlen = lstrlen(va_arg(args,LPCTSTR));
			itemlen = max(1, itemlen);
			break;

		case 'S':
#ifndef _UNICODE
			itemlen = wcslen(va_arg(args,LPWSTR));
#else
			itemlen = lstrlenA(va_arg(args,LPCSTR));
#endif
			itemlen = max(1, itemlen);
			break;

		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			itemlen = lstrlenA(va_arg(args,LPCSTR));
			itemlen = max(1, itemlen);
			break;
#ifndef _MAC
		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			itemlen = wcslen(va_arg(args,LPWSTR));
			itemlen = max(1, itemlen);
			break;
#endif
		}

		// adjust itemlen for strings
		if (itemlen != 0)
		{
			itemlen = max(itemlen, width);
			if (precision != 0)
				itemlen = min(itemlen, precision);
		}
		else
		{
			switch (*fmt)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				va_arg(args,int);
				itemlen = 32;
				itemlen = max(itemlen, width+precision);
				break;

			case 'e':
			case 'f':
			case 'g':
			case 'G':
				va_arg(args,DOUBLE_ARG);
				itemlen = 128;
				itemlen = max(itemlen, width+precision);
				break;

			case 'p':
				va_arg(args,void*);
				itemlen = 32;
				itemlen = max(itemlen, width+precision);
				break;

			// no output
			case 'n':
				va_arg(args,int*);
				break;

			default:
				OOASSERT(FALSE);  // unknown formatting option
			}
		}

		// adjust maxlen for output itemlen
		maxlen += itemlen;
	}

    if (maxlen != 0)
        {
        ULONG newsize, oldsize;
        maxlen++; // Pour le zéro de fin

        //------ On fait la place dans le buffer pour sprintf()
        oldsize = P2O(buf)->CurSize;
        newsize = oldsize + maxlen;
        if ((buf = (char*) OOBuf_Resize( ubuf, newsize, TRUE )) == NULL)
            return NULL;
        if (start < oldsize)      // décale le bout
            memmove( buf+start+maxlen, buf+start, oldsize - start );

        //------ On imprime et on enlève la place en trop
        len = vsprintf( buf+start, (char*)fmtstr, arglist ); // _vstprintf
				  
        //------ On fait un équivalent de BufCut mais sans ré-allocation
        if (len < maxlen) // Au moins de 1 pour le zéro de fin
            {
            start = start + len;
            len = maxlen - len;
            if (start < P2O(buf)->CurSize)
                {
                ULONG max = P2O(buf)->CurSize - start;
                if (max < len) len = max;
                if (max = P2O(buf)->CurSize - start - len) // Ce qui reste au bout
                    memmove( buf+start, buf+start+len, max ); // comble le vide sur place
                buf = (char*) OOBuf_Resize( ubuf, P2O(buf)->CurSize - len, TRUE );
                }
            }
        }

	*ubuf = (char*)buf; // If OK update the pointer
    return (char*)buf;
}

#endif // _WIN32
//===============================================================================
/////////////////////////////////////////////////////////////////////////////////

//---

/////////////////////////////////////////////////////////////////////////////////
//===============================================================================
#ifdef _MAC

#define TCHAR_ARG   int
#define WCHAR_ARG   unsigned
#define CHAR_ARG    int

#if defined(_68K_) || defined(_X86_)
	#define DOUBLE_ARG  { char doubleBits[sizeof(double)]; };
#else
	#define DOUBLE_ARG  double
#endif

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000

OOBuf *OOBuf_Format( OOBuf **ubuf, ULONG start, char *fmtstr, va_list args )
{
	...
}

#endif // _MAC
//===============================================================================
/////////////////////////////////////////////////////////////////////////////////
