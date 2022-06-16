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
#include "oo_buf.h"
#include "oo_pool.h"
#include "oo_sound.h"
#include "oo_3d_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static void subadd_attrs( OOClass *cl, OObject *obj, OOTagItem *taglist, ULONG sub );
static ULONG SetGet_All( OOSetGetParms *sgp );


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_3DSolid( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_3DSolid *inst = OOINST_DATA(cl,obj);

                // Init instance datas
                inst->X = inst->Y = inst->Z = (FLOAT)0;
                inst->SizeX = inst->SizeY = inst->SizeZ = (FLOAT).10;
                inst->R = (FLOAT)0;
                inst->Profile = (FLOAT)200; // l'aérodynamisme d'un parachute
                inst->Masse = (FLOAT)1;   // 1kg
                inst->Rot.Cx = inst->Rot.Cz = inst->Rot.Ix = inst->Rot.Iy = inst->Rot.Iz = inst->Rot.SIx = inst->Rot.SIy = inst->Rot.SIz = (FLOAT)0;
                inst->Rot.Ax = inst->Rot.SAx = OO3DMath_Deg2Rad((FLOAT)0);
                inst->Rot.Ay = inst->Rot.SAy = OO3DMath_Deg2Rad((FLOAT)0);
                inst->Rot.Az = inst->Rot.SAz = OO3DMath_Deg2Rad((FLOAT)0);
                inst->Speed.Vx = inst->Speed.Ax = inst->Speed.nx = (FLOAT)0;
                inst->Speed.Vy = inst->Speed.Ay = inst->Speed.ny = (FLOAT)0;
                inst->Speed.Vz = inst->Speed.Az = inst->Speed.nz = (FLOAT)0;
                inst->Flags = D3F_SOLID_COLLIDE | D3F_SOLID_VISIBLE;
                inst->DefFaces  = NULL;
                inst->DefCoords = NULL;
                inst->RotCoords = NULL;
                inst->ColObject = NULL;
                inst->Name = NULL;
                inst->CBFunc_Speed = NULL;

                // Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); return (ULONG)NULL; }

                // Prise en compte des définitions de faces
                if (inst->DefFaces && inst->DefCoords)
                    {
                    UWORD *p;
                    OOFaceDef *f;
                    UWORD i, ind, numfaces;
                    FLOAT flt, minx=(FLOAT)0, maxx=(FLOAT)0, miny=(FLOAT)0, maxy=(FLOAT)0, minz=(FLOAT)0, maxz=(FLOAT)0;
                    for (numfaces=0, p=(UWORD*)inst->DefFaces; *p != ENDFACES; numfaces++) // *p=f->NumPoints, si 0 : fin des faces
                        {
                        f = (OOFaceDef*)p;
                        p = (UWORD*)&f->CoordIndex[0];
                        for (i=0; i < f->NumPoints; i++, p++)
                            {
                            ind = f->CoordIndex[i];
                            if (ind > inst->NumCoords) inst->NumCoords = ind;
                            flt = inst->DefCoords[ind].X;   if (FCMP(flt,minx) < 0) minx = flt;
                                                            if (FCMP(flt,maxx) > 0) maxx = flt;
                            flt = inst->DefCoords[ind].Y;   if (FCMP(flt,miny) < 0) miny = flt;
                                                            if (FCMP(flt,maxy) > 0) maxy = flt;
                            flt = inst->DefCoords[ind].Z;   if (FCMP(flt,minz) < 0) minz = flt;
                                                            if (FCMP(flt,maxz) > 0) maxz = flt;
                            }
                        }
                    inst->NumCoords++;
                    if (! (inst->RotCoords = (APTR) OOPool_Alloc( OOGlobalPool, inst->NumCoords*sizeof(OO3DPoint) )))
                        { OO_DoMethod( obj, OOM_DELETE, NULL ); return (ULONG)NULL; }
                    minx = FNEG(minx); inst->SizeX = FMUL((FLOAT)2, FMAX(maxx,minx));
                    miny = FNEG(miny); inst->SizeY = FMUL((FLOAT)2, FMAX(maxy,miny));
                    minz = FNEG(minz); inst->SizeZ = FMUL((FLOAT)2, FMAX(maxz,minz));
                    }

                // Calcul du rayon de la sphère englobante
                if (inst->Flags & D3F_SOLID_SPHERE)
                    {
                    inst->SizeX = inst->SizeY = inst->SizeZ = FMUL((FLOAT)2,inst->R);
                    }
                else if (FZERO(inst->R))
                    {
                    OO3DPoint P;
                    P.X = FDIV(inst->SizeX,(FLOAT)2);
                    P.Y = FDIV(inst->SizeY,(FLOAT)2);
                    P.Z = FDIV(inst->SizeZ,(FLOAT)2);
                    inst->R = OO3DMath_Modulo( &P );
                    }
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
            OOInst_3DSolid *inst = OOINST_DATA(cl,obj);
            if (inst->Sound != NULL)
                {
                OO_Delete( inst->Sound );
                }
            if (inst->ColObject != NULL)
                {
                OObject **o = &(D3SOL(inst->ColObject)->ColObject);
                if (*o == obj) *o = NULL;
                }
            OOPool_Free( OOGlobalPool, (UBYTE*)inst->RotCoords );
            if (inst->Flags & D3F_LOADALLOC) // Loaded coords
                {
                OOBuf_Free( &inst->DefCoords );
                OOBuf_Free( &inst->DefFaces );
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
        case D3M_START3D:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_Handle3D && attrlist[1].ti_Tag == D3A_ObjSpace );
            {
            OOInst_3DSolid *inst = OOINST_DATA(cl,obj);
            // Prépare l'attachement à un autre objet
            if (inst->AttachObj != NULL)
                {
                OOInst_3DSolid *sa = D3SOL(inst->AttachObj);
                inst->AttachVec.X = inst->X - sa->X; // Vecteur pour joindre les 2 objets
                inst->AttachVec.Y = inst->Y - sa->Y;
                inst->AttachVec.Z = inst->Z - sa->Z;
                }
            // Prépare la rotation autour d'un centre absolu
            else if (inst->Flags & D3F_SOLID_CENTER_ABS)
                {
                inst->AttachVec.X = inst->X - inst->Rot.Cx; // Vecteur pour joindre l'objet au centre de rotation
                inst->AttachVec.Y = inst->Y - inst->Rot.Cy;
                inst->AttachVec.Z = inst->Z - inst->Rot.Cz;
                }
            // Enregistre en tant qu'acteur
            if (inst->Actions != 0)
                {
                OObject *space = OO_GetParentOfClass( obj, &Class_3DSpace );
                if (space != NULL)
                    return OO_DoMethodV( space, D3M_ADDACTOR, OOA_Object, obj, TAG_END );
                return OOV_ERROR;
                }
            }
            break;

        case D3M_STOP3D:
            {
            OOInst_3DSolid *inst = OOINST_DATA(cl,obj);
            if (inst->Actions != 0)
                {
                OObject *space = OO_GetParentOfClass( obj, &Class_3DSpace );
                if (space != NULL)
                    return OO_DoMethodV( space, D3M_REMOVEACTOR, OOA_Object, obj, TAG_END );
                return OOV_ERROR;
                }
            }

        case D3M_SETSPEED:
            {
            OOInst_3DSolid *inst = OOINST_DATA(cl,obj);
            if (inst->CBFunc_Speed) (*inst->CBFunc_Speed)( inst );
            }
            break;

        case D3M_SETIMAGE:  // Choix de l'image pour ce tour
            break;

        case D3M_SETSOUND:  // Choix du son pour ce tour
            {
            OOInst_3DSolid *inst = OOINST_DATA(cl,obj);
            UBYTE *filename;
            if (inst->Flags & D3F_SOLID_AUDIBLE && inst->Sound == NULL)
                if ((filename = (UBYTE*) OO_GetAttr( obj, OOA_SoundFileName )) != NULL)
                    inst->Sound = OO_NewV( &Class_Sound,
                            OOA_SoundFileName,  filename,
                            OOA_SoundLoop,      TRUE,
                            OOA_SoundStreamed,  FALSE,
                            OOA_SoundPreLoad,   FALSE,
                            TAG_END );
            }
            break;

        case D3M_DOACTION:
            break;

        case D3M_SUB:
            subadd_attrs( cl, obj, attrlist, 1 );
            break;

        case D3M_ADD:
            subadd_attrs( cl, obj, attrlist, 0 );
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

static void subadd_attrs( OOClass *cl, OObject *obj, OOTagItem *ti, ULONG sub )
{
    OOTagItem setgettags[2];
    FLOAT fcur, fadd;

    setgettags[1].ti_Tag32 = OOV_TAG_END;
    setgettags[1].ti_More = NULL;
    for (; ; ti++)
        {
        OOTAG_GETITEM(ti);
        if (ti == NULL) break;

        // Get the current value
        setgettags[0].ti_Tag32 = ti->ti_Tag;
        setgettags[0].ti_Ptr = &fcur;
        OOPrv_MGet( obj->Class, obj, setgettags );
        // Get and convert value to add
        fadd = F2FLT(ti->ti_Data);
        // Compute and set the new value
        setgettags[0].ti_Float = sub ? FSUB(fcur,fadd) : FADD(fcur,fadd);
        OOPrv_MSet( obj->Class, obj, setgettags, OOM_SET );
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
    OOInst_3DSolid *inst = (OOInst_3DSolid*)sgp->Instance;
    ULONG change = OOV_NOCHANGE;

    if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
        {
        FLOAT angle, flt = sgp->ExtAttr->ti_Float;
        switch (sgp->ExtAttr->ti_Tag)
            {
            case D3A_X          : if (inst->X != flt) { change = OOV_CHANGED; inst->X = flt; } break;
            case D3A_Y          : if (inst->Y != flt) { change = OOV_CHANGED; inst->Y = flt; } break;
            case D3A_Z          : if (inst->Z != flt) { change = OOV_CHANGED; inst->Z = flt; } break;
            case D3A_R          : if (inst->R != flt) { change = OOV_CHANGED; inst->R = flt; } break;
            case D3A_SizeX      : if (inst->SizeX != flt) { change = OOV_CHANGED; inst->SizeX = flt; } break;
            case D3A_SizeY      : if (inst->SizeY != flt) { change = OOV_CHANGED; inst->SizeY = flt; } break;
            case D3A_SizeZ      : if (inst->SizeZ != flt) { change = OOV_CHANGED; inst->SizeZ = flt; } break;
            case D3A_SpeedVx    : if (inst->Speed.Vx != flt) { change = OOV_CHANGED; inst->Speed.Vx = flt; } break;
            case D3A_SpeedVy    : if (inst->Speed.Vy != flt) { change = OOV_CHANGED; inst->Speed.Vy = flt; } break;
            case D3A_SpeedVz    : if (inst->Speed.Vz != flt) { change = OOV_CHANGED; inst->Speed.Vz = flt; } break;
            case D3A_SpeedAx    : if (inst->Speed.Ax != flt) { change = OOV_CHANGED; inst->Speed.Ax = flt; } break;
            case D3A_SpeedAy    : if (inst->Speed.Ay != flt) { change = OOV_CHANGED; inst->Speed.Ay = flt; } break;
            case D3A_SpeedAz    : if (inst->Speed.Az != flt) { change = OOV_CHANGED; inst->Speed.Az = flt; } break;
            case D3A_AttachObj  : if (inst->AttachObj != *(OObject**)sgp->ExtAttr->ti_Ptr) { change = OOV_CHANGED; inst->AttachObj = *(OObject**)sgp->ExtAttr->ti_Ptr; } break;
            case D3A_RotCx      : if (inst->Rot.Cx != flt) { change = OOV_CHANGED; inst->Rot.Cx = flt; } break;
            case D3A_RotCy      : if (inst->Rot.Cy != flt) { change = OOV_CHANGED; inst->Rot.Cy = flt; } break;
            case D3A_RotCz      : if (inst->Rot.Cz != flt) { change = OOV_CHANGED; inst->Rot.Cz = flt; } break;
            case D3A_RotIx      : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.Ix  != flt) { change = OOV_CHANGED; inst->Rot.Ix  = angle; } break;
            case D3A_RotIy      : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.Iy  != flt) { change = OOV_CHANGED; inst->Rot.Iy  = angle; } break;
            case D3A_RotIz      : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.Iz  != flt) { change = OOV_CHANGED; inst->Rot.Iz  = angle; } break;
            case D3A_RotAx      : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.Ax  != flt) { change = OOV_CHANGED; inst->Rot.Ax  = angle; } break;
            case D3A_RotAy      : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.Ay  != flt) { change = OOV_CHANGED; inst->Rot.Ay  = angle; } break;
            case D3A_RotAz      : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.Az  != flt) { change = OOV_CHANGED; inst->Rot.Az  = angle; } break;
            case D3A_RotSIx     : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.SIx != flt) { change = OOV_CHANGED; inst->Rot.SIx = angle; } break;
            case D3A_RotSIy     : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.SIy != flt) { change = OOV_CHANGED; inst->Rot.SIy = angle; } break;
            case D3A_RotSIz     : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.SIz != flt) { change = OOV_CHANGED; inst->Rot.SIz = angle; } break;
            case D3A_RotSAx     : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.SAx != flt) { change = OOV_CHANGED; inst->Rot.SAx = angle; } break;
            case D3A_RotSAy     : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.SAy != flt) { change = OOV_CHANGED; inst->Rot.SAy = angle; } break;
            case D3A_RotSAz     : angle = OO3DMath_Deg2Rad( flt ); if (inst->Rot.SAz != flt) { change = OOV_CHANGED; inst->Rot.SAz = angle; } break;
            case D3A_Masse      : if (inst->Masse   != flt) { change = OOV_CHANGED; inst->Masse   = flt; } break;
            case D3A_Profile    : if (inst->Profile != flt) { change = OOV_CHANGED; inst->Profile = flt; } break;
            case D3A_DefFaces   : if (inst->DefFaces  != sgp->ExtAttr->ti_Ptr) { change = OOV_CHANGED; inst->DefFaces  = sgp->ExtAttr->ti_Ptr; } break;
            case D3A_DefCoords  : if (inst->DefCoords != sgp->ExtAttr->ti_Ptr) { change = OOV_CHANGED; inst->DefCoords = sgp->ExtAttr->ti_Ptr; } break;
            case D3A_DefTextures: if (inst->DefTextures != sgp->ExtAttr->ti_Ptr) { change = OOV_CHANGED; inst->DefTextures = sgp->ExtAttr->ti_Ptr; } break;
            case D3A_CBFuncSpeed: if (inst->CBFunc_Speed != sgp->ExtAttr->ti_Ptr) { change = OOV_CHANGED; inst->CBFunc_Speed = sgp->ExtAttr->ti_Ptr; } break;
            case D3A_Actions    : if (inst->Actions   != sgp->ExtAttr->ti_Data) { change = OOV_CHANGED; inst->Actions       = sgp->ExtAttr->ti_Data; } break;
            case D3A_Sensitivity: if (inst->Sensitivity != sgp->ExtAttr->ti_Data) { change = OOV_CHANGED; inst->Sensitivity = sgp->ExtAttr->ti_Data; } break;
            }
        sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
        }
    else{
        FLOAT *fltptr = (FLOAT*) sgp->ExtAttr->ti_Ptr;
        switch (sgp->ExtAttr->ti_Tag)
            {
            case D3A_X : *fltptr = inst->X; break;
            case D3A_Y : *fltptr = inst->Y; break;
            case D3A_Z : *fltptr = inst->Z; break;
            case D3A_R : *fltptr = inst->R; break;
            case D3A_SizeX : *fltptr = inst->SizeX; break;
            case D3A_SizeY : *fltptr = inst->SizeY; break;
            case D3A_SizeZ : *fltptr = inst->SizeZ; break;
            case D3A_SpeedVx: *fltptr = inst->Speed.Vx; break;
            case D3A_SpeedVy: *fltptr = inst->Speed.Vy; break;
            case D3A_SpeedVz: *fltptr = inst->Speed.Vz; break;
            case D3A_SpeedAx: *fltptr = inst->Speed.Ax; break;
            case D3A_SpeedAy: *fltptr = inst->Speed.Ay; break;
            case D3A_SpeedAz: *fltptr = inst->Speed.Az; break;
            case D3A_AttachObj: *(ULONG*)sgp->ExtAttr->ti_Ptr = (ULONG)inst->AttachObj; break;
            case D3A_RotCx : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Cx  ); break;
            case D3A_RotCy : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Cy  ); break;
            case D3A_RotCz : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Cz  ); break;
            case D3A_RotIx : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Ix  ); break;
            case D3A_RotIy : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Iy  ); break;
            case D3A_RotIz : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Iz  ); break;
            case D3A_RotAx : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Ax  ); break;
            case D3A_RotAy : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Ay  ); break;
            case D3A_RotAz : *fltptr = OO3DMath_Rad2Deg( inst->Rot.Az  ); break;
            case D3A_RotSIx: *fltptr = OO3DMath_Rad2Deg( inst->Rot.SIx ); break;
            case D3A_RotSIy: *fltptr = OO3DMath_Rad2Deg( inst->Rot.SIy ); break;
            case D3A_RotSIz: *fltptr = OO3DMath_Rad2Deg( inst->Rot.SIz ); break;
            case D3A_RotSAx: *fltptr = OO3DMath_Rad2Deg( inst->Rot.SAx ); break;
            case D3A_RotSAy: *fltptr = OO3DMath_Rad2Deg( inst->Rot.SAy ); break;
            case D3A_RotSAz: *fltptr = OO3DMath_Rad2Deg( inst->Rot.SAz ); break;
            case D3A_Masse  : *fltptr = inst->Masse ; break;
            case D3A_Profile: *fltptr = inst->Profile; break;
            case D3A_DefFaces : *(ULONG*)sgp->ExtAttr->ti_Ptr = (ULONG)inst->DefFaces ; break;
            case D3A_DefCoords: *(ULONG*)sgp->ExtAttr->ti_Ptr = (ULONG)inst->DefCoords; break;
            case D3A_DefTextures: *(ULONG*)sgp->ExtAttr->ti_Ptr = (ULONG)inst->DefTextures; break;
            case D3A_CBFuncSpeed: *(ULONG*)sgp->ExtAttr->ti_Ptr = (ULONG)inst->CBFunc_Speed; break;
            case D3A_Actions    : *(ULONG*)sgp->ExtAttr->ti_Ptr = (ULONG)inst->Actions      ; break;
            case D3A_Sensitivity: *(ULONG*)sgp->ExtAttr->ti_Ptr = (ULONG)inst->Sensitivity  ; break;
            }
        }
    return change;
}

