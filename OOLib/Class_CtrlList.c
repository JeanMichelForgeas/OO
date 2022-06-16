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
#define INDEX_OOA_Pos		1
#define INDEX_OOA_Total		2
#define INDEX_OOA_List		3
#define INDEX_OOA_ListIndex	4
#define INDEX_OOA_Text		5
#define INDEX_OOA_ReadOnly	6
#define INDEX_OOA_TextWidth	7
#define INDEX_OOA_TextLines	8

typedef struct 
{
	ULONG	Pad;
}
OOInst_CtrlList;


/******* Imported ************************************************/

extern void  Ctrl_AdaptBorders( OObject *obj, UWORD innerwidth, UWORD innerheight );
extern ULONG Ctrl_GetStartInfo( OObject *obj, OOTagList *attrlist, ULONG *place, ULONG *styles, ULONG *extstyles, UBYTE **title );


/******* Exported ************************************************/


/******* Statics *************************************************/

/*
static ULONG	CtrlList_HandleSystemEvent	( OOClass *cl, OObject *obj, OOTagList *attrlist, OOTagItem *ti );
static void		CtrlList_AreaSetup		( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG	CtrlList_AreaStart		( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlList_AreaRefresh	( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void		CtrlList_AreaStop		( OOClass *cl, OObject *obj );
static void		CtrlList_Set			( OOClass *cl, OObject *obj, ULONG newsel );
static void		CtrlList_SetInfoSizes	( OOClass *cl, OObject *obj );
*/


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_CtrlList( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem inittags[2] = { OOA_ControlType, OOT_CTRL_LIST, OOV_TAG_MORE, (ULONG)attrlist };
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, inittags )) != NULL)
				{
				BOOL ok = FALSE;
				ULONG id = OO_GetAttr( obj, OOA_ControlId );
				if (id != 0)
					{
					OOInst_Ctrl	*ctrl_inst = OOINST_DATA(&Class_Ctrl,obj);
					ULONG extstyles = WS_EX_NOPARENTNOTIFY;
					ULONG styles = /*WS_CHILD | WS_VISIBLE | WS_TABSTOP |*/ WS_DISABLED;
					if ((ctrl_inst->Win = CreateWindowEx( extstyles, "LISTVIEW", NULL, styles,
						0, 0, 20, 20, NULL, (void*)id, OOInstance, obj )) != NULL)
						{
						if (OOPrv_MNew( cl, obj, attrlist ) == OOV_OK)
							{
							ok = TRUE;
							}
						}
					else{
						UBYTE *buf;
						FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
										NULL, GetLastError(),
										MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
										(LPTSTR)&buf, 0, NULL );
						OOTRACE( buf );
						LocalFree( buf );
						}
					}
				if (ok == FALSE)
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
		case OOM_HANDLEEVENT:
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK)	break;
			return OOV_OK; // CtrlList_HandleSystemEvent( cl, obj, attrlist, (OOTagItem*)OOResult );

		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break;//**********************
//			CtrlList_SetInfoSizes( cl, obj );
//			CtrlList_AreaSetup( cl, obj, attrlist );
			break;

		case OOM_AREA_START:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
//			CtrlList_AreaStart( cl, obj, attrlist );
			break;

		case OOM_AREA_REFRESH:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) != OOV_OK) break; //**********************	Ne pas empêcher l'ouverture de la fenêtre...
//			CtrlList_AreaRefresh( cl, obj, attrlist );
			break;

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			OO_DoBaseMethod( cl, obj, method, attrlist );
//			CtrlList_AreaStop( cl, obj );
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




















