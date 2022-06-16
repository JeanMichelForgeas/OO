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


//****** Includes ***********************************************

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_text.h"

#include "oo_gui_prv.h"

#define INDEX_OOA_BoxType			0
#define INDEX_OOA_Title 			1
#define INDEX_OOA_TitleIndex 		2
#define INDEX_OOA_TitlePlace		3
#define INDEX_OOA_FontName			4   
#define INDEX_OOA_FontHeight		5   
#define INDEX_OOA_FontHeightP100	6   
#define INDEX_OOA_FontBold			7   
#define INDEX_OOA_FontItalic		8   
#define INDEX_OOA_FontUnderline		9   
#define INDEX_OOA_FontFixed			10  
#define INDEX_OOA_FontVariable		11  


//****** Imported ***********************************************


//****** Exported ***********************************************


//****** Statics ************************************************

static void		AreaGroupBox_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	AreaGroupBox_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_AreaGroupBox( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
				{
				OOInst_AreaGroupBox *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				inst->Font  = NULL;
				inst->Flags = 0;
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
		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder 
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			{
			OOInst_AreaGroupBox *inst = OOINST_DATA(cl,obj);
			OORenderInfo *ri = (OORenderInfo*) attrlist[2].ti_Ptr;	// OOA_RenderInfo

			if ((inst->Font = OOGfx_OpenFont( ri->Font, OOINST_ATTRS(cl,obj) )) == NULL) break;
			if (inst->Font != ri->Font) inst->Flags |= OOF_GRPBOX_ALLOCFONT;
			OOGfx_FontToTagList( inst->Font, OOINST_ATTRS(cl,obj) );
			}
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) == OOV_ERROR)return OOV_ERROR;
			AreaGroupBox_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_REFRESH:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_RastPort );
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			AreaGroupBox_AreaRefresh( cl, obj, attrlist );
			return OO_DoBaseMethod( cl, obj, method, attrlist );

		case OOM_AREA_CLEANUP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			OO_DoBaseMethod( cl, obj, method, attrlist );
			{
			OOInst_AreaGroupBox *inst = OOINST_DATA(cl,obj);
			if (inst->Flags & OOF_GRPBOX_ALLOCFONT)
				{
				OOGfx_CloseFont( inst->Font );
				inst->Font = NULL;
				inst->Flags &= ~OOF_GRPBOX_ALLOCFONT;
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
 *		Utilities
 *
 ***************************************************************************/

static void AreaGroupBox_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_AreaGroupBox *inst = OOINST_DATA(cl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	OOInst_Area	*ar = _DATA_AREA(obj);
	UBYTE *title = attrs[INDEX_OOA_Title].ti_String;
	ULONG place	= attrs[INDEX_OOA_TitlePlace].ti_Data;
	OOSize _s;
	OOBorder _b;

	//--- Obtention de _SubBorder
	_b = * OOGfx_GetBoxBorder( attrs[INDEX_OOA_BoxType].ti_Data );
	ar->_SubBorder.Left	  += _b.Left  ;
	ar->_SubBorder.Right  += _b.Right ;
	ar->_SubBorder.Top    += _b.Top   ;
	ar->_SubBorder.Bottom += _b.Bottom;

	//--- Obtention de _Border
	if (title != NULL) 
		{
		OOGfx_GetSizeOfString( inst->Font, title, -1, &_s );
		switch (place)
			{
			case OOV_LEFT:
				ar->_Border.Left = _s.W + OOV_HORIZTITLESPACING;
				break;
			case OOV_RIGHT:
				ar->_Border.Right = _s.W + OOV_HORIZTITLESPACING;
				break;
			case OOV_TOP:
				ar->_Border.Top = _s.H + OOV_VERTTITLESPACING;
				break;
			case OOV_BOTTOM:
				ar->_Border.Bottom = _s.H + OOV_VERTTITLESPACING;
				break;
			default: // A cheval sur le bord supérieur
				ar->_Border.Top = (_s.H + 1) / 2;
				if (ar->_SubBorder.Top < _s.H - ar->_Border.Top) 
					ar->_SubBorder.Top = _s.H - ar->_Border.Top;
				break;
			}
		}
}

static ULONG AreaGroupBox_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	UBYTE		*title	= attrs[INDEX_OOA_Title].ti_String;
	OOInst_Area *ar = _DATA_AREA(obj);
	OORastPort  *rp = attrlist[1].ti_Ptr;
	OOSize _s;

	if (rp == NULL) rp = ar->RI->rp;

	OOGfx_DrawBox( rp,	ar->_B.X + ar->_Border.Left,
						ar->_B.Y + ar->_Border.Top,
						ar->_B.X + ar->_B.W - ar->_Border.Right,
						ar->_B.Y + ar->_B.H - ar->_Border.Bottom,
						attrs[INDEX_OOA_BoxType].ti_Data
						);

	if (title != NULL)
		{
		OOInst_AreaGroupBox *inst = OOINST_DATA(cl,obj);
		ULONG place = attrs[INDEX_OOA_TitlePlace].ti_Data;
		ULONG x, y, w, h, flags = DT_NOCLIP|DT_SINGLELINE;
		ULONG groupwidth  = ar->_B.W - (ar->_Border.Left + ar->_Border.Right);
		ULONG groupheight = ar->_B.H - (ar->_Border.Top + ar->_Border.Bottom);
		RECT _R;
		COLORREF color;

		switch (place)
			{
			case OOV_LEFT:	
				flags |= DT_VCENTER|DT_RIGHT;
				x = ar->_B.X;
				y = ar->_B.Y;
				w = ar->_Border.Left - OOV_HORIZTITLESPACING;
				h = ar->_B.H;
				break;
			case OOV_RIGHT:	
				flags |= DT_VCENTER|DT_LEFT;
				x = ar->_B.X + groupwidth + OOV_HORIZTITLESPACING;
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
				y = ar->_B.Y + groupheight + OOV_VERTTITLESPACING;
				w = ar->_B.W;
				h = ar->_Border.Bottom - OOV_VERTTITLESPACING;
				break;
			default: // A cheval sur le bord supérieur
				flags = DT_VCENTER|DT_LEFT|DT_SINGLELINE; // CLIP
				x = ar->_B.X + ar->_Border.Left + 10;                      
				y = ar->_B.Y;                       
				w = groupwidth - 10;
				OOGfx_GetSizeOfString( inst->Font, title, -1, &_s );
				h = _s.H;
				break;
			}
		SetBkMode( rp->DC, OPAQUE );
		color = SetBkColor( rp->DC, GetSysColor(COLOR_3DFACE) );
		SelectObject( rp->DC, inst->Font );
		_R.left		= x;
		_R.top		= y;
		_R.right	= x + w;
		_R.bottom	= y + h;
		DrawText( rp->DC, title, -1, &_R, flags );
		SetBkColor( rp->DC, color );
		}
	return OOV_OK;
}


/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

// If a SetGet function is given to the attribute, the function has perhaps to change
//	the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same of the 
//	current attr value.

static ULONG SetGet_All( OOSetGetParms *sgp )
{
	OOInst_AreaGroupBox *inst = (OOInst_AreaGroupBox*)sgp->Instance;
	ULONG change = OOV_NOCHANGE, data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		data = sgp->ExtAttr->ti_Data;
		switch (sgp->ExtAttr->ti_Tag)
			{
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
    { OOA_BoxType		, OOV_BOX_GROOVEDOWN, "BoxType"			, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET |            OOF_MAREA	},
	{ OOA_Title 		, (ULONG)NULL		, "Title"			, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_TitleIndex 	, 0					, "TitleIndex"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
	{ OOA_TitlePlace	, 0					, "TitlePlace"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			| OOF_MAREA },
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

OOClass Class_AreaGroupBox = { 
	Dispatcher_AreaGroupBox, 
	&Class_AreaGroup,			// Base class
	"Class_AreaGroupBox",		// Class Name
	0,							// InstOffset
	sizeof(OOInst_AreaGroupBox),// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
