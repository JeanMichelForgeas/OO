
#include <stdio.h>
#include <intuition/intuition.h>
#define DISABLE_CTRLC 1
#include "oo_gui.h"
#include "oo_3d.h"

struct OP {
    OObject *appl, *pic;
    OObject *gui, *os1, *ow1, *os2, *ow2, *area1, *step;
    OObject *h3d, *ospace, *osol1, *osol2, *osol3, *osol4, *ocamera;
    OObject *tx, *ty, *tz, *rx, *ry, *rz, *dn1, *dn2, *dn3;
    };

static struct OP P;

static void BoingSpeedFunc( OOInst_3DSolid * );
static void ShipSpeedFunc( OOInst_3DSolid * );

/*------------------------------------------------------------------------*/
/*                              Définition petite pyramide                */

static UWORD Pyramide_Faces[] = {
    3, B2W(13,0), D3F_FLINES, 0, 0,    0,1,3,
    3, B2W(13,0), D3F_FLINES, 0, 0,    2,0,3,
    3, B2W(13,0), D3F_FLINES, 0, 0,    2,3,1,
    3, B2W(13,0), D3F_FLINES, 0, 0,    2,1,0,
    ENDFACES
};
static OO3DPoint Pyramide_Coords[] = {
    -.40, -.20,  .20,
     .40, -.20,  .20,
       0,  .40,  .20,
       0,    0, -.40,
};

/*------------------------------------------------------------------------*/
/*                              Définition balle Boing                    */

static UWORD Boing_Faces[] = {
    4, B2W(0,9), D3F_FHIDE, D3T_FFILL, 0,    1,0,3,4,
    4, B2W(0,9), D3F_FHIDE, D3T_FFILL, 0,    3,2,6,7,
    4, B2W(0,9), D3F_FHIDE, D3T_FFILL, 0,    5,4,8,9,
    4, B2W(0,9), D3F_FHIDE, D3T_FFILL, 0,    8,7,10,11,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    16,15,12,13,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    19,18,14,15,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    21,20,16,17,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    23,22,19,20,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    0,12,14,2,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    6,18,22,10,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    11,23,21,9,
    4, B2W(0,10), D3F_FHIDE, D3T_FFILL, 0,    5,17,13,1,
    3, B2W(0,5), D3F_FHIDE, D3T_FFILL, 0,    0,2,3,
    3, B2W(0,5), D3F_FHIDE, D3T_FFILL, 0,    5,1,4,
    4, B2W(0,4), D3F_FHIDE, D3T_FFILL, 0,    4,3,7,8,
    3, B2W(0,5), D3F_FHIDE, D3T_FFILL, 0,    7,6,10,
    3, B2W(0,5), D3F_FHIDE, D3T_FFILL, 0,    9,8,11,
    3, B2W(0,7), D3F_FHIDE, D3T_FFILL, 0,    15,14,12,
    3, B2W(0,7), D3F_FHIDE, D3T_FFILL, 0,    17,16,13,
    4, B2W(0,7), D3F_FHIDE, D3T_FFILL, 0,    20,19,15,16,
    3, B2W(0,7), D3F_FHIDE, D3T_FFILL, 0,    22,18,19,
    3, B2W(0,7), D3F_FHIDE, D3T_FFILL, 0,    21,23,20,
    4, B2W(0,6), D3F_FHIDE, D3T_FFILL, 0,    1,13,12,0,
    4, B2W(0,6), D3F_FHIDE, D3T_FFILL, 0,    2,14,18,6,
    4, B2W(0,6), D3F_FHIDE, D3T_FFILL, 0,    10,22,23,11,
    4, B2W(0,6), D3F_FHIDE, D3T_FFILL, 0,    9,21,17,5,
    ENDFACES
};
static OO3DPoint Boing_Coords[] = {
    -.50,  .17, -.17,
    -.50,  .17,  .17,
    -.17,  .17, -.50,
    -.17,  .50, -.17,
    -.17,  .50,  .17,
    -.17,  .17,  .50,
     .17,  .17, -.50,
     .17,  .50, -.17,
     .17,  .50,  .17,
     .17,  .17,  .50,
     .50,  .17, -.17,
     .50,  .17,  .17,
    -.50, -.17, -.17,
    -.50, -.17,  .17,
    -.17, -.17, -.50,
    -.17, -.50, -.17,
    -.17, -.50,  .17,
    -.17, -.17,  .50,
     .17, -.17, -.50,
     .17, -.50, -.17,
     .17, -.50,  .17,
     .17, -.17,  .50,
     .50, -.17, -.17,
     .50, -.17,  .17,
};

