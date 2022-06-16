#ifndef OO_GUI_PRV_H
#define OO_GUI_PRV_H 1

#include "oo_gui.h"
#include "oo_gfx_prv.h"

//================================================================================
//================================================================================
//================================================================================
#ifdef AMIGA
#include <intuition/screens.h>

typedef struct _OOInst_Boopsi		OOInst_Boopsi;
typedef struct _OOInst_GadTools 	OOInst_GadTools;

typedef struct IntuiMessage		OOEventMsg;

//---------------------------------- Divers -----------------------------------

struct _OOMsg_SGId2Obj		{ ULONG MethodId; OOTagItem *AttrList; ULONG ControlId; };

//---------------------------- RenderInfo -----------------------------------

struct _OORenderInfo {
	struct Screen		*sc;
	struct Window		*wi;
	struct Requester	*rq;
	OORastPort 			*rp;
	OOFont 				*Font;
	OOPalette			*Palette;
	struct DrawInfo 	*di;
	struct VisualInfo	*vi;	 // gadtools 
	struct Gadget		*GTList; // gadtools 
	struct Gadget		*GTLast; // gadtools 
	};

//------------------------- HandleGUI --------------------------------------

struct _OOInst_HandleGUI {
	OORenderInfo	_RI;
	OObject 		*SelectedObj;
	ULONG			Flags;
	OObject 		*SyncObj;
	};

#define OOF_GUI_AREACHANGED 0x0001

#define OOA_EventMsg		(0x1201|TAGT_OO|TAGT_BYTE|TAGT_PTR)
#define OOA_ParentOffs		(0x1202|TAGT_OO|TAGT_LONG|TAGT_NUM)
#define OOA_VarBase			(0x1203|TAGT_OO|TAGT_LONG|TAGT_NUM)
#define OOA_TreeItem		(0x1204|TAGT_OO|TAGT_BYTE|TAGT_PTR)

//-------------------------- Area -------------------------------------

struct _OOInst_Area {
	OObject 		*GUI;
	OORenderInfo	*RI;
	OOMinMax		_MM;
	OOBox			_B; 		// Box totale bords compris 
	OOBorder		_Border;	// A l'intérieur de la box _B	 de OOInst_Area (ex: bords de fenêtre, de titre de gadget) 
	OOBorder		_SubBorder; // A l'intérieur de la box _SubB de OOInst_Area (ex: bordure à l'intérieur d'un cadre de groupe ou de gadget) 
	SWORD			AskedW; 	// taille demandée par tags (Methods OOM_NEW ou OOM_SET) 
	SWORD			AskedH; 	// 
	UBYTE			Weight;
	UBYTE			StatusFlags;
	UWORD			padw;
	ULONG			Flags;
	};

#define OOA_AreaFlags		(0x122C|TAGT_OO|TAGT_LONG|TAGT_NUM) // Normalement il n'est pas à utiliser 

// StatusFlags 
#define OOF_AREA_SETUP		0x01
#define OOF_AREA_STARTED	0x02
#define OOF_START_SPECIFY	0x04
#define OOF_AREA_CHANGED	0x08

// Flags OOA_AreaFlags mis à jour dans OOM_AREA_SETUP 
#define OOF_EXPANDH 		0x0001
#define OOF_EXPANDW 		0x0002
#define OOF_AINSIDE_LEFT	0x0004
#define OOF_AINSIDE_RIGHT	0x0008
#define OOF_AINSIDE_TOP 	0x0010
#define OOF_AINSIDE_BOTTOM	0x0020
#define OOF_AINSIDE_CENTERW 0x0040
#define OOF_AINSIDE_CENTERH 0x0080
#define OOF_AINSIDE_FULLW	0x0100
#define OOF_AINSIDE_FULLH	0x0200
#define OOF_ANEWRENDERINFO	0x0400
#define OOF_ISGROUP 		0x0800
#define OOF_SAMEW			0x1000
#define OOF_SAMEH			0x2000

#define OOF_MEMSAMEW		0x10000000
#define OOF_MEMSAMEH		0x20000000
#define OOF_MEMEXPANDW		0x40000000
#define OOF_MEMEXPANDH		0x80000000
#define OOMASK_KEEPAFLAGS (OOF_MEMSAMEW|OOF_MEMSAMEH|OOF_MEMEXPANDW|OOF_MEMEXPANDH) // pour mettre à 0 tous les flags sauf ceux-là 

