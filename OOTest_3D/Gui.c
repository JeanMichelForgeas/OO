/******************************************************************
 *
 *       Project:    Test 3D
 *       Function:   Interface utilisateur
 *
 *       Created:    06/08/99    Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1999 Jean-Michel Forgeas
 *
 *****************************************************************/


//******* Includes ************************************************

#include "ootypes.h"
#include "ooprotos.h"

//#include "oo_tags.h"
//#include "oo_buf.h"
//#include "oo_text.h"
//#include "oo_pictures.h"
//#include "oo_init.h"
#include "oo_class.h"
#include "oo_gui.h"
#include "oo_gfx.h"
#include "oo_3d.h"

#include "data.h"


//******* Imported ************************************************

extern struct ApplObjects ApplObj;

extern void * Test0_Init( void );


//******* Exported ************************************************


//******* Statics *************************************************

//------ Tags

static OOTagList WinTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_WINDOW1,
	OOA_HorizGroup	, TRUE,
	OOA_CloseQuit	, TRUE,
	TAG_END
	};

static OOTagList VertGroupTags[] = {
	OOA_VertGroup		, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};

static OOTagList HorizGroupTags[] = {
	OOA_HorizGroup		, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};

static OOTagList DisplayImageTags[] = {
	OOA_ImageIndex	, OOPIC_ICON_LOGO,
	OOA_BoxType		, OOV_BOX_NULL,
	TAG_END
	};

static OOTagList DisplayTextTags[] = {
	OOA_TextIndex	, OOTEXT_COPYRIGHT,
	OOA_BoxType		, OOV_BOX_NULL,
	TAG_END
	};

static OOTagList Options_HorizGroupBoxTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_GROUPH3D,
	OOA_FontBold	, TRUE,
	OOA_VertGroup	, TRUE,
	OOA_Margins		, TRUE,
	OOA_ExpandWidth	, TRUE,
	TAG_END
	};

static OOTagItem TL_Area3D[] = {
    OOA_WidthHeight,    W2L(641,401),
    TAG_END
    };

static OOTagList TL_ChkVBL[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_WAITVBL,
	OOA_Selected	, FALSE,
	OOA_3States		, FALSE,
	TAG_END
	};
static OOTagList TL_ChkClr[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_CLEARSCR,
	OOA_Selected	, TRUE,
	OOA_3States		, FALSE,
	TAG_END
	};

static OOTagList TL_RadTest[] = { 
	OOA_TitleIndex,	OOTEXT_TITLE_RADIOTEST, 
	OOA_ListIndex,	OOTEXT_LIST_RADIOTEST, 
	OOA_Selected,	-1,
	OOA_TitlePlace,	OOV_TOP, 
	TAG_END 
	};
static OOTagList TL_ButStart[] = { 
	OOA_WidthHeight,	W2L(50,50),
	TAG_END 
	};

//------ Notifications

static OONotifyItem NF_ChkVBL[] = { { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0, OF(ApplObjects,H3D), OON_ACT_DOMETHODVALUE, OOM_SET, D3A_WaitVBlank, 0, TAG_END }, OOV_ENDTABLE };

static OONotifyItem NF_ChkClr[] = { { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0, OF(ApplObjects,H3D), OON_ACT_DOMETHODVALUE, OOM_SET, D3A_ClearScreen, 0, TAG_END }, OOV_ENDTABLE };

static ULONG NFunc_RadTest( OObject *srcobj, ULONG attr, ULONG val, ULONG min, ULONG max, OObject *destobj, OOTagItem *taglist );
static OONotifyItem NF_RadTest[] = { { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0, 0, OON_ACT_CALLFUNC, (ULONG)NFunc_RadTest }, OOV_ENDTABLE };

static ULONG NFunc_ButStart( OObject *srcobj, ULONG attr, ULONG val, ULONG min, ULONG max, OObject *destobj, OOTagItem *taglist );
static OONotifyItem NF_ButStart[] = { { OON_TRIG_EQUAL, OOA_PressedButton, TRUE,0, 0, OON_ACT_CALLFUNC, (ULONG)NFunc_ButStart }, OOV_ENDTABLE };


//------ Tree

static OOTreeItem Tree[] =
    {
	OBJECT( 0, OF(ApplObjects,GUI),  &Class_HandleGUI, NULL, NULL ),
		OBJECT( 1, OF(ApplObjects,Screen),  &Class_AreaScreen, NULL, NULL ),
			OBJECT( 2, OF(ApplObjects,Window),  &Class_AreaWindow, WinTags, NULL ),
				OBJECT( 3, OOV_NOOFFS,  &Class_AreaGroup, VertGroupTags, 0 ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, 0 ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlDisplayImage, DisplayImageTags, NULL ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlDisplayText, DisplayTextTags, NULL ),
						//OBJECT( 5, OOV_NOOFFS,  &Class_CtrlDisplayImage, DisplayImageTags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroupBox, Options_HorizGroupBoxTags, NULL ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlChecker, TL_ChkVBL, NF_ChkVBL ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlChecker, TL_ChkClr, NF_ChkClr ),
						OBJECT( 5, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, 0 ),
							OBJECT( 6, OF(ApplObjects,TestType),  &Class_CtrlRadio, TL_RadTest, NF_RadTest ),
							OBJECT( 6, OF(ApplObjects,ButStart),  &Class_CtrlButton, TL_ButStart, NF_ButStart ),
				OBJECT( 3, OF(ApplObjects,Area3D), &Class_Area, TL_Area3D, NULL ),
    OBJECTEND
    };


/******************************************************************
 *
 *      Code
 *
 *****************************************************************/

void * GUI_Init( void )
{
	OObject *gui;

	gui = OO_NewTree( ApplObj.Appl, &ApplObj, Tree );

	return gui;
}

/******************************************************************
 *
 *      Notifications
 *
 *****************************************************************/

static ULONG NFunc_RadTest( OObject *srcobj, ULONG attr, ULONG val, ULONG min, ULONG max, OObject *destobj, OOTagItem *taglist )
{
	if (ApplObj.Space != NULL)
		{ OO_Delete( ApplObj.Space ); ApplObj.Space = NULL; }

	switch (val)
		{
		case 0: ApplObj.Space = Test0_Init(); break;
		//case 1: Test1_Init(); break;
		//case 2: Test2_Init(); break;
		default: OOASSERT (FALSE); break;
		}

	return OOV_OK;	
}

static ULONG NFunc_ButStart( OObject *srcobj, ULONG attr, ULONG val, ULONG min, ULONG max, OObject *destobj, OOTagItem *taglist )
{
	BOOL onoff = OO_GetAttr( ApplObj.H3D, OOA_OnOff );
	OO_DoMethod( ApplObj.H3D, (onoff == TRUE) ? D3M_STOP3D : D3M_START3D, 0 );
	return OOV_OK;	
}