static ULONG SetGet_Flag( OOSetGetParms *sgp )
{
    OOInst_3DSolid *inst = (OOInst_3DSolid*)sgp->Instance;
    ULONG change = OOV_NOCHANGE;
    ULONG flag = 0;
    ULONG data = sgp->ExtAttr->ti_Data;

    if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
        {
        switch (sgp->ExtAttr->ti_Tag)
            {
            case D3A_CenterAbs      : flag = D3F_SOLID_CENTER_ABS   ; break;
            case D3A_CenterObj      : flag = D3F_SOLID_CENTER_OBJ   ; break;
            case D3A_CenterSelf     : flag = D3F_SOLID_CENTER_SELF  ; break;
            case D3A_Rubbery        : flag = D3F_SOLID_RUBBERY      ; break;
            case D3A_Collide        : flag = D3F_SOLID_COLLIDE      ; break;
            case D3A_Visible        : flag = D3F_SOLID_VISIBLE      ; break;
            case D3A_Audible        : flag = D3F_SOLID_AUDIBLE      ; break;
            case D3A_Sphere         : flag = D3F_SOLID_SPHERE       ; break;
            case D3A_SensGravity    : flag = D3F_SENS_GRAVITY       ; break;
            case D3A_SensMagnetism  : flag = D3F_SENS_MAGNETISM     ; break;
            case D3A_SensElectricity: flag = D3F_SENS_ELECTRICITY   ; break;
            case D3A_SensLuminosity : flag = D3F_SENS_LUMINOSITY    ; break;
            case D3A_SensCharism    : flag = D3F_SENS_CHARISM       ; break;
            case D3A_SensMagic      : flag = D3F_SENS_MAGIC         ; break;
            case D3A_Sens_7         : flag = D3F_SENS_7             ; break;
            case D3A_Sens_8         : flag = D3F_SENS_8             ; break;
            }

        if (flag)
            {
            sgp->Attr->ti_Data = data;
            if (data == TRUE)
                 { change = (inst->Flags & flag) ? OOV_NOCHANGE : OOV_CHANGED ; inst->Flags |= flag; }
            else { change = (inst->Flags & flag) ? OOV_CHANGED  : OOV_NOCHANGE; inst->Flags &= ~flag; }
            }
        }
    else{
        *(ULONG*)sgp->ExtAttr->ti_Data = sgp->Attr->ti_Data;
        }
    return change;
}

