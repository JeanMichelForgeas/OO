
#include <stdio.h>
#include <stdlib.h>

#include "ootypes.h"
#include "ooprotos.h"

#include "oo_init.h"
#include "oo_class.h"
#include "oo_sound.h"
#include "oo_gfx.h"
#include "oo_gui.h"
#include "oo_3d.h"

static void Test_3D( char opt, int argc, char **argv );
static void Test_Simple( char opt, int argc, char **argv );
static void Test_ObjRot( char opt, int argc, char **argv );
static void Test_Load3DS( char opt, int argc, char **argv );
static void Test_TextureCube( char opt, int argc, char **argv );

struct OptionChoice {
	char	c1, c2;
	void	(*func)( char opt, int argc, char **argv );
	};
static struct OptionChoice OptionChoices[] = { 
	'1','1', Test_3D,
	'2','2', Test_3D,
	'3','3', Test_3D,
	'4','4', Test_3D,
	'S','s', Test_Simple,
	'R','r', Test_ObjRot,
	'L','l', Test_Load3DS,
	'T','t', Test_TextureCube,
	}; 


void main( int argc, char **argv )
{
	ULONG numopts = sizeof(OptionChoices)/sizeof(struct OptionChoice);
	void (*func)( argc, argv );

	if (OO_InitV( OOA_Console, TRUE, TAG_END ) == OOV_ERROR) return;
	if (numopts == 1)
		{
		func = OptionChoices[0].func;
		(*func)( 1, argc, argv );
		printf( "------------ Press RETURN..." );
		fflush( stdout );
		getchar();
		fflush( stdin );
		}
	else{
		for (;;)
			{
			char i, opt = 0;

			if (argc ==	2)
				{
				opt = *argv[1];
				}
			else{
				printf( "===================================\n" );
				printf( "	T E S T   M E N U\n" );
				printf( "\n" );
				printf( "	   1 - Test 320\n" );
				printf( "	   2 - Test 640\n" );
				printf( "	   3 - Test 320 - 50f/s\n" );
				printf( "	   4 - Test 640 - 50f/s\n" );
				printf( "	   S - Simple polygone\n" );
				printf( "	   R - Rotations autour d'objets\n" );
				printf( "	   L - Load 3DS scene\n" );
				printf( "	   T - Cube Texturé\n" );
				printf( "\n" );
				printf( "	   Q - Quit\n" );
				printf( "\n" );
				printf( "Enter option ==> " );
				opt = (char) getchar();
				fflush( stdin );
				}
			if (opt == 'q' || opt == 'Q' || opt == '\n' || opt == '\r')
				break;

			for (func=NULL, i=0; i < sizeof(OptionChoices)/sizeof(struct OptionChoice); i++)
				if (opt == OptionChoices[i].c1 || opt == OptionChoices[i].c2)
					{ func = OptionChoices[i].func; break; }

			if (func != NULL)
				{
				printf( "\n" );
				(*func)( opt, argc, argv );
				printf( "------------ Press RETURN..." );
				getchar();
				fflush( stdin );
				}
			else{ 
				printf( "*** Invalid choice\n" );
				fflush( stdout );
				if (argc > 1) break;
				}
			printf( "\n\n" );
			}
		}
    OO_Cleanup();
}

//****************************************************************
//**************************   Test 1   **************************
//****************************************************************

struct OP {
    OObject *appl, *pic;
    OObject *gui, *os1, *ow1, *ow2, *area1, *area2;
    OObject *h3d, *ospace, *camera1, *camera2, *camera3, *camera4, *camera5,*mike;
	OObject *hdlsnd;
    OObject *soleil, *terre, *lune;
	OObject *Cube;

    };

static struct OP P;

static OOTagItem TL_Scr1[] = {
    OOA_DefaultScreen, TRUE,
    TAG_END
    };

