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

#define INDEX_OOA_Min		   0
#define INDEX_OOA_Max		   1
#define INDEX_OOA_Pos		   2
#define INDEX_OOA_Total		   3
#define INDEX_OOA_Visible	   4
#define INDEX_OOA_Arrows	   5
#define INDEX_OOA_Vertical	   6
#define INDEX_OOA_Horizontal   7


/******* Imported ************************************************/

extern ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG	CtrlScroller_HandleSystemEvent( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti );
static void		CtrlScroller_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlScroller_AreaMinMax	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlScroller_AreaStart	( OOClass *cl, OObject *obj, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlScroller( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_SCROLLER, OOV_TAG_MORE, (ULONG)attrlist };
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
			return CtrlScroller_HandleSystemEvent( cl, obj, attrlist, (OOTagItem*)OOResult );

		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
			CtrlScroller_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //********************* OOInst_Area*
			CtrlScroller_AreaMinMax( cl, obj, attrlist );
			break;

		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlScroller_AreaStart( cl, obj, attrlist );
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

static ULONG CtrlScroller_HandleSystemEvent( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti )
{
	OOInst_HandleGUI	*gui	= attrlist[0].ti_Ptr; // OOA_HandleGUI
	OOEventMsg			*msg	= attrlist[1].ti_Ptr; // OOA_EventMsg
	OOTagList			*notify	= attrlist[2].ti_Ptr; // OOA_TagList
	OOInst_Area			*ar		= _DATA_AREA(obj);
	ULONG rc = OOV_OK;

	OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_HandleGUI && attrlist[1].ti_Tag == OOA_EventMsg && attrlist[2].ti_Tag == OOA_TagList && attrlist[3].ti_Tag == OOA_TagItem);

	switch (msg->message)
		{
		case WM_HSCROLL:
		case WM_VSCROLL:
			{
			UWORD		scrollcode	= LOWORD(msg->wParam);
			HWND		ctrl_win	= (HWND)msg->lParam;
			OOTagList	*attrs		= OOINST_ATTRS(cl,obj);
			SLONG		pos			= GetScrollPos( ctrl_win, SB_CTL );

			switch (scrollcode)
				{
				case SB_LINEUP			: pos -= 1; break;
				case SB_LINEDOWN		: pos += 1; break;
				case SB_PAGEUP			: pos -= attrs[INDEX_OOA_Visible].ti_Data; break;
				case SB_PAGEDOWN		: pos += attrs[INDEX_OOA_Visible].ti_Data; break;
				case SB_TOP				: pos = attrs[INDEX_OOA_Min].ti_Long; break;
				case SB_BOTTOM			: pos = attrs[INDEX_OOA_Max].ti_Long; break;
				case SB_ENDSCROLL		: break;
				case SB_THUMBPOSITION	: // Limitation à pos 16 bits si pris dans message
				case SB_THUMBTRACK		: pos = (SLONG)(SWORD)HIWORD(msg->wParam); break;
				}

			if (pos < attrs[INDEX_OOA_Min].ti_Long) pos = attrs[INDEX_OOA_Min].ti_Long;
			else if (pos > attrs[INDEX_OOA_Max].ti_Long) pos = attrs[INDEX_OOA_Max].ti_Long;
			if (pos != attrs[INDEX_OOA_Pos].ti_Long)
				{
				SetScrollPos( ctrl_win, SB_CTL, pos, TRUE );
				attrs[INDEX_OOA_Pos].ti_Long = pos;
				ti->ti_Tag = OOA_Pos; ti->ti_Long = pos; ti++;
				}
			}
			break;
		} 

	OOResult = (OORESULT)ti;
    return rc;
}

