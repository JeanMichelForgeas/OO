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
#include "oo_pool.h"
#include "oo_text.h"

#include "oo_gui_prv.h"

#define INDEX_OOA_Selected	0
#define INDEX_OOA_Total	   	1
#define INDEX_OOA_List	   	2
#define INDEX_OOA_ListIndex 3
#define INDEX_OOA_Text	   	4
#define INDEX_OOA_TextPlace	5

#define OOV_RADIOVSPACING	4
#define OOV_RADIOHSPACING	8
#define OOV_SYSRADIOWIDTH	13
#define OOV_SYSRADIOHEIGHT	12

typedef struct 
{
	UBYTE	*Text;
	OOSize	Size;
	HWND	Win;
}
OORadioInfo;

typedef struct 
{
	OOSize		TextSize; // Size of the text part
	OOSize		CtrlSize; // Size of total ctrl without title : Radio buttons + TextSize
	OORadioInfo	*InfoTable;
}
OOInst_CtrlRadio;


/******* Imported ************************************************/

extern void  Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight );
extern ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG	CtrlRadio_HandleSystemEvent	( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti );
static void		CtrlRadio_AreaSetup		( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlRadio_AreaStart		( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlRadio_AreaRefresh	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlRadio_AreaStop		( OOClass *cl, OObject *obj );
static void		CtrlRadio_Set			( OOClass *cl, OObject *obj, ULONG newsel );
static ULONG	CtrlRadio_CreateInfo	( OOClass *cl, OObject *obj, UBYTE *newlist );
static void		CtrlRadio_DeleteInfo	( OOClass *cl, OObject *obj );
static void		CtrlRadio_SetInfoSizes	( OOClass *cl, OObject *obj );

static UBYTE *DefaultRadioList = "A\0B\0";


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlRadio( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_RADIO, OOV_TAG_MORE, (ULONG)attrlist };
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, inittags )) != NULL)
				{
				BOOL ok = FALSE;
				if (OOPrv_MNew( cl, obj, attrlist ) == OOV_OK)
					{
					OOTagList *attrs = OOINST_ATTRS(cl,obj);
					if (CtrlRadio_CreateInfo( cl, obj, attrs[INDEX_OOA_List].ti_String ) == OOV_OK)
						ok = TRUE;
					}
				if (ok == FALSE)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			}
			return (ULONG)obj;

		case OOM_DELETE:
			CtrlRadio_DeleteInfo( cl, obj );
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
			return CtrlRadio_HandleSystemEvent( cl, obj, attrlist, (OOTagItem*)OOResult );

		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
			CtrlRadio_SetInfoSizes( cl, obj );
			CtrlRadio_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlRadio_AreaStart( cl, obj, attrlist );
			break;

		case OOM_AREA_REFRESH:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
			CtrlRadio_AreaRefresh( cl, obj, attrlist );
			break;

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			OO_DoBaseMethod( cl, obj, method, attrlist );
			CtrlRadio_AreaStop( cl, obj );
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