/*------------------------------------------------------------------------*/
/*                              Définition Cube                           */

/*        5_________6
 *       /|         /
 *     1/_________2/|
 *      | |   0   | |
 *      |8|_______|_|7
 *      |/        |/
 *     4|_________|3
 */

static UWORD Cube_Faces[] = {
    4, B2W(4,2), D3F_FHIDE, D3T_FFILL, 0,    0,3,2,1,
    4, B2W(4,3), D3F_FHIDE, D3T_FFILL, 0,    4,7,3,0,
    4, B2W(4,4), D3F_FHIDE, D3T_FFILL, 0,    4,0,1,5,
    4, B2W(4,5), D3F_FHIDE, D3T_FFILL, 0,    1,2,6,5,
    4, B2W(4,6), D3F_FHIDE, D3T_FFILL, 0,    3,7,6,2,
    4, B2W(4,7), D3F_FHIDE, D3T_FFILL, 0,    5,6,7,4,
    ENDFACES
};

static OO3DPoint Cube_Coords[] = {
    -.80, +.80, -.80,
    +.80, +.80, -.80,
    +.80, -.80, -.80,
    -.80, -.80, -.80,
    -.80, +.80, +.80,
    +.80, +.80, +.80,
    +.80, -.80, +.80,
    -.80, -.80, +.80,
};

/*------------------------------------------------------------------------*/
/*                              Définition vaisseau                       */

static UWORD Vaisseau_Faces[] = {
    4, B2W(1,7),  D3F_FHIDE, D3T_FFILL,  0,    0,1,2,3,       /* dessus cokpit */
    3, B2W(12,14),D3F_FHIDE, D3T_FFILL,  1,    1,4,2,         /* cokpit droit */
    3, B2W(12,14),D3F_FHIDE, D3T_FFILL,  2,    3,5,0,         /* cokpit gauche */
    4, B2W(12,15),D3F_FHIDE, D3T_FFILL,  3,    2,4,5,3,       /* cokpit avant */
    4, B2W(1,7),  D3F_FHIDE, D3T_FFILL,  4,    4,6,7,5,       /* pointe avant */
    3, B2W(1,6),  D3F_FHIDE, D3T_FFILL,  5,    1,6,4,         /* pointe avant droite */
    3, B2W(1,6),  D3F_FHIDE, D3T_FFILL,  6,    0,5,7,         /* pointe avant gauche */
    3, B2W(1,5),  D3F_FHIDE, D3T_FFILL,  7,    1,9,6,         /* pointe avant droite latérale */
    3, B2W(1,5),  D3F_FHIDE, D3T_FFILL,  8,    0,7,8,         /* pointe avant gauche latérale */
    4, B2W(1,1),  D3F_FHIDE, D3T_FFILL,  9,    10,12,13,11,   /* dessous */
    4, B2W(1,2),  D3F_FHIDE, D3T_FFILL, 10,    7,6,12,10,     /* dessous avant */
    4, B2W(1,2),  D3F_FHIDE, D3T_FFILL, 11,    11,13,15,14,   /* dessous arrière */
    3, B2W(1,3),  D3F_FHIDE, D3T_FFILL, 12,    8,7,10,        /* dessous avant gauche */
    3, B2W(1,2),  D3F_FHIDE, D3T_FFILL, 13,    8,10,11,       /* dessous gauche */
    3, B2W(1,3),  D3F_FHIDE, D3T_FFILL, 14,    8,11,14,       /* dessous arrière gauche */
    3, B2W(1,3),  D3F_FHIDE, D3T_FFILL, 15,    6,9,12,        /* dessous avant droit */
    3, B2W(1,2),  D3F_FHIDE, D3T_FFILL, 16,    9,13,12,       /* dessous droit */
    3, B2W(1,3),  D3F_FHIDE, D3T_FFILL, 17,    9,15,13,       /* dessous arrière droit */
    3, B2W(1,4),  D3F_FHIDE, D3T_FFILL, 18,    8,14,17,       /* côté gauche */
    3, B2W(1,5),  D3F_FHIDE, D3T_FFILL, 19,    0,8,17,        /* côté gauche dessus */
    3, B2W(1,4),  D3F_FHIDE, D3T_FFILL, 20,    9,16,15,       /* côté droit */
    3, B2W(1,5),  D3F_FHIDE, D3T_FFILL, 21,    9,1,16,        /* côté droit dessus */
    4, B2W(1,6),  D3F_FHIDE, D3T_FFILL, 22,    1,0,17,16,     /* dessus arrière */
    4, B2W(1,3),  D3F_FHIDE, D3T_FFILL, 23,    14,15,16,17,   /* derrière */
    4, B2W(1,8),  D3F_FHIDE, D3T_FFILL, 24,    18,19,20,21,   /* tuyère */
    4, B2W(1,6),          0, D3T_FFILL, 25,    8,25,26,27,    /* aileron avant gauche */
    4, B2W(1,6),          0, D3T_FFILL, 26,    22,9,24,23,    /* aileron avant droit */
    ENDFACES
    };