static void CtrlScroller_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Area	*ar		= _DATA_AREA(obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);

	//--- Flags
	if (attrs[INDEX_OOA_Vertical].ti_Bool == TRUE)
		 ar->Flags |= OOF_AINSIDE_FULLH | OOF_EXPANDH;
	else ar->Flags |= OOF_AINSIDE_FULLW | OOF_EXPANDW;

	//--- Obtention de _Border

	//--- Obtention de _SubBorder
	if (ar->Flags & OOF_AINSIDE_FULLH) 
		{
		UWORD VScroll_ArrowHeight = (UWORD) GetSystemMetrics( SM_CYVSCROLL );
		ar->_SubBorder.Left = ar->_SubBorder.Right = 0;
		ar->_SubBorder.Top = ar->_SubBorder.Bottom = VScroll_ArrowHeight;
		}
	else{
		UWORD HScroll_ArrowWidth = (UWORD) GetSystemMetrics( SM_CXHSCROLL );
		ar->_SubBorder.Top = ar->_SubBorder.Bottom = 0;
		ar->_SubBorder.Left = ar->_SubBorder.Right = HScroll_ArrowWidth;
		}
}

static void CtrlScroller_AreaMinMax( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*inst	= OOINST_DATA(&Class_Ctrl,obj);
	OOInst_Area	*ar		= _DATA_AREA(obj);
	SWORD minw, minh;
	UWORD VScroll_Width = (UWORD) GetSystemMetrics( SM_CXVSCROLL );
	UWORD HScroll_Height = (UWORD) GetSystemMetrics( SM_CYHSCROLL );

	/*--- Obtention de _MM */
    if (ar->Flags & OOF_AINSIDE_FULLH)
        {
        minw = ar->_Border.Left + ar->_Border.Right + ar->_SubBorder.Left + ar->_SubBorder.Right + VScroll_Width;
        minh = ar->_Border.Top + ar->_Border.Bottom + ar->_SubBorder.Top + ar->_SubBorder.Bottom + GetSystemMetrics(SM_CYVTHUMB);
        }
    else{
        minw = ar->_Border.Left + ar->_Border.Right + ar->_SubBorder.Left + ar->_SubBorder.Right + GetSystemMetrics(SM_CXHTHUMB);
        minh = ar->_Border.Top + ar->_Border.Bottom + ar->_SubBorder.Top + ar->_SubBorder.Bottom + HScroll_Height;
        }
    if (ar->_MM.MinW < minw) ar->_MM.MinW = minw;
    else if (ar->_MM.MinW > minw)
        {
        SWORD diff = (ar->_MM.MinW - minw) / 2;
        ar->_Border.Left += diff;
        ar->_Border.Right += diff;
        }
    if (ar->_MM.MinH < minh) ar->_MM.MinH = minh;
    else if (ar->_MM.MinH > minh)
        {
        SWORD diff = (ar->_MM.MinH - minh) / 2;
        ar->_Border.Top += diff;
        ar->_Border.Bottom += diff;
        }

	/*--- Obtention de _B.W et _B.H */
	// ar->_B.W = VScroll_Width;
	// ar->_B.H = HScroll_Height;
	if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
	if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;

	if (ar->Flags & OOF_AINSIDE_FULLH) 
			{ ar->_MM.MinW = ar->_B.W; } //+++ + ar->_Border.Left + ar->_Border.Right; }
	else	{ ar->_MM.MinH = ar->_B.H; } //+++ + ar->_Border.Top + ar->_Border.Bottom; }
}