#define _DATA_AREA(o)		((OOInst_Area*)((ULONG)o+Class_Area.InstOffset))	// Root -> Area 
#define _DATA_AREAGROUP(o)	((OOInst_AreaGroup*)((ULONG)o+Class_AreaGroup.InstOffset))	 // Root -> AreaGroup 
#define _DATA_HANDLEGUI(o)	((OOInst_HandleGUI*)((ULONG)(_DATA_AREA(o)->GUI)+Class_HandleGUI.InstOffset))
#define _RENDERINFO(o)		(_DATA_AREA(o)->RI)

//-------------------------- Group -------------------------------------

struct _OOInst_AreaGroup {
	APTR		Ptr;
	UBYTE		VSpacing;
	UBYTE		HSpacing;
	UWORD		Flags;
	};

#define OOV_GRP_HOFFS	6
#define OOV_GRP_VOFFS	4

// Flags internes mis à jour dans OOM_AREA_MINMAX par chaque AreaGroup 
#define OOF_GRP_VERT		0x0001
#define OOF_GRP_HORIZ		0x0002
#define OOF_GRP_MARGINS 	0x0004
#define OOF_GRP_SPACING 	0x0008
#define OOF_GRP_ALLOCFONT	0x0010

//------------------------- Screen --------------------------------------

struct _OOInst_AreaScreen {
	OORenderInfo		_RI;
	struct DrawInfo 	_DI;
	OOTagItem			PosTags[8];
	ULONG				Flags;
	ULONG				ScrDesc; // nom public si OOF_SCRPUBLIC 
	//--- Support double buffer 
	struct ScreenBuffer *ScreenBuffer[2];
	struct MsgPort		*Ports[2];
	struct RastPort		RPort[2];
	UBYTE				SafeToChange;
	UBYTE				SafeToWrite;
	UBYTE				CurBuffer;
	UBYTE				padb;
	//--- Support tailles SysGadgets 
	UBYTE				CloseW;
	UBYTE				ZoomW;
	UBYTE				DepthW;
	UBYTE				SizeW;
	UBYTE				SizeH;
	UBYTE				TitleH;
	};

#define OOF_SCRDEFAULT		0x0001
#define OOF_SCRPUBLIC		0x0002
#define OOF_SCRFRONT		0x0004
#define OOF_SCRADDRESS		0x0008
#define OOF_SCRMASK 		0x000f
#define OOF_SCRDBLBUF		0x0010

//----------------------- Window ----------------------------------------

struct _OOInst_AreaWindow {
	OORenderInfo		_RI;
	OOTagItem			PosTags[13];
	OOKeySupp			Key;
	struct IntuiMessage *GTImsg;
	ULONG				WinFlags;
	ULONG				Flags;
	ULONG				Pattern;
	ULONG				CurrentPtr;
	ULONG				Installed;
	OObject 			*SyncObj;
	OObject 			*SyncPrev;
	struct Requester	*SyncRequest;
	};

#define OOV_PTR_SYS 	1
#define OOV_PTR_WAIT	2
#define OOV_PTR_NULL	3

//----------------------- Request ----------------------------------------

struct _OOInst_AreaRequest {
	OORenderInfo		_RI;
	ULONG				Flags;
	};

#define OOF_REQALLOCFONT	0x0001

//------------------------ GroupBox ---------------------------------------

struct _OOInst_AreaGroupBox {
	OOFont	*Font;
	UBYTE	Type;
	UBYTE	Flags;
	UWORD	padw;
	UBYTE	*Title;
	UWORD	TitleWidth;
	UBYTE	TitleFlags;
	UBYTE	padb;
	ULONG	Pattern;
	};

//---------------------------------------------------------------

struct _OOInst_Boopsi {
	OOFont			*Font;
	APTR			LibBase;
	struct Gadget	*Gadget;
	struct IClass	*Class;
	UBYTE			*ClassId;
	OOTagItem		PosTags[8];
	};
#define OOF_BOOPSI_GADGET 1

