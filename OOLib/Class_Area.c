/*****************************************************************
 *
 *       Project:    OO
 *       Function:   Base class for GUI elements
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


//****** Includes ***********************************************

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_pool.h"

#include "oo_gui_prv.h"


//******* Imported ***********************************************


//******* Exported ***********************************************


//******* Statics ************************************************


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_Area( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_Area *inst = OOINST_DATA(cl,obj);
				// Init instance datas
                inst->AskedW = inst->AskedH = 0;
                inst->_MM.MaxW = inst->_MM.MaxH = MAXSWORD;
				// Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
            OOInst_Area *inst = _DATA_AREA(obj);
            if (inst->RI != NULL)
                {
                if (inst->RI->wi != NULL) 
					OO_DoMethodV( obj, OOM_AREA_STOP, OOA_Flags, 0, TAG_END );
                OO_DoMethodV( obj, OOM_AREA_CLEANUP, OOA_Flags, 0, TAG_END );
                }
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

        case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder 
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_HandleGUI && attrlist[2].ti_Tag == OOA_RenderInfo );
            {
            OOInst_Area *inst = _DATA_AREA(obj);
            inst->GUI = (OObject*) attrlist[1].ti_Ptr;		// OOA_HandleGUI
            inst->RI = (OORenderInfo*) attrlist[2].ti_Ptr;	// OOA_RenderInfo
			OOASSERT( inst->GUI != NULL && inst->RI != NULL );

            inst->Flags &= OOMASK_KEEPAFLAGS;
            if (inst->Flags & OOF_MEMEXPANDW) inst->Flags |= OOF_EXPANDW;
            if (inst->Flags & OOF_MEMEXPANDH) inst->Flags |= OOF_EXPANDH;
            if (inst->Flags & OOF_MEMFULLW  ) inst->Flags |= OOF_AINSIDE_FULLW ;
            if (inst->Flags & OOF_MEMFULLH  ) inst->Flags |= OOF_AINSIDE_FULLH ;
            if (inst->Flags & OOF_MEMSAMEW  ) inst->Flags |= OOF_SAMEW  ;
            if (inst->Flags & OOF_MEMSAMEH  ) inst->Flags |= OOF_SAMEH  ;

            inst->_Border.Left = inst->_Border.Right = inst->_Border.Top = inst->_Border.Bottom = 0;
            inst->_SubBorder.Left = inst->_SubBorder.Right = inst->_SubBorder.Top = inst->_SubBorder.Bottom = 0;

            inst->StatusFlags |= OOF_AREA_SETUP;
            }
            break;

        case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
            if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)
                {
                OOInst_Area *inst = _DATA_AREA(obj);

                //--- Obtention de _MM 
                inst->_MM.MinW = inst->_Border.Left + inst->_SubBorder.Left + inst->_SubBorder.Right + inst->_Border.Right;
                inst->_MM.MinH = inst->_Border.Top + inst->_SubBorder.Top + inst->_SubBorder.Bottom + inst->_Border.Bottom;
                inst->_MM.MaxW = inst->_MM.MaxH = MAXSWORD;

                //--- Obtention de _B.W et _B.H 
                inst->_B.W = MAX(inst->_MM.MinW,inst->AskedW);
                inst->_B.H = MAX(inst->_MM.MinH,inst->AskedH);

				//--- Correction 
				if (!(inst->Flags & OOF_EXPANDW) && (inst->_MM.MinW < inst->_B.W)) inst->_MM.MinW = inst->_B.W;
				if (!(inst->Flags & OOF_EXPANDH) && (inst->_MM.MinH < inst->_B.H)) inst->_MM.MinH = inst->_B.H;

                OOResult = (OORESULT)inst;
                }
            else{
				OOASSERT( FALSE );
				OOResult = (OORESULT)NULL;
				return OOV_ERROR;
				}
			break;

        case OOM_AREA_SAMERI:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_RenderInfo );
            {
            OOInst_Area *inst = _DATA_AREA(obj);
            OOResult = (inst->RI == attrlist->ti_Ptr) ? (OORESULT)inst : (OORESULT)NULL;
            }
			break;

		case OOM_AREA_ISGROUP:
			OOResult = (OORESULT)NULL;
			break;

        case OOM_AREA_BOX: // retaille _B 
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_OOBox );
            if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)
                {
                OOInst_Area *inst = _DATA_AREA(obj);
                OOBox *b = &inst->_B;
				OOBox *lim = (OOBox*) attrlist[1].ti_Ptr; // OOA_OOBox
                SWORD diff;
                if (lim != NULL)
                    {
                    if (inst->Flags & OOF_ANEWRENDERINFO)
						{
						inst->Flags &= ~(OOF_AINSIDE_FULLW|OOF_AINSIDE_FULLH);
						}
					else{
                        b->X = lim->X; b->Y = lim->Y;
                        if (b->W > lim->W) { b->W = MAX(lim->W,inst->_MM.MinW); }
                        if (b->H > lim->H) { b->H = MAX(lim->H,inst->_MM.MinH); }
                        }

                    diff = lim->W - b->W;
                    if (inst->Flags & OOF_AINSIDE_FULLW)  b->W += diff;
                    if (b->W < inst->_MM.MinW)            b->W = inst->_MM.MinW;
                    else if (b->W > inst->_MM.MaxW)       b->W = inst->_MM.MaxW;

                    diff = lim->W - b->W;
                    if (inst->Flags & OOF_AINSIDE_LEFT)         ;
                    else if (inst->Flags & OOF_AINSIDE_RIGHT)   b->X += diff;
                    else if (inst->Flags & OOF_AINSIDE_CENTERW) b->X += diff / 2;

                    diff = lim->H - b->H;
                    if (inst->Flags & OOF_AINSIDE_FULLH)  b->H += diff;
                    if (b->H < inst->_MM.MinH)            b->H = inst->_MM.MinH;
                    else if (b->H > inst->_MM.MaxH)       b->H = inst->_MM.MaxH;

                    diff = lim->H - b->H;
                    if (inst->Flags & OOF_AINSIDE_TOP)          ;
                    else if (inst->Flags & OOF_AINSIDE_BOTTOM)  b->Y += diff;
                    else if (inst->Flags & OOF_AINSIDE_CENTERH) b->Y += diff / 2;
                    }
                OOResult = (OORESULT)inst;
                }
            else{
				OOASSERT( FALSE );
				OOResult = (OORESULT)NULL;
				return OOV_ERROR;
				}
            break;

        case OOM_AREA_START:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
            {
            OOInst_Area *inst = _DATA_AREA(obj);
            inst->StatusFlags |= OOF_AREA_STARTED;
            }
			break;

        case OOM_AREA_REFRESH:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_RastPort );
            break;

        case OOM_AREA_STOP:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
            _DATA_AREA(obj)->StatusFlags &= ~OOF_AREA_STARTED;
            break;

        case OOM_AREA_CLEANUP:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
            {
            OOInst_Area *inst = _DATA_AREA(obj);
            inst->RI = 0;
            inst->StatusFlags &= ~OOF_AREA_SETUP;
            }
            break;

        //---------------

        case OOM_AREA_SIGNALCHANGE:
            {
            OOInst_Area *inst = _DATA_AREA(obj);
		    if (inst->GUI != NULL) 
				OO_DoMethodV( inst->GUI, OOM_AREA_CHANGED, OOA_Flags, 0, OOA_Object, obj, TAG_END );
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
 *      Utilities
 *
 ***************************************************************************/