static void CtrlScroller_Set( OOClass *cl, OObject *obj )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagItem	*attrs = OOINST_ATTRS(cl,obj);
	SLONG min = attrs[INDEX_OOA_Min].ti_Long, max = attrs[INDEX_OOA_Max].ti_Long, pos = attrs[INDEX_OOA_Pos].ti_Long;
	ULONG diff, tot = attrs[INDEX_OOA_Total].ti_Data, vis = attrs[INDEX_OOA_Visible].ti_Data;
	SCROLLINFO _SI;

	diff = max - min; if (tot > diff) tot = diff; else max = tot + min;
	diff = max - vis; if (pos > max) pos = max; else if (pos < min) pos = min;
	_SI.cbSize	= sizeof(SCROLLINFO);
	_SI.fMask	= SIF_ALL;
	_SI.nMin	= min;
	_SI.nMax	= max;
	_SI.nPage	= vis;
	_SI.nPos	= pos;
	SetScrollInfo( ctrl_inst->Win, SB_CTL, &_SI, TRUE );
	GetScrollInfo( ctrl_inst->Win, SB_CTL, &_SI );
	attrs[INDEX_OOA_Visible].ti_Data = _SI.nPage;
	attrs[INDEX_OOA_Pos].ti_Long = _SI.nPos;
	attrs[INDEX_OOA_Total].ti_Data = tot;
}

