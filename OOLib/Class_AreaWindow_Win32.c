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
#include "oo_pool.h"
#include "oo_buf.h"
#include "oo_text.h"

#include "oo_gui_prv.h"

#define OOF_WIN_FILLED			0x00000001
//+++#define OOF_WIN_OVERFLOW	0x00000002
//+++#define OOF_WIN_OVFLAST 	0x00000004
#define OOF_WIN_SVERIFY 		0x00000008
#define OOF_WIN_ALLOCFONT		0x00000010
#define OOF_WIN_ALLOCFONTNAME	0x00000020
#define OOF_WIN_BORDERLESS		0x00000040
#define OOF_WIN_SYNC			0x00000080
#define OOF_WIN_SYNCREQUEST		0x00000100
#define OOF_WIN_CENTERED		0x00000200
#define OOF_WIN_CLOSEQUIT		0x00000400

#define OOF_WCTL_CLOSE			0x00010000
#define OOF_WCTL_ICON			0x00020000
#define OOF_WCTL_ZOOM			0x00040000
#define OOF_WCTL_DEPTH			0x00080000
#define OOF_WCTL_DRAG			0x00100000
#define OOF_WCTL_SIZE			0x00200000
#define OOF_WCTL_INSRIGHT		0x00400000
#define OOF_WCTL_INSBOT 		0x00800000

#define INDEX_OOA_Title		1


//****** Imported ***********************************************


//****** Exported ***********************************************


//****** Statics ************************************************

static OOTagItem wflags_boolmap[] = {
	{ OOA_CloseQuit		, OOF_WIN_CLOSEQUIT },
	{ OOA_Sync			, OOF_WIN_SYNC		},
	{ OOA_Filled		, OOF_WIN_FILLED	},
	{ OOA_Borderless	, OOF_WIN_BORDERLESS},
	{ OOA_WinCtlSize	, OOF_WCTL_SIZE 	},
	{ OOA_WinCtlDrag	, OOF_WCTL_DRAG 	},
//	{ OOA_WinCtlDepth	, OOF_WCTL_DEPTH	},	ignore
	{ OOA_WinCtlClose	, OOF_WCTL_CLOSE	},
	{ OOA_WinCtlZoom	, OOF_WCTL_ZOOM 	},
	{ OOA_WinCtlIcon	, OOF_WCTL_ICON 	},  
//	{ OOA_WinCtlInsRight, OOF_WCTL_INSRIGHT	},	ignore
//	{ OOA_WinCtlInsBot	, OOF_WCTL_INSBOT	},	ignore
	{ TAG_END }
	};

//+++ static UWORD chip NullPointer[(1 * 2) + 4] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, };

static void close_req( OObject *wo, OOInst_AreaWindow *inst );
static ULONG blockwin( OObject *obj, struct _parm *parm );
static void block_other_wins( OObject *wo, OOInst_AreaWindow *inst, BOOL block );
static void close_window_safely( HWND w );
static void AW_MakeWinStyles( OOInst_AreaWindow *inst, OOInst_Area *ar, UBYTE *title );
static void AW_MakeWinBorders( OObject *obj, OOInst_AreaWindow *inst );
static void AW_UngetWindow( OObject *obj, OOInst_AreaWindow *inst );
static ULONG AW_GetWindow( OObject *obj, OObject *gui, OOInst_AreaWindow *inst );
static void AW_DrawFill( OOInst_AreaWindow *inst, OOInst_Area *ar, OORastPort *rp );
static void AW_WindowSizeVerify( OObject *obj, OOInst_AreaWindow *inst );
static void AW_WindowNewSize( OObject *obj, OOInst_AreaWindow *inst );
static void AW_ChangeWindow( OObject *obj, OOInst_AreaWindow *inst );
static ULONG AW_HandleWindowMsg( OOClass *cl, OObject *obj, OOTagList *attrlist );
static ULONG AW_SetPointer( OOInst_AreaWindow *inst, ULONG pointer );


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
				OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
				OOTagList *tl = OOINST_ATTRS(cl,obj);
				// Init instance datas
				inst->CurrentPtr = inst->Installed = OOV_PTR_SYS;
				inst->BufTitle = NULL;
				OOGfx_InitRPort( &inst->_PlainRP );
				// Init default attrs & change defaults with new values
		        if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				// Specialized defaults
				if (OOTag_FindTag( attrlist, OOA_Margins ) == NULL) 
					OO_SetAttr( obj, OOA_Margins, TRUE );
				if (OOTag_FindTag( attrlist, OOA_LeftTop ) == NULL) 
					inst->Flags |= OOF_WIN_CENTERED;
				// Make default flags given by booleen tags defaults
				inst->Flags = OOTag_PackBool( tl, wflags_boolmap, inst->Flags );
				}
			return (ULONG)obj;

		case OOM_DELETE:
			OOBuf_Free( &((OOInst_AreaWindow*)OOINST_DATA(cl,obj))->BufTitle );
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
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			ULONG rc, passed_flags = attrlist[0].ti_Data; // OOA_Flags
			OORenderInfo *ri = (OORenderInfo*) attrlist[2].ti_Ptr;	// OOA_RenderInfo

			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (passed_flags & OOF_SAME_RI) break;

			inst->_RI.sc = ri->sc;
			inst->_RI.Palette = ri->Palette;
			inst->_PlainRP.Palette = ri->Palette;
			if ((inst->_RI.Font = OOGfx_OpenFont( ri->Font, OOINST_ATTRS(cl,obj) )) == NULL) break;
			if (inst->_RI.Font != ri->Font) inst->Flags |= OOF_WIN_ALLOCFONT;
			OOGfx_FontToTagList( inst->_RI.Font, OOINST_ATTRS(cl,obj) );

			//......... Remplace par le nouveau OOA_RenderInfo dans attrlist
			attrlist[2].ti_Ptr = &inst->_RI;
			if ((rc = OO_DoBaseMethod( cl, obj, method, attrlist )) == OOV_OK) //**********************
				{
				AW_MakeWinBorders( obj, inst );
				}
			//......... Restore l'ancien OOA_RenderInfo dans attrlist
			attrlist[2].ti_Ptr = ri;
			return rc;
			}

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);

			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if ((passed_flags & (OOF_SAME_RI|OOF_SELF_RI)) == OOF_SAME_RI) 
				{ OOResult = (OORESULT)_DATA_AREA(obj); break; }
			}

			//--- Minimax des objets fils 
			OO_DoBaseMethod( cl, obj, method, attrlist ); // ********************* OOResult = OOInst_Area* 
			if (ar->_MM.MinW == ar->_MM.MaxW && ar->_MM.MinH == ar->_MM.MaxH)
				if (inst->Flags & (OOF_WCTL_ZOOM|OOF_WCTL_SIZE))
					{
					// On refait car l'absence de resize modifie l'épaisseurs des bords
					inst->Flags &= ~(OOF_WCTL_ZOOM|OOF_WCTL_SIZE);
					AW_MakeWinBorders( obj, inst );
					OO_DoBaseMethod( cl, obj, method, attrlist );
					}

			//--- Obtention de _MM 
			{
			SWORD minw, minh;
			OObject *par;
			OOInst_AreaScreen *scinst;
			OOInst_Area *scarea;

			par = OO_GetParentOfClass( obj, &Class_AreaScreen );
			scinst = OOINST_DATA(&Class_AreaScreen,par);
			scarea = _DATA_AREA(par);

			//--- Largeur minimum 
			minw = ar->_Border.Left + ar->_Border.Right;
			if ((inst->Styles & (WS_DLGFRAME|WS_BORDER)) == (WS_DLGFRAME|WS_BORDER)) 
				{
				minw += 32;
				if (inst->Styles & WS_SYSMENU)
					{
					NONCLIENTMETRICS NCM;
					NCM.cbSize = sizeof(NCM);
					SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof(NCM), &NCM, 0 );					
					minw += 2 * NCM.iCaptionWidth; //+++ Approximation légèrement supérieure à la normale (reste à voir la fonte)
					if (inst->Styles & (WS_MINIMIZEBOX|WS_MAXIMIZEBOX))
						minw += 2 * NCM.iCaptionWidth;
					}
				}
			if (ar->_MM.MinW < minw) ar->_MM.MinW = minw;

			//--- Hauteur minimum 
			minh = ar->_Border.Top + ar->_Border.Bottom;
			if (ar->_MM.MinH < minh) ar->_MM.MinH = minh;

			//--- On repousse les maxi car on a touché les mini 
			if (ar->_MM.MaxW < ar->_MM.MinW) ar->_MM.MaxW = ar->_MM.MinW;
			if (ar->_MM.MaxH < ar->_MM.MinH) ar->_MM.MaxH = ar->_MM.MinH;
