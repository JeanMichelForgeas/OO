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

#include "oo_gui_prv.h"

#include <clib/keymap_protos.h>
#include <graphics/gfxmacros.h>

#define OOF_WIN_FILLED		0x0001
#define OOF_WIN_BADSIZE 	0x0002
#define OOF_WIN_OVERFLOW	0x0004
#define OOF_WIN_OVFLAST 	0x0008
#define OOF_WIN_SVERIFY 	0x0010
#define OOF_WIN_ALLOCFONT	0x0020
#define OOF_WIN_BORDERLESS	0x0040
#define OOF_WIN_SYNC		0x0080
#define OOF_WIN_SYNCREQUEST 0x0100
#define OOF_WIN_CENTERED	0x0200

#define OOF_WCTL_CLOSE		0x0100
#define OOF_WCTL_ICON		0x0200
#define OOF_WCTL_ZOOM		0x0400
#define OOF_WCTL_DEPTH		0x0800
#define OOF_WCTL_DRAG		0x1000
#define OOF_WCTL_SIZE		0x2000
#define OOF_WCTL_INSRIGHT	0x4000
#define OOF_WCTL_INSBOT 	0x8000

#define IDCMPS (IDCMP_REFRESHWINDOW|INTUITICKS|CLOSEWINDOW|ACTIVEWINDOW|MOUSEMOVE|\
INACTIVEWINDOW|GADGETDOWN|GADGETUP|REQSET|REQCLEAR|RAWKEY|DISKREMOVED|DISKINSERTED|IDCMP_MENUPICK|\
/*IDCMP_MENUVERIFY|*/IDCMP_MENUHELP|IDCMP_CHANGEWINDOW|IDCMP_IDCMPUPDATE|IDCMP_GADGETHELP|MOUSEBUTTONS|\
IDCMP_REQVERIFY|IDCMP_NEWPREFS|IDCMP_SIZEVERIFY|IDCMP_NEWSIZE)


//****** Imported ***********************************************


//****** Exported ***********************************************


//****** Statics ************************************************

static OOTagItem __far wflags_boolmap[] =
	{
	{ OOA_WinCtlSize	, WFLG_SIZEGADGET  },
	{ OOA_WinCtlDrag	, WFLG_DRAGBAR	   },
	{ OOA_WinCtlDepth	, WFLG_DEPTHGADGET },
	{ OOA_WinCtlClose	, WFLG_CLOSEGADGET },
	{ OOA_WinCtlZoom	, WFLG_HASZOOM	   },
	{ OOA_WinCtlInsRight, WFLG_SIZEBRIGHT  },
	{ OOA_WinCtlInsBot	, WFLG_SIZEBBOTTOM },
	{ OOA_Borderless	, WFLG_BORDERLESS  },
	{ TAG_DONE }
	};

static UWORD chip NullPointer[(1 * 2) + 4] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, };