static OOTagItem TL_Win1[] = {
	OOA_Title		, (ULONG)"Fenêtre 1 - 320x200",
    OOA_WinCtlClose	, TRUE,
    OOA_WinCtlDrag	, TRUE,
    OOA_WinCtlDepth	, TRUE,
    OOA_LeftTop		, W2L(10,17),
    OOA_Margins		, FALSE,
    OOA_CloseQuit	, TRUE,
    OOA_FontHeight	, 26,
    TAG_END
    };
static OOTagItem TL_Area1[] = {
    OOA_WidthHeight,    W2L(320,200),
    TAG_END
    };

static OOTagItem TL_Win2[] = {
	OOA_Title		, (ULONG)"Fenêtre 2 - 640x400",
    OOA_WinCtlClose	, TRUE,
    OOA_WinCtlDrag	, TRUE,
    OOA_WinCtlDepth	, TRUE,
    OOA_LeftTop		, W2L(430,17),
    OOA_Margins		, FALSE,
    OOA_CloseQuit	, TRUE,
    TAG_END
    };
static OOTagItem TL_Area2[] = {
    OOA_WidthHeight,    W2L(641,401),
    TAG_END
    };

static OOTagItem TL_H3D[] = {
    D3A_ScreenAddr	, (ULONG)&P.os1,
	D3A_ClearScreen	, TRUE,
	D3A_WaitVBlank	, FALSE,
    TAG_END
    };

#include "Data_Obj_Defs.c"
#include "Data_Obj_Tags.c"

//------------------------------------------------------------------------

static OOTreeItem Tree_320[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, NULL, NULL ),

     OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
        OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, NULL ),
         OBJECT( 3, OF(OP,ow1), &Class_AreaWindow, TL_Win1, NULL ),
            OBJECT( 4, OF(OP,area1), &Class_Area, TL_Area1, NULL ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, NULL ),
        OBJECT( 2, OF(OP,ospace)		, &Class_3DSpace	, TL_Space		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Solid1		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle1		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle2		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle3		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle4		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle5		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Solid3		, NULL ),
			//OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Ship		, NULL ),
			//OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Shuttle	, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSoleil	, TL_Soleil		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DLight	, TL_Light		, NULL ),
			OBJECT( 3, OF(OP,camera1)	, &Class_3DCamera	, TL_Camera1	, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DScreen	, TL_Screen1	, NULL ),


    OBJECTEND
    };

static OONotifyItem NF_KeyTranslate1[] = {
	{ OON_TRIG_EQUAL, OOA_KeyDown, ' '	   ,0, OF(OP,h3d), OON_ACT_DOMETHOD, D3M_STEP3D, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_RETURN,0, OF(OP,h3d), OON_ACT_DOMETHOD, OOM_SET, OOA_OnOff, FALSE, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_LEFT ,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_SUB, D3A_X, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_RIGHT,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_ADD, D3A_X, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_DOWN ,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_SUB, D3A_Z, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_UP   ,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_ADD, D3A_Z, 1, TAG_END },
	OOV_ENDTABLE
	};

static OOTreeItem Tree_640[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, NULL, NULL ),

     OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
			OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, NULL ),
			OBJECT( 3, OF(OP,ow2), &Class_AreaWindow, TL_Win2, NF_KeyTranslate1 ),
				OBJECT( 4, OF(OP,area2), &Class_Area, TL_Area2, NULL ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, NULL ),
        OBJECT( 2, OF(OP,ospace)		, &Class_3DSpace	, TL_Space		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Solid1		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle1		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle2		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle3		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle4		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle5		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Solid3		, NULL ),
			//OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Ship		, NULL ),
			//OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Shuttle	, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSoleil	, TL_Soleil		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DLight	, TL_Light		, NULL ),
			OBJECT( 3, OF(OP,camera2)	, &Class_3DCamera	, TL_Camera2	, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DScreen	, TL_Screen2	, NULL ),

    OBJECTEND
    };

