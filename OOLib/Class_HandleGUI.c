/*****************************************************************
 *
 *		 Project:	 OO
 *		 Function:	 Manage all GUI elements
 *
 *		 Created:		 Jean-Michel Forgeas
 *
 *		 This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#ifdef AMIGA
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#endif

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_pool.h"
#include "oo_pictures.h"

#include "oo_gui_prv.h"

#ifdef _WIN32
#include <commctrl.h>
#endif


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

#define IDBLOCKNUM 20
static struct _IdList { OObject *Obj; ULONG Flags; };
static struct _IdList * IdList = NULL;

#ifdef _WIN32
static OOInst_HandleGUI *Inst = NULL;
static HBRUSH BackBrush = NULL;

ULONG HG_RegisterWindowClass( OOClass *cl, OObject *obj );
void  HG_UnregisterWindowClass( OOInst_HandleGUI *inst );
ULONG APIENTRY HG_WindowProc( HWND win, UINT msgid, UINT wparam, LONG lparam ); 
#endif


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_HandleGUI( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		case OOM_NEW:
			{
			OOTagItem init_taglist[] = { OOA_EventMask, OOF_EVENT_GUI, TAG_END };

			if (attrlist == NULL)
				obj = (OObject*) OO_DoBaseMethod( cl, obj, method, init_taglist );
			else{
				OOTagItem *join_tag = OOTag_Join( attrlist, init_taglist );
				obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist );
				OOTag_Split( join_tag );
				}
			if (obj != NULL)
				{
				OOInst_HandleGUI *inst = Inst = OOINST_DATA(cl,obj);
				BOOL ok = FALSE;
				// Init instance datas
				inst->_RI.Palette	= &OOPalette256;
				inst->SelectedObj	= NULL;
				inst->SyncObj		= NULL;
				if ((IdList = OOPool_Alloc( OOGlobalPool, IDBLOCKNUM*sizeof(struct _IdList) )) != NULL)
					{
					// Init default attrs & change defaults with new values
					if (OOPrv_MNew( cl, obj, attrlist ) != OOV_ERROR)
						ok = TRUE;
					}

				if (ok == FALSE)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; Inst = NULL; }
				}
			}
			return (ULONG)obj;

		case OOM_DELETE:
			{
			ULONG rc;
			OO_DoMethodV( obj, OOM_HIDE, OOA_Object, obj, TAG_END );
			rc = OO_DoBaseMethod( cl, obj, method, attrlist );
			HG_UnregisterWindowClass( OOINST_DATA(cl,obj) );
			IdList = OOPool_Free( OOGlobalPool, IdList ); 
			return rc;
			}

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
			OOInst_HandleGUI *inst = OOINST_DATA(cl,obj);
#ifdef AMIGA
			struct MsgPort *port = _DATA_SYSEVENT(obj)->Port;
			struct IntuiMessage *imsg;
			while (imsg = (struct IntuiMessage *) GetMsg( port ))
				{
				/*if (imsg->Class == IDCMP_MOUSEMOVE)
					{
					struct Node *node, *succ;
					Forbid();
					for (node=port->mp_MsgList.lh_Head; succ=node->Succ; node=succ)
						{
						if (((struct IntuiMessage *)node)->IDCMPWindow == w)
							{
							Remove( msg ); ReplyIntuiMsg( msg );
							if (msg == umsg)
								{
								obj->UIK->IntuiMsg = 0;
								}
							}
						}
					Permit();
					}
				else*/{
					//OO_DoNotify( obj, 0, OOA_IDCMPEvent, imsg, TAG_END ); 
					if (DispatchGUIMsg( inst, imsg ))
						ReplyMsg( imsg );
					}

				// Faire ça après chaque message pour éviter de le faire après un resize, ou trop tard 
				if (inst->Flags & OOF_GUI_AREACHANGED)
					{
					inst->Flags &= ~OOF_GUI_AREACHANGED;
					OO_DoChildrenMethodV( 0, obj, OOM_AREA_CHANGED, OOA_Flags, 0, OOA_Object, NULL, TAG_END );
					}
				}