#ifdef AMIGA
			if (ar->_MM.MaxW > scarea->_B.W) ar->_MM.MaxW = scarea->_B.W;
			if (ar->_MM.MaxH > scarea->_B.H) ar->_MM.MaxH = scarea->_B.H;
#endif
			if (ar->_MM.MinW > ar->_MM.MaxW) { ar->_MM.MinW = ar->_MM.MaxW; } //+++ inst->Flags |= OOF_WIN_OVERFLOW; }
			if (ar->_MM.MinH > ar->_MM.MaxH) { ar->_MM.MinH = ar->_MM.MaxH; } //+++ inst->Flags |= OOF_WIN_OVERFLOW; }

			//--- Obtention de _B.W et _B.H 
			if (ar->_B.W < ar->_MM.MinW) ar->_B.W = ar->_MM.MinW;
			if (ar->_B.H < ar->_MM.MinH) ar->_B.H = ar->_MM.MinH;
			if (ar->_B.W > ar->_MM.MaxW) ar->_B.W = ar->_MM.MaxW;
			if (ar->_B.H > ar->_MM.MaxH) ar->_B.H = ar->_MM.MaxH;

			OOResult = (OORESULT) ar;
			}
			}
			break;

		case OOM_AREA_BOX: // Accorde _B et _SubB avec _MM 
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if (passed_flags & OOF_SAME_RI) { OOResult = (OORESULT)_DATA_AREA(obj); break; }
			}
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);
			OObject *par;
			OOInst_Area *scarea;

			par = OO_GetParentOfClass( obj, &Class_AreaScreen );
			scarea = _DATA_AREA(par);

			//--- On centre la fenêtre dans une area 
			if (inst->Flags & OOF_WIN_CENTERED)
				{
				OObject *par;
				OOInst_Area *ref;
				SWORD w, h;
			    for (par=obj->ParentsTable.Table[0]; par->ParentsTable.Count; par=par->ParentsTable.Table[0])
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
			if (ar->_B.X) { if (ar->_B.X < 0) ar->_B.X = 0; if (ar->_B.X + ar->_B.W > scarea->_B.W) ar->_B.X = scarea->_B.W - ar->_B.W; }
			if (ar->_B.Y) { if (ar->_B.Y < 0) ar->_B.Y = 0; if (ar->_B.Y + ar->_B.H > scarea->_B.H) ar->_B.Y = scarea->_B.H - ar->_B.H; }
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
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);
			ULONG rc = FALSE;

			if (AW_GetWindow( obj, /*((OOMsg_Setup*)msg)*/ar->GUI, inst ) == OOV_OK)
				{
//+++				if ((inst->_RI.GTLast = CreateContext( &inst->_RI.GTList )) != NULL) // gadtools 
					{
					if ((rc = OO_DoBaseMethod( cl, obj, method, attrlist )) == OOV_OK)	//********************* 
						{														// ajout des gadgets 
;//+++						AddGList( inst->_RI.wi, inst->_RI.GTList, -1, -1, inst->_RI.rq ); // gadtools 
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
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
//+++			SetFont( inst->_RI.wi->RPort, inst->_RI.font );
			AW_DrawFill( inst, _DATA_AREA(obj), attrlist[1].ti_Ptr ); // OOA_RastPort
			}
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) == OOV_OK) //********************* 
				{
				OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
//+++				if (inst->_RI.GTList)
//+++					{
//+++					RefreshGList( inst->_RI.GTList, inst->_RI.wi, inst->_RI.rq, -1 );	// gadtools 
//+++					GT_RefreshWindow( inst->_RI.wi, inst->_RI.rq ); 					// gadtools 
//+++					}
				}
			break;

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;
			{
			ULONG passed_flags = attrlist[0].ti_Data; // OOA_Flags
			if (passed_flags & OOF_SAME_RI) break;
			}
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);
			RECT rect;
			if (GetWindowRect( inst->_RI.wi, &rect ))
				{
				ar->_B.X = (SWORD)rect.left;
				ar->_B.Y = (SWORD)rect.top;
				ar->_B.W = (SWORD)(rect.right - rect.left);
				ar->_B.H = (SWORD)(rect.bottom - rect.top);
				}
