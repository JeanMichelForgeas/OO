
#include <stdio.h>
#include <intuition/screens.h>

#define DISABLE_CTRLC 1

#include <oo/oogui.h>
#include <oo/oo3d.h>

struct OP {
    OObject *appl;
    OObject *gui, *os1, *ow1, *os2, *ow2, *area1, *step;
    OObject *h3d, *ospace, *ocamera;
    OObject *dn1, *dn2, *dn3;
    OObject *soleil ;
    OObject *mercure;
    OObject *venus  ;
    OObject *terre  ;
    OObject *mars   ;
    OObject *jupiter;
    OObject *saturne;
    OObject *uranus ;
    OObject *neptune;
    OObject *pluton ;
    };

static struct OP P;

/**************************************************************************/
/*                       Interface Utilisateur                            */

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
    OOA_Borderless,     TRUE,
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
    OOA_Borderless,     TRUE,
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

static OOTagItem TL_SlidTime     [] = { OOA_Title, (ULONG)"T", OOA_Pos,  0, OOA_Min, 0, OOA_Max, MAXWORD, OOA_Vertical, TRUE, TAG_END };

static OOTagItem TL_SlidTransX   [] = { OOA_Title, (ULONG)"X", OOA_Pos, 10, OOA_Min, -10000, OOA_Max, 10000, TAG_END };
static OOTagItem TL_SlidTransY   [] = { OOA_Title, (ULONG)"Y", OOA_Pos,  6, OOA_Min, -10000, OOA_Max, 10000, TAG_END };
static OOTagItem TL_SlidTransZ   [] = { OOA_Title, (ULONG)"Z", OOA_Pos, 50, OOA_Min, -10000, OOA_Max, 10000, TAG_END };
static OOTagItem TL_SlidRotX     [] = { OOA_Title, (ULONG)"X", OOA_Pos,  0, OOA_Min, -1800, OOA_Max, 1810, OOA_Vertical, TRUE, TAG_END };
static OOTagItem TL_SlidRotY     [] = { OOA_Title, (ULONG)"Y", OOA_Pos,  0, OOA_Min, -1800, OOA_Max, 1810, OOA_Vertical, TRUE, TAG_END };
static OOTagItem TL_SlidRotZ     [] = { OOA_Title, (ULONG)"Z", OOA_Pos,  0, OOA_Min, -1800, OOA_Max, 1810, OOA_Vertical, TRUE, TAG_END };

/**************************************************************************/
/*                                Handle 3D                               */

static OOTagItem TL_H3D[] = {
    D3A_ScreenAddr, (ULONG)&P.os1,
    OOA_Filled,     TRUE,
    TAG_END
    };

/*------------------------------------------------------------------------*/
/*                            Définition Camera                           */