static OO3DPoint Vaisseau_Coords[] = {
     1.20,  1.00, -1.00,  /*  0  dessus avant */
    -1.20,  1.00, -1.00,  /*  1 */
     -.50,  1.00, -2.00,  /*  2 */
      .50,  1.00, -2.00,  /*  3  cokpit */
     -.50,   .40, -2.50,  /*  4 */
      .50,   .40, -2.50,  /*  5 */
     -.50,     0, -4.00,  /*  6  avant */
      .50,     0, -4.00,  /*  7 */
     1.50,     0, -1.00,  /*  8 */
    -1.50,     0, -1.00,  /*  9 */
      .50, -1.00, -2.00,  /* 10  dessous */
      .50, -1.00,  -.50,  /* 11 */
     -.50, -1.00, -2.00,  /* 12 */
     -.50, -1.00,  -.50,  /* 13 */
     1.00,  -.30,  3.00,  /* 14  arrière */
    -1.00,  -.30,  3.00,  /* 15 */
    -1.50,   .30,  3.00,  /* 16 */
     1.50,   .30,  3.00,  /* 17 */

     1.00,  -.20,  3.01,  /* 18  tuyère */
    -1.00,  -.20,  3.01,  /* 19 */
    -1.20,   .20,  3.01,  /* 20 */
     1.20,   .20,  3.01,  /* 21 */
    -1.00,     0, -2.50,  /* 22  aileron avant droit */
    -2.50,     0, -2.00,  /* 23 */
    -2.50,     0, -1.00,  /* 24 */
     1.00,     0, -2.50,  /* 25  aileron avant gauche */
     2.50,     0, -2.00,  /* 26 */
     2.50,     0, -1.00,  /* 27 */
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
    OOA_DoubleBuffer,   TRUE,
    SA_DisplayID,       /*PAL_MONITOR_ID|*/LORES_KEY,
    SA_Overscan,        OSCAN_TEXT,
    SA_VideoControl,    (ULONG)vctags,
    SA_Depth,           4,
    SA_AutoScroll,      TRUE,
    SA_MinimizeISG,     TRUE,
    SA_Quiet,           TRUE,
    SA_Interleaved,     TRUE,
    TAG_END
    };

static OOTagItem TL_Win1[] = {
    OOA_Borderless,     TRUE,
    OOA_Margins,        FALSE,
    TAG_END
    };
