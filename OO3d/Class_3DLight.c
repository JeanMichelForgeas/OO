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
#include "oo_class_prv.h"
#include "oo_3d_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static UWORD Light_Faces[] = {
    1, 1,9, OOF_ELLIPSE|OOF_FILL, 0, 0,
    ENDFACES
};


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_3DLight( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            {
            OOTagItem TL[6];
            TL[0].ti_Tag32 = D3A_DefFaces	; TL[0].ti_Data = (ULONG)Light_Faces;
            TL[1].ti_Tag32 = D3A_R			; TL[1].ti_Float = (FLOAT).03F;	// 3 cm
            TL[2].ti_Tag32 = D3A_Masse		; TL[2].ti_Float = (FLOAT).1F; // 100 g
            TL[3].ti_Tag32 = D3A_Sphere		; TL[3].ti_Data = TRUE;
            TL[4].ti_Tag32 = D3A_Actions	; TL[4].ti_Data = D3F_SENS_LUMINOSITY;
            TL[5].ti_Tag32 = OOV_TAG_MORE	; TL[5].ti_Ptr = attrlist;
            if ((obj = (OObject*)OO_DoBaseMethod( cl, obj, method, TL )) != NULL)
                {
                // OOInst_3DLight *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				// Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, TL ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }

            return (ULONG)obj;
			}			
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
        case D3M_OBJ2ACTOR:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Object );
            {
            OOInst_3DSolid *s1 = D3SOL(obj);
            OOInst_3DSolid *s2 = D3SOL(attrlist[0].ti_Ptr);

			if (s2->Sensitivity & D3F_SENS_LUMINOSITY)	// Dire à l'objet qu'il est éclairé 
				{
				// ... oui mais bon, hein ?
				}
            }
            break;

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


/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

OOClass Class_3DLight = { 
	Dispatcher_3DLight, 
	&Class_3DSolid,		// Base class
	"Class_3DLight",	// Class Name
	0,					// InstOffset
	0,					// InstSize  : Taille de l'instance
	NULL,				// AttrsDesc : Table de description des attributs
	0					// AttrsSize : size of instance own taglist
};