static ULONG SetGet_File( OOSetGetParms *sgp )
{
    OOInst_3DSolid *inst = (OOInst_3DSolid*)sgp->Instance;
    ULONG change = OOV_NOCHANGE;

    if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
        {
        UBYTE *filename = sgp->ExtAttr->ti_Ptr;
        FILE *file;
        if (filename != NULL && OOTag_DataEqual( sgp->ExtAttr, sgp->Attr ) == FALSE)
            {
            if ((file = fopen( filename, "r" )) != NULL)
                {
                ULONG i, j, num_coords, num_faces, num_edges;
                UBYTE *mem_coords=NULL, *mem_faces=NULL;
                OO3DPoint *pt;
                UWORD tmp;
                double x, y, z;
                OOFaceDef FD;

                if (fscanf( file, "%d%d%d\n", &num_coords, &num_faces, &num_edges ) != 3) goto END_ERROR;
                if (num_coords == 0 || num_faces == 0) goto END_ERROR;

                //------ Allocation du buffer pour les coordonnées
                if ((mem_coords = OOBuf_Alloc( OOGlobalPool, num_coords * sizeof(OO3DPoint), 4 )) == NULL) goto END_ERROR;
                //------ Lecture des coordonnées
                for (i=0, pt=(OO3DPoint*)mem_coords; i < num_coords; i++, pt++)
                    {
                    if (fscanf( file, "%hf%hf%hf\n", &x, &y, &z ) != 3) goto END_ERROR;
                    pt->X = (FLOAT)x; pt->Y = (FLOAT)y; pt->Z = (FLOAT)z;
                    }

                //------ Allocation du buffer pour les faces
                if ((mem_faces = OOBuf_Alloc( OOGlobalPool, 0, 100 )) == NULL) goto END_ERROR;
                //------ Lecture des faces
                for (i=0; i < num_faces; i++)
                    {
                    // Préparation OOFaceDef
                    if (fscanf( file, "%hu", &FD.NumPoints ) != 1) goto END_ERROR;
                    FD.LineColor = 1; //+++ Voir pour la couleur...
                    FD.FillColor = 3; //+++ Voir pour la couleur...
                    FD.Flags = OOF_HIDE|OOF_FILL|OOF_LINES;
                    FD.TextureIndex = 0;
                    if (OOBuf_Paste( (char*)&FD, sizeof(OOFaceDef)-sizeof(UWORD), &mem_faces, MAXULONG, 0 ) == NULL) goto END_ERROR;
                    // CoordIndex[]
                    for (j=0; j < FD.NumPoints; j++)
                        {
                        if (fscanf( file, "%hu", &tmp ) != 1) goto END_ERROR;
                        tmp--; // Zero based
                        if (OOBuf_Paste( (char*)&tmp, sizeof(UWORD), &mem_faces, MAXULONG, 0 ) == NULL) goto END_ERROR;
                        }
                    }
                tmp = ENDFACES;
                if (OOBuf_Paste( (char*)&tmp, sizeof(UWORD), &mem_faces, MAXULONG, 0 ) == NULL) goto END_ERROR;

                //------ OK : remplacement des anciennes tables
                if (inst->Flags & D3F_LOADALLOC)
                    {
                    OOBuf_Free( &inst->DefCoords ); // NULL testé
                    OOBuf_Free( &inst->DefFaces ); // NULL testé
                    }
                inst->DefCoords = (OO3DPoint*)mem_coords;
                inst->DefFaces = (OOFaceDef*)mem_faces;
                inst->Flags |= D3F_LOADALLOC;
                mem_coords = mem_faces = NULL; // Ne pas libérer les nouvelles
                //------ Changer l'attribut
                sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
                change = OOV_CHANGED;

                //------ Fin : si erreur pas de remplacement
                END_ERROR:
                OOBuf_Free( &mem_coords ); // NULL testé
                OOBuf_Free( &mem_faces ); // NULL testé
                fclose( file );
                }
            }
        }
    else{
        *(ULONG*)sgp->ExtAttr->ti_Data = sgp->Attr->ti_Data;
        }
    return change;
}

