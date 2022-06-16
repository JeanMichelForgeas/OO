/*****************************************************************
 *
 *       Project:    TrayMon
 *       Function:   Tray icons Monitor
 *
 *       Created:    25/08/98    Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"

//#include "oo_tags.h"
#include "oo_buf.h"
#include "oo_text.h"
#include "oo_pictures.h"
#include "oo_init.h"
#include "oo_class.h"
#include "oo_gui.h"
#include "oo_gfx.h"

//#include "oo_class_prv.h"

#include "TrayMon_Resource.h"

#define OOTEXT_TITLE_COMPAGNY	1
#define OOTEXT_TITLE_AUTHOR		2 
#define OOTEXT_TITLE_APPL		3 
#define OOTEXT_TITLE_WINDOW1	4 
#define OOTEXT_COPYRIGHT		5 
#define OOTEXT_TITLE_CPUGROUP	6 
#define OOTEXT_TITLE_CPUCHECK	7 
#define OOTEXT_TITLE_CPUCOLOR	8 
#define OOTEXT_TITLE_CPUSCROLL	9 
#define OOTEXT_TITLE_CPUDELAY	10
#define OOTEXT_TITLE_MEMGROUP	11
#define OOTEXT_TITLE_MEMCHECK	12
#define OOTEXT_TITLE_MEMCOLOR	13
#define OOTEXT_TITLE_MEMSCROLL	14
#define OOTEXT_TITLE_MEMDELAY	15
#define OOTEXT_TITLE_NETGROUP	16
#define OOTEXT_TITLE_NETCHECK	17
#define OOTEXT_TITLE_NETCOLOR	18
#define OOTEXT_TITLE_NETSCROLL	19
#define OOTEXT_TITLE_NETDELAY	20

#define OOPIC_ICON_APPL	1
#define OOPIC_ICON_LOGO	2


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static OOTextItem text_table[] = {
		{ 0 }, // 0 reserved
		{ 0, 0, 0, "JM Forgeas" },
		{ 0, 0, 0, "Jean-Michel Forgeas" },
		{ 0, 0, 0, "TrayMon" },
		{ 0, 0, 0, "Change monitoring" },
		{ 0, 0, 0, "Copyright © 1998, Jean-Michel Forgeas\nAll Rights Reserved" },
		{ 0, 0, 0, " CPU usage " },
		{ 0, 0, 0, "" },
		{ 0, 0, 0, "Color" },
		{ 0, 0, 0, "Delay" },
		{ 0, 0, 0, "(ms)" },
		{ 0, 0, 0, " Free memory " },
		{ 0, 0, 0, "" },
		{ 0, 0, 0, "Color" },
		{ 0, 0, 0, "Delay" },
		{ 0, 0, 0, "(ms)" },
		{ 0, 0, 0, " Network activity " },
		{ 0, 0, 0, "" },
		{ 0, 0, 0, "Color" },
		{ 0, 0, 0, "Delay" },
		{ 0, 0, 0, "(ms)" },
		{ 0, 0, 0, (char*)OOV_ENDTABLE }, // end marker
	};

static OOPicItem images_table[] = {
		{ 0 }, // 0 reserved
		{ MAKEINTRESOURCE(IDI_APPL), 0, OOF_PIC_RESOURCE|OOF_PIC_ICON, NULL },
		{ MAKEINTRESOURCE(IDI_LOGO), 0, OOF_PIC_RESOURCE|OOF_PIC_ICON, NULL },
		{ NULL, 0, 0, (void*)OOV_ENDTABLE }, // end marker
	};

static struct OP {
    OObject *appl, *timer, *gui;
    OObject *os1, *ow1;
    OObject *cpu_check, *cpu_color, *cpu_scroll, *cpu_delay;
    OObject *mem_check, *mem_color, *mem_scroll, *mem_delay;
    OObject *net_check, *net_color, *net_scroll, *net_delay;
    };
struct OP P;


/***************************************************************************
 *
 *      Taglists
 *
 ***************************************************************************/

static ULONG ValueNum = 120;
static OOBuf *ValueString1 = NULL; //"Valeur de String1";
static OOBuf *ValueString2 = NULL; //"Valeur de String2";
static OOBuf *ValueBinary1 = NULL;
static OOBuf *ValueBinary2 = NULL;

