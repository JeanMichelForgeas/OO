
#include "oo_class.h"
#include "oo_pool.h"
#include "oo_gui.h" // pour OOM_AREA_SIGNALCHANGE

#include "oo_class_prv.h"

//-------------------------------------------------------------

OObject * OO_NewV( OOClass *cl, ULONG attr1, ... )
{
    return OO_New( cl, (OOTagList*)&attr1 );
}

OObject * OO_New( OOClass *cl, OOTagList *attrlist )
{
    return (OObject*) (*cl->Dispatcher)( cl, (OObject*)cl, OOM_NEW, attrlist );
}

ULONG OO_Delete( OObject *obj )
{
	OOASSERT (obj != NULL);
    return (ULONG) (*obj->Class->Dispatcher)( obj->Class, obj, OOM_DELETE, NULL );
}

//---------------------------------------------------------

ULONG OO_DoMethodV( OObject *obj, ULONG mid, ULONG attr1, ... )
{
    return (*obj->Class->Dispatcher)( obj->Class, obj, mid, (OOTagList*)&attr1 );
}

ULONG OO_DoMethod( OObject *obj, ULONG mid, OOTagList *tl )
{
    return (*obj->Class->Dispatcher)( obj->Class, obj, mid, tl );
}

//----------------

ULONG OO_DoClassMethodV( OOClass *cl, OObject *obj, ULONG mid, ULONG attr1, ... )
{
    return (*cl->Dispatcher)( cl, obj, mid, (OOTagList*)&attr1 );
}

ULONG OO_DoClassMethod( OOClass *cl, OObject *obj, ULONG mid, OOTagList *tl )
{
    return (*cl->Dispatcher)( cl, obj, mid, tl );
}

//----------------

ULONG OO_DoBaseMethodV( OOClass *cl, OObject *obj, ULONG mid, ULONG attr1, ... )
{
    return (*cl->BaseClass->Dispatcher)( cl->BaseClass, obj, mid, (OOTagList*)&attr1 );
}

ULONG OO_DoBaseMethod( OOClass *cl, OObject *obj, ULONG mid, OOTagList *tl )
{
    return (*cl->BaseClass->Dispatcher)( cl->BaseClass, obj, mid, tl );
}

//----------------

ULONG OO_DoChildrenMethodV( ULONG flags, OObject *obj, ULONG mid, ULONG attr1, ... )
{
	return OO_DoChildrenMethod( flags, obj, mid, (OOTagList*)&attr1 );
}

// Objects are taken from end to start in case a child should be detached
//	by the method called, so its pointer should be removed and other moved
ULONG OO_DoChildrenMethod( ULONG flags, OObject *obj, ULONG mid, OOTagList *tl )
{
	ULONG i=0, count = obj->ChildrenTable.Count;
	OObject *child, **tab = obj->ChildrenTable.Table + (count-1);

	// Loop to send method
	if (flags & OOF_TESTCHILD)
		{
		for	(; i < count; i++)
			{
			child = *tab--;
			if ((child->Class->Dispatcher)( child->Class, child, mid, tl ) == OOV_ERROR)
				return OOV_ERROR;
			}
		}
	else{
		for	(; i < count; i++)
			{
			child = *tab--;
			(child->Class->Dispatcher)( child->Class, child, mid, tl );
			}
		}
    return OOV_OK;
}

//----------------------------------------------------------------------------

static ULONG OOAr_VerifResize( OObjArray *oa, ULONG blockksize )
{
	OObject **ptr;

	if (! (ptr = (APTR) OOPool_Resize( OOGlobalPool, (UBYTE*)oa->Table, (oa->Max + blockksize) * sizeof(APTR) ))) return OOV_ERROR;
	oa->Table = ptr;
	oa->Max += (UWORD)blockksize;
	return OOV_OK;
}

void OOAr_Init( OObjArray *oa, BOOL table )
{
	if (table == TRUE) { oa->Max = 0; oa->Table = NULL; }
	oa->Count = 0;
}