#define OOA_Boopsi_Class			(0x001A|TAG_OO|TAG_APTR)
#define OOA_Boopsi_ClassId			(0x001B|TAG_OO|TAG_STRING)
#define OOA_Boopsi_ClassFileName	(0x0028|TAG_OO|TAG_STRING)
#define OOA_Boopsi_Object			(0x001F|TAG_OO|TAG_APTR)
#define OOA_Boopsi_RememberLong 	(0x0020|TAG_OO|TAG_APTR32)
#define OOA_Boopsi_RememberString	(0x0021|TAG_OO|TAG_APTR)
#define OOA_Boopsi_TagDrawInfo		(0x0022|TAG_OO|TAG_NUMBER|TAG_HEX)
#define OOA_Boopsi_TagVisualInfo	(0x0026|TAG_OO|TAG_NUMBER|TAG_HEX)
#define OOA_Boopsi_TagScreen		(0x0023|TAG_OO|TAG_NUMBER|TAG_HEX)
#define OOA_Boopsi_TagWindow		(0x0024|TAG_OO|TAG_NUMBER|TAG_HEX)
#define OOA_Boopsi_TagRequest		(0x0025|TAG_OO|TAG_NUMBER|TAG_HEX)
#define OOA_Boopsi_TagFont			(0x0027|TAG_OO|TAG_NUMBER|TAG_HEX)

//---------------------------------------------------------------

struct _OOInst_GadTools {
	OOFont			*Font;
	struct Gadget	*Gadget;
	OOTagItem		*TL_OO;
	OOTagItem		*TL_GT;
	OOTagItem		*TL_OW;
	OOTagItem		*MapList;
	UBYTE			*FlagsList;
	ULONG			Flags;
	APTR			SubClassData;
	};
#define OOF_GADTOOLS_GADGET 1

#define OGTID(o) ((OOInst_GadTools*)((struct{OOInst_Root Root;OOInst_Area Area;}*)o + 1))
#define IDGTO(o) ((OOInst_Root*)((struct{OOInst_Root Root;OOInst_Area Area;}*)o - 1))

#define OOF_NEW 	0x01
#define OOF_SET 	0x02
#define OOF_GET 	0x04
#define OOF_NOTIFY	0x08
#define OOF_AREA	0x10
#define OOF_GTNEW	0x20
#define OOF_GTSET	0x40
#define OOF_GTGET	0x80

//---------------------------------------------------------------

//extern APTR _open_font_attr( struct TTextAttr *tta );
//extern APTR _open_font_name( UBYTE *name, ULONG height );
//extern void _font_to_tta( OOFont *font, struct TTextAttr *tta );
//extern APTR _open_font_taglist( OOTagItem *taglist, struct TTextAttr *tta, APTR *pfont );


#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include <pragmas/gadtools_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>

extern struct Library * __far GfxBase;
extern struct Library * __far GadToolsBase;
extern struct Library * __far IntuitionBase;

#endif // AMIGA


//================================================================================
//================================================================================
//================================================================================
#ifdef _WIN32

#define OOV_VERTTITLESPACING	2
#define OOV_HORIZTITLESPACING	6

typedef struct _OOInst_HandleGUI	OOInst_HandleGUI;

typedef MSG		OOEventMsg;


//---------------------------------- Divers -----------------------------------


//---------------------------- RenderInfo -----------------------------------

struct _OORenderInfo {
	HWND		sc;
	HWND		wi;
	HWND		rq;
	OORastPort	*rp;
	OOFont		*Font;
	OOPalette	*Palette;
	ULONG		SavedDC;
/*
	struct DrawInfo 	*di;
	struct VisualInfo	*vi;	 // gadtools 
	struct Gadget		*GTList; // gadtools 
	struct Gadget		*GTLast; // gadtools 
*/
	};


//------------------------- HandleGUI --------------------------------------

struct _OOInst_HandleGUI {
	OORenderInfo	_RI;
	OObject 		*SelectedObj;
	ULONG			Flags;
	OObject 		*SyncObj;
	MSG				CurrentMsg;
	ATOM			RegisteredClass;
	};

#define OOWINCLASSNAME	"OOWINDOWCLASS"

#define OOF_GUI_AREACHANGED 0x0001