/*
        case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder
            if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
            if (OO_DoSuperMethodA( cl, obj, msg )) //**********************
                {
                OOInst_GadTools *id = OGTID(obj);
                OOInst_Area *ar = _DATA_AREA(obj);
                short min, fh;

                //--- Flags
                ar->Flags |= OOF_AINSIDE_FULLW | OOF_AINSIDE_FULLH | OOF_EXPANDW | OOF_EXPANDH;

                //--- Obtention de _Border

                //--- Obtention de _SubBorder
                min = 24 + OOStr_CharWidth( id->Font, 'M' );
                if (id->TL_OO[13].ti_Data == TRUE) ar->AskedW = 24 + get_listwidth( id ); // OOA_TextWidth
                ar->_SubBorder.Left = min / 2;
                ar->_SubBorder.Right = min - ar->_SubBorder.Left;

                fh = id->Font->tf_YSize;
                min = MAX(14,4+fh);
                if (id->TL_OO[14].ti_Data) // OOA_TextLines
                    {
                    ar->AskedH = 4 + (fh * id->TL_OO[14].ti_Data);
                    if (id->TL_OO[3].ti_Data && (id->TL_OO[4].ti_Data == OOV_ABOVE || id->TL_OO[4].ti_Data == OOV_BELOW))
                        ar->AskedH += 4 + fh;     // title && (above || below)
                    if (min > ar->AskedH) ar->AskedH = 0;
                    }
                ar->_SubBorder.Top = min / 2;
                ar->_SubBorder.Bottom = min - ar->_SubBorder.Top;
                }
            break;

        default:
            retval = OO_DoSuperMethodA( cl, obj, msg );
            break;
        }
  END_ERROR:
    return( retval );
}

#include <oo/prvoogui.h>

#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>

extern ULONG gtsubclass_new( OObject *obj, OOMsg *msg, OOTagItem *oomodel, OOTagItem *gtmodel, OOTagItem *maplist, UBYTE *flagslist );

//»»»»»»»» Définitions pour la partie portable

static OOTagItem __far def_oo_tags[] = {
    OOA_SysGad_Kind , LISTVIEW_KIND ,  // laisser en position 0
    OOA_GadId       , 0             ,  // laisser en position 1
    OOA_Disabled    , FALSE         ,  // laisser en position 2
    OOA_Title       , 0             ,  // laisser en position 3
    OOA_TitlePlace  , OOV_ABOVE     ,  // laisser en position 4
    OOA_FontName    , 0             ,  // laisser en position 5
    OOA_FontHeight  , 0             ,  // laisser en position 6
    OOA_List        , 0             ,  //  7
    OOA_Selected    ,~0             ,  //  8
    OOA_Pos         , 0             ,  //  9
    OOA_ReadOnly    , FALSE         ,  // 10
    OOA_Text        , 0             ,  // 11
    OOA_FontHeightP100, 0           ,  // 12
    OOA_TextWidth   , TRUE          ,  // 13
    OOA_TextLines   , 0             ,  // 14
    TAG_END
    };

static UBYTE __far def_oo_tagflags[] = {
                       OOF_GET,
                       OOF_GET,
    OOF_NEW | OOF_SET |OOF_GET | OOF_NOTIFY |            OOF_GTNEW | OOF_GTSET |OOF_GTGET,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA,
    OOF_NEW | OOF_SET |OOF_GET |                         OOF_GTNEW | OOF_GTSET |OOF_GTGET,
    OOF_NEW | OOF_SET |OOF_GET | OOF_NOTIFY |            OOF_GTNEW | OOF_GTSET |OOF_GTGET,
    OOF_NEW | OOF_SET |OOF_GET | OOF_NOTIFY |            OOF_GTNEW | OOF_GTSET |OOF_GTGET,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA | OOF_GTNEW,
    OOF_NEW | OOF_SET |OOF_GET | OOF_NOTIFY |            OOF_GTNEW | OOF_GTSET |OOF_GTGET,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA,
    OOF_NEW | OOF_SET |OOF_GET |              OOF_AREA,
    };

//»»»»»»»» Définitions pour la partie spécifique Amiga (GadTools)

static OOTagItem __far def_gt_tags[] = {
    GA_Disabled      ,  FALSE       ,
    GTLV_Labels      ,  0           ,
    GTLV_Selected    , ~0           ,
    GTLV_Top         ,  0           ,
    GTLV_ReadOnly    ,  FALSE       ,
    GTLV_ScrollWidth ,  16          ,
    GTLV_ShowSelected,  NULL        ,
    LAYOUTA_Spacing  ,  0           ,
  //GTLV_ItemHeight  ,
  //GTLV_CallBack    ,
  //GTLV_MaxPen      ,
    GT_Underscore    ,  (ULONG)'_'  ,
    GA_Immediate     ,  TRUE        ,
    GA_RelVerify     ,  TRUE        ,
    TAG_END
    };

static OOTagItem __far def_map_tags[] = {
    OOA_Disabled,   GA_Disabled     ,
    OOA_List    ,   GTLV_Labels     ,
    OOA_Selected,   GTLV_Selected   ,
    OOA_Pos     ,   GTLV_Top        ,
    OOA_ReadOnly,   GTLV_ReadOnly   ,
    TAG_END
    };

//***********************************************************************
//*                                  Code
//***********************************************************************

typedef struct {
    OONode  Node;
    UBYTE   *String;
} OOLVNode;

typedef struct {
    OOList  List;
    APTR    Pool;
} OOLVSupp;

static void delete_list( OOInst_GadTools *id )
{
    if (id->SubClassData)
        {
        OOMem_PoolDelete( ((OOLVSupp*)id->SubClassData)->Pool );
        id->SubClassData = 0;
        id->TL_OO[8].ti_Data = 0;
        id->TL_OO[11].ti_Data = 0;
        }
}

static ULONG add_line( APTR pool, OOList *list, UBYTE *str, ULONG len )
{
   OOLVNode *node;

    if (node = (OOLVNode*) OOMem_Alloc( pool, sizeof(OOLVNode) + len + 1 ))
        {
        node->String = (UBYTE*)(node+1);
        stccpy( node->String, str, len+1 );
        OOList_AddTail( list, (OONode*)node );
        }
    return( (ULONG)node );
}

static ULONG build_list( OOInst_GadTools *id, UBYTE *text )
{
  APTR pool=0;
  OOLVSupp *newlist;
  ULONG ok=0;
  UBYTE *ptr;

    if (text == (APTR)~0) // détache la liste du gadet
        {
        ok = 1;
        newlist = (APTR)~0;
        }
    else if (text && (pool = OOMem_PoolCreate( 1, 0 )))
        {
        if (newlist = (OOLVSupp*) OOMem_Alloc( pool, sizeof(OOLVSupp) ))
            {
            OOList_Init( newlist );
            newlist->Pool = pool;
            ok = 1;
            ptr = text;
            while (TRUE)
                {
                if (*ptr == '\n' || *ptr == 0)
                    {
                    if (! (ok = add_line( pool, newlist, text, ptr-text ))) break;
                    if (*ptr) text = ++ptr; else break;
                    }
                else{
                    if (*ptr) ptr++; else break;
                    }
                }
            }
        }

    if (ok)
        {
        if (id)
            {
            delete_list( id );
            if (newlist != (APTR)~0) id->SubClassData = newlist;
            }
        }
    else OOMem_PoolDelete( pool );

    return( ok ? (ULONG)newlist : 0 );
}

static void set_selected( OOInst_GadTools *id )
{
  ULONG i, active = id->TL_OO[8].ti_Data;
  OOLVNode *node, *new;
  OOLVSupp *list = id->SubClassData;
  UBYTE **ptext = (UBYTE**)&id->TL_OO[11].ti_Data;
  OOInst_Area *area = (OOInst_Area *)(((ULONG)id)-sizeof(OOInst_Area));
  ULONG len;

    if (*ptext && list && (active != ~0))
        {
        len = strlen(*ptext);
        for (i=0, node=(OOLVNode*)list->List.lh_Head; node->Node.ln_Succ; node = (OOLVNode*)node->Node.ln_Succ, i++)
            {
            if (i == active)
                {
                if (new = (OOLVNode*) OOMem_Alloc( list->Pool, sizeof(OOLVNode) + len + 1 ))
                    {
                    new->String = (UBYTE*)(new+1);
                    stccpy( new->String, *ptext, len+1 );
                    if (new->Node.ln_Pred = node->Node.ln_Pred) new->Node.ln_Pred->ln_Succ = new;
                    if (new->Node.ln_Succ = node->Node.ln_Succ) new->Node.ln_Succ->ln_Pred = new;
                    *ptext = new->String;
                    OOMem_Free( list->Pool, node );

                    if (area->StatusFlags & OOF_AREA_STARTED)
                        GT_SetGadgetAttrs( id->Gadget, area->RI->wi, area->RI->rq, GTLV_Labels, (ULONG)list, TAG_END ); // modifie le gadtool
                    }
                break;
                }
            }
        }
}

static ULONG get_selected( OOInst_GadTools *id )
{
  ULONG i, active=0;
  OOLVNode *node;
  OOList *list = id->SubClassData;
  OOInst_Area *area = (OOInst_Area *)(((ULONG)id)-sizeof(OOInst_Area));
  UBYTE **ptext = (UBYTE**)&id->TL_OO[11].ti_Data;

    if (id->Gadget) GT_GetGadgetAttrs( id->Gadget, area->RI->wi, area->RI->rq, GTLV_Selected, &active, TAG_END );
    id->TL_OO[8].ti_Data = active;
    *ptext = "";
    if (list && (active != ~0))
        {
        for (i=0, node=(OOLVNode*)list->lh_Head; node->Node.ln_Succ; node = (OOLVNode*)node->Node.ln_Succ, i++)
            { if (i == active) { *ptext = node->String; break; } }
        }
    return( active );
}

static ULONG get_listwidth( OOInst_GadTools *id )
{
  ULONG len, maxlen=0;
  OOLVNode *node;
  OOList *list = id->SubClassData;

    if (list)
        {
        for (node=(OOLVNode*)list->lh_Head; node->Node.ln_Succ; node = (OOLVNode*)node->Node.ln_Succ)
            {
            len = OOStr_Width( id->Font, node->String );
            if (len > maxlen) maxlen = len;
            }
        }
    return( maxlen );
}

//----------------------------------

static void HandleIMsg( OOInst_GadTools *id, OOMsg_GUIEvent *msg )
{
  OOTagItem *ti = msg->Current;
  struct IntuiMessage *imsg = msg->IMsg;

    switch (imsg->Class)
        {
        //case IDCMP_MOUSEMOVE:
        //case IDCMP_INTUITICKS:
        case IDCMP_GADGETUP:
            if (get_selected( id ) != -1)
                {
                *ti++ = id->TL_OO[8]; // OOA_Selected
                *ti++ = id->TL_OO[11]; // OOA_Text
                }
            break;
        }
    msg->Current = ti;
}
*/

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
//					CtrlEditText_Set( sgp->Obj );
					change = OOV_CHANGED;
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
    { OOA_Selected	, 0			 , "Selected" , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT				},
    { OOA_Pos		, 0			 , "Pos"	  , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT				},
    { OOA_Total		, 0			 , "Total"	  , SetGet_All	, 						OOF_MGET						},
    { OOA_List		, (ULONG)NULL, "List"	  , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT | OOF_MAREA },
    { OOA_ListIndex	, 0			 , "ListIndex", SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT | OOF_MAREA },
    { OOA_Text		, (ULONG)NULL, "Text"	  , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT				},
    { OOA_ReadOnly	, FALSE		 , "ReadOnly" , SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET |			  OOF_MAREA },
    { OOA_TextWidth	, TRUE		 , "TextWidth", SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET |			  OOF_MAREA },
    { OOA_TextLines	, 0			 , "TextLines", SetGet_All	, OOF_MNEW | OOF_MSET | OOF_MGET |			  OOF_MAREA },
	{ TAG_END }
	};

OOClass Class_CtrlList = { 
	Dispatcher_CtrlList, 
	&Class_Ctrl,				// Base class
	"Class_CtrlList",			// Class Name
	0,							// InstOffset
	sizeof(OOInst_CtrlList),	// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance's own taglist
	};