void OOAr_Cleanup( OObjArray *oa )
{
	OOPool_Free( OOGlobalPool, oa->Table );	
	oa->Count = 0;
	oa->Max = 0; 
	oa->Table = NULL;
}

ULONG OOAr_Add( OObjArray *oa, OObject *obj, ULONG blockksize )
{
	if (oa->Count >= oa->Max) //+++ optim : mettre OOAr_VerifResize() en ligne
		if (OOAr_VerifResize( oa, blockksize ) == OOV_ERROR) return OOV_ERROR;
	oa->Table[oa->Count++] = obj;
	return OOV_OK;
}

// Pour une table sans trou : les pointeurs sont décalés
void OOAr_Remove( OObjArray *oa, OObject *obj )
{
	ULONG i;

	for (i=0; i < oa->Count; i++)
		{
		if (oa->Table[i] == obj)
			{ 
			oa->Count--; 
			for (; i < oa->Count; i++)
				oa->Table[i] = oa->Table[i+1];
			break;	
			}
		}
}

// Pour une table sans décalage : on laisse des trous à NULL
void OOAr_Sweep( OObjArray *oa, OObject *obj )
{
	ULONG i;

	for (i=0; i < oa->Count; i++)
		{
		if (oa->Table[i] == obj)
			{ 
			oa->Table[i] = NULL;
			break;	
			}
		}
}

//---------------------------------

ULONG OO_AttachTarget( OObject *obj, void *target )
{
	OOASSERT( obj != NULL && target != NULL);

	return OOAr_Add( &obj->TargetsTable, target, OOV_TARGETLIST_BLOCKSIZE );
}

void OO_RemoveTarget( OObject *obj, void *target )
{
	OOASSERT( obj != NULL && target != NULL);

	OOAr_Remove( &obj->TargetsTable, target );
	OOPool_Free( OOGlobalPool, target );
}

void OO_RemoveTargets( OObject *obj )
{
	ULONG i, count = obj->TargetsTable.Count;
	OObject **tab = obj->TargetsTable.Table;

	OOASSERT( obj != NULL);
	for	(i=0; i < count; i++)
		OOPool_Free( OOGlobalPool, tab[i] );
	OOAr_Cleanup( &obj->TargetsTable );
}

//---------------------------------

ULONG OO_AttachParent( OObject *child, OObject *parent )
{
	OOASSERT( parent != NULL && child != NULL);

	if (OOAr_Add( &parent->ChildrenTable, child, OOV_CHILDLIST_BLOCKSIZE ) == OOV_ERROR)
		return OOV_ERROR;

	if (OOAr_Add( &child->ParentsTable, parent, OOV_PARENTLIST_BLOCKSIZE ) == OOV_ERROR)
		{
		OOAr_Remove( &parent->ChildrenTable, child );
		return OOV_ERROR;
		}

	return OOV_OK;
}

void OO_DetachParent( OObject *child, OObject *parent )
{
	OOASSERT( parent != NULL && child != NULL);

	OOAr_Remove( &parent->ChildrenTable, child );
	OOAr_Remove( &child->ParentsTable, parent );
}

void OO_DetachParents( OObject *obj )
{
	ULONG i, count = obj->ParentsTable.Count;
	OObject **tab = obj->ParentsTable.Table;

	OOASSERT( obj != NULL);
	for	(i=0; i < count; i++)
		OOAr_Remove( &tab[i]->ChildrenTable, obj );
	OOAr_Cleanup( &obj->ParentsTable );
}

void OO_DetachChildren( OObject *obj )
{
	ULONG i, count = obj->ChildrenTable.Count;
	OObject **tab = obj->ChildrenTable.Table;

	OOASSERT( obj != NULL);
	for	(i=0; i < count; i++)
		OOAr_Remove( &tab[i]->ParentsTable, obj );
	OOAr_Cleanup( &obj->ChildrenTable );
}

void OO_DetachAll( OObject *obj )
{
	OOASSERT( obj != NULL);
	OO_DetachParents( obj );
	OO_DetachChildren( obj );
}

//----------------------------------------------------------------------------