OOTagList ConfigTags[] = {
	OOA_ValueName,	(ULONG)"Number120",	OOA_ValueNumber, (ULONG)&ValueNum,
	OOA_SectionName,(ULONG)"Section Toto",
	OOA_ValueName,	(ULONG)"String1",	OOA_ValueString, (ULONG)&ValueString1,
	OOA_ValueName,	(ULONG)"String2",	OOA_ValueString, (ULONG)&ValueString2,
	OOA_SectionName,(ULONG)"Section Titi",
	OOA_ValueName,	(ULONG)"Binary1",	OOA_ValueBinary, (ULONG)&ValueBinary1,
	OOA_ValueName,	(ULONG)"Binary2",	OOA_ValueBinary, (ULONG)&ValueBinary2,
	TAG_END
	};

OOTagList ApplTags[] = {
	OOA_CompagnyIndex	, OOTEXT_TITLE_COMPAGNY,
	OOA_AuthorIndex		, OOTEXT_TITLE_AUTHOR,
	OOA_TitleIndex		, OOTEXT_TITLE_APPL,
	OOA_ImageIndex		, OOPIC_ICON_APPL,
	OOA_ConfigTagList	, (ULONG)ConfigTags,
	TAG_END
	};

OOTagList WinTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_WINDOW1,
	OOA_VertGroup	, TRUE,
	TAG_END
	};

OOTagList HorizGroupTags[] = {
	OOA_HorizGroup		, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};

OOTagList DisplayImageTags[] = {
	OOA_ImageIndex	, OOPIC_ICON_LOGO,
	OOA_BoxType		, OOV_BOX_NULL,
	TAG_END
	};

OOTagList DisplayTextTags[] = {
	OOA_TextIndex	, OOTEXT_COPYRIGHT,
	OOA_BoxType		, OOV_BOX_NULL,
	TAG_END
	};

//-------- CPU

OOTagList CPU_HorizGroupBoxTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_CPUGROUP,
	OOA_FontBold	, TRUE,
	OOA_HorizGroup	, TRUE,
	OOA_Margins		, TRUE,
	TAG_END
	};

OOTagList CPU_CheckerTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_CPUCHECK,
	OOA_Selected	, TRUE,
	OOA_3States		, FALSE,
	TAG_END
	};

OOTagList CPU_ButtonTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_CPUCOLOR,
	OOA_WidthHeight	, W2L(40,20),
	TAG_END
	};

OOTagList CPU_ScrollTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_CPUSCROLL,
	OOA_Horizontal	, TRUE,
	OOA_WidthHeight	, W2L(100,0),
	OOA_Min			, 20,
	OOA_Max			, 9999,
	OOA_Pos    		, 1000,
	//OOA_Visible		, 10000-20,
	TAG_END
	};

OOTagList CPU_EditNumTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_CPUDELAY,
	OOA_TitlePlace	, OOV_RIGHT,
	OOA_Min			, 20,
	OOA_Max			, 9999,
	OOA_Number    	, 1000,
	OOA_NumDigits	, 4,
	TAG_END
	};

//-------- MEM

OOTagList MEM_HorizGroupBoxTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_MEMGROUP,
	OOA_FontBold	, TRUE,
	OOA_HorizGroup	, TRUE,
	OOA_Margins		, TRUE,
	TAG_END
	};

OOTagList MEM_CheckerTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_MEMCHECK,
	OOA_Selected	, TRUE,
	OOA_3States		, FALSE,
	TAG_END
	};

OOTagList MEM_ButtonTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_MEMCOLOR,
	OOA_WidthHeight	, W2L(40,20),
	TAG_END
	};

OOTagList MEM_ScrollTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_MEMSCROLL,
	OOA_Horizontal	, TRUE,
	OOA_WidthHeight	, W2L(100,0),
	OOA_Min			, 20,
	OOA_Max			, 9999,
	OOA_Pos    		, 1000,
	//OOA_Visible		, 10000-20,
	TAG_END
	};

OOTagList MEM_EditNumTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_MEMDELAY,
	OOA_TitlePlace	, OOV_RIGHT,
	OOA_Min			, 20,
	OOA_Max			, 9999,
	OOA_Number    	, 1000,
	OOA_NumDigits	, 4,
	TAG_END
	};

//-------- NET

OOTagList NET_HorizGroupBoxTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_NETGROUP,
	OOA_FontBold	, TRUE,
	OOA_HorizGroup	, TRUE,
	OOA_Margins		, TRUE,
	TAG_END
	};

OOTagList NET_CheckerTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_NETCHECK,
	OOA_Selected	, TRUE,
	OOA_3States		, FALSE,
	TAG_END
	};

OOTagList NET_ButtonTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_NETCOLOR,
	OOA_WidthHeight	, W2L(40,20),
	TAG_END
	};

