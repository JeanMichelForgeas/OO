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

#define INDEX_OOA_Min 			0
#define INDEX_OOA_Max			1
#define INDEX_OOA_Number    	2
#define INDEX_OOA_Total    		3
#define INDEX_OOA_BoxType    	4
#define INDEX_OOA_TextPlace 	5
#define INDEX_OOA_NumDigits 	6
#define INDEX_OOA_VerifyFunc 	7

typedef LRESULT (CALLBACK*WINDOWPROC) (HWND, UINT, WPARAM, LPARAM);
typedef struct 
{
	WINDOWPROC OldEditProc;
}
OOInst_CtrlEditNum;


/******* Imported ************************************************/

extern void	Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight );
extern ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG	CtrlEditNum_HandleSystemEvent	( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti );
static void		CtrlEditNum_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlEditNum_AreaMinMax	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlEditNum_AreaStart	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static SLONG	CtrlEditNum_Get			( OObject *obj );
static void		CtrlEditNum_Set			( OObject *obj );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlEditNum( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_EDITNUM, OOV_TAG_MORE, (ULONG)attrlist };
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, inittags )) != NULL)
				{
				if (OOPrv_MNew( cl, obj, attrlist ) != OOV_OK)
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
			return CtrlEditNum_HandleSystemEvent( cl, obj, attrlist, (OOTagItem*)OOResult );

		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
			CtrlEditNum_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //********************* OOInst_Area*
			CtrlEditNum_AreaMinMax( cl, obj, attrlist );
			break;

		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlEditNum_AreaStart( cl, obj, attrlist );
			break;

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			{
			OOInst_Ctrl *ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
			OOInst_CtrlEditNum *inst = OOINST_DATA(cl,obj);
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

static ULONG CtrlEditNum_HandleSystemEvent( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti )
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

static ULONG verif_correct_val( OObject *obj, SLONG *valptr )
{
	OOTagList *attrs = OOINST_ATTRS(obj->Class,obj);
	ULONG (*func)( OObject *obj, SLONG *valptr );
	ULONG rc = OOV_OK;

	if ((func = attrs[INDEX_OOA_VerifyFunc].ti_Ptr) != NULL)
		{
		rc = func( obj, valptr );
		}
	if (*valptr < attrs[INDEX_OOA_Min].ti_Long || *valptr > attrs[INDEX_OOA_Max].ti_Long)
		{
		rc = OOV_ERROR;
		}
	return rc;
}

static ULONG ctrl2obj( OObject *obj )
{
	SLONG oldval, val = oldval = CtrlEditNum_Get( obj );
	ULONG rc = OOV_OK;

	if ((rc = verif_correct_val( obj, &val )) == OOV_OK)
		{
		OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
		OOTagItem change_tl[] = { OOA_Number, val, TAG_END };
		UBYTE buf[20];

		itoa( val, buf, 10 );
		SetWindowText( ctrl_inst->Win, buf ); // Corrige
		return OOPrv_MSet( obj->Class, obj, change_tl, OOF_MSET );
		}
	return rc;
}

static ULONG APIENTRY NewEditProc( HWND w, UINT msgid, UINT wparam, LONG lparam ) 
{
	OObject *obj = (OObject*) GetWindowLong( w, GWL_USERDATA );
	OOInst_CtrlEditNum *inst = OOINST_DATA(obj->Class,obj);

	switch (msgid) 
		{ 
		case WM_KILLFOCUS:
			if (ctrl2obj( obj ) != OOV_OK) { SetFocus( w ); return 0; }
			break;

		case WM_CHAR:
			{
			OOTagList *attrs = OOINST_ATTRS(obj->Class,obj);
			UBYTE ch = (UBYTE)wparam;

			if (ch >= '0' && ch <= '9')
				break;

			switch (ch)
				{
				case 0x08:
					break;

				case '-': 
					if (attrs[INDEX_OOA_Min].ti_Long >= 0) return 0;
					break;
				case '+': 
					if (attrs[INDEX_OOA_Max].ti_Long <= 0) return 0;
					break;

				case 0x1B: 
					CtrlEditNum_Set( obj );	// Annule entrée en remettant l'ancienne valeur stockée
					SetFocus( GetParent(w) );
					return 0;
				case '\n': 
				case '\r': 
					if (ctrl2obj( obj ) == OOV_OK) SetFocus( GetParent(w) );
					return 0;

				default: 
					return 0;
				}
			}
			break;
		}
	return CallWindowProc( inst->OldEditProc, w, msgid, wparam, lparam );
}

static void CtrlEditNum_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
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
	_S.W = (UWORD)(_S.W * attrs[INDEX_OOA_NumDigits].ti_Data);
	if (attrs[INDEX_OOA_Min].ti_Long < 0)
		{ OOSize _s; OOGfx_GetSizeOfString( ctrl_inst->Font, "+", 1, &_s ); _S.W += _s.W; }
	// _S.W += 2; si on ajoute pas 2 aux SubBorders
	Ctrl_AdaptBorders( obj, (UWORD)(_S.W + ar->_SubBorder.Left + ar->_SubBorder.Right), (UWORD)(_S.H + ar->_SubBorder.Top + ar->_SubBorder.Bottom) ); 
}

static void CtrlEditNum_AreaMinMax( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	OOInst_Area	*ar	= _DATA_AREA(obj);
	SWORD min;
	OOSize _S;

	OOGfx_GetSizeOfString( ctrl_inst->Font, "8", 1, &_S );
	_S.W = (UWORD)(_S.W * attrs[INDEX_OOA_NumDigits].ti_Data);
	if (attrs[INDEX_OOA_Min].ti_Long < 0)
		{ OOSize _s; OOGfx_GetSizeOfString( ctrl_inst->Font, "+", 1, &_s ); _S.W += _s.W; }
	//_S.W += 2; si on ajoute pas 2 aux subborders

	min = _S.W + ar->_Border.Left + ar->_Border.Right + ar->_SubBorder.Left + ar->_SubBorder.Right;
	if (ar->_MM.MinW < min) ar->_MM.MinW = min;
	min = _S.H + ar->_Border.Top + ar->_Border.Bottom + ar->_SubBorder.Top + ar->_SubBorder.Bottom;
	if (ar->_MM.MinH < min) ar->_MM.MinH = min;

	/*--- Obtention de _B.W et _B.H */
	if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
	if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;
}

static ULONG CtrlEditNum_AreaStart( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOInst_CtrlEditNum *inst = OOINST_DATA(cl,obj);
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
		CtrlEditNum_Set( obj );
		return OOV_OK;
		}
	return OOV_ERROR;
}

