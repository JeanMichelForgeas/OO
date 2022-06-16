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
#include "oo_buf.h"

#include "oo_gui_prv.h"

#define INDEX_OOA_Text    		0
#define INDEX_OOA_BoxType    	1
#define INDEX_OOA_TextPlace 	2
#define INDEX_OOA_NumDigits 	3
#define INDEX_OOA_VerifyFunc 	4

typedef LRESULT (CALLBACK*WINDOWPROC) (HWND, UINT, WPARAM, LPARAM);
typedef struct 
{
	WINDOWPROC	OldEditProc;
	OOBuf		*TextBuf;
}
OOInst_CtrlEditText;


/******* Imported ************************************************/

extern void	Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight );
extern ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG	CtrlEditText_HandleSystemEvent( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti );
static void		CtrlEditText_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlEditText_AreaMinMax	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlEditText_AreaStart	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static UBYTE *	CtrlEditText_Get		( OObject *obj );
static void		CtrlEditText_Set		( OObject *obj );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlEditText( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_EDITTEXT, OOV_TAG_MORE, (ULONG)attrlist };
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, inittags )) != NULL)
				{
				OOInst_CtrlEditText *inst = OOINST_DATA(cl,obj);
				ULONG ok = OOV_ERROR;
				if ((inst->TextBuf = OOBuf_Alloc( OOGlobalPool, 0, 4 )) != NULL)
					ok = OOPrv_MNew( cl, obj, attrlist );
				if (ok != OOV_OK)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				else{
					ULONG titleplace = OOTag_GetData( attrlist, OOA_TitlePlace, OOV_LEFT ); // Change default
					OOTagList *ctrl_attrs = OOINST_ATTRS(&Class_Ctrl,obj);
					if (titleplace == OOV_CENTER) titleplace = OOV_LEFT;
					OOTag_SetData( ctrl_attrs, OOA_TitlePlace, titleplace );
					}
				}
			}
			return (ULONG)obj;

		case OOM_DELETE:
				{
				OOInst_CtrlEditText *inst = OOINST_DATA(cl,obj);
				OOBuf_Free( &inst->TextBuf );
				}
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
			return CtrlEditText_HandleSystemEvent( cl, obj, attrlist, (OOTagItem*)OOResult );

		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
			CtrlEditText_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //********************* OOInst_Area*
			CtrlEditText_AreaMinMax( cl, obj, attrlist );
			break;

		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlEditText_AreaStart( cl, obj, attrlist );
			break;

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			{
			OOInst_Ctrl *ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
			OOInst_CtrlEditText *inst = OOINST_DATA(cl,obj);
			if (ctrl_inst->Win != NULL) SetWindowLong( ctrl_inst->Win, GWL_WNDPROC, (ULONG)inst->OldEditProc );
			}
			OO_DoBaseMethod( cl, obj, method, attrlist );
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

static ULONG CtrlEditText_HandleSystemEvent( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti )
{
/*
	OOInst_HandleGUI	*gui	= attrlist[0].ti_Ptr; // OOA_HandleGUI
	OOEventMsg			*msg	= attrlist[1].ti_Ptr; // OOA_EventMsg
	OOTagList			*notify	= attrlist[2].ti_Ptr; // OOA_TagList
	OOInst_Area			*ar		= _DATA_AREA(obj);
*/
	ULONG rc = OOV_OK;
/*
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
				case EN_...:
					if (ctrl2obj( obj ) != OOV_OK) ...
					break;
				}
			}
			break;
		} 
*/
	OOResult = (OORESULT)ti;
    return rc;
}

static ULONG verif_correct_val( OObject *obj, UBYTE **textptr )
{
	OOTagList *attrs = OOINST_ATTRS(obj->Class,obj);
	ULONG (*func)( OObject *obj, UBYTE **valptr );
	ULONG rc = OOV_OK;

	if ((func = attrs[INDEX_OOA_VerifyFunc].ti_Ptr) != NULL)
		{
		rc = func( obj, textptr ); // Func can change *textptr to give a new string
		}
	return rc;
}

static ULONG ctrl2obj( OObject *obj )
{
	UBYTE *text = CtrlEditText_Get( obj );
	ULONG rc = OOV_OK;

	if (text != NULL && (rc = verif_correct_val( obj, &text )) == OOV_OK)
		{
		OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
		OOTagItem change_tl[] = { OOA_Text, (ULONG)text, TAG_END };
		// fait par OOM_SET : SetWindowText( ctrl_inst->Win, text ); // Corrige
		rc = OOPrv_MSet( obj->Class, obj, change_tl, OOF_MSET );
		}
	return rc;
}