/***************************************************************************
 *
 *      Attributes setting
 *
 ***************************************************************************/

// If a SetGet function is given to the attribute, the function has perhaps to change
//	the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same than the 
//	current attr value.

static ULONG SetGet_All( OOSetGetParms *sgp )
{
	OOInst_Area *inst = (OOInst_Area*)sgp->Instance;
	ULONG flag = 0, change = OOV_NOCHANGE;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
        switch (sgp->ExtAttr->ti_Tag)
            {
            case OOA_LeftTop       :
                if (*((ULONG*)&inst->_B.X) != data) 
					{ change = OOV_CHANGED; *((ULONG*)&inst->_B.X) = data; }
                break;
            case OOA_WidthHeight   :
                if (*((ULONG*)&inst->_B.W) != data) 
					{ change = OOV_CHANGED; *((ULONG*)&inst->_B.W) = *((ULONG*)&inst->AskedW) = data;}
                break;
            case OOA_MinWidthHeight:
                if (*((ULONG*)&inst->_MM.MinW) != data) 
					{ change = OOV_CHANGED; *((ULONG*)&inst->_MM.MinW) = data; }
                break;
            case OOA_MaxWidthHeight:
                if (*((ULONG*)&inst->_MM.MaxW) != data) 
					{ change = OOV_CHANGED; *((ULONG*)&inst->_MM.MaxW) = data; }
                break;
            case OOA_Weight        :
                if (inst->Weight != data) 
					{ change = OOV_CHANGED; inst->Weight = (UBYTE)data; }
                break;

			case OOA_ExpandWidth	: flag = OOF_MEMEXPANDW ; break;
			case OOA_ExpandHeight	: flag = OOF_MEMEXPANDH ; break;
			case OOA_FullWidth		: flag = OOF_MEMFULLW	; break;
			case OOA_FullHeight		: flag = OOF_MEMFULLH	; break;
			case OOA_SameWidth		: flag = OOF_MEMSAMEW	; break;
			case OOA_SameHeight		: flag = OOF_MEMSAMEH	; break;
			}

		if (flag)
			{
			if (data == TRUE)
				 { change = (inst->Flags & flag) ? OOV_NOCHANGE : OOV_CHANGED ; inst->Flags |= flag; }
			else { change = (inst->Flags & flag) ? OOV_CHANGED	: OOV_NOCHANGE; inst->Flags &= ~flag; }
			sgp->Attr->ti_Data = data;
			}
		}
	else{
        switch (sgp->ExtAttr->ti_Tag)
            {
            case OOA_LeftTop       : (*(ULONG*)data) = *((ULONG*)&inst->_B.X); break;
            case OOA_WidthHeight   : (*(ULONG*)data) = *((ULONG*)&inst->_B.W); break;
            case OOA_MinWidthHeight: (*(ULONG*)data) = *((ULONG*)&inst->_MM.MinW); break;
            case OOA_MaxWidthHeight: (*(ULONG*)data) = *((ULONG*)&inst->_MM.MaxW); break;
            case OOA_Weight        : (*(ULONG*)data) = (ULONG)inst->Weight; break;
            case OOA_RenderInfo    : (*(ULONG*)data) = (ULONG)inst->RI; break;
            case OOA_HandleGUI     : (*(ULONG*)data) = (ULONG)inst->GUI; break;
            case OOA_RastPort      : (*(ULONG*)data) = (ULONG)inst->RI->rp; break;

			default: *(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr; break;
            }
		}
	return change;
}

