/*****************************************************************
 *
 *       Project:    OO
 *       Function:   
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "oo_class.h"
#include "class_root.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_...( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_... *inst = OOINST_DATA(cl,obj);
				// Init instance datas
                inst-> = ;
				// Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;

        case OOM_DELETE:
			return OO_DoBaseMethod( cl, obj, method, attrlist );

        //---------------
        case OOM_GET:
			// Do not call base method
            return OOPrv_MGet( cl, obj, attrlist );

        case OOM_SET:
			// Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

        //---------------
        default:
            return OO_DoBaseMethod( cl, obj, method, attrlist );
        }
    return OOV_OK;
}

/***************************************************************************
 *
 *      Utilities
 *
 ***************************************************************************/


/***************************************************************************
 *
 *      Attributes setting
 *
 ***************************************************************************/

// If a SetGet function is given to the attribute, the function has perhaps to change
//	the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same than the 
//	current attr value.

static ULONG SetGet_***( OOSetGetParms *sgp )
{
	OOInst_... *inst = (OOInst_...*)sgp->Instance;
	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		// Use direct ==, or quick macro, or directly OOTag_DataEqual() if you know they are not numbers
		if (OOTAGDATAEQUAL(sgp->Attr,sgp->ExtAttr) == FALSE)
			{
			// Get the new value for the attr
			ULONG new_value = sgp->ExtAttr->ti_Data;
			// Eventually correct the new value
			if (new_value ...) 
				{
				new_value = ...;
				// In this case we must retest the difference
				if (new_value == sgp->Attr->ti_Data)
					return OOV_NOCHANGE;
				}
			// Set the attr and the passed tagitem to the new corrected value
			sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data = new_value;
			return OOV_CHANGED;
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return OOV_NOCHANGE;
}

/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_***, 0, "***", NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT | OOF_MAREA },
    { TAG_END }
	};

OOClass Class_... = { 
	Dispatcher_..., 
	&Class_<<<,			// Base class
	"Class_...",		// Class Name
	0,					// InstOffset
	sizeof(OOInst_...),	// InstSize  : Taille de l'instance
	attrs,				// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