//+++			if (inst->_RI.GTList)
//+++				RemoveGList( inst->_RI.wi, inst->_RI.GTList, -1 );		// gadtools 
			}
			OO_DoBaseMethod( cl, obj, method, attrlist ); //********************* 
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
//+++			if (inst->_RI.GTList)
//+++				{
//+++				FreeGadgets( inst->_RI.GTList );	  // gadtools 
//+++				inst->_RI.GTList = 0;
//+++				}
			AW_UngetWindow( obj, inst );
			}
			break;

		case OOM_AREA_CLEANUP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;
			OO_DoBaseMethod( cl, obj, method, attrlist );
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			if (inst->Flags & OOF_WIN_ALLOCFONT)
				{
				 OOGfx_CloseFont( inst->_RI.Font );
				inst->Flags &= ~OOF_WIN_ALLOCFONT;
				inst->_RI.Font = NULL;
				}
			}
			break;

		case OOM_HANDLEEVENT:
			//OO_DoBaseMethod( cl, obj, method, attrlist ); 
			return AW_HandleWindowMsg( cl, obj, attrlist );

		case OOM_AREA_CHANGED:
			OO_DoBaseMethod( cl, obj, method, attrlist );
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = (OOInst_Area*)_DATA_AREA(obj);

			if (! (ar->StatusFlags & OOF_AREA_STARTED)) break;
			if (! (ar->StatusFlags & OOF_AREA_CHANGED)) break;
			ar->StatusFlags &= ~OOF_AREA_CHANGED;

			// case OOM_AREA_STOP 
//+++			if (inst->_RI.GTList) 
				AW_WindowSizeVerify( obj, inst );

			// case OOM_AREA_CLEANUP 
			OO_DoChildrenMethodV( 0, obj, OOM_AREA_CLEANUP, OOA_Flags, OOF_SAME_RI, TAG_END );

			// case OOM_AREA_SETUP 
			OO_DoChildrenMethodV( OOF_TESTCHILD, obj, OOM_AREA_SETUP, OOA_Flags, OOF_SAME_RI, OOA_HandleGUI, ar->GUI, OOA_RenderInfo, ar->RI, TAG_END );

			AW_ChangeWindow( obj, inst );
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

struct _parm { OObject *wo; BOOL block; };

static void close_req( OObject *wo, OOInst_AreaWindow *inst )
{
	if (inst->SyncRequest != NULL)
		{
		if (inst->Flags & OOF_WIN_SYNCREQUEST)
			{
			if (inst->_RI.wi != NULL) 
;//+++				EndRequest( inst->SyncRequest, inst->_RI.wi );
			inst->Flags &= ~OOF_WIN_SYNCREQUEST;
			}
		OOPool_Free( OOGlobalPool, inst->SyncRequest );
		inst->SyncRequest = NULL;
		}
}

static ULONG blockwin( OObject *obj, struct _parm *parm )
{
	if (obj->Class == &Class_AreaWindow && obj != parm->wo)
		{
		OOInst_AreaWindow *inst = OOINST_DATA(&Class_AreaWindow,obj);
		if (parm->block == TRUE)
			{
			if (inst->SyncObj == NULL)
				{
				inst->SyncObj = parm->wo;
				OO_SetAttr( obj, OOA_PtrWait, TRUE );
				if (inst->_RI.wi != NULL)
					if (inst->SyncRequest = OOPool_AllocClear( OOGlobalPool, 4 )) //+++ sizeof(struct Requester) ))
						{
						//+++InitRequester( inst->SyncRequest );
						//+++inst->SyncRequest->LeftEdge = -2;
						//+++inst->SyncRequest->TopEdge	= -2;
						//+++inst->SyncRequest->Width	= 1;
						//+++inst->SyncRequest->Height	= 1;
						//+++if (Request( inst->SyncRequest, inst->_RI.wi )) 
							inst->Flags |= OOF_WIN_SYNCREQUEST;
						}
				}
			}
		else{
			if (inst->SyncObj == parm->wo)
				{
				inst->SyncObj = 0;
				OO_SetAttr( obj, OOA_PtrSys, TRUE );
				close_req( obj, inst);
				}
			}
		}
	return 0;
}

static void block_other_wins( OObject *wo, OOInst_AreaWindow *inst, BOOL block )
{
  OOInst_HandleGUI *hdlinst = _DATA_HANDLEGUI(wo);
  struct _parm PARM;

	if ((block == TRUE) && !(inst->Flags & OOF_WIN_SYNC)) return;

	PARM.wo = wo;
	PARM.block = block;
	if (block == TRUE)
		{
		inst->SyncPrev = hdlinst->SyncObj;
		hdlinst->SyncObj = wo;
		}
	else{
		if (hdlinst->SyncObj == wo)
			{
			hdlinst->SyncObj = inst->SyncPrev;
			inst->SyncPrev = 0;
			}
		}
	OO_ParseTree( OO_GetRootObject(wo), 0, blockwin, (ULONG)&PARM );
}

//--------

