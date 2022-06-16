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
#include "oo_pictures.h"

#include "oo_gui_prv.h"

#define INDEX_OOA_ImageIndex   	0
#define INDEX_OOA_ImagePlace 	1
#define INDEX_OOA_BoxType     	2


/******* Imported ************************************************/

extern void	Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight );


/******* Exported ************************************************/


/******* Statics *************************************************/

static void		CtrlDisplayImage_AreaSetup	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlDisplayImage_AreaMinMax	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlDisplayImage_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlDisplayImage( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_DISPIMAGE, OOV_TAG_MORE, (ULONG)attrlist };
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, inittags )) != NULL)
				{
				ULONG titleplace = OOTag_GetData( attrlist, OOA_TitlePlace, OOV_TOP ); // Change default
				OOTagList *ctrl_attrs;
				if (OOPrv_MNew( cl, obj, attrlist ) != OOV_OK)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				else{
					ctrl_attrs = OOINST_ATTRS(&Class_Ctrl,obj);
					if (titleplace == OOV_CENTER) titleplace = OOV_TOP;
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
			CtrlDisplayImage_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //********************* OOInst_Area*
			CtrlDisplayImage_AreaMinMax( cl, obj, attrlist );
			break;

		case OOM_AREA_REFRESH:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlDisplayImage_AreaRefresh( cl, obj, attrlist );
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

static void CtrlDisplayImage_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Area	*ar		= _DATA_AREA(obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	OOSize _S;

	//--- Flags
	ar->Flags &= ~OOMASK_AINSIDEPLACE;
	ar->Flags |= OOF_AINSIDE_CENTERW | OOF_AINSIDE_CENTERH;

	//--- Obtention de _SubBorder
	ar->_SubBorder = * OOGfx_GetBoxBorder( attrs[INDEX_OOA_BoxType].ti_Data );

	//--- Correction de _Border
	OOPic_GetSize( OOINST_ATTR(cl,obj,INDEX_OOA_ImageIndex), &_S );
	Ctrl_AdaptBorders( obj, (UWORD)(_S.W + ar->_SubBorder.Left + ar->_SubBorder.Right), (UWORD)(_S.H + ar->_SubBorder.Top + ar->_SubBorder.Bottom) ); 
}

static void CtrlDisplayImage_AreaMinMax( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Area	*ar	= _DATA_AREA(obj);
	SWORD min;
	OOSize _S;

	OOPic_GetSize( OOINST_ATTR(cl,obj,INDEX_OOA_ImageIndex), &_S );

	min = _S.W + ar->_Border.Left + ar->_Border.Right + ar->_SubBorder.Left + ar->_SubBorder.Right;
	if (ar->_MM.MinW < min) ar->_MM.MinW = min;
	min = _S.H + ar->_Border.Top + ar->_Border.Bottom + ar->_SubBorder.Top + ar->_SubBorder.Bottom;
	if (ar->_MM.MinH < min) ar->_MM.MinH = min;

	/*--- Obtention de _B.W et _B.H */
	if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
	if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;
}

static void draw_image( OOClass *cl, OObject *obj, OORastPort *rp )
	{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	OOInst_Area *ar		= _DATA_AREA(obj);
	RECT 		_R;
	OOBox		_B;

	_R.left		= ar->_B.X + ar->_Border.Left + ar->_SubBorder.Left;
	_R.top		= ar->_B.Y + ar->_Border.Top + ar->_SubBorder.Top;
	_R.right	= ar->_B.X + ar->_B.W - ar->_Border.Right - ar->_SubBorder.Right;
	_R.bottom	= ar->_B.Y + ar->_B.H - ar->_Border.Bottom - ar->_SubBorder.Bottom;
	if (rp == NULL) // If not called by refresh -> must erase background
		{
		rp = ar->RI->rp;
		FillRect( rp->DC, &_R, (HBRUSH)GetClassLong( ar->RI->wi, GCL_HBRBACKGROUND ) );
		}
	SetBkMode( rp->DC, TRANSPARENT );
	_B.X = (UWORD)_R.left;
	_B.Y = (UWORD)_R.top;
	_B.W = (UWORD)(_R.right - _R.left);
	_B.H = (UWORD)(_R.bottom - _R.top);
	OOPic_Draw( attrs[INDEX_OOA_ImageIndex].ti_Data, rp, &_B, attrs[INDEX_OOA_ImagePlace].ti_Data );
	}

static ULONG CtrlDisplayImage_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist )
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
	draw_image( cl, obj, rp );

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
			case OOA_ImageIndex: 
				if (sgp->Attr->ti_Data != data)
					{
					sgp->Attr->ti_Data = data;
					if ((_DATA_AREA(sgp->Obj)->StatusFlags) & OOF_AREA_STARTED)
						draw_image( sgp->Class, sgp->Obj, NULL );
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
    { OOA_ImageIndex, 0					, "ImageIndex"	, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT 			},
    { OOA_ImagePlace, OOV_CENTER		, "ImagePlace"	, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET						},
    { OOA_BoxType	, OOV_BOX_LIGHTDOWN	, "BoxType"		, NULL		, OOF_MNEW | OOF_MSET | OOF_MGET |           OOF_MAREA	},
	{ TAG_END }
	};

OOClass Class_CtrlDisplayImage = { 
	Dispatcher_CtrlDisplayImage, 
	&Class_Ctrl,					// Base class
	"Class_CtrlDisplayImage", 		// Class Name
	0,								// InstOffset
	0,								// InstSize  : Taille de l'instance
	attrs,							// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance's own taglist
	};
