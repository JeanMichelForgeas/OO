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


//****** Includes ***********************************************

#ifdef AMIGA
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#endif

#include "oo_class.h"
#include "oo_class_prv.h"

#include "oo_gui_prv.h"


//****** Imported ***********************************************


//****** Exported ***********************************************


//****** Statics ************************************************

static void unget_screen( OOInst_AreaScreen *inst );
static ULONG get_screen( OOInst_AreaScreen *inst );
static void get_fonttags( OOInst_AreaScreen *inst );
static ULONG get_screen_sizes( OOInst_AreaScreen *inst );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_AreaScreen( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
				{
				BOOL ok = FALSE;
				OOTagItem *taglist;
				if ((taglist = OOTag_Clone( attrlist )) != NULL)
					{
					OOInst_AreaScreen *inst = OOINST_DATA(cl,retval);
					// Init instance datas
					inst->PosTags[0].ti_Tag = SA_Left;
					inst->PosTags[1].ti_Tag = SA_Top;
					inst->PosTags[2].ti_Tag = SA_Width;
					inst->PosTags[3].ti_Tag = SA_Height;
					inst->PosTags[4].ti_Tag = TAG_IGNORE; //SA_Font
					inst->PosTags[5].ti_Tag = TAG_IGNORE; //SA_Colors
					inst->PosTags[6].ti_Tag = TAG_IGNORE; //SA_Title
					inst->PosTags[7].ti_Tag = TAG_MORE; inst->PosTags[7].ti_Data = (ULONG)taglist;
					// Init default attrs & change defaults with new values
					if (OOPrv_MNew( cl, obj, attrlist ) == OOV_OK)
						ok = TRUE;
					if (inst->Flags & OOF_SCRDEFAULT) 
						OO_SetAttr( obj, OOF_SCRDBLBUF, FALSE );
					}
				if (ok == FALSE)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			return (ULONG)obj;

		case OOM_DELETE:
			{
			OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
			OOTag_Free( (OOTagItem*)inst->PosTags[7].ti_Data );
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
		case OOM_AREA_SETUP: // Obtention du Screen et de _Border et _SubBorder 
			{
			OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
			ULONG rc, passed_flags = attrlist[0].ti_Data; // OOA_Flags
			OORenderInfo *ri = (OORenderInfo*) attrlist[2].ti_Ptr;	// OOA_RenderInfo
			OOInst_Area *ar = _DATA_AREA(obj);

			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (passed_flags & OOF_SAME_RI) break;

			inst->_RI = *ri;
			//......... Remplace par le nouveau OOA_RenderInfo dans attrlist
			attrlist[2].ti_Ptr = &inst->_RI;
			inst->PosTags[0].ti_Data = ar->_B.X; // si pas défaut, alors != 0 
			inst->PosTags[1].ti_Data = ar->_B.Y;
			inst->PosTags[2].ti_Data = ar->AskedW;
			inst->PosTags[3].ti_Data = ar->AskedH;
			get_fonttags( inst );
			rc = OOV_ERROR;
			if (get_screen( inst ) == OOV_OK) // La SuperMethod a besoin que l'écran soit ouvert 
				{
				if (get_screen_sizes( inst ) == OOV_OK)
					{
					if (rc = OO_DoSuperMethodA( cl, obj, msg )) //********************* 
						{
						// pour prendre 0,0 au lieu de _B.X,_B.Y dans AREA_BOX de la SuperCLass 
						ar->Flags = OOF_ANEWRENDERINFO; // élimine FULLW et FULLH 

						ar->_B.X = inst->_RI.sc->LeftEdge;
						ar->_B.Y = inst->_RI.sc->TopEdge;
						ar->_B.W = inst->_RI.sc->Width;
						ar->_B.H = inst->_RI.sc->Height;

						ar->_Border.Top = inst->_RI.sc->BarHeight + 1;
						}
					}
				}
			//......... Restore l'ancien OOA_RenderInfo dans attrlist
			attrlist[2].ti_Ptr = ri;
			return rc;
			}

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if ((passed_flags & (OOF_SAME_RI|OOF_SELF_RI)) == OOF_SAME_RI) 
				{ OOResult = (OORESULT)_DATA_AREA(obj); break; }
			}

			//--- Minimax des objets fils 
			OO_DoBaseMethod( cl, obj, method, attrlist ); // ********************* OOResult = OOInst_Area* 

			//--- Obtention de _MM 
			{
			OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);

			//--- Obtention de _MM 
			ar->_MM.MinW = ar->_MM.MaxW = inst->_RI.sc->Width;
			ar->_MM.MinH = ar->_MM.MaxH = inst->_RI.sc->Height;

			//--- Obtention de _B.W et _B.H 
			ar->_B.X = inst->_RI.sc->LeftEdge;
			ar->_B.Y = inst->_RI.sc->TopEdge;
			ar->_B.W = inst->_RI.sc->Width;
			ar->_B.H = inst->_RI.sc->Height;

			OOResult = (OORESULT) ar;
			break;
			}

		case OOM_AREA_CLEANUP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if ((passed_flags & (OOF_SAME_RI|OOF_SELF_RI)) == OOF_SAME_RI) break;
			}
			OO_DoBaseMethod( cl, obj, method, attrlist );
			unget_screen( OOINST_DATA(cl,obj) );
			break;

		case OOM_AREA_CHANGED:
			OO_DoBaseMethod( cl, obj, method, attrlist );
			// pas de gestion pour l'instant 
			break;

		//---------------
		case OOM_DBWAIT_SAFETOWRITE:
			{
			OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
			if (inst->_RI.sc)
				{
				if (! inst->SafeToWrite)
					{
					while(! GetMsg(inst->Ports[0])) Wait( 1<<(inst->Ports[0]->mp_SigBit) );
					inst->SafeToWrite=TRUE;
					}
				}
			retval = (ULONG)&inst->RPort[inst->CurBuffer];
			break;
			}

		case OOM_DBWAIT_SAFETOCHANGE:
			{
			OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
			if (inst->_RI.sc)
				{
				if (! inst->SafeToChange)
					{
					while(! GetMsg(inst->Ports[1])) Wait( 1<<(inst->Ports[1]->mp_SigBit) );
					inst->SafeToChange=TRUE;
					}
				if (((OOMsg_Data*)msg)->Data) WaitBlit();
				while (! ChangeScreenBuffer( inst->_RI.sc, inst->ScreenBuffer[inst->CurBuffer] ))
					WaitTOF();
				inst->SafeToWrite  = FALSE;
				inst->SafeToChange = FALSE;
				inst->CurBuffer   ^= 1;
				}
			retval = (ULONG)&inst->RPort[inst->CurBuffer];
			break;
			}

		//---------------
		default:
			return OO_DoBaseMethod( cl, obj, method, attrlist );
		}

	return OV_OK;
}