static void close_req( OObject *wo, OOInst_AreaWindow *inst )
static ULONG blockwin( OObject *obj, struct _parm *parm )
static void block_other_wins( OObject *wo, OOInst_AreaWindow *inst, ULONG block )
static void reply_gtmsg( OOInst_AreaWindow *inst )
static void strip_idcmp( struct Window *w, ULONG idcmpmask )
static void close_window_safely( struct Window *w )
static void unget_window( OObject *obj, OOInst_AreaWindow *inst )
static ULONG get_window( OObject *obj, OObject *gui, OOInst_AreaWindow *inst )
static void DrawFill( OOInst_AreaWindow *inst, OOInst_Area *ar )
static void WindowSizeVerify( OObject *obj, OOInst_AreaWindow *inst )
static void WindowNewSize( OObject *obj, OOInst_AreaWindow *inst )
static void ChangeWindow( OObject *obj, OOInst_AreaWindow *inst )
static void HandleWindowMsg( OOClass *cl, OObject *obj, OOTagList *attrlist );
static void install_ptr( OOInst_AreaWindow *inst, ULONG pointer )


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_AreaWindow( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
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
					OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
					// Init instance datas
					inst->PosTags[0].ti_Tag = WA_Left;
					inst->PosTags[1].ti_Tag = WA_Top;
					inst->PosTags[2].ti_Tag = WA_Width;
					inst->PosTags[3].ti_Tag = WA_Height;
					inst->PosTags[4].ti_Tag = WA_MinWidth;
					inst->PosTags[5].ti_Tag = WA_MinHeight;
					inst->PosTags[6].ti_Tag = WA_MaxWidth;
					inst->PosTags[7].ti_Tag = WA_MaxHeight;
					inst->PosTags[8].ti_Tag = WA_CustomScreen;
					inst->PosTags[9].ti_Tag = TAG_IGNORE;
					inst->PosTags[10].ti_Tag = WA_Title;
					inst->PosTags[11].ti_Tag = WA_Flags;
					inst->PosTags[12].ti_Tag = TAG_MORE; inst->PosTags[12].ti_Data = (ULONG)taglist;
					inst->Flags = OOF_WIN_FILLED;
					inst->CurrentPtr = inst->Installed = OOV_PTR_SYS;
					if (OOTag_FindTag( attrlist, OOA_Margins ) == NULL) _DATA_AREAGROUP(obj)->Flags |= OOF_GRP_MARGINS;
					if (OOTag_FindTag( attrlist, OOA_LeftTop ) == NULL) inst->Flags |= OOF_WIN_CENTERED;
					// Init default attrs & change defaults with new values
					if (OOPrv_MNew( cl, obj, attrlist ) == OOV_OK)
						ok = TRUE;
					}
				if (ok == FALSE)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			return (ULONG)obj;

		case OOM_DELETE:
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			OOTag_Free( (OOTagItem*)inst->PosTags[12].ti_Data );
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
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			ULONG rc, passed_flags = attrlist[0].ti_Data; // OOA_Flags
			OORenderInfo *ri = (OORenderInfo*) attrlist[2].ti_Ptr;	// OOA_RenderInfo

			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (passed_flags & OOF_SAME_RI) break;

			inst->_RI.sc   = ri->sc;
			inst->_RI.di   = ri->di;
			inst->_RI.vi   = ri->vi;
			inst->_RI.font = ri->font;
			inst->_RI._tta = ri->_tta;
			if (_open_font_taglist( inst->PosTags, &inst->_RI._tta, (APTR*)&inst->_RI.font ))
				inst->Flags |= OOF_WIN_ALLOCFONT;

			//......... Remplace par le nouveau OOA_RenderInfo dans attrlist
			attrlist[2].ti_Ptr = &inst->_RI;
			if ((rc = OO_DoBaseMethod( cl, obj, method, attrlist )) == OOV_OK) //**********************
				{
				OOInst_Area *ar = _DATA_AREA(obj);
				OOInst_AreaScreen *scid;
				OObject *par = OO_GetParentOfClass( obj, &Class_AreaScreen );
				scid = OINST_DATA(&Class_AreaScreen,par);

				// pour prendre 0,0 au lieu de _B.X,_B.Y dans AREA_BOX de la SuperCLass 
				ar->Flags = OOF_ANEWRENDERINFO; // élimine FULLW et FULLH 

				//--- Obtention de _Border 
				inst->PosTags[11].ti_Data = OOTag_PackBool( inst->PosTags, wflags_boolmap, WFLG_ACTIVATE );
				if (inst->Flags & OOF_WCTL_SIZE)
					{
					if (inst->Flags & OOF_WCTL_DEPTH) inst->Flags |= OOF_WCTL_ZOOM;
					if (! (inst->Flags & (OOF_WCTL_INSRIGHT | OOF_WCTL_INSBOT))) inst->Flags |= OOF_WCTL_INSRIGHT; // default 
					}

				if (! (inst->Flags & OOF_WIN_BORDERLESS))
					{
					ar->_Border.Left   = inst->_RI.sc->WBorLeft;
					ar->_Border.Right  = inst->_RI.sc->WBorRight;
					ar->_Border.Top    = inst->_RI.sc->WBorTop;
					ar->_Border.Bottom = inst->_RI.sc->WBorBottom;
					}

				// dimension du bord haut 
				if ((inst->Flags & (OOF_WCTL_CLOSE | OOF_WCTL_ZOOM | OOF_WCTL_DEPTH | OOF_WCTL_DRAG))
					|| OOTag_GetData( inst->PosTags, WA_Title, 0 ))
					ar->_Border.Top = scid->TitleH;

				// dimension du bord droit 
				if ((inst->Flags & (OOF_WCTL_SIZE | OOF_WCTL_INSRIGHT)) == (OOF_WCTL_SIZE | OOF_WCTL_INSRIGHT))
					ar->_Border.Right = scid->SizeW;

				// dimension du bord bas 
				if ((inst->Flags & (OOF_WCTL_SIZE | OOF_WCTL_INSBOT)) == (OOF_WCTL_SIZE | OOF_WCTL_INSBOT))
					ar->_Border.Bottom = scid->SizeH;

				//--- Obtention de _SubBorder 
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
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);
			struct Screen *sc = inst->_RI.sc;
			OObject *par;
			OOInst_AreaScreen *scid;
			short minw, minh;

			par = OO_GetParentOfClass( obj, &Class_AreaScreen );
			scid = OINST_DATA(&Class_AreaScreen,par);

			//--- Largeur minimum 
			minw = 0;
			if (inst->Flags & OOF_WCTL_CLOSE) minw += scid->CloseW - 1; // -1 car chevauchement 
			if (inst->Flags & OOF_WCTL_ZOOM ) minw += scid->ZoomW  - 1;
			if (inst->Flags & OOF_WCTL_DEPTH) minw += scid->DepthW - 1;
			if (ar->_MM.MinW < minw) ar->_MM.MinW = minw;

			minw = ar->_Border.Left + ar->_Border.Right;
			if (ar->_MM.MinW < minw) ar->_MM.MinW = minw;

			if (inst->Flags & OOF_WCTL_SIZE)
				{
				if (inst->Flags & (OOF_WCTL_CLOSE|OOF_WCTL_ZOOM|OOF_WCTL_DEPTH|OOF_WCTL_DRAG))
					minw = ar->_Border.Left + scid->SizeW;
				else minw = scid->SizeW;
				if (ar->_MM.MinW < minw) ar->_MM.MinW = minw;
				}

			//--- Hauteur minimum 
			minh = ar->_Border.Top + ar->_Border.Bottom;
			if (ar->_MM.MinH < minh) ar->_MM.MinH = minh;

			if (inst->Flags & OOF_WCTL_SIZE)
				{
				if (inst->Flags & (OOF_WCTL_CLOSE|OOF_WCTL_ZOOM|OOF_WCTL_DEPTH|OOF_WCTL_DRAG))
					minh = ar->_Border.Top + scid->SizeH;
				else minh = scid->SizeH;
				if (ar->_MM.MinH < minh) ar->_MM.MinH = minh;
				}

			//--- On repousse les maxi car on a touché les mini 
			if (ar->_MM.MaxW < ar->_MM.MinW) ar->_MM.MaxW = ar->_MM.MinW;
			if (ar->_MM.MaxH < ar->_MM.MinH) ar->_MM.MaxH = ar->_MM.MinH;
			if (ar->_MM.MaxW > sc->Width) ar->_MM.MaxW = sc->Width;
			if (ar->_MM.MaxH > sc->Height) ar->_MM.MaxH = sc->Height;
			if (ar->_MM.MinW > ar->_MM.MaxW) { ar->_MM.MinW = ar->_MM.MaxW; inst->Flags |= OOF_WIN_OVERFLOW; }
			if (ar->_MM.MinH > ar->_MM.MaxH) { ar->_MM.MinH = ar->_MM.MaxH; inst->Flags |= OOF_WIN_OVERFLOW; }

			//--- Obtention de _B.W et _B.H 
			if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
			if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;
			if (ar->_B.W > ar->_MM.MaxW) ar->_B.W = ar->_MM.MaxW;
			if (ar->_B.H > ar->_MM.MaxH) ar->_B.H = ar->_MM.MaxH;

			OOResult = (OORESULT) ar;
			}
			break;

		case OOM_AREA_BOX: // Accorde _B et _SubB avec _MM 
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if (passed_flags & OOF_SAME_RI) { OOResult = (OORESULT)_DATA_AREA(obj); break; }
			}
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			struct Screen *sc = inst->_RI.sc;
			OOInst_Area *ar = _DATA_AREA(obj);

			//--- On centre la fenêtre dans une area 
			if (inst->Flags & OOF_WIN_CENTERED)
				{
				OObject *par;
				OOInst_Area *ref;
				short w, h;
			    for (par=obj->ParentsTable[0]; par->ParentsTable != NULL; par=par->ParentsTable[0])
					{ if (par->Class == &Class_AreaWindow || par->Class == &Class_AreaScreen) break; }
				ref = _DATA_AREA(par);
				w = ref->_B.W - (ref->_Border.Left + ref->_Border.Right);
				ar->_B.X = ref->_Border.Left + (w - ar->_B.W) / 2;
				h = ref->_B.H - (ref->_Border.Top + ref->_Border.Bottom);
				ar->_B.Y = ref->_Border.Top + (h - ar->_B.H) / 2;
				if (par->Class == &Class_AreaWindow) 
					{ ar->_B.X += ref->_B.X; ar->_B.Y += ref->_B.Y; }
				}

			//--- On vérifie que la fenêtre est bien dans l'écran 
			if (ar->_B.X) { if (ar->_B.X < 0) ar->_B.X = 0; if (ar->_B.X + ar->_B.W > sc->Width) ar->_B.X = sc->Width - ar->_B.W; }
			if (ar->_B.Y) { if (ar->_B.Y < 0) ar->_B.Y = 0; if (ar->_B.Y + ar->_B.H > sc->Height) ar->_B.Y = sc->Height - ar->_B.H; }
			}
			//--- On arrange l'areagroup dans lim 
			return OO_DoBaseMethod( cl, obj, method, attrlist ); // ********************* 

		case OOM_AREA_START:
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if (passed_flags & OOF_SAME_RI) break;
			}
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);
			ULONG rc = OOV_ERROR;

			inst->PosTags[0].ti_Data = ar->_B.X;
			inst->PosTags[1].ti_Data = ar->_B.Y;
			inst->PosTags[2].ti_Data = ar->_B.W;
			inst->PosTags[3].ti_Data = ar->_B.H;
			inst->PosTags[4].ti_Data = ar->_MM.MinW;
			inst->PosTags[5].ti_Data = ar->_MM.MinH;
			inst->PosTags[6].ti_Data = ar->_MM.MaxW == MAXWORD ? inst->_RI.sc->Width : ar->_MM.MaxW;
			inst->PosTags[7].ti_Data = ar->_MM.MaxH == MAXWORD ? inst->_RI.sc->Height : ar->_MM.MaxH;
			inst->PosTags[8].ti_Data = (ULONG)inst->_RI.sc;
			if (get_window( obj, /*((OOMsg_Setup*)msg)*/ar->GUI, inst ) == OOV_OK)
				{
				if ((inst->_RI.GTLast = CreateContext( &inst->_RI.GTList )) != NULL) // gadtools 
					{
					if ((rc = OO_DoBaseMethod( cl, obj, method, attrlist )) == OOV_OK)	//********************* 
						{														// ajout des gadgets 
						AddGList( inst->_RI.wi, inst->_RI.GTList, -1, -1, inst->_RI.rq ); // gadtools 
						}
					}
				}
			return rc;
			}

		case OOM_AREA_REFRESH:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_RastPort );
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if (passed_flags & OOF_SAME_RI) break;
			}
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			SetFont( inst->_RI.wi->RPort, inst->_RI.font );
			DrawFill( inst, _DATA_AREA(obj) );
			}
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) == OOV_OK) //********************* 
				{
				OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
				if (inst->_RI.GTList)
					{
					RefreshGList( inst->_RI.GTList, inst->_RI.wi, inst->_RI.rq, -1 );	// gadtools 
					GT_RefreshWindow( inst->_RI.wi, inst->_RI.rq ); 					// gadtools 
					}
				}
			break;

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if (passed_flags & OOF_SAME_RI) break;
			}
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);

			ar->_B.X = inst->_RI.wi->LeftEdge;
			ar->_B.Y = inst->_RI.wi->TopEdge;
			ar->_B.W = inst->_RI.wi->Width;
			ar->_B.H = inst->_RI.wi->Height;

			if (inst->_RI.GTList)
				RemoveGList( inst->_RI.wi, inst->_RI.GTList, -1 );		// gadtools 
			}
			OO_DoBaseMethod( cl, obj, method, attrlist ); //********************* 
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			if (inst->_RI.GTList)
				{
				FreeGadgets( inst->_RI.GTList );	  // gadtools 
				inst->_RI.GTList = 0;
				}
			reply_gtmsg( inst );
			unget_window( obj, inst );
			}
			break;

		case OOM_AREA_CLEANUP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			OO_DoBaseMethod( cl, obj, method, attrlist );
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			if (inst->Flags & OOF_WIN_ALLOCFONT)
				{
				CloseFont( inst->_RI.font );
				inst->_RI.font = 0;
				inst->Flags &= ~OOF_WIN_ALLOCFONT;
				}
			}
			break;

		case OOM_HANDLEEVENT:
			//OO_DoBaseMethod( cl, obj, method, attrlist ); 
			HandleWindowMsg( cl, obj, attrlist );
			break;

		case OOM_AREA_CHANGED:
			OO_DoBaseMethod( cl, obj, method, attrlist );
			{
			OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
			OOInst_Area *ar = (OOInst_Area*)_DATA_AREA(obj);

			if (! (ar->StatusFlags & OOF_AREA_STARTED)) break;
			if (! (ar->StatusFlags & OOF_AREA_CHANGED)) break;
			ar->StatusFlags &= ~OOF_AREA_CHANGED;

			// case OOM_AREA_STOP 
			if (inst->_RI.GTList) WindowSizeVerify( obj, inst );

			// case OOM_AREA_CLEANUP 
			OO_DoChildrenMethodV( 0, obj, OOM_AREA_CLEANUP, OOFlags, OOF_SAME_RI, TAG_END );

			// case OOM_AREA_SETUP 
			OO_DoChildrenMethodV( OOF_TESTCHILD, obj, OOM_AREA_SETUP, OOA_Flags, OOF_SAME_RI, OOA_HandleGUI, ar->GUI, OOA_RenderInfo, ar->RI, TAG_END );

			ChangeWindow( obj, inst );
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
 *		Utilities
 *
 ***************************************************************************/

struct _parm { OObject *wo; ULONG block; };

static void close_req( OObject *wo, OOInst_AreaWindow *inst )
{
	if (inst->SyncRequest)
		{
		if (inst->Flags & OOF_WIN_SYNCREQUEST)
			{
			if (inst->_RI.wi) EndRequest( inst->SyncRequest, inst->_RI.wi );
			inst->Flags &= ~OOF_WIN_SYNCREQUEST;
			}
		OOPool_Free( wo->Vars->Pool, inst->SyncRequest );
		inst->SyncRequest = 0;
		}
}

static ULONG blockwin( OObject *obj, struct _parm *parm )
{
	if (obj->Class == &Class_AreaWindow && obj != parm->wo)
		{
		OOInst_AreaWindow *inst = OINST_DATA(&Class_AreaWindow,obj);
		if (parm->block)
			{
			if (! inst->SyncObj)
				{
				inst->SyncObj = parm->wo;
				OO_SetAttrs( obj, OOA_PtrWait, TRUE, TAG_END );
				if (inst->_RI.wi)
					if (inst->SyncRequest = OOPool_AllocClear( OOGlobalPool, sizeof(struct Requester) ))
						{
						InitRequester( inst->SyncRequest );
						inst->SyncRequest->LeftEdge = -2;
						inst->SyncRequest->TopEdge	= -2;
						inst->SyncRequest->Width	= 1;
						inst->SyncRequest->Height	= 1;
						if (Request( inst->SyncRequest, inst->_RI.wi )) inst->Flags |= OOF_WIN_SYNCREQUEST;
						}
				}
			}
		else{
			if (inst->SyncObj == parm->wo)
				{
				inst->SyncObj = 0;
				OO_SetAttrs( obj, OOA_PtrSys, TRUE, TAG_END );
				close_req( obj, inst);
				}
			}
		}
	return(0);
}

static void block_other_wins( OObject *wo, OOInst_AreaWindow *inst, ULONG block )
{
  OObject *hdl;
  OOInst_HandleGUI *hdlid;
  struct _parm PARM;

	if (block && !(inst->Flags & OOF_WIN_SYNC)) return;

	for (hdl=wo; hdl->Class != &Class_HandleGUI; hdl=hdl->Parent) ;
	hdlid = OINST_DATA(&Class_HandleGUI,hdl);
	PARM.wo = wo;
	PARM.block = block;
	if (block)
		{
		inst->SyncPrev = hdlid->SyncObj;
		hdlid->SyncObj = wo;
		}
	else{
		if (hdlid->SyncObj == wo)
			{
			hdlid->SyncObj = inst->SyncPrev;
			inst->SyncPrev = 0;
			}
		}
	OO_ParseTree( OO_GetRootObject(wo), 0, blockwin, (ULONG)&PARM );
}

//--------

static void reply_gtmsg( OOInst_AreaWindow *inst )
{
	if (inst->GTImsg)
		{
		inst->GTImsg = GT_PostFilterIMsg( inst->GTImsg );
		ReplyMsg( (struct Message *)inst->GTImsg );
		inst->GTImsg = 0;
		}
}

static void strip_idcmp( struct Window *w, ULONG idcmpmask )
{
  struct Node *node, *succ;

	Forbid();
	for (node=w->UserPort->mp_MsgList.lh_Head; succ=node->ln_Succ; node=succ)
		{
		if (((struct IntuiMessage *)node)->IDCMPWindow == w)
			if (((struct IntuiMessage *)node)->Class & idcmpmask)
				{
				Remove( node );
				ReplyMsg( (struct Message *)node );
				}
		}
	Permit();
}

static void close_window_safely( struct Window *w )
{
	Forbid();
	strip_idcmp( w, 0xffffffff );
	w->UserPort = NULL;
	ModifyIDCMP( w, 0 );
	Permit();
	CloseWindow( w );
}

static void unget_window( OObject *obj, OOInst_AreaWindow *inst )
{
	if (inst->_RI.wi)
		{
		inst->_RI.rp = 0;
		close_req( obj, inst);
		close_window_safely( inst->_RI.wi );
		inst->_RI.wi = 0;
		}
	inst->Installed = 0;
	block_other_wins( obj, inst, 0 );
}

static ULONG get_window( OObject *obj, OObject *gui, OOInst_AreaWindow *inst )
{
  ULONG idcmp=0;
  struct MsgPort *port=0;
  OOTagItem *ti;

	inst->_RI.wi = 0;
	inst->_RI.rq = 0;
	inst->_RI.rp = 0;

	if ((ti = OOTag_FindTag( inst->PosTags, WA_IDCMP )) != NULL) { idcmp = ti->ti_Data; ti->ti_Data = 0; }

	OO_GetAttrsV( gui, OOA_EventPort, &port, TAG_END );
	if (port)
		{
		block_other_wins( obj, inst, 1 );
		if (inst->_RI.wi = OpenWindowTagList( NULL, inst->PosTags ))
			{
			install_ptr( inst, inst->CurrentPtr );
			inst->_RI.wi->UserPort = port;
			inst->_RI.wi->UserData = (APTR)obj;
			if (IDCMPS & ~idcmp) idcmp = ModifyIDCMP( inst->_RI.wi, IDCMPS & ~idcmp );
			else idcmp = 1;
			if (idcmp)
				{
				inst->_RI.rp = inst->_RI.wi->RPort;
				return OOV_OK;
				}
			unget_window( obj, inst );
			}
		}

	if (idcmp) ti->ti_Data = idcmp;

	return OOV_ERROR;
}

static void DrawFill( OOInst_AreaWindow *inst, OOInst_Area *ar )
{
	if (inst->Flags & OOF_WIN_FILLED)
		{
		short *pens = inst->_RI.di->dri_Pens;

		if (inst->Pattern) { SetABPenDrMd( inst->_RI.rp, pens[SHINEPEN], pens[BACKGROUNDPEN], JAM2 ); SetAfPt( inst->_RI.rp, (UWORD*)&inst->Pattern, 1 ); }
		else SetABPenDrMd( inst->_RI.rp, pens[BACKGROUNDPEN], pens[SHINEPEN], JAM2 );
		RectFill( inst->_RI.rp, ar->_Border.Left, ar->_Border.Top, ar->_B.W - ar->_Border.Right - 1, ar->_B.H - ar->_Border.Bottom - 1 );
		SetAfPt( inst->_RI.rp, 0, 0 );
		}
}

static void WindowSizeVerify( OObject *obj, OOInst_AreaWindow *inst )
{
	// case OOM_AREA_STOP 
	if (inst->_RI.GTList) RemoveGList( inst->_RI.wi, inst->_RI.GTList, -1 );	  // gadtools 
	reply_gtmsg( inst );
	OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_STOP, OOA_Flags, OOF_SAME_RI, TAG_END );
	if (inst->_RI.GTList) { FreeGadgets( inst->_RI.GTList ); inst->_RI.GTList = 0; } // gadtools 
}

