/******************************************************************
 *
 *       Project:    Test 3D
 *       Function:   Test 0
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
#include "oo_3d.h"
//#include "oo_gui.h"
#include "oo_gfx.h"

#include "data.h"


//******* Imported ************************************************

extern struct ApplObjects ApplObj;


//******* Exported ************************************************


//******* Statics *************************************************

//------ Types

static OO3DefTexture Textures[] = {
	{ "Face 2", "Textures\\de2.bmp", 0, 0, 10, 10, 20, 20, 0 },
	{ "Mur de pierres", "Textures\\tile.bmp", 0, 0, 10, 10, 20, 20, 0 },
	};

static OO3DefMaterial Materials[] = {
	{ "Black",		OORGB(0,0,0),			NULL,NULL,NULL, 0, 0 },
	{ "White",		OORGB(255,255,255),		NULL,NULL,NULL, 0, 0 },
	{ "Light grey",	OORGB(0xa0,0xa0,0xa0),	NULL,NULL,NULL, 0, 0 },
	{ "Dark grey",	OORGB(0x80,0x80,0x80),	NULL,NULL,NULL, 0, 0 },
	{ "Red",		OORGB(255,0,0),			NULL,NULL,NULL, 0, 0 },
	{ "Green",		OORGB(0,255,0),			NULL,NULL,NULL, 0, 0 },
	{ "Blue",		OORGB(0,0,255),			NULL,NULL,NULL, 0, 0 },
	{ "Yellow",		OORGB(0xff,0xff,0x00),	NULL,NULL,NULL, 0, 0 },
	{ "Magenta",	OORGB(0xff,0x00,0xff),	NULL,NULL,NULL, 0, 0 },
	{ "Cyan",		OORGB(0x00,0xff,0xff),	NULL,NULL,NULL, 0, 0 },
	{ "Face 2",			0, &Textures[0], NULL,NULL, 0, 0 },
	{ "Mur de pierres",	0, &Textures[1], NULL,NULL, 0, 0 },
	};

static OO3DPoint Cube_Vertices[] = {
    (FLOAT)-.80, (FLOAT)+.80, (FLOAT)+.80,
    (FLOAT)+.80, (FLOAT)+.80, (FLOAT)+.80,
    (FLOAT)+.80, (FLOAT)-.80, (FLOAT)+.80,
    (FLOAT)-.80, (FLOAT)-.80, (FLOAT)+.80,
    (FLOAT)-.80, (FLOAT)+.80, (FLOAT)-.80,
    (FLOAT)+.80, (FLOAT)+.80, (FLOAT)-.80,
    (FLOAT)+.80, (FLOAT)-.80, (FLOAT)-.80,
    (FLOAT)-.80, (FLOAT)-.80, (FLOAT)-.80,
	};
static OO3DefPolygon Cube_Polygons[] = {
	&Materials[2],	OOF_HIDE|OOF_FILL, 4,	0,3,2,1,
	&Materials[4],	OOF_HIDE|OOF_FILL, 4,	4,7,3,0,
	&Materials[5],	OOF_HIDE|OOF_FILL, 4,	4,0,1,5,
	&Materials[6],	OOF_HIDE|OOF_FILL, 4,	1,2,6,5,
	&Materials[7],	OOF_HIDE|OOF_FILL, 4,	3,7,6,2,
	&Materials[8],	OOF_HIDE|OOF_FILL, 4,	5,6,7,4,
	};
static OO3DefType Cube_Type = { "Cube", 8, sizeof(Cube_Polygons)/sizeof(OO3DefPolygon), Cube_Vertices, Cube_Polygons };

//------ Tags

static OOTagItem TL_H3D[] = {
    D3A_ScreenAddr	,	(ULONG)&ApplObj.Screen,
    D3A_ClearScreen	,	TRUE,
	D3A_WaitVBlank	,	FALSE,
    TAG_END
    };

static OOFltTagItem TL_FltSpace[] = {
    D3A_Gravity,    (FLOAT)10,
    TAGFLT_END
    };
static OOTagItem TL_Space[] = {
    OOV_TAG_MORE, (ULONG)TL_FltSpace,
    };

static OOFltTagItem TL_FltSolid1[] = {
    D3A_X, (FLOAT)0, D3A_SpeedVx, (FLOAT)0,	D3A_SpeedAx, (FLOAT)0,	D3A_RotSIx, (FLOAT)10,
    D3A_Y, (FLOAT)0, D3A_SpeedVy, (FLOAT)0, D3A_SpeedAy, (FLOAT)0,
    D3A_Z, (FLOAT)7, D3A_SpeedVz, (FLOAT)0, D3A_SpeedAz, (FLOAT)0,	D3A_RotSIz, (FLOAT)60,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Solid1[] = {
	D3A_Type		, (ULONG)&Cube_Type,
    //D3A_Sensitivity	, D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltSolid1,
    };

static OOFltTagItem TL_FltCamera1[] = {
    D3A_X, (FLOAT)0,   D3A_WindowWidth,  (FLOAT) 1,
    D3A_Y, (FLOAT)0,   D3A_WindowHeight, (FLOAT).625,   //D3A_RotSAy, (FLOAT)0,
    D3A_Z, (FLOAT)0,   D3A_ObserverDist, (FLOAT) 1,
    D3A_Masse,   (FLOAT)0,
    D3A_Profile, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Camera1[] = {
    D3A_Collide	  , FALSE,
	//D3A_CenterSelf, TRUE ,
    OOV_TAG_MORE, (ULONG)TL_FltCamera1,
    };

static OOTagItem TL_Screen1[] = {
    OOA_AreaAddr  , (ULONG)&ApplObj.Area3D,
    D3A_ConnectObj, (ULONG)&ApplObj.Camera1,
    D3A_FillColor , OORGB(0,0,0),
	TAG_END
    };

//------ Notifications

//------ Tree

static OOTreeItem Tree[] =
    {
	OBJECT( 1, OF(ApplObjects,H3D), &Class_Handle3D, TL_H3D, NULL ),
        OBJECT( 2, OF(ApplObjects,Space), &Class_3DSpace	, TL_Space, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Solid1		, NULL ),
			OBJECT( 3, OF(ApplObjects,Camera1)	, &Class_3DCamera	, TL_Camera1	, NULL ),
			OBJECT( 3, OOV_NOOFFS		, &Class_3DScreen	, TL_Screen1	, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle1		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle2		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle3		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle4		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Balle5		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Solid3		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Ship		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSolid	, TL_Shuttle	, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DSoleil	, TL_Soleil		, NULL ),
//			OBJECT( 3, OOV_NOOFFS		, &Class_3DLight	, TL_Light		, NULL ),
    OBJECTEND
    };


/******************************************************************
 *
 *      Code
 *
 *****************************************************************/

void * Test0_Init( void )
{
	OObject *h3d = NULL;

	if ((h3d = OO_NewTree( ApplObj.Appl, &ApplObj, Tree )) != NULL)
		{
		}

	return h3d;
}