static OOTagItem TL_Area1[] = {
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
    OOA_LeftTop,        W2L(0,0),
    OOA_WidthHeight,    W2L(640,57),
    OOA_Borderless,     TRUE,
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

static OOTagItem TL_SlidTransX   [] = { OOA_Title, (ULONG)"X", OOA_Min, -1000, OOA_Max, 1000, TAG_END };
static OOTagItem TL_SlidTransY   [] = { OOA_Title, (ULONG)"Y", OOA_Min, -1000, OOA_Max, 1000, TAG_END };
static OOTagItem TL_SlidTransZ   [] = { OOA_Title, (ULONG)"Z", OOA_Min, -1000, OOA_Max, 1000, TAG_END };
static OOTagItem TL_SlidRotX     [] = { OOA_Title, (ULONG)"X", OOA_Min, -180, OOA_Max, 181, OOA_Vertical, TRUE, TAG_END };
static OOTagItem TL_SlidRotY     [] = { OOA_Title, (ULONG)"Y", OOA_Min, -180, OOA_Max, 181, OOA_Vertical, TRUE, TAG_END };
static OOTagItem TL_SlidRotZ     [] = { OOA_Title, (ULONG)"Z", OOA_Min, -180, OOA_Max, 181, OOA_Vertical, TRUE, TAG_END };

/*------------------------------------------------------------------------*/

static OOTagItem TL_H3D[] = {
    D3A_ScreenAddr, (ULONG)&P.os1,
    OOA_Filled,     TRUE,
    TAG_END
    };

static OOFltTagItem TL_FltSpace[] = {
    D3A_Gravity,    10,
    TAG_END
    };
static OOTagItem TL_Space[] = {
    TAG_MORE, (ULONG)TL_FltSpace,
    };

static OOFltTagItem TL_FltSolid1[] = {
    D3A_X,-2,  D3A_SpeedVx, 0,    D3A_SpeedAx, 0,     D3A_RotSIx, 1,
    D3A_Y, 0,  D3A_SpeedVy, 0,    D3A_SpeedAy, 0,
    D3A_Z, 1,  D3A_SpeedVz, 0,    D3A_SpeedAz, 0,
    D3A_Masse, 100,
    TAG_END
    };
static OOTagItem TL_Solid1[] = {
    D3A_DefFaces,  (ULONG)Pyramide_Faces,
    D3A_DefCoords, (ULONG)Pyramide_Coords,
    TAG_MORE, (ULONG)TL_FltSolid1,
    };

static OOFltTagItem TL_FltSolid2[] = {
    D3A_X,  0, D3A_SpeedVx, .015,   D3A_SpeedAx, 0,
    D3A_Y,  0, D3A_SpeedVy,   0,    D3A_SpeedAy, 0,     D3A_RotSIy, .6,
    D3A_Z,  4, D3A_SpeedVz, .02,    D3A_SpeedAz, 0,                     D3A_RotSAz, 20,
    D3A_Masse, 100,
    TAG_END
    };
static OOTagItem TL_Solid2[] = {
    D3A_DefFaces,  (ULONG)Boing_Faces,
    D3A_DefCoords, (ULONG)Boing_Coords,
    D3A_UserSpeedFunc,  (ULONG)BoingSpeedFunc,
    TAG_MORE, (ULONG)TL_FltSolid2,
    };

static OOFltTagItem TL_FltSolid3[] = {
    D3A_X, 5, D3A_SpeedVx, 0,    D3A_SpeedAx, 0,     D3A_RotSIx, .2,
    D3A_Y, 0, D3A_SpeedVy, 0,    D3A_SpeedAy, 0,
    D3A_Z, 7, D3A_SpeedVz, 0,    D3A_SpeedAz, 0,     D3A_RotSIz, .4,
    D3A_Masse, 100,
    TAG_END
    };
static OOTagItem TL_Solid3[] = {
    D3A_DefFaces,  (ULONG)Cube_Faces,
    D3A_DefCoords, (ULONG)Cube_Coords,
    TAG_MORE, (ULONG)TL_FltSolid3,
    };

static OOFltTagItem TL_FltSolid4[] = {
    D3A_X,  0,  D3A_RotSIx, -.3,
    D3A_Y, -1,  D3A_RotSIy, -.5,
    D3A_Z,  4,  D3A_RotSIz, -.3,
    D3A_Masse, 1000,
    TAG_END
    };
static OOTagItem TL_Solid4[] = {
    D3A_DefFaces,  (ULONG)Vaisseau_Faces,
    D3A_DefCoords, (ULONG)Vaisseau_Coords,
    D3A_UserSpeedFunc,  (ULONG)ShipSpeedFunc,
    TAG_MORE, (ULONG)TL_FltSolid4,
    };

static OOFltTagItem TL_FltCamera[] = {
    D3A_WindowWidth,   1,
    D3A_WindowHeight, .7,
    D3A_ObserverDist,  1,
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

static OONotifyItem NF_SlidTransX[] = {
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_X, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn1), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
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

     /*------ Interface */
     OBJECT( 1, OF(OP,gui), &Class_HandleGUI, 0, 0 ),

        /*--- Ecran de visu */
        OBJECT( 2, OF(OP,os1), &Class_AreaScreen, TL_Scr1, 0 ),
         OBJECT( 3, OF(OP,ow1), &Class_AreaWindow, TL_Win1, 0 ),
            OBJECT( 4, OF(OP,area1), &Class_Area,   TL_Area1, 0 ),

        /*--- Ecran d'interface */
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

     /*------ Monde 3D */
     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, 0 ),

        OBJECT( 2, OF(OP,ospace), &Class_3DSpace, TL_Space, 0 ),
         OBJECT( 3, OF(OP,osol1)  , &Class_3DSolid,  TL_Solid1, 0 ),
         OBJECT( 3, OF(OP,osol2)  , &Class_3DSolid,  TL_Solid2, 0 ),
         OBJECT( 3, OF(OP,osol3)  , &Class_3DSolid,  TL_Solid3, 0 ),
         OBJECT( 3, OF(OP,osol4)  , &Class_3DSolid,  TL_Solid4, 0 ),
         OBJECT( 3, OF(OP,ocamera), &Class_3DCamera, TL_Camera, 0 ),

    OBJECTEND
    };