static void AW_MakeWinStyles( OOInst_AreaWindow *inst, OOInst_Area *ar, UBYTE *title )
{
	inst->Styles = WS_POPUP; // | WS_CLIPCHILDREN;

	if (title != NULL || (inst->Flags & OOF_WCTL_DRAG))	
		inst->Styles |= (WS_BORDER|WS_DLGFRAME); else inst->Styles &= ~(WS_BORDER|WS_DLGFRAME);

	if (inst->Flags & OOF_WIN_BORDERLESS)	inst->Styles &= ~WS_BORDER		; else inst->Styles |= WS_BORDER 		;
	if (inst->Flags & OOF_WCTL_CLOSE)		inst->Styles |= WS_SYSMENU		; else inst->Styles &= ~WS_SYSMENU		;
	if (inst->Flags & OOF_WCTL_ZOOM)		inst->Styles |= WS_MAXIMIZEBOX 	; else inst->Styles &= ~WS_MAXIMIZEBOX 	;
	if (inst->Flags & OOF_WCTL_ICON)		inst->Styles |= WS_MINIMIZEBOX 	; else inst->Styles &= ~WS_MINIMIZEBOX 	;
	if (inst->Flags & OOF_WCTL_SIZE)		inst->Styles |= WS_SIZEBOX		; else inst->Styles &= ~WS_SIZEBOX		;
}

static void AW_MakeWinBorders( OObject *obj, OOInst_AreaWindow *inst )
{
	OOInst_Area *ar = _DATA_AREA(obj);
	OObject *par = OO_GetParentOfClass( obj, &Class_AreaScreen );
	OOTagList *tl = OOINST_ATTRS(obj->Class,obj);
	UBYTE *title = tl[INDEX_OOA_Title].ti_String;
	//RECT rect;

	int CXBORDER  = GetSystemMetrics( SM_CXBORDER );
	int CYBORDER  = GetSystemMetrics( SM_CYBORDER );
	int CXFIXEDFRAME = GetSystemMetrics( SM_CXFIXEDFRAME );
	int CYFIXEDFRAME = GetSystemMetrics( SM_CYFIXEDFRAME );
	int CXSIZEFRAME  = GetSystemMetrics( SM_CXSIZEFRAME );
	int CYSIZEFRAME  = GetSystemMetrics( SM_CYSIZEFRAME );

	//int CXMAXTRACK = GetSystemMetrics( SM_CXMAXTRACK );
	//int CYMAXTRACK = GetSystemMetrics( SM_CYMAXTRACK );
	//int CXMINTRACK = GetSystemMetrics( SM_CXMINTRACK );
	//int CYMINTRACK = GetSystemMetrics( SM_CYMINTRACK );
	int CYCAPTION  = GetSystemMetrics( SM_CYCAPTION  );
	//int CYMENU     = GetSystemMetrics( SM_CYMENU     );

	//SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );

	// pour prendre 0,0 au lieu de _B.X,_B.Y dans AREA_BOX de la SuperCLass 
	ar->Flags = OOF_ANEWRENDERINFO; // élimine OOF_AINSIDE_FULLW et OOF_AINSIDE_FULLH 

	//--- Obtention de _Border 
	AW_MakeWinStyles( inst, ar, title );
	switch (inst->Styles & (WS_BORDER|WS_SIZEBOX|WS_DLGFRAME))
		{
		case 0:
			ar->_Border.Left = ar->_Border.Right = 0;
			ar->_Border.Top = ar->_Border.Bottom = 0;
			break;
		case WS_BORDER:
			ar->_Border.Left = ar->_Border.Right = CXBORDER;
			ar->_Border.Top = ar->_Border.Bottom = CYBORDER;
			break;
		case WS_SIZEBOX:
			ar->_Border.Left = ar->_Border.Right = CXSIZEFRAME;
			ar->_Border.Top = ar->_Border.Bottom = CXSIZEFRAME;
			break;
		case WS_BORDER|WS_SIZEBOX:
			ar->_Border.Left = ar->_Border.Right = CXSIZEFRAME;
			ar->_Border.Top = ar->_Border.Bottom = CXSIZEFRAME;
			break;
		case WS_DLGFRAME:
			ar->_Border.Left = ar->_Border.Right = CXFIXEDFRAME;
			ar->_Border.Top = ar->_Border.Bottom = CYFIXEDFRAME;
			ar->_Border.Top += CYCAPTION;
			break;
		case WS_BORDER|WS_DLGFRAME:
			ar->_Border.Left = ar->_Border.Right = CXFIXEDFRAME;
			ar->_Border.Top = ar->_Border.Bottom = CYFIXEDFRAME;
			ar->_Border.Top += CYCAPTION;
			break;
		case WS_SIZEBOX|WS_DLGFRAME:
			ar->_Border.Left = ar->_Border.Right = CXSIZEFRAME;
			ar->_Border.Top = ar->_Border.Bottom = CXSIZEFRAME;
			ar->_Border.Top += CYCAPTION; //+++ A verifier
			break;
		case WS_BORDER|WS_SIZEBOX|WS_DLGFRAME:
			ar->_Border.Left = ar->_Border.Right = CXSIZEFRAME;
			ar->_Border.Top = ar->_Border.Bottom = CXSIZEFRAME;
			ar->_Border.Top += CYCAPTION;
			break;
		}

	//--- Obtention de _SubBorder 
}

static void close_window_safely( HWND w )
{
	if (DestroyWindow( w ) == FALSE)
		{
		UBYTE *buf;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL, GetLastError(),
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR)&buf, 0, NULL );
		OOTRACE( buf );
		LocalFree( buf );
		}
}

static void AW_UngetWindow( OObject *obj, OOInst_AreaWindow *inst )
{
	if (inst->_RI.wi)
		{
		RestoreDC( inst->_PlainRP.DC, (ULONG)inst->_RI.SavedDC ); 
		ReleaseDC( inst->_RI.wi, inst->_PlainRP.DC );
		inst->_PlainRP.DC = NULL;
		inst->_RI.rp = NULL;

		close_req( obj, inst);
		close_window_safely( inst->_RI.wi );
		inst->_RI.wi = NULL;
		}
	inst->Installed = 0;
	block_other_wins( obj, inst, FALSE );
}