#endif // AMIGA
#ifdef _WIN32
			while (PeekMessage( &inst->CurrentMsg, NULL, 0, 0, PM_NOREMOVE ))
				{
				// If not a window event, let it for the application (signals watcher)
				if (inst->CurrentMsg.hwnd == NULL)
					break;

				// Remove and process the message if it is for a window
				PeekMessage( &inst->CurrentMsg, (HWND)NULL, inst->CurrentMsg.message, inst->CurrentMsg.message, PM_REMOVE ); 
				// If WM_QUIT, generates a Ctrl-C event
				if (inst->CurrentMsg.message == WM_QUIT)
					OOASSERT( FALSE ); // Should be catched into the signals watcher

				// process this message
				OOASSERT( inst->CurrentMsg.hwnd != NULL );
				//if (! TranslateAccelerator( win, accels, &inst->CurrentMsg ))
					{
					TranslateMessage( &inst->CurrentMsg );

					// If the msg parameter points to a WM_TIMER message and the 
					//	lparam parameter of the WM_TIMER message is not NULL, 
					//	lparam points to a function that is called instead of 
					//	the window procedure.
					DispatchMessage( &inst->CurrentMsg ); //  Calls window proc

					// Faire ça après chaque message pour éviter de le faire après un resize, ou trop tard 
					if (inst->Flags & OOF_GUI_AREACHANGED)
						{
						inst->Flags &= ~OOF_GUI_AREACHANGED;
						OO_DoChildrenMethodV( 0, obj, OOM_AREA_CHANGED, OOA_Flags, 0, OOA_Object, NULL, TAG_END );
						}
					}
				}
