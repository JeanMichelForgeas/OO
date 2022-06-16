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
#include "oo_sound.h"
#include "oo_gui_prv.h"
#include "oo_3d_prv.h"

#define INDEX_D3A_OnOff         0
#define INDEX_D3A_Directional   1
#define INDEX_D3A_Sensibility   2


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static void AddSoundList( OObject *mikeobj, OOInst_3DMike *inst, OObject **tab, ULONG numobj );


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_3DMike( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_3DMike *inst = OOINST_DATA(cl,obj);
                // Init instance datas
                OOAr_Init( &inst->SoundList, TRUE );
                // Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
            OOInst_3DMike *inst = OOINST_DATA(cl,obj);
            OOAr_Cleanup( &inst->SoundList );
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
            OO_DoBaseMethod( cl, obj, method, attrlist );
            break;

        case D3M_STOP3D:
            OO_DoBaseMethod( cl, obj, method, attrlist );
            break;

        //---------------
        case D3M_OBJTOMIKE:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_ObjSpeaker && attrlist[1].ti_Tag == D3A_ObjTable && attrlist[2].ti_Tag == D3A_ObjCount );
            {
            OOInst_3DMike *inst = OOINST_DATA(cl,obj);
            if (OOINST_ATTR(cl,obj,INDEX_D3A_OnOff) == TRUE)
                AddSoundList( obj, inst, attrlist[1].ti_Ptr, attrlist[2].ti_Data ); // D3A_ObjTable, D3A_ObjCount
            }
            break;

        case D3M_MIXSOUND:
            OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_ObjSpeaker );
            {
            OOInst_3DMike *inst = OOINST_DATA(cl,obj);
            OOResult = (OOINST_ATTR(cl,obj,INDEX_D3A_OnOff) == TRUE) ? (ULONG)&inst->SoundList : (ULONG)NULL;
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

static void AddSoundList( OObject *mikeobj, OOInst_3DMike *inst, OObject **tab, ULONG numobj )
{
    OObject *obj, *sound;
    ULONG i;
    OOInst_3DSolid *sm = D3SOL(mikeobj);
    OOInst_3DSolid *so;
    OO3DPoint vec;
    FLOAT dist2;
    SLONG volume;

    //+++ Voir pour un micro directionnel :
    //      Pyramide de vision genre camera (ZTgPsi), mais qui diminue
    //      le son proportionnellement à l'angle

    OOAr_Init( &inst->SoundList, FALSE ); // Remet à 0 sans désallouer

    for (i=0; i < numobj; i++)
        {
        obj = tab[i];
        so = D3SOL(obj);
        sound = so->Sound;
        if (so->Flags & D3F_SOLID_AUDIBLE && sound != NULL)
            {
            // Comme il faut un objet Sound par speaker, si le son de l'objet est
            //  déjà utilisé il faut le dupliquer
            //if (OO_GetParentOfClass( sound, &Class_HandleSound ) != NULL)
            if (OOGETPARENT(sound) != NULL)
                {
                /*
                OObject *snd;
                if ((snd = OO_New( sound->Class, NULL)) != NULL)
                    if (OO_DoMethod( sound, OOM_COPY, NULL ) == OOV_OK)
                        OO_SetAttr( sound=snd, OOA_Orphan, TRUE );
                */
                }

            // Prend le volume su solide
            OO_GetAttrsV( obj, OOA_SoundVolume, &volume, TAG_END );
            // Si volume trop faible on ne joue pas
            if (volume > -10000) // 0 à -10000 decibels
                {
                // Atténuation en fonction de la distance du micro
                //------ Changement repère : translation
                vec.X = FSUB(so->X, sm->X);
                vec.Y = FSUB(so->Y, sm->Y);
                vec.Z = FSUB(so->Z, sm->Z);

                //------ Distance objet - micro
                dist2 = FADD( FADD(FMUL(vec.X,vec.X), FMUL(vec.Y,vec.Y)), FMUL(vec.Z,vec.Z) );

                //------ Formule d'abaissement du volume en fonction de la distance
                //+++ Prendre en compte la sensibilité du micro
                //+++ Formule bidon juste en attendant de mettre la vraie !!!
                volume = F2LONG(FMUL(dist2,(FLOAT)-8));

                // Encore : si volume trop faible on ne joue pas
                if (volume > -10000)  // 0 à -10000 decibels
                    {
                    //------ Ici l'objet est forcément audible par au moins un micro (et ça ne mange toujours pas de pain)
                    so->Flags |= D3F_SOLID_ISAUDIBLE;

                    //------ Assigne le nouveau volume au son
                    OO_SetAttr( sound, OOA_SoundVolume, volume );

                    //------ Ajout de l'objet dans la liste
                    OOAr_Add( &inst->SoundList, sound, OOV_SOUND_BLOCKSIZE );
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


/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_OnOff      , TRUE , "OnOff"       , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Directional, FALSE, "Directional" , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_Sensibility, 100  , "Sensibility" , NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }
    };

OOClass Class_3DMike = {
    Dispatcher_3DMike,
    &Class_3DSolid,         // Base class
    "Class_3DMike",     // Class Name
    0,                      // InstOffset
    sizeof(OOInst_3DMike),// InstSize  : Taille de l'instance
    attrs,                  // AttrsDesc : Table de description des attributs
    sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)  // AttrsSize : size of instance own taglist
    };