static OOFltTagItem TL_FltCamera[] = {
    D3A_X, 0,     D3A_WindowWidth,  .02,
    D3A_Y, 0,     D3A_WindowHeight, .012, /*D3A_RotSIy, 1,*/
    D3A_Z, -1.49597870e11, D3A_ObserverDist, .13,
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

/*------------------------------------------------------------------------*/
/*                            Définition Planètes                         */

static OOFltTagItem TL_FltMercure [] = { D3A_Planet_Id, D3ID_MERCURE, D3A_X, 1.0e9, TAG_END };
static OOFltTagItem TL_FltVenus   [] = { D3A_Planet_Id, D3ID_VENUS  , D3A_X, 1.2e9, TAG_END };
static OOFltTagItem TL_FltTerre   [] = { D3A_Planet_Id, D3ID_TERRE  , D3A_X, 1.4e9, TAG_END };
static OOFltTagItem TL_FltMars    [] = { D3A_Planet_Id, D3ID_MARS   , D3A_X, 1.6e9, TAG_END };
static OOFltTagItem TL_FltJupiter [] = { D3A_Planet_Id, D3ID_JUPITER, D3A_X, 1.8e9, TAG_END };
static OOFltTagItem TL_FltSaturne [] = { D3A_Planet_Id, D3ID_SATURNE, D3A_X, 2.0e9, TAG_END };
static OOFltTagItem TL_FltUranus  [] = { D3A_Planet_Id, D3ID_URANUS , D3A_X, 2.2e9, TAG_END };
static OOFltTagItem TL_FltNeptune [] = { D3A_Planet_Id, D3ID_NEPTUNE, D3A_X, 2.4e9, TAG_END };
static OOFltTagItem TL_FltPluton  [] = { D3A_Planet_Id, D3ID_PLUTON , D3A_X, 2.6e9, TAG_END };
static OOTagItem TL_Mercure [] = { D3A_Planet_Id, D3ID_MERCURE, TAG_MORE, (ULONG)TL_FltMercure };
static OOTagItem TL_Venus   [] = { D3A_Planet_Id, D3ID_VENUS  , TAG_MORE, (ULONG)TL_FltVenus   };
static OOTagItem TL_Terre   [] = { D3A_Planet_Id, D3ID_TERRE  , TAG_MORE, (ULONG)TL_FltTerre   };
static OOTagItem TL_Mars    [] = { D3A_Planet_Id, D3ID_MARS   , TAG_MORE, (ULONG)TL_FltMars    };
static OOTagItem TL_Jupiter [] = { D3A_Planet_Id, D3ID_JUPITER, TAG_MORE, (ULONG)TL_FltJupiter };
static OOTagItem TL_Saturne [] = { D3A_Planet_Id, D3ID_SATURNE, TAG_MORE, (ULONG)TL_FltSaturne };
static OOTagItem TL_Uranus  [] = { D3A_Planet_Id, D3ID_URANUS , TAG_MORE, (ULONG)TL_FltUranus  };
static OOTagItem TL_Neptune [] = { D3A_Planet_Id, D3ID_NEPTUNE, TAG_MORE, (ULONG)TL_FltNeptune };
static OOTagItem TL_Pluton  [] = { D3A_Planet_Id, D3ID_PLUTON , TAG_MORE, (ULONG)TL_FltPluton  };

/**************************************************************************/
/*                              Notifications                             */

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
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_WindowWidth, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn1), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
NOTIFYEND };
static OONotifyItem NF_SlidTransY[] = {
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_WindowHeight, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn2), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
NOTIFYEND };
static OONotifyItem NF_SlidTransZ[] = {
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_ObserverDist, 0, TAG_END },
{ OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dn3), OON_ACT_DOMETHODVALUE, OOM_UPDATE, OOA_Number, 0, TAG_END },
NOTIFYEND };

/* On pourrait profiter ici aussi de la conversion automatique long->float
 * et exécuter une méthode directement en mettant OON_ACT_DOMETHODVALUE, OOM_UPDATE.
 * Mais pour avoir des angles plus petits on va diviser par 100 pour avoir une plage
 * en fractions de degré (de -18° à +18°), donc on passe par une fonction.
 */
ULONG rot_func( OObject *srcobj, ULONG attreçu, ULONG valreçue, ULONG min, ULONG max, OObject *destobj, OOFltTagItem *taglist )
{
    taglist->ti_Flt = FDIV(F2FLT(valreçue),(FLOAT)100);
    OO_DoMethod( destobj, OOM_UPDATE, (OOTagItem*)taglist );
    return(1);
}
static OONotifyItem NF_SlidRotX[] = { { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_CALLFUNC, (ULONG)rot_func, D3A_RotSAx, 0, TAG_END }, NOTIFYEND };
static OONotifyItem NF_SlidRotY[] = { { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_CALLFUNC, (ULONG)rot_func, D3A_RotSAy, 0, TAG_END }, NOTIFYEND };
static OONotifyItem NF_SlidRotZ[] = { { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ocamera), OON_ACT_CALLFUNC, (ULONG)rot_func, D3A_RotSAz, 0, TAG_END }, NOTIFYEND };

static OONotifyItem NF_SlidTime[] = { { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,ospace), OON_ACT_DOMETHODVALUE, OOM_UPDATE, D3A_TimeAccel, 0, TAG_END }, NOTIFYEND };