ULONG OO_SetAttrsV( OObject *obj, ULONG attr, ... )
{
	OOASSERT( obj != NULL);
    return (*obj->Class->Dispatcher)( obj->Class, obj, OOM_SET, (OOTagList*)&attr );
}

ULONG OO_SetAttrs( OObject *obj, OOTagItem *taglist )
{
	OOASSERT( obj != NULL);
    return (*obj->Class->Dispatcher)( obj->Class, obj, OOM_SET, taglist );
}

ULONG OO_SetAttr( OObject *obj, ULONG tag, ULONG data )
{
	OOTagItem taglist[2] = { tag, data, TAG_END };

	OOASSERT( obj != NULL);
    return (*obj->Class->Dispatcher)( obj->Class, obj, OOM_SET, taglist );
}

ULONG OO_GetAttrsV( OObject *obj, ULONG attr, ... )
{
	OOASSERT( obj != NULL);
    return (*obj->Class->Dispatcher)( obj->Class, obj, OOM_GET, (OOTagList*)&attr );
}

ULONG OO_GetAttrs( OObject *obj, OOTagList *taglist )
{
	OOASSERT( obj != NULL);
    return (*obj->Class->Dispatcher)( obj->Class, obj, OOM_GET, taglist );
}

ULONG OO_GetAttr( OObject *obj, ULONG tag )
{
	OOTagItem taglist[2] = { tag, 0, TAG_END };
	ULONG result;

	OOASSERT( obj != NULL);
	taglist[0].ti_Data = (ULONG)&result;
    (*obj->Class->Dispatcher)( obj->Class, obj, OOM_GET, taglist );
	return result;
}

//----------------------------------------------------------------------------

void *OO_AddNotify( OObject *obj, ULONG trigtype, ULONG attr, ULONG min, ULONG max,
					OObject **pdest, ULONG actiontype,
					ULONG mid, OOTagList *taglist )
{
  OOTarget *target;
  ULONG size, numtags;

	numtags = (taglist == NULL)	? 0 : OOTag_GetCount( taglist );
    size = sizeof(OOTarget) + (sizeof(OOTagItem)*numtags);
	OOASSERT( OOGlobalPool != NULL );
    if (target = (OOTarget*) OOPool_Alloc( OOGlobalPool, size ))
        {
        target->TrigType    = (UBYTE)trigtype;
        target->ActionType  = (UBYTE)actiontype;
        target->SrcAttr     = attr;
        target->Min         = min;
        target->Max         = max;
        target->PDest       = (pdest == (APTR)OOV_NTOBJITSELF ? (APTR)OOV_NTOBJITSELF : pdest);
        target->MethodId    = mid;
		OOTag_Copy( taglist, target->TagList ); // Met TAG_END même si taglist==NULL
		if (OO_AttachTarget( obj, target ) != OOV_OK)
			target = OOPool_Free( OOGlobalPool, target );
        }
    return( target );
}

void *OO_AddNotifyV( OObject *obj, ULONG trigtype, ULONG attr, ULONG min, ULONG max,
					 OObject **pdest, ULONG actiontype,
					 ULONG mid, ULONG tagattr, ... )
{
	return OO_AddNotify( obj, trigtype, attr, min, max, pdest, actiontype, mid, (OOTagList*)&tagattr );
}

void *OO_AddNotifyItem( OObject *obj, OONotifyItem *ni, APTR ptrbase )
{
	return OO_AddNotify( obj, 
					(UBYTE)ni->TrigType, 
					ni->SrcAttr, 
					ni->Min, 
					ni->Max, 
					(ni->DestVar == OOV_NTOBJITSELF ? (OObject**)OOV_NTOBJITSELF : (OObject**)((ULONG)ptrbase + ni->DestVar)), 
					(UBYTE)ni->ActionType, 
					ni->MethodId, 
					(OOTagList*)&ni->Attr1 
					);
}

ULONG OO_DoNotifyV( OObject *obj, ULONG attr1, ... )
{
    return( OO_DoNotify( obj, (OOTagList*)&attr1 ) );
}

