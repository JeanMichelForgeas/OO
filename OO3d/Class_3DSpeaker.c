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
#include "oo_gui_prv.h"
#include "oo_sound.h"
#include "oo_3d_prv.h"

#define	INDEX_D3A_OnOff			0
#define	INDEX_D3A_ConnectObj	1


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG add_speaker( OObject *obj );
static ULONG remove_speaker( OObject *obj );

static ULONG search_handlesound( OObject *tree_obj, OOInst_3DSpeaker *inst );


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_3DSpeaker( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_3DSpeaker *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				inst->HandleSound = NULL;
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
		case D3M_START3D:
			OO_DoBaseMethod( cl, obj, method, attrlist );
			add_speaker( obj );
			break;

		case D3M_STOP3D:
			remove_speaker( obj );
			OO_DoBaseMethod( cl, obj, method, attrlist );
			break;

        //---------------
		case D3M_OBJTOSPEAKER:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort && attrlist[1].ti_Tag == D3A_ObjTable && attrlist[2].ti_Tag == D3A_ObjCount );
			if (OOINST_ATTR(cl,obj,INDEX_D3A_OnOff) == TRUE)
				{
				OObject **connectobj = (OObject**) OOINST_ATTR(cl,obj,INDEX_D3A_ConnectObj);
				if (connectobj != NULL && *connectobj != NULL)
					{
					OOTagItem TI = attrlist[0];
					attrlist[0].ti_Tag = D3A_ObjSpeaker;
					attrlist[0].ti_Ptr = obj;
					OO_DoMethod( *connectobj, D3M_OBJTOMIKE, attrlist ); // Seules les micros ont cette méthode
					attrlist[0] = TI;
					}
				}
			break;

		case D3M_PLAYSOUND:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort );
			if (OOINST_ATTR(cl,obj,INDEX_D3A_OnOff) == TRUE)
				{
                OOInst_3DSpeaker *inst = OOINST_DATA(cl,obj);
				OObject **connectobj = (OObject**) OOINST_ATTR(cl,obj,INDEX_D3A_ConnectObj);
				OObjArray *soundlist;
				OOTagList TL[2];
				if (connectobj != NULL && *connectobj != NULL)
					{
					TL[0].ti_Tag32 = D3A_ObjSpeaker;	TL[0].ti_Ptr = obj;
					TL[1].ti_Tag32 = OOV_TAG_END;		TL[1].ti_Ptr  = NULL;
					OO_DoMethod( *connectobj, D3M_MIXSOUND, TL );

					if ((soundlist = (OObjArray*)OOResult) != NULL)
						{
						ULONG i;
						if (inst->HandleSound == NULL)
							OO_ParseTree( OO_GetRootObject(obj), NULL, search_handlesound, (ULONG)inst );
						if (inst->HandleSound != NULL)
							{
							for (i=0; i < soundlist->Count; i++)
								{
								OObject *sound = soundlist->Table[i];
								OO_DoMethodV( sound, OOM_STARTPLAY, OOA_HandleSound, inst->HandleSound, TAG_END );
								}
							}
						}
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

static ULONG add_speaker( OObject *obj )
{
	OObject *space = OO_GetParentOfClass( obj, &Class_3DSpace );
	if (space != NULL) 
		return OO_DoMethodV( space, D3M_ADDSPEAKER, D3A_ObjSpeaker, obj, TAG_END );
	return OOV_ERROR;
}

static ULONG remove_speaker( OObject *obj )
{
	OObject *space = OO_GetParentOfClass( obj, &Class_3DSpace );
	if (space != NULL) 
		return OO_DoMethodV( space, D3M_REMOVESPEAKER, D3A_ObjSpeaker, obj, TAG_END );
	return OOV_ERROR;
}

static ULONG search_handlesound( OObject *tree_obj, OOInst_3DSpeaker *inst )
{
	if (tree_obj->Class != &Class_HandleSound) return 0;

	inst->HandleSound = tree_obj;
	return 1;
}


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

static OOAttrDesc attrs[] = {
    { OOA_OnOff		, TRUE		 , "OnOff"		, NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_ConnectObj, (ULONG)NULL, "ConnectObj"	, NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }
	};

OOClass Class_3DSpeaker = { 
	Dispatcher_3DSpeaker, 
	&Class_3DSolid,			// Base class
	"Class_3DSpeaker",		// Class Name
	0,						// InstOffset
	sizeof(OOInst_3DSpeaker),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