static ULONG CtrlRadio_HandleSystemEvent( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti )
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
					OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
					OOTagList *attrs = OOINST_ATTRS(cl,obj);
					OORadioInfo *rain;
					ULONG index, total;
					for (rain=inst->InfoTable, index=0, total=attrs[INDEX_OOA_Total].ti_Data; index < total; index++, rain++)
						if (rain->Win == ctrlwin)
							{
							if (index != attrs[INDEX_OOA_Selected].ti_Data)
								{
								CtrlRadio_Set( cl, obj, index );
								ti->ti_Tag = OOA_Selected; ti->ti_Data = attrs[INDEX_OOA_Selected].ti_Data; ti++;
								ti->ti_Tag = OOA_Text; ti->ti_String = attrs[INDEX_OOA_Text].ti_String; ti++;
								}
							break;
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

static void CtrlRadio_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	OOInst_Area	*ar	= _DATA_AREA(obj);
	ULONG textplace = attrs[INDEX_OOA_TextPlace].ti_Data;

	//--- Correction de _Border
	Ctrl_AdaptBorders( obj, inst->CtrlSize.W, inst->CtrlSize.H ); 

	//--- Correction des flags de boutons standards
	switch (textplace)
		{
		case OOV_LEFT:
			ar->Flags &= ~(OOF_AINSIDE_LEFT);
			ar->Flags |= OOF_AINSIDE_RIGHT;
			break;
		case OOV_RIGHT:
			ar->Flags &= ~(OOF_AINSIDE_RIGHT);
			ar->Flags |= OOF_AINSIDE_LEFT;
			break;
		}

	//--- Obtention de _SubBorder
	ar->_SubBorder.Left   = inst->CtrlSize.W / 2;
	ar->_SubBorder.Right  = inst->CtrlSize.W - (inst->CtrlSize.W / 2);
	ar->_SubBorder.Top	  = inst->CtrlSize.H / 2;
	ar->_SubBorder.Bottom = inst->CtrlSize.H - (inst->CtrlSize.H / 2);
}

