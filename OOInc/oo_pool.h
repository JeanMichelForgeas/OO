#ifndef OO_POOL_H
#define OO_POOL_H

// Flags de debug pour CreatePool
#ifndef	OOF_DEBUG_WALLALLOC
#define OOF_DEBUG_WALLALLOC		0x01
#define OOF_DEBUG_FILLALLOC		0x02
#define OOF_DEBUG_FILLFREE		0x04
#define OOF_DEBUG_VERIFFREE		0x08
#endif

/*
#ifdef _DEBUG
#define OOPool_GetLength(mem) (*((ULONG*)mem-1-(0x10/sizeof(ULONG))))
#else
#define OOPool_GetLength(mem) (*--(ULONG*)mem)
#endif
*/

//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

#ifdef __cplusplus
extern "C" {
#endif

void *			OOPool_Create		( ULONG puddleSize, UBYTE debugflags );
void *			OOPool_Delete		( void *pool );
void			OOPool_Reset		( void *pool );
void *			OOPool_Resize		( void *pool, void *oldblock, ULONG newsize );
ULONG			OOPool_GetCount		( void *pool );
ULONG			OOPool_GetLength	( void *mem );
void *			OOPool_AllocClear	( void *pool, ULONG memsize );
void *			OOPool_Alloc		( void *pool, ULONG memsize );
void *			OOPool_Free			( void *pool, void *mem );
void			OOPool_EnableFree	( void *pool );
void			OOPool_DisableFree	( void *pool );
BOOL			OOPool_IsFreeEnabled( void *pool );

#ifdef __cplusplus
	}
#endif

#endif // OO_POOL_H
