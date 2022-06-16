#ifndef OO_POOLI_H
#define OO_POOLI_H

// Flags de debug pour CreatePool
#ifndef	OOF_DEBUG_WALLALLOC
#define OOF_DEBUG_WALLALLOC		0x01
#define OOF_DEBUG_FILLALLOC		0x02
#define OOF_DEBUG_FILLFREE		0x04
#define OOF_DEBUG_VERIFFREE		0x08
#endif

/*
#ifdef _DEBUG
#define OOIPool_GetLength(mem) (*((ULONG*)mem-1-(0x10/sizeof(ULONG))))
#else
#define OOIPool_GetLength(mem) (*--(ULONG*)mem)
#endif
*/

//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

void *			OOIPool_Create		( ULONG puddleSize, UBYTE debugflags, ULONG arraysize );
void *			OOIPool_Delete		( void *pool );
void			OOIPool_Reset		( void *pool );
void *			OOIPool_Resize		( void *pool, void *oldblock, ULONG newsize );
ULONG			OOIPool_GetCount	( void *pool );
ULONG			OOIPool_GetLength	( void *mem );
void *			OOIPool_AllocClear	( void *pool, ULONG memsize, ULONG *aid, BOOL want_id );
void *			OOIPool_Alloc		( void *pool, ULONG memsize, ULONG *aid, BOOL want_id );
void			OOIPool_EnableFree	( void *pool );
void			OOIPool_DisableFree	( void *pool );
BOOL			OOIPool_IsFreeEnabled( void *pool );
ULONG			OOIPool_FreeId		( void *pool, ULONG id );
void *			OOIPool_Free		( void *pool, void *mem );
BOOL			OOIPool_WritePool	( void *pool, FILE *file );
BOOL			OOIPool_ReadPool	( void *pool, FILE *file );
ULONG			OOIPool_GetId		( void *ptr );
ULONG			OOIPool_SetId		( void *pool, void *ptr, ULONG newid );
void *			OOIPool_GetAddress	( void *pool, ULONG id );
ULONG			OOIPool_GetLastId	( void *pool );

#endif // OO_POOLI_H