static void Test_3D( char opt, int argc, char **argv )
{
	if (OO_Init3D( NULL ) == OOV_ERROR) goto END_ERROR;

	//------ Synchro 50 f/s
	OOTag_SetData( TL_H3D, D3A_WaitVBlank, (opt > '2') ? TRUE : FALSE );

	//------ 320 ou 640
	if (opt == '1' || opt == '3')
		{ if (OO_NewTree( 0, &P, Tree_320 ) == NULL) goto END_ERROR; }
	else
		{ if (OO_NewTree( 0, &P, Tree_640 ) == NULL) goto END_ERROR; }

    //------ Ouvre l'écran et démarre l'animation
    if (OO_DoMethodV( P.gui, OOM_PRESENT, OOA_Object, P.gui, TAG_END ) == OOV_ERROR) goto END_ERROR;
    if (OO_DoMethod( P.h3d, D3M_START3D, 0 ) == OOV_ERROR) goto END_ERROR;

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethodV( P.appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

  END_ERROR:
	if (P.appl != NULL) OO_Delete( P.appl );
    OO_Cleanup3D();
}

//****************************************************************
//**********************   Simple polygone   *********************
//****************************************************************

static OONotifyItem NF_KeyTranslateS[] = {
	{ OON_TRIG_EQUAL, OOA_KeyDown, ' '	   ,0, OF(OP,h3d), OON_ACT_DOMETHOD, D3M_STEP3D, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_LEFT ,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_SUB, D3A_X, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_RIGHT,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_ADD, D3A_X, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_DOWN ,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_SUB, D3A_Z, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_UP   ,0, OF(OP,camera2), OON_ACT_DOMETHOD, D3M_ADD, D3A_Z, 1, TAG_END },
	OOV_ENDTABLE
	};

static OOTreeItem Tree_Simple[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, NULL, NULL ),

     OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
			OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, NULL ),
			OBJECT( 3, OF(OP,ow2), &Class_AreaWindow, TL_Win2, NF_KeyTranslateS ),
				OBJECT( 4, OF(OP,area2), &Class_Area, TL_Area2, NULL ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, NULL ),
        OBJECT( 2, OF(OP,ospace)		, &Class_3DSpace	, TL_Space		, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_MurS		, NULL ),
			OBJECT( 3, OF(OP,camera2)	, &Class_3DCamera	, TL_Camera2	, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DScreen	, TL_Screen2	, NULL ),

    OBJECTEND
    };

static void Test_Simple( char opt, int argc, char **argv )
{
	if (OO_Init3D( NULL ) == OOV_ERROR) goto END_ERROR;

	//------ Synchro 50 f/s
	OOTag_SetData( TL_H3D, D3A_WaitVBlank, TRUE );

	//------ Tree
	if (OO_NewTree( 0, &P, Tree_Simple ) == NULL) goto END_ERROR;

    //------ Ouvre l'écran et démarre l'animation
    if (OO_DoMethodV( P.gui, OOM_PRESENT, OOA_Object, P.gui, TAG_END ) == OOV_ERROR) goto END_ERROR;
    if (OO_DoMethod( P.h3d, D3M_START3D, 0 ) == OOV_ERROR) goto END_ERROR;
	//OO_SetAttr( P.h3d, OOA_OnOff, FALSE ); // Arrête la 3D

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethodV( P.appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

  END_ERROR:
	if (P.appl != NULL) OO_Delete( P.appl );
    OO_Cleanup3D();
}


//****************************************************************
//**********************   Test rotations   **********************
//****************************************************************

static OOTreeItem Tree_ObjRot[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, NULL, NULL ),

     OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
        OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, NULL ),
          OBJECT( 3, OF(OP,ow2), &Class_AreaWindow, TL_Win2, NULL ),
            OBJECT( 4, OF(OP,area2), &Class_Area, TL_Area2, NULL ),

     OBJECT( 1, OF(OP,hdlsnd), &Class_HandleSound, NULL, NULL ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, NULL ),
        OBJECT( 2, OF(OP,ospace), &Class_3DSpace, TL_Space, NULL ),
			OBJECT( 3, OF(OP,soleil)	, &Class_3DSolid	, TL_PseudoSoleil,	NULL ),
			OBJECT( 3, OF(OP,terre )	, &Class_3DSolid	, TL_PseudoTerre ,	NULL ),
			OBJECT( 3, OF(OP,lune  )	, &Class_3DSolid	, TL_PseudoLune	 ,	NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Mur1		 ,	NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Mur2		 ,	NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Mur3		 ,	NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Mur4		 ,	NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Mur5		 ,	NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DScreen	, TL_Screen3	 ,	NULL ),
			OBJECT( 3, OF(OP,camera1)	, &Class_3DCamera	, TL_Camera1	 ,	NULL ),
			OBJECT( 3, OF(OP,camera2)	, &Class_3DCamera	, TL_Camera2	 ,	NULL ),
			OBJECT( 3, OF(OP,camera3)	, &Class_3DCamera	, TL_Camera3	 ,	NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSpeaker	, TL_Speaker	 ,	NULL ),
			OBJECT( 3, OF(OP,mike)		, &Class_3DMike		, TL_Mike		 ,	NULL ),

    OBJECTEND
    };