static OOBuf *make_window_title( OObject *obj, UBYTE *wintitle )
{
	OOInst_AreaWindow *inst = OOINST_DATA(obj->Class,obj);

	if (inst->BufTitle == NULL) 
		inst->BufTitle = OOBuf_Alloc( OOGlobalPool, 0, 4 );

	if (inst->BufTitle != NULL)
		{
		OObject *appl = OO_GetParentOfClass( obj, &Class_Application );
		UBYTE *apptitle = (UBYTE*) OO_GetAttr( appl, OOA_Title );

		OOBuf_Truncate( inst->BufTitle, 0 );
		if (apptitle != NULL || wintitle != NULL)
			{
			if (apptitle != NULL && wintitle != NULL) OOBuf_FormatV( &inst->BufTitle, 0, "%s - %s", apptitle, wintitle );
			else if (apptitle != NULL) OOBuf_FormatV( &inst->BufTitle, 0, "%s", apptitle );
			else if (wintitle != NULL) OOBuf_FormatV( &inst->BufTitle, 0, "%s", wintitle );
			}
		}
	return inst->BufTitle;
}

static ULONG AW_GetWindow( OObject *obj, OObject *gui, OOInst_AreaWindow *inst )
{
	OOInst_Area *ar = (OOInst_Area*)_DATA_AREA(obj);
	OOTagList *attrs = OOINST_ATTRS(obj->Class,obj);
	UBYTE *title = attrs[INDEX_OOA_Title].ti_String;

	inst->_RI.wi = NULL;
	inst->_RI.rq = NULL;
	inst->_RI.rp = NULL;

	block_other_wins( obj, inst, TRUE );

	AW_MakeWinStyles( inst, ar, title );
		
	if ((inst->_RI.wi = CreateWindowEx( 
							WS_EX_CONTROLPARENT,
							OOWINCLASSNAME, 
							make_window_title( obj, title ),
							inst->Styles,
							ar->_B.X, ar->_B.Y,
							ar->_B.W, ar->_B.H,
							NULL, NULL, 
							OOInstance, 
							obj )) != NULL)
		{
		SetWindowLong( inst->_RI.wi, GWL_USERDATA, (ULONG)obj );
		inst->_PlainRP.DC = GetDC( inst->_RI.wi );
		inst->_RI.SavedDC = SaveDC( inst->_PlainRP.DC );
		inst->_RI.rp = &inst->_PlainRP;
		
		SelectObject( inst->_PlainRP.DC, inst->_RI.Font );
		//SendMessage( inst->_RI.wi, WM_SETFONT, (WPARAM)inst->_RI.Font, MAKELPARAM(FALSE,0) );
		//ReleaseDC( inst->_RI.wi, inst->_PlainRP.DC );

		SelectPalette( inst->_PlainRP.DC, inst->_PlainRP.Palette->Palette, TRUE );
		UnrealizeObject( inst->_PlainRP.Palette->Palette );
		RealizePalette( inst->_PlainRP.DC );

		ShowWindow( inst->_RI.wi, OOCmdShow );
		UpdateWindow( inst->_RI.wi );
		return OOV_OK;
		}

	return OOV_ERROR;
}

static void AW_DrawFill( OOInst_AreaWindow *inst, OOInst_Area *ar, OORastPort *rp )
{
	if (inst->Flags & OOF_WIN_FILLED)
		{
		HDC dc = (rp == NULL) ? inst->_PlainRP.DC : rp->DC;
		SendMessage( inst->_RI.wi, WM_ERASEBKGND, (WPARAM)dc, 0 );
		//+++
		/*
		SWORD *pens = inst->_RI.di->dri_Pens;
		if (inst->Pattern) { SetABPenDrMd( inst->_RI.rp, pens[SHINEPEN], pens[BACKGROUNDPEN], JAM2 ); SetAfPt( inst->_RI.rp, (UWORD*)&inst->Pattern, 1 ); }
		else SetABPenDrMd( inst->_RI.rp, pens[BACKGROUNDPEN], pens[SHINEPEN], JAM2 );
		RectFill( inst->_RI.rp, ar->_Border.Left, ar->_Border.Top, ar->_B.W - ar->_Border.Right - 1, ar->_B.H - ar->_Border.Bottom - 1 );
		SetAfPt( inst->_RI.rp, 0, 0 );
		*/
		}
}

static void AW_WindowSizeVerify( OObject *obj, OOInst_AreaWindow *inst )
{
	// case OOM_AREA_STOP 
	OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_STOP, OOA_Flags, OOF_SAME_RI, TAG_END ); // Group stop
}

static void AW_WindowNewSize( OObject *obj, OOInst_AreaWindow *inst )
{
	OOInst_Area *ar = _DATA_AREA(obj);
	RECT rect;

//+++	WindowLimits( inst->_RI.wi, ar->_MM.MinW, ar->_MM.MinH, ar->_MM.MaxW, ar->_MM.MaxH );

	// case OOM_AREA_BOX 
	if (GetWindowRect( inst->_RI.wi, &rect ))
		{
		ar->_B.W = (SWORD)(rect.right - rect.left);
		ar->_B.H = (SWORD)(rect.bottom - rect.top);
		}
	OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_BOX, OOA_Flags, OOF_SAME_RI, OOA_OOBox, NULL, TAG_END );	// Group layout

	// case OOM_AREA_START 
	OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_START, OOA_Flags, OOF_SAME_RI, TAG_END ); // ajout des gadgets

	// case OOM_AREA_REFRESH 
	AW_DrawFill( inst, ar, NULL );														    // Group refresh
	if (OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_REFRESH, OOA_Flags, OOF_SAME_RI, OOA_RastPort, NULL, TAG_END ) == OOV_OK)
		{
//+++
/*
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
*/
		}
}

static void AW_ChangeWindow( OObject *obj, OOInst_AreaWindow *inst )
{
	OOInst_Area *ar = _DATA_AREA(obj);
	OObject *par;
	OOInst_Area *scarea;
	RECT rect;
	SWORD width, height;

	par = OO_GetParentOfClass( obj, &Class_AreaScreen );
	scarea = _DATA_AREA(par);

	OO_DoMethodV( obj, OOM_AREA_MINMAX, OOA_Flags, OOF_SELF_RI|OOF_SAME_RI, TAG_END );
	if (GetWindowRect( inst->_RI.wi, &rect ))
		{
		ar->_B.X = (SWORD)rect.left;
		ar->_B.Y = (SWORD)rect.top;
		width  = (SWORD)(rect.right - rect.left);
		height = (SWORD)(rect.bottom - rect.top);
		}
	if (ar->_B.X + ar->_B.W > scarea->_B.W) ar->_B.X = scarea->_B.W - ar->_B.W;
	if (ar->_B.Y + ar->_B.H > scarea->_B.H) ar->_B.Y = scarea->_B.H - ar->_B.H;

	if ((ar->_B.W == width) && (ar->_B.H == height))
		{
		AW_WindowNewSize( obj, inst );
		}
	else{
		// SetWindowPos() provoque l'appel de AW_WindowNewSize()
		SetWindowPos( inst->_RI.wi, NULL, ar->_B.X, ar->_B.Y, ar->_B.W, ar->_B.H, SWP_NOZORDER );
		}
}

