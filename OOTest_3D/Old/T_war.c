
#include <stdio.h>
#include <intuition/screens.h>

#define DISABLE_CTRLC 1

#include "oo_gui.h"
#include "oo_3d.h"

struct OP {
    OObject *appl, *pic;
    OObject *gui, *os1, *ow1, *os2, *ow2, *area1, *step;
    OObject *h3d, *ospace, *oterrain, *ohuman, *ocamera;
    OObject *tx, *ty, *tz, *rx, *ry, *rz, *dn1, *dn2, *dn3;
    };

static struct OP P;

/*------------------------------------------------------------------------*/
/*                              Définition Terrain                        */

static UWORD Terrain_Faces[] = {
    4, 2,4, D3F_FLINES, D3T_FFILL, 1,    0,1,2,3,
    ENDFACES
};
static OO3DPoint Terrain_Coords[] = {
    -500, 0,    0,
     500, 0,    0,
     500, 0, 1000,
    -500, 0, 1000,
};

/*------------------------------------------------------------------------*/
/*                              Définition Humain                         */

static UWORD Human_Faces[] = {
    4, 1,6, D3F_FLINES, D3T_FFILL, 2,    0,1,2,3,
    ENDFACES
};
static OO3Point Human_Coords[] = {
    -0.20,   0, 0,
     0.20,   0, 0,
     0.20, 1.7, 0,
    -0.20, 1.7, 0,
};

/*------------------------------------------------------------------------*/

#include <graphics/videocontrol.h>
struct TagItem vctags[] =
{
    VTAG_BORDERSPRITE_SET, TRUE,
    TAG_DONE, 0,
};

static OOTagItem TL_Scr1[] = {
    OOA_WidthHeight,    W2L(320,200),
    OOA_DoubleBuffer,  TRUE,
    SA_DisplayID,       /*PAL_MONITOR_ID|*/LORES_KEY,
    SA_Overscan,        OSCAN_TEXT,
    SA_VideoControl,    (ULONG)vctags,
    SA_Depth,           3,
    SA_AutoScroll,      TRUE,
    SA_MinimizeISG,     TRUE,
    SA_Quiet,           TRUE,
    SA_Interleaved,     TRUE,
    TAG_END
    };

static OOTagItem TL_Win1[] = {
    OOA_BorderLess,     TRUE,
    OOA_WidthHeight,    W2L(320,200),
    TAG_END
    };
static OOTagItem TL_Area1[] = {
    OOA_LeftTop,        W2L(0,0),
    OOA_WidthHeight,    W2L(320,200),
    TAG_END
    };

/*-----------------------------------------*/

static OOTagItem TL_Scr2[] = {
    OOA_LeftTop,        W2L(0,203),
    OOA_WidthHeight,    W2L(640,57),
    SA_DisplayID,       /*PAL_MONITOR_ID|*/HIRES_KEY,
    SA_Overscan,        OSCAN_TEXT,
    SA_VideoControl,    (ULONG)vctags,
    SA_Depth,           2,
    SA_AutoScroll,      TRUE,
    SA_MinimizeISG,     TRUE,
    SA_Quiet,           TRUE,
    SA_LikeWorkbench,   TRUE,
    TAG_END
    };

static OOTagItem TL_VertGroup[] = { OOA_VertGroup, TRUE, TAG_END };
static OOTagItem TL_HorizGroup[] = { OOA_HorizGroup, TRUE, TAG_END };

static OOTagItem TL_Win2[] = {
    OOA_BorderLess,     TRUE,
    OOA_LeftTop,        W2L(0,0),
    OOA_WidthHeight,    W2L(640,57),
    OOA_HorizGroup,     TRUE,
    TAG_END
    };
static OOTagItem TL_GadPause[] = {
    OOA_Title,          (ULONG)"Pause 3D",
    TAG_END
    };
static OOTagItem TL_GadStep[] = {
    OOA_Title,          (ULONG)"Step 3D",
    OOA_Disabled,       TRUE,
    TAG_END
    };
static OOTagItem TL_GadQuit[] = {
    OOA_Title,          (ULONG)"Quit",
    TAG_END
    };

static OOTagItem TL_DispNum[] = {
    OOA_Weight,         5,
    OOA_BoxType,        OOV_BOX_NULL,
    OOA_NumDigits,      6,
    TAG_END
    };