/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_SoundVolume   , 0          , "SoundVolume"    , NULL          , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT }, // 0 à -10000 decibels
    { D3A_X             , 0          , "X"              , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Y             , 0          , "Y"              , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Z             , 0          , "Z"              , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_R             , 0          , "R"              , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SizeX         , 0          , "SizeX"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SizeY         , 0          , "SizeY"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SizeZ         , 0          , "SizeZ"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SpeedVx       , 0          , "SpeedVx"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SpeedVy       , 0          , "SpeedVy"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SpeedVz       , 0          , "SpeedVz"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SpeedAx       , 0          , "SpeedAx"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SpeedAy       , 0          , "SpeedAy"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SpeedAz       , 0          , "SpeedAz"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_AttachObj     , (ULONG)NULL, "AttachObj"      , SetGet_All    , OOF_MNEW |             OOF_MGET            },
    { D3A_RotCx         , 0          , "RotCx"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotCy         , 0          , "RotCy"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotCz         , 0          , "RotCz"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotIx         , 0          , "RotIx"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotIy         , 0          , "RotIy"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotIz         , 0          , "RotIz"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotAx         , 0          , "RotAx"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotAy         , 0          , "RotAy"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotAz         , 0          , "RotAz"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotSIx        , 0          , "RotSIx"         , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotSIy        , 0          , "RotSIy"         , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotSIz        , 0          , "RotSIz"         , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotSAx        , 0          , "RotSAx"         , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotSAy        , 0          , "RotSAy"         , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_RotSAz        , 0          , "RotSAz"         , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Masse         , 0          , "Masse"          , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Profile       , 0          , "Profile"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_ObjType		, (ULONG)NULL, "ObjType"		, SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_FileName      , (ULONG)NULL, "FileName"       , SetGet_File   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_Name			, (ULONG)NULL, "Name"			, SetGet_File   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_CBFuncSpeed   , (ULONG)NULL, "CBFuncSpeed"    , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Actions       , 0          , "Actions"        , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Sensitivity   , 0          , "Sensitivity"    , SetGet_All    , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Rubbery       , FALSE      , "Rubbery"        , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Collide       , TRUE       , "Collide"        , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Visible       , TRUE       , "Visible"        , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Audible       , FALSE      , "Audible"        , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Sphere        , FALSE      , "Sphere"         , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_CenterAbs     , FALSE      , "CenterAbs"      , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_CenterObj     , FALSE      , "CenterObj"      , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_CenterSelf    , FALSE      , "CenterSelf"     , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SensGravity   , FALSE      , "SensGravity"    , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SensMagnetism , FALSE      , "SensMagnetism"  , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SensElectricity,FALSE      , "SensElectricity", SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SensLuminosity, FALSE      , "SensLuminosity" , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SensCharism   , FALSE      , "SensCharism"    , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_SensMagic     , FALSE      , "SensMagic"      , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Sens_7        , FALSE      , "Sens_7"         , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Sens_8        , FALSE      , "Sens_8"         , SetGet_Flag   , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_SoundFileName , (ULONG)NULL, "SoundFileName"  , NULL          , OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }
    };

OOClass Class_3DSolid = {
    Dispatcher_3DSolid,
    &Class_Root,            // Base class
    "Class_3DSolid",        // Class Name
    0,                      // InstOffset
    sizeof(OOInst_3DSolid), // InstSize  : Taille de l'instance
    attrs,                  // AttrsDesc : Table de description des attributs
    sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)  // AttrsSize : size of instance own taglist
    };