static void WindowNewSize( OObject *obj, OOInst_AreaWindow *inst )
{
  OOInst_Area *ar = _DATA_AREA(obj);
  struct Window *wi = inst->_RI.wi;

	if (inst->_RI.GTList) WindowSizeVerify( obj, inst ); // pas eu de SIZEVERIFY avant (bug MultiCX OPAQSIZE) 
	strip_idcmp( wi, IDCMP_GADGETHELP|IDCMP_INTUITICKS|IDCMP_GADGETDOWN|IDCMP_GADGETUP|IDCMP_MOUSEMOVE );

	WindowLimits( inst->_RI.wi, ar->_MM.MinW, ar->_MM.MinH, ar->_MM.MaxW, ar->_MM.MaxH );

	// case OOM_AREA_BOX 
	ar->_B.W = wi->Width;
	ar->_B.H = wi->Height;
	OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_BOX, OOA_Flags, OOF_SAME_RI, TAG_END );

	// case OOM_AREA_START 
	if (inst->_RI.GTLast = CreateContext( &inst->_RI.GTList )) // gadtools 
		{
		if (OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_START, OOA_Flags, OOF_SAME_RI, TAG_END ) == OOV_OK) // ajout des gadgets 
			if (inst->_RI.GTList)
				AddGList( wi, inst->_RI.GTList, -1, -1, inst->_RI.rq ); // gadtools 
		}

	// case OOM_AREA_REFRESH 
	SetFont( wi->RPort, inst->_RI.font );
	DrawFill( inst, ar, NULL );
	if (OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_REFRESH, OOA_Flags, OOF_SAME_RI, OOA_RastPort, NULL, TAG_END ) == OOV_OK)
		{
		if (inst->_RI.GTList)
			{
			if (inst->Flags & OOF_WIN_OVERFLOW)
				{
				RefreshWindowFrame( wi );
				inst->Flags &= ~OOF_WIN_OVERFLOW;
				inst->Flags |= OOF_WIN_OVFLAST;
				}
			else if (inst->Flags & OOF_WIN_OVFLAST)
				{
				RefreshWindowFrame( wi );
				inst->Flags &= ~OOF_WIN_OVFLAST;
				}
			RefreshGList( inst->_RI.GTList, wi, inst->_RI.rq, -1 ); // gadtools 
			GT_RefreshWindow( wi, inst->_RI.rq );				  // gadtools 
			}
		}
}