static ULONG CtrlScroller_AreaStart( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OORenderInfo *ri = _RENDERINFO(obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	UBYTE *title;
	ULONG id, place, styles, extstyles;

	if ((id = Ctrl_GetStartInfo( obj, attrlist, &place, &styles, &extstyles, &title )) == 0) 
		return OOV_ERROR;

	styles |= (ar->Flags & OOF_AINSIDE_FULLH) ? SBS_VERT : SBS_HORZ;
	if ((ctrl_inst->Win = CreateWindowEx( extstyles, "SCROLLBAR", title, styles, 
				ar->_B.X + ar->_Border.Left,
				ar->_B.Y + ar->_Border.Top,
				ar->_B.W - (ar->_Border.Left + ar->_Border.Right),
				ar->_B.H - (ar->_Border.Top + ar->_Border.Bottom),
				ri->wi, (void*)id, OOInstance, obj )) != NULL)
		{
		CtrlScroller_Set( cl, obj );
		return OOV_OK;
		}
	return OOV_ERROR;
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

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (sgp->ExtAttr->ti_Data != sgp->Attr->ti_Data) 
			{
			OOTagItem *attrs = OOINST_ATTRS(sgp->Class,sgp->Obj);
			SLONG min = attrs[INDEX_OOA_Min].ti_Long, max = attrs[INDEX_OOA_Max].ti_Long, pos = attrs[INDEX_OOA_Pos].ti_Long;
			ULONG tot = attrs[INDEX_OOA_Total].ti_Data, vis = attrs[INDEX_OOA_Visible].ti_Data;
			SLONG oldmin = min, oldmax = max, oldpos = pos;
			ULONG oldtot = tot, oldvis = vis;
			OOTagItem _TL[6], *ti = _TL;

			switch (sgp->ExtAttr->ti_Tag)
				{
				case OOA_Min:
					min = sgp->ExtAttr->ti_Long;
					if (min > max) min = max;
					tot = max - min + 1;
					break;
				case OOA_Max:
					max = sgp->ExtAttr->ti_Long;
					if (max < min) max = min;
					tot = max - min + 1;
					break;
				case OOA_Pos:
					pos = sgp->ExtAttr->ti_Long;
					if (pos < min) pos = min;
					else if (pos > max) pos = max;
					break;
				case OOA_Total:
					tot = sgp->ExtAttr->ti_Data;
					max = min + tot - 1;
					break;
				case OOA_Visible:
					vis = sgp->ExtAttr->ti_Data;
					if (vis > tot) vis = tot;
					break;
				}
			if (tot != oldtot)
				{
				if (pos < min) pos = min;
				else if (pos > max) pos = max;
				if (vis > tot) vis = tot;
				}
			attrs[INDEX_OOA_Min].ti_Long = min;
			attrs[INDEX_OOA_Max].ti_Long = max;
			attrs[INDEX_OOA_Pos].ti_Long = pos;
			attrs[INDEX_OOA_Total].ti_Data = tot;
			attrs[INDEX_OOA_Visible].ti_Data = vis;

			CtrlScroller_Set( sgp->Class, sgp->Obj );

			if (attrs[INDEX_OOA_Min].ti_Long != oldmin) 
				{ ti->ti_Tag = OOA_Min; ti->ti_Long = attrs[INDEX_OOA_Min].ti_Long; ti++; }
			if (attrs[INDEX_OOA_Max].ti_Long != oldmax) 
				{ ti->ti_Tag = OOA_Max; ti->ti_Long = attrs[INDEX_OOA_Max].ti_Long; ti++; }
			if (attrs[INDEX_OOA_Pos].ti_Long != oldpos) 
				{ ti->ti_Tag = OOA_Pos; ti->ti_Long = attrs[INDEX_OOA_Pos].ti_Long; ti++; }
			if (attrs[INDEX_OOA_Total].ti_Data != oldtot) 
				{ ti->ti_Tag = OOA_Total; ti->ti_Data = attrs[INDEX_OOA_Total].ti_Data; ti++; }
			if (attrs[INDEX_OOA_Visible].ti_Data != oldvis) 
				{ ti->ti_Tag = OOA_Visible; ti->ti_Data = attrs[INDEX_OOA_Visible].ti_Data; ti++; }
			if (ti != _TL && (sgp->MethodFlag & OOF_MSET))
				{
				ti->ti_Tag32 = OOV_TAG_END;
				ti->ti_Ptr = NULL;
				if (OO_DoNotify( sgp->Obj, _TL ) == -1) return OOV_ERROR; // exécution de OOM_DELETE 
				}
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return change;
}

static ULONG SetGet_HorVert( OOSetGetParms *sgp )
{
	OOInst_Ctrl *inst = (OOInst_Ctrl*)sgp->Instance;
	ULONG change = OOV_NOCHANGE;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (data != sgp->Attr->ti_Data) 
			{
			OOTagItem *attrs = OOINST_ATTRS(sgp->Class,sgp->Obj);
			switch (sgp->ExtAttr->ti_Tag)
				{
				case OOA_Vertical:
					if (data == TRUE) { attrs[INDEX_OOA_Vertical].ti_Bool = TRUE; attrs[INDEX_OOA_Horizontal].ti_Bool = FALSE; }
					else { attrs[INDEX_OOA_Vertical].ti_Bool = FALSE; attrs[INDEX_OOA_Horizontal].ti_Bool = TRUE; }
					break;
				case OOA_Horizontal:
					if (data == TRUE) { attrs[INDEX_OOA_Vertical].ti_Bool = FALSE; attrs[INDEX_OOA_Horizontal].ti_Bool = TRUE; }
					else { attrs[INDEX_OOA_Vertical].ti_Bool = TRUE; attrs[INDEX_OOA_Horizontal].ti_Bool = FALSE; }
					break;
				}
			change = OOV_CHANGED;
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
    { OOA_Min		, 0		, "Min"			, SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT           },
    { OOA_Max		, 0		, "Max"			, SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT           },
    { OOA_Pos		, 0		, "Pos"			, SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT           },
    { OOA_Total		, 1		, "Total"		, SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT           },
    { OOA_Visible	, 1		, "Visible"		, SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT           },
    { OOA_Arrows	, TRUE	, "Arrows"		, NULL			,			 OOF_MGET						| OOF_MAREA },
    { OOA_Vertical	, FALSE	, "Vertical"	, SetGet_HorVert, OOF_MNEW			  | OOF_MGET 			| OOF_MAREA },
    { OOA_Horizontal, TRUE	, "Horizontal"	, SetGet_HorVert, OOF_MNEW			  | OOF_MGET			| OOF_MAREA },
	{ TAG_END }
	};

OOClass Class_CtrlScroller = { 
	Dispatcher_CtrlScroller, 
	&Class_Ctrl,			// Base class
	"Class_CtrlScroller",	// Class Name
	0,						// InstOffset
	0,						// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