/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_LeftTop       , 0				,"LeftTop"       , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_WidthHeight   , 0				,"WidthHeight"   , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_MinWidthHeight, 0				,"MinWidthHeight", SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_MaxWidthHeight, 0				,"MaxWidthHeight", SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_Weight        , 0				,"Weight"        , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_ExpandWidth   , 0				,"ExpandWidth"   , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_ExpandHeight  , 0				,"ExpandHeight"  , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_FullWidth		, 0				,"FullWidth"	 , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_FullHeight	, 0				,"FullHeight"	 , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_SameWidth     , 0				,"SameWidth"     , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_SameHeight    , 0				,"SameHeight"    , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |        OOF_MAREA },
    { OOA_RenderInfo    , (ULONG)NULL	,"RenderInfo"    , SetGet_All,                       OOF_MGET                    },
    { OOA_HandleGUI     , (ULONG)NULL	,"HandleGUI"     , SetGet_All,                       OOF_MGET                    },
    { OOA_RastPort      , (ULONG)NULL	,"RastPort"      , SetGet_All,                       OOF_MGET                    },
    { TAG_END }
	};

OOClass Class_Area = { 
	Dispatcher_Area, 
	&Class_Root,		// Base class
	"Class_Area",		// Class Name
	0,					// InstOffset
	sizeof(OOInst_Area),// InstSize  : Taille de l'instance
	attrs,				// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