/***************************************************************************
 *
 *		Utilities
 *
 ***************************************************************************/

static ULONG get_screen( OOInst_AreaScreen *inst )
{
	if (inst->Flags & OOF_SCRDEFAULT)		inst->_RI.sc = LockPubScreen( NULL );
	else if (inst->Flags & OOF_SCRPUBLIC)	inst->_RI.sc = LockPubScreen( (UBYTE*)inst->ScrDesc );
	else if (inst->Flags & OOF_SCRFRONT)	inst->_RI.sc = ((struct IntuitionBase *)IntuitionBase)->FirstScreen;
	else if (inst->Flags & OOF_SCRADDRESS)	inst->_RI.sc = (struct Screen *)inst->ScrDesc;
	else									inst->_RI.sc = OpenScreenTagList( NULL, inst->PosTags );

	inst->_RI.di = 0;
	inst->_RI.vi = 0;
	inst->_RI.wi = 0;
	inst->_RI.rq = 0;
	inst->_RI.rp = 0;
	if (inst->_RI.sc)
		{
		if (inst->Flags & OOF_SCRDBLBUF)
			{
			if (!(inst->Ports[0] = CreateMsgPort()) || !(inst->Ports[1] = CreateMsgPort()))
				goto END_ERROR;

			if (!(inst->ScreenBuffer[0] = AllocScreenBuffer( inst->_RI.sc, NULL, SB_SCREEN_BITMAP )))
				goto END_ERROR;
			inst->ScreenBuffer[0]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = inst->Ports[0];
			inst->ScreenBuffer[0]->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = inst->Ports[1];
			InitRastPort( &inst->RPort[0] );
			inst->RPort[0].BitMap = inst->ScreenBuffer[0]->sb_BitMap;

			if (!(inst->ScreenBuffer[1] = AllocScreenBuffer( inst->_RI.sc, NULL, SB_COPY_BITMAP )))
				goto END_ERROR;
			inst->ScreenBuffer[1]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = inst->Ports[0];
			inst->ScreenBuffer[1]->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = inst->Ports[1];
			InitRastPort( &inst->RPort[1] );
			inst->RPort[1].BitMap = inst->ScreenBuffer[1]->sb_BitMap;

			inst->SafeToChange = TRUE;
			inst->SafeToWrite = TRUE;
			inst->CurBuffer = 1;
			}
		if (inst->_RI.di = GetScreenDrawInfo( inst->_RI.sc ))
			{
			inst->_DI = *inst->_RI.di;
			FreeScreenDrawInfo( inst->_RI.sc, inst->_RI.di );
			inst->_RI.di = &inst->_DI;
			if (inst->_RI.vi = GetVisualInfoA( inst->_RI.sc, NULL ))
				{
				inst->_RI.rp = &inst->_RI.sc->RastPort;
				inst->_RI.font = inst->_RI.di->dri_Font;
				_font_to_tta( inst->_RI.di->dri_Font, &inst->_RI._tta );
				return OOV_OK;
				}
			}
		}
  END_ERROR:
	unget_screen( inst );
	return OOV_ERROR;
}

