/*****************************************************************
 *
 *       Project:    OO
 *       Function:
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
#include "oo_3d_prv.h"
#include "oo_gfx.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#define INDEX_D3A_OnOff     0

#define OOM_CLIPMASK		0x1f
#define OOF_CLIPLEFT		0x01
#define OOF_CLIPRIGHT		0x02
#define OOF_CLIPTOP 		0x04
#define OOF_CLIPBOT 		0x08
#define OOF_CLIPBACK		0x10


//****** Imported ***********************************************


//****** Exported ***********************************************


//****** Statics ************************************************

static void InitVisible( OOInst_3DCamera *inst );
static void CleanupVisible( OOInst_3DCamera *inst );
static void AddVisibleList( OObject *camobj, OOInst_3DCamera *inst, OOTagList *attrlist );
static void DrawnScene( OObject *camobj, OOInst_3DCamera *inst, OOTagList *attrlist );


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_3DCamera( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_3DCamera *inst = OOINST_DATA(cl,obj);
                // Init instance datas
                inst->WindowWidth2  = (FLOAT).02;
                inst->WindowHeight2 = (FLOAT).012;
                inst->ObserverDist  = (FLOAT).13;
                InitVisible( inst );
                // Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
            OOInst_3DCamera *inst = OOINST_DATA(cl,obj);
			CleanupVisible( inst );
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
        case D3M_OBJTOCAMERA:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort && attrlist[1].ti_Tag == D3A_ObjTable && attrlist[2].ti_Tag == D3A_ObjCount );
            if (OOINST_ATTR(cl,obj,INDEX_D3A_OnOff) == TRUE)
                {
                OOInst_3DCamera *inst = OOINST_DATA(cl,obj);
                AddVisibleList( obj, inst, attrlist ); // D3A_RastPort, D3A_ObjTable, D3A_ObjCount
                }
            break;

        case D3M_DRAW:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort && attrlist[1].ti_Tag == OOA_WidthHeight );
            if (OOINST_ATTR(cl,obj,INDEX_D3A_OnOff) == TRUE)
                {
                DrawnScene( obj, OOINST_DATA(cl,obj), attrlist );
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

#define MAXLIFECOUNT	100

typedef struct _OOCamObjInfo 
	{
	OONode		Node;
    OObject     *Obj;
    UBYTE       Flags;
    UBYTE       LifeCount;
    UWORD       NumVertices;
	OO3DPoint	*Vertices;
	UBYTE		*Map;
    }
	OOCamObjInfo;

static OOCamObjInfo *ObjInfo_SearchOldObj( OOInst_3DCamera *inst, OObject *obj )
{
	OOCamObjInfo *oi, *succ;

    for (oi=(OOCamObjInfo*)inst->VisOldList->lh_Head; succ = (OOCamObjInfo*)oi->Node.ln_Succ; oi = succ)
        {
		if (oi->Obj == obj)
			{
			OOList_Remove( (OONode*)oi );
			return oi;
			}
        }
    for (oi=(OOCamObjInfo*)inst->VisWaitList.lh_Head; succ = (OOCamObjInfo*)oi->Node.ln_Succ; oi = succ)
        {
		if (oi->Obj == obj)
			{
			OOList_Remove( (OONode*)oi );
			return oi;
			}
        }
	return NULL;
}

static OOCamObjInfo *ObjInfo_Create( OObject *obj )
{
	OOInst_3DSolid *so = D3SOL(obj);
	UWORD numvertices = (so->Type != NULL) ? so->Type->NumVertices : 0;
	ULONG size = sizeof(OOCamObjInfo) + numvertices * (sizeof(UBYTE) + sizeof(OO3DPoint));
	OOCamObjInfo *oi;

	if ((oi = OOPool_Alloc( OOGlobalPool, size )) == NULL)
		return NULL;

	oi->Obj = obj;
	oi->NumVertices = numvertices;
	oi->Vertices = (OO3DPoint*) ((UBYTE*)oi + sizeof(OOCamObjInfo));
	oi->Map = (UBYTE*)oi + sizeof(OOCamObjInfo) + numvertices * sizeof(OO3DPoint);

	return oi;
}

static void ObjInfo_Delete( OOCamObjInfo *oi )
{
	OOPool_Free( OOGlobalPool, oi );
}

static OOCamObjInfo *ObjInfo_Resize( OOCamObjInfo *oi )
{
	OObject *obj = oi->Obj;
	OOInst_3DSolid *so = D3SOL(obj);
	UWORD numvertices = (so->Type != NULL) ? so->Type->NumVertices : 0;
	ULONG newsize = sizeof(OOCamObjInfo) + numvertices * (sizeof(UBYTE) + sizeof(OO3DPoint));
	ULONG oldsize = OOPool_GetLength( oi );

	if (newsize <= oldsize)
		{
		oi->NumVertices = numvertices;
		return oi;
		}

	ObjInfo_Delete( oi ); // Puisque le type a changé on delete de toute façon
	return ObjInfo_Create( obj );
}

static void ObjInfo_SwapLists( OOInst_3DCamera *inst )
{
	//------ Current devient Old
	if (inst->VisCurList == &inst->VisList1)
		{
		inst->VisCurList = &inst->VisList2;
		inst->VisOldList = &inst->VisList1;
		}
	else{
		inst->VisCurList = &inst->VisList1;
		inst->VisOldList = &inst->VisList2;
		}
}

static void ObjInfo_OldToWait( OOInst_3DCamera *inst )
{
	OOCamObjInfo *oi, *succ;

	//------ Décrémente/Purge les anciens nodes dans la WaitList
    for (oi=(OOCamObjInfo*)inst->VisWaitList.lh_Head; succ = (OOCamObjInfo*)oi->Node.ln_Succ; oi = succ)
		{
		if (--oi->LifeCount == 0)
			{
			OOList_Remove( (OONode*)oi );
			ObjInfo_Delete( oi );
			}
		}

	//------ Met les nodes restants de la OldList dans la WaitList
    for (oi=(OOCamObjInfo*)inst->VisOldList->lh_Head; succ = (OOCamObjInfo*)oi->Node.ln_Succ; oi = succ)
		{
		oi->LifeCount = MAXLIFECOUNT;
		OOList_Remove( (OONode*)oi );
		OOList_AddTail( &inst->VisWaitList, (OONode*)oi );
		}
}

//===========================================================================

// Appelée depuis OOM_NEW
static void InitVisible( OOInst_3DCamera *inst )
{
	OOList_Init( &inst->VisList1 );
	OOList_Init( &inst->VisList2 );
	OOList_Init( &inst->VisWaitList );
	inst->VisCurList = &inst->VisList1;
	inst->VisOldList = &inst->VisList2;
}

// Appelée depuis OOM_DELETE
static void CleanupVisible( OOInst_3DCamera *inst )
{
	OONode *node, *succ;

    for (node=inst->VisList1.lh_Head; succ = node->ln_Succ; node = succ)
		ObjInfo_Delete( (OOCamObjInfo*)node );

    for (node=inst->VisList2.lh_Head; succ = node->ln_Succ; node = succ)
		ObjInfo_Delete( (OOCamObjInfo*)node );

    for (node=inst->VisWaitList.lh_Head; succ = node->ln_Succ; node = succ)
		ObjInfo_Delete( (OOCamObjInfo*)node );
}

// Appelée depuis D3M_OBJTOCAMERA
static void AddVisibleList( OObject *camobj, OOInst_3DCamera *inst, OOTagList *attrlist )
{
    OObject *obj;
    ULONG i, j;
    UBYTE flags;
    FLOAT ztgphi, ztgpsi, mr, pr, zmax;
    OOInst_3DSolid *se = D3SOL(camobj);
    OORastPort *rp = attrlist[0].ti_Ptr;
    OObject **tab = attrlist[1].ti_Ptr;
    ULONG numobj = attrlist[2].ti_Data;

    // Re-init des constantes à chaque tour
    inst->TgPsi = FDIV(inst->WindowWidth2, inst->ObserverDist);
    inst->TgPhi = FDIV(inst->WindowHeight2, inst->ObserverDist);
    inst->Trans.AreaW2 = (rp->BitMap->Width+1) / 2 - 1;
    inst->Trans.AreaH2 = (rp->BitMap->Height+1) / 2 - 1;
    inst->Trans.DispCoefX = FDIV(F2FLT(inst->Trans.AreaW2), inst->WindowWidth2);
    inst->Trans.DispCoefY = FDIV(F2FLT(inst->Trans.AreaH2), inst->WindowHeight2);

    zmax = inst->ObserverDist;

	// On vide la vielle liste dans la liste d'attente
	ObjInfo_OldToWait( inst );

	// La vieille liste vide devient courante, les objets précédents sont alors dans la vieille
	ObjInfo_SwapLists( inst );

    for (i=0; i < numobj; i++)
        {
        OOCamObjInfo *oi;
		OOInst_3DSolid *so;
        OO3DPoint CO;
		FLOAT r;

        obj = tab[i];
        so = D3SOL(obj);
		r = so->Type->R;

        //------ Changement repère : translation

        CO.X = FSUB(FADD(so->X,so->Type->Center.X), se->X);
        CO.Y = FSUB(FADD(so->Y,so->Type->Center.Y), se->Y);
        CO.Z = FSUB(FADD(so->Z,so->Type->Center.Z), se->Z);

        //------ Changement repère : rotation

		if (se->Flags & D3F_DO_SELFROTATE)
			OO3DMath_Rotate( &CO, (OO3DPoint*)&se->Rot.SAx );

        //------ Vérif approx si obj visible : sphère englobante (test avec ± R indépendement de la rotation)

        flags = 0;

        if (FCMP(FADD(CO.Z, r), zmax) < 0) continue;
        if (FCMP(FSUB(CO.Z, r), zmax) < 0) flags |= OOF_CLIPBACK;

        ztgpsi = FMUL(CO.Z, inst->TgPsi);
        pr = FADD(CO.X, r); mr = FSUB(CO.X, r);
        if (FCMP(pr, FNEG(ztgpsi)) < 0) continue;               
        if (FCMP(mr, FNEG(ztgpsi)) < 0) flags |= OOF_CLIPLEFT;  
        if (FCMP(mr, ztgpsi      ) > 0) continue;               
        if (FCMP(pr, ztgpsi      ) > 0) flags |= OOF_CLIPRIGHT; 

        ztgphi = FMUL(CO.Z, inst->TgPhi);
        pr = FADD(CO.Y, r); mr = FSUB(CO.Y, r);
        if (FCMP(pr, FNEG(ztgphi)) < 0) continue;               
        if (FCMP(mr, FNEG(ztgphi)) < 0) flags |= OOF_CLIPBOT;   
        if (FCMP(mr, ztgphi      ) > 0) continue;               
        if (FCMP(pr, ztgphi      ) > 0) flags |= OOF_CLIPTOP;   

        //------ Ici l'objet est forcément visible par au moins une caméra

        so->Flags |= D3F_SOLID_ISVISIBLE;     // (et ça ne mange pas de pain)

        //------ Obtention d'un node, ancien ou nouveau

		// On obtient un bon NodeInfo depuis la OldList ou la WaitList
        if ((oi = ObjInfo_SearchOldObj(	inst, obj )) != NULL)
			{
			if (so->Flags & D3F_SOLID_TYPECHANGED)
				if ((oi = ObjInfo_Resize( oi )) == NULL)
					continue; // Manque de mémoire, on passe au suivant...
			}
		// Sinon on en crée un nouveau
		else if ((oi = ObjInfo_Create( obj )) == NULL)
			continue; // Manque de mémoire, on passe au suivant...

        //------ Changement de repère : translation + rotation autour de la caméra
		//	Copie des sommets dans le node de la camera
        OOASSERT( so->RotVertices != NULL );
        {
		OO3DPoint *src, *dst;
        for (j=0, src=so->RotVertices, dst=oi->Vertices; j < oi->NumVertices; j++, src++, dst++)
            {
            dst->X = FSUB(src->X, se->X);
            dst->Y = FSUB(src->Y, se->Y);
            dst->Z = FSUB(src->Z, se->Z);
            OO3DMath_Rotate( dst, (OO3DPoint*)&se->Rot.SAx );
            }
        }

        //------ Ajout de l'objet dans la liste

		oi->Flags = flags;
		OOList_AddTail( inst->VisCurList, (OONode*)oi );
        }
}

static void DrawnScene( OObject *obj, OOInst_3DCamera *inst, OOTagList *attrlist )
{
	OORastPort *rp = (OORastPort*) attrlist[0].ti_Data;

	switch (rp->DrawnBy)
		{
		case OOV_RP_DRAWNBY_SYS		:
		case OOV_RP_DRAWNBY_HAND	:
			break;

		case OOV_RP_DRAWNBY_DIRECTX	:
			break;

		case OOV_RP_DRAWNBY_OPENGL	:
			{
			OONode *node;

			//glLoadIdentity();		// Reset The Current Modelview Matrix

			for (node=inst->VisCurList->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
				{
				OOCamObjInfo *oi = (OOCamObjInfo*)node;
			 	OOInst_3DSolid *so = D3SOL(oi->Obj);
				ULONG numpolys;

				if (so->Type != NULL && (numpolys = so->Type->NumPolygons))
					{
					OO3DPolygon *poly = so->Type->Polygons;
					ULONG i, j;

					for (i=0; i < numpolys; i++, poly++)
						{
						ULONG fc = poly->Material->FillColor;
						GLfloat r = (GetRValue(fc))/255.0f;
						GLfloat g = (GetGValue(fc))/255.0f;
						GLfloat b = (GetBValue(fc))/255.0f;
						glColor3f( r, g, b );

						glBegin( GL_POLYGON );
						for (j=0; j < poly->NumVertices; j++)
							{
							ULONG ind = poly->VerticesIndex[j];
							OO3DPoint *p = &so->RotVertices[ind];
							glVertex3f( (GLfloat)p->X, (GLfloat)p->Y, -(GLfloat)p->Z );
							}
						glEnd();
						}
					}
				}
			}
		}
}

/***************************************************************************
 *
 *      Attributes setting
 *
 ***************************************************************************/