/**************************************************************************/

static OOTreeItem Tree1[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Appl, 0, 0 ),

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
            OBJECT( 5,         -1, &Class_GadSlider, TL_SlidTime, NF_SlidTime ),
            OBJECT( 5,         -1, &Class_GadSlider, TL_SlidRotX, NF_SlidRotX ),
            OBJECT( 5,         -1, &Class_GadSlider, TL_SlidRotY, NF_SlidRotY ),
            OBJECT( 5,         -1, &Class_GadSlider, TL_SlidRotZ, NF_SlidRotZ ),

           OBJECT( 4,         -1, &Class_AreaGroup, TL_VertGroup, 0 ),
            OBJECT( 5,         -1, &Class_AreaGroup, TL_HorizGroup, 0 ),
             OBJECT( 6, OF(OP,dn1), &Class_GadDispNum, TL_DispNum, 0 ),
             OBJECT( 6,         -1, &Class_GadSlider, TL_SlidTransX, NF_SlidTransX ),
            OBJECT( 5,         -1, &Class_AreaGroup, TL_HorizGroup, 0 ),
             OBJECT( 6, OF(OP,dn2), &Class_GadDispNum, TL_DispNum, 0 ),
             OBJECT( 6,         -1, &Class_GadSlider, TL_SlidTransY, NF_SlidTransY ),
            OBJECT( 5,         -1, &Class_AreaGroup, TL_HorizGroup, 0 ),
             OBJECT( 6, OF(OP,dn3), &Class_GadDispNum, TL_DispNum, 0 ),
             OBJECT( 6,         -1, &Class_GadSlider, TL_SlidTransZ, NF_SlidTransZ ),

     OBJECT( 1, OF(OP,h3d), &Class_Handle3D, TL_H3D, 0 ),

        OBJECT( 2, OF(OP,ospace), &Class_3DSpace, 0, 0 ),
         OBJECT( 3, OF(OP,ocamera), &Class_3DCamera, TL_Camera , 0 ),
         OBJECT( 3, OF(OP,soleil ), &Class_3DSoleil, 0 , 0 ),
         OBJECT( 3, OF(OP,mercure), &Class_3DPlanet, TL_Mercure, 0 ),
         OBJECT( 3, OF(OP,venus  ), &Class_3DPlanet, TL_Venus  , 0 ),
         OBJECT( 3, OF(OP,terre  ), &Class_3DPlanet, TL_Terre  , 0 ),
         OBJECT( 3, OF(OP,mars   ), &Class_3DPlanet, TL_Mars   , 0 ),
         OBJECT( 3, OF(OP,jupiter), &Class_3DPlanet, TL_Jupiter, 0 ),
         OBJECT( 3, OF(OP,saturne), &Class_3DPlanet, TL_Saturne, 0 ),
         OBJECT( 3, OF(OP,uranus ), &Class_3DPlanet, TL_Uranus , 0 ),
         OBJECT( 3, OF(OP,neptune), &Class_3DPlanet, TL_Neptune, 0 ),
         OBJECT( 3, OF(OP,pluton ), &Class_3DPlanet, TL_Pluton , 0 ),

    OBJECTEND
    };

/*--------------------------------------------*/

void main( int argc, char **argv )
{
  OOVars *vars;

    if (! (vars = OO_Init( OOA_MemDebugFlags, 0xffff, TAG_END ))) goto END_ERROR;

    if (! OO_NewTreeA( vars, 0, &P, Tree1 )) goto END_ERROR;
    if (! OO_DoMethod( P.gui, OOM_PRESENT, 0, 0, P.gui )) goto END_ERROR;
    if (! OO_DoMethod( P.h3d, D3M_START3D, 0 )) goto END_ERROR;

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethod( P.appl, OOM_WAITEVENTS, 0, OOF_BREAK_C, 0, 1 );

  END_ERROR:
    OO_DisposeObject( P.appl );
    OO_Cleanup( vars );
}
