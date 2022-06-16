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


/******* Includes ************************************************/

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_pool.h"
#include "oo_3d_prv.h"
#include "3dsfile.h"

#define INDEX_D3A_TimeAccel 0
#define INDEX_D3A_Gravity   1
#define INDEX_D3A_Density   2
#define INDEX_D3A_SizeX     3
#define INDEX_D3A_SizeY     4
#define INDEX_D3A_SizeZ     5


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static void AbsoluteAttachRotate( OObject *space, OObject *child );

static ULONG AddActor( OOInst_3DSpace *inst, OObject *new );
static void RemoveActor( OOInst_3DSpace *inst, OObject *rem );
static void SendToActors( OOInst_3DSpace *inst, OObject *obj );

static void SendToObjList( OObjArray *oa, ULONG method, OOTagList *attrs );


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_3DSpace( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_3DSpace *inst = OOINST_DATA(cl,obj);
                // Init instance datas
                OOAr_Init( &inst->DispList, TRUE );
                OOAr_Init( &inst->HearList, TRUE );
                OOAr_Init( &inst->DeadList, TRUE );
                OOAr_Init( &inst->ActorsList, TRUE );
                OOAr_Init( &inst->ScreensList, TRUE );
                OOAr_Init( &inst->SpeakersList, TRUE );
                OOList_Init( &inst->LoadList );
                // Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
            OOInst_3DSpace *inst = OOINST_DATA(cl,obj);
            OONode *node, *next;
#ifdef AMIGA
            if (inst->PicObj && *inst->PicObj && inst->PicHandle) OO_DoMethod( *inst->PicObj, OOM_PIC_UNLOAD, 0, inst->PicHandle );
#endif
            for (node=inst->LoadList.lh_Head; next = node->ln_Succ; node = next) OOPool_Free( OOGlobalPool, node );
            OOAr_Cleanup( &inst->DispList );
            OOAr_Cleanup( &inst->HearList );
            OOAr_Cleanup( &inst->DeadList );
            OOAr_Cleanup( &inst->ActorsList );
            OOAr_Cleanup( &inst->ScreensList );
            OOAr_Cleanup( &inst->SpeakersList );
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
        case D3M_MOVEALL:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_Handle3D && attrlist[1].ti_Tag == D3A_ElapsedTime );
            {
            OOInst_3DSpace *inst = OOINST_DATA(cl,obj);
            OOTagList *attrs = OOINST_ATTRS(cl,obj);
            OObject *child, **tab;
            ULONG childcount = obj->ChildrenTable.Count;
            OOInst_3DSolid *sol;
            FLOAT secs, timeaccel = attrs[INDEX_D3A_TimeAccel].ti_Float;
            ULONG add, i, num_abs_rotations, num_attached, elapsedtime = attrlist[1].ti_Data;

            secs = FMUL( attrs[INDEX_D3A_TimeAccel].ti_Float, FDIV(F2FLT(elapsedtime),(FLOAT)50) );

            OOAr_Init( &inst->DispList, FALSE );
            OOAr_Init( &inst->HearList, FALSE );
            OOAr_Init( &inst->DeadList, FALSE );

            if (childcount)
                {
                //------ Préparation avant boucle de déplacement
                for (tab=obj->ChildrenTable.Table, i=0, child=*tab; i < childcount; i++, child=*++tab)
                    {
                    sol = D3SOL(child);
                    sol->ColObject = 0;
                    sol->Flags &= ~(D3F_SOLID_TYPECHANGED | D3F_DO_ABSATTACH | D3F_DO_ABSROTATION | D3F_DO_SELFROTATE | D3F_SOLID_ISVISIBLE | D3F_SOLID_ISAUDIBLE | D3F_SOLID_ISMOVING | D3F_UNDER_COLLISION);
                    sol->Speed.Ax = (FLOAT)0;
                    sol->Speed.Ay = (FLOAT)0;
                    sol->Speed.Az = (FLOAT)0;
                    }

                //------ Translation des centres d'objets et rotation sur eux-mêmes
                num_abs_rotations = num_attached = 0;
                for (tab=obj->ChildrenTable.Table, i=0, child=*tab; i < childcount; i++, child=*++tab)
                    {
                    sol = D3SOL(child);

                    // Une accélération n'existe que si une force est exercée par un acteur ou l'objet lui-même (moteur...)
                    if (sol->Sensitivity != 0)
                        SendToActors( inst, child ); // Exemple si gravité : modif de l'accélération : chaque objet peut être attiré par plusieurs attracteurs (pesanteur, aimants, ...)

					// Chaque objet change son accélération (et dans certains cas, sa vitesse)
					// Il peut changer son type -> le flag D3F_SOLID_TYPECHANGED est positionné
                    OO_DoMethod( child, D3M_SETSPEED, NULL );
					// Si type changé, on force les recopies et recalculs
					if (sol->Flags & D3F_SOLID_TYPECHANGED)
						sol->Flags |= D3F_SOLID_ISMOVING;

                    // Rotation de l'objet autour d'un point
                    if (sol->Flags & (D3F_SOLID_CENTER_OBJ|D3F_SOLID_CENTER_ABS))
                        {
                        sol->Rot.Ax = FADD(sol->Rot.Ax, FMUL(sol->Rot.Ix, secs));
                        sol->Rot.Ay = FADD(sol->Rot.Ay, FMUL(sol->Rot.Iy, secs));
                        sol->Rot.Az = FADD(sol->Rot.Az, FMUL(sol->Rot.Iz, secs));
						// Si rotation du centre à faire
                        if (!FZERO(sol->Rot.Ax) || !FZERO(sol->Rot.Ay) || !FZERO(sol->Rot.Az))
							{
                            sol->Flags |= (D3F_DO_ABSROTATION);
                            num_abs_rotations++;
							}
                        }
                    // Rotation de l'objet autour de son centre
                    if (sol->Flags & D3F_SOLID_CENTER_SELF)
                        {
                        sol->Rot.SAx = FADD(sol->Rot.SAx, FMUL(sol->Rot.SIx, secs));
                        sol->Rot.SAy = FADD(sol->Rot.SAy, FMUL(sol->Rot.SIy, secs));
                        sol->Rot.SAz = FADD(sol->Rot.SAz, FMUL(sol->Rot.SIz, secs));
						// Si différence avec les angles précédents
                        if (FCMP(sol->Rot.SAx,sol->Rot.OldSAx)!=0L || FCMP(sol->Rot.SAy,sol->Rot.OldSAy)!=0L || FCMP(sol->Rot.SAz,sol->Rot.OldSAz)!=0L)
                            {
							sol->Rot.OldSAx = sol->Rot.SAx; 
							sol->Rot.OldSAy = sol->Rot.SAy; 
							sol->Rot.OldSAz = sol->Rot.SAz; 
                            sol->Flags |= (D3F_SOLID_ISMOVING|D3F_DO_SELFROTATE);
							}
                        }

                    // Eh patron, un demi de gammatédeu plus vézéroté plus ixeuzéro, siouplait
                    if (sol->AttachObj != NULL)
                        {
                        sol->Flags |= D3F_DO_ABSATTACH;
                        num_attached++;
                        }
                    else{
                        if (!FZERO(sol->Speed.Ax) || !FZERO(sol->Speed.Vx))
                            {
                            sol->Speed.nx = FADD(FADD(FDIV(FMUL(sol->Speed.Ax,FMUL(secs,secs)),(FLOAT)2), FMUL(sol->Speed.Vx,secs)), sol->X);
                            sol->Speed.Vx = FADD(sol->Speed.Vx, FMUL(sol->Speed.Ax,secs));
                            }
                        else sol->Speed.nx = sol->X;
                        if (!FZERO(sol->Speed.Ay) || !FZERO(sol->Speed.Vy))
                            {
                            sol->Speed.ny = FADD(FADD(FDIV(FMUL(sol->Speed.Ay,FMUL(secs,secs)),(FLOAT)2), FMUL(sol->Speed.Vy,secs)), sol->Y);
                            sol->Speed.Vy = FADD(sol->Speed.Vy, FMUL(sol->Speed.Ay,secs));
                            }
                        else sol->Speed.ny = sol->Y;
                        if (!FZERO(sol->Speed.Az) || !FZERO(sol->Speed.Vz))
                            {
                            sol->Speed.nz = FADD(FADD(FDIV(FMUL(sol->Speed.Az,FMUL(secs,secs)),(FLOAT)2), FMUL(sol->Speed.Vz,secs)), sol->Z);
                            sol->Speed.Vz = FADD(sol->Speed.Vz, FMUL(sol->Speed.Az,secs));
                            }
                        else sol->Speed.nz = sol->Z;

                        // J'en vois un qui bouge, là :
                        if (FCMP(sol->Speed.nx,sol->X) || FCMP(sol->Speed.ny,sol->Y) || FCMP(sol->Speed.nz,sol->Z))
                            {
                            sol->Flags |= D3F_SOLID_ISMOVING;
							OO_DoMethodV( child, D3M_DOSPACECOL, D3A_ObjSpace, obj, TAG_END ); // collision de l'objet avec les bords de cet espace
                            }
                        }

					// Rotation puis translation de tous les sommets par rapport au centre de l'espace
					if (sol->Flags & D3F_SOLID_ISMOVING && sol->Type != NULL)
						{
						OO_DoMethodV( child, D3M_BUILDVERTICES, TAG_END ); // collision de l'objet avec les bords de cet espace
						}
                    }

                //------ Attachement et rotation de l'objet autour d'un point
                if (num_attached != 0 || num_abs_rotations != 0)
                    for (tab=obj->ChildrenTable.Table, i=0, child=*tab; i < childcount; i++, child=*++tab)
                        if (D3SOL(child)->Flags & (D3F_DO_ABSATTACH|D3F_DO_ABSROTATION))
                            AbsoluteAttachRotate( obj, child ); // Récursive

                /*
                //------ Détection des collisions avec les limites de l'espace
                for (tab=obj->ChildrenTable.Table, i=0, child=*tab; i < childcount; i++, child=*++tab)
                    if ((D3SOL(child)->Flags & (D3F_SOLID_ISMOVING|D3F_SOLID_COLLIDE)) == (D3F_SOLID_ISMOVING|D3F_SOLID_COLLIDE))
                        OO_DoMethodV( child, D3M_DOSPACECOL, D3A_ObjSpace, obj, TAG_END );

                //------ Détection des collisions avec les autres objets
                for (tab=obj->ChildrenTable.Table, i=0, child=*tab; i < childcount; i++, child=*++tab)
                    {
                    sol = D3SOL(child);
                    if (sol->Flags & D3F_SOLID_COLLIDE)
                        OO_DoMethod( child, D3M_DOOBJECTCOL, NULL );
                    }
                */

                //------ Action propre à chaque objet, y compris destruction
                for (tab=obj->ChildrenTable.Table, i=0, child=*tab; i < childcount; i++, child=*++tab)
                    {
                    add = OO_DoMethod( child, D3M_DOACTION, NULL ); // Si OOV_OK : on peut afficher l'objet
                    // Un objet détruit existe toujours, mais est dans la DeadList, voir D3M_DEADCHILD ci-dessous
                    sol = D3SOL(child);
                    sol->X = sol->Speed.nx;
                    sol->Y = sol->Speed.ny;
                    sol->Z = sol->Speed.nz;
                    if (add == OOV_OK)
                        {
                        if ((sol->Flags & D3F_SOLID_VISIBLE) && sol->Type != NULL)
                            {
                            OO_DoMethod( child, D3M_SETIMAGE, NULL );
                            OOAr_Add( &inst->DispList, child, OOV_DISP_BLOCKSIZE );
                            }
                        if (sol->Flags & D3F_SOLID_AUDIBLE)
                            {
                            OO_DoMethod( child, D3M_SETSOUND, NULL );
                            OOAr_Add( &inst->HearList, child, OOV_HEAR_BLOCKSIZE );
                            }
                        }
                    }

                //------ Delete réel des objets détruits
                if (childcount = inst->DeadList.Count)
                    {
                    tab = inst->DeadList.Table;
                    for (tab=obj->ChildrenTable.Table, i=0, child=*tab; i < childcount; i++, child=*++tab)
                        OO_DoMethod( child, OOM_DELETE, NULL );
                    }
                }
            break;
            }

        case D3M_DEADCHILD:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Object );
            {
            OOInst_3DSpace *inst = OOINST_DATA(cl,obj);
            if (OOAr_Add( &inst->DeadList, attrlist[0].ti_Ptr, OOV_DEAD_BLOCKSIZE ))
                OO_DetachParents( attrlist[0].ti_Ptr );
            break;
            }

        //---------------
        case D3M_START3D:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_Handle3D && attrlist[1].ti_Tag == D3A_ObjSpace );
            attrlist[1].ti_Ptr = obj; // Chaque 3DSpace met à jour le pointeur avec lui-même
            OO_DoChildrenMethod( 0, obj, method, attrlist );
            break;

        case D3M_STOP3D:
            OO_DoChildrenMethod( 0, obj, method, attrlist );
            break;

        //---------------
        case D3M_CLEARVIEW:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort );
			{
            OOInst_3DSpace *inst = (OOInst_3DSpace*) OOINST_DATA(cl,obj);
            SendToObjList( &inst->ScreensList, D3M_CLEARSCREEN, attrlist );
			}
            break;

        case D3M_OBJTOVIEW:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort && attrlist[1].ti_Tag == D3A_ObjTable && attrlist[2].ti_Tag == D3A_ObjCount );
            {
            OOInst_3DSpace *inst = (OOInst_3DSpace*) OOINST_DATA(cl,obj);
            // Envoi des objets visibles aux caméras via leur écran
            if (inst->DispList.Count)
                {
                attrlist[1].ti_Ptr = inst->DispList.Table;
                attrlist[2].ti_Data = inst->DispList.Count;
                SendToObjList( &inst->ScreensList, D3M_OBJTOSCREEN, attrlist );
                }
            // Envoi des objets audibles aux microphones via leur haut-parleur
            if (inst->SpeakersList.Count)
                {
                attrlist[1].ti_Ptr = inst->HearList.Table;
                attrlist[2].ti_Data = inst->HearList.Count;
                SendToObjList( &inst->SpeakersList, D3M_OBJTOSPEAKER, attrlist );
                }
            }
            break;

        case D3M_REFRESHVIEW:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort );
			{
            OOInst_3DSpace *inst = (OOInst_3DSpace*) OOINST_DATA(cl,obj);
            SendToObjList( &inst->ScreensList, D3M_DRAWSCREEN, attrlist );
            SendToObjList( &inst->SpeakersList, D3M_PLAYSOUND, attrlist );
			}
            break;

        case D3M_SYNCVIEW:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort );
			{
            OOInst_3DSpace *inst = (OOInst_3DSpace*) OOINST_DATA(cl,obj);
            SendToObjList( &inst->ScreensList, D3M_SYNCSCREEN, attrlist );
			}
            break;

        //---------------
        case D3M_ADDSCREEN:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_ObjScreen );
            return OOAr_Add( &(((OOInst_3DSpace*)OOINST_DATA(cl,obj))->ScreensList), attrlist[0].ti_Ptr, OOV_SCREENS_BLOCKSIZE );

        case D3M_REMOVESCREEN:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_ObjScreen );
            OOAr_Remove( &(((OOInst_3DSpace*)OOINST_DATA(cl,obj))->ScreensList), attrlist[0].ti_Ptr );
            break;

        case D3M_ADDSPEAKER:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_ObjSpeaker );
            return OOAr_Add( &(((OOInst_3DSpace*)OOINST_DATA(cl,obj))->SpeakersList), attrlist[0].ti_Ptr, OOV_SPEAKERS_BLOCKSIZE );

        case D3M_REMOVESPEAKER:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_ObjSpeaker );
            OOAr_Remove( &(((OOInst_3DSpace*)OOINST_DATA(cl,obj))->SpeakersList), attrlist[0].ti_Ptr );
            break;

        case D3M_ADDACTOR:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Object );
            return AddActor( OOINST_DATA(cl,obj), attrlist[0].ti_Ptr );

        case D3M_REMOVEACTOR:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Object );
            RemoveActor( OOINST_DATA(cl,obj), attrlist[0].ti_Ptr );
            break;
        //---------------
        case D3M_LOADSCENE:
            {
                char *FileName = (char *)OOTag_GetData( attrlist,D3A_SceneFileName,(ULONG)NULL );
                OOASSERT( attrlist != NULL );
                if( FileName!=NULL )
                    ;//ReadFile3DS( OOINST_DATA(cl,obj), FileName );
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

static void AbsoluteAttachRotate( OObject *space, OObject *child )
{
    OOInst_3DSolid *sr, *so = D3SOL(child);
    OO3DPoint *p = NULL;

    // Rotation absolue autour d'un éventuel objet (doit être récursive, avec flag indiquant que la rotation absolue est faite)
    if (so->Flags & D3F_DO_ABSATTACH)
        {
        // Rotation des objets dont celui-ci dépend
        sr = D3SOL(so->AttachObj);
        if (sr->Flags & (D3F_DO_ABSATTACH|D3F_DO_ABSROTATION))
            AbsoluteAttachRotate( space, so->AttachObj );
        // Si attaché, voir pour rotation autour de l'objet
        if (so->Flags & D3F_SOLID_CENTER_OBJ)
            p = (OO3DPoint*) &sr->X;
        // Sinon pas de rotation du tout : on calque juste le mouvement
        else{
            so->Speed.nx = FADD(sr->X, so->AttachVec.X);
            so->Speed.ny = FADD(sr->Y, so->AttachVec.Y);
            so->Speed.nz = FADD(sr->Z, so->AttachVec.Z);
            }
        }
    // Rotation autour d'un éventuel centre absolu
    else if (so->Flags & D3F_SOLID_CENTER_ABS)
        p = (OO3DPoint*) &so->Rot.Cx;

    // Alors cette rotation du centre ?
    if (p != NULL)
        {
        so->Speed.nx = so->AttachVec.X;
        so->Speed.ny = so->AttachVec.Y;
        so->Speed.nz = so->AttachVec.Z;
        OO3DMath_Rotate( (OO3DPoint*)&so->Speed.nx, (OO3DPoint*)&so->Rot.Ax );
        so->Speed.nx = FADD(so->Speed.nx, p->X);
        so->Speed.ny = FADD(so->Speed.ny, p->Y);
        so->Speed.nz = FADD(so->Speed.nz, p->Z);
        }

    // Flag mouvement, collision sur limites de l'espace
    if (FCMP(so->Speed.nx,so->X) || FCMP(so->Speed.ny,so->Y) || FCMP(so->Speed.nz,so->Z))
        {
        so->Flags |= D3F_SOLID_ISMOVING;
        OO_DoMethodV( child, D3M_DOSPACECOL, D3A_ObjSpace, space, TAG_END ); // collision de l'objet avec les bords de cet espace
        }

    // Pour ne pas le faire 36 fois, car récursive
    so->Flags &= ~(D3F_DO_ABSATTACH|D3F_DO_ABSROTATION);
}

//------------------------------------------------------------------------

static void SendToObjList( OObjArray *oa, ULONG method, OOTagList *attrs )
{
    ULONG i;
    OObject *dest;

    for (i=0; i < oa->Count; i++)
        {
        dest = oa->Table[i];
        OOASSERT( dest != NULL );
        OO_DoMethod( dest, method, attrs );
        }
}

//--------------------------------------------------------------

static ULONG AddActor( OOInst_3DSpace *inst, OObject *obj )
{
    if (D3SOL(obj)->Actions == 0) return OOV_ERROR;
    return OOAr_Add( &inst->ActorsList, obj, OOV_ACTORS_BLOCKSIZE );
}

static void RemoveActor( OOInst_3DSpace *inst, OObject *rem )
{
    OOAr_Remove( &inst->ActorsList, rem );
}

static void SendToActors( OOInst_3DSpace *inst, OObject *obj )
{
    OObjArray *oa = &inst->ActorsList;
    ULONG i;
    OObject *dest;
    OOInst_3DSolid *so = D3SOL(obj);

    for (i=0; i < oa->Count; i++)
        {
        dest = oa->Table[i];
        OOASSERT( dest != NULL );
        if (D3SOL(dest)->Actions & so->Sensitivity)
            OO_DoMethodV( dest, D3M_OBJ2ACTOR, OOA_Object, obj, TAG_END );
        }
}


/***************************************************************************
 *
 *      Attributes setting
 *
 ***************************************************************************/

// Automatic tic tic tic    BOOOOMMMM


/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOFltAttrDesc attrs[] = {
    { D3A_TimeAccel, 1.0F    , "D3A_TimeAccel"  , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Gravity  , 9.81F   , "D3A_Gravity"    , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Density  , 1.0F    , "D3A_Density"    , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SizeX    , MAXFLT, "D3A_SizeX"        , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SizeY    , MAXFLT, "D3A_SizeY"        , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SizeZ    , MAXFLT, "D3A_SizeZ"        , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOV_TAG_END, (FLOAT)0 }
    };

OOClass Class_3DSpace = {
    Dispatcher_3DSpace,
    &Class_Root,            // Base class
    "Class_3DSpace",        // Class Name
    0,                      // InstOffset
    sizeof(OOInst_3DSpace), // InstSize  : Taille de l'instance
    (OOAttrDesc*)attrs,                 // AttrsDesc : Table de description des attributs
    sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)  // AttrsSize : size of instance own taglist
    };
