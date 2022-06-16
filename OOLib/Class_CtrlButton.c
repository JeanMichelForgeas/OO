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

#include "oo_class.h"
#include "oo_class_prv.h"

#include "oo_gui_prv.h"


/******* Imported ************************************************/

extern ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG CtrlButton_AreaStart( OOClass *cl, OObject *obj, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlButton( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_BUTTON, OOV_TAG_MORE, (ULONG)attrlist };
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, inittags )) != NULL)
				{
				if (OOPrv_MNew( cl, obj, attrlist ) != OOV_OK)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
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
		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlButton_AreaStart( cl, obj, attrlist );
			break;

		case OOM_AREA_REFRESH:
			// Pour ne pas faire le refresh des titres, on saute Class_Ctrl.
			if (OO_DoBaseMethod( cl->BaseClass, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			// Sous Win32 les titres des boutons se dessinent eux-mêmes.
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

static ULONG CtrlButton_AreaStart( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OORenderInfo *ri = _RENDERINFO(obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	UBYTE *title;
	ULONG id, place, styles, extstyles;

	if ((id = Ctrl_GetStartInfo( obj, attrlist, &place, &styles, &extstyles, &title )) == 0) 
		return OOV_ERROR;

	styles |= BS_TEXT;
	if (place == OOV_CENTER) styles |= BS_VCENTER;
	else title = NULL;

	if ((ctrl_inst->Win = CreateWindowEx( extstyles, "BUTTON", title, styles, 
				ar->_B.X + ar->_Border.Left,
				ar->_B.Y + ar->_Border.Top,
				ar->_B.W - (ar->_Border.Left + ar->_Border.Right),
				ar->_B.H - (ar->_Border.Top + ar->_Border.Bottom),
				ri->wi, (void*)id, OOInstance, obj )) != NULL)
		{
		SendMessage( ctrl_inst->Win, WM_SETFONT, (WPARAM)ctrl_inst->Font, MAKELPARAM(FALSE,0) );
		return OOV_OK;
		}
	return OOV_ERROR;
}


/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/


/***************************************************************************
 *
 *		Class definitions
 *
 ***************************************************************************/

OOClass Class_CtrlButton = { 
	Dispatcher_CtrlButton, 
	&Class_Ctrl,				// Base class
	"Class_CtrlButton", 		// Class Name
	0,							// InstOffset
	0,							// InstSize  : Taille de l'instance
	NULL,						// AttrsDesc : Table de description des attributs
	0							// AttrsSize : size of instance own taglist
	};