// !!! Clear the tmp flag of tagitems which must notify before calling this function
// This allow the temporary modification of input taglist without destroying items 
// nor having to clone/delete a taglist during the notify process.
ULONG OO_DoNotify( OObject *obj, OOTagList *taglist )
{
	ULONG count = obj->TargetsTable.Count;

    // On permet à l'objet de se notifier lui-même,
    // mais pas à un autre de le notifier s'il est l'initiateur.
    // C'est pourquoi le test est fait avant la mise du flag.
    if (count && !(obj->Flags & OOF_NOTIFYING) && taglist != NULL)
        {
		OOTagItem *ti;
		OOTarget *target, **tab = (OOTarget**)obj->TargetsTable.Table;
		OObject *dest;
		ULONG res, i;
		BOOL ok;

        obj->Flags |= (OOF_NOTIFYING|OOF_DONOTDELETE);
        for (ti=taglist; ; ti++)
            {
			// skip tags with flag ignore or tmp set, as function OOPrv_MSet())
			OOTAG_GETITEM_TMP(ti);
	        if (ti == NULL) 
				break;

			// Loop on each target
			for	(i=0; i < count && (target = tab[i]) != NULL; i++)
                {
                if (ti->ti_Tag == (target->SrcAttr & TAGM_TAGBITS)) // attribut déclencheur ?
                    {
                    ok = FALSE;
					//---------------- Trigger type
                    if (target->TrigType == OON_TRIG_EVERYVALUE)
                        {
                        ok = TRUE;
                        }
                    else{
                        switch (target->TrigType)
                            {
                            case OON_TRIG_EQUAL: // If strings, strcmp is used()
								ok = OOTag_DataEqual( ti, (OOTagItem*)&target->SrcAttr );
                                break;
                            case OON_TRIG_RANGE: // Must be a number
								if ((ti->ti_Tag & TAGMASK_TINFO) != TAGT_NUM) 
									{ ok = FALSE; }
								else if (OOTAGISFLT(ti))
									{ if (ti->ti_Float >= *(FLOAT*)&target->Min && ti->ti_Float <= *(FLOAT*)&target->Max) ok = TRUE; }
								else if (ti->ti_Tag & TAGMASK_SIGNED)
									{ if (((long)ti->ti_Data) >= ((long)target->Min) && ((long)ti->ti_Data) <= ((long)target->Max)) ok = TRUE; }
								else
									{ if (ti->ti_Data >= target->Min && ti->ti_Data <= target->Max) ok = TRUE; }
                                break;
                            }
                        }

					//---------------- Resulting action type
                    if (ok == TRUE)
                        {
						OOTagItem *to = target->TagList;
						dest = (target->PDest == (APTR)OOV_NTOBJITSELF ? obj : *(target->PDest));

                        switch (target->ActionType)
                            {
                            case OON_ACT_CALLFUNC:
                                {
                                ULONG (*func)( OObject *srcobj, ULONG attr, ULONG val, ULONG min, ULONG max, OObject *destobj, OOTagItem *taglist );
                                if (func = (APTR)target->MethodId)
                                    if (func( obj, target->SrcAttr, ti->ti_Data, target->Min, target->Max, dest, target->TagList ) != OOV_OK)
                                        goto END_NOTIFY; // Erreur ou volonté d'arrêt de la notification
                                }
                                break;

                            case OON_ACT_COPYSTR:
                                if (ti->ti_Data) strcpy( (UBYTE*)to->ti_Tag32, (UBYTE*)ti->ti_Data );
                                break;

                            case OON_ACT_FORMATSTR:
                                sprintf( (UBYTE*)to->ti_Tag32, (UBYTE*)to->ti_Data, ti->ti_Data );
                                break;

                            case OON_ACT_COPYCONST:
                                *((ULONG*)to->ti_Tag32) = to->ti_Data;
                                break;

                            case OON_ACT_COPYVALUE:
                                *((ULONG*)to->ti_Tag32) = ti->ti_Data;
                                break;

                            default:
                                if (target->ActionType == OON_ACT_DOMETHODVALUE)
                                    {
                                    if (OOTAGISFLT(ti))	//---------- Arrive Tag == FLOAT
                                        {
                                        if (OOTAGISFLT(to)) to->ti_Float = ti->ti_Float;
                                        else to->ti_Data = (SLONG) ti->ti_Float; // IEEESPFix( *(FLOAT*)&ti->ti_Data );
                                        }
                                    else				//---------- Arrive Tag == ULONG
                                        {
                                        if (OOTAGISFLT(to)) to->ti_Float = (FLOAT) ti->ti_Data; // IEEESPFlt( ti->ti_Data );
                                        else to->ti_Data = ti->ti_Data;
                                        }
                                    }
                                if (dest != NULL)
                                    {
                                    if (! (dest->Flags & OOF_BREAKNOTIFY))
                                        {
										// Si le flag est déjà positionné : on est imbriqué dans DoNotify
										BOOL can_delete = (dest->Flags & OOF_DONOTDELETE) ? FALSE : TRUE;
                                        dest->Flags |= OOF_DONOTDELETE;
                                        res = OO_DoMethod( dest, target->MethodId, target->TagList );

										if (can_delete == TRUE) 
											{ if (dest != obj) dest->Flags &= ~OOF_DONOTDELETE; }

										if (res == OOV_ERROR) goto END_NOTIFY; // Erreur sur une action ou méthode inconnue pour cette class

										if (can_delete == TRUE)
											{
											if (dest != obj && (dest->Flags & OOF_DELETED)) // Seules les classes dérivées ont exécuté OOM_DELETE
												{                           // à cause du flag OOF_DONOTDELETE
												OO_DoClassMethod( &Class_Root, dest, OOM_DELETE, NULL );
												return((ULONG)-1);
												}
											if (obj->Flags & OOF_DELETED) // Seules les classes dérivées ont exécuté OOM_DELETE
												{                           // à cause du flag OOF_DONOTDELETE
												obj->Flags &= ~OOF_DONOTDELETE;
												OO_DoClassMethod( &Class_Root, obj, OOM_DELETE, NULL );
												return((ULONG)-1);
												}
											}
                                        }
                                    }
                                break;
                            }
                        }
                    }
                }
            }
		END_NOTIFY: obj->Flags &= ~(OOF_NOTIFYING|OOF_DONOTDELETE);
        return 1;
        }
    return 0;
}

