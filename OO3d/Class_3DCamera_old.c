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

#define INDEX_D3A_OnOff     0


//****** Imported ***********************************************

extern ULONG IntOO_ClipPolygoneX( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa, ULONG numpoints );
extern ULONG IntOO_ClipPolygoneY( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa, ULONG numpoints );
extern ULONG IntOO_ClipPolygoneZ( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa, ULONG numpoints );


//****** Exported ***********************************************


//****** Statics ************************************************

static void InitVisibleList( OOInst_3DCamera *inst );
static void AddVisibleList( OObject *obj, OOInst_3DCamera *inst, OOTagList *attrlist  );
static void RotateVisible( OObject *obj, OOInst_3DCamera *inst );

static void InitFacesList( OOInst_3DCamera *inst );
static void AddFacesList( OOInst_3DCamera *inst, OOTagList *attrlist );
//static void SortFacesList( OOInst_3DCamera *inst );
static void InsertFace( OOInst_3DCamera *inst, OOFace *fa );

static void DrawFaces( OObject *camobj, OOInst_3DCamera *inst, OOTagList *attrlist );

#define D3F_MAX_FACEPOINTS 256
static OOPoint Points2D[D3F_MAX_FACEPOINTS];
static UBYTE LineInfo[D3F_MAX_FACEPOINTS];
UBYTE Index3D[D3F_MAX_FACEPOINTS];


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
                inst->VList = NULL;
                inst->ClPtMem1 = NULL;
                inst->ClPtMem2 = NULL;
                inst->FPtrs = NULL;
                inst->FList = NULL;
                // Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
            OOInst_3DCamera *inst = OOINST_DATA(cl,obj);
            OOPool_Free( OOGlobalPool, (UBYTE*)inst->FPtrs );
            OOPool_Free( OOGlobalPool, (UBYTE*)inst->FList );
            OOPool_Free( OOGlobalPool, (UBYTE*)inst->VList );
            OOPool_Free( OOGlobalPool, (UBYTE*)inst->ClPtMem1 );
            OOPool_Free( OOGlobalPool, (UBYTE*)inst->ClPtMem2 );
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
                InitVisibleList( inst );
                AddVisibleList( obj, inst, attrlist ); // D3A_RastPort, D3A_ObjTable, D3A_ObjCount
                RotateVisible( obj, inst );
                InitFacesList( inst );
                AddFacesList( inst, attrlist );
                // SortFacesList( inst );
                }
            break;

        case D3M_DRAW:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort && attrlist[1].ti_Tag == OOA_WidthHeight );
            if (OOINST_ATTR(cl,obj,INDEX_D3A_OnOff) == TRUE)
                {
                DrawFaces( obj, OOINST_DATA(cl,obj), attrlist );
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

#define VLISTBLOCK  100

static ULONG VerifVisibleList( OOInst_3DCamera *inst )
{
  struct _ObjNode *new;

    if (! (new = (APTR) OOPool_Resize( OOGlobalPool, (UBYTE*)inst->VList, (inst->NumVObjSpace + VLISTBLOCK) * sizeof(struct _ObjNode) ))) return(0);
    inst->VList = new;
    inst->NumVObjSpace += VLISTBLOCK;
    return(1);
}

static void InitVisibleList( OOInst_3DCamera *inst )
{
    inst->NumVObj = 0;
}

static void AddVisibleList( OObject *camobj, OOInst_3DCamera *inst, OOTagList *attrlist )
{
    OObject *obj;
    ULONG i;
    UWORD flags;
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

    for (i=0; i < numobj; i++)
        {
        OOInst_3DSolid *so;
        OO3DPoint CO;
        struct _ObjNode *on;

        obj = tab[i];
        so = D3SOL(obj);

        //------ Changement repère : translation

        CO.X = FSUB(so->X, se->X);
        CO.Y = FSUB(so->Y, se->Y);
        CO.Z = FSUB(so->Z, se->Z);

        //------ Changement repère : rotation

        OO3DMath_Rotate( &CO, (OO3DPoint*)&se->Rot.SAx );

        //------ Vérif approx si obj visible : sphère englobante (test avec ± R indépendement de la rotation)

        flags = 0;

        if (FCMP(FADD(CO.Z, so->R), zmax) < 0) continue;
        if (FCMP(FSUB(CO.Z, so->R), zmax) < 0) flags |= OOF_CLIPBACK;

        ztgpsi = FMUL(CO.Z, inst->TgPsi);
        pr = FADD(CO.X, so->R); mr = FSUB(CO.X, so->R);
        if (FCMP(pr, FNEG(ztgpsi)) < 0) continue;               // if (CO.X + so->R < -ztgpsi) continue;
        if (FCMP(mr, FNEG(ztgpsi)) < 0) flags |= OOF_CLIPLEFT;  // if (CO.X - so->R < -ztgpsi) flags |= OOF_CLIPLEFT;
        if (FCMP(mr, ztgpsi      ) > 0) continue;               // if (CO.X - so->R >  ztgpsi) continue;
        if (FCMP(pr, ztgpsi      ) > 0) flags |= OOF_CLIPRIGHT; // if (CO.X + so->R >  ztgpsi) flags |= OOF_CLIPRIGHT;

        ztgphi = FMUL(CO.Z, inst->TgPhi);
        pr = FADD(CO.Y, so->R); mr = FSUB(CO.Y, so->R);
        if (FCMP(pr, FNEG(ztgphi)) < 0) continue;               // if (CO.Y + so->R < -ztgphi) continue;
        if (FCMP(mr, FNEG(ztgphi)) < 0) flags |= OOF_CLIPBOT;   // if (CO.Y - so->R < -ztgphi) flags |= OOF_CLIPBOT;
        if (FCMP(mr, ztgphi      ) > 0) continue;               // if (CO.Y - so->R >  ztgphi) continue;
        if (FCMP(pr, ztgphi      ) > 0) flags |= OOF_CLIPTOP;   // if (CO.Y + so->R >  ztgphi) flags |= OOF_CLIPTOP;

        //------ Ici l'objet est forcément visible par au moins une caméra

        so->Flags |= D3F_SOLID_ISVISIBLE;     // (et ça ne mange pas de pain)

        //------ Ajout de l'objet dans la liste

        if (inst->NumVObj >= inst->NumVObjSpace) if (! VerifVisibleList( inst )) return;
        on = &inst->VList[inst->NumVObj++];
        on->Obj   = obj;
        on->Flags = flags;
        on->CO    = CO;
        }
}

static void RotateVisible( OObject *obj, OOInst_3DCamera *inst )
{
    ULONG i, j;
    OO3DPoint *p;
    OOInst_3DSolid *se = D3SOL(obj);

    for (i=0; i < inst->NumVObj; i++)
        {
        struct _ObjNode *on = &inst->VList[i];
        OOInst_3DSolid *so = D3SOL(on->Obj);

        if (so->RotCoords)
            {
            //------ Recopie dans coordonnées locales, rotation sur lui-même, vraies coords
            for (j=0, p=so->RotCoords; j < so->NumCoords; j++, p++)
                {
                *p = so->DefCoords[j];
                OO3DMath_Rotate( p, (OO3DPoint*)&so->Rot.SAx );
                p->X = FADD(p->X, so->X);
                p->Y = FADD(p->Y, so->Y);
                p->Z = FADD(p->Z, so->Z);
                }

            //------ Changement de repère : translation + rotation autour de la caméra
            for (j=0, p=so->RotCoords; j < so->NumCoords; j++, p++)
                {
                p->X = FSUB(p->X, se->X);
                p->Y = FSUB(p->Y, se->Y);
                p->Z = FSUB(p->Z, se->Z);
                OO3DMath_Rotate( p, (OO3DPoint*)&se->Rot.SAx );
                }
            }
        }
}

//--------------------------------------------------------------

#define FLISTBLOCK  100

static ULONG VerifFPtrsList( OOInst_3DCamera *inst )
{
  APTR *new;

    if (! (new = (APTR*) OOPool_Resize( OOGlobalPool, (UBYTE*)inst->FPtrs, (inst->FPtrsSpace + FLISTBLOCK) * sizeof(APTR) ))) return(0);
    inst->FPtrs = new;
    inst->FPtrsSpace += FLISTBLOCK;
    return(1);
}

static ULONG VerifFacesList( OOInst_3DCamera *inst )
{
    struct _ObjNode *new;
    ULONG i;
    SLONG *p, diff;
    OOFace *fa;

    if (! (new = (APTR) OOPool_Resize( OOGlobalPool, (UBYTE*)inst->FList, inst->FacesSpace + (FLISTBLOCK*10) ))) return(0);
    diff = (ULONG)new - (ULONG)inst->FList;
    inst->FList = (APTR)new;
    inst->FacesSpace += (FLISTBLOCK*10);

    // Remet à jour les pointeurs sur les faces
    for (i=0, p=(LONG*)inst->FPtrs; i < inst->NumFaces; i++) *p++ += diff;

    // Remet à jour les pointeurs sur les plans et les points
    for (i=0; i < inst->NumFaces; i++)
        {
        fa = inst->FPtrs[i];
        fa->_P.Plan = &fa->_Plan;
        }

    return(1);
}

static void InitFacesList( OOInst_3DCamera *inst )
{
    inst->NumFaces = 0;
    inst->UsedSpace = 0;
}

static ULONG VerifClipPoints( OOInst_3DCamera *inst, ULONG numpoints )
{
    ULONG actsize = inst->ClPtMem1 == NULL ? 0 : OOPool_GetLength( inst->ClPtMem1 );
    ULONG newsize = sizeof(OO3DPoint) * numpoints;

    if (newsize > actsize)
        {
        if (! (inst->ClPtMem1 = (APTR) OOPool_Resize( OOGlobalPool, (UBYTE*)inst->ClPtMem1, newsize ))) return(0);
        if (! (inst->ClPtMem2 = (APTR) OOPool_Resize( OOGlobalPool, (UBYTE*)inst->ClPtMem2, newsize ))) return(0);
        inst->NumClPt = numpoints;
        }
    return(1);
}

static void set_minmax( OOFace *fa, OO3DPoint *pt, ULONG numpoints )
{
    ULONG j;

    fa->_Plan.Min.X = fa->_Plan.Min.Y = fa->_Plan.Min.Z = MAXFLT;
    fa->_Plan.Max.X = fa->_Plan.Max.Y = fa->_Plan.Max.Z = MINFLT;

    for (j=0; j < numpoints; j++, pt++)
        {
        if (FCMP(fa->_Plan.Min.X, pt->X) > 0) fa->_Plan.Min.X = pt->X;
        if (FCMP(fa->_Plan.Max.X, pt->X) < 0) fa->_Plan.Max.X = pt->X;
        if (FCMP(fa->_Plan.Min.Y, pt->Y) > 0) fa->_Plan.Min.Y = pt->Y;
        if (FCMP(fa->_Plan.Max.Y, pt->Y) < 0) fa->_Plan.Max.Y = pt->Y;
        if (FCMP(fa->_Plan.Min.Z, pt->Z) > 0) fa->_Plan.Min.Z = pt->Z;
        if (FCMP(fa->_Plan.Max.Z, pt->Z) < 0) fa->_Plan.Max.Z = pt->Z;
        }
}

/*
static void prep_texsupp( OOFace *fa )
{
    OO3DPoint *p0 = &fa->_Plan.RP0;
    OO3DPoint *p1 = &fa->_Plan.RP1;
    OO3DPoint *p2 = &fa->_Plan.RP2;

    // Points de définition d'un rectangle en coordonées réelles
    *p0 = fa->_Plan.Points[0];
    *p1 = fa->_Plan.Points[1];
    *p2 = fa->_Plan.Points[fa->_Plan.NumPoints-1];
    p1->X -= p0->X; p1->Y -= p0->Y;
    p2->X -= p0->X; p2->Y -= p0->Y;

    // Pente du polygone sur x et y
    fa->_Plan.DZX = FDIV( FSUB( FMUL(p1->Z,p2->Y), FMUL(p2->Z,p1->Y) ), FSUB( FMUL(p1->X,p2->Y), FMUL(p2->X,p1->Y) ) );
    fa->_Plan.DZY = FDIV( FSUB( FMUL(p1->Z,p2->X), FMUL(p2->Z,p1->X) ), FSUB( FMUL(p1->Y,p2->X), FMUL(p2->Y,p1->X) ) );
}
*/

#define ELLIPSENUMPAS  6

static void AddFacesList( OOInst_3DCamera *inst, OOTagList *attrlist )
{
    ULONG i, j;
    UWORD *p;
    OO3DPoint *pt;
    FLOAT zmax = inst->ObserverDist;
    FLOAT anglestep = FDIV(PI,(FLOAT)(2*ELLIPSENUMPAS));
    OORastPort *rp = attrlist[0].ti_Ptr;

    // OOTRACE( "\n\n----------------------\n" );
    for (i=0; i < inst->NumVObj; i++)
        {
        struct _ObjNode *on = &inst->VList[i];
        OOInst_3DSolid *so = D3SOL(on->Obj);

        if (so->DefFaces)
            {
            for (p=(UWORD*)so->DefFaces; *p != ENDFACES; ) // *p=f->NumPoints, si ENDFACES : fin des faces
                {
                OOFaceDef *f = (OOFaceDef*)p;
                ULONG facesize;
                LONG orient;
                OOFace *fa;
                OO3DPoint *src, *dst;
                ULONG num, numpoints = (f->Flags & OOF_ELLIPSE) ? ELLIPSENUMPAS*4 : f->NumPoints;

                if (numpoints)
                    {
                    //------ Préparation des buffers

                    if (inst->NumClPt < (((ULONG)numpoints+1) * 2)) if (! VerifClipPoints( inst, (numpoints+1) * 2 ))
                        { p = (UWORD*)&f->CoordIndex[numpoints]; continue; } // si pas de mémoire, face suivante

                    //------ Init de la face

                    facesize = sizeof(OOFace) + (sizeof(OO3DPoint) * numpoints * 2); // +1 OO3DPoint dans OOFace
                    while (inst->UsedSpace + facesize >= inst->FacesSpace) if (! VerifFacesList( inst )) return;
                    fa = (OOFace*)((ULONG)inst->FList + inst->UsedSpace);

                    fa->Obj       = on->Obj;
                    fa->_P.Flags  = f->Flags;
                    fa->_P.Flags |= on->Flags;
                    fa->_P.Plan   = &fa->_Plan;
                    fa->_P.LineColor = f->LineColor;
                    fa->_P.FillColor = f->FillColor;
                    fa->_P.TextureInfo = (so->DefTextures != NULL) ? &so->DefTextures[f->TextureIndex] : NULL;
                    fa->_Plan.NumPoints = (UWORD)numpoints;
                    fa->_Plan.Trans = &inst->Trans;
                    if (fa->_P.TextureInfo != NULL)
                        {
                        if ((fa->_P.TextureInfo->BitMap = so->TextureBitMap) == NULL)
                            fa->_P.TextureInfo = NULL;
                        }
                    if (fa->_P.TextureInfo == NULL) fa->_P.Flags &= ~OOF_MAP;

                    // après verif car allocations
                    src = fa->_Plan.Points; // Peut-être pas besoin de recopie finale
                    dst = inst->ClPtMem2;

                    //------ Recopie des points

                    if (fa->_P.Flags & OOF_ELLIPSE)
                        {
                        FLOAT X, Y, angle;
                        for (j=0, angle=(FLOAT)0; j < ELLIPSENUMPAS; j++, angle=FADD(angle,anglestep))
                            {
                            X = FMUL(so->R,FCOS(angle));
                            Y = FMUL(so->R,FSIN(angle));

                            src[j+0*ELLIPSENUMPAS].X = FADD(on->CO.X,X);
                            src[j+0*ELLIPSENUMPAS].Y = FADD(on->CO.Y,Y);
                            src[j+0*ELLIPSENUMPAS].Z = on->CO.Z;

                            src[j+1*ELLIPSENUMPAS].X = FADD(on->CO.X,FNEG(Y));
                            src[j+1*ELLIPSENUMPAS].Y = FADD(on->CO.Y,X);
                            src[j+1*ELLIPSENUMPAS].Z = on->CO.Z;

                            src[j+2*ELLIPSENUMPAS].X = FADD(on->CO.X,FNEG(X));
                            src[j+2*ELLIPSENUMPAS].Y = FADD(on->CO.Y,FNEG(Y));
                            src[j+2*ELLIPSENUMPAS].Z = on->CO.Z;

                            src[j+3*ELLIPSENUMPAS].X = FADD(on->CO.X,Y);
                            src[j+3*ELLIPSENUMPAS].Y = FADD(on->CO.Y,FNEG(X));
                            src[j+3*ELLIPSENUMPAS].Z = on->CO.Z;
                            }
                        src[fa->_Plan.NumPoints] = src[0]; // Pour faciliter l'examen des segments
                        p = (UWORD*)&f->CoordIndex[1]; // Pointe sur ENDFACES
                        }
                    else{
                        p = (UWORD*)&f->CoordIndex[0];
                        for (j=0; j < fa->_Plan.NumPoints; j++, p++)
                            src[j] = so->RotCoords[*p];
                        src[fa->_Plan.NumPoints] = src[0]; // Pour faciliter l'examen des segments
                        }

                    if (fa->_P.Flags & OOF_MAP)
                        {
                        OOPlan *plan = &fa->_Plan;
                        plan->P1 = src[0];
                        plan->P2 = src[numpoints-1]; // Horizontal
                        plan->P3.X = - src[1].X;     // Vertical, positif vers le bas
                        plan->P3.Y = - src[1].Y;
                        plan->P3.Z = - src[1].Z;

                        plan->P2.X -= plan->P1.X;
                        plan->P2.Y -= plan->P1.Y;
                        plan->P2.Z -= plan->P1.Z;
                        plan->P3.X += plan->P1.X;
                        plan->P3.Y += plan->P1.Y;
                        plan->P3.Z += plan->P1.Z;

                        plan->P1.X *= inst->Trans.DispCoefX;
                        plan->P1.Y *= inst->Trans.DispCoefY;
                        plan->P2.X *= inst->Trans.DispCoefX;
                        plan->P2.Y *= inst->Trans.DispCoefY;
                        plan->P3.X *= inst->Trans.DispCoefX;
                        plan->P3.Y *= inst->Trans.DispCoefY;
                        }

                    // !!! A partir de là ne plus toucher à fa->_Plan.NumPoints !!!

                    //------ Clipping arrière de la face (obligatoirement avant la projection perspective cubique)

                    if (on->Flags & OOF_CLIPBACK)
                        {
                        if (! (numpoints = IntOO_ClipPolygoneZ( zmax, src, dst, fa, numpoints ))) continue;
                        if (dst == inst->ClPtMem2) { src = inst->ClPtMem2; dst = inst->ClPtMem1; } else { src = inst->ClPtMem1; dst = inst->ClPtMem2; }
                        }
                    else // Si pas de clip on recopie les points pour garder les originaux avant projection temporairement
                        {
                        src = inst->ClPtMem1;
                        for (j=0, pt=fa->_Plan.Points; j < numpoints; j++, pt++) src[j] = *pt;
                        }

                    //------ Projection perspective cubique
                    {
                    OOPoint old, p2d0, p2d;

                    pt = src;
                    pt->X = FDIV(pt->X, pt->Z); // pt->Z toujours != 0 grâce au clipping arrière
                    pt->Y = FDIV(pt->Y, pt->Z);
                    orient = 0;
                    if (numpoints > 1)
                        {
                        p2d0.x = old.x = F2LONG(FMUL(pt->X, inst->Trans.DispCoefX));
                        p2d0.y = old.y = F2LONG(FMUL(pt->Y, inst->Trans.DispCoefY));
                        num = 1;
                        for (j=1, pt++; j < numpoints; j++, pt++)
                            {
                            pt->X = FDIV(pt->X, pt->Z);
                            pt->Y = FDIV(pt->Y, pt->Z);
                            p2d.x = F2LONG(FMUL(pt->X, inst->Trans.DispCoefX));
                            p2d.y = F2LONG(FMUL(pt->Y, inst->Trans.DispCoefY));
                            if (p2d.x != old.x || p2d.y != old.y)
                                {
                                orient += (old.x - p2d.x) * (old.y + p2d.y);
                                old.x = p2d.x; old.y = p2d.y;
                                num++;
                                }
                            }

                        if (num > 1)
                            {
                            if (p2d0.x == old.x && p2d0.y == old.y)
                                {
                                num--;      // Après_le_dernier est égal au premier, pour faciliter l'examen des segments
                                }
                            else{
                                src[num] = src[0];  // Après_le_dernier est égal au premier, pour faciliter l'examen des segments
                                orient += (p2d.x - p2d0.x) * (p2d.y + p2d0.y);
                                }
                            }
                        else{
                            fa->_P.Flags |= OOF_TEST2DPOINT;
                            fa->Lonely2DPoint = p2d;
                            }
                        numpoints = num;
                        }
                    // Verif si on enlève cette face
//+++               if (numpoints == 1 && previous_1point.x,y sont égaux, continue)
//+++               if (numpoints == 2 && previous_2points.x,y sont égaux, continue)
                    }

                    //------ Calcul des MiniMax et recopie des points

                    if (numpoints == 1) // C'est un point
                        {
                        //OOTRACE( "." );
                        fa->_Plan.Min = fa->_Plan.Max = src[0];
                        fa->_Plan.N.X = fa->_Plan.N.Y = fa->_Plan.N.Z = (FLOAT)0;
                        fa->_Plan.D = (FLOAT)0;
                        }
                    else if (numpoints == 2) // C'est un segment
                        {
                        //OOTRACE( "." );
                        fa->_Plan.N.X = fa->_Plan.N.Y = fa->_Plan.N.Z = (FLOAT)0;
                        fa->_Plan.D = (FLOAT)0;
                        fa->_Plan.Min.X = FMIN(src[0].X,src[1].X);
                        fa->_Plan.Min.Y = FMIN(src[0].Y,src[1].Y);
                        fa->_Plan.Min.Z = FMIN(src[0].Z,src[1].Z);
                        fa->_Plan.Max.X = FMAX(src[0].X,src[1].X);
                        fa->_Plan.Max.Y = FMAX(src[0].Y,src[1].Y);
                        fa->_Plan.Max.Z = FMAX(src[0].Z,src[1].Z);
                        }
                    else // if (numpoints > 2) c'est un polygone
                        {
                        BOOL make_line = FALSE;
                        FLOAT proscal;

                        //------ Calcul des MiniMax et recopie des points

                        set_minmax( fa, src, numpoints );

                        //------ Calcul du vecteur normal à la face et décision si face visible

                        if (orient == 0)
                            {
                            make_line = TRUE;
                            //OOTRACE( "*" );
                            }
                        else{
                            OO3DMath_Normale( &src[0], &src[1], &src[numpoints-1], &fa->_Plan.N, FALSE );
                            /*
                            if (oldnumpoints != numpoints)
                                {
                                if (FZERO(OO3DMath_Modulo( &fa->_Plan.N )))
                                    {
                                    proscal = (FLOAT)0;
                                    make_line = TRUE;
                                    }
                                }
                            */
                            if (make_line == FALSE)
                                {
                                // vecteur de visée == coords du sommet car translation déjà faite
                                proscal = OO3DMath_DotProduct( &fa->_Plan.N, &src[0] );
                                // Test de fa->_Plan.N.Z car projection perspective cubique.
                                // Sinon : if (proscal <= 0) face invisible ou sur la tranche
                                if (FTST(fa->_Plan.N.Z) <= 0)
                                    {
                                    if (FZERO(fa->_Plan.N.Z)) make_line = TRUE;
                                    else{ // < 0
                                        if (fa->_P.Flags & OOF_HIDE) continue;
                                        else fa->_P.Flags |= OOF_BACKORIENTED;
                                        }
                                    }
                                if (orient > 0 && (fa->_P.Flags & OOF_BACKORIENTED)) { /*OOTRACE( "+" );*/ continue; }
                                else if (orient < 0 && !(fa->_P.Flags & OOF_BACKORIENTED)) { /*OOTRACE( "+" );*/ continue; }
                                }
                            }

                        // Range 2 points dans buffer (on peut perdre de la profondeur)
                        if (make_line == TRUE)
                            {
                            OO3DPoint P1, P2;
                            for (j=0; j < numpoints; j++)
                                {
                                if (FCMP(src[j].X,fa->_Plan.Min.X) == 0) P1 = src[j];
                                if (FCMP(src[j].X,fa->_Plan.Max.X) == 0) P2 = src[j];
                                }
                            numpoints = 2;
                            proscal = 0;
                            src[0] = P1;
                            src[1] = P2;
                            src[2] = P1;
                            }
                        // Verif si on enlève cette face
    //+++               if (numpoints == 1 && previous_1point.x,y sont égaux, continue)
    //+++               if (numpoints == 2 && previous_2points.x,y sont égaux, continue)

                        fa->_Plan.D = FNEG(proscal); // 0 si pas une face
                        }

                    //------ Clipping de la face (suite)

                    if (on->Flags & OOM_CLIPMASK)
                        {
                        ULONG old_num_points = numpoints;

                        //--- Clipping de la face

                        if (on->Flags & OOF_CLIPLEFT)
                            {
                            if (! (numpoints = IntOO_ClipPolygoneX( FNEG(inst->WindowWidth2), src, dst, fa, numpoints ))) continue;
                            if (dst == inst->ClPtMem2) { src = inst->ClPtMem2; dst = inst->ClPtMem1; } else { src = inst->ClPtMem1; dst = inst->ClPtMem2; }
                            }
                        if (on->Flags & OOF_CLIPRIGHT)
                            {
                            if (! (numpoints = IntOO_ClipPolygoneX( inst->WindowWidth2, src, dst, fa, numpoints ))) continue;
                            if (dst == inst->ClPtMem2) { src = inst->ClPtMem2; dst = inst->ClPtMem1; } else { src = inst->ClPtMem1; dst = inst->ClPtMem2; }
                            }
                        if (on->Flags & OOF_CLIPTOP)
                            {
                            if (! (numpoints = IntOO_ClipPolygoneY( inst->WindowHeight2, src, dst, fa, numpoints ))) continue;
                            if (dst == inst->ClPtMem2) { src = inst->ClPtMem2; dst = inst->ClPtMem1; } else { src = inst->ClPtMem1; dst = inst->ClPtMem2; }
                            }
                        if (on->Flags & OOF_CLIPBOT)
                            {
                            if (! (numpoints = IntOO_ClipPolygoneY( FNEG(inst->WindowHeight2), src, dst, fa, numpoints ))) continue;
                            if (dst == inst->ClPtMem2) { src = inst->ClPtMem2; dst = inst->ClPtMem1; } else { src = inst->ClPtMem1; dst = inst->ClPtMem2; }
                            }

                        //--- Optimize à nouveau si le clipping a enlevé des points

                        if (old_num_points != numpoints)
                            {
                            if (numpoints == 1) // C'est un point
                                {
                                //OOTRACE( "." );
                                fa->_Plan.Min = fa->_Plan.Max = src[0];
                                fa->_Plan.N.X = fa->_Plan.N.Y = fa->_Plan.N.Z = (FLOAT)0;
                                fa->_Plan.D = (FLOAT)0;
                                }
                            else if (numpoints == 2) // C'est un segment
                                {
                                //OOTRACE( "+" );
                                fa->_Plan.N.X = fa->_Plan.N.Y = fa->_Plan.N.Z = (FLOAT)0;
                                fa->_Plan.D = (FLOAT)0;
                                }
                            }
                        }

                    //------ Ajout de la face et recopie des points

                    facesize = sizeof(OOFace) + (sizeof(OO3DPoint) * numpoints);
                    inst->UsedSpace += facesize;

                    if (src != fa->_Plan.Points)
                        {
                        for (j=0, pt=fa->_Plan.Points; j < numpoints; j++, pt++)
                            *pt = src[j];
                        }
                    fa->_Plan.NumPoints = (UWORD)numpoints;

                    //------ Ajout du pointeur pour le tri

                    if (inst->NumFaces >= inst->FPtrsSpace) if (! VerifFPtrsList( inst )) return;
                    //inst->FPtrs[inst->NumFaces++] = (APTR)fa;
                    InsertFace( inst, fa );
                    }
                else p = (UWORD*)(sizeof(OOFaceDef)+(ULONG)p);
                }
            }
        }
}

//------------------------

/*
    (X - p1->X) * dyp - (Y - p1->Y) * dxp = 0
    Y = (X * (dyp/dxp)) - (p->x1 * (dyp/dxp)) + p->y1

    Y = y1 + dy * (MinX - x1) / dx
    X = x1 + dx * (valY - y1) / dy
*/

static ULONG intersecte_seg( OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *s1, OO3DPoint *s2, OO3DPoint *buf )
{
  FLOAT dyp, dxp, dys, dxs;
  OO3DPoint N;

    dyp = FSUB(p2->Y, p1->Y);
    dxp = FSUB(p2->X, p1->X);
    dys = FSUB(s2->Y, s1->Y);
    dxs = FSUB(s2->X, s1->X);

    if (FZERO(dyp))       //---- Horizontale
        {
        if (FZERO(dys))           //---- Horizontale
            {
            return(0);          // --> Parallèles
            }
        else if (FZERO(dxs))      //---- Verticale
            {
            N.X = s1->X;
            N.Y = p1->Y;
            }
        else{                   //---- Oblique
            N.Y = p1->Y;
            N.X = FADD(s1->X, FDIV(FMUL(dxs, FSUB(N.Y, s1->Y)), dys));
            }
        }
    else if (FZERO(dxp))  //---- Verticale
        {
        if (FZERO(dxs))           //---- Verticale
            {
            return(0);          // --> Parallèles
            }
        else if (FZERO(dys))      //---- Horizontale
            {
            N.X = p1->X;
            N.Y = s1->Y;
            }
        else{                   //---- Oblique
            N.X = p1->X;
            N.Y = FADD(s1->Y, FDIV(FMUL(dys, FSUB(N.X, s1->X)), dxs));
            }
        }
    else                //---- Oblique
        {
        if (FZERO(dys))           //---- Horizontale
            {
            N.Y = s1->Y;
            N.X = FADD(p1->X, FDIV(FMUL(dxp, FSUB(N.Y, p1->Y)), dyp));
            }
        else if (FZERO(dxs))      //---- Verticale
            {
            N.X = s1->X;
            N.Y = FADD(p1->Y, FDIV(FMUL(dyp, FSUB(N.X, p1->X)), dxp));
            }
        else{                   //---- Oblique
            // if (! OO3DMath_InterSeg( p1->X, dxp, dyp, p1->Y, s1->X, dxs, dys, s1->Y, &N )) return(0); // parallèles
            FLOAT a, b, A, B, d;

            a = FDIV(dyp,dxp);
            b = FSUB(p1->Y,FMUL(a,p1->X));
            A = FDIV(dys,dxs);
            B = FSUB(s1->Y,FMUL(A,s1->X));

            d = FSUB(a,A);
            if (FZERO(d)) return(0);

            N.X = FDIV(FSUB(B,b),d);
            N.Y = FMUL(a,FADD(N.X,b));
            }
        }

    if (FCMP(p1->X,p2->X) < 0) { if (FCMP(N.X, p1->X) < 0 || FCMP(N.X, p2->X) > 0) return(0); }
    else { if (FCMP(N.X, p2->X) < 0 || FCMP(N.X, p1->X) > 0) return(0); }
    if (FCMP(s1->X, s2->X) < 0) { if (FCMP(N.X, s1->X) < 0 || FCMP(N.X, s2->X) > 0) return(0); }
    else { if (FCMP(N.X, s2->X) < 0 || FCMP(N.X, s1->X) > 0) return(0); }

    if (FCMP(p1->Y, p2->Y) < 0) { if (FCMP(N.Y, p1->Y) < 0 || FCMP(N.Y, p2->Y) > 0) return(0); }
    else { if (FCMP(N.Y, p2->Y) < 0 || FCMP(N.Y, p1->Y) > 0) return(0); }
    if (FCMP(s1->Y, s2->Y) < 0) { if (FCMP(N.Y, s1->Y) < 0 || FCMP(N.Y, s2->Y) > 0) return(0); }
    else { if (FCMP(N.Y, s2->Y) < 0 || FCMP(N.Y, s1->Y) > 0) return(0); }

    *buf = N;
    return(1);
}

static ULONG intersecte( OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *Int, ULONG ni, OOFace *f2 )
{
  register LONG /*i,*/ k, num=0, actual;
  register OO3DPoint *s1, *s2;

    for (k=1, s1=f2->_Plan.Points, s2=s1+1; k < f2->_Plan.NumPoints; k++, s1++, s2++)
        {
        actual = ni + num;
        if (intersecte_seg( p1, p2, s1, s2, &Int[actual] ))
            {
            // for (i=0; i < actual; i++)
            //  { if (FCMP(Int[actual].x, Int[i].x) == 0 && FCMP(Int[actual].y, Int[i].y) == 0) { num--; break; } }
            num++;
            }
        }
    if (f2->_Plan.NumPoints > 2)
        {
        s2 = f2->_Plan.Points;
        actual = ni + num;
        if (intersecte_seg( p1, p2, s1, s2, &Int[actual] ))
            {
            // for (i=0; i < actual; i++)
            //  { if (FCMP(Int[actual].x, Int[i].x) == 0 && FCMP(Int[actual].y, Int[i].y) == 0) { num--; break; } }
            num++;
            }
        }
    return( (ULONG)num );
}

static LONG sortfaces_cmp( OOFace *f1, OOFace *f2 )
{
  LONG inverse, invfaces;

    if (! (FCMP(f1->_Plan.Min.Z, f2->_Plan.Max.Z) <= 0 && FCMP(f2->_Plan.Min.Z, f1->_Plan.Max.Z) <= 0)) // Intersecte en Z ?
        {
        return( (FCMP(f1->_Plan.Min.Z, f2->_Plan.Min.Z) < 0) ? 1 : -1 );
        }

    inverse = -1;

    if (FCMP(f1->_Plan.Min.X, f2->_Plan.Max.X) < 0 && FCMP(f2->_Plan.Min.X, f1->_Plan.Max.X) < 0 &&
        FCMP(f1->_Plan.Min.Y, f2->_Plan.Max.Y) < 0 && FCMP(f2->_Plan.Min.Y, f1->_Plan.Max.Y) < 0) // Intersecte en X et Y ?
        {
        FLOAT z1, z2, x, y, z, xmin, xmax, ymin, ymax;
        ULONG affine=0, div1=0, div2=0;

        invfaces = 0;
        if ((ULONG)f1 > (ULONG)f2) { OOFace *tmp = f1; f1 = f2; f2 = tmp; invfaces = 1; }

        xmin = FMAX( f1->_Plan.Min.X, f2->_Plan.Min.X );
        xmax = FMIN( f1->_Plan.Max.X, f2->_Plan.Max.X );
        ymin = FMAX( f1->_Plan.Min.Y, f2->_Plan.Min.Y );
        ymax = FMIN( f1->_Plan.Max.Y, f2->_Plan.Max.Y );

        x = FDIV( FADD(xmin,xmax), (FLOAT)2 );
        y = FDIV( FADD(ymin,ymax), (FLOAT)2 );

        //--- Si PlanN.Z == 0 on ne peut pas calculer les Z avec l'équation d'un plan
        //    C'est le cas d'une droite ou d'un point

        if (FTST(f1->_Plan.N.Z))
            {
            z1 = OO3DMath_PlanZ( &f1->_Plan, x, y );
            if (FCMP(z1, f1->_Plan.Min.Z) < 0 || FCMP(z1, f1->_Plan.Max.Z) > 0) affine = 1;
            }
        else { div1 = 1; z1 = FDIV( FADD( f1->_Plan.Min.Z, f1->_Plan.Max.Z ), (FLOAT)2 ); }

        if (FTST(f2->_Plan.N.Z))
            {
            z2 = OO3DMath_PlanZ( &f2->_Plan, x, y );
            if (FCMP(z2, f2->_Plan.Min.Z) < 0 || FCMP(z2, f2->_Plan.Max.Z) > 0) affine = 1;
            }
        else { div2 = 1; z2 = FDIV( FADD( f2->_Plan.Min.Z, f2->_Plan.Max.Z ), (FLOAT)2 ); }

        if (affine)
            {
            LONG ni=0, k;
            OO3DPoint Int[50];
            OO3DPoint *p;

            for (k=1, p=f1->_Plan.Points; k < f1->_Plan.NumPoints; k++, p++)
                ni += intersecte( p, p+1, Int, ni, f2 );
            if (f1->_Plan.NumPoints > 2)
                ni += intersecte( p, f1->_Plan.Points, Int, ni, f2 );

            if (! ni) // Si pas d'intersection, l'un est englobé ? Si oui -> ses points sont l'intersection
                {
                OOFace *f=0;
                if (FCMP(f1->_Plan.Max.X, f2->_Plan.Max.X) <= 0 &&
                    FCMP(f1->_Plan.Min.X, f2->_Plan.Min.X) >= 0 &&
                    FCMP(f1->_Plan.Max.Y, f2->_Plan.Max.Y) <= 0 &&
                    FCMP(f1->_Plan.Min.Y, f2->_Plan.Min.Y) >= 0) f = f1;
                else
                    if (FCMP(f1->_Plan.Max.X, f2->_Plan.Max.X) >= 0 &&
                        FCMP(f1->_Plan.Min.X, f2->_Plan.Min.X) <= 0 &&
                        FCMP(f1->_Plan.Max.Y, f2->_Plan.Max.Y) >= 0 &&
                        FCMP(f1->_Plan.Min.Y, f2->_Plan.Min.Y) <= 0) f = f2;
                if (f)
                    {
                    for (k=0, p=f->_Plan.Points; k < f->_Plan.NumPoints; k++, p++)
                        {
                        Int[ni].X = p->X;
                        Int[ni].Y = p->Y;
                        ni++;
                        }
                    }
                }

            if (ni < 2)
                {
                z1 = f1->_Plan.Min.Z;
                z2 = f2->_Plan.Min.Z;
                // z1 = FDIV( FADD( f1->_Plan.Min.Z, f1->_Plan.Max.Z ), (FLOAT)2 );
                // z2 = FDIV( FADD( f2->_Plan.Min.Z, f2->_Plan.Max.Z ), (FLOAT)2 );
                }
            else{
                FLOAT fni = F2FLT(ni);
                for (k=0, x=y=z=(FLOAT)0; k < ni; k++)
                    { x = FADD(x,Int[k].X); y = FADD(y,Int[k].Y); }
                x = FDIV(x,fni); y = FDIV(y,fni);

                if (! div1)
                    {
                    z1 = OO3DMath_PlanZ( &f1->_Plan, x, y );
                    if (FCMP(z1, f1->_Plan.Min.Z) < 0 || FCMP(z1, f1->_Plan.Max.Z) > 0) div1 = 0; else div1 = 1;
                    }
                if (! div1) z1 = FDIV( FADD( f1->_Plan.Min.Z, f1->_Plan.Max.Z ), (FLOAT)2 );
                if (! div2)
                    {
                    z2 = OO3DMath_PlanZ( &f2->_Plan, x, y );
                    if (FCMP(z2, f2->_Plan.Min.Z) < 0 || FCMP(z2, f2->_Plan.Max.Z) > 0) div2 = 0; else div2 = 1;
                    }
                if (! div2) z2 = FDIV( FADD( f2->_Plan.Min.Z, f2->_Plan.Max.Z ), (FLOAT)2 );
                }
            }

        if (invfaces) { if (FCMP(z2,z1) < 0) inverse = 1; }
        else { if (FCMP(z1,z2) < 0) inverse = 1; }
        }
    /*
    else{
        FLOAT z1 = FDIV( FADD( f1->_Plan.Min.Z, f1->_Plan.Max.Z ), (FLOAT)2 );
        FLOAT z2 = FDIV( FADD( f2->_Plan.Min.Z, f2->_Plan.Max.Z ), (FLOAT)2 );
        if (invfaces) { if (FCMP(z2, z1) < 0) inverse = 1; }
        else { if (FCMP(z1, z2) < 0) inverse = 1; }
        }
    */

    return( inverse );
}

static void InsertFace( OOInst_3DCamera *inst, OOFace *f1 )
{
    SLONG i, num = inst->NumFaces;

    /*if (f1->_Plan.NumPoints == 1)
        {
        OOFace *f2;
        for (i=num-1; i >= 0; i--)   // sur 10 : de 9 à -1  >>>  donc insérer après, de 0 à 10
            {
            f2 = inst->FPtrs[i];
            if  (f2->_Plan.NumPoints == 1)
                {
                if ((f1->_P.Flags & OOF_TEST2DPOINT) && (f2->_P.Flags & OOF_TEST2DPOINT))
                    {
                    if (FCMP(f1->_Plan.Points[0].z, f2->_Plan.Points[0].z) < 0)
                        {
                        if (f1->Lonely2DPoint.x == f2->Lonely2DPoint.x && f1->Lonely2DPoint.y == f2->Lonely2DPoint.y)
                            {
                            inst->FPtrs[i] = f1;
                            return;
                            }
                        break;
                        }
                    }
                else{
                    if (FCMP(f1->_Plan.Points[0].z, f2->_Plan.Points[0].z) < 0)
                        break;
                    }
                }
            else{
                if (sortfaces_cmp( f1, f2 ) >= 0)
                    break;
                }
            }
        }
    else*/{
        for (i=num-1; i >= 0; i--)   // sur 10 : de 9 à -1  >>>  donc insérer après, de 0 à 10
            {
            //if (sortfaces_cmp( f1, inst->FPtrs[i] ) >= 0)
                break;
            }
        }
    i++;
    if (i < (SLONG)inst->NumFaces) memmove( &inst->FPtrs[i+1], &inst->FPtrs[i], (inst->NumFaces - i) * 4 );
    inst->FPtrs[i] = f1;
    inst->NumFaces++;
}

//--------------------------------------------------------------

#define LIMIT_BACK  0x01
#define LIMIT_LEFT  0x02
#define LIMIT_RIGHT 0x04
#define LIMIT_BOT   0x08
#define LIMIT_TOP   0x10

ULONG IsLimit( LONG minx, LONG maxx, LONG miny, LONG maxy, FLOAT z, OOPoint *p )
{
  ULONG limit=0;

    if (FZERO(z))     limit |= LIMIT_BACK ;
    if (p->x == minx) limit |= LIMIT_LEFT ;
    if (p->x == maxx) limit |= LIMIT_RIGHT;
    if (p->y == miny) limit |= LIMIT_BOT  ;
    if (p->y == maxy) limit |= LIMIT_TOP  ;
    return( limit );
}

static void DrawFaces( OObject *camobj, OOInst_3DCamera *inst, OOTagList *attrlist )    // D3A_RastPort, OOA_WidthHeight
{
    OOFace *fa;
    OO3DPoint *p;
    ULONG i, j;
    UWORD num_2d_points;
    LONG minx, maxx, miny, maxy;
    OORastPort *rp = attrlist[0].ti_Ptr;
    UBYTE *lineinfo;

    //------ Calcul des constantes pour ce tour
    minx = 0;
    maxx = 2 * inst->Trans.AreaW2;
    miny = 0;
    maxy = 2 * inst->Trans.AreaH2;

    //------ Dessins des faces
    for (i=0; i < inst->NumFaces; i++)
        {
        OOPoint old, *dst;

        fa = inst->FPtrs[i];

        //--- Projection d'affichage
        dst = Points2D;
        for (j=num_2d_points=0, p=&fa->_Plan.Points[0]; j < fa->_Plan.NumPoints; j++, p++)
            {
            dst->x = inst->Trans.AreaW2 + F2LONG(FMUL(p->X, inst->Trans.DispCoefX));
            dst->y = inst->Trans.AreaH2 - F2LONG(FMUL(p->Y, inst->Trans.DispCoefY));
            if (j == 0 || dst->x != old.x || dst->y != old.y)
                { old = *dst++; Index3D[num_2d_points++] = (UBYTE)j; }
            }
        if (num_2d_points > 1)
            {
            if (Points2D[0].x == Points2D[num_2d_points-1].x && Points2D[0].y == Points2D[num_2d_points-1].y)
                {
                num_2d_points--;                        // Après_le_dernier est égal au premier
                }
            else
                Points2D[num_2d_points] = Points2D[0];  // Après_le_dernier est égal au premier
            }

        //--- Paramètres pour le tracé de polygones
        lineinfo = NULL;
        fa->_P.NumPoints = num_2d_points;
        fa->_P.Points = Points2D;
        // Empêche le tracé de lignes sur les bords clippés
        if ((num_2d_points > 2) && (fa->_P.Flags & OOF_LINES))
            {
            if (fa->_P.Flags & OOF_CLIPPED)
                {
                LONG limit, new;
                OOPoint *pt = &Points2D[0];
                lineinfo = LineInfo;
                limit = IsLimit( minx, maxx, miny, maxy, fa->_Plan.Points[0].Z, pt );
                for (j=1, pt++; j <= num_2d_points; j++, pt++)  // <= car après_le_dernier est égal au premier
                    {
                    new = IsLimit( minx, maxx, miny, maxy, fa->_Plan.Points[j].Z, pt );
                    LineInfo[j-1] = (limit & new) ? 0 : 1;
                    limit = new;
                    }
                }

            }
        // Dessine soit point, droite, ou polygone. Points2D[num_2d_points] doit contenir Points2D[0]
        OOGfx_DrawPolygon( rp, &fa->_P, lineinfo );
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