static OOTagItem TL_SlidTransX[] = { OOA_Title, (ULONG)"X", OOA_Min, -1000, OOA_Max, 1000, TAG_END };
static OOTagItem TL_SlidTransY[] = { OOA_Title, (ULONG)"Y", OOA_Min, -1000, OOA_Max, 1000, TAG_END };
static OOTagItem TL_SlidTransZ[] = { OOA_Title, (ULONG)"Z", OOA_Min, -1000, OOA_Max, 1000, TAG_END };
static OOTagItem TL_SlidRotX  [] = { OOA_Title, (ULONG)"X", OOA_Min, -180, OOA_Max, 181, OOA_Vertical, TRUE, TAG_END };
static OOTagItem TL_SlidRotY  [] = { OOA_Title, (ULONG)"Y", OOA_Min, -180, OOA_Max, 181, OOA_Vertical, TRUE, TAG_END };
static OOTagItem TL_SlidRotZ  [] = { OOA_Title, (ULONG)"Z", OOA_Min, -180, OOA_Max, 181, OOA_Vertical, TRUE, TAG_END };

/*------------------------------------------------------------------------*/

static OOTagItem TL_H3D[] = {
    D3A_ScreenAddr, (ULONG)&P.os1,
    OOA_Filled,     TRUE,
    TAG_END
    };

static OOFltTagItem TL_FltSpace[] = {
    D3A_SizeX,   1000,
    D3A_SizeY,   1000,
    D3A_SizeZ,   1000,
    D3A_Gravity, 10,
    TAG_END
    };
static OOTagItem TL_Space[] = {
    TAG_MORE, (ULONG)TL_FltSpace,
    };

static OOFltTagItem TL_FltTerrain[] = {
    D3A_X,        0,
    D3A_Y,        0,
    D3A_Z,        0,
    D3A_Masse, 1e15,
    TAG_END
    };
static OOTagItem TL_Terrain[] = {
    D3A_DefFaces,  (ULONG)Terrain_Faces,
    D3A_DefCoords, (ULONG)Terrain_Coords,
    TAG_MORE, (ULONG)TL_FltTerrain,
    };

static OOFltTagItem TL_FltHuman[] = {
    D3A_X,  0, D3A_SpeedVx, 0,    D3A_SpeedAx, 0,
    D3A_Y,  0, D3A_SpeedVy, 0,    D3A_SpeedAy, 0,
    D3A_Z, 10, D3A_SpeedVz, 0,    D3A_SpeedAz, 0,
    D3A_Masse, 70,
    TAG_END
    };
static OOTagItem TL_Human[] = {
    D3A_DefFaces,  (ULONG)Human_Faces,
    D3A_DefCoords, (ULONG)Human_Coords,
    TAG_MORE, (ULONG)TL_FltHuman,
    };


static OOFltTagItem TL_FltCamera[] = {
    D3A_WindowWidth,  .02,
    D3A_WindowHeight, .014,
    D3A_ObserverDist, .02,
    D3A_Masse,   0,
    D3A_Profile, 0,
    TAG_END
    };
static OOTagItem TL_Camera[] = {
    OOA_AreaAddr,   (ULONG)&P.area1,
    D3A_Collide,	FALSE,
    D3A_Visible,	FALSE,
    TAG_MORE, (ULONG)TL_FltCamera,
    };

/*--------------------------------------------*/

static OONotifyItem NF_GadPause[] = {
    { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0,     OF(OP,h3d),  OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_Pause3D },
    { OON_TRIG_EQUAL,      OOA_Selected, TRUE,0,  OF(OP,step), OON_ACT_DOMETHOD,      OOM_UPDATE, OOA_Disabled, FALSE },
    { OON_TRIG_EQUAL,      OOA_Selected, FALSE,0, OF(OP,step), OON_ACT_DOMETHOD,      OOM_UPDATE, OOA_Disabled, TRUE },
    NOTIFYEND
    };
static OONotifyItem NF_GadStep[] = {
    { OON_TRIG_EQUAL, OOA_PressedButton, TRUE,0, OF(OP,h3d), OON_ACT_DOMETHOD, D3M_STEP3D },
    NOTIFYEND
    };
static OONotifyItem NF_GadQuit[] = {
    { OON_TRIG_EQUAL, OOA_PressedButton, TRUE,0, OF(OP,appl), OON_ACT_DOMETHOD, OOM_BREAK_C },
    NOTIFYEND
    };

/*--------------------------------------------*/

static OONotifyItem NF_SlidTransX[] = {
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_X, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn1), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
/*
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_X, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn1), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
*/
NOTIFYEND };
static OONotifyItem NF_SlidTransY[] = {
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_Y, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn2), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
NOTIFYEND };
static OONotifyItem NF_SlidTransZ[] = {
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_Z, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn3), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
NOTIFYEND };
static OONotifyItem NF_SlidRotX[] = { { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_RotSAx, 0, TAG_END }, NOTIFYEND };
static OONotifyItem NF_SlidRotY[] = { { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_RotSAy, 0, TAG_END }, NOTIFYEND };
static OONotifyItem NF_SlidRotZ[] = { { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_RotSAz, 0, TAG_END }, NOTIFYEND };

/*--------------------------------------------*/