#define OOA_EventMsg		(0x1201|TAGT_OO|TAGT_BYTE|TAGT_PTR)
#define OOA_ParentOffs		(0x1202|TAGT_OO|TAGT_LONG|TAGT_NUM)
#define OOA_VarBase			(0x1203|TAGT_OO|TAGT_BYTE|TAGT_PTR)
#define OOA_TreeItem		(0x1204|TAGT_OO|TAGT_BYTE|TAGT_PTR)


//-------------------------- Area -------------------------------------

struct _OOInst_Area {
	OObject 		*GUI;
	OORenderInfo	*RI;
	OOMinMax		_MM;
	OOBox			_B; 		// Box totale bords compris 
	OOBorder		_Border;	// A l'intérieur de la box _B	 de OOInst_Area (ex: bords de fenêtre, de titre de gadget) 
	OOBorder		_SubBorder; // A l'intérieur de la box _SubB de OOInst_Area (ex: bordure à l'intérieur d'un cadre de groupe ou de gadget) 
	SWORD			AskedW; 	// taille demandée par tags (Methods OOM_NEW ou OOM_SET) 
	SWORD			AskedH; 	// 
	UBYTE			Weight;
	UBYTE			StatusFlags;
	UWORD			padw;
	ULONG			Flags;
	};

#define OOA_AreaFlags		(0x122C|TAGT_OO|TAGT_LONG|TAGT_NUM) // Normalement il n'est pas à utiliser 

// StatusFlags 
#define OOF_AREA_SETUP		0x01
#define OOF_AREA_STARTED	0x02
#define OOF_START_SPECIFY	0x04
#define OOF_AREA_CHANGED	0x08

// Flags OOA_AreaFlags mis à jour dans OOM_AREA_SETUP 
#define OOF_EXPANDH 		0x0001 // -+- Pour OOM_AREA_MINMAX
#define OOF_EXPANDW 		0x0002 // _|
#define OOF_AINSIDE_LEFT	0x0004 // _
#define OOF_AINSIDE_RIGHT	0x0008 //  |
#define OOF_AINSIDE_TOP 	0x0010 //  |
#define OOF_AINSIDE_BOTTOM	0x0020 //  +- Pour OOM_AREA_BOX
#define OOF_AINSIDE_CENTERW 0x0040 //  |
#define OOF_AINSIDE_CENTERH 0x0080 //  |
#define OOF_AINSIDE_FULLW	0x0100 //  | 
#define OOF_AINSIDE_FULLH	0x0200 // -
#define OOF_ANEWRENDERINFO	0x0400
#define OOF_ISGROUP 		0x0800
#define OOF_SAMEW			0x1000
#define OOF_SAMEH			0x2000
#define OOMASK_AINSIDEPLACE (OOF_AINSIDE_LEFT|OOF_AINSIDE_RIGHT|OOF_AINSIDE_TOP|OOF_AINSIDE_BOTTOM|OOF_AINSIDE_CENTERW|OOF_AINSIDE_CENTERH) // pour mettre à 0 tous les flags sauf ceux-là 

#define OOF_MEMFULLW		0x04000000
#define OOF_MEMFULLH		0x08000000
#define OOF_MEMSAMEW		0x10000000
#define OOF_MEMSAMEH		0x20000000
#define OOF_MEMEXPANDW		0x40000000
#define OOF_MEMEXPANDH		0x80000000
#define OOMASK_KEEPAFLAGS (OOF_MEMFULLW|OOF_MEMFULLH|OOF_MEMSAMEW|OOF_MEMSAMEH|OOF_MEMEXPANDW|OOF_MEMEXPANDH) // pour mettre à 0 tous les flags sauf ceux-là 

#define _DATA_AREA(o)		((OOInst_Area*)(((ULONG)o)+Class_Area.InstOffset))	// Root -> Area 
#define _DATA_AREAGROUP(o)	((OOInst_AreaGroup*)(((ULONG)o)+Class_AreaGroup.InstOffset))	 // Root -> AreaGroup 
#define _DATA_HANDLEGUI(o)	((OOInst_HandleGUI*)(((ULONG)(_DATA_AREA(o)->GUI))+Class_HandleGUI.InstOffset))
#define _RENDERINFO(o)		(_DATA_AREA(o)->RI)