OOTagList NET_ScrollTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_NETSCROLL,
	OOA_Horizontal	, TRUE,
	OOA_WidthHeight	, W2L(100,0),
	OOA_Min			, 20,
	OOA_Max			, 9999,
	OOA_Pos    		, 1000,
	//OOA_Visible		, 10000-20,
	TAG_END
	};

OOTagList NET_EditNumTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_NETDELAY,
	OOA_TitlePlace	, OOV_RIGHT,
	OOA_Min			, 20,
	OOA_Max			, 9999,
	OOA_Number    	, 1000,
	OOA_NumDigits	, 4,
	TAG_END
	};


/***************************************************************************
 *
 *      Notifications
 *
 ***************************************************************************/

static OONotifyItem NF_Win1[] = {
    { OON_TRIG_EQUAL, OOA_WinCloseAsk, TRUE,0, OF(OP,appl), OON_ACT_DOMETHOD, OOM_SETEVENT, OOA_EventMask, OOF_BREAK_C, TAG_END },
    OOV_ENDTABLE
    };

static OONotifyItem NF_CPU_Scroller[] = {
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,cpu_delay), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Number, 0, TAG_END },
    OOV_ENDTABLE
    };
static OONotifyItem NF_CPU_EditNum[] = {
    { OON_TRIG_EVERYVALUE, OOA_Number, 0,0, OF(OP,cpu_scroll), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };

static OONotifyItem NF_MEM_Scroller[] = {
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,mem_delay), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Number, 0, TAG_END },
    OOV_ENDTABLE
    };
static OONotifyItem NF_MEM_EditNum[] = {
    { OON_TRIG_EVERYVALUE, OOA_Number, 0,0, OF(OP,mem_scroll), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };

static OONotifyItem NF_NET_Scroller[] = {
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,net_delay), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Number, 0, TAG_END },
    OOV_ENDTABLE
    };
static OONotifyItem NF_NET_EditNum[] = {
    { OON_TRIG_EVERYVALUE, OOA_Number, 0,0, OF(OP,net_scroll), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };


/***************************************************************************
 *
 *      Trees
 *
 ***************************************************************************/

static OOTreeItem Tree1[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, ApplTags, NULL ),
		OBJECT( 1, OF(OP,timer),  &Class_HandleTimer, NULL, NULL ),
		OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
			OBJECT( 2, OF(OP,os1),  &Class_AreaScreen, NULL, NULL ),
				OBJECT( 3, OF(OP,ow1),  &Class_AreaWindow, WinTags, NF_Win1 ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, 0 ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlDisplayImage, DisplayImageTags, NULL ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlDisplayText, DisplayTextTags, NULL ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlDisplayImage, DisplayImageTags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroupBox, CPU_HorizGroupBoxTags, NULL ),
						OBJECT( 5, OF(OP,cpu_check),  &Class_CtrlChecker	, CPU_CheckerTags, NULL ),
						OBJECT( 5, OF(OP,cpu_color),  &Class_CtrlButton		, CPU_ButtonTags, NULL ),
						OBJECT( 5, OF(OP,cpu_scroll), &Class_CtrlScroller	, CPU_ScrollTags, NF_CPU_Scroller ),
						OBJECT( 5, OF(OP,cpu_delay),  &Class_CtrlEditNum	, CPU_EditNumTags, NF_CPU_EditNum ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroupBox, MEM_HorizGroupBoxTags, NULL ),
						OBJECT( 5, OF(OP,mem_check),  &Class_CtrlChecker	, MEM_CheckerTags, NULL ),
						OBJECT( 5, OF(OP,mem_color),  &Class_CtrlButton		, MEM_ButtonTags, NULL ),
						OBJECT( 5, OF(OP,mem_scroll), &Class_CtrlScroller	, MEM_ScrollTags, NF_MEM_Scroller ),
						OBJECT( 5, OF(OP,mem_delay),  &Class_CtrlEditNum	, MEM_EditNumTags, NF_MEM_EditNum ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroupBox, NET_HorizGroupBoxTags, NULL ),
						OBJECT( 5, OF(OP,net_check),  &Class_CtrlChecker	, NET_CheckerTags, NULL ),
						OBJECT( 5, OF(OP,net_color),  &Class_CtrlButton		, NET_ButtonTags, NULL ),
						OBJECT( 5, OF(OP,net_scroll), &Class_CtrlScroller	, NET_ScrollTags, NF_NET_Scroller ),
						OBJECT( 5, OF(OP,net_delay),  &Class_CtrlEditNum	, NET_EditNumTags, NF_NET_EditNum ),
    OBJECTEND
    };


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

static ULONG timer1;
static ULONG timer2;

