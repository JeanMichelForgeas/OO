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

#include "oo_class_prv.h"
#include "oo_gui_prv.h"
#include "oo_3d_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

#ifdef AMIGA
static void StartVBL( OOInst_Handle3D *inst );
static void StopVBL( OOInst_Handle3D *inst );
#endif

static ULONG Handle3DEvent( OOInst_Handle3D *inst, OObject *obj, ULONG method, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_Handle3D( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
				{
				OOInst_Handle3D *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				inst->MaxPermit = 100;	// 2 secondes
				// Init default attrs & change defaults with new values
				if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			return (ULONG)obj;

		case OOM_DELETE:
			//OO_DoMethod( obj, D3M_STOP3D, NULL );
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
			{
			OOInst_Handle3D *inst = OOINST_DATA(cl,obj);
			if (inst->Flags & D3F_H3D_3DSTARTED)
				return Handle3DEvent( inst, obj, method, attrlist );
			break;
			}

		//---------------
		 case D3M_STEP3D:
			{
			OOInst_Handle3D *inst = OOINST_DATA(cl,obj);
			inst->Elapsed = 1;
			return Handle3DEvent( inst, obj, method, attrlist );
			}

		case D3M_START3D:
			{
			struct _box { SWORD x, y; UWORD w, h; } B;
			OOInst_Handle3D *inst = OOINST_DATA(cl,obj);
			ULONG dblbuf=FALSE;
			OORenderInfo *ri=0;

#ifdef AMIGA
			if (inst->RastPort == NULL)
#endif
				{
				if (inst->ScreenAddr != NULL && *inst->ScreenAddr != NULL)
					{
					if (OO_GetAttrsV( *inst->ScreenAddr, OOA_DoubleBuffer, &dblbuf, OOA_WidthHeight, &B.w, OOA_RenderInfo, &ri, TAG_END ) == OOV_OK)
						{
						if (dblbuf == TRUE) inst->Flags |= D3F_H3D_DOUBLEBUF; else inst->Flags &= ~D3F_H3D_DOUBLEBUF;

						if (ri != NULL)
							{
							if (inst->Flags & D3F_H3D_DOUBLEBUF)
								{
#ifdef AMIGA
								if ((inst->MaskBitMap = OO_AllocBMap( 1, ri->sc->Width, ri->sc->Height, 0 )) != NULL)
									{
									inst->AreaMask = inst->MaskBitMap->Planes[0];
									inst->AreaMaskSize = inst->MaskBitMap->BytesPerRow * inst->MaskBitMap->Rows;
									inst->RastPort = &ri->sc->RastPort;
									}
#endif
								}
							else{
#ifdef AMIGA
								if ((inst->RastPort = OO_AllocBMapRPort( ri->sc->RastPort.BitMap->Depth, ri->sc->Width, ri->sc->Height, OOBMAP_CONTIG|OOBMAP_MASKED )) != NULL)
									{
									inst->AreaMask = BM2OO(inst->RastPort->BitMap)->Mask;
									inst->AreaMaskSize = inst->RastPort->BitMap->BytesPerRow * inst->RastPort->BitMap->Rows;
									}
#endif
#ifdef _WIN32
								/*
								if ((inst->RastPort = CreateCompatibleDC( ri->rp )) != NULL)
									{
									if ((inst->BitMap = CreateCompatibleBitmap( ri->rp, (int)B.w, (int)B.h )) != NULL)
										inst->OldBitMap = SelectObject( inst->RastPort, inst->BitMap );
									else { DeleteDC( inst->RastPort ); inst->RastPort = NULL; }
									}
								*/
#endif
								}
							if (inst->RastPort != NULL)
								{
#ifdef AMIGA
								InitArea( &inst->AreaInfo, inst->AreaBuffer, AREANUMVECS );
								InitTmpRas( &inst->TmpRas, inst->AreaMask, inst->AreaMaskSize );
								inst->RastPort->AreaInfo = &inst->AreaInfo;
								inst->RastPort->TmpRas = &inst->TmpRas;
#endif
#ifdef _WIN32
#endif
								}
							}
						}
					}
				}

#ifdef AMIGA
			if (inst->RastPort == NULL)
				return OOV_ERROR;
#endif
			OO_SetAttr( obj, OOA_OnOff, TRUE );	// Set flag 3DSTARTED et envoie methode SETEVENT, notifie
			OO_DoChildrenMethodV( 0, obj, D3M_START3D, D3A_Handle3D, obj, D3A_ObjSpace, NULL, TAG_END );
#ifdef AMIGA
			StartVBL( inst );
#endif
#ifdef _WIN32
			inst->LastTime = GetTickCount();
#endif
			inst->Elapsed = 0;
			break;
			}

		case D3M_STOP3D:
			{
			OOInst_Handle3D *inst = OOINST_DATA(cl,obj);
#ifdef AMIGA
			StopVBL( inst );
#endif
			OO_DoChildrenMethod( 0, obj, D3M_STOP3D, NULL );
			OO_SetAttr( obj, OOA_OnOff, FALSE ); // Clear flag 3DSTARTED et envoie methode CLEAREVENT, notifie

#ifdef AMIGA
			if (inst->RastPort != NULL)
#endif
				{
#ifdef _WIN32
				inst->AddElapsed /= 20;
#endif // _WIN32
				if (inst->AddElapsed && inst->NumLoops)
					{
					UBYTE buf[100];
					sprintf( buf, "Frame/sec Average = %g, MaxVBL = %d\n", ((FLOAT)inst->NumLoops)/(FLOAT)((FLOAT)(inst->AddElapsed-inst->MaxElapsed)/(FLOAT)50), inst->MaxElapsed );
#ifdef _DEBUG
					OOTRACE( buf );
#else
#ifdef _WIN32
					MessageBox( NULL, buf, NULL, 0 );
#endif // _WIN32
#endif // _DEBUG
					}
#ifdef AMIGA
				WaitBlit();
				inst->RastPort->AreaInfo = 0;
				inst->RastPort->TmpRas = 0;
#endif
				if (inst->Flags & D3F_H3D_DOUBLEBUF)
					{
#ifdef AMIGA
					OO_FreeBMap( inst->MaskBitMap );
#endif
					}
				else{
#ifdef AMIGA
					OO_FreeBMapRPort( inst->RastPort );
#endif
#ifdef _WIN32
					/*
					if (inst->BitMap != NULL)
						{
						SelectObject( inst->RastPort, inst->OldBitMap );
						DeleteObject( inst->BitMap );
						inst->BitMap = NULL;
						}
					DeleteDC( inst->RastPort );
					*/
#endif
					}
				inst->RastPort = NULL;
				}
			break;
			}

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

static ULONG Handle3DEvent( OOInst_Handle3D *inst, OObject *obj, ULONG method, OOTagList *attrlist )
{
	ULONG elapsedtime;

#ifdef _WIN32
	ULONG ticks = GetTickCount();
	ULONG diff = ticks - inst->LastTime;
	if (inst->Flags & D3F_H3D_WAITVBLANK)
		{
		while (diff < 20)
			diff = (ticks = GetTickCount()) - inst->LastTime;
		}
	inst->Elapsed = diff / 20; // -> 1/50 sec
	if (inst->Elapsed != 0) 
		{
		inst->AddElapsed += diff;
		inst->LastTime = ticks;
		}
	// printf( "diff=%d, add=%d, elapsed=%d\n", diff, inst->AddElapsed, inst->Elapsed );
#endif
#ifdef AMIGA
	inst->AddElapsed += inst->Elapsed;
#endif
	elapsedtime = inst->Elapsed; inst->Elapsed = 0;
	if (elapsedtime > inst->MaxPermit) 
		{ 
		OOTRACE( "+++ elapsedtime %d > MaxPermit %d\n", elapsedtime, inst->MaxPermit );
		elapsedtime = inst->MaxPermit;
		}
	if (elapsedtime > inst->MaxElapsed) inst->MaxElapsed = elapsedtime;
	if (inst->TimeOffset) elapsedtime = inst->TimeOffset;

	if (!inst->ScreenAddr || !(*inst->ScreenAddr))
		{
#ifdef AMIGA
		if (inst->RastPort != NULL)
			{ OO_DoMethod( obj, D3M_STOP3D, NULL ); return OOV_ERROR; }
#endif
		}
	else // le RastPort existe forcément 
		{
		if (inst->Flags & D3F_H3D_DOUBLEBUF)
			{
#ifdef AMIGA
			OORastPort *rp = (OOHRastPort) OO_DoMethod( *inst->ScreenAddr, OOM_DBWAIT_SAFETOWRITE, 0 );
			rp->AreaInfo = &inst->AreaInfo;
			rp->TmpRas = &inst->TmpRas;

			//--- Effacement des modifs 
			if (inst->Flags & D3F_H3D_SCRCLEAR)
				SetRast( rp, 0 );
												
			//--- Calculs (mouvements et représentations) 
			OO_DoChildrenMethodV( 0, obj, D3M_MOVEALL, D3A_Handle3D, obj, D3A_ElapsedTime, elapsedtime, TAG_END );
			OO_DoChildrenMethodV( 0, obj, D3M_OBJTOVIEW, D3A_RastPort, rp, D3A_ObjTable, NULL, D3A_ObjCount, 0, TAG_END );

			//--- Dessin des caméras, sons des micros 
			OO_DoChildrenMethod( 0, obj, D3M_REFRESHVIEW, NULL );

			OO_DoMethodV( *inst->ScreenAddr, OOM_DBWAIT_SAFETOCHANGE, 0, 1 ); // 1 pour WaitBlit() 
#endif
			}
		else{
			//--- Effacement des modifs 
			if (inst->Flags & D3F_H3D_SCRCLEAR)
				{
#ifdef AMIGA
				struct BitMap *dbm = inst->RastPort->BitMap; // Ici la bitmap destination est aussi une OOBitMap 
				BltClear( dbm->Planes[0], inst->AreaMaskSize * dbm->Depth, 0 );
#endif
#ifdef _WIN32
				OO_DoChildrenMethodV( 0, obj, D3M_CLEARVIEW, D3A_RastPort, inst->RastPort, TAG_END );
#endif
				}
			//--- Calculs (mouvements et représentations) 
			OO_DoChildrenMethodV( 0, obj, D3M_MOVEALL, D3A_Handle3D, obj, D3A_ElapsedTime, elapsedtime, TAG_END );
			OO_DoChildrenMethodV( 0, obj, D3M_OBJTOVIEW, D3A_RastPort, inst->RastPort, D3A_ObjTable, NULL, D3A_ObjCount, 0, TAG_END );

			//--- Dessin des caméras, sons des micros 
			OO_DoChildrenMethodV( 0, obj, D3M_REFRESHVIEW, D3A_RastPort, inst->RastPort, TAG_END );

			OO_DoChildrenMethodV( 0, obj, D3M_SYNCVIEW, D3A_RastPort, inst->RastPort, TAG_END );
			}
		}

	inst->NumLoops++;
	OO_DoClassMethodV( &Class_Application, NULL, OOM_SETEVENT, OOA_EventMask, OO_GetAttr( obj, OOA_EventMask ), TAG_END );
	return OOV_OK;
}

#ifdef AMIGA
static ULONG __asm VBL_Routine( register __a1 OOInst_Handle3D *inst )
{
	inst->Elapsed++;
	return 0;
}
static void StartVBL( OOInst_Handle3D *inst )
{
	if (inst->Flags & D3F_H3D_VBLON) return;
	inst->IVBL.is_Node.ln_Pri = -10;
	inst->IVBL.is_Node.ln_Name = "Handle3D_VBL";
	inst->IVBL.is_Data = (APTR)inst;
	inst->IVBL.is_Code = (APTR)VBL_Routine;

	AddIntServer( (LONG)INTB_VERTB, &inst->IVBL );
	inst->Flags |= D3F_H3D_VBLON;
}
static void StopVBL( OOInst_Handle3D *inst )
{
	if (inst->Flags & D3F_H3D_VBLON) RemIntServer( (LONG)INTB_VERTB, &inst->IVBL );
	inst->Flags &= ~D3F_H3D_VBLON;
}
#endif


/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

// If a SetGet function is given to the attribute, the function has perhaps to change
//	the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same of the 
//	current attr value.

static ULONG SetGet_All( OOSetGetParms *sgp )
{
	OOInst_Handle3D *inst = (OOInst_Handle3D*)sgp->Instance;
	ULONG change = OOV_NOCHANGE;
	ULONG flag = 0;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (sgp->Attr->ti_Data != data) change = OOV_CHANGED;

		switch (sgp->ExtAttr->ti_Tag)
			{
			case D3A_MaxTime   : inst->MaxPermit  = data; break;
			case D3A_TimeOffset: inst->TimeOffset = data; break;
			case D3A_ScreenAddr: inst->ScreenAddr = (APTR)data; break;

			case OOA_OnOff		: flag = D3F_H3D_3DSTARTED	; break;
			case D3A_ClearScreen: flag = D3F_H3D_SCRCLEAR	; break;
			case D3A_WaitVBlank	: flag = D3F_H3D_WAITVBLANK	; break;

			default: return OOV_NOCHANGE;
			}
		sgp->Attr->ti_Data = data;

		if (flag)
			{
			if (data == TRUE)
				 { change = (inst->Flags & flag) ? OOV_NOCHANGE : OOV_CHANGED ; inst->Flags |= flag; }
			else { change = (inst->Flags & flag) ? OOV_CHANGED	: OOV_NOCHANGE; inst->Flags &= ~flag; }

			if (sgp->ExtAttr->ti_Tag == OOA_OnOff && change == OOV_CHANGED)
				{
				ULONG eventmask = OO_GetAttr( sgp->Obj, OOA_EventMask );
				ULONG method = (data == TRUE) ? OOM_SETEVENT : OOM_CLEAREVENT;
				OO_DoClassMethodV( &Class_Application, NULL, method, OOA_EventMask, eventmask, TAG_END );
				inst->Elapsed = (data == TRUE) ? 1 : 0;
				}
			}
		}
	else{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case D3A_MaxTime   : *(ULONG*)data = inst->MaxPermit ; break;
			case D3A_TimeOffset: *(ULONG*)data = inst->TimeOffset; break;
			case D3A_ScreenAddr: *(ULONG*)data = (ULONG)inst->ScreenAddr; break;

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
    { OOA_OnOff 		, FALSE			, "OnOff"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_MaxTime 		, 0				, "MaxTime"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_TimeOffset	, 0				, "TimeOffset"	, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_ScreenAddr	, (ULONG)NULL	, "ScreenAddr"	, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_ClearScreen	, FALSE			, "ClearScreen"	, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_WaitVBlank	, FALSE			, "WaitVBlank"	, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET			},
    { TAG_END }
	};

OOClass Class_Handle3D = { 
	Dispatcher_Handle3D, 
	&Class_SysEvent,		// Base class
	"Class_Handle3D",		// Class Name
	0,						// InstOffset
	sizeof(OOInst_Handle3D),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