//-------------------------- Group -------------------------------------

struct _OOInst_AreaGroup {
	APTR	Ptr;
	UBYTE	VSpacing;
	UBYTE	HSpacing;
	UWORD	Flags;
	};

#define OOV_GRP_HOFFS	8
#define OOV_GRP_VOFFS	8

// Flags internes mis à jour dans OOM_AREA_MINMAX par chaque AreaGroup 
#define OOF_GRP_VERT		0x0001
#define OOF_GRP_HORIZ		0x0002
#define OOF_GRP_MARGINS 	0x0004
#define OOF_GRP_SPACING 	0x0008


//------------------------- Screen --------------------------------------

struct _OOInst_AreaScreen {
	OORenderInfo		_RI;
	OORastPort			_PlainRP;
	ULONG				Flags;
	ULONG				ScrDesc; // nom public si OOF_SCRPUBLIC
/*
	struct DrawInfo 	_DI;
	OOTagItem			PosTags[8];
	//--- Support double buffer 
	struct ScreenBuffer *ScreenBuffer[2];
	struct MsgPort		*Ports[2];
	struct RastPort 	RPort[2];
	UBYTE				SafeToChange;
	UBYTE				SafeToWrite;
	UBYTE				CurBuffer;
	UBYTE				padb;
	//--- Support tailles SysGadgets 
	UBYTE				CloseW;
	UBYTE				ZoomW;
	UBYTE				DepthW;
	UBYTE				SizeW;
	UBYTE				SizeH;
	UBYTE				TitleH;
*/
	};

#define OOF_SCRDEFAULT		0x0001
#define OOF_SCRPUBLIC		0x0002
#define OOF_SCRFRONT		0x0004
#define OOF_SCRADDRESS		0x0008
#define OOF_SCRMASK 		0x000f
#define OOF_SCRDBLBUF		0x0010
#define OOF_SCRALLOCFONT	0x0020


//----------------------- Window ----------------------------------------

struct _OOInst_AreaWindow {
	OORenderInfo		_RI;
	OORastPort			_PlainRP;
	ULONG				Flags;
	ULONG				Styles;
	ULONG				CurrentPtr;
	ULONG				Installed;
	OObject 			*SyncObj;
	OObject 			*SyncPrev;
	void				*SyncRequest;
	ULONG				Pattern;
	OOBuf				*BufTitle;
//	OOKeySupp			Key;
	};

#define OOV_PTR_SYS 	1
#define OOV_PTR_WAIT	2
#define OOV_PTR_NULL	3


//------------------------ GroupBox ---------------------------------------

struct _OOInst_AreaGroupBox {
	OOFont	*Font;
	UWORD	Flags;
	UWORD	padw;
	};

#define OOF_GRPBOX_ALLOCFONT	0x0001


//------------------------ Controls ------------------------------------

struct _OOInst_Ctrl {
	OOFont	*Font;
	HWND	Win;
	ULONG	Flags;
	OOSize	TitleSize;
	};
typedef struct _OOInst_Ctrl OOInst_Ctrl;

#define OOT_CTRL_UNKNOWN	0
#define OOT_CTRL_BUTTON		1
#define OOT_CTRL_SCROLLER	2
#define OOT_CTRL_SLIDER		3
#define OOT_CTRL_CHECKER	4
#define OOT_CTRL_RADIO		5
#define OOT_CTRL_DISPNUM	6
#define OOT_CTRL_DISPTEXT	7
#define OOT_CTRL_DISPIMAGE	8
#define OOT_CTRL_EDITNUM	9
#define OOT_CTRL_EDITTEXT	10
#define OOT_CTRL_EDITIMAGE	11
#define OOT_CTRL_LIST		12
#define OOT_CTRL_TREE		13
#define OOT_CTRL_COMBO		14


//---------------------------------------------------------------

#endif // _WIN32

extern ULONG	OObjId_Obtain	( OObject *obj, ULONG flags );
extern OObject *OObjId_GetObj	( ULONG gadid );
extern ULONG	OObjId_GetFlags	( ULONG gadid );
extern void		OObjId_Release	( ULONG gadid );


#endif // OO_GUI_PRV_H 
