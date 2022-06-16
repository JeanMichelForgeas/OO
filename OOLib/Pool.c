/*****************************************************************
 *
 *       Project:    OO
 *       Function:   Memory pool processing
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1997 Jean-Michel Forgeas
 *
 * 
 * A MemPool is intended to be a universal memory pool manager which 
 * can be used for any situation where :
 * 	- there are many small allocations 
 * 	- we want to free all allocs at once
 * 	- we want an id for each alloc to survive save/reload
 * 
 * To save a pool to disk, WritePool() does :
 *   0 - Leave place for total size of this save operation
 *   1 - Save the number of elements of the pool : pool->LastId
 *   2 - Save first the identifier table : it is a table of pointers,  
 * 	but for save operation, each pointer (which is not null at 
 * 	identifier position) must be writen as a puddle number followed  
 * 	by the offset into the puddle. 
 * 	The puddle number is the place of the puddle during the save, so 
 *  its	place in the liked list.
 *   3 - For each puddle in the linked list, save :
 * 		4 - its size
 * 		5 - all its contents in only one operation
 *   6 - Update the total size into (0)
 * 					  ------------------------------
 * Pour regénérer Pool.c :
 *	- Copier PoolI.c en Pool.c
 * 	- Renommer tous les OOPool_ en OOPool_
 * 	- Enlever les lignes comprises entre #ifdef MEMPOOL_USE_ID_TABLE
 *	- Changer les noms des includes oo_pool...
 * 
 *****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_pool.h"
#include "oo_array.h"

#include "oo_pool_prv.h"

#define OOPOOL	((OOPool*)pool)


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static void *allocate( OOPuddle *pdl, ULONG size );
static void deallocate( OOPuddle *pdl, OOChunk *mem, ULONG size );
#ifdef _DEBUG
static void verif( void *pool );
static void fill_walls( UBYTE *mem, ULONG memsize );
static void verif_walls( UBYTE *mem );
static void fill_alloc( ULONG *ptr, ULONG size, ULONG value );
#endif


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

void *OOPool_Create( ULONG puddleSize, UBYTE debugflags
					)
{
  OOPool *pool;

	if (puddleSize < sizeof(OOPuddle)+64)
		puddleSize = sizeof(OOPuddle)+64;

    if (pool = (OOPool*) malloc( sizeof(OOPool) ))
        {
        OOList_Init( &pool->List );
#ifdef _DEBUG
        pool->DebugFlags = debugflags;
		pool->DebugFlags |= OOF_DEBUG_WALLALLOC; // Force this for macro GetLength()
#endif
        pool->PuddleSize = (puddleSize + 7) & 0xfffffff8;
        pool->Flags		 = 0;
        pool->AllocCount = 0;
        }
    return( (void*)pool );
}

void *OOPool_Delete( void *pool )
{
	if (pool)
		{
		OONode *next, *node;
#ifdef _OODEBUG
		verif( pool );
#endif
		for (node = OOPOOL->List.lh_Head; next = node->ln_Succ; node = next)
			free( node );
		free( pool );
		}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// MemPool commands

void OOPool_Reset( void *pl )
{
	OONode *next, *node;
	OOPool *pool = (OOPool*)pl;

	for (node = pool->List.lh_Head; next = node->ln_Succ; node = next)
		free( node );
}

//------------------------------------------------

ULONG OOPool_GetLength( void *mem )
{
#ifdef _DEBUG
	return *((ULONG*)mem-1-(0x10/sizeof(ULONG)));
#else
	return *--(ULONG*)mem;
#endif
}

void *OOPool_Resize( void *pool, void *oldblock, ULONG newsize )
{
	void *newblock;

    if (newblock = OOPool_AllocClear( pool, newsize 
		))
        {
        if (oldblock)
            {
            ULONG size = OOPool_GetLength( oldblock );
            if (newsize < size) 
				size = newsize;
            if (size) 
				memmove( newblock, oldblock, size );
            OOPool_Free( pool, oldblock );
            }
        }
    return newblock;
}

ULONG OOPool_GetCount( void *pool )
{
    return OOPOOL->AllocCount;
}

void *OOPool_AllocClear( void *pool, ULONG memsize
						)
{
  void *ptr;

    if (ptr = OOPool_Alloc( pool, memsize
		))
        memset( ptr, 0, memsize );
    return( ptr );
}

void *OOPool_Alloc( void *pl, ULONG memsize
				   )
{
  OOPool *pool = (OOPool*)pl;
  ULONG *ptr=0, sepsize, realsize=0;
  OOPuddle *node;
  OOPuddle *next;
  OOChunk *free;

#ifdef _DEBUG
	OOASSERT( pool != NULL );
    if (pool->DebugFlags) 
		{
		verif( pool );
		if (pool->DebugFlags & OOF_DEBUG_WALLALLOC) realsize = 0x20;
		}
#endif
    realsize += ((memsize + OOIDOFFSET + 4 + 7) & 0xfffffff8);
	if (realsize >= pool->PuddleSize) goto lF8223E;

  lF821F6:
    next = (OOPuddle*)pool->List.lh_Head;
  lF821F8:
    node = next;
    if (! (next = node->ln_Succ)) goto lF8226E;
    if (! node->TotalSize) goto lF8226E;   // Fin de la liste des puddles de mémoire libre 
    if (realsize > node->mh_Free) goto lF821F8;
    if (! (ptr = (ULONG*) allocate( node, realsize ))) goto lF821F8;
  lF82238:
    *ptr++ = memsize;
	pool->AllocCount++;
#ifdef _DEBUG
    if (pool->DebugFlags)
        {
        if (pool->DebugFlags & OOF_DEBUG_WALLALLOC) { fill_walls( (UBYTE*)ptr, memsize ); ptr = (ULONG*)((ULONG)ptr + 0x10); }
        if (pool->DebugFlags & OOF_DEBUG_FILLALLOC) fill_alloc( (ULONG*)ptr, memsize, 0xABADF00D );
        }
#endif
    return (void*)ptr;

  lF8223E:          // Fait une allocation séparée car trop grande 
    sepsize = realsize + 12;
    if ((ptr = (ULONG*) malloc( sepsize )) == NULL)
		return NULL;
	OOList_AddTail( (OOList*)&pool->List, (OONode*)ptr );
    ptr += 2;		// List header
    *ptr++ = 0;     // TotalSize = 0; n'est pas un puddle de mémoire libre 
    goto lF82238;

  lF8226E:          // Alloue un nouveau puddle de mémoire libre 
    sepsize = ((sizeof(OOPuddle) + 4 + 7) & 0xfffffff8) + pool->PuddleSize;
    if ((node = (OOPuddle*) malloc( sepsize )) == NULL)
		return NULL;
    free = (OOChunk*)((((ULONG)node + 7) & 0xfffffff8) + sizeof(OOPuddle));
    OOList_AddHead( (OOList*)&pool->List, (OONode*)node );
    node->TotalSize = sepsize;  // Indique un puddle dans la liste car != 0 
#ifdef _DEBUG
    node->DebugFlags = pool->DebugFlags;
#endif
    node->mh_OffsFirstChunk = (UBYTE*)free - (UBYTE*)&node->mh_OffsFirstChunk;
    node->mh_OffsLower = (UBYTE*)free - (UBYTE*)node;
    node->mh_OffsUpper = node->mh_OffsLower + pool->PuddleSize;
    node->mh_Free  = pool->PuddleSize;	 
    free->OffsNext = LASTFREECHUNK;	// Pas de chunk suivant
    free->Bytes    = pool->PuddleSize;
#ifdef _DEBUG
    if (node->DebugFlags & OOF_DEBUG_FILLFREE) fill_alloc( (ULONG*)((ULONG)free+8), pool->PuddleSize-8, 0xDEADBEEF );
#endif
    goto lF821F6;
}

void OOPool_EnableFree( void *pool )
{
	OOPOOL->Flags &= ~OOF_MEMPOOL_DISABLEFREE;
}

void OOPool_DisableFree( void *pool )
{
	OOPOOL->Flags |= OOF_MEMPOOL_DISABLEFREE;
}

BOOL OOPool_IsFreeEnabled( void *pool )
{
	return (OOPOOL->Flags & OOF_MEMPOOL_DISABLEFREE) ? FALSE : TRUE;
}

void *OOPool_Free( void *pl, void *mem )
{
  OOPool *pool = (OOPool*)pl;
  ULONG size, realsize=0;
  OOPuddle *pre1;
  OOPuddle *pre2;
  OOPuddle *node=NULL;
  OOPuddle *next;
	
	OOASSERT( pool != NULL );
	if (pool->Flags & OOF_MEMPOOL_DISABLEFREE)
		return NULL;
    if (mem == NULL)
		return NULL;

#ifdef _DEBUG
    if (pool->DebugFlags)
        {
        verif( pool );
        if (pool->DebugFlags & OOF_DEBUG_WALLALLOC) 
			{ mem = (void*)((ULONG)((char*)mem-0x10)); realsize = 0x20; verif_walls( (UBYTE*)mem ); }
        }
#endif
    mem = (void*)((ULONG)((char*)mem-4)); 
	size = *(ULONG*)mem; 
	realsize += ((size + OOIDOFFSET + 4 + 7) & 0xfffffff8);
    if (realsize >= pool->PuddleSize) goto lC5E924;	// Libère à l'intérieur d'un puddle 

    next = (OOPuddle*)pool->List.lh_Head;
  lC5E8D2:												// Trouve le bon puddle 
    node = next;
    if (! (next = node->ln_Succ)) goto FREE_GURU;
    if (! node->TotalSize) goto FREE_GURU;				// Fin de la liste des puddles 
    if ((UBYTE*)mem < (UBYTE*)node+node->mh_OffsLower) goto lC5E8D2;
    if ((UBYTE*)mem >= (UBYTE*)node+node->mh_OffsUpper) goto lC5E8D2;

    deallocate( node, (OOChunk*)mem, realsize );
    pre1 = node->ln_Pred;
    pre2 = pre1->ln_Pred;
    if (!pre2) goto lC5E910;

    pre2->ln_Succ = node;  // Interverti node avec pred 
    node->ln_Pred = pre2;  // Optimisation pour mettre les plus libres en premier ? 
    pre1->ln_Pred = node;
    next = node->ln_Succ;
    pre1->ln_Succ = next;
    next->ln_Pred = pre1;
    node->ln_Succ = pre1;

  lC5E910:
    if ((node->mh_Free + (long)node->mh_OffsLower - (long)node->mh_OffsUpper) == 0)
        { mem = (void*)node; realsize = node->TotalSize; goto lC5E928; } // Libère tout le puddle 
	pool->AllocCount--;
    return NULL;
											
  lC5E924:
    mem = (void*)((ULONG)mem - 12);	// Libère une allocation séparée 
    realsize += 12;

  lC5E928:
    OOList_Remove( (OONode*)mem );
#ifdef _DEBUG
    if (node != NULL && (node->DebugFlags & OOF_DEBUG_FILLFREE)) 
		fill_alloc( (ULONG*)mem, realsize, 0xDEADBEEF );
#endif
    free( mem );
	pool->AllocCount--;
    return NULL;

  FREE_GURU:
	OOASSERT( FALSE );
    OOTRACE( "*** MemPool Warning : Free (3) memory is not own by any puddle\n" );
    return mem;
}

//------------------------------------------------------------ 
// Allocations dans les puddles

static void *allocate( OOPuddle *pdl, ULONG size )
{
  OOChunk *pred;
  OOChunk *next;
  OOChunk *newblock;

    pred = (OOChunk*)&pdl->mh_OffsFirstChunk;

  lF81D9C:
    if (pred->OffsNext == LASTFREECHUNK) return(0); // Fin des chunks libres
    next = (OOChunk*)((UBYTE*)pred + pred->OffsNext);
    if (size <= next->Bytes) goto lF81DB6;
    pred = next;
    goto lF81D9C;

  lF81DB6:
    if (size == next->Bytes) goto lF81DD6;
    newblock = (OOChunk*)((ULONG)next + size);
    pred->OffsNext += size;
	newblock->OffsNext = (next->OffsNext == LASTFREECHUNK) ? LASTFREECHUNK : next->OffsNext - size;
    newblock->Bytes	= next->Bytes - size;
    pdl->mh_Free -= size;
    return( next );

  lF81DD6:
	if (next->OffsNext == LASTFREECHUNK)
		pred->OffsNext = LASTFREECHUNK;
	else
		pred->OffsNext += next->OffsNext;
    pdl->mh_Free -= size;
    return( next );
}

//---------------------- 

static void deallocate( OOPuddle *pdl, OOChunk *mem, ULONG size )
{
  OOChunk *pred;
  OOChunk *next;
  long diff;

    pred = (OOChunk*)&pdl->mh_OffsFirstChunk;
	next = NULL;
    if (pred->OffsNext == LASTFREECHUNK) goto lF81CE8; // Fin des chunks libres
    next = (OOChunk*)((UBYTE*)pred + pred->OffsNext);

  lF81CC6:
    if (mem <= next) goto lF81CD2;
    pred = next;
	next = NULL;
    if (pred->OffsNext == LASTFREECHUNK) goto lF81CD4; // Fin des chunks libres
    next = (OOChunk*)((UBYTE*)pred + pred->OffsNext);
    goto lF81CC6;  // Next loop

  lF81CD2:
    if (mem == next) goto ERROR_lF81D1E; // block already free

  lF81CD4:
    if (pred == (OOChunk*)&pdl->mh_OffsFirstChunk) goto lF81CE8; // Insert block at head of the list 
    diff = ((long)pred + pred->Bytes) - (long)mem;		// Can we join the block to the previous OOChunk ? 
    if (diff == 0) goto lF81CF2;                        // If equal, join the two together 
    if (diff > 0) goto ERROR_lF81D32;                   // If greater, free list is corrupt 

  lF81CE8:      // Enter here to make a new OOChunk and link it into the free list. 
    pred->OffsNext = (UBYTE*)mem - (UBYTE*)pred;
	mem->OffsNext = (next == NULL) ? LASTFREECHUNK : (UBYTE*)next - (UBYTE*)mem;
    mem->Bytes = size;
    goto lF81CF8;

  lF81CF2:      // Enter here to attach the new block to the previous OOChunk. 
    pred->Bytes += size;
    mem = pred;

  lF81CF8:      // It remains to check whether we can join this OOChunk with the next one 
    if (next == NULL) goto lF81D14;
    diff = (long)mem + mem->Bytes - (long)next;
    if (diff > 0) goto ERROR_lF81D34;	// If higher, free list is corrupt 
    if (diff) goto lF81D14;				// If not equal, we can't join them 

	if (next->OffsNext == LASTFREECHUNK)
		mem->OffsNext = LASTFREECHUNK;
	else
		mem->OffsNext += next->OffsNext;
    mem->Bytes += next->Bytes;  // Add to size of our OOChunk 

  lF81D14:
#ifdef _DEBUG
    if (pdl->DebugFlags & OOF_DEBUG_FILLFREE) 
		fill_alloc( ((ULONG*)mem)+2, mem->Bytes-8, 0xDEADBEEF );
#endif
    pdl->mh_Free += size;
  lF81D1C:
    return;

  ERROR_lF81D1E:
	OOASSERT( FALSE );
    OOTRACE( "*** MemPool Warning : block freed twice (adress of a free block)\n" );
    goto lF81D1C;
  ERROR_lF81D32:
	OOASSERT( FALSE );
    OOTRACE( "*** MemPool Warning : Free (1) memory list corrupted\n" );
    goto lF81D1C;
  ERROR_lF81D34:
	OOASSERT( FALSE );
    OOTRACE( "*** MemPool Warning : Free (2) memory list corrupted\n" );
    goto lF81D1C;
}

//-------------------------------------------
//	Routines for debugging

#ifdef _DEBUG

static void verif_chunk( OOChunk *ch )
{
	if (ch == NULL || ch->OffsNext == LASTFREECHUNK || (((ULONG)ch->OffsNext & 1) == 0 && ch->OffsNext < 4200))
		return;
	OOASSERT(FALSE);
}

static void verif( void *pl )
{
  OOPool *pool = (OOPool*)pl;
  OOPuddle *nodepdl;
  OOPuddle *nextpdl;
  OOChunk *pred, *next;

    nextpdl = (OOPuddle*)pool->List.lh_Head;
  lF821F8:
    nodepdl = nextpdl;
    if (! (nextpdl = nodepdl->ln_Succ)) 
		return;
    if (! nodepdl->TotalSize) 
		return;   // Fin de la liste des puddles de mémoire libre 

	pred = (OOChunk*)&nodepdl->mh_OffsFirstChunk;
  lF81D9C:
	if (pred->OffsNext == LASTFREECHUNK) goto lF821F8; // Fin des chunks libres
	next = (OOChunk*)((UBYTE*)pred + pred->OffsNext);
	verif_chunk(next); verif_chunk(pred);
	if ((void*)pred < (void*)nodepdl || (void*)pred > (void*)((ULONG)nodepdl + nodepdl->mh_OffsUpper))
		OOASSERT(FALSE);
	if ((void*)next < (void*)nodepdl || (void*)next > (void*)((ULONG)nodepdl + nodepdl->mh_OffsUpper))
		OOASSERT(FALSE);
	pred = next;
	goto lF81D9C;

	goto lF821F8;
}

static void fill_walls( UBYTE *mem, ULONG memsize )
{
  ULONG i;

    for (i=0; i < 0x10; i++) *mem++ = (UBYTE)0xFB; // ne pas mettre en commentaires pour OOPool_Length 
    mem += memsize;
    for (i=0; i < 0x10; i++) *mem++ = (UBYTE)0xFB;
}

static void verif_walls( UBYTE *mem )
{
  ULONG size, i;
  UBYTE *ptr;

    size = *(ULONG*)(mem-4);
    ptr = mem + 0x10 + size;
    for (i=0; i < 0x10; i++) 
		if (mem[i] != (UBYTE)0xFB || ptr[i] != (UBYTE)0xFB) goto PRINT_ERR;
    return;
  PRINT_ERR:
    OOTRACE( "VerifWalls failed for alloc 0x%08.8lx, size %ld, i=%d\n", mem+0x10, size, i );
    for (i=0; i < 0x10; i++) OOTRACE( "%02.2x ", (UBYTE)mem[i] );
    OOTRACE( " < > " );
    for (i=0; i < 0x10; i++) OOTRACE( "%02.2x ", (UBYTE)ptr[i] );
    OOTRACE( "\n" );
	OOASSERT (FALSE);
	return;
}

static void fill_alloc( ULONG *ptr, ULONG size, ULONG value )
{
    long i, num; 

	for (i=0, num=size/4; i < num; i++) 
		*ptr++ = value;

    switch (size - (4*num))
        {
        case 1: ((UBYTE*)ptr)[0] = ((UBYTE*)&value)[0]; break;
        case 2: ((UBYTE*)ptr)[0] = ((UBYTE*)&value)[0]; ((UBYTE*)ptr)[1] = ((UBYTE*)&value)[1]; break;
        case 3: ((UBYTE*)ptr)[0] = ((UBYTE*)&value)[0]; ((UBYTE*)ptr)[1] = ((UBYTE*)&value)[1]; ((UBYTE*)ptr)[2] = ((UBYTE*)&value)[2]; break;
        }
}

#endif