//--------------------------------------------------------------------------

struct _IsId {
    OObject *After;
    ULONG    Res;
    ULONG    Parm;
    ULONG    Flags;
    ULONG    (*Func)(void*,...);
    };

static ULONG parse_children( OObject *obj, struct _IsId *isid )
{
	ULONG i, count = obj->ChildrenTable.Count;
	OObject *child, **tab = obj->ChildrenTable.Table;

	// Loop to send method, start at end in case of delete
	for	(i=0, tab+=(count-1); i < count; i++)
		{
		child = *tab--;
		if (isid->Flags & 1)
			{
			if (isid->Res = isid->Func( child, isid->Parm )) return 0; // Arrêt de la recherche
			}
		else{
			if (isid->After == child) isid->Flags = 1; // On commence a appeler la fonction
			}
		if (parse_children( child, isid ) == 0) return 0;
		}
    return 1;
}

ULONG OO_ParseTree( OObject *root,  OObject *after, ULONG (*func)(void*,...), ULONG parm )
{
  struct _IsId ISID;

    ISID.After = after;
    ISID.Res   = 0;
    ISID.Parm  = parm;
    ISID.Flags = (after == NULL) ? 1 : 0;
    ISID.Func  = func;

    parse_children( root, &ISID );

    return ISID.Res;
}

