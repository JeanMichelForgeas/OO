
#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_pool.h"

#define INDEX_OOA_Child		0
#define INDEX_OOA_Parent	1
#define INDEX_OOA_StoreAddr	2

void free_attrs_buffers( OObject *obj );


//********************************************************************************
//********************************************************************************
//***************************                        *****************************
//***************************    Class Dispatcher    *****************************
//***************************                        *****************************
//********************************************************************************
//********************************************************************************

static ULONG Dispatcher_Root( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
			{
            OOClass *newclass = (OOClass*)obj;
			OOASSERT( newclass->InstOffset != 0 );
			OOASSERT( OOGlobalPool != NULL );
            if ((obj = (OObject*) OOPool_AllocClear( OOGlobalPool, newclass->InstOffset + newclass->InstSize + newclass->AttrsSize )) != NULL)
                {
                obj->Class = newclass;
				OOAr_Init( &obj->ParentsTable , TRUE );
				OOAr_Init( &obj->ChildrenTable, TRUE );
				OOAr_Init( &obj->SendersTable , TRUE );
				OOAr_Init( &obj->TargetsTable , TRUE );
				// Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			}
            return( (ULONG)obj );

        case OOM_DELETE:
			OOASSERT( obj != NULL );
            OO_DoChildrenMethod( 0, obj, method, attrlist );
            if (obj->Flags & OOF_DONOTDELETE)
                {
                obj->Flags |= OOF_DELETED;
                }
            else{
				OOTagList *tl = OOINST_ATTRS(cl,obj);
				OObject **aobj;

				OO_DetachParents( obj );
                OO_RemoveTargets( obj );
				
				aobj = (OObject**) OOINST_ATTR(cl,obj,INDEX_OOA_StoreAddr);
                if (aobj != NULL) *aobj = NULL;

				free_attrs_buffers( obj );

				OOAr_Cleanup( &obj->ParentsTable  );
				OOAr_Cleanup( &obj->ChildrenTable );
				OOAr_Cleanup( &obj->SendersTable  );
				OOAr_Cleanup( &obj->TargetsTable  );

				OOPool_Free( OOGlobalPool, obj );
                }
            return OOV_OK;

        case OOM_COPY:
			break;

        //---------------
        case OOM_GET:
            return OOPrv_MGet( cl, obj, attrlist );

        case OOM_SET:
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );
/*
        //---------------
        case OOM_ADDNOTIFY:
            //return( (ULONG) OO_AddNotifyA( obj, ((OOMsg_AddNotify*)msg)->Dest, ((OOMsg_AddNotify*)msg)->PDest, (OOTagItem*)((OOMsg_AddNotify*)msg)->MapList ) );
            break;

		case OOM_NOTIFY:
            return( OO_DoNotifyA( obj, ((OOMsg_Notify*)msg)->flags, ((OOMsg_Notify*)msg)->AttrList ) );
*/
        //---------------
        case OOM_ADDCHILD:
			{
			OObject *child = (OObject*) OOTag_GetData( attrlist, OOA_Child, (ULONG)NULL );
			if (child != NULL)
				return OO_AttachParent( child, obj );
            return OOV_OK;
			}
        case OOM_REMCHILD:
			{
			OObject *child = (OObject*) OOTag_GetData( attrlist, OOA_Child, (ULONG)NULL );
			if (child != NULL)
				OO_DetachParent( child, obj );
            return OOV_OK;
			}
        case OOM_ADDPARENT:
			{
			OObject *parent = (OObject*) OOTag_GetData( attrlist, OOA_Parent, (ULONG)NULL );
			if (parent != NULL)
				return OO_AttachParent( obj, parent );
            return OOV_OK;
			}
        case OOM_REMPARENT:
			{
			OObject *parent = (OObject*) OOTag_GetData( attrlist, OOA_Parent, (ULONG)NULL );
			if (parent != NULL)
				OO_DetachParent( obj, parent );
            return OOV_OK;
			}
        case OOM_DETACH:
			{
			OO_DetachAll( obj );
            return OOV_OK;
			}
        }
    return OOV_OK;
}

//********************************************************************************
//********************************************************************************
//***************************                        *****************************
//***************************        Utilities       *****************************
//***************************                        *****************************
//********************************************************************************
//********************************************************************************

void free_attrs_buffers( OObject *obj )
{
	OOClass *cl;
	for (cl=obj->Class; cl != NULL; cl=cl->BaseClass)
		if (cl->AttrsSize != 0)
			OOTag_FreeBuffers( OOINST_ATTRS(cl,obj) );
}


//********************************************************************************
//********************************************************************************
//***************************                         ****************************
//***************************    Attributes setting   ****************************
//***************************                         ****************************
//********************************************************************************
//********************************************************************************

// If a SetGet function is given to the attribute, the function has perhaps to change
//	the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same of the 
//	current attr value.

static ULONG SetGet_Child( OOSetGetParms *sgp )
{
	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (OO_AttachParent( (OObject*)sgp->ExtAttr->ti_Ptr, sgp->Obj ) == OOV_OK)
			return OOV_CHANGED;
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = (APTR)&sgp->Obj->ChildrenTable;
		}
	return OOV_NOCHANGE;
}

static ULONG SetGet_Parent( OOSetGetParms *sgp )
{
	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (OO_AttachParent( sgp->Obj, (OObject*)sgp->ExtAttr->ti_Ptr ) == OOV_OK)
			return OOV_CHANGED;
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = (APTR)&sgp->Obj->ParentsTable;
		}
	return OOV_NOCHANGE;
}

static ULONG SetGet_StoreAddr( OOSetGetParms *sgp )
{
	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (sgp->Attr->ti_Ptr != sgp->ExtAttr->ti_Ptr)
			{
			// Get the new value for the attr
			OObject** obj_addr = (OObject**)sgp->ExtAttr->ti_Ptr;
			// Put object into the address
			if (obj_addr != NULL)
				*obj_addr = sgp->Obj;
			// Set the attr to the new corrected value
			sgp->Attr->ti_Ptr = (APTR)obj_addr;
			return OOV_CHANGED;
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return OOV_NOCHANGE;
}

//********************************************************************************
//********************************************************************************
//***************************                         ****************************
//***************************    Class definitions    ****************************
//***************************                         ****************************
//********************************************************************************
//********************************************************************************

static OOAttrDesc attrs[] = {
    { OOA_Child		, (ULONG)NULL, "Child"		, SetGet_Child		, OOF_MNEW | OOF_MSET | OOF_MGET			},
    { OOA_Parent	, (ULONG)NULL, "Parent"		, SetGet_Parent		, OOF_MNEW | OOF_MSET | OOF_MGET			},
    { OOA_StoreAddr	, (ULONG)NULL, "StoreAddr"	, SetGet_StoreAddr	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT	},
    { TAG_END }
	};

OOClass Class_Root = { 
	Dispatcher_Root, 
	NULL,				// Base class
	"Class_Root",		// Class Name
	0,					// InstOffset
	sizeof(OOInst_Root),// InstSize  : Taille de l'instance
	attrs,				// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