/*--------------------------------------------*/

void main( int argc, char **argv )
{
  OOVars *vars;
  long i;

    for (i=1; i < argc; i++)
        {
        if (argv[i][0] == 'd') OOTag_SetTagData( OOA_DoubleBuffer, FALSE, TL_Scr1 );
        if (argv[i][0] == 'f') OOTag_SetTagData( OOA_Filled, FALSE, TL_H3D );
        }

    if (! (vars = OO_Init( OOA_MemDebugFlags, 0xffff, TAG_END ))) goto END_ERROR;

    /*------ Crée les objets */
    if (! OO_NewTreeA( vars, 0, &P, Tree1 )) goto END_ERROR;

    /*------ Pour initialiser le tout en profitant des notifications */
    OO_SetAttrs( P.tx, OOA_Pos,  1, TAG_END );
    OO_SetAttrs( P.ty, OOA_Pos,  1, TAG_END );
    OO_SetAttrs( P.tz, OOA_Pos, -7, TAG_END );
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

/*--------------- Fonction de décision pour la vitesse de la balle */

static void BoingSpeedFunc( OOInst_3DSolid *id )
{
    if (FCMP(id->X,(FLOAT)-4) <= 0)
        {
        id->Speed.Vx = FABS(id->Speed.Vx);
        id->Speed.Vz = FABS(id->Speed.Vz);
        }
    else if (FCMP(id->X,(FLOAT)2.5) >= 0)
        {
        id->Speed.Vx = FNEG(FABS(id->Speed.Vx));
        id->Speed.Vz = FNEG(FABS(id->Speed.Vz));
        }
}

/*--------------- Fonction de décision pour la rotation du vaisseau (angles en radians) */

static void ShipSpeedFunc( OOInst_3DSolid *id )
{
    if (FCMP(id->Rot.SAx,(FLOAT)-.3) <= 0) id->Rot.SIx = FABS(id->Rot.SIx);
    else if (FCMP(id->Rot.SAx,(FLOAT)+.3) >= 0) id->Rot.SIx = FNEG(FABS(id->Rot.SIx));

    if (FCMP(id->Rot.SAz,(FLOAT)-.3) <= 0) id->Rot.SIz = FABS(id->Rot.SIz);
    else if (FCMP(id->Rot.SAz,(FLOAT)+.3) >= 0) id->Rot.SIz = FNEG(FABS(id->Rot.SIz));
}