static OObject *new_treea( OObject *parent, APTR ptrbase, OOTreeItem *it, SWORD startlevel )
{
  OObject *obj=0;
  SWORD level;

    level = it->Level;
    while (TRUE)
        {
        if (it->Level == level)
            {
            if (it->Var != OOV_NOOFFS && *((ULONG*)((ULONG)ptrbase + it->Var))) // objet déjà créé ?
                {
                do it++; while (it->Level > level);
                }
            else{
                if ((obj = OO_New( it->Class, it->TagList )) == NULL)
					goto END_ERROR;

				if ((it->Var != OOV_NOOFFS) || (parent != NULL))
					{
					OOTagItem newobj_attrs[3], *cur = newobj_attrs;
					if (it->Var != OOV_NOOFFS)
						{
						cur->ti_Tag32 = OOA_StoreAddr;
						cur->ti_Data = (ULONG)ptrbase + it->Var;
						cur++;
						}
					if (parent != NULL)
						{
						cur->ti_Tag32 = OOA_Parent;
						cur->ti_Ptr = parent;
						cur++;
						}
					if (cur != newobj_attrs)
						{
						cur->ti_Tag32 = OOV_TAG_END;
						cur->ti_Ptr = NULL;
						OO_DoMethod( obj, OOM_SET, newobj_attrs ); 
						}
					}

                if (it->NotifyTable)
                    {
                    OONotifyItem *nt;
                    for (nt=it->NotifyTable; nt->TrigType != OOV_ENDTABLE; nt++)
                        OO_AddNotifyItem( obj, nt, ptrbase ); // nt->DestObj, (OObject**)((ULONG)ptrbase + nt->DestVar), nt->MapList );
                    }
                it++;
                }
            }
        else{
            if (it->Level < level) break;
            if (! (it = (OOTreeItem*) new_treea( obj, ptrbase, it, startlevel ))) goto END_ERROR;
            }
        }
    if (it->Level == OBJECTEND && level == startlevel) return( obj );
    return (OObject*)it;

  END_ERROR:
    return 0;
}

OObject *OO_NewTreeV( OObject *parent, APTR ptrbase, ULONG itval, ... )
{
    return OO_NewTree( parent, ptrbase, (OOTreeItem*)&itval );
}

OObject *OO_NewTree( OObject *parent, APTR ptrbase, OOTreeItem *it )
{
    return new_treea( parent, ptrbase, it, it->Level );
}

OObject *OO_GetRootObject( OObject *obj )
{
    for (; obj->ParentsTable.Count; obj=obj->ParentsTable.Table[0]) ;
	return obj;
}

OOClass *OO_GetRootClass( OObject *obj )
{
	OOClass *cl;
    for (cl=obj->Class; cl->BaseClass; cl=cl->BaseClass) ;
    return cl;
}

OObject *OO_GetParentOfClass( OObject *obj, OOClass *cl )
{
	ULONG i, count = obj->ParentsTable.Count;
	OObject *par, **tab = obj->ParentsTable.Table;
	OOASSERT( tab != NULL );
	for (i=0, par=*tab; i < count; i++, par=*++tab)
		{
		if (par->Class == cl) return par;
		if ((obj = OO_GetParentOfClass( par, cl )) != NULL) return obj;
		}
    return NULL;
}


//*****************************************************************************
//*****************************************************************************
//**********													***************
//**********      Functions private to internal class code      ***************
//**********													***************
//*****************************************************************************
//*****************************************************************************