static void begin_sizing( OObject *obj, OOInst_AreaWindow *inst )
{
	if (! (inst->Flags & OOF_WIN_SVERIFY))
		{
		inst->Flags |= OOF_WIN_SVERIFY;
		AW_WindowSizeVerify( obj, inst );
		}
}

static void end_sizing( OObject *obj, OOInst_AreaWindow *inst, OOInst_Area *ar )
{
	if (inst->Flags & OOF_WIN_SVERIFY)
		{
		OOInst_Area *ar = _DATA_AREA(obj);
		RECT rect;
		if (GetWindowRect( inst->_RI.wi, &rect ))
			{
			ar->AskedW = (SWORD)(rect.right - rect.left);
			ar->AskedH = (SWORD)(rect.bottom - rect.top);
			}
		inst->Flags &= ~OOF_WIN_SVERIFY;
		AW_WindowNewSize( obj, inst );
		}
}

static ULONG AW_HandleWindowMsg( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOEventMsg	*msg = attrlist[1].ti_Ptr; // OOA_EventMsg
	OOTagItem	*ti  = attrlist[3].ti_Ptr; // OOA_TagItem
	ULONG		rc = OOV_OK;
	OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_HandleGUI && attrlist[1].ti_Tag == OOA_EventMsg && attrlist[2].ti_Tag == OOA_TagList && attrlist[3].ti_Tag == OOA_TagItem);

	switch (msg->message)
		{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
			ti->ti_Tag = OOA_MousePos;
			ti->ti_Data = W2L((SWORD)msg->pt.x,(SWORD)msg->pt.y);
			ti++;
			break;

		case WM_ACTIVATE:
			ti->ti_Tag = OOA_WinActive; 
			ti->ti_Data = (msg->wParam == WA_INACTIVE) ? FALSE : TRUE; 
			ti++;
			break;

		case WM_GETMINMAXINFO:
			{
			OOInst_Area *ar = _DATA_AREA(obj);
			MINMAXINFO *mmi = (MINMAXINFO*)msg->lParam;
			mmi->ptMinTrackSize.x = ar->_MM.MinW;
			mmi->ptMinTrackSize.y = ar->_MM.MinH;
			mmi->ptMaxTrackSize.x = ar->_MM.MaxW;
			mmi->ptMaxTrackSize.y = ar->_MM.MaxH;
			rc = OOV_CANCEL;
			}
			break;

		//------------------------------------------

		case WM_SIZING:
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			RECT _wr, *dr = (RECT*)msg->lParam;
			if (GetWindowRect( inst->_RI.wi, &_wr ))
				if (! EqualRect( &_wr, dr ))
					begin_sizing( obj, inst );
				else rc = OOV_CANCEL;
			}
			break;

		case WM_SIZE:
			// if (msg->lParam != 0) // width & height of client area 
			end_sizing( obj, OOINST_DATA(cl,obj), _DATA_AREA(obj) );
			ti->ti_Tag = OOA_WinChange; ti->ti_Data = TRUE; ti++;
			if (((OOInst_AreaWindow*)OOINST_DATA(cl,obj))->Flags & OOF_WIN_SVERIFY)
				{
				OOASSERT( FALSE );
				}
			break;

		case WM_MOVE:
			ti->ti_Tag = OOA_WinChange; ti->ti_Data = TRUE; ti++;
			break;

		case WM_WINDOWPOSCHANGING:
			{
			/*
			WINDOWPOS *wp = (WINDOWPOS*)msg->lParam;
			if (! (wp->flags & SWP_NOSIZE))
				begin_sizing( obj, OOINST_DATA(cl,obj) );
			*/
			}
			break;

		case WM_WINDOWPOSCHANGED:
			{
			WINDOWPOS *wp = (WINDOWPOS*)msg->lParam;
			if (!(wp->flags & SWP_NOMOVE) || !(wp->flags & (SWP_NOSIZE)))
				{
				OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
				if (! (wp->flags & SWP_NOSIZE))
					{
					begin_sizing( obj, inst );
					end_sizing( obj, inst, _DATA_AREA(obj) );
					}
				if (! (wp->flags & SWP_NOMOVE))
					{
					RECT rect;
					if (GetWindowRect( inst->_RI.wi, &rect ))
						{
						OOInst_Area *ar = _DATA_AREA(obj);
						ar->_B.X = (SWORD)rect.left;
						ar->_B.Y = (SWORD)rect.top;
						}
					}
				ti->ti_Tag = OOA_WinChange; ti->ti_Data = TRUE; ti++;
				}
			if (((OOInst_AreaWindow*)OOINST_DATA(cl,obj))->Flags & OOF_WIN_SVERIFY)
				{
				OOASSERT( FALSE );
				}
			}
			break;

		//------------------------------------------

		case WM_KEYDOWN:
			ti->ti_Tag = OOA_KeyDown; ti->ti_Data = msg->wParam; ti++;
			break;
		case WM_KEYUP:
			ti->ti_Tag = OOA_KeyUp; ti->ti_Data = msg->wParam; ti++;
			break;

		//------------------------------------------

		case WM_LBUTTONDOWN  :	case WM_NCLBUTTONDOWN  :
			ti->ti_Tag = OOA_ButLeftDown; ti->ti_Data = TRUE; ti++;
			break;
		case WM_LBUTTONUP    :	case WM_NCLBUTTONUP    :
			ti->ti_Tag = OOA_ButLeftUp; ti->ti_Data = TRUE; ti++;
			break;
		case WM_MBUTTONDOWN  :	case WM_NCRBUTTONDOWN  :
			ti->ti_Tag = OOA_ButMidDown; ti->ti_Data = TRUE; ti++;
			break;
		case WM_MBUTTONUP    :	case WM_NCRBUTTONUP    :
			ti->ti_Tag = OOA_ButMidUp; ti->ti_Data = TRUE; ti++;
			break;
		case WM_RBUTTONDOWN  :	case WM_NCMBUTTONDOWN  :
			ti->ti_Tag = OOA_ButRightDown; ti->ti_Data = TRUE; ti++;
			break;
		case WM_RBUTTONUP    :	case WM_NCMBUTTONUP    :
			ti->ti_Tag = OOA_ButRightUp; ti->ti_Data = TRUE; ti++;
			break;

		//------------------------------------------

		case WM_PAINT:
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			RECT _R;
			if (GetUpdateRect( inst->_RI.wi, &_R, FALSE ))
				{
				OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_REFRESH, OOA_Flags, OOF_SAME_RI, OOA_RastPort, NULL, TAG_END );
				//+++ Mis en comm car ne redessine pas tout quand déplace une fenêtre par-dessus
				//+++ comme si le clip n'était pas assez actualisé ou que le fond était trop effacé
				/*
				PAINTSTRUCT _PS;
				HDC dc = inst->_PlainRP.DC;
				inst->_PlainRP.DC = BeginPaint( inst->_RI.wi, &_PS ); // sends WM_ERASEBKGND
				// BeginPaint sent it... SendMessage( inst->_RI.wi, WM_ERASEBKGND, (WPARAM)inst->_PlainRP.DC, 0 );
				OO_DoBaseMethodV( obj->Class, obj, OOM_AREA_REFRESH, OOA_Flags, OOF_SAME_RI, OOA_RastPort, &_RP, TAG_END );
				inst->_PlainRP.DC = dc;
				EndPaint( inst->_RI.wi, &_PS );
				*/
				}
			}
			ti->ti_Tag = OOA_WinRefreshAsk; ti->ti_Data = TRUE; ti++;
			break;

		//------------------------------------------

		case WM_COMMAND: 
			switch (LOWORD(msg->wParam)) 
				{
				default: 
					break;
	            } 
            break;

		//------------------------------------------

		case WM_CLOSE:
			{
			OOInst_AreaWindow *inst = OOINST_DATA(cl,obj);
			if (inst->Flags & OOF_WIN_CLOSEQUIT)
				{
				PostQuitMessage(0);
				rc = OOV_CANCEL;
				}
			else{
				ti->ti_Tag = OOA_WinCloseAsk; ti->ti_Data = TRUE; ti++; 
				rc = OOV_CANCEL;
				}
			}
			break;

        case WM_CREATE:
			break;

        case WM_DESTROY:
			break;

        default: 
			break;
		} 