static OOTreeItem Tree1[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Appl, 0, 0 ),

     OBJECT( 1, OF(OP,pic), &Class_Pic, 0, 0 ),

     OBJECT( 1, OF(OP,gui), &Class_HandleGUI, 0, 0 ),

        OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, 0 ),
         OBJECT( 3, OF(OP,ow1), &Class_AreaWindow, TL_Win1, 0 ),
            OBJECT( 4, OF(OP,area1), &Class_Area,   TL_Area1, 0 ),

        OBJECT( 2, OF(OP,os2), &Class_AreaScreen, TL_Scr2, 0 ),
         OBJECT( 3, OF(OP,ow2), &Class_AreaWindow, TL_Win2, 0 ),

           OBJECT( 4,         -1, &Class_AreaGroup,  TL_VertGroup, 0 ),
                OBJECT( 5,          -1, &Class_GadChecker, TL_GadPause, NF_GadPause ),
                OBJECT( 5, OF(OP,step), &Class_GadButton,  TL_GadStep,  NF_GadStep ),
                OBJECT( 5,          -1, &Class_GadButton,  TL_GadQuit,  NF_GadQuit ),

           OBJECT( 4,         -1, &Class_AreaGroup, TL_HorizGroup, 0 ),
            OBJECT( 5,  OF(OP,rx), &Class_GadSlider, TL_SlidRotX, NF_SlidRotX ),
            OBJECT( 5,  OF(OP,ry), &Class_GadSlider, TL_SlidRotY, NF_SlidRotY ),
            OBJECT( 5,  OF(OP,rz), &Class_GadSlider, TL_SlidRotZ, NF_SlidRotZ ),

           OBJECT( 4,         -1, &Class_AreaGroup, TL_VertGroup, 0 ),
            OBJECT( 5,         -1, &Class_AreaGroup, TL_HorizGroup, 0 ),
             OBJECT( 6, OF(OP,dn1), &Class_GadDispNum, TL_DispNum, 0 ),
             OBJECT( 6,  OF(OP,tx), &Class_GadSlider, TL_SlidTransX, NF_SlidTransX ),
            OBJECT( 5,         -1, &Class_AreaGroup, TL_HorizGroup, 0 ),
             OBJECT( 6, OF(OP,dn2), &Class_GadDispNum, TL_DispNum, 0 ),
             OBJECT( 6,  OF(OP,ty), &Class_GadSlider, TL_SlidTransY, NF_SlidTransY ),
            OBJECT( 5,         -1, &Class_AreaGroup, TL_HorizGroup, 0 ),
             OBJECT( 6, OF(OP,dn3), &Class_GadDispNum, TL_DispNum, 0 ),
             OBJECT( 6,  OF(OP,tz), &Class_GadSlider, TL_SlidTransZ, NF_SlidTransZ ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, 0 ),

        OBJECT( 2, OF(OP,ospace), &Class_3DSpace, TL_Space, 0 ),
         OBJECT( 3, OF(OP,oterrain), &Class_3DSolid  , TL_Terrain, 0 ),
         OBJECT( 3, OF(OP,ohuman)  , &Class_3DHuman  , TL_Human,   0 ),
         OBJECT( 3, OF(OP,ocamera) , &Class_3DCamera , TL_Camera,  0 ),

    OBJECTEND
    };

/*--------------------------------------------*/

void main( int argc, char **argv )
{
  OOVars *vars;

    if (! (vars = OO_Init( OOA_MemDebugFlags, 0xffff, TAG_END ))) goto END_ERROR;

    /*------ Crée les objets */
    if (! OO_NewTreeA( vars, 0, &P, Tree1 )) goto END_ERROR;

    /*------ Pour initialiser le tout en profitant des notifications */
    OO_SetAttrs( P.tx, OOA_Pos,  0, TAG_END );
    OO_SetAttrs( P.ty, OOA_Pos,  1, TAG_END );
    OO_SetAttrs( P.tz, OOA_Pos, -1, TAG_END );
    OO_SetAttrs( P.rx, OOA_Pos,  0, TAG_END );
    OO_SetAttrs( P.ry, OOA_Pos,  0, TAG_END );
    OO_SetAttrs( P.rz, OOA_Pos,  0, TAG_END );

    /*------ Donne une image de fond à l'espace qui transmet les couleurs à l'écran */
    OO_SetAttrs( P.ospace, OOA_PicObj, &P.pic, OOA_PicName, "EXE:Screen.pic", TAG_END );

    /*------ Ouvre l'écran et démarre l'animation */
    if (! OO_DoMethod( P.gui, OOM_PRESENT, 0, 0, P.gui )) goto END_ERROR;
    if (! OO_DoMethod( P.h3d, D3M_START3D, 0 )) goto END_ERROR;

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethod( P.appl, OOM_WAITEVENTS, 0, OOF_BREAK_C, 0, 1 );

  END_ERROR:
    OO_DisposeObject( P.appl );
    OO_Cleanup( vars );
}