// A cause de DirectMerdeSound (WIN32) il faut une fenêtre valide
OOTagList InitSound_TagList[2] = { OOA_AreaAddr, (ULONG)&P.area2, TAG_END };

static void Test_ObjRot( char opt, int argc, char **argv )
{
	OOTagList TL[2] = { OOA_AreaAddr, (ULONG)&P.area2, TAG_END };

	if (OO_Init3D( NULL ) == OOV_ERROR) goto END_ERROR;
	if (OO_InitSound( InitSound_TagList ) == OOV_ERROR) goto END_ERROR;

	//------ Synchro 50 f/s
	OOTag_SetData( TL_H3D, D3A_WaitVBlank, TRUE );

	//------ Construit l'arborescence des objets
	if (OO_NewTree( 0, &P, Tree_ObjRot ) == NULL) goto END_ERROR;

    //------ Ouvre l'écran et démarre l'animation
    if (OO_DoMethodV( P.gui, OOM_PRESENT, OOA_Object, P.gui, TAG_END ) == OOV_ERROR) goto END_ERROR;
	if (OO_DoMethod( P.hdlsnd, OOM_STARTPLAY, 0 ) == OOV_ERROR) goto END_ERROR;
    if (OO_DoMethod( P.h3d, D3M_START3D, 0 ) == OOV_ERROR) goto END_ERROR;

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethodV( P.appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

  END_ERROR:
	if (P.appl != NULL) OO_Delete( P.appl );
    OO_CleanupSound();
    OO_Cleanup3D();
}

//****************************************************************
//**********************   Test load 3DS   ***********************
//****************************************************************
static OONotifyItem NF_KeyTranslate[] = {
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_LEFT ,0, OF(OP,camera4), OON_ACT_DOMETHOD, D3M_SUB, D3A_X, 10, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_RIGHT,0, OF(OP,camera4), OON_ACT_DOMETHOD, D3M_ADD, D3A_X, 10, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_DOWN ,0, OF(OP,camera4), OON_ACT_DOMETHOD, D3M_SUB, D3A_Z, 10, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_UP   ,0, OF(OP,camera4), OON_ACT_DOMETHOD, D3M_ADD, D3A_Z, 10, TAG_END },
	OOV_ENDTABLE
	};

static OOTreeItem Tree_Load3DS[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, NULL, NULL ),

     OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
        OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, NULL ),
          OBJECT( 3, OF(OP,ow2), &Class_AreaWindow, TL_Win2, NF_KeyTranslate ),
            OBJECT( 4, OF(OP,area2), &Class_Area, TL_Area2, NULL ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, NULL ),
        OBJECT( 2, OF(OP,ospace), &Class_3DSpace, TL_Space, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DScreen	, TL_Screen4	 ,	NULL ),
			OBJECT( 3, OF(OP,camera4)	, &Class_3DCamera	, TL_Camera4	 ,	NULL ),

    OBJECTEND
    };