ULONG OOPrv_MNew( OOClass *cl, OObject *obj, OOTagList *tl )
{
	// Initialize attrs taglist of instance with the class default values
	OOAttrDesc *ad = cl->AttrsDesc;
	if (ad != NULL)
		{
		// Install defaults values
		OOTagItem *ti = OOINST_ATTRS(cl,obj);
		for (; ad->TagItem.ti_Tag32 != OOV_TAG_END; ad++, ti++)
			*ti = ad->TagItem;
		*ti = ad->TagItem; // TAG_END

		// Update attributes with taglist values
		if (tl != NULL)
			{
			OOSetGetParms SGP;
			OOAttrDesc *desc;
			OOTagItem *attr;

			SGP.Class = cl;
			SGP.Obj = obj;
			SGP.Instance = OOINST_DATA(cl,obj);
			SGP.MethodFlag = OOF_MNEW;
			for (ti=tl; ; ti++)
				{
				OOTAG_GETITEM(ti);
				if (ti == NULL) break;
				SGP.ExtAttr = ti;
				// For each attribute of attrlist examine if the value changes
				for (desc=cl->AttrsDesc, attr=OOINST_ATTRS(cl,obj); attr->ti_Tag32 != OOV_TAG_END; attr++, desc++)
					{
					// If permited for this method and If tags are same, perhaps update the data
					if (attr->ti_Tag == ti->ti_Tag)
						{
						if (desc->Flags & OOF_MNEW)
							{
							// If a function exists, let it do the change
							if (desc->SetGet != NULL)
								{
								SGP.Attr = attr;
								if (desc->SetGet( &SGP ) == OOV_ERROR)
									return OOV_ERROR;
								}
							else{
								// If datas are different, update them
								if (OOTAGDATAEQUAL(attr,ti) == FALSE)
									attr->ti_Data = ti->ti_Data;
								}
							}
						// if a tag is found do not search it in other classes instances
						goto NEXT_ENTRY;
						}
					}
				NEXT_ENTRY:;
				}
			}
		}
	return OOV_OK;
}

static ULONG process_one_tag( OOSetGetParms *sgp, OOTagItem *ti, ULONG *num_notify, ULONG *num_area )
{
	OOAttrDesc *desc;
	OOTagItem *attr;
	OOClass *cl;

	sgp->ExtAttr = ti;
	// For each valid tag, mark it as to be ignored by Set processing
	ti->ti_Tag32 |= TAGMASK_TMP;
	// For each class, examine its attrlist
	for (cl=sgp->Obj->Class; cl != NULL; cl=cl->BaseClass)
		{
		if ((desc = cl->AttrsDesc) != NULL)
			{
			sgp->Instance = OOINST_DATA(cl,sgp->Obj);
			// For each attribute of attrlist examine if the value changes
			for (attr=OOINST_ATTRS(cl,sgp->Obj); attr->ti_Tag32 != OOV_TAG_END; attr++, desc++)
				{
				// If permited for this method and If tags are same, perhaps update the data
				if (attr->ti_Tag == ti->ti_Tag)
					{
					if (desc->Flags & sgp->MethodFlag)
						{
						// If a function exists, let it do the change
						if (desc->SetGet != NULL)
							{
							ULONG rc;
							sgp->Attr = attr;
							rc = desc->SetGet( sgp );
							if (rc == OOV_ERROR) return OOV_ERROR;
							if (rc == OOV_NOCHANGE) goto NEXT_ENTRY;
							}
						else{
							// If datas are different, update them
							if (OOTAGDATAEQUAL(attr,ti) == FALSE)
								attr->ti_Data = ti->ti_Data;
							else goto NEXT_ENTRY;
							}
						// Must this attribute invoke notification ?
						if (desc->Flags & OOF_MNOT)
							{
							ti->ti_Tag32 &= ~TAGMASK_TMP; // Do not ignore for notify
							(*num_notify)++;
							}
						// Must this attribute invoke parent's area recomputations
						if (desc->Flags & OOF_MAREA)
							{
							(*num_area)++;
							}
						}
					// if a tag is found do not search it in other classes instances
					goto NEXT_ENTRY;
					}
				}
			}
		}
NEXT_ENTRY:
	return OOV_OK;
}

// Examine each tag of tl for all attrs of all class instances of the object
ULONG OOPrv_MSet( OOClass *cl, OObject *obj, OOTagList *tl, UWORD methodflag )
{
	OOSetGetParms SGP;
	ULONG num_notify = 0;
	ULONG num_area = 0;
	OOTagItem *ti;

	// Do it only from the most derivated class
	if (cl != obj->Class || tl == NULL)
		return OOV_OK;

	// Parse given taglist for new values
	SGP.Class = cl;
	SGP.Obj = obj;
	SGP.MethodFlag = methodflag;
	for (ti=tl; ; ti++)
		{
		OOTAG_GETITEM(ti);
        if (ti == NULL) break;
		if (process_one_tag( &SGP, ti, &num_notify, &num_area ) == OOV_ERROR) return OOV_ERROR;
		}

	if (methodflag & OOF_MSET) // not for OOF_MNEW
		{
		// See if notification must occurs
		if (num_notify) if (OO_DoNotify( obj, tl ) == -1) return OOV_ERROR;	// exécution de OOM_DELETE
		// See Area change must invoke recomputations
		if (num_area) OO_DoMethod( obj, OOM_AREA_SIGNALCHANGE, NULL );
		}

	return OOV_OK;
}