static void ChangeWindow( OObject *obj, OOInst_AreaWindow *inst )
{
  OOInst_Area *ar = _DATA_AREA(obj);
  struct Screen *sc = inst->_RI.sc;

	OO_DoMethodV( obj, OOM_AREA_MINMAX, OOA_Flags, OOF_SELF_RI|OOF_SAME_RI, TAG_END );
	ar->_B.X = inst->_RI.wi->LeftEdge;
	ar->_B.Y = inst->_RI.wi->TopEdge;
	if (ar->_B.X + ar->_B.W > sc->Width) ar->_B.X = sc->Width - ar->_B.W;
	if (ar->_B.Y + ar->_B.H > sc->Height) ar->_B.Y = sc->Height - ar->_B.H;

	if ((ar->_B.W == inst->_RI.wi->Width) && (ar->_B.H == inst->_RI.wi->Height))
		{
		WindowNewSize( obj, inst );
		}
	else{
		WindowSizeVerify( obj, inst );
		ChangeWindowBox( inst->_RI.wi, ar->_B.X, ar->_B.Y, ar->_B.W, ar->_B.H );
		}
}

static void HandleWindowMsg( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_AreaWindow *inst = OINST_DATA(cl,obj);
	OOInst_Area *ar = _DATA_AREA(obj);
	struct InputEvent ie;

	OOInst_HandleGUI	*gui	= attrlist[0].ti_Ptr; // OOA_HandleGUI
	OOEventMsg			*imsg	= attrlist[1].ti_Ptr; // OOA_EventMsg
	OOTagList			*attrs	= attrlist[2].ti_Ptr; // OOA_TagList
	OOTagItem			*ti		= attrlist[3].ti_Ptr; // OOA_TagItem
	OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_HandleGUI && attrlist[].ti_Tag == OOA_EventMsg && attrlist[2].ti_Tag == OOA_TagList && attrlist[3].ti_Tag == OOA_TagItem)

	switch (imsg->Class)
		{
		case IDCMP_MOUSEMOVE:
			ti->ti_Tag = OOA_MousePos; ti->ti_Data = *((ULONG*)&imsg->MouseX); ti++;
			break;

		case IDCMP_INTUITICKS:
			ti->ti_Tag = OOA_Second10; ti->ti_Data = TRUE; ti++;
			break;

		case IDCMP_MOUSEBUTTONS:
			switch (imsg->Code)
				{
				case SELECTUP:
					ti->ti_Tag = OOA_ButLeftUp; ti->ti_Data = TRUE; ti++;
					break;
				case SELECTDOWN:
					ti->ti_Tag = OOA_ButLeftDown; ti->ti_Data = TRUE; ti++;
					break;
				case MIDDLEUP:
					ti->ti_Tag = OOA_ButMidUp; ti->ti_Data = TRUE; ti++;
					break;
				case MIDDLEDOWN:
					ti->ti_Tag = OOA_ButMidDown; ti->ti_Data = TRUE; ti++;
					break;
				case MENUUP:
					ti->ti_Tag = OOA_ButRightUp; ti->ti_Data = TRUE; ti++;
					break;
				case MENUDOWN:
					ti->ti_Tag = OOA_ButRightDown; ti->ti_Data = TRUE; ti++;
					break;
				}
			break;

		case IDCMP_RAWKEY:
			inst->Key.Code = ie.ie_Code = imsg->Code;
			inst->Key.Qual = ie.ie_Qualifier = imsg->Qualifier;
			ie.ie_EventAddress = (APTR *) *((ULONG *)imsg->IAddress);
			inst->Key.Length = MapRawKey( &ie, inst->Key.Buffer, 7, 0 );
			inst->Key.Key = inst->Key.Buffer[0];
			ti->ti_Tag = (imsg->Code & IECODE_UP_PREFIX) ? OOA_KeyUp : OOA_KeyDown; ti->ti_Data = (ULONG)&inst->Key; ti++;
			break;

		case IDCMP_ACTIVEWINDOW:
			ti->ti_Tag = OOA_WinActive; ti->ti_Data = TRUE; ti++;
			break;
		case IDCMP_INACTIVEWINDOW:
			ti->ti_Tag = OOA_WinActive; ti->ti_Data = FALSE; ti++;
			break;

		case IDCMP_REQSET:
		case IDCMP_REQCLEAR:
			break;

		case IDCMP_DISKINSERTED:
		case IDCMP_DISKREMOVED:
			break;

		case IDCMP_MENUPICK:
		case IDCMP_MENUVERIFY:
		case IDCMP_MENUHELP:
			break;

		case IDCMP_SIZEVERIFY:
			inst->Flags |= OOF_WIN_SVERIFY;
			WindowSizeVerify( obj, inst );
			break;

		case IDCMP_NEWSIZE:
			if (inst->Flags & OOF_WIN_SVERIFY)
				{
				inst->Flags &= ~OOF_WIN_SVERIFY;
				ar->AskedW = inst->_RI.wi->Width;
				ar->AskedH = inst->_RI.wi->Height;
				}
			WindowNewSize( obj, inst );
			ti->ti_Tag = OOA_WinChange; ti->ti_Data = TRUE; ti++;
			break;

		case IDCMP_CHANGEWINDOW:
			ar->_B.X = inst->_RI.wi->LeftEdge;
			ar->_B.Y = inst->_RI.wi->TopEdge;
			ti->ti_Tag = OOA_WinChange; ti->ti_Data = TRUE; ti++;
			break;

		case IDCMP_REFRESHWINDOW:
			GT_BeginRefresh( imsg->IDCMPWindow );
			GT_EndRefresh( imsg->IDCMPWindow, TRUE );
			{ // Pour bug MultiCX qui oublie le SIZEVERIFY 
			if ((ar->_B.W != inst->_RI.wi->Width) || (ar->_B.H != inst->_RI.wi->Height)) inst->Flags |= OOF_WIN_BADSIZE;
			if (inst->Flags & OOF_WIN_BADSIZE) RefreshWindowFrame( imsg->IDCMPWindow );
			}
			ti->ti_Tag = OOA_WinRefreshAsk; ti->ti_Data = TRUE; ti++;
			break;

		case IDCMP_CLOSEWINDOW:
			ti->ti_Tag = OOA_WinCloseAsk; ti->ti_Data = TRUE; ti++;
			break;

		case IDCMP_GADGETHELP:
			break;
		}

	OOResult = (OORESULT)ti;
}

