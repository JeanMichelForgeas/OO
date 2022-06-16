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

#define INDEX_OOA_Selected		0
#define INDEX_OOA_Determinate	1
#define INDEX_OOA_3States		2


/******* Imported ************************************************/

extern void  Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight );
extern ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG	CtrlChecker_HandleSystemEvent	( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti );
static void		CtrlChecker_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlChecker_AreaStart	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlChecker_Get			( OOClass *cl, OObject *obj, BOOL *determinate, BOOL *selected );
static void		CtrlChecker_Set			( OOClass *cl, OObject *obj );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlChecker( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_CHECKER, OOV_TAG_MORE, (ULONG)attrlist };
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, inittags )) != NULL)
				{
				if (OOPrv_MNew( cl, obj, attrlist ) != OOV_OK)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				else{
					ULONG titleplace = OOTag_GetData( attrlist, OOA_TitlePlace, OOV_RIGHT ); // Change default
					OOTagList *ctrl_attrs = OOINST_ATTRS(&Class_Ctrl,obj);
					if (titleplace == OOV_CENTER) titleplace = OOV_RIGHT;
					OOTag_SetData( ctrl_attrs, OOA_TitlePlace, titleplace );
					}
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
		case OOM_HANDLEEVENT:
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK)	break;
			return CtrlChecker_HandleSystemEvent( cl, obj, attrlist, (OOTagItem*)OOResult );

		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
			CtrlChecker_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlChecker_AreaStart( cl, obj, attrlist );
			break;

		case OOM_AREA_CLEANUP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			OO_DoBaseMethod( cl, obj, method, attrlist );
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

static ULONG CtrlChecker_HandleSystemEvent( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti )
{
	OOInst_HandleGUI	*gui	= attrlist[0].ti_Ptr; // OOA_HandleGUI
	OOEventMsg			*msg	= attrlist[1].ti_Ptr; // OOA_EventMsg
	OOTagList			*notify	= attrlist[2].ti_Ptr; // OOA_TagList
	OOInst_Area			*ar		= _DATA_AREA(obj);
	ULONG rc = OOV_OK;

	OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_HandleGUI && attrlist[1].ti_Tag == OOA_EventMsg && attrlist[2].ti_Tag == OOA_TagList && attrlist[3].ti_Tag == OOA_TagItem);

	switch (msg->message)
		{
		case WM_COMMAND:
			{
			UWORD notifycode = HIWORD(msg->wParam);
			UWORD ctrlid = LOWORD(msg->wParam);
			HWND ctrlwin = (HWND)msg->lParam;
			switch (notifycode)
				{
				case BN_CLICKED:
					{
					OOTagList *attrs = OOINST_ATTRS(cl,obj);
					BOOL determinate, selected;
					CtrlChecker_Get( cl, obj, &determinate, &selected );
					if (determinate != attrs[INDEX_OOA_Determinate].ti_Bool)
						{
						attrs[INDEX_OOA_Determinate].ti_Bool = determinate;
			            ti->ti_Tag = OOA_Determinate; ti->ti_Data = determinate; ti++;
						}
					if (determinate == TRUE && selected != attrs[INDEX_OOA_Selected].ti_Bool)
						{
						attrs[INDEX_OOA_Selected].ti_Bool = selected;
						ti->ti_Tag = OOA_Selected; ti->ti_Data = selected; ti++;
						}
					}
					break;
				}
			}
			break;
		} 

	OOResult = (OORESULT)ti;
    return rc;
}

static void CtrlChecker_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Area	*ar	= _DATA_AREA(obj);
	UWORD butwidth = (UWORD) GetSystemMetrics(SM_CXMENUCHECK);
	UWORD butheight = (UWORD) GetSystemMetrics(SM_CYMENUCHECK);

	//--- Correction de _Border
	Ctrl_AdaptBorders( obj, butwidth, butheight ); 

	//--- Obtention de _SubBorder
	ar->_SubBorder.Left   = butwidth / 2;
	ar->_SubBorder.Right  = butwidth - (butwidth / 2);
	ar->_SubBorder.Top	  = butheight / 2;
	ar->_SubBorder.Bottom = butheight - (butheight / 2);
}

static ULONG CtrlChecker_AreaStart( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OORenderInfo *ri = _RENDERINFO(obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	UBYTE *title;
	ULONG id, place, styles, extstyles;

	UWORD butwidth = (UWORD) GetSystemMetrics(SM_CXMENUCHECK);
	UWORD butheight = (UWORD) GetSystemMetrics(SM_CYMENUCHECK);

	if ((id = Ctrl_GetStartInfo( obj, attrlist, &place, &styles, &extstyles, &title )) == 0) 
		return OOV_ERROR;

	styles |= (attrs[INDEX_OOA_3States].ti_Bool) == TRUE ? BS_AUTO3STATE : BS_AUTOCHECKBOX;
	if ((ctrl_inst->Win = CreateWindowEx( extstyles, "BUTTON", NULL, styles, 
				ar->_B.X + ar->_Border.Left,
				ar->_B.Y + ar->_Border.Top,
				butwidth, // ar->_B.W - (ar->_Border.Left + ar->_Border.Right),
				butheight, // ar->_B.H - (ar->_Border.Top + ar->_Border.Bottom),
				ri->wi, (void*)id, OOInstance, obj )) != NULL)
		{
		CtrlChecker_Set( cl, obj );
		return OOV_OK;
		}
	return OOV_ERROR;
}

static void CtrlChecker_Get( OOClass *cl, OObject *obj, BOOL *determinate, BOOL *selected )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	ULONG state = SendMessage( ctrl_inst->Win, BM_GETCHECK, 0, 0 );

	*determinate = *selected = FALSE;
	if (state != BST_INDETERMINATE)
		{
		*determinate = TRUE;
		*selected = (state == BST_CHECKED) ? TRUE : FALSE;
		}
}

static void CtrlChecker_Set( OOClass *cl, OObject *obj )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagItem	*attrs = OOINST_ATTRS(cl,obj);
	ULONG		state;

	if (attrs[INDEX_OOA_Determinate].ti_Bool == FALSE) state = BST_INDETERMINATE;
	else if (attrs[INDEX_OOA_Selected].ti_Bool == TRUE) state = BST_CHECKED;
	else state = BST_UNCHECKED;
	SendMessage( ctrl_inst->Win, BM_SETCHECK, state, 0 );
}


/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

static ULONG SetGet_Checker( OOSetGetParms *sgp )
{
	OOInst_Ctrl *inst = (OOInst_Ctrl*)sgp->Instance;
	ULONG change = OOV_NOCHANGE;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (data != sgp->Attr->ti_Data) 
			{
			change = OOV_CHANGED;
			sgp->Attr->ti_Data = data;
			CtrlChecker_Set( sgp->Class, sgp->Obj );
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return change;
}


/***************************************************************************
 *
 *		Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_Selected	 , FALSE, "Selected"	, SetGet_Checker, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_Determinate, TRUE , "Determinate"	, SetGet_Checker, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_3States	 , TRUE, "3States"		, NULL		  , OOF_MNEW |			  OOF_MGET			  },
	{ TAG_END }
	};

OOClass Class_CtrlChecker = { 
	Dispatcher_CtrlChecker, 
	&Class_Ctrl,		// Base class
	"Class_CtrlChecker",	// Class Name
	0,					// InstOffset
	0,					// InstSize  : Taille de l'instance
	attrs,				// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