#endif // _WIN32
			}
			break;

		//--------------- 
		case OOM_TREEPRESENT:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_ParentOffs && attrlist[1].ti_Tag == OOA_VarBase && attrlist[2].ti_Tag == OOA_TreeItem );
			{
			OObject *treeobj, *parent;
			ULONG		parentoffs = attrlist[0].ti_Data; // OOA_ParentOffs
			APTR		varbase    = (APTR) attrlist[1].ti_Data; // OOA_VarBase
			OOTreeItem *treeitem   = (OOTreeItem*) attrlist[2].ti_Data; // OOA_TreeItem
			if (varbase == NULL || treeitem == NULL)
				return OOV_ERROR;

			if (parent = *((OObject**)((ULONG)varbase + parentoffs)))
				if (treeobj = OO_NewTree( parent, varbase, treeitem ))
					if (OO_DoMethodV( obj, OOM_PRESENT, OOA_Object, treeobj, TAG_END )) 
						return (ULONG)treeobj;
					else OO_DoMethod( treeobj, OOM_DELETE, NULL );
			return (ULONG)NULL;
			}

		case OOM_PRESENT:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Object );
			{
			OObject *present_obj = (OObject*) attrlist[0].ti_Ptr; // OOA_Object
			ULONG rc = OOV_OK;

			if (present_obj != obj)
				{
				ULONG gui, ri, do_start;
				OOInst_Area *present_ar = _DATA_AREA(present_obj);
				OObject *parent = OOGETPARENT(present_obj);

				OOASSERT( parent != NULL );
				if (parent != NULL)
					{
					OO_GetAttrsV( parent, OOA_HandleGUI, &gui, OOA_RenderInfo, &ri, TAG_END );

					if ((rc = OO_DoMethodV( present_obj, OOM_AREA_SETUP, OOA_Flags, 0, OOA_HandleGUI, gui, OOA_RenderInfo, ri, TAG_END )) == OOV_OK)
						{
						if (! (do_start = (present_ar->Flags & OOF_ANEWRENDERINFO) ? 1 : 0))
							{
							/*--- L'objet n'est pas un nouveau RenderInfo, donc on cherche le
							 *	  parent qui en est un et on lui demande si il veut faire
							 *	  lui-même son resize, etc... Sinon on le fait pour l'objet.
							 */
							OO_DoMethodV( obj, OOM_AREA_CHANGED, OOA_Flags, 0, OOA_Object, present_obj, TAG_END );
							}
						if (do_start)
							{
							OO_DoMethodV( present_obj, OOM_AREA_MINMAX, OOA_Flags, 0, TAG_END );
							OO_DoMethodV( present_obj, OOM_AREA_BOX, OOA_Flags, 0, OOA_OOBox, NULL, TAG_END );
							if ((rc = OO_DoMethodV( present_obj, OOM_AREA_START, OOA_Flags, 0, TAG_END )) == OOV_OK)
								rc = OO_DoMethodV( present_obj, OOM_AREA_REFRESH, OOA_Flags, 0, OOA_RastPort, NULL, TAG_END );
							}
						}
					}
				}
			else{
				OOInst_HandleGUI *inst = OOINST_DATA(cl,obj);
				if ((rc = OO_DoMethodV( present_obj, OOM_AREA_SETUP, OOA_Flags, 0, OOA_HandleGUI, (ULONG)obj, OOA_RenderInfo, (ULONG)&inst->_RI, TAG_END )) == OOV_OK)
					{
					OO_DoMethodV( present_obj, OOM_AREA_MINMAX, OOA_Flags, 0, TAG_END );
					OO_DoMethodV( present_obj, OOM_AREA_BOX, OOA_Flags, 0, OOA_OOBox, NULL, TAG_END );
					if ((rc = OO_DoMethodV( present_obj, OOM_AREA_START, OOA_Flags, 0, TAG_END )) == OOV_OK)
						rc = OO_DoMethodV( present_obj, OOM_AREA_REFRESH, OOA_Flags, 0, OOA_RastPort, NULL, TAG_END );
					}
				}
			return rc;
			}

		case OOM_HIDE:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Object );
			{
			OObject *hide_obj = (OObject*) attrlist[0].ti_Ptr; // OOA_Object
			if (hide_obj != NULL)
				{
				OO_DoMethodV( hide_obj, OOM_AREA_STOP, OOA_Flags, 0, TAG_END );
				OO_DoMethodV( hide_obj, OOM_AREA_CLEANUP, OOA_Flags, 0, TAG_END );
				if (hide_obj != obj)
					{
					OOInst_Area *hide_ar = _DATA_AREA(hide_obj);
					ULONG do_size;
					if (! (do_size = (hide_ar->Flags & OOF_ANEWRENDERINFO) ? 1 : 0))
						{
						/*--- L'objet n'est pas un nouveau RenderInfo, donc on cherche le
						 *	  parent qui en est un et on lui demande si il veut faire
						 *	  un resize, etc...
						 */
						OO_DoMethodV( obj, OOM_AREA_CHANGED, OOA_Flags, 0, OOA_Object, hide_obj, TAG_END );
						}
					}
				}
			}
			break;

		//--------------- Pour utiliser PRESENT et HIDE sur handlegui lui-même 
		case OOM_AREA_SETUP:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_HandleGUI && attrlist[2].ti_Tag == OOA_RenderInfo );
			if (HG_RegisterWindowClass( cl, obj ) == OOV_ERROR) return OOV_ERROR;
			return OO_DoChildrenMethod( OOF_TESTCHILD, obj, method, attrlist );
		case OOM_AREA_MINMAX:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
			return OO_DoChildrenMethod( 0, obj, method, attrlist );
		case OOM_AREA_BOX:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_OOBox );
			return OO_DoChildrenMethod( 0, obj, method, attrlist );
		case OOM_AREA_START:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
			return OO_DoChildrenMethod( OOF_TESTCHILD, obj, method, attrlist );
		case OOM_AREA_REFRESH:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_RastPort );
			return OO_DoChildrenMethod( 0, obj, method, attrlist );

		case OOM_AREA_STOP:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
			OO_DoChildrenMethod( 0, obj, method, attrlist );
			break;
		case OOM_AREA_CLEANUP:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags );
			OO_DoChildrenMethod( 0, obj, method, attrlist );
			break;

		case OOM_AREA_CHANGED:
			OOASSERT( attrlist != NULL && attrlist[1].ti_Tag == OOA_Object );
			{
			OObject *riobj = (OObject*) attrlist[1].ti_Ptr; // OOA_Object
			for (; riobj != NULL; riobj = OOGETPARENT(riobj))
				{
				if (_DATA_AREA(riobj)->Flags & OOF_ANEWRENDERINFO)
					{
					OOInst_HandleGUI *inst = OOINST_DATA(cl,obj);
					_DATA_AREA(riobj)->StatusFlags |= OOF_AREA_CHANGED;
					inst->Flags |= OOF_GUI_AREACHANGED;
					break;
					}
				}
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

#ifdef _WIN32
ULONG HG_RegisterWindowClass( OOClass *cl, OObject *obj )
{
	OOInst_HandleGUI *inst = OOINST_DATA(cl,obj);
	ULONG toto;
	if (inst->RegisteredClass == 0)
		{
		WNDCLASSEX wndclass;
		OObject *appl;
		ULONG ind_icon;
		OOPicItem *pi;
		HICON icon32 = NULL;
		HICON icon16 = NULL;
		PALETTEENTRY *pe = &Inst->_RI.Palette->PaletteEntries[OOCOLORINDEX(Inst->_RI.Palette,OOV_COLOR_BACK)];

		BackBrush = CreateSolidBrush( RGB(pe->peRed,pe->peGreen,pe->peBlue) );

		if ((appl = OO_GetParentOfClass( obj, &Class_Application)) != NULL)
			if ((ind_icon = OO_GetAttr( appl, OOA_ImageIndex )) != 0)
				if ((pi = OOPic_GetItem( ind_icon )) != NULL)
					{
					icon32 = LoadImage( OOInstance, pi->Name, IMAGE_ICON, 32, 32, 0);
					icon16 = LoadImage( OOInstance, pi->Name, IMAGE_ICON, 16, 16, 0);
					}
		wndclass.cbSize        = sizeof(wndclass);
		wndclass.style         = CS_OWNDC;
		wndclass.lpfnWndProc   = HG_WindowProc;
		wndclass.cbClsExtra    = 0;
		wndclass.cbWndExtra    = 0;
		wndclass.hInstance     = OOInstance;
		wndclass.hIcon         = icon32;
		wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
		wndclass.hbrBackground = BackBrush;
		wndclass.lpszMenuName  = NULL;
		wndclass.lpszClassName = OOWINCLASSNAME;
		wndclass.hIconSm       = icon16;
		if ((inst->RegisteredClass = RegisterClassEx( &wndclass )) == 0) goto END_ERROR;
		}
	return OOV_OK;

  END_ERROR:
	toto = GetLastError();
	return OOV_ERROR;
}

void HG_UnregisterWindowClass( OOInst_HandleGUI *inst )
{
	if (inst->RegisteredClass != 0)
		{
		UnregisterClass( OOWINCLASSNAME, OOInstance );
		inst->RegisteredClass = 0;
		DeleteObject( BackBrush );
		BackBrush = NULL;
		}
}
#endif

//---------------------------------------------------

ULONG OObjId_Obtain( OObject *obj, ULONG flags )
{
  ULONG storenum, i;
  APTR ptr;

	storenum = OOPool_GetLength( IdList ) / sizeof(struct _IdList);
	for (i=1; i < storenum; i++)
		{ if (IdList[i].Obj == NULL) break; }

	if (i >= storenum)
		{
		if (! (ptr = OOPool_Resize( OOGlobalPool, IdList, (storenum+IDBLOCKNUM)*sizeof(struct _IdList) ))) return 0;
		IdList = ptr;
		}

	IdList[i].Obj = obj;
	IdList[i].Flags = flags;
	return i;
}

OObject *OObjId_GetObj( ULONG gadid )
{
	if (gadid > (OOPool_GetLength(IdList) / sizeof(struct _IdList))) return NULL;
	return IdList[gadid].Obj;
}

ULONG OObjId_GetFlags( ULONG gadid )
{
	if (gadid > (OOPool_GetLength(IdList) / sizeof(struct _IdList))) return 0;
	return IdList[gadid].Flags;
}

void OObjId_Release( ULONG gadid )
{
	if (gadid > (OOPool_GetLength(IdList) / sizeof(struct _IdList))) return;
	IdList[gadid].Obj = NULL;
	IdList[gadid].Flags = 0;
}

#ifdef AMIGA
static struct IntuiMessage *DispatchGUIMsg( OOInst_HandleGUI *inst, struct IntuiMessage *imsg )
{
  struct Window *w = imsg->IDCMPWindow;
  OObject *wobj = (OObject*)w->UserData;
  ULONG ok = 1, class = imsg->Class;

	/* Sur Amiga, le fait de poser un requester est plus efficace que cette
	 * ligne de test seule. On utilise donc les deux façons à la fois pour
	 * rester le plus compatible possible.
	 */
	if (inst->SyncObj && (inst->SyncObj != wobj)) // En cas de fenêtre synchrone ouverte, il faut 
		{									  // ignorer presque tous les évènements des autres 
		switch (class)
			{
			case IDCMP_SIZEVERIFY:
			case IDCMP_NEWSIZE:
			case IDCMP_CHANGEWINDOW:
			case IDCMP_REFRESHWINDOW: ok = 1; break;

			default: ok = 0; break;
			}
		}

	if (ok)
		{
		UWORD code = imsg->Code;
		OOInst_AreaWindow *wid = (OOInst_AreaWindow*)OOINST_DATA(&Class_AreaWindow,wobj);
		OOTagItem attrlist[10], *ti = attrlist;
		OObject *destobj;

		if (wid->GTImsg = GT_FilterIMsg( imsg ))
			{
			ti->ti_Tag = OOA_EventMsg; ti->ti_Data = (ULONG)imsg; ti++;
			if (! (destobj = inst->SelectedObj))
				{
				if (w->FirstRequest) destobj = (OObject*)w->FirstRequest->ImageBMap;
				else destobj = (OObject*)w->UserData;
				}
			switch (class)
				{
				case IDCMP_MOUSEBUTTONS:
					if (code == SELECTUP) { if (inst->SelectedObj != NULL) inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; inst->SelectedObj = NULL; }
					break;

				case IDCMP_GADGETDOWN:
					if (inst->SelectedObj != NULL) inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; inst->SelectedObj = NULL;
					destobj = inst->SelectedObj = (OObject*)((struct Gadget *)imsg->IAddress)->UserData;
					inst->SelectedObj->Flags |= OOF_BREAKNOTIFY;
					break;

				case IDCMP_GADGETUP:
					destobj = (OObject*)((struct Gadget *)imsg->IAddress)->UserData;
					if (inst->SelectedObj != NULL) inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; inst->SelectedObj = NULL;
					break;

				case IDCMP_ACTIVEWINDOW:
				case IDCMP_INACTIVEWINDOW:
				case IDCMP_REQSET:
				case IDCMP_REQCLEAR:
				case IDCMP_MENUPICK:
				case IDCMP_MENUVERIFY:
				case IDCMP_MENUHELP:
				case IDCMP_SIZEVERIFY:
				case IDCMP_NEWSIZE:
				case IDCMP_CLOSEWINDOW:
				case IDCMP_CHANGEWINDOW:
				case IDCMP_REFRESHWINDOW:
					destobj = (OObject*)w->UserData;
					if (inst->SelectedObj != NULL) inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; inst->SelectedObj = NULL;
					break;

				case IDCMP_IDCMPUPDATE:
					{
					OOTagItem *mti; OObject *src;
					if (mti = OOTag_FindTagItem( GA_ID, (OOTagItem*)imsg->IAddress ))
						if (src = gadid2obj( mti->ti_Data )) destobj = src;
					}

				default:
					break;
				}

			if (destobj)
				{
				OO_DoMethodV( destobj, OOM_HANDLEEVENT, 
											OOA_HandleGUI	, inst		 , 
											OOA_EventMsg	, wid->GTImsg, 
											OOA_TagList		, attrlist	 , 
											OOA_TagItem		, ti		 , 
											TAG_END );
				ti = (OOTagItem*)OOResult; ti->ti_Tag = TAG_END;
				if (OO_DoNotify( destobj, ..., attrlist ) == -1) // exécution de OOM_DISPOSE 
					if (destobj == wobj) return(0); // exécution de OOM_DISPOSE sur window 
				}

			if (wid->GTImsg) GT_PostFilterIMsg( wid->GTImsg ); else imsg = 0;
			wid->GTImsg = 0;
			}
		}
	return( imsg );
}
#endif // AMIGA

#ifdef _WIN32

static void install_palette( OObject *obj )
{
	OORenderInfo *ri = (OORenderInfo*) OO_GetAttr( obj, OOA_RenderInfo );
	OOPalette *palette;
	if (ri != NULL && (palette = ri->rp->Palette) != NULL && palette->Palette != NULL)
		{
		SelectPalette( ri->rp->DC, palette->Palette, TRUE );
		// UnrealizeObject(  );
		RealizePalette( ri->rp->DC );
		}
}

ULONG APIENTRY HG_WindowProc( HWND w, UINT msgid, UINT wparam, LONG lparam ) 
{ 
	OObject *wobj = (OObject*) GetWindowLong( w, GWL_USERDATA );
	ULONG ok = 1;
	ULONG rc = OOV_OK; // Execute default window proc
	MSG _tmpmsg = { w, msgid, wparam, lparam }; 
	_tmpmsg.time = Inst->CurrentMsg.time;
	_tmpmsg.pt = Inst->CurrentMsg.pt;

/*
	// Sur Amiga, le fait de poser un requester est plus efficace que cette
	// ligne de test seule. On utilise donc les deux façons à la fois pour
	// rester le plus compatible possible.
	if (Inst->SyncObj && (Inst->SyncObj != wobj))	// En cas de fenêtre synchrone ouverte, il faut 
		{											// ignorer presque tous les évènements des autres 
		switch (msgid)
			{
			case WM_SIZING:
			case WM_SIZE:
			case WM_MOVING:
			case WM_MOVE:
			case WM_WINDOWPOSCHANGING:
			case WM_WINDOWPOSCHANGED:
			case WM_PAINT: 
				ok = 1; 
				break;

			default: 
				ok = 0; 
				break;
			}
		}
*/
	if (ok)
		{
		OOTagItem attrlist[10], *ti = attrlist;
		OObject *destobj;
		ULONG i;
		for (i=0; i < 10; i++) attrlist[i].ti_Tag32 = 0; // Met les flags IGNORE et TMP à 0

		ti->ti_Tag = OOA_EventMsg; ti->ti_Ptr = &_tmpmsg; ti++;

		if ((destobj = Inst->SelectedObj) == NULL)
			destobj = wobj;

		switch (msgid) 
			{
			case WM_LBUTTONDOWN  :	case WM_NCLBUTTONDOWN  :
			case WM_MBUTTONDOWN  :	case WM_NCRBUTTONDOWN  :
			case WM_RBUTTONDOWN  :	case WM_NCMBUTTONDOWN  :
				SetFocus( w );
				if (Inst->SelectedObj != NULL) { Inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; Inst->SelectedObj = NULL; }
				break;

			case WM_LBUTTONUP    :	case WM_NCLBUTTONUP    :
			case WM_RBUTTONUP    :	case WM_NCRBUTTONUP    :
			case WM_MBUTTONUP    :	case WM_NCMBUTTONUP    :
				if (Inst->SelectedObj != NULL) { Inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; Inst->SelectedObj = NULL; }
				break;

			case WM_HSCROLL:
			case WM_VSCROLL:
				{
				UWORD scrollcode = LOWORD(wparam);
				HWND ctrl_win = (HWND)lparam;
				if (ctrl_win != NULL)
					{
					ULONG id;
					if (Inst->SelectedObj != NULL) { Inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; Inst->SelectedObj = NULL; }
					if ((id = GetWindowLong( ctrl_win, GWL_ID )) != 0)
						if ((destobj = OObjId_GetObj( id )) != NULL)
							if (scrollcode != SB_ENDSCROLL && scrollcode != SB_THUMBPOSITION)
								{
								Inst->SelectedObj = destobj;
								Inst->SelectedObj->Flags |= OOF_BREAKNOTIFY;
								}
					}
				}
				break;

			case WM_COMMAND:
				{
				UWORD notifycode = HIWORD(wparam);
				HWND ctrl_win = (HWND)lparam;
				if (ctrl_win != NULL)
					{
					ULONG id;
					if (Inst->SelectedObj != NULL) { Inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; Inst->SelectedObj = NULL; }
					if ((id = GetWindowLong( ctrl_win, GWL_ID )) != 0)
						if ((destobj = OObjId_GetObj( id )) != NULL)
							{
							switch (notifycode)
								{
								case NM_KILLFOCUS:
									break;

								default:
									Inst->SelectedObj = destobj;
									Inst->SelectedObj->Flags |= OOF_BREAKNOTIFY;
									break;
								}
							}
					}
				}
			break;

			case WM_PAINT: 
			case WM_ACTIVATE:
			case WM_SIZING:
			case WM_SIZE:
			case WM_MOVING:
			case WM_MOVE:
			case WM_WINDOWPOSCHANGING:
			case WM_WINDOWPOSCHANGED:
			case WM_CLOSE:
			case WM_CREATE:
			case WM_DESTROY:
				destobj = wobj;
				if (Inst->SelectedObj != NULL) Inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; Inst->SelectedObj = NULL;
				break;
			/*
			case IDCMP_IDCMPUPDATE:
				{
				OOTagItem *mti; OObject *src;
				if (mti = OOTag_FindTagItem( GA_ID, (OOTagItem*)imsg->IAddress ))
					if (src = gadid2obj( mti->ti_Data )) destobj = src;
				}
			*/

			case WM_QUERYNEWPALETTE:
				install_palette( wobj );
				destobj = NULL; // Pas de notification
				break;
			/*
			case WM_CLOSE:
				destobj = wobj;
				break;
			case WM_CREATE:
				destobj = wobj;
				break;
			case WM_DESTROY:
				destobj = wobj;
				break;
			*/
			default:
				break;
			}

		if (destobj != NULL)
			{
			if ((rc = OO_DoMethodV( destobj, OOM_HANDLEEVENT,
										OOA_HandleGUI	, Inst		, 
										OOA_EventMsg	, &_tmpmsg	, 
										OOA_TagList		, attrlist	, 
										OOA_TagItem		, ti		, 
										TAG_END )) != OOV_ERROR)
				{
				ti = (OOTagItem*)OOResult; 
				ti->ti_Tag32 = OOV_TAG_END;
				ti->ti_Ptr = NULL;
				if (ti > &attrlist[1]) // Y a-t-il des nouveaux attributs à notifier ?
					if (OO_DoNotify( destobj, attrlist ) == -1) // exécution de OOM_DELETE 
						if (destobj == wobj) return 0; // exécution de OOM_DELETE sur window 
				}
			}
		}

	if (rc == OOV_CANCEL) 
		return 0; // Do not execute DefWindowProc()

    return DefWindowProc( w, msgid, wparam, lparam );
} 
#endif // _WIN32

/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

// If a SetGet function is given to the attribute, the function has perhaps to change
//	the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same of the 
//	current attr value.

static ULONG SetGet_Selected( OOSetGetParms *sgp )
{
	OOInst_HandleGUI *inst = (OOInst_HandleGUI*)sgp->Instance;
	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		if (inst->SelectedObj != sgp->ExtAttr->ti_Ptr)
			{
			if (inst->SelectedObj != NULL) 
				inst->SelectedObj->Flags &= ~OOF_BREAKNOTIFY; 
			inst->SelectedObj = sgp->ExtAttr->ti_Ptr;
			return OOV_CHANGED;
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = inst->SelectedObj;
		}
	return OOV_NOCHANGE;
}

static ULONG SetGet_RenderInfo( OOSetGetParms *sgp )
{
	if (sgp->MethodFlag & OOF_MGET)
		{
		OOInst_HandleGUI *inst = (OOInst_HandleGUI*)sgp->Instance;
		*(APTR*)sgp->ExtAttr->ti_Ptr = &inst->_RI;
		}
	return OOV_NOCHANGE;
}

static ULONG SetGet_HandleGUI( OOSetGetParms *sgp )
{
	if (sgp->MethodFlag & OOF_MGET)
		{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Obj;
		}
	return OOV_NOCHANGE;
}


/***************************************************************************
 *
 *		Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
	{ OOA_SelectedObj, (ULONG)NULL, "SelectedObj",	SetGet_Selected  ,	OOF_MSET |	OOF_MGET | OOF_MNOT },
	{ OOA_RenderInfo , (ULONG)NULL, "RenderInfo" ,	SetGet_RenderInfo,				OOF_MGET },
	{ OOA_HandleGUI  , (ULONG)NULL, "HandleGUI"  ,	SetGet_HandleGUI ,				OOF_MGET },
	{ TAG_END }
	};

OOClass Class_HandleGUI = { 
	Dispatcher_HandleGUI, 
	&Class_SysEvent,	// Base class
	"Class_HandleGUI",	// Class Name
	0,					// InstOffset
	sizeof(OOInst_HandleGUI),// InstSize  : Taille de l'instance
	attrs,				// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