static void Test_Load3DS( char opt, int argc, char **argv )
{
	if (OO_Init3D( NULL ) == OOV_ERROR) goto END_ERROR;

	//------ Synchro 50 f/s
	OOTag_SetData( TL_H3D, D3A_WaitVBlank, TRUE );

	//------ Construit l'arborescence des objets
	if (OO_NewTree( 0, &P, Tree_Load3DS ) == NULL) goto END_ERROR;

    //------ Ouvre l'écran et démarre l'animation
    if (OO_DoMethodV( P.gui, OOM_PRESENT, OOA_Object, P.gui, TAG_END ) == OOV_ERROR) goto END_ERROR;
    if (OO_DoMethod( P.h3d, D3M_START3D, 0 ) == OOV_ERROR) goto END_ERROR;

	//------ CHARGEMENT !!!!!!!!!!!!!!!!
    if (OO_DoMethodV( P.ospace, D3M_LOADSCENE, D3A_SceneFileName, "Scenes\\Studio01.3ds", TAG_END ) == OOV_ERROR) goto END_ERROR;

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethodV( P.appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

  END_ERROR:
	if (P.appl != NULL) OO_Delete( P.appl );
    OO_Cleanup3D();
}

//****************************************************************
//**********************   Test Textures   ***********************
//****************************************************************

static OONotifyItem NF_KeyTranslate2[] = {
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_UP	,0, OF(OP,camera5), OON_ACT_DOMETHOD, D3M_SUB, D3A_Z, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_DOWN	,0, OF(OP,camera5), OON_ACT_DOMETHOD, D3M_ADD, D3A_Z, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_LEFT	,0, OF(OP,camera5), OON_ACT_DOMETHOD, D3M_SUB, D3A_X, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_RIGHT	,0, OF(OP,camera5), OON_ACT_DOMETHOD, D3M_ADD, D3A_X, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_NUMPAD8	,0, OF(OP,Cube), OON_ACT_DOMETHOD, D3M_SUB, D3A_RotSAx, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_NUMPAD2	,0, OF(OP,Cube), OON_ACT_DOMETHOD, D3M_ADD, D3A_RotSAx, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_NUMPAD4	,0, OF(OP,Cube), OON_ACT_DOMETHOD, D3M_SUB, D3A_RotSAy, 1, TAG_END },
	{ OON_TRIG_EQUAL, OOA_KeyDown, VK_NUMPAD6	,0, OF(OP,Cube), OON_ACT_DOMETHOD, D3M_ADD, D3A_RotSAy, 1, TAG_END },
	OOV_ENDTABLE
	};

static OOTreeItem Tree_CubeTexture[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, NULL, NULL ),

     OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
        OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, NULL ),
          OBJECT( 3, OF(OP,ow2), &Class_AreaWindow, TL_Win2, NF_KeyTranslate2 ),
            OBJECT( 4, OF(OP,area2), &Class_Area, TL_Area2, NULL ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, NULL ),
        OBJECT( 2, OF(OP,ospace), &Class_3DSpace, TL_Space, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DScreen	, TL_Screen5, NULL ),
			OBJECT( 3, OF(OP,camera5)	, &Class_3DCamera	, TL_Camera5, NULL ),
			OBJECT( 3, OF(OP,Cube)		, &Class_3DSolid	, TL_Cube	, NULL ),

    OBJECTEND
    };

static void Test_TextureCube( char opt, int argc, char **argv )
{
	if (OO_Init3D( NULL ) == OOV_ERROR) goto END_ERROR;

	//------ Synchro 50 f/s
	OOTag_SetData( TL_H3D, D3A_WaitVBlank, TRUE );

	//------ Construit l'arborescence des objets
	if (OO_NewTree( 0, &P, Tree_CubeTexture ) == NULL) goto END_ERROR;

    //------ Ouvre l'écran et démarre l'animation
    if (OO_DoMethodV( P.gui, OOM_PRESENT, OOA_Object, P.gui, TAG_END ) == OOV_ERROR) goto END_ERROR;
    if (OO_DoMethod( P.h3d, D3M_START3D, 0 ) == OOV_ERROR) goto END_ERROR;

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethodV( P.appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

  END_ERROR:
	if (P.appl != NULL) OO_Delete( P.appl );
    OO_Cleanup3D();
}