/*
	switch (imsg->Class)
		{
		case IDCMP_INTUITICKS:
			ti->ti_Tag = OOA_Second10; ti->ti_Data = TRUE; ti++;
			break;

		case IDCMP_RAWKEY:
			inst->Key.Code = ie.ie_Code = imsg->Code;
			inst->Key.Qual = ie.ie_Qualifier = imsg->Qualifier;
			ie.ie_EventAddress = (APTR *) *((ULONG *)imsg->IAddress);
			inst->Key.Length = MapRawKey( &ie, inst->Key.Buffer, 7, 0 );
			inst->Key.Key = inst->Key.Buffer[0];
			ti->ti_Tag = (imsg->Code & IECODE_UP_PREFIX) ? OOA_KeyUp : OOA_KeyDown; ti->ti_Data = (ULONG)&inst->Key; ti++;
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

		case IDCMP_GADGETHELP:
			break;
		}
*/
	OOResult = (OORESULT)ti;
    return rc;
}

static ULONG AW_SetPointer( OOInst_AreaWindow *inst, ULONG pointer )
{
	if (inst->CurrentPtr == pointer) return OOV_NOCHANGE;

	inst->CurrentPtr = pointer;
	if (pointer && (inst->Installed != inst->CurrentPtr) && inst->_RI.wi)
		{
		switch (pointer)
			{
			case OOV_PTR_SYS:
//+++				{ OOTagItem tl[] = { TAG_END };
//+++				SetWindowPointerA( inst->_RI.wi, tl );
//+++				}
				break;

			case OOV_PTR_WAIT:
//+++				{ OOTagItem tl[] = { WA_BusyPointer, TRUE, WA_PointerDelay, TRUE, TAG_END };
//+++				SetWindowPointerA( inst->_RI.wi, tl );
//+++				}
				break;

			case OOV_PTR_NULL:
//+++				SetPointer( inst->_RI.wi, &NullPointer[0], 1, 16, 0, 0 );
				break;

			default:
//+++				SetPointer( inst->_RI.wi, (APTR)pointer, 1, 16, 0, 0 );
				break;
			}
		inst->Installed = inst->CurrentPtr;
		}
	return OOV_CHANGED;
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
	ULONG flag = 0;
	UBYTE *title = NULL;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_TitleIndex:
				sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
				if (OOPrv_MSetBounceV( sgp, OOA_Title, OOStr_Get( sgp->ExtAttr->ti_Data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_Title:	
				title = sgp->ExtAttr->ti_String; 
				break;

			case OOA_Pattern:
				if (inst->Pattern != data) { change = OOV_CHANGED; inst->Pattern = data; }
				break;

			case OOA_PtrCurrent: if (data) change = AW_SetPointer( inst, data		  ); break;
			case OOA_PtrNull   : if (data) change = AW_SetPointer( inst, OOV_PTR_NULL ); break;
			case OOA_PtrWait   : if (data) change = AW_SetPointer( inst, OOV_PTR_WAIT ); break;
			case OOA_PtrSys    : if (data) change = AW_SetPointer( inst, OOV_PTR_SYS  ); break;

			case OOA_Sync			: flag = OOF_WIN_SYNC		; break;
			case OOA_Filled			: flag = OOF_WIN_FILLED		; break;
			case OOA_Borderless		: flag = OOF_WIN_BORDERLESS	; break;
			case OOA_CloseQuit		: flag = OOF_WIN_CLOSEQUIT	; break;
			case OOA_WinCtlSize		: flag = OOF_WCTL_SIZE 		; break;
			case OOA_WinCtlDrag		: flag = OOF_WCTL_DRAG 		; break;
			case OOA_WinCtlDepth	: flag = OOF_WCTL_DEPTH		; break;
			case OOA_WinCtlClose	: flag = OOF_WCTL_CLOSE		; break;
			case OOA_WinCtlZoom		: flag = OOF_WCTL_ZOOM 		; break;
			case OOA_WinCtlIcon		: flag = OOF_WCTL_ICON 		; break;
			case OOA_WinCtlInsRight	: flag = OOF_WCTL_INSRIGHT	; break;
			case OOA_WinCtlInsBot	: flag = OOF_WCTL_INSBOT	; break;

			case OOA_FontName: if (OOTag_SetString( sgp->Attr, sgp->ExtAttr->ti_String ) != NULL) change = OOV_CHANGED; break;
			}

		if (flag)
			{
			if (data == TRUE)
				 { change = (inst->Flags & flag) ? OOV_NOCHANGE : OOV_CHANGED ; inst->Flags |= flag; }
			else { change = (inst->Flags & flag) ? OOV_CHANGED	: OOV_NOCHANGE; inst->Flags &= ~flag; }
			sgp->Attr->ti_Data = data;
			}

		if (title != NULL)
			{
			sgp->Attr->ti_Data = data;
			if (inst->_RI.wi != NULL)
				{
				UBYTE buf[256];
				BOOL do_it = TRUE;
				if (GetWindowText( inst->_RI.wi, buf, 255 ) > 0)
					if (strcmp( buf, title ) == 0)
						do_it = FALSE;
				if (do_it == TRUE)
					{
					SetWindowText( inst->_RI.wi, (APTR)make_window_title( sgp->Obj, title ) );
					change = OOV_CHANGED;
					}
				}
			}
		}
	else{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_Pattern   : *((ULONG*)data) = inst->Pattern 	; break;
			case OOA_PtrCurrent: *((ULONG*)data) = inst->CurrentPtr	; break;
			case OOA_WindowHandle: *((ULONG*)data) = (ULONG)inst->_RI.wi	; break;

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
	{ OOA_CloseQuit		, FALSE			, "CloseQuit"	  , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_Title 		, (ULONG)NULL	, "Title"		  , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_TitleIndex    , 0				, "TitleIndex"    , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_FontName		, (ULONG)NULL	, "FontName"	  , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_FontHeight	, 0				, "FontHeight"	  , NULL	  , OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_FontHeightP100, 100			, "FontHeightP100", NULL	  , OOF_MNEW | OOF_MSET	|				OOF_MAREA	},
	{ OOA_FontBold		, FALSE			, "FontBold"	  , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontItalic	, FALSE			, "FontItalic"    , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontUnderline	, FALSE			, "FontUnderline" , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontFixed		, FALSE			, "FontFixed"     , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_FontVariable	, TRUE			, "FontVariable"  , NULL	  , OOF_MNEW | OOF_MSET | OOF_MGET |		OOF_MAREA },
	{ OOA_Pattern		, 0				, "Pattern" 	  , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_Filled		, TRUE			, "Filled"		  , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET |	OOF_MAREA	},
	{ OOA_PtrCurrent	, (ULONG)NULL	, "PtrCurrent"	  , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_PtrNull		, (ULONG)NULL	, "PtrNull" 	  , SetGet_All, OOF_MNEW | OOF_MSET								},
	{ OOA_PtrWait		, (ULONG)NULL	, "PtrWait" 	  , SetGet_All, OOF_MNEW | OOF_MSET								},
	{ OOA_PtrSys		, (ULONG)NULL	, "PtrSys"		  , SetGet_All, OOF_MNEW | OOF_MSET								},
	{ OOA_Sync			, FALSE			, "Sync"		  , SetGet_All, OOF_MNEW | OOF_MSET	| OOF_MGET					},
	{ OOA_Borderless	, FALSE			, "Borderless"	  , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlIcon	, TRUE			, "WinCtlIcon"	  , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlZoom	, TRUE			, "WinCtlZoom"	  , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlDepth	, TRUE			, "WinCtlDepth"   , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlSize	, TRUE			, "WinCtlSize"	  , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlDrag	, TRUE			, "WinCtlDrag"	  , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlClose	, TRUE			, "WinCtlClose"   , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlInsRight, TRUE			, "WinCtlInsRight", SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_WinCtlInsBot	, TRUE			, "WinCtlInsBot"  , SetGet_All, OOF_MNEW 			| OOF_MGET					},
	{ OOA_MousePos		, 0				, "MousePos"	  , SetGet_All,								OOF_MNOT 	},
	{ OOA_Second10		, 0				, "Second10"	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_ButLeftDown	, 0				, "ButLeftDown"   , SetGet_All,									OOF_MNOT 	},
	{ OOA_ButLeftUp 	, 0				, "ButLeftUp"	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_ButMidDown	, 0				, "ButMidDown"	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_ButMidUp		, 0				, "ButMidUp"	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_ButRightDown	, 0				, "ButRightDown"  , SetGet_All,									OOF_MNOT 	},
	{ OOA_ButRightUp	, 0				, "ButRightUp"	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_KeyDown		, 0				, "KeyDown" 	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_KeyUp 		, 0				, "KeyUp"		  , SetGet_All,									OOF_MNOT 	},
	{ OOA_WinActive 	, 0				, "WinActive"	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_WinCloseAsk	, 0				, "WinCloseAsk"   , SetGet_All,									OOF_MNOT 	},
	{ OOA_WinRefreshAsk , 0				, "WinRefreshAsk" , SetGet_All,									OOF_MNOT 	},
	{ OOA_WinChange 	, 0				, "WinChange"	  , SetGet_All,									OOF_MNOT 	},
	{ OOA_WindowHandle 	, 0				, "OOA_WindowHandle", SetGet_All,					  OOF_MGET			 	},
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