// If a SetGet function is given to the attribute, the function has perhaps to change
//  the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same of the
//  current attr value.

static ULONG SetGet_All( OOSetGetParms *sgp )
{
    OOInst_3DCamera *inst = (OOInst_3DCamera*)sgp->Instance;
    ULONG change = OOV_NOCHANGE;

    if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
        {
        FLOAT val, flt = sgp->ExtAttr->ti_Float;
        switch (sgp->ExtAttr->ti_Tag)
            {
            case D3A_WindowWidth : val = FDIV(flt,(FLOAT)2); if (inst->WindowWidth2  != val) { change = OOV_CHANGED; inst->WindowWidth2 = val; } break;
            case D3A_WindowHeight: val = FDIV(flt,(FLOAT)2); if (inst->WindowHeight2 != val) { change = OOV_CHANGED; inst->WindowHeight2= val; } break;
            case D3A_ObserverDist: if (inst->ObserverDist != flt) { change = OOV_CHANGED; inst->ObserverDist = flt; } break;
            }
        sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
        }
    else{
        FLOAT *fltptr = (FLOAT*) sgp->ExtAttr->ti_Ptr;
        switch (sgp->ExtAttr->ti_Tag)
            {
            case D3A_WindowWidth : *fltptr = FMUL(inst->WindowWidth2, (FLOAT)2); break;
            case D3A_WindowHeight: *fltptr = FMUL(inst->WindowHeight2,(FLOAT)2); break;
            case D3A_ObserverDist: *fltptr = inst->ObserverDist; break;
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
    { OOA_OnOff       , TRUE, "OnOff"       , NULL      , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_WindowWidth , 0   , "WindowWidth" , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_WindowHeight, 0   , "WindowHeight", SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_ObserverDist, 0   , "ObserverDist", SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }
    };

OOClass Class_3DCamera = {
    Dispatcher_3DCamera,
    &Class_3DSolid,         // Base class
    "Class_3DCamera",       // Class Name
    0,                      // InstOffset
    sizeof(OOInst_3DCamera),// InstSize  : Taille de l'instance
    attrs,                  // AttrsDesc : Table de description des attributs
    sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)  // AttrsSize : size of instance own taglist
    };
