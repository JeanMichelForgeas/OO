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

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_text.h"

#include "oo_gui_prv.h"


//****** Imported ***********************************************


//****** Exported ***********************************************


//****** Statics ************************************************

static void unget_screen( OOInst_AreaScreen *inst );
static ULONG get_screen( OOInst_AreaScreen *inst, OOTagList *attrs );
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
				OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				inst->Flags |= OOF_SCRDEFAULT;
				OOGfx_InitRPort( &inst->_PlainRP );
				// Init default attrs & change defaults with new values
				if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); return (ULONG)NULL; }
				if ((inst->Flags & (OOF_SCRDEFAULT|OOF_SCRDBLBUF)) == (OOF_SCRDEFAULT|OOF_SCRDBLBUF))
					OO_SetAttr( obj, OOA_DoubleBuffer, FALSE );
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
		case OOM_AREA_SETUP: // Obtention du Screen et de _Border et _SubBorder 
			{
			OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
			OOTagList *scrattrs = OOINST_ATTRS(cl,obj);
			ULONG rc, passed_flags = attrlist[0].ti_Data; // OOA_Flags
			OORenderInfo *ri = (OORenderInfo*) attrlist[2].ti_Ptr;	// OOA_RenderInfo
			OOInst_Area *ar = _DATA_AREA(obj);

			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (passed_flags & OOF_SAME_RI) break;

			inst->_RI = *ri;
			inst->_PlainRP.Palette = ri->Palette;
			//......... Remplace par le nouveau OOA_RenderInfo dans attrlist
			attrlist[2].ti_Ptr = &inst->_RI;
			rc = OOV_ERROR;
			if (get_screen( inst, scrattrs ) == OOV_OK) // La SuperMethod a besoin que l'écran soit ouvert 
				{
				if (rc = OO_DoBaseMethod( cl, obj, method, attrlist )) //********************* 
					{
					RECT rect;
					SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );

					// pour prendre 0,0 au lieu de _B.X,_B.Y dans AREA_BOX de la SuperCLass 
					ar->Flags = OOF_ANEWRENDERINFO; // élimine OOF_AINSIDE_FULLW et OOF_AINSIDE_FULLH 

					ar->_B.X = (SWORD)rect.left;
					ar->_B.Y = (SWORD)rect.top;
					ar->_B.W = (SWORD)(rect.right - rect.left);
					ar->_B.H = (SWORD)(rect.bottom - rect.top);
					if (ar->AskedW == 0 && ar->AskedH == 0)
						{ // Fait persiter la taille de l'écran
						ar->AskedW = ar->_B.W;
						ar->AskedH = ar->_B.H;
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

			{
			OOInst_AreaScreen *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);

			//--- Obtention de _MM 
			ar->_MM.MinW = ar->_MM.MaxW = ar->_B.W; 
			ar->_MM.MinH = ar->_MM.MaxH = ar->_B.H;

			//--- Obtention de _B.W et _B.H 

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
		/*
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
		*/

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

static ULONG get_screen( OOInst_AreaScreen *inst, OOTagList *attrs )
{
	OOFont *font;

	if (inst->Flags & OOF_SCRDEFAULT)		inst->_RI.sc = GetDesktopWindow();
/*
	else if (inst->Flags & OOF_SCRPUBLIC)	inst->_RI.sc = LockPubScreen( (UBYTE*)inst->ScrDesc );
	else if (inst->Flags & OOF_SCRFRONT)	inst->_RI.sc = ((struct IntuitionBase *)IntuitionBase)->FirstScreen;
	else if (inst->Flags & OOF_SCRADDRESS)	inst->_RI.sc = (struct Screen *)inst->ScrDesc;
	else									inst->_RI.sc = OpenScreenTagList( NULL, inst->PosTags );
*/

	if (inst->_RI.sc != NULL)
		{
		/*
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
		*/
				if ((font = OOGfx_OpenFont( inst->_RI.Font, attrs )) != NULL)
					{
					if (font != inst->_RI.Font) inst->Flags |= OOF_SCRALLOCFONT;
					if ((inst->_PlainRP.DC = GetDC( inst->_RI.sc )) != NULL)
						{
						inst->_RI.rp = &inst->_PlainRP;
						inst->_RI.Font = font;
						OOGfx_FontToTagList( inst->_RI.Font, attrs );
						//inst->_RI.OldFont = SelectObject( inst->_PlainRP.DC, font );
						//ReleaseDC( inst->_RI.sc, inst->_RI.rp );
						return OOV_OK;
						}
					}
		/*
				}
			}
		*/
		}
	unget_screen( inst );
	return OOV_ERROR;
}

static void unget_screen( OOInst_AreaScreen *inst )
{
	if (inst->_RI.sc != NULL)
		{
		/*
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
		if (inst->Flags & (OOF_SCRPUBLIC)) UnlockPubScreen( NULL, inst->_RI.sc );
		else if (inst->Flags & (OOF_SCRFRONT|OOF_SCRADDRESS)) ;
		else CloseScreen( inst->_RI.sc );
		*/
		if (inst->_PlainRP.DC != NULL)
			{
			//SelectObject( inst->_PlainRP.DC, inst->_RI.OldFont );
			ReleaseDC( inst->_RI.sc, inst->_PlainRP.DC );
			inst->_PlainRP.DC = NULL;
			}
		if (inst->Flags & OOF_SCRALLOCFONT) 
			OOGfx_CloseFont( inst->_RI.Font );
		inst->Flags &= ~OOF_SCRALLOCFONT;
		inst->_RI.Font = NULL;
		inst->_RI.sc = NULL;
		}
}

static ULONG get_screen_sizes( OOInst_AreaScreen *inst )
{
	return OOV_OK;
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
	ULONG prevflags, flag = 0, title = 0;

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
			/*
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
			*/
			case OOA_DefaultScreen	: flag = OOF_SCRDEFAULT ; inst->Flags &= ~OOF_SCRMASK; break;
			case OOA_ScreenAddress	: flag = OOF_SCRADDRESS ; inst->Flags &= ~OOF_SCRMASK; inst->ScrDesc = sgp->ExtAttr->ti_Data; break;
			case OOA_FrontScreen	: flag = OOF_SCRFRONT	; inst->Flags &= ~OOF_SCRMASK; break;
			case OOA_PublicScreen	: flag = OOF_SCRPUBLIC	; inst->Flags &= ~OOF_SCRMASK; inst->ScrDesc = sgp->ExtAttr->ti_Data; break;
			case OOA_DoubleBuffer	: flag = OOF_SCRDBLBUF	; break;

			case OOA_FontName: if (OOTag_SetString( sgp->Attr, sgp->ExtAttr->ti_String ) != NULL) change = OOV_CHANGED; break;
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
	{ OOA_DefaultScreen	, TRUE			, "DefaultScreen" , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_PublicScreen	, (ULONG)NULL	, "PublicScreen"  , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FrontScreen	, FALSE			, "FrontScreen"   , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_ScreenAddress	, (ULONG)NULL	, "ScreenAddress" , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_DoubleBuffer	, FALSE			, "DoubleBuffer"  , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_ColorTable	, (ULONG)NULL	, "ColorTable"    , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT		  },
	{ OOA_Title 		, (ULONG)NULL	, "Title" 		  , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET					  },
	{ OOA_TitleIndex    , 0				, "TitleIndex"    , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET					  },
	{ OOA_FontName		, (ULONG)NULL	, "FontName"	  , SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontHeight	, 0				, "FontHeight"    , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontHeightP100, 100			, "FontHeightP100", NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontBold		, FALSE			, "FontBold"	  , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontItalic	, FALSE			, "FontItalic"    , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontUnderline	, FALSE			, "FontUnderline" , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontFixed		, FALSE			, "FontFixed"     , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontVariable	, TRUE			, "FontVariable"  , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ TAG_END }
	};

OOClass Class_AreaScreen = { 
	Dispatcher_AreaScreen, 
	&Class_AreaGroup,			// Base class
	"Class_AreaScreen", 		// Class Name
	0,							// InstOffset
	sizeof(OOInst_AreaScreen),	// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
