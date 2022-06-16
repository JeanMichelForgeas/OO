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

#include "oo_text.h"

#include "oo_gui_prv.h"

#ifdef _WIN32
#include <commctrl.h>
#endif

#define OOF_CTRLALLOCFONT	0x0001

#define INDEX_OOA_ControlType		0	
#define INDEX_OOA_ControlId			1
#define INDEX_OOA_Disabled			2
#define INDEX_OOA_Title 			3
#define INDEX_OOA_TitleIndex 		4    
#define INDEX_OOA_TitlePlace		5    
#define INDEX_OOA_FontName			6    
#define INDEX_OOA_FontHeight		7    
#define INDEX_OOA_FontHeightP100	8    
#define INDEX_OOA_FontBold			9    
#define INDEX_OOA_FontItalic		10   
#define INDEX_OOA_FontUnderline		11   
#define INDEX_OOA_FontFixed			12   
#define INDEX_OOA_FontVariable		13


/******* Imported ************************************************/


/******* Exported ************************************************/

ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Statics *************************************************/

static ULONG	Ctrl_HandleSystemEvent( OOInst_Ctrl *inst, OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		Ctrl_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		Ctrl_AreaMinMax	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		Ctrl_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_Ctrl( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
				{
				OOInst_Ctrl *inst = OOINST_DATA(cl,obj);
				BOOL ok = FALSE;
				ULONG id;
				// Init instance datas
				if ((id = OObjId_Obtain( obj, 0 )) != 0)
					{
					inst->Win	 = NULL;
					inst->Flags = 0;
					// Init default attrs & change defaults with new values
					if (OOPrv_MNew( cl, obj, attrlist ) == OOV_OK)
						{
						//OOTag_SetData( OOINST_ATTRS(cl,obj), OOA_ControlType, id );
						OOTag_SetData( OOINST_ATTRS(cl,obj), OOA_ControlId, id );
						ok = TRUE;
						}
					}
				if (ok == FALSE)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			return (ULONG)obj;

		case OOM_DELETE:
			{
			OOTagList *attrs = OOINST_ATTRS(cl,obj);
			OObjId_Release( attrs[INDEX_OOA_ControlId].ti_Data );
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
			{
			OOInst_Ctrl *inst = OOINST_DATA(cl,obj);
			return Ctrl_HandleSystemEvent( inst, cl, obj, attrlist );
			}
			break;

		case OOM_AREA_SETUP: /* Obtention de _Border et _SubBorder */
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;

			{
			OOInst_Ctrl *inst = OOINST_DATA(cl,obj);
			OORenderInfo *ri = (OORenderInfo*) attrlist[2].ti_Ptr;	// OOA_RenderInfo

			if ((inst->Font = OOGfx_OpenFont( ri->Font, OOINST_ATTRS(cl,obj) )) == NULL) break;
			if (inst->Font != ri->Font) inst->Flags |= OOF_CTRLALLOCFONT;
			OOGfx_FontToTagList( inst->Font, OOINST_ATTRS(cl,obj) );
			}

			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
			Ctrl_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //********************* OOInst_Area*
			Ctrl_AreaMinMax( cl, obj, attrlist );
			break;

		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			break;

		case OOM_AREA_REFRESH:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_RastPort );
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			Ctrl_AreaRefresh( cl, obj, attrlist );
			/*{
			OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);
			RECT _R;
			HDC dc;
			HPEN  oldpen;

			dc = GetDC( ar->RI->wi );
			oldpen = GetCurrentObject( dc, OBJ_PEN );  

			_R.left		= ar->_B.X;
			_R.top		= ar->_B.Y;
			_R.right	= ar->_B.X + ar->_B.W;
			_R.bottom	= ar->_B.Y + ar->_B.H;
			SelectObject( dc, GetStockObject(WHITE_PEN) );
			MoveToEx( dc, _R.left, _R.bottom-1, NULL );
			LineTo( dc, _R.right-1, _R.bottom-1 );
			LineTo( dc, _R.right-1, _R.top );
			LineTo( dc, _R.left, _R.top );
			LineTo( dc, _R.left, _R.bottom-1 );

			SelectObject( dc, oldpen );
			ReleaseDC( ar->RI->wi, dc );
			}*/
			break;

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			{
			OOInst_Ctrl *inst = OOINST_DATA(cl,obj);
			if (inst->Win != NULL)
				{
				//+++sync_gad2tags( obj, attrlist );
				DestroyWindow( inst->Win );
				inst->Win = NULL;
				}
			}
			OO_DoBaseMethod( cl, obj, method, attrlist );
			break;

		case OOM_AREA_CLEANUP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			{
			OOInst_Ctrl *inst = OOINST_DATA(cl,obj);
			if (inst->Flags & OOF_CTRLALLOCFONT)
				{
				OOGfx_CloseFont( inst->Font );
				inst->Font = NULL;
				inst->Flags &= ~OOF_CTRLALLOCFONT;
				}
			}
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

static ULONG Ctrl_HandleSystemEvent( OOInst_Ctrl *inst, OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_HandleGUI	*gui	= attrlist[0].ti_Ptr; // OOA_HandleGUI
	OOEventMsg			*msg	= attrlist[1].ti_Ptr; // OOA_EventMsg
	OOTagList			*notify	= attrlist[2].ti_Ptr; // OOA_TagList
	OOTagItem			*ti		= attrlist[3].ti_Ptr; // OOA_TagItem
	OOTagList			*attrs  = OOINST_ATTRS(cl,obj);
	OOInst_Area			*ar		= _DATA_AREA(obj);
	ULONG rc = OOV_OK;

	OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_HandleGUI && attrlist[1].ti_Tag == OOA_EventMsg && attrlist[2].ti_Tag == OOA_TagList && attrlist[3].ti_Tag == OOA_TagItem);

	switch (msg->message)
		{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
			ti->ti_Tag = OOA_MousePos;
			ti->ti_Data = W2L((SWORD)msg->pt.x,(SWORD)msg->pt.y);
			ti++;
			break;

		//------------------------------------------

		case WM_LBUTTONDOWN  :	case WM_NCLBUTTONDOWN  :
			ti->ti_Tag = OOA_ButLeftDown; ti->ti_Data = TRUE; ti++;
			break;
		case WM_LBUTTONUP    :	case WM_NCLBUTTONUP    :
			ti->ti_Tag = OOA_ButLeftUp; ti->ti_Data = TRUE; ti++;
			break;
		case WM_MBUTTONDOWN  :	case WM_NCRBUTTONDOWN  :
			ti->ti_Tag = OOA_ButMidDown; ti->ti_Data = TRUE; ti++;
			break;
		case WM_MBUTTONUP    :	case WM_NCRBUTTONUP    :
			ti->ti_Tag = OOA_ButMidUp; ti->ti_Data = TRUE; ti++;
			break;
		case WM_RBUTTONDOWN  :	case WM_NCMBUTTONDOWN  :
			ti->ti_Tag = OOA_ButRightDown; ti->ti_Data = TRUE; ti++;
			break;
		case WM_RBUTTONUP    :	case WM_NCMBUTTONUP    :
			ti->ti_Tag = OOA_ButRightUp; ti->ti_Data = TRUE; ti++;
			break;

		//------------------------------------------

		case WM_COMMAND:
			{
			UWORD notifycode = HIWORD(msg->wParam);
			UWORD ctrlid = LOWORD(msg->wParam);
			HWND ctrlwin = (HWND)msg->lParam;
			// ctrlwin peut ne pas être inst->Win, exemple du ctrl radio
			switch (notifycode)
				{
				case BN_CLICKED:
		            ti->ti_Tag = OOA_PressedButton; ti->ti_Data = TRUE; ti++;
					break;
				}
			}
			break;

		//------------------------------------------

        default: 
			break;
		} 

	ti->ti_Tag = OOA_Id; ti->ti_Data = attrs[INDEX_OOA_ControlId].ti_Data; ti++;
	OOResult = (OORESULT)ti;
    return rc;
}

static void Ctrl_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*inst	= OOINST_DATA(cl,obj);
	OOInst_Area	*ar		= _DATA_AREA(obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	UBYTE		*title	= attrs[INDEX_OOA_Title].ti_String;
	ULONG		place	= attrs[INDEX_OOA_TitlePlace].ti_Data;

	//--- Obtention de _Border
	if (title != NULL) 
		{
		OOGfx_GetSizeOfString( inst->Font, title, -1, &inst->TitleSize );
		switch (place)
			{
			case OOV_LEFT:
				ar->_Border.Left += inst->TitleSize.W + OOV_HORIZTITLESPACING;
				break;
			case OOV_RIGHT:
				ar->_Border.Right += inst->TitleSize.W + OOV_HORIZTITLESPACING;
				break;
			case OOV_TOP:
				ar->_Border.Top += inst->TitleSize.H + OOV_VERTTITLESPACING;
				break;
			case OOV_BOTTOM:
				ar->_Border.Bottom += inst->TitleSize.H + OOV_VERTTITLESPACING;
				break;
			}
		}
	ar->Flags &= ~OOMASK_AINSIDEPLACE;
	switch (place)
		{
		case OOV_LEFT:
			ar->Flags |= OOF_AINSIDE_RIGHT|OOF_AINSIDE_CENTERH;
			break;
		case OOV_RIGHT:
			ar->Flags |= OOF_AINSIDE_LEFT|OOF_AINSIDE_CENTERH;
			break;
		case OOV_TOP:
			ar->Flags |= OOF_AINSIDE_TOP|OOF_AINSIDE_CENTERW;
			break;
		case OOV_BOTTOM:
			ar->Flags |= OOF_AINSIDE_BOTTOM|OOF_AINSIDE_CENTERW;
			break;
		default: // OOV_CENTER
			ar->Flags |= OOF_AINSIDE_FULLW|OOF_AINSIDE_CENTERH;
			break;
		}

	//--- Obtention de _SubBorder
	ar->_SubBorder.Left = ar->_SubBorder.Right = 5;
	ar->_SubBorder.Top = ar->_SubBorder.Bottom = 3;
}

static void Ctrl_AreaMinMax( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*inst	= OOINST_DATA(cl,obj);
	OOInst_Area	*ar		= _DATA_AREA(obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	UBYTE		*title	= attrs[INDEX_OOA_Title].ti_String;
	ULONG		place	= attrs[INDEX_OOA_TitlePlace].ti_Data;
	SWORD min;

	/*--- Obtention de _MM */
	switch (place)
		{
		case OOV_LEFT:
		case OOV_RIGHT:
			if (ar->_MM.MinH < inst->TitleSize.H) ar->_MM.MinH = inst->TitleSize.H;
			break;
		case OOV_TOP:
		case OOV_BOTTOM:
			if (ar->_MM.MinW < inst->TitleSize.W) ar->_MM.MinW = inst->TitleSize.W;
			break;
		default: // OOV_CENTER
			min = inst->TitleSize.W + ar->_SubBorder.Left + ar->_SubBorder.Right;
			if (ar->_MM.MinW < min) ar->_MM.MinW = min;
			min = inst->TitleSize.H + ar->_SubBorder.Top + ar->_SubBorder.Bottom;
			if (ar->_MM.MinH < min) ar->_MM.MinH = min;
			break;
		}

	/*--- Obtention de _B.W et _B.H */
	if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
	if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;
}

static void Ctrl_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	UBYTE		*title	= attrs[INDEX_OOA_Title].ti_String;

	if (title != NULL)
		{
		OOInst_Ctrl	*inst	= OOINST_DATA(cl,obj);
		OOInst_Area	*ar		= _DATA_AREA(obj);
		ULONG		place	= attrs[INDEX_OOA_TitlePlace].ti_Data;
		ULONG x, y, w, h, flags = DT_NOCLIP|DT_SINGLELINE;
		ULONG ctrlwidth  = ar->_B.W - (ar->_Border.Left + ar->_Border.Right);
		ULONG ctrlheight = ar->_B.H - (ar->_Border.Top + ar->_Border.Bottom);
		RECT _R;
		OORastPort *rp = attrlist[1].ti_Ptr;
		HDC dc;
		COLORREF curr_textcolor;

		if (rp == NULL) dc = GetDC( ar->RI->wi ); else dc = rp->DC;
		if (obj->Class == &Class_CtrlRadio) curr_textcolor = SetTextColor( dc, GetSysColor(COLOR_3DHILIGHT) );
		switch (place)
			{
			case OOV_LEFT :	
				flags |= DT_VCENTER|DT_RIGHT;
				x = ar->_B.X;
				y = ar->_B.Y;
				w = ar->_Border.Left - OOV_HORIZTITLESPACING;
				h = ar->_B.H;
				break;
			case OOV_RIGHT:	
				flags |= DT_VCENTER|DT_LEFT;
				x = ar->_B.X + ctrlwidth + OOV_HORIZTITLESPACING;
				y = ar->_B.Y;
				w = ar->_Border.Right - OOV_HORIZTITLESPACING;
				h = ar->_B.H;
				break;
			case OOV_TOP:	
				flags |= DT_CENTER|DT_BOTTOM;
				x = ar->_B.X;
				y = ar->_B.Y;
				w = ar->_B.W;
				h = ar->_Border.Top - OOV_VERTTITLESPACING;
				break;
			case OOV_BOTTOM:	
				flags |= DT_CENTER|DT_TOP;
				x = ar->_B.X;
				y = ar->_B.Y + ctrlheight + OOV_VERTTITLESPACING;
				w = ar->_B.W;
				h = ar->_Border.Bottom - OOV_VERTTITLESPACING;
				break;
			case OOV_CENTER:
				flags |= DT_VCENTER|DT_CENTER;
				x = ar->_B.X + ar->_Border.Left;                      
				y = ar->_B.Y + ar->_Border.Top;                       
				w = ctrlwidth;
				h = ctrlheight;
				break;
			default:
				return;
			}
		SetBkMode( dc, TRANSPARENT );
		SelectObject( dc, inst->Font );
		_R.left		= x;
		_R.top		= y;
		_R.right	= x + w;
		_R.bottom	= y + h;
		DrawText( dc, title, -1, &_R, flags );
		if (obj->Class == &Class_CtrlRadio) SetTextColor( dc, curr_textcolor );
		if (rp == NULL) ReleaseDC( ar->RI->wi, dc );
		}
}


//------ Directly called by derived classes

// Called from OOM_AREA_SETUP
void Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight )
{
	OOInst_Ctrl	*inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList *attrs = OOINST_ATTRS(&Class_Ctrl,obj);
	OOInst_Area	*ar	= _DATA_AREA(obj);
	SWORD diff;

	switch (attrs[INDEX_OOA_TitlePlace].ti_Data)
		{
		case OOV_LEFT:
		case OOV_RIGHT:
			if (!(ar->Flags & OOF_AINSIDE_FULLH) && (diff = inst->TitleSize.H - innerheight) > 0)
				{
				ar->_Border.Top = diff / 2;
				ar->_Border.Bottom = diff - (diff / 2);
				}
			break;
		case OOV_TOP:
		case OOV_BOTTOM:
			if (!(ar->Flags & OOF_AINSIDE_FULLW) && (diff = inst->TitleSize.W - innerwidth) > 0)
				{
				ar->_Border.Left = diff / 2;
				ar->_Border.Right = diff - (diff / 2);
				}
			break;
		}
}

// Called from OOM_AREA_START
ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title )
{
	OOTagList *attrs = OOINST_ATTRS(&Class_Ctrl,obj);

	*place	= attrs[INDEX_OOA_TitlePlace].ti_Data;
	*title = attrs[INDEX_OOA_Title].ti_String;
	*styles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ((attrs[INDEX_OOA_Disabled].ti_Bool==TRUE)?WS_DISABLED:0);
	*extstyles = WS_EX_NOPARENTNOTIFY;

	return attrs[INDEX_OOA_ControlId].ti_Data;
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
	ULONG styles, data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_Disabled: 
				styles = GetWindowLong( inst->Win, GWL_STYLE );
				if ((styles & WS_DISABLED) && (data == FALSE)) { SetWindowLong( inst->Win, GWL_STYLE, (styles & ~WS_DISABLED) ); change = OOV_CHANGED; }
				else if (!(styles & WS_DISABLED) && (data == TRUE)) { SetWindowLong( inst->Win, GWL_STYLE, (styles | WS_DISABLED) ); change = OOV_CHANGED; }
				sgp->Attr->ti_Data = data; 
				break;

			case OOA_TitleIndex:
				sgp->Attr->ti_Data = data;
				if (OOPrv_MSetBounceV( sgp, OOA_Title, OOStr_Get( data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_Title: 
			case OOA_TitlePlace: 
				if (sgp->Attr->ti_Data != data) { sgp->Attr->ti_Data = data; change = OOV_CHANGED; }
				break;

			case OOA_FontName:
				if (OOTag_SetString( sgp->Attr, sgp->ExtAttr->ti_String ) != NULL) change = OOV_CHANGED;
				break;
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
	{ OOA_ControlType	, OOT_CTRL_UNKNOWN	, "ControlType"		, NULL		,						OOF_MGET					  },
	{ OOA_ControlId		, 0					, "ControlId"		, NULL		,						OOF_MGET					  },
	{ OOA_Disabled		, FALSE				, "Disabled"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT 		  },
	{ OOA_Title 		, (ULONG)NULL		, "Title"			, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_TitleIndex 	, 0					, "TitleIndex"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_TitlePlace	, OOV_CENTER		, "TitlePlace"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontName		, (ULONG)NULL		, "FontName"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontHeight	, 0 				, "FontHeight"		, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontHeightP100, 100 				, "FontHeightP100"	, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontBold		, FALSE				, "FontBold"		, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontItalic	, FALSE				, "FontItalic"		, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontUnderline	, FALSE				, "FontUnderline"	, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontFixed		, FALSE				, "FontFixed"		, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_FontVariable	, TRUE				, "FontVariable"	, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ TAG_END }
	};

OOClass Class_Ctrl = { 
	Dispatcher_Ctrl, 
	&Class_Area,			// Base class
	"Class_Ctrl", 			// Class Name
	0,						// InstOffset
	sizeof(OOInst_Ctrl),	// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