static ULONG APIENTRY NewEditProc( HWND w, UINT msgid, UINT wparam, LONG lparam ) 
{
	OObject *obj = (OObject*) GetWindowLong( w, GWL_USERDATA );
	OOInst_CtrlEditText *inst = OOINST_DATA(obj->Class,obj);

	switch (msgid) 
		{ 
		case WM_KILLFOCUS:
			if (ctrl2obj( obj ) != OOV_OK) { SetFocus( w ); return 0; }
			break;

		case WM_CHAR:
			{
			OOTagList *attrs = OOINST_ATTRS(obj->Class,obj);
			UBYTE ch = (UBYTE)wparam;

			switch (ch)
				{
				case 0x1B: 
					CtrlEditText_Set( obj );	// Annule entrée en remettant l'ancienne valeur stockée
					SetFocus( GetParent(w) );
					return 0;

				case '\n': 
				case '\r': 
					if (ctrl2obj( obj ) == OOV_OK) SetFocus( GetParent(w) );
					return 0;
				}
			}
			break;
		}
	return CallWindowProc( inst->OldEditProc, w, msgid, wparam, lparam );
}

static void CtrlEditText_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	OOInst_Area	*ar	= _DATA_AREA(obj);
	OOSize _S;

	//--- Flags
	ar->Flags &= ~OOMASK_AINSIDEPLACE;
	ar->Flags |= OOF_AINSIDE_CENTERW | OOF_AINSIDE_CENTERH;

	//--- Obtention de _SubBorder
	if (attrs[INDEX_OOA_BoxType].ti_Data != OOV_BOX_NULL)
		{
		ar->_SubBorder.Left = ar->_SubBorder.Right = (UWORD) 2 + GetSystemMetrics(SM_CXEDGE);
		ar->_SubBorder.Top = ar->_SubBorder.Bottom = (UWORD) 1 + GetSystemMetrics(SM_CYEDGE);
		}
	else{
		ar->_SubBorder.Left = ar->_SubBorder.Right = 0;
		ar->_SubBorder.Top = ar->_SubBorder.Bottom = 0;
		}

	//--- Correction de _Border
	OOGfx_GetSizeOfString( ctrl_inst->Font, "8", 1, &_S );
	if (ar->AskedW != 0)
		_S.W = ar->AskedW;
	else if (attrs[INDEX_OOA_NumDigits].ti_Data != 0)
		_S.W = (UWORD)(_S.W * attrs[INDEX_OOA_NumDigits].ti_Data);
	else if (attrs[INDEX_OOA_Text].ti_String != NULL)
		_S.W = (UWORD)(_S.W * strlen(attrs[INDEX_OOA_Text].ti_String));
	Ctrl_AdaptBorders( obj, (UWORD)(_S.W + ar->_SubBorder.Left + ar->_SubBorder.Right), (UWORD)(_S.H + ar->_SubBorder.Top + ar->_SubBorder.Bottom) ); 
}

static void CtrlEditText_AreaMinMax( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	OOInst_Area	*ar	= _DATA_AREA(obj);
	SWORD min;
	OOSize _S;

	OOGfx_GetSizeOfString( ctrl_inst->Font, "8", 1, &_S );
	if (ar->AskedW != 0)
		_S.W = ar->AskedW;
	else if (attrs[INDEX_OOA_NumDigits].ti_Data != 0)
		_S.W = (UWORD)(_S.W * attrs[INDEX_OOA_NumDigits].ti_Data);
	else if (attrs[INDEX_OOA_Text].ti_String != NULL)
		_S.W = (UWORD)(_S.W * strlen(attrs[INDEX_OOA_Text].ti_String));

	min = _S.W + ar->_Border.Left + ar->_Border.Right + ar->_SubBorder.Left + ar->_SubBorder.Right;
	if (ar->_MM.MinW < min) ar->_MM.MinW = min;
	min = _S.H + ar->_Border.Top + ar->_Border.Bottom + ar->_SubBorder.Top + ar->_SubBorder.Bottom;
	if (ar->_MM.MinH < min) ar->_MM.MinH = min;

	/*--- Obtention de _B.W et _B.H */
	if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
	if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;
}