ULONG OOPrv_MSetBounceV( OOSetGetParms *sgp, ULONG attr, ... )
{
	return OOPrv_MSetBounce( sgp, (OOTagList*)&attr );
}

ULONG OOPrv_MSetBounce( OOSetGetParms *sgp, OOTagList *tl )
{
	ULONG num_notify = 0;
	ULONG num_area = 0;
	OOTagItem *ti; //, *saveattr;
	OORESULT rc;

	for (ti=tl; ; ti++)
		{
		OOTAG_GETITEM(ti);
        if (ti == NULL) break;

		// saveattr = sgp->Attr;
		rc = process_one_tag( sgp, ti, &num_notify, &num_area );
		// sgp->Attr = saveattr;
		if (rc == OOV_ERROR) return OOV_ERROR;
		}

	if (sgp->MethodFlag & OOF_MSET) // not for OOF_MNEW
		{
		// See if notification must occurs
		if (num_notify) if (OO_DoNotify( sgp->Obj, tl ) == -1) return OOV_ERROR;	// exécution de OOM_DELETE
		// See Area change must invoke recomputations
		if (num_area) OO_DoMethod( sgp->Obj, OOM_AREA_SIGNALCHANGE, NULL );
		}
	return OOV_OK;
}

ULONG OOPrv_MGet( OOClass *cl, OObject *obj, OOTagList *ti )
{
	OOSetGetParms SGP;
	OOTagItem *attr;
	OOAttrDesc *desc;

	// Do it only from the most derivated class
	if (cl != obj->Class)
		return OOV_OK;

	// Parse given taglist for new values
	SGP.Class = cl;
	SGP.Obj = obj;
	SGP.MethodFlag = OOF_MGET;
	for (; ; ti++)
		{
		OOTAG_GETITEM(ti);
        if (ti == NULL) break;
		OOASSERT( ti->ti_Ptr != NULL );
		SGP.ExtAttr = ti;
		// For each class, examine its attrlist
		for (cl=obj->Class; cl != NULL; cl=cl->BaseClass)
			{
			if ((desc = cl->AttrsDesc) != NULL)
				{
				SGP.Instance = OOINST_DATA(cl,obj);
				// For each attribute of attrlist examine if the value changes
				for (attr=OOINST_ATTRS(cl,obj); attr->ti_Tag32 != OOV_TAG_END; attr++, desc++)
					{
					// If tags are same, perhaps get the data
					if (attr->ti_Tag == ti->ti_Tag)
						{
						// If permited for this method
						if (desc->Flags & OOF_MGET)
							{
							// If a function exists, let it give the value
							if (desc->SetGet != NULL)
								{
								SGP.Attr = attr;
								if (desc->SetGet( &SGP ) == OOV_ERROR)
									return OOV_ERROR;
								}
							else{
								*(ULONG*)ti->ti_Ptr = attr->ti_Data;
								}
							goto NEXT_ENTRY_FOUND;
							}
						// if a tag is found do not search it in other classes instances
						goto NEXT_ENTRY_NOTFOUND;
						}
					}
				}
			}
		NEXT_ENTRY_NOTFOUND:
		*(ULONG*)ti->ti_Ptr = 0; // Get 0 if nothing else
		NEXT_ENTRY_FOUND:;
		}

	return OOV_OK;
}

ULONG OOPrv_MClone( OOClass *cl, OObject *obj, OOTagList *ti )
{
	OOResult = (OORESULT)NULL;
	return OOV_OK;
}