static SLONG CtrlEditNum_Get( OObject *obj )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	UBYTE buf[300] = "";

	if (ctrl_inst->Win != NULL)
		GetWindowText( ctrl_inst->Win, buf, 300 );

	return atoi( buf );
}

static void CtrlEditNum_Set( OObject *obj )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	if (ctrl_inst->Win != NULL)
		{
		OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
		OOTagItem	*attrs = OOINST_ATTRS(obj->Class,obj);
		UBYTE buf[20], dig;

		verif_correct_val( obj, &attrs[INDEX_OOA_Number].ti_Long );
		itoa( attrs[INDEX_OOA_Number].ti_Long, buf, 10 );
		SetWindowText( ctrl_inst->Win, buf );
		dig = (UBYTE)attrs[INDEX_OOA_NumDigits].ti_Data;
		if (attrs[INDEX_OOA_Min].ti_Long < 0) dig++;
		SendMessage( ctrl_inst->Win, EM_SETLIMITTEXT, dig, 0 );
		}
}

static void limfordigits( SLONG *min, SLONG *max, ULONG dig )
{
	UBYTE buf[11] = "9999999999";
	OOASSERT( dig <= 10 );
	if (*min < 0)
		{
		SLONG lim;
		if (dig == 10) lim = MAXSLONG;
		else { buf[dig] = 0; lim = atol( buf ); }
		if (*min < -lim) *min = -lim; else if (*min > lim) *min = lim;
		if (*max < -lim) *max = -lim; else if (*max > lim) *max = lim;
		}
	else{
		ULONG lim;
		if (dig == 10) lim = MAXULONG;
		else { buf[dig] = 0; lim = strtoul( buf, NULL, 10 ); }
		if (*(ULONG*)min > lim) *(ULONG*)min = lim;
		if (*(ULONG*)max > lim) *(ULONG*)max = lim;
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
			OOTagItem *attrs = OOINST_ATTRS(sgp->Class,sgp->Obj);
			SLONG min = attrs[INDEX_OOA_Min].ti_Long, max = attrs[INDEX_OOA_Max].ti_Long, num = attrs[INDEX_OOA_Number].ti_Long;
			ULONG tot = attrs[INDEX_OOA_Total].ti_Data, dig = attrs[INDEX_OOA_NumDigits].ti_Data;
			SLONG oldmin = min, oldmax = max, oldnum = num;
			ULONG oldtot = tot, olddig = dig;
			OOTagItem _TL[6], *ti = _TL;

			switch (sgp->ExtAttr->ti_Tag)
				{
				case OOA_Min:
					min = sgp->ExtAttr->ti_Long;
					limfordigits( &min, &max, dig );
					if (min > max) min = max;
					tot = max - min + 1;
					test_notify = TRUE;
					break;
				case OOA_Max:
					max = sgp->ExtAttr->ti_Long;
					limfordigits( &min, &max, dig );
					if (max < min) max = min;
					tot = max - min + 1;
					test_notify = TRUE;
					break;
				case OOA_Number:
					num = sgp->ExtAttr->ti_Long;
					if (num < min) num = min;
					else if (num > max) num = max;
					test_notify = TRUE;
					break;
				case OOA_Total:
					tot = sgp->ExtAttr->ti_Data;
					max = min + tot - 1;
					limfordigits( &min, &max, dig );
					tot = max - min + 1;
					test_notify = TRUE;
					break;
				case OOA_NumDigits:
					dig = sgp->ExtAttr->ti_Data;
					limfordigits( &min, &max, dig );
					if (max < min) max = min;
					tot = max - min + 1;
					test_notify = TRUE;
					break;

				case OOA_TextPlace:
					{
					OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,sgp->Obj);
					ULONG styles = GetWindowLong( ctrl_inst->Win, GWL_STYLE );
					styles &= ~(ES_LEFT|ES_RIGHT);
					styles |= (attrs[INDEX_OOA_TextPlace].ti_Data) == OOV_RIGHT ? ES_RIGHT : ES_LEFT;
					SetWindowLong( ctrl_inst->Win, GWL_STYLE, styles );
					}
					break;
				}

			if (test_notify == TRUE)
				{
				if (tot != oldtot)
					{
					if (num < min) num = min;
					else if (num > max) num = max;
					}

				attrs[INDEX_OOA_Min].ti_Long = min;
				attrs[INDEX_OOA_Max].ti_Long = max;
				attrs[INDEX_OOA_Number].ti_Long = num;
				attrs[INDEX_OOA_Total].ti_Data = tot;
				attrs[INDEX_OOA_NumDigits].ti_Data = dig;

				CtrlEditNum_Set( sgp->Obj );

				if (attrs[INDEX_OOA_Min].ti_Long != oldmin) { ti->ti_Tag = OOA_Min; ti->ti_Long = attrs[INDEX_OOA_Min].ti_Long; ti++; }
				if (attrs[INDEX_OOA_Max].ti_Long != oldmax) { ti->ti_Tag = OOA_Max; ti->ti_Long = attrs[INDEX_OOA_Max].ti_Long; ti++; }
				if (attrs[INDEX_OOA_Number].ti_Long != oldnum) { ti->ti_Tag = OOA_Number; ti->ti_Long = attrs[INDEX_OOA_Number].ti_Long; ti++; }
				if (attrs[INDEX_OOA_Total].ti_Data != oldtot) { ti->ti_Tag = OOA_Total; ti->ti_Data = attrs[INDEX_OOA_Total].ti_Data; ti++; }
				if (ti != _TL && (sgp->MethodFlag & OOF_MSET))
					{
					ti->ti_Tag32 = OOV_TAG_END;
					ti->ti_Ptr = NULL;
					if (OO_DoNotify( sgp->Obj, _TL ) == -1) return OOV_ERROR; // exécution de OOM_DELETE 
					}
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
    { OOA_Min		, 1					, "Min"		 , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT				},
    { OOA_Max		, MAXSLONG			, "Max"		 , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT				},
    { OOA_Number	, 1					, "Number"   , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT 			},
    { OOA_Total		, MAXSLONG			, "Total"	 , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT             },
    { OOA_BoxType	, OOV_BOX_HEAVYDOWN	, "BoxType"  , NULL		    , OOF_MNEW | OOF_MSET | OOF_MGET |           OOF_MAREA	},
    { OOA_TextPlace	, OOV_RIGHT			, "TextPlace", SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET						},
    { OOA_NumDigits	, 10				, "NumDigits", SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET |           OOF_MAREA	},
    { OOA_VerifyFunc, (ULONG)NULL		, "VerifyFunc",NULL			, OOF_MNEW | OOF_MSET | OOF_MGET						},
	{ TAG_END }
	};

OOClass Class_CtrlEditNum = { 
	Dispatcher_CtrlEditNum, 
	&Class_Ctrl,				// Base class
	"Class_CtrlEditNum",		// Class Name
	0,							// InstOffset
	sizeof(OOInst_CtrlEditNum),	// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