static void func_timer1( ULONG timer, ULONG data )
{
	OOTRACE( "[%d]", data );
}

static void func_timer2( ULONG timer, ULONG data )
{
	OOTRACE( "%d", data );
}

void main( int argc, char **argv )
{
    if (OO_InitV( OOA_Console, TRUE, TAG_END ) == OOV_ERROR) goto END_ERROR;

	OOStr_SetTable(	text_table );
	OOPic_SetTable(	images_table );

	OOBuf_FormatV( &ValueBinary1, 0, "Binaire 1" );
	OOBuf_FormatV( &ValueBinary2, 0, "Binaire 2" );

    if (! OO_NewTree( 0, &P, Tree1 )) goto END_ERROR;
    if (! OO_DoMethodV( P.gui, OOM_PRESENT, OOA_Object, P.gui, TAG_END )) goto END_ERROR;

    if (! OO_DoMethodV( P.timer, OOM_ADDTIMER, OOA_Delay, 1000, OOA_CallbackFunc, func_timer1, OOA_ClientData, 1, TAG_END )) goto END_ERROR;
    if (! OO_DoMethodV( P.timer, OOM_ADDTIMER, OOA_Delay, 500, OOA_CallbackFunc, func_timer2, OOA_ClientData, 2, TAG_END )) goto END_ERROR;
	OO_SetAttr( P.timer, OOA_OnOff, TRUE );

    OO_DoMethodV( P.appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

	ValueNum = 2000;                       
	OOBuf_Set( "ttttt", -1, &ValueString1 );
	OOBuf_Set( "uuuuuuuuuuu", -1, &ValueString2 );  
	OOBuf_Set( "kkkkkkkkkkkkkkkkkkk", -1, &ValueBinary1 );                 
	OOBuf_Set( "llllllllllllllllll", -1, &ValueBinary2 );
	
  END_ERROR:
    OO_Delete( P.appl );
	OOTag_FreeBuffers( ConfigTags );
    OO_Cleanup();
}
/*
static OONotifyItem NF_Radio1[] = {
    { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0, OF(OP,scroll1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };
static OONotifyItem NF_Radio2[] = {
    { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0, OF(OP,scrollv3), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };
static ULONG func_radio3( void ) { return OO_SetAttrsV( P.radio4, OOA_Selected, -1, OOA_ListIndex, OOTEXT_LONGLIST, TAG_END ); }
static OONotifyItem NF_Radio3[] = {
    { OON_TRIG_EQUAL, OOA_PressedButton, TRUE,0, 0, OON_ACT_CALLFUNC, (ULONG)func_radio3 },
    OOV_ENDTABLE
    };
static ULONG func_radio4( void ) { OO_SetAttr( P.radio3, OOA_Selected, -1 ); return OO_SetAttr( P.radio4, OOA_List, (ULONG)"Choix 2\0" ); }
static OONotifyItem NF_Radio4[] = {
    { OON_TRIG_EQUAL, OOA_PressedButton, TRUE,0, 0, OON_ACT_CALLFUNC, (ULONG)func_radio4 },
    OOV_ENDTABLE
    };

static OONotifyItem NF_EditNum1[] = {
    { OON_TRIG_EVERYVALUE, OOA_Number, 0,0, OF(OP,scroll1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };

static OONotifyItem NF_Scroller1[] = {
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,editnum1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Number, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,scrollv3), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,radio1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Selected, 0, TAG_END },
    OOV_ENDTABLE
    };
static OONotifyItem NF_ScrollerV3[] = {
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dispnum2), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Number, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,scroll1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,radio2), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Selected, 0, TAG_END },
    OOV_ENDTABLE
    };

static ULONG close_func1( void ) { printf( "Fermeture demandée, d'accord...\n" ); return OOV_OK; }
static ULONG close_func2( void ) { printf( "Fermeture demandée, PAS d'accord !\n" ); return OOV_CANCEL; }
static OONotifyItem NF_Win1[] = {
    //{ OON_TRIG_EQUAL, OOA_WinCloseAsk, TRUE,0, 0, OON_ACT_CALLFUNC, (ULONG)close_func2 },
    //{ OON_TRIG_EQUAL, OOA_WinCloseAsk, TRUE,0, OF(OP,ow1), OON_ACT_DOMETHOD, OOM_DELETE, TAG_END },
    { OON_TRIG_EQUAL, OOA_WinCloseAsk, TRUE,0, OF(OP,appl), OON_ACT_DOMETHOD, OOM_SETEVENT, OOA_EventMask, OOF_BREAK_C, TAG_END },
    OOV_ENDTABLE
    };
*/
