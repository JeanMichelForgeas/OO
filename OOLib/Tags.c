/*****************************************************************
 *
 *       Project:    OO
 *       Function:   Tags processing
 *
 *       Created:    05/02/91    Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1997 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_tags.h"
#include "oo_pool.h"
#include "oo_buf.h"

#include "oo_tags_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

OOTagItem *OOTag_NextItem( OOTagItem **ti_addr )
{
  OOTagItem *ti;

    if ((ti = *ti_addr) != NULL)
        {
		OOTAG_GETITEM(ti);
		*ti_addr = ti + 1;
        }
    return ti;
}

//--------------------------------------------------------------------------

BOOL OOTag_InArray( ULONG *tagarray, ULONG tag )
{
	tag &= TAGM_TAGBITS;
    for (; *tagarray != OOV_TAG_END; tagarray++)
        { if (((*tagarray) & TAGM_TAGBITS) == tag) return TRUE; }
    return FALSE;
}

//--------------------------------------------------------------------------

OOTagItem *OOTag_FindTag( OOTagList *ti, ULONG tag )
{
    if (ti != NULL)
        {
		tag &= TAGM_TAGBITS;
        for (; ; ti++)
            {
			OOTAG_GETITEM(ti);
            if (ti == NULL || ti->ti_Tag == tag) break;
            }
        }
    return ti;
}

// Function looks only into the first taglist, without going to ti_More,
//	and scan all tags even those marked as ti_Ignore
OOTagItem *OOTag_FindTagIndex( OOTagList *ti, ULONG tag, ULONG *indexaddr )
{
    if (ti != NULL)
        {
		ULONG i;
		tag &= TAGM_TAGBITS;
        for (i=0; ; ti++, i++)
            {
			if (ti->ti_Tag32 == OOV_TAG_END)
				return NULL; // Do not test for ti_More	for continuation taglist
            if (ti->ti_Tag == tag) break;
            }
		*indexaddr = i;
        }
    return ti;
}

//--------------------------------------------------------------------------

ULONG OOTag_GetData( OOTagList *ti, ULONG tag, ULONG defaultdata )
{
    if ((ti = OOTag_FindTag( ti, tag )) != NULL)
        return ti->ti_Data;
    else
        return defaultdata;
}

//--------------------------------------------------------------------------

OOTagItem *OOTag_SetData( OOTagItem *tl, ULONG tag, ULONG newdata )
{
    if (tl = OOTag_FindTag( tl, tag )) tl->ti_Data = newdata;
    return tl;
}

//--------------------------------------------------------------------------

BOOL OOTag_DataEqual( OOTagItem *t1, OOTagItem *t2 )
{
	OOASSERT( t1 != NULL && t2 != NULL );

	switch (t1->ti_Tag & TAGMASK_TINFO)
		{
		case TAGT_NUM:
		case TAGT_PTR:
			if (t1->ti_Data == t2->ti_Data)
				return TRUE;
			break;

		case TAGT_STR:
		case TAGT_LST:
			if (OOTYPEQUAL(t1,t2))
				{
				ULONG data1 = t1->ti_Data;
				ULONG data2 = t2->ti_Data;
				if (data1 != 0 && data2 != 0)
					{
					ULONG type1 = t1->ti_Tag & TAGMASK_TYPE;
					if (type1 == TAGT_BYTE)
						{
						if (strcmp( (char*)data1, (char*)data2 ) == 0)
							return TRUE;
						}
					else if (type1 == TAGT_WORD)
						{
						UWORD *p1 = (UWORD*)data1, *p2 = (UWORD*)data2;
						for (; *p1 && *p2; p1++, p2++)
							if (*p1 != *p2)
								return FALSE;
						return TRUE;
						}
					else if (type1 == TAGT_LONG)
						{
						ULONG *p1 = (ULONG*)data1, *p2 = (ULONG*)data2;
						for (; *p1 && *p2; p1++, p2++)
							if (*p1 != *p2)
								return FALSE;
						return TRUE;
						}
					}
				}
			break;
		}

	return FALSE;
}

//--------------------------------------------------------------------------

ULONG OOTag_FilterList( OOTagItem *ti, ULONG *tagarray, ULONG logic )
{
  ULONG num=0;

    if (ti != NULL)
        {
        for (; ; ti++)
            {
			OOTAG_GETITEM(ti);
			if (ti == NULL) 
				break;
            if (OOTag_InArray( tagarray, ti->ti_Tag ))
                { if (logic == OOV_FILTER_NOT) ti->ti_Ignore = 1; else num++; }
            else
                { if (logic == OOV_FILTER_NOT) num++; else ti->ti_Ignore = 1; }
            }
        }
    return num;
}

//--------------------------------------------------------------------------

static ULONG each_FilterChanges( OOTagItem *change_ti, OOTagItem *old_ti, ULONG apply )
{
    if (old_ti)
        {
        if (OOTag_DataEqual( change_ti, old_ti ) == TRUE) { change_ti->ti_Ignore = 1; }
        else if (apply == OOV_FILTER_UPDATE) { old_ti->ti_Data = change_ti->ti_Data; }
        }
    else{
        change_ti->ti_Ignore = 1;
        }
    return( apply );
}

// ** Eliminate tags which specify no change. This function goes through change_tl.
// **
// ** For each item found in change_tl, if the item is also present in original list,
// ** and their data values are identical, then the tag is removed from change_tl.
// ** If the two tag's data values are different and the 'apply' value is
// ** OOV_UPDATE_ORIGIN, then the tag data in original list will be updated to match
// ** the value from change_tl.

void OOTag_FilterChanges( OOTagList *change_tl, OOTagList *oldvalues, ULONG apply )
{
    OOTag_ApplyFind( change_tl, oldvalues, each_FilterChanges, apply );
}

//--------------------------------------------------------------------------

ULONG OOTag_ApplyList( OOTagList *tl, ULONG (*Func)(OOTagItem*,ULONG), ULONG funcparm, ULONG testrc, ULONG testvalue )
{
  ULONG retcode = 0;

    if (tl != NULL)
        {
        for (; ; tl++)
            {
			OOTAG_GETITEM(tl);
			if (tl == NULL) 
				break;
            retcode = (*Func)( tl, funcparm );
            if (testrc && retcode == testvalue) 
				break;
            }
        }
    return retcode;
}

//--------------------------------------------------------------------------

ULONG OOTag_ApplyFind( OOTagList *ti, OOTagList *find_tl, ULONG (*Perform)(OOTagList*,OOTagItem*,ULONG), ULONG parm )
{
    if (ti != NULL)
        {
		OOTagList *found_ti;
        for (; ; ti++)
            {
			OOTAG_GETITEM(ti);
	        if (ti == NULL) 
				break;
            found_ti = OOTag_FindTag( find_tl, ti->ti_Tag );
            parm = (*Perform)( ti, found_ti, parm );    // found_ti is null if ti->ti_Tag was not found
            }
        }
    return( parm );
}

//--------------------------------------------------------------------------

static ULONG each_PackBool( OOTagItem *ti, OOTagItem *found_ti, ULONG flags )
{
    if (found_ti != NULL) 
		{ if (ti->ti_Data == TRUE) flags |= found_ti->ti_Data; else flags &= ~found_ti->ti_Data; }
    return( flags );
}

ULONG OOTag_PackBool( OOTagList *tl, OOTagList *boolmap, ULONG initial_flags )
{
    return( OOTag_ApplyFind( tl, boolmap, each_PackBool, initial_flags ) );
}

//--------------------------------------------------------------------------

static ULONG each_ApplyChanges( OOTagItem *ti, OOTagItem *change_ti, ULONG parm )
{
    if (change_ti != NULL) 
		{ ti->ti_Data = change_ti->ti_Data; }
    return(0);
}

// ** Change a tag list based on a second tag list.
// **
// ** For any tag that appears in both 'tl' and 'change_tl', this
// ** function will change the ti_Data field of the tag in 'tl' to
// ** match the ti_Data field of the tag in 'change_tl'. In effect,
// ** 'change_tl' contains a series of new values for tags already in
// ** 'tl'. Any tag in 'change_tl' that is not in 'tl' is ignored.

void OOTag_ApplyChanges( OOTagList *tl, OOTagList *change_tl )
{
    OOTag_ApplyFind( tl, change_tl, each_ApplyChanges, 0 );
}

//--------------------------------------------------------------------------

static ULONG each_MapTags( OOTagItem *ti, OOTagItem *mi, ULONG include_miss )
{
    if (mi) { ti->ti_Tag32 = mi->ti_Data; }
    else if (include_miss == OOV_MAP_REMOVE_NOTFOUND) { ti->ti_Ignore = 1; }
    return include_miss;
}

void OOTag_MapTags( OOTagItem *tl, OOTagItem *maplist, ULONG include_miss )
{
    OOTag_ApplyFind( tl, maplist, each_MapTags, include_miss );
}

//--------------------------------------------------------------------------

void OOTag_Validate( OOTagList *ti )
{
    if (ti != NULL)
        {
        for (; ; ti++)
            {
			if (ti->ti_Tag32 == OOV_TAG_END)
				{ if ((ti = ti->ti_More) == NULL) break; }
			ti->ti_Ignore = 0;
            }
        }
}

//--------------------------------------------------------------------------

OOTagList *OOTag_Alloc( ULONG numitems )
{
  OOTagList *taglist;

	OOASSERT( OOGlobalPool != NULL );
    if ((taglist = (OOTagList*) OOPool_Alloc( OOGlobalPool, (numitems + 1) * sizeof(OOTagItem) )) != NULL)
		{
        memset( taglist, 0, (numitems) * sizeof(OOTagItem) );
		taglist[numitems].ti_Tag32 = OOV_TAG_END;
		taglist[numitems].ti_More = NULL;
		}
    return taglist;
}

//--------------------------------------------------------------------------

void OOTag_Free( OOTagList *taglist )
{
    if (taglist != NULL) 
		OOPool_Free( OOGlobalPool, taglist );
}

//--------------------------------------------------------------------------

ULONG OOTag_GetCount( OOTagList *tl )
{
  ULONG num;

    for (num=0; ; num++, tl++)
		{
		OOTAG_GETITEM(tl);
        if (tl == NULL) 
			break;
		}
    return num;
}

//--------------------------------------------------------------------------/

OOTagList *OOTag_Clone( OOTagList *tl_src )
{
  OOTagList *tl_dest;

    if ((tl_dest = OOTag_Alloc( OOTag_GetCount( tl_src ) )) != NULL)
        OOTag_Copy( tl_src, tl_dest );
    return tl_dest ;
}

//--------------------------------------------------------------------------

void OOTag_Copy( OOTagList *ti, OOTagList *dest_tl )
{
	OOASSERT( dest_tl != NULL );
    if (ti != NULL)
		{
        for (; ; )
            {
			OOTAG_GETITEM(ti);
			if (ti == NULL) 
				break;
            *dest_tl++ = *ti++;
            }
		}
	dest_tl->ti_Tag32 = OOV_TAG_END;
	dest_tl->ti_Ptr	= NULL;
}

//--------------------------------------------------------------------------

OOTagList *OOTag_Merge( OOTagList *tl, OOTagList *Next )
{
  ULONG num1, num2;
  OOTagItem *ti;

    num1 = OOTag_GetCount( tl );
    num2 = OOTag_GetCount( Next );
    if (ti = OOTag_Alloc( num1 + num2 ))
        {
        if (num1) OOTag_Copy( tl, ti );
        if (num2) OOTag_Copy( Next, ti + num1 );
        }
    return ti;
}

//--------------------------------------------------------------------------

OOTagItem *OOTag_Join( OOTagList *ti, OOTagList *tl2 )
{
    for (; ; ti++)
        {
        if (ti->ti_Tag32 == OOV_TAG_END)
            {
			if (ti->ti_More != NULL)
				{
				if (ti->ti_More == tl2)
					break;
				ti = ti->ti_More;
				}
			else{
				ti->ti_Data = (ULONG)tl2;
				break;
				}
            }
        }
	return ti;
}

OOTagList *OOTag_Split( OOTagItem *ti )
{
	OOTagList *tl = ti->ti_More;
	OOASSERT( ti->ti_Tag32 == OOV_TAG_END );
	ti->ti_More = NULL;
	return tl;
}

//--------------------------------------------------------------------------

UBYTE * OOTag_SetString( OOTagItem *ti, UBYTE *str )
{
	UBYTE *mem = NULL;
	if (str != NULL && *str != 0)
		{
		if (ti->ti_String == NULL || strcmp( ti->ti_String, str ) != 0)
			{
			ULONG len = strlen( str );
			if ((mem = OOBuf_Alloc( OOGlobalPool, len, 8 )) != NULL)
				{
				strcpy( mem, str );
				if (OOTAGISBUF(ti) && ti->ti_String != NULL) 
					OOBuf_Free( &ti->ti_String );
				ti->ti_String = mem;
				ti->ti_IsBuf = 1;
				}
			}
		}
	return mem;
}

//--------------------------------------------------------------------------

ULONG OOTag_FreeBuffers( OOTagList *tl )
{
  ULONG retcode = 0;

    if (tl != NULL)
        {
        for (; ; tl++)
            {
			OOTAG_GETITEM(tl);
			if (tl == NULL) 
				break;

			if (OOTAGISBUF(tl) && tl->ti_Ptr != NULL)
				{
				OOBuf_Free( &tl->ti_Ptr );
				tl->ti_IsBuf = 0;
				}
            }
        }
    return retcode;
}