static void unget_screen( OOInst_AreaScreen *inst )
{
	if (inst->_RI.sc)
		{
		if (inst->Flags & OOF_SCRDBLBUF)
			{
			if (inst->Ports[1])
				{
				if (! inst->SafeToChange) while(! GetMsg(inst->Ports[1])) Wait(1l<<(inst->Ports[1]->mp_SigBit));
				DeleteMsgPort( inst->Ports[1] );
				inst->Ports[1] = 0;
				}
			if (inst->Ports[0])
				{
				if (! inst->SafeToWrite) while(! GetMsg(inst->Ports[0])) Wait(1l<<(inst->Ports[0]->mp_SigBit));
				DeleteMsgPort( inst->Ports[0] );
				inst->Ports[0] = 0;
				}
			if (inst->ScreenBuffer[0])
				{
				FreeScreenBuffer( inst->_RI.sc, inst->ScreenBuffer[0] );
				}
			if (inst->ScreenBuffer[1])
				{
				FreeScreenBuffer( inst->_RI.sc, inst->ScreenBuffer[1] );
				}
			inst->ScreenBuffer[0] = 0;
			inst->ScreenBuffer[1] = 0;
			}
		if (inst->_RI.di)
			{
			if (inst->_RI.vi)
				{
				FreeVisualInfo( inst->_RI.vi );
				inst->_RI.vi = 0;
				inst->_RI.rp = 0;
				}
			inst->_RI.di = 0;
			}
		if (inst->Flags & (OOF_SCRDEFAULT|OOF_SCRPUBLIC)) UnlockPubScreen( NULL, inst->_RI.sc );
		else if (inst->Flags & (OOF_SCRFRONT|OOF_SCRADDRESS)) ;
		else CloseScreen( inst->_RI.sc );
		inst->_RI.sc = 0;
		}
}

static void get_fonttags( OOInst_AreaScreen *inst )
{
	if (inst->_RI._tta.tta_Name = (UBYTE *) OOTag_GetTagData( OOA_FontName, 0, inst->PosTags ))
		{
		if (inst->_RI._tta.tta_YSize = OOTag_GetTagData( OOA_FontHeight, 0, inst->PosTags ))
			{
			inst->_RI._tta.tta_Style = 0;
			inst->_RI._tta.tta_Tags = 0;
			inst->_RI._tta.tta_Flags = FPF_DISKFONT;
			inst->PosTags[4].ti_Tag = SA_Font;
			inst->PosTags[4].ti_Data = (ULONG)&inst->_RI._tta;
			}
		}
}

