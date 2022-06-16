/*****************************************************************
 *
 *       Project:    OO
 *       Function:   Array processing
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       CopyRight © 1991-1997 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_array.h"
#include "oo_pool.h"

#include "oo_array_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

static ULONG compute_new_entries( ULONG numentries, ULONG blkentries ) /* résultat toujours > numentries */
{
    return ((numentries + blkentries) / blkentries) * blkentries;
}

ULONG *OOArray_Alloc( void *pool, ULONG numentries, ULONG blkentries )
{
  ULONG totsize, totentries;
  OOArray *ooar;

    if (blkentries < 4) blkentries = 4;
    totentries = compute_new_entries( numentries, blkentries );
	totsize = totentries * sizeof(ULONG);

	OOASSERT( OOGlobalPool != NULL );
	if (pool == NULL) pool = OOGlobalPool;
    if ((ooar = (OOArray*) OOPool_Alloc( pool, sizeof(OOArray)+totsize )) != NULL)
        {
        ooar->Pool		 = (OOPool*)pool;
        ooar->TotEntries = totentries;
        ooar->CurEntries = numentries;
        ooar->BlkEntries = blkentries;
        O2A(ooar)[numentries] = 0;
        return O2A(ooar);
        }
    return NULL;
}

static void ooarray_free( ULONG *ar )
{
    if (ar)
		{
		OOArray *ooar = A2O(ar);
        OOPool_Free( ooar->Pool, ooar );
		}
}

void OOArray_Free( ULONG **pmem )
{
	ooarray_free( *pmem );
    *pmem = 0;
}

//------------------------------------------------------------------

ULONG OOArray_GetCount( ULONG **add_array )
{
	OOASSERT( *add_array != NULL );
	if (*add_array == NULL)
		return 0;
	return A2O(*add_array)->CurEntries;
}

static ULONG *resize( ULONG *ar, ULONG newentries )
{
  ULONG oldentries, newtotsize, newtotentries;
  ULONG copylen;
  OOArray *ooar;

    if (! ar) return NULL;

    newtotentries = compute_new_entries( newentries, A2O(ar)->BlkEntries );
    if (newtotentries == A2O(ar)->TotEntries) goto INPLACE;

	newtotsize = newtotentries * sizeof(ULONG);
	OOASSERT( OOGlobalPool != NULL );
    if ((ooar = (OOArray*) OOPool_Alloc( A2O(ar)->Pool, sizeof(OOArray)+newtotsize )) == NULL)
        { if (newentries >= A2O(ar)->TotEntries) return NULL; else goto INPLACE; } // if no memory, leave entire buffer
    ooar->Pool		 = A2O(ar)->Pool;
    ooar->TotEntries = newtotentries;
    ooar->CurEntries = A2O(ar)->CurEntries;
    ooar->BlkEntries = A2O(ar)->BlkEntries;

	oldentries = A2O(ar)->CurEntries;
    copylen = MIN(newentries,oldentries) * sizeof(ULONG);
    if (copylen) 
		memmove( O2A(ooar), ar, copylen );
	if (newentries > oldentries)
		memset( &O2A(ooar)[oldentries], 0, (newentries-oldentries)*sizeof(ULONG) ); 
    O2A(ooar)[newentries] = 0;

    ooarray_free( ar );
    return O2A(ooar);

  INPLACE:
    ar[newentries] = 0;
    return ar;
}

// Returns new entry count, or old if not successfull
ULONG OOArray_SetCount( ULONG **add_array, ULONG numentries )
{
	OOArray	*ooar;
	ULONG *ptr;

	OOASSERT( *add_array != NULL );
	ooar = A2O(*add_array); // First test for speed
	if (ooar->TotEntries == compute_new_entries( numentries, ooar->BlkEntries ))
		{
	    ooar->CurEntries = numentries;
		O2A(ooar)[numentries] = 0;
		return numentries;
		}
	if ((ptr = resize( *add_array, numentries )) != NULL) *add_array = ptr;
	if (*add_array == NULL)return 0;
    A2O(*add_array)->CurEntries = numentries;
	return numentries;
}

//------------------------------------------------------------------

ULONG OOArray_Get( ULONG **add_array, ULONG index )
{
	OOASSERT( *add_array != NULL );
	return (*add_array)[index];
}

ULONG OOArray_Set( ULONG **add_array, ULONG index, ULONG data )
{
	ULONG *p, old;

	OOASSERT( *add_array != NULL );
	p = &(*add_array)[index];
	old = *p;
	*p = data;
	return old;
}

//------------------------------------------------------------------

// Returns the num of entries
ULONG OOArray_Insert( ULONG **add_array, ULONG index, ULONG data )
{
  ULONG *ar, oldentries, newentries;

	OOASSERT( *add_array != NULL );
	ar = *add_array;
    oldentries = A2O(ar)->CurEntries;
    if (index == MAXULONG) index = oldentries;
    newentries = 1 + ((index > oldentries) ? index : oldentries);
    if ((ar = resize( ar, newentries )) == NULL) return 0;
    if (index < oldentries)      // décale le bout
        memmove( &ar[index+1], &ar[index], (oldentries-index)*sizeof(ULONG) );
	//else if (index > oldentries)
	//	memset( &ar[oldentries], 0, (index-oldentries)*sizeof(ULONG) ); 
    ar[index] = data;
	*add_array = ar;
    A2O(ar)->CurEntries = newentries;
    return A2O(ar)->CurEntries;
}

// Returns the num of entries
ULONG OOArray_AddTail( ULONG **add_array, ULONG data )
{
  ULONG *ar, index;

	OOASSERT( *add_array != NULL );
	ar = *add_array;
    index = A2O(ar)->CurEntries;
	if (A2O(ar)->TotEntries != compute_new_entries( index+1, A2O(ar)->BlkEntries ))
		{
		if ((ar = resize( ar, index+1 )) == NULL) return 0;
		*add_array = ar;
		}
    ar[index++] = data;
    A2O(ar)->CurEntries = index;
    return index;
}

//------------------------------------------------------------------

// Returns the num of entries
ULONG OOArray_Remove( ULONG **add_array, ULONG index )
{
  ULONG *ar, num, oldentries;

	OOASSERT( *add_array != NULL );
	ar = *add_array;
	if ((oldentries = A2O(ar)->CurEntries) == 0) goto END_ERROR;

    if (index == MAXULONG) index = oldentries - 1;
    else if (index >= oldentries) goto END_ERROR; // Rien à couper
    if (num = oldentries - index - 1) // Ce qui reste au bout
        memmove( &ar[index], &ar[+index+1], num*sizeof(ULONG) ); // comble le vide sur place
    if ((ar = resize( ar, oldentries-1 )) == NULL) return 0;
	*add_array = ar;
	A2O(ar)->CurEntries--;
  END_ERROR:
    return A2O(ar)->CurEntries;
}

ULONG OOArray_RemoveTail( ULONG **add_array )
{
  ULONG *ar, oldentries;

	ar = *add_array;
	OOASSERT( ar != NULL );
	if ((oldentries = A2O(ar)->CurEntries) == 0) goto END_ERROR;
    ar = resize( ar, oldentries-1 ); // Shrink resize cannot fail
	*add_array = ar;
	A2O(ar)->CurEntries--;
  END_ERROR:
    return A2O(ar)->CurEntries;
}
