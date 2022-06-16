/*****************************************************************
 *
 *		 Project:	 OO
 *		 Function:	 
 *
 *		 Created:		 Jean-Michel Forgeas
 *
 *		 This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "oo_class_prv.h"
#include "oo_sound_prv.h"

#define INDEX_OOA_OnOff	0


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG HandleSoundEvent( OObject *obj, ULONG method, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_HandleSound( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
				{
				OOInst_HandleSound *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				// Init default attrs & change defaults with new values
				if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			return (ULONG)obj;

		case OOM_DELETE:
			OO_DoMethod( obj, OOM_STOPPLAY, NULL );
			return OO_DoBaseMethod( cl, obj, method, attrlist );

		//---------------
		case OOM_GET:
			// Do not call base method
			return OOPrv_MGet( cl, obj, attrlist );

		case OOM_SET:
			// Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

		//---------------
		case OOM_HANDLEEVENT:
			if (OOINST_ATTR(cl,obj,INDEX_OOA_OnOff) == TRUE)
				return HandleSoundEvent( obj, method, attrlist );
			break;

		//---------------
		case OOM_STARTPLAY:
			OO_InitSillyDirectSound();
			OO_SetAttr( obj, OOA_OnOff, TRUE );	// Envoie methode SETEVENT, notifie
			OO_DoChildrenMethodV( 0, obj, OOM_STARTPLAY, OOA_HandleSound, obj, TAG_END );
			break;

		case OOM_STOPPLAY:
			OO_DoChildrenMethod( 0, obj, OOM_STOPPLAY, NULL );
			OO_SetAttr( obj, OOA_OnOff, FALSE ); // Envoie methode CLEAREVENT, notifie
			break;

		//---------------
		default:
			return OO_DoBaseMethod( cl, obj, method, attrlist );
		}
	return OOV_OK;
}


/***************************************************************************
 *
 *		Utilities
 *
 ***************************************************************************/

static ULONG HandleSoundEvent( OObject *obj, ULONG method, OOTagList *attrlist )
{
	OO_DoChildrenMethod( 0, obj, OOM_LOADNEXT, NULL );
	OO_DoClassMethodV( &Class_Application, NULL, OOM_SETEVENT, OOA_EventMask, OO_GetAttr( obj, OOA_EventMask ), TAG_END );
	return OOV_OK;
}

/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

static ULONG SetGet_OnOff( OOSetGetParms *sgp ) // OOA_OnOff
{
	ULONG change = OOV_NOCHANGE;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		BOOL data = sgp->ExtAttr->ti_Bool;
		if (sgp->Attr->ti_Bool != data) 
			{
			ULONG method = (data == TRUE) ?	OOM_SETEVENT : OOM_CLEAREVENT;
			OO_DoClassMethodV( &Class_Application, NULL, method, OOA_EventMask, OO_GetAttr( sgp->Obj, OOA_EventMask ), TAG_END );
			change = OOV_CHANGED;
			sgp->Attr->ti_Bool = data;
			}
		}
	else{
		*(ULONG*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Data;
		}
	return change;
}

/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_OnOff , FALSE	, "OnOff"	, SetGet_OnOff, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }					
	};

OOClass Class_HandleSound = { 
	Dispatcher_HandleSound, 
	&Class_SysEvent,			// Base class
	"Class_HandleSound",		// Class Name
	0,							// InstOffset
	sizeof(OOInst_HandleSound),	// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
