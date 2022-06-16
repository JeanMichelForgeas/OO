#ifndef OO_GUI_H
#define OO_GUI_H

#include "oo_class.h"

typedef struct _OOMinMax		OOMinMax;
typedef struct _OOBorder		OOBorder;
typedef struct _OOBox			OOBox;
typedef struct _OOSize			OOSize;
typedef struct _OORenderInfo	OORenderInfo;

typedef struct _OOInst_Area 		OOInst_Area;
typedef struct _OOInst_AreaGroup	OOInst_AreaGroup;
typedef struct _OOInst_AreaScreen	OOInst_AreaScreen;
typedef struct _OOInst_AreaWindow	OOInst_AreaWindow;
typedef struct _OOInst_AreaRequest	OOInst_AreaRequest;
typedef struct _OOInst_AreaGroupBox	OOInst_AreaGroupBox;

//------------- General use attributes (0x1000-0x11FF) --------------

#define OOA_FontName		(0x1000|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)
#define OOA_FontHeight		(0x1001|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_FontHeightP100	(0x1002|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_FontBold		(0x1003|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_FontItalic		(0x1004|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_FontUnderline	(0x1005|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_FontFixed		(0x1006|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_FontVariable	(0x1007|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)

#define OOA_TitleIndex		(0x1010|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT) // Comme OOA_Title  mais on passe un index de catalogue 
#define OOA_TextIndex 		(0x1011|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT) // Comme OOA_Text   mais on passe un index de catalogue 
#define OOA_ListIndex 		(0x1012|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT) // Comme OOA_List   mais on passe un index de catalogue 
//#define OOA_		 		(0x1013|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)

#define OOA_Title			(0x1020|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)
#define OOA_TitlePlace		(0x1021|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Text			(0x1022|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)
#define OOA_TextPlace		(0x1023|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_List			(0x1024|TAGT_OO|TAGT_BYTE|TAGT_LST|TAGT_LIST)
#define OOA_FmtString		(0x1025|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)
#define OOA_NumDigits		(0x1026|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Image			(0x1027|TAGT_OO|TAGT_BYTE|TAGT_PTR|TAGT_HEXA)
#define OOA_ImagePlace		(0x1028|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)

#define OOA_ControlType		(0x1030|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_ControlId		(0x1031|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Id				(0x1032|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Number			(0x1033|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Min 			(0x1034|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGT_SIGNED)
#define OOA_Max 			(0x1035|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGT_SIGNED)
#define OOA_Callback		(0x1036|TAGT_OO|TAGT_LONG|TAGT_PTR)
#define OOA_Result			(0x1037|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Pos 			(0x1038|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGT_SIGNED)
#define OOA_Total			(0x1039|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Visible 		(0x103A|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Disabled		(0x103B|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_SelectedObj		(0x103C|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Activated		(0x103D|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ReplaceMode 	(0x103E|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_ReadOnly		(0x103F|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_BoxType			(0x1040|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Arrows			(0x1041|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Vertical		(0x1042|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Horizontal		(0x1043|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Borderless		(0x1044|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_TextWidth		(0x1045|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_TextLines		(0x1046|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_TextCols		(0x1047|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_OOMinMax		(0x1048|TAGT_OO|TAGT_WORD|TAGT_PTR|TAGT_SIGNED)
#define OOA_OOBox			(0x1049|TAGT_OO|TAGT_WORD|TAGT_PTR|TAGT_SIGNED)
#define OOA_OORect			(0x104A|TAGT_OO|TAGT_WORD|TAGT_PTR|TAGT_SIGNED)
#define OOA_OOBorder		(0x104B|TAGT_OO|TAGT_BYTE|TAGT_PTR)
#define OOA_AreaAddr		(0x104C|TAGT_OO|TAGT_LONG|TAGT_PTR)
#define OOA_Selected		(0x104D|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Determinate		(0x104E|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_3States			(0x104F|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)

#define OOA_Depth			(0x1060|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_NumColors		(0x1061|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_FirstColor		(0x1062|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_ColorTable		(0x1063|TAGT_OO|TAGT_LONG|TAGT_PTR)
#define OOA_Hue 			(0x1064|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Saturation		(0x1065|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Red 			(0x1066|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Green			(0x1067|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Blue			(0x1068|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)

//---------------------------------------------------------------

struct _OOMinMax {
	UWORD	MinW, MinH;
	UWORD	MaxW, MaxH;
	};

struct _OOBorder {
	UWORD	Left, Right, Top, Bottom;
	};