static ULONG CtrlEditText_AreaStart( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOInst_CtrlEditText *inst = OOINST_DATA(cl,obj);
	OORenderInfo *ri = _RENDERINFO(obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	UBYTE *title;
	ULONG id, place, styles, extstyles;

	if ((id = Ctrl_GetStartInfo( obj, attrlist, &place, &styles, &extstyles, &title )) == 0) 
		return OOV_ERROR;

	styles |= ES_AUTOHSCROLL | ((attrs[INDEX_OOA_TextPlace].ti_Data) == OOV_RIGHT ? ES_RIGHT : ES_LEFT);
	if (attrs[INDEX_OOA_BoxType].ti_Data != OOV_BOX_NULL) extstyles |= WS_EX_CLIENTEDGE;
	if ((ctrl_inst->Win = CreateWindowEx( extstyles, "EDIT", NULL, styles, 
				ar->_B.X + ar->_Border.Left,
				ar->_B.Y + ar->_Border.Top,
				ar->_B.W - (ar->_Border.Left + ar->_Border.Right),
				ar->_B.H - (ar->_Border.Top + ar->_Border.Bottom),
				ri->wi, (void*)id, OOInstance, obj )) != NULL)
		{
		SetWindowLong( ctrl_inst->Win, GWL_USERDATA, (ULONG)obj ); // Nécessaire...
		inst->OldEditProc = (WINDOWPROC) SetWindowLong( ctrl_inst->Win, GWL_WNDPROC, (ULONG)NewEditProc );
		SendMessage( ctrl_inst->Win, WM_SETFONT, (WPARAM)ctrl_inst->Font, MAKELPARAM(FALSE,0) );
		CtrlEditText_Set( obj );
		return OOV_OK;
		}
	return OOV_ERROR;
}

static UBYTE *CtrlEditText_Get( OObject *obj )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOInst_CtrlEditText	*inst = OOINST_DATA(obj->Class,obj);

	if (ctrl_inst->Win != NULL)
		{
		ULONG len = GetWindowTextLength( ctrl_inst->Win );
		if (OOBuf_Resize( &inst->TextBuf, len+1, TRUE ) != NULL)
			GetWindowText( ctrl_inst->Win, inst->TextBuf, len+1 );
		}
	return inst->TextBuf;
}

static void CtrlEditText_Set( OObject *obj )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	if (ctrl_inst->Win != NULL)
		{
		OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
		OOTagItem	*attrs = OOINST_ATTRS(obj->Class,obj);
		UBYTE dig;

		verif_correct_val( obj, &attrs[INDEX_OOA_Text].ti_String );
		SetWindowText( ctrl_inst->Win, attrs[INDEX_OOA_Text].ti_String );
		dig = (UBYTE)attrs[INDEX_OOA_NumDigits].ti_Data;
		SendMessage( ctrl_inst->Win, EM_SETLIMITTEXT, dig, 0 );
		}
}


/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

static ULONG SetGet_All( OOSetGetParms *sgp )
{
	OOInst_Ctrl *inst = (OOInst_Ctrl*)sgp->Instance;
	ULONG change = OOV_NOCHANGE;
	BOOL test_notify = FALSE;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (sgp->ExtAttr->ti_Data != sgp->Attr->ti_Data) 
			{
			sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;

			switch (sgp->ExtAttr->ti_Tag)
				{
				case OOA_Text:
					CtrlEditText_Set( sgp->Obj );
					change = OOV_CHANGED;
					break;

				case OOA_TextPlace:
					{
					OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,sgp->Obj);
					ULONG styles = GetWindowLong( ctrl_inst->Win, GWL_STYLE );
					styles &= ~(ES_LEFT|ES_RIGHT);
					styles |= (sgp->Attr->ti_Data) == OOV_RIGHT ? ES_RIGHT : ES_LEFT;
					SetWindowLong( ctrl_inst->Win, GWL_STYLE, styles );
					change = OOV_CHANGED;
					}
					break;
				}
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
    { OOA_Text		, (ULONG)NULL		, "Number"   , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT 			},
    { OOA_BoxType	, OOV_BOX_HEAVYDOWN	, "BoxType"  , NULL		    , OOF_MNEW | OOF_MSET | OOF_MGET |           OOF_MAREA	},
    { OOA_TextPlace	, OOV_LEFT			, "TextPlace", SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET						},
    { OOA_NumDigits	, 0					, "NumDigits", NULL			, OOF_MNEW | OOF_MSET | OOF_MGET |           OOF_MAREA	},
    { OOA_VerifyFunc, (ULONG)NULL		, "VerifyFunc",NULL			, OOF_MNEW | OOF_MSET | OOF_MGET						},
	{ TAG_END }
	};

OOClass Class_CtrlEditText = { 
	Dispatcher_CtrlEditText, 
	&Class_Ctrl,				// Base class
	"Class_CtrlEditText",		// Class Name
	0,							// InstOffset
	sizeof(OOInst_CtrlEditText),// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