static ULONG get_screen_sizes( OOInst_AreaScreen *inst )
{
  ULONG ok = OOV_ERROR;
  struct Window *w;
  struct Gadget *gad;

	if (w = OpenWindowTags( NULL, WA_Flags,WFLG_BORDERLESS|WFLG_CLOSEGADGET|WFLG_SIZEGADGET|WFLG_DEPTHGADGET,
		WA_Left,inst->_RI.sc->Width-1, WA_Top,inst->_RI.sc->Height-1, WA_Width,1, WA_Height,1, TAG_END ))
		{
		for (gad=w->FirstGadget; gad; gad=gad->NextGadget)
			{
			switch (gad->GadgetType & GTYP_SYSTYPEMASK)
				{
				case GTYP_CLOSE : inst->CloseW = gad->Width; inst->TitleH = gad->Height; break;
				case GTYP_WZOOM : inst->ZoomW  = gad->Width; break;
				case GTYP_WDEPTH: inst->DepthW = gad->Width; break;
				case GTYP_SIZING: inst->SizeW  = gad->Width; inst->SizeH  = gad->Height; break;
				}
			}
		CloseWindow( w );
		ok = OOV_OK;
		}
	return ok;
}


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
	OOInst_AreaScreen *inst = (OOInst_AreaScreen*)sgp->Instance;
	ULONG change = OOV_NOCHANGE;
	ULONG prevflags, flag = 0; title = 0;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		prevflags = inst->Flags;

		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_TitleIndex:
				sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
				if (OOPrv_MSetBounceV( sgp, OOA_Title, OOStr_Get( sgp->ExtAttr->ti_Data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_Title: 
				title = sgp->ExtAttr->ti_Data;
				break;

			case OOA_ColorTable:
				if (sgp->ExtAttr->ti_Ptr != NULL)
					{
					if (inst->_RI.sc) LoadRGB32( &inst->_RI.sc->ViewPort, (ULONG*)sgp->ExtAttr->ti_Ptr );
					if (tmp = OOTag_FindTagItem( SA_Colors32, taglist )) tmp->ti_Data = sgp->ExtAttr->ti_Data;
					else inst->PosTags[5].ti_Tag = SA_Colors32;
					inst->PosTags[5].ti_Data = sgp->ExtAttr->ti_Data; // Dans tous les cas pour Get 
					sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
					change = OOV_CHANGED;
					}
				break;

			case OOA_DefaultScreen	: flag = OOF_SCRDEFAULT ; inst->Flags &= ~OOF_SCRMASK; break;
			case OOA_ScreenAddress	: flag = OOF_SCRADDRESS ; inst->Flags &= ~OOF_SCRMASK; inst->ScrDesc = sgp->ExtAttr->ti_Data; break;
			case OOA_FrontScreen	: flag = OOF_SCRFRONT	; inst->Flags &= ~OOF_SCRMASK; break;
			case OOA_PublicScreen	: flag = OOF_SCRPUBLIC	; inst->Flags &= ~OOF_SCRMASK; inst->ScrDesc = sgp->ExtAttr->ti_Data; break;
			case OOA_DoubleBuffer	: flag = OOF_SCRDBLBUF	; break;
			}

		if (flag)
			{
			if (sgp->ExtAttr->ti_Data == TRUE || sgp->ExtAttr->ti_Ptr != NULL)
				 { change = (prevflags & flag) ? OOV_NOCHANGE : OOV_CHANGED ; inst->Flags |= flag; }
			else { change = (prevflags & flag) ? OOV_CHANGED  : OOV_NOCHANGE; inst->Flags &= ~flag; }
			sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
			}

		if (title)
			{
			inst->PosTags[6].ti_Tag = SA_Title;
			inst->PosTags[6].ti_Data = title;
			if (sgp->Attr->ti_Data != sgp->ExtAttr->ti_Data)
				{
				sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
				change = OOV_CHANGED;
				}
			}
		}
	else{
		ULONG *ptr = (ULONG*)sgp->ExtAttr->ti_Ptr;
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_DefaultScreen: *ptr = (inst->Flags & OOF_SCRDEFAULT) ? TRUE : FALSE; break;
			case OOA_PublicScreen : *ptr = (inst->Flags & OOF_SCRPUBLIC)  ? inst->ScrDesc : 0; break;
			case OOA_FrontScreen  : *ptr = (inst->Flags & OOF_SCRFRONT)	  ? TRUE : FALSE; break;
			case OOA_ScreenAddress: *ptr = (inst->Flags & OOF_SCRADDRESS) ? inst->ScrDesc : 0; break;

			default: *ptr = sgp->Attr->ti_Data; break;
			}
		}
	return change;
}

/***************************************************************************
 *
 *		Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
	{ OOA_DefaultScreen, 0, "DefaultScreen", SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |			OOF_MAREA },
	{ OOA_PublicScreen , 0, "PublicScreen" , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |			OOF_MAREA },
	{ OOA_FrontScreen  , 0, "FrontScreen"  , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |			OOF_MAREA },
	{ OOA_ScreenAddress, 0, "ScreenAddress", SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |			OOF_MAREA },
	{ OOA_DoubleBuffer , 0, "DoubleBuffer" , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |			OOF_MAREA },
	{ OOA_ColorTable   , 0, "ColorTable"   , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOTIFY		  },
	{ OOA_Title 	   , 0, "Title" 	   , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET 					  },
	{ OOA_TitleIndex   , 0, "TitleIndex"   , SetGet_All, OOF_MNEW |	OOF_MSET | OOF_MGET						  },
	{ OOA_FontName	   , 0, "FontName"	   , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |			OOF_MAREA },
	{ OOA_FontHeight   , 0, "FontHeight"   , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |			OOF_MAREA },
	{ TAG_END }
	};

OOClass Class_AreaScreen = { 
	Dispatcher_AreaScreen, 
	&Class_AreaGroup,		// Base class
	"Class_AreaScreen", 	// Class Name
	0,						// InstOffset
	sizeof(OOInst_AreaScreen),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