struct _OOBox {
	SWORD	X, Y;
	UWORD	W, H;
	};

struct _OOSize {
	UWORD	W, H;
	};


//======================== HandleGUI =======================================

//------------------------ Methods (0x1200-0x121F) --------------------------------

//------------------------ Attributes (0x1200-0x121F) --------------------------------

#define OOM_TREEPRESENT 0x1200 // construit un arbre : envoie NEW et PRESENT 
#define OOM_PRESENT 	0x1201 // envoie AREA... SETUP MINMAX START REFRESH 
#define OOM_HIDE		0x1202 // envoie AREA... STOP CLEANUP 

// For Speed these methods expect some tags in the order below :
//		 OOM_TREEPRESENT :	OOA_ParentOffs	OOA_VarBase	OOA_TreeItem
//		 OOM_PRESENT 	 :	OOA_Object
//		 OOM_HIDE		 :	OOA_Object
//

#define OOA_PressedButton	(0x1200|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
// #define OOA_EventMsg		0x1201 - private
// #define OOA_ParentOffs	0x1202 - private
// #define OOA_VarBase		0x1203 - private
// #define OOA_TreeItem		0x1204 - private


//======================== Area =======================================

//------------------------ Methods (0x1220-0x123F) --------------------------------

#define OOM_AREA_SETUP			0x1220
#define OOM_AREA_MINMAX 		0x1221	// OOResult = (OOInst_Area*)
#define OOM_AREA_SAMERI 		0x1222	// OOResult = (OOInst_Area*)
#define OOM_AREA_BOX			0x1223	// OOResult = (OOInst_Area*)
#define OOM_AREA_START			0x1224
#define OOM_AREA_REFRESH		0x1225
#define OOM_AREA_STOP			0x1226
#define OOM_AREA_CLEANUP		0x1227
#define OOM_AREA_CHANGED		0x1228
#define OOM_AREA_SIGNALCHANGE	0x1229

// For Speed these methods expect some tags in the order below :
//		OOM_AREA_SETUP		:	OOA_Flags		OOA_HandleGUI	OOA_RenderInfo
//		OOM_AREA_MINMAX 	:	OOA_Flags
//		OOM_AREA_BOX		:	OOA_Flags		OOA_OOBox
//		OOM_AREA_START		:	OOA_Flags
//		OOM_AREA_REFRESH	:	OOA_Flags		OOA_RastPort
//		OOM_AREA_STOP		:	OOA_Flags
//		OOM_AREA_CLEANUP	:	OOA_Flags
//		OOM_AREA_CHANGED	:	OOA_Flags		OOA_Object
//		OOM_AREA_SAMERI 	:	OOA_RenderInfo

//------------------------ Attributes (0x1220-0x123F) --------------------------------

#define OOA_LeftTop 		(0x1220|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGF_DOUBLEWORD)
#define OOA_WidthHeight 	(0x1221|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGF_DOUBLEWORD)
#define OOA_MinWidthHeight	(0x1222|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGF_DOUBLEWORD)
#define OOA_MaxWidthHeight	(0x1223|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGF_DOUBLEWORD)
#define OOA_Weight			(0x1224|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_ExpandWidth 	(0x1225|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ExpandHeight	(0x1226|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_FullWidth 		(0x1227|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_FullHeight		(0x1228|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_SameWidth		(0x1229|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_SameHeight		(0x122A|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_RenderInfo		(0x122B|TAGT_OO|TAGT_BYTE|TAGT_PTR)
// #define OOA_AreaFlags	 0x122C - private
#define OOA_HandleGUI		(0x122D|TAGT_OO|TAGT_OBJ|TAGT_PTR)
#define OOA_RastPort		(0x122E|TAGT_OO|TAGT_BYTE|TAGT_PTR)

// Flags pour messages de OOM_AREA...
#define OOF_SAME_RI 	0x0001
#define OOF_SELF_RI 	0x0002


//======================== Group =======================================

//------------------------ Methods (0x1240-0x125F) --------------------------------

#define OOM_AREA_ISGROUP	0x1240	// OOResult = (OOInst_Area*)

//------------------------ Attributes (0x1240-0x125F) --------------------------------

#define OOA_VertGroup		(0x1240|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_HorizGroup		(0x1241|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Margins 		(0x1242|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
//#define OOA_IsGroup 		(0x1243|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Spacing 		(0x1244|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)


//======================== Frame =======================================

//------------------------ Methods (0x1260-0x127F) --------------------------------

//------------------------ Attributes (0x1260-0x127F) --------------------------------