static ULONG CtrlRadio_AreaStart( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	OORenderInfo *ri = _RENDERINFO(obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	ULONG id, place, styles, extstyles;
	ULONG index, total, rc = OOV_OK;
	UBYTE *title;
	UWORD x, y, xoffs, yoffs;
	SWORD diff;

	if ((id = Ctrl_GetStartInfo( obj, attrlist, &place, &styles, &extstyles, &title )) == 0) 
		return OOV_ERROR;

	styles |= BS_RADIOBUTTON;
	x = ar->_B.X + ar->_Border.Left;
	xoffs = (attrs[INDEX_OOA_TextPlace].ti_Data == OOV_LEFT) ? inst->TextSize.W + OOV_HORIZTITLESPACING : 0;
	y = ar->_B.Y + ar->_Border.Top;
	if ((diff = inst->InfoTable[0].Size.H - OOV_SYSRADIOHEIGHT) > 0) y += (diff / 2);
	yoffs = MAX(inst->InfoTable[0].Size.H,OOV_SYSRADIOHEIGHT) + OOV_RADIOVSPACING;

	for (index=0, total=attrs[INDEX_OOA_Total].ti_Data; index < total; index++)
		{
		if ((inst->InfoTable[index].Win = CreateWindowEx( extstyles, "BUTTON", NULL, styles,  
		x + xoffs, y, OOV_SYSRADIOWIDTH, OOV_SYSRADIOHEIGHT, ri->wi, (void*)id, OOInstance, obj )) == NULL)
			{ rc = OOV_ERROR; break; }
		y += yoffs;
		}

	if (rc == OOV_OK)
		CtrlRadio_Set( cl, obj, attrs[INDEX_OOA_Selected].ti_Data );

	return rc;
}

static void CtrlRadio_AreaRefresh( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	OORadioInfo *rain;
	ULONG index, total, rc = OOV_OK;
	UWORD x, y, yoffs;
	SWORD diff;
	OORastPort *rp = attrlist[1].ti_Ptr;
	HDC dc;

	x = ar->_B.X + ar->_Border.Left;
	x += (attrs[INDEX_OOA_TextPlace].ti_Data == OOV_LEFT) ? 0 : OOV_SYSRADIOWIDTH + OOV_HORIZTITLESPACING;
	y = ar->_B.Y + ar->_Border.Top;
	if ((diff = OOV_SYSRADIOHEIGHT - inst->InfoTable[0].Size.H) > 0)
		{
		y += (OOV_SYSRADIOHEIGHT / 2);
		yoffs = OOV_SYSRADIOHEIGHT + OOV_RADIOVSPACING;
		}
	else{ 
		y += (inst->InfoTable[0].Size.H / 2);
		yoffs = inst->InfoTable[0].Size.H + OOV_RADIOVSPACING;
		}

	if (rp == NULL) dc = GetDC( ar->RI->wi ); else dc = rp->DC;
	SetBkMode( dc, TRANSPARENT );
	SelectObject( dc, ctrl_inst->Font );
	for (rain=inst->InfoTable, index=0, total=attrs[INDEX_OOA_Total].ti_Data; index < total; index++, rain++)
		{
		RECT _R;
		UBYTE *title = rain->Text;
		ULONG flags = DT_NOCLIP|DT_SINGLELINE;
		switch (attrs[INDEX_OOA_TextPlace].ti_Data)
			{
			case OOV_LEFT :	
				flags |= DT_VCENTER|DT_RIGHT;
				break;
			case OOV_RIGHT:	
				flags |= DT_VCENTER|DT_LEFT;
				break;
			case OOV_TOP:	
			case OOV_BOTTOM:	
			case OOV_CENTER:
			default:
				flags = 0;
				break;
			}
		if (flags)
			{
			_R.left		= x;
			_R.top		= y - (rain->Size.H / 2);
			_R.right	= _R.left + inst->TextSize.W;
			_R.bottom	= _R.top + rain->Size.H;
			DrawText( dc, title, -1, &_R, flags );
			}
		y += yoffs;
		}
	if (rp == NULL) ReleaseDC( ar->RI->wi, dc );
}

static void CtrlRadio_AreaStop( OOClass *cl, OObject *obj )
{
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOTagList *attrs = OOINST_ATTRS(cl,obj);
	ULONG index, total;

	for (index=0, total=attrs[INDEX_OOA_Total].ti_Data; index < total; index++)
		{
		DestroyWindow( inst->InfoTable[index].Win );
		inst->InfoTable[index].Win = NULL;
		}
}

static void CtrlRadio_Set( OOClass *cl, OObject *obj, ULONG newsel )
{
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOTagItem *attrs = OOINST_ATTRS(cl,obj);
	ULONG oldsel = attrs[INDEX_OOA_Selected].ti_Data;
	ULONG total = attrs[INDEX_OOA_Total].ti_Data;

	if (inst->InfoTable != NULL)
		{
		if (oldsel < total)
			if (inst->InfoTable[oldsel].Win != NULL) SendMessage( inst->InfoTable[oldsel].Win, BM_SETCHECK, BST_UNCHECKED, 0 );
		if (newsel < total)
			{
			if (inst->InfoTable[newsel].Win != NULL) SendMessage( inst->InfoTable[newsel].Win, BM_SETCHECK, BST_CHECKED, 0 );
			attrs[INDEX_OOA_Text].ti_String = inst->InfoTable[newsel].Text;
			}
		}
	attrs[INDEX_OOA_Selected].ti_Data = newsel;
}

//---------------- Multi info manager

static ULONG CtrlRadio_CreateInfo( OOClass *cl, OObject *obj, UBYTE *newlist )
{
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOTagItem *attrs = OOINST_ATTRS(cl,obj);
	OORadioInfo *rain;
	UWORD numtitles, len;
	UBYTE *ptr;

	if (newlist == NULL) newlist = attrs[INDEX_OOA_List].ti_String = DefaultRadioList;

	//------ Number of titles & alloc list
	for (numtitles=0, ptr=newlist; (len=strlen(ptr)) != 0; numtitles++, ptr+=len+1) ;
	if ((rain = OOPool_Alloc( OOGlobalPool, numtitles*sizeof(OORadioInfo) )) == NULL)
		return OOV_ERROR;
	//------ Delete previous table
	if (inst->InfoTable != NULL) CtrlRadio_DeleteInfo( cl, obj );
	//------ Replace with new allocated one
	inst->InfoTable = rain;
	attrs[INDEX_OOA_List].ti_String = newlist;
	attrs[INDEX_OOA_Total].ti_Data = numtitles;
	//------ Store titles ptrs and sizes
	for (ptr=newlist; (len=strlen(ptr)) != 0; ptr+=len+1, rain++)
		{
		rain->Text = ptr;
		rain->Win = NULL;
		}

	return OOV_OK;
}

static void CtrlRadio_DeleteInfo( OOClass *cl, OObject *obj )
{
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOTagItem *attrs = OOINST_ATTRS(cl,obj);

	CtrlRadio_AreaStop( cl, obj );
	inst->InfoTable = OOPool_Free( OOGlobalPool, inst->InfoTable );
	attrs[INDEX_OOA_Total].ti_Data = 0;
}

static void CtrlRadio_SetInfoSizes( OOClass *cl, OObject *obj )
{
	OOInst_CtrlRadio *inst = OOINST_DATA(cl,obj);
	OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
	OOTagItem *attrs = OOINST_ATTRS(cl,obj);
	UWORD total, index;

	//------ Store titles ptrs and sizes
	inst->TextSize.W = inst->TextSize.H = 0;
	inst->CtrlSize.W = inst->CtrlSize.H = 0;
	for (index=0, total=(UWORD)attrs[INDEX_OOA_Total].ti_Data; index < total; index++)
		{
		OORadioInfo *rain = &inst->InfoTable[index];
		OOGfx_GetSizeOfString( ctrl_inst->Font, rain->Text, -1, &rain->Size );
		if (inst->TextSize.W < rain->Size.W) inst->TextSize.W = rain->Size.W;
		inst->TextSize.H += rain->Size.H;
		}
	inst->TextSize.H += ((total-1) * OOV_RADIOVSPACING);
	//------ Total control size
	inst->CtrlSize.W = OOV_SYSRADIOWIDTH + OOV_HORIZTITLESPACING + inst->TextSize.W;
	inst->CtrlSize.H = (total * OOV_SYSRADIOHEIGHT) + ((total-1) * OOV_RADIOVSPACING);
	if (inst->CtrlSize.H < inst->TextSize.H) inst->CtrlSize.H = inst->TextSize.H;
}


/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

static ULONG SetGet_Selected( OOSetGetParms *sgp )
{
	ULONG change = OOV_NOCHANGE;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (data != sgp->Attr->ti_Data) 
			{
			change = OOV_CHANGED;
			CtrlRadio_Set( sgp->Class, sgp->Obj, data );
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return change;
}

static ULONG SetGet_List( OOSetGetParms *sgp )
{
	ULONG change = OOV_NOCHANGE;
	UBYTE *newlist = sgp->ExtAttr->ti_String;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_ListIndex:
				sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
				if (OOPrv_MSetBounceV( sgp, OOA_List, OOStr_Get( sgp->ExtAttr->ti_Data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_List: 
				if (newlist != sgp->Attr->ti_String) 
					{
					if (CtrlRadio_CreateInfo( sgp->Class, sgp->Obj, newlist ) == OOV_OK)
						{
						// Appelé par OOM_AREA_SETUP (invokée OOF_MAREA ) CtrlRadio_SetInfoSizes( sgp->Class, sgp->Obj );
						change = OOV_CHANGED;
						}
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
    { OOA_Selected	, 0			 , "Selected" , SetGet_Selected	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT				},
    { OOA_Total		, 0			 , "Total"	  , NULL			, 						OOF_MGET						},
    { OOA_List		, (ULONG)NULL, "List"	  , SetGet_List		, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT | OOF_MAREA },
    { OOA_ListIndex	, 0			 , "ListIndex", SetGet_List		, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT | OOF_MAREA },
    { OOA_Text		, (ULONG)NULL, "Text"	  , NULL			,						OOF_MGET | OOF_MNOT				},
    { OOA_TextPlace	, OOV_RIGHT	 , "TextPlace", NULL			, OOF_MNEW | OOF_MSET | OOF_MGET |			  OOF_MAREA },
	{ TAG_END }
	};

OOClass Class_CtrlRadio = { 
	Dispatcher_CtrlRadio, 
	&Class_Ctrl,				// Base class
	"Class_CtrlRadio",			// Class Name
	0,							// InstOffset
	sizeof(OOInst_CtrlRadio),	// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance's own taglist
	};
