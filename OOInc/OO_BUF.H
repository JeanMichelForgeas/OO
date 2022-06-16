#ifndef OO_BUF_H
#define OO_BUF_H


//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

ULONG	OOBuf_GetDatas	( OOBuf *buf, UWORD *pw );
void	OOBuf_SetDatas	( OOBuf *buf, UWORD uw, ULONG ul );
OOBuf *	OOBuf_Clone		( OOBuf *src, OOBuf **pdest );
OOBuf *	OOBuf_Copy		( OOBuf *buf, OOBuf **pdest, ULONG start, ULONG len );
OOBuf *	OOBuf_Cut		( OOBuf **pbuf, ULONG start, ULONG len );
OOBuf *	OOBuf_Paste		( char *src, ULONG len, OOBuf **pdest, ULONG start, char pad );
OOBuf *	OOBuf_Set		( char *src, ULONG len, OOBuf **pdest );
OOBuf *	OOBuf_Truncate	( OOBuf *buf, ULONG newsize );
OOBuf *	OOBuf_Alloc		( void *pool, ULONG size, UWORD blksize );
void	OOBuf_Free		( void **pmem );
ULONG	OOBuf_Length	( OOBuf *buf );
OOBuf *	OOBuf_Resize	( OOBuf **pold, ULONG newsize, BOOL leave );
OOBuf *	OOBuf_FormatV	( OOBuf **ubuf, ULONG start, char *fmtstr, ... );
OOBuf *	OOBuf_FormatVI	( OOBuf **ubuf, ULONG start, ULONG fmtid, ... );
OOBuf *	OOBuf_Format	( OOBuf **ubuf, ULONG start, char *fmtstr, va_list args );
OOBuf *	OOBuf_FormatI	( OOBuf **ubuf, ULONG start, ULONG fmtid, va_list args );

#endif // OO_BUF_H