#define OOA_Pattern 	(0x0035|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_Filled		(0x003C|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Rounded 	(0x003D|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
//#define OOA_BoxType   dans les généraux : 0x1040


//======================== Screen =======================================

//------------------------ Methods (0x1280-0x129F) --------------------------------

#define OOM_DBWAIT_SAFETOWRITE	0x1280
#define OOM_DBWAIT_SAFETOCHANGE 0x1281

//------------------------ Attributes (0x1280-0x129F) --------------------------------

#define OOA_DoubleBuffer	(0x1280|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_DefaultScreen	(0x1281|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_PublicScreen	(0x1282|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)
#define OOA_FrontScreen 	(0x1283|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ScreenAddress	(0x1284|TAGT_OO|TAGT_LONG|TAGT_PTR)


//======================== Window =======================================

//------------------------ Methods (0x12A0-0x12FF) --------------------------------

//------------------------ Attributes (0x12A0-0x12FF) --------------------------------

#define OOA_MousePos		(0x12A0|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT|TAGF_DOUBLEWORD)
#define OOA_ButLeftDown 	(0x12A1|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ButLeftUp		(0x12A2|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ButMidDown		(0x12A3|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ButMidUp		(0x12A4|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ButRightDown	(0x12A5|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_ButRightUp		(0x12A6|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_KeyDown 		(0x12A7|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_KeyUp			(0x12A8|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinActive		(0x12A9|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCloseAsk 	(0x12AA|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinRefreshAsk	(0x12AB|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinChange		(0x12AC|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_Second10		(0x12AD|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_CloseQuit		(0x12AE|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)

#define OOA_PressedKey		OOA_KeyUp
#define OOA_PressedRight	OOA_ButRightUp
#define OOA_PressedLeft 	OOA_ButLeftUp

#define OOA_WinCtlIcon		(0x12B0|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCtlZoom		(0x12B1|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCtlDepth 	(0x12B2|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCtlSize		(0x12B3|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCtlDrag		(0x12B4|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCtlClose 	(0x12B5|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCtlInsRight	(0x12B6|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WinCtlInsBot	(0x12B7|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)

#define OOA_PtrCurrent		(0x12C0|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)
#define OOA_PtrNull 		(0x12C1|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_PtrWait 		(0x12C2|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_PtrSys			(0x12C3|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)

#define OOA_Sync			(0x12D0|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_WindowHandle	(0x12D1|TAGT_OO|TAGT_LONG|TAGT_PTR)


//======================== Dialog =======================================

//------------------------ Methods (0x1300-0x133F) --------------------------------

#define OOM_WAITDIALOG		0x1300 // Attend une fin de fenêtre de dialogue 
#define OOM_SATISFYDIALOG	0x1301 // Met fin au dialogue avec une valeur dans AttrList, en data de OOA_Result 

//------------------------ Attributes (0x1300-0x133F) --------------------------------


//======================== Controls =======================================

#define OOV_LEFT	0x0001	// PLACETEXT_LEFT
#define OOV_RIGHT	0x0002	// PLACETEXT_RIGHT
#define OOV_TOP		0x0004	// PLACETEXT_ABOVE
#define OOV_BOTTOM	0x0008	// PLACETEXT_BELOW
#define OOV_CENTER	0x0010	// PLACETEXT_IN

//---------------------------------------------------------------

/*
extern OOClass Class_Dialog;
extern OOClass Class_AreaRequest;
extern OOClass Class_DebugEvents;
extern OOClass Class_Boopsi;
extern OOClass Class_GadListView;
extern OOClass Class_GadPalette;
extern OOClass Class_GadPopup;
*/
extern OOClass Class_HandleGUI;
extern OOClass Class_Area;
extern OOClass Class_AreaGroup;
extern OOClass Class_AreaScreen;
extern OOClass Class_AreaWindow;
extern OOClass Class_AreaGroupBox;
extern OOClass Class_Ctrl;
extern OOClass Class_CtrlButton;
extern OOClass Class_CtrlScroller;
extern OOClass Class_CtrlChecker;
extern OOClass Class_CtrlRadio;
extern OOClass Class_CtrlDisplayNum;
extern OOClass Class_CtrlDisplayText;
extern OOClass Class_CtrlDisplayImage;
extern OOClass Class_CtrlEditNum;
extern OOClass Class_CtrlEditText;
extern OOClass Class_CtrlEditImage;
extern OOClass Class_CtrlList;
extern OOClass Class_CtrlTree;
extern OOClass Class_CtrlCombo;

#endif // OO_GUI_H 