static void install_ptr( OOInst_AreaWindow *inst, ULONG pointer )
{
	inst->CurrentPtr = pointer;
	if (pointer && (inst->Installed != inst->CurrentPtr) && inst->_RI.wi)
		{
		switch (pointer)
			{
			case OOV_PTR_SYS:
				{ OOTagItem tl[] = { TAG_END };
				SetWindowPointerA( inst->_RI.wi, tl );
				}
				break;

			case OOV_PTR_WAIT:
				{ OOTagItem tl[] = { WA_BusyPointer, TRUE, WA_PointerDelay, TRUE, TAG_END };
				SetWindowPointerA( inst->_RI.wi, tl );
				}
				break;

			case OOV_PTR_NULL:
				SetPointer( inst->_RI.wi, &NullPointer[0], 1, 16, 0, 0 );
				break;

			default:
				SetPointer( inst->_RI.wi, (APTR)pointer, 1, 16, 0, 0 );
				break;
			}
		inst->Installed = inst->CurrentPtr;
		}
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
	OOInst_AreaWindow *inst = (OOInst_AreaWindow*)sgp->Instance;
	ULONG change = OOV_NOCHANGE;
	ULONG flag = 0, title = 0;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_TitleIndex:
				sgp->Attr->ti_Data = data;
				if (OOPrv_MSetBounceV( sgp, OOA_Title, OOStr_Get( sgp->ExtAttr->ti_Data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_Title: 
				title = data; 
				break;

			case OOA_Pattern:
				if (inst->Pattern != data) { change = OOV_CHANGED; inst->Pattern = data; }
				break;

			case OOA_PtrCurrent: if (data) AW_SetPointer( inst, data		 ); break;
			case OOA_PtrNull   : if (data) AW_SetPointer( inst, OOV_PTR_NULL ); break;
			case OOA_PtrWait   : if (data) AW_SetPointer( inst, OOV_PTR_WAIT ); break;
			case OOA_PtrSys    : if (data) AW_SetPointer( inst, OOV_PTR_SYS	 ); break;

			case OOA_Sync			: flag = OOF_WIN_SYNC		; break;
			case OOA_Filled			: flag = OOF_WIN_FILLED		; break;
			case OOA_Borderless		: flag = OOF_WIN_BORDERLESS	; break;
			case OOA_WinCtlSize		: flag = OOF_WCTL_SIZE 		; break;
			case OOA_WinCtlDrag		: flag = OOF_WCTL_DRAG 		; break;
			case OOA_WinCtlDepth	: flag = OOF_WCTL_DEPTH		; break;
			case OOA_WinCtlClose	: flag = OOF_WCTL_CLOSE		; break;
			case OOA_WinCtlZoom		: flag = OOF_WCTL_ZOOM 		; break;
			case OOA_WinCtlIcon		: flag = OOF_WCTL_ICON 		; break;
			case OOA_WinCtlInsRight	: flag = OOF_WCTL_INSRIGHT	; break;
			case OOA_WinCtlInsBot	: flag = OOF_WCTL_INSBOT	; break;
			}

		if (flag)
			{
			if (sgp->ExtAttr->ti_Data == TRUE)
				 { change = (inst->Flags & flag) ? OOV_NOCHANGE : OOV_CHANGED ; inst->Flags |= flag; }
			else { change = (inst->Flags & flag) ? OOV_CHANGED	: OOV_NOCHANGE; inst->Flags &= ~flag; }
			sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
			}

		if (title)
			{
			inst->PosTags[10].ti_Data = title;
			if (inst->_RI.wi)
				if (strcmp( inst->_RI.wi->Title, (UBYTE*)title ) != 0)
					{
					SetWindowTitles( inst->_RI.wi, (APTR)title, (APTR)-1 );
					change = OOV_CHANGED;
					}
			}
		}
	else{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_Title	   : *((ULONG*)data) = inst->PosTags[10].ti_Data; break;
			case OOA_Pattern   : *((ULONG*)data) = inst->Pattern 			; break;
			case OOA_PtrCurrent: *((ULONG*)data) = inst->CurrentPtr			; break;

			default: *(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr; break;
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
	{ OOA_Title 		, "Title"		  , (ULONG)NULL	, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_TitleIndex    , "TitleIndex"    , 0			, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_FontName		, "FontName"	  , (ULONG)NULL	, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_FontHeight	, "FontHeight"	  , 0			, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_FontHeightP100, "FontHeightP100", 0			, SetGet_All, OOF_MNEW | OOF_MSET	|				OOF_MAREA	},
	{ OOA_Pattern		, "Pattern" 	  , 0			, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_Filled		, "Filled"		  , TRUE		, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_PtrCurrent	, "PtrCurrent"	  , (ULONG)NULL	, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_PtrNull		, "PtrNull" 	  , (ULONG)NULL	, SetGet_All, OOF_MNEW | OOF_MSET								},
	{ OOA_PtrWait		, "PtrWait" 	  , (ULONG)NULL	, SetGet_All, OOF_MNEW | OOF_MSET								},
	{ OOA_PtrSys		, "PtrSys"		  , (ULONG)NULL	, SetGet_All, OOF_MNEW | OOF_MSET								},
	{ OOA_Sync			, "Sync"		  , FALSE		, SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_Borderless	, "Borderless"	  , FALSE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlIcon	, "WinCtlIcon"	  , TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlZoom	, "WinCtlZoom"	  , TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlDepth	, "WinCtlDepth"   , TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlSize	, "WinCtlSize"	  , TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlDrag	, "WinCtlDrag"	  , TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlClose	, "WinCtlClose"   , TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlInsRight, "WinCtlInsRight", TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_WinCtlInsBot	, "WinCtlInsBot"  , TRUE		, SetGet_All, OOF_MNEW 				| OOF_MGET					},
	{ OOA_MousePos		, "MousePos"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_Second10		, "Second10"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_ButLeftDown	, "ButLeftDown"   , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_ButLeftUp 	, "ButLeftUp"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_ButMidDown	, "ButMidDown"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_ButMidUp		, "ButMidUp"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_ButRightDown	, "ButRightDown"  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_ButRightUp	, "ButRightUp"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_KeyDown		, "KeyDown" 	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_KeyUp 		, "KeyUp"		  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_WinActive 	, "WinActive"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_WinCloseAsk	, "WinCloseAsk"   , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_WinRefreshAsk , "WinRefreshAsk" , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ OOA_WinChange 	, "WinChange"	  , 0			, SetGet_All,									OOF_MNOTIFY 	},
	{ TAG_END }
	};

OOClass Class_AreaWindow = { 
	Dispatcher_AreaWindow, 
	&Class_AreaGroup,		// Base class
	"Class_AreaWindow", 	// Class Name
	0,						// InstOffset
	sizeof(OOInst_AreaWindow),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
