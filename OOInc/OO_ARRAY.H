#ifndef OO_ARRAY_H
#define OO_ARRAY_H

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

ULONG *			OOArray_Alloc		( void *pool, ULONG numentries, ULONG blkentries );
void			OOArray_Free		( ULONG **add_array );
ULONG			OOArray_GetCount	( ULONG **add_array );
ULONG			OOArray_SetCount	( ULONG **add_array, ULONG numentries );
ULONG			OOArray_Get			( ULONG **add_array, ULONG index );
ULONG			OOArray_Set			( ULONG **add_array, ULONG index, ULONG data );
ULONG			OOArray_Insert		( ULONG **add_array, ULONG index, ULONG data );
ULONG			OOArray_AddTail		( ULONG **add_array, ULONG data );
ULONG			OOArray_Remove		( ULONG **add_array, ULONG index );
ULONG			OOArray_RemoveTail	( ULONG **add_array );

#ifdef __cplusplus
	}
#endif

#endif // OO_ARRAY_H
