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

#define INDEX_OOA_Text	    	0
#define INDEX_OOA_TextIndex	   	1
#define INDEX_OOA_TextPlace 	2
#define INDEX_OOA_BoxType    	3


/******* Imported ************************************************/

extern void	Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight );


/******* Exported ************************************************/


/******* Statics *************************************************/

static void		CtrlDisplayText_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlDisplayText_AreaMinMax	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlDisplayText_AreaRefresh	( OOClass *cl, OObject *obj, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlDisplayText( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_DISPTEXT, OOV_TAG_MORE, (ULONG)attrlist };
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
		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
			CtrlDisplayText_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //********************* OOInst_Area*
			CtrlDisplayText_AreaMinMax( cl, obj, attrlist );
			break;

		case OOM_AREA_REFRESH:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlDisplayText_AreaRefresh( cl, obj, attrlist );
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

static void get_text_size( OOFont *font, UBYTE *str, OOSize *size )
	{
	OOSize _S;
	UBYTE *p;
	ULONG num;

	size->W = size->H = 0;
	if (str == NULL || *str == 0) str = " ";

	for (p=str; ; p++)
		{
		if (*p == '\n' || *p == 0)
			{
			num = p - str;
			if (num == 0) { num = 1; str = " ";	}

			OOGfx_GetSizeOfString( font, str, num, &_S );
			if (_S.W > size->W)	size->W = _S.W;
			size->H += _S.H;

			if (*p == 0) break; else str = p+1;
			}
		}
	}

static void CtrlDisplayText_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOInst_Area	*ar		= _DATA_AREA(obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	OOSize _S;

	//--- Flags
	ar->Flags &= ~OOMASK_AINSIDEPLACE;
	ar->Flags |= OOF_AINSIDE_CENTERW | OOF_AINSIDE_CENTERH;

	//--- Obtention de _Border

	//--- Obtention de _SubBorder
	ar->_SubBorder = * OOGfx_GetBoxBorder( attrs[INDEX_OOA_BoxType].ti_Data );

	//--- Correction de _Border
	get_text_size( ctrl_inst->Font, attrs[INDEX_OOA_Text].ti_String, &_S );
	Ctrl_AdaptBorders( obj, (UWORD)(_S.W + ar->_SubBorder.Left + ar->_SubBorder.Right), (UWORD)(_S.H + ar->_SubBorder.Top + ar->_SubBorder.Bottom) ); 
}

static void CtrlDisplayText_AreaMinMax( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList	*attrs = OOINST_ATTRS(cl,obj);
	OOInst_Area	*ar	= _DATA_AREA(obj);
	SWORD min;
	OOSize _S;

	get_text_size( ctrl_inst->Font, attrs[INDEX_OOA_Text].ti_String, &_S );

	min = _S.W + ar->_Border.Left + ar->_Border.Right + ar->_SubBorder.Left + ar->_SubBorder.Right;
	if (ar->_MM.MinW < min) ar->_MM.MinW = min;
	min = _S.H + ar->_Border.Top + ar->_Border.Bottom + ar->_SubBorder.Top + ar->_SubBorder.Bottom;
	if (ar->_MM.MinH < min) ar->_MM.MinH = min;

	/*--- Obtention de _B.W et _B.H */
	if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
	if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;
}

static void draw_text( OOClass *cl, OObject *obj, OORastPort *rp )
	{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	OOInst_Area *ar		= _DATA_AREA(obj);
	UBYTE		*text	= attrs[INDEX_OOA_Text].ti_String;
	ULONG		place, flags = DT_NOCLIP;//|DT_SINGLELINE;
	RECT 		_R;
	HFONT		oldfont;

	place = attrs[INDEX_OOA_TextPlace].ti_Data;
	if (place & OOV_CENTER)	flags |= DT_CENTER|DT_VCENTER;
	if (place & OOV_LEFT)	{ flags |= DT_LEFT; flags &= ~DT_CENTER; }
	if (place & OOV_TOP)	{ flags |= DT_TOP; flags &= ~DT_VCENTER; }
	if (place & OOV_RIGHT)	{ flags |= DT_RIGHT; flags &= ~DT_CENTER; }
	if (place & OOV_BOTTOM)	{ flags |= DT_BOTTOM; flags &= ~DT_VCENTER; }
	/*
	switch (attrs[INDEX_OOA_TextPlace].ti_Data)
		{
		case OOV_LEFT	: flags |= DT_VCENTER|DT_LEFT	; break;
		case OOV_RIGHT	: flags |= DT_VCENTER|DT_RIGHT	; break;
		case OOV_TOP	: flags |= DT_CENTER|DT_TOP		; break;
		case OOV_BOTTOM	: flags |= DT_CENTER|DT_BOTTOM	; break;
		case OOV_CENTER	:
		default			: flags |= DT_CENTER|DT_VCENTER	; break;
		}
	*/
	_R.left		= ar->_B.X + ar->_Border.Left + ar->_SubBorder.Left;
	_R.top		= ar->_B.Y + ar->_Border.Top + ar->_SubBorder.Top;
	_R.right	= ar->_B.X + ar->_B.W - ar->_Border.Right - ar->_SubBorder.Right;
	_R.bottom	= ar->_B.Y + ar->_B.H - ar->_Border.Bottom - ar->_SubBorder.Bottom;
	if (rp == NULL) // If not called by refresh -> must erase background
		{
		rp = ar->RI->rp;
		FillRect( rp->DC, &_R, (HBRUSH)GetClassLong( ar->RI->wi, GCL_HBRBACKGROUND ) );
		}

	oldfont = SelectObject( rp->DC, ctrl_inst->Font );

	if (text == NULL) text = " ";
	SetBkMode( rp->DC, TRANSPARENT );
	DrawText( rp->DC, text, -1, &_R, flags );

	SelectObject( rp->DC, oldfont );
	}

static ULONG CtrlDisplayText_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	OORastPort *rp = attrlist[1].ti_Ptr;

	if (rp == NULL) rp = ar->RI->rp;
	OOGfx_DrawBox( rp,	ar->_B.X + ar->_Border.Left,
						ar->_B.Y + ar->_Border.Top,
						ar->_B.X + ar->_B.W - ar->_Border.Right,
						ar->_B.Y + ar->_B.H - ar->_Border.Bottom,
						attrs[INDEX_OOA_BoxType].ti_Data
						);
	draw_text( cl, obj, rp );

	return OOV_OK;
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
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_TextIndex:
				sgp->Attr->ti_Data = data;
				if (OOPrv_MSetBounceV( sgp, OOA_Text, OOStr_Get( data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_Text: 
				if (sgp->Attr->ti_Data != data)
					{
					sgp->Attr->ti_Data = data;
					if ((_DATA_AREA(sgp->Obj)->StatusFlags) & OOF_AREA_STARTED)
						draw_text( sgp->Class, sgp->Obj, NULL );
					change = OOV_CHANGED;
					}
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
    { OOA_Text		, (ULONG)NULL		, "Text"	 , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT 			},
    { OOA_TextIndex	, (ULONG)NULL		, "TextIndex", SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT 			},
    { OOA_TextPlace	, OOV_CENTER		, "TextPlace", NULL			, OOF_MNEW | OOF_MSET | OOF_MGET						},
    { OOA_BoxType	, OOV_BOX_LIGHTDOWN , "BoxType"  , NULL			, OOF_MNEW | OOF_MSET | OOF_MGET |           OOF_MAREA	},
	{ TAG_END }
	};

OOClass Class_CtrlDisplayText = { 
	Dispatcher_CtrlDisplayText, 
	&Class_Ctrl,					// Base class
	"Class_CtrlDisplayText", 		// Class Name
	0,								// InstOffset
	0,								// InstSize  : Taille de l'instance
	attrs,							// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance's own taglist
	};
