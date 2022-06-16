#ifndef OO_3DFLT_PRV_H
#define OO_3DFLT_PRV_H 1

#include "oo_gfx_prv.h"
#include "oo_list.h"
#include "oo_3d.h"

//==================================================================
#ifdef AMIGA
#include <graphics/rastport.h>
#include <exec/interrupts.h>
#include <hardware/intbits.h>
#endif
//==================================================================
#ifdef _WIN32
#endif
//==================================================================

/*
struct _OOPlan {
    UWORD       NumPoints;  // Nombre de points 3D
    UWORD       padw;
    OO3DPoint   Min,Max;
    OO3DPoint   N;          // Normale au plan
    FLOAT       D;          // Paramètre d du plan
    OO3DPoint   P1, P2, P3;
    OODispTrans *Trans;
    OO3DPoint   Points[1];  // Points 3D
    };

struct _OOFace {
    OObject     *Obj;
    OOPoint     Lonely2DPoint;
    OOPolygon   _P;
    OOPlan      _Plan;
    };

typedef struct _OOLoadTypeInfo {
    OONode      Node;
    OObject     *Solid;
    } OOLoadTypeInfo;
*/

//======================== 3DTypes ========================================

typedef struct _OONameNode	{
	OONode		Node;
	UBYTE		*Name;
	ULONG		UseCount;
	}
	OONameNode;

typedef struct _OOImageNode	{
	OONameNode	Node;
	OOBitMap	*BitMap;
	}
	OOImageNode;

typedef struct _OO3DTexture {
	OONameNode	Node;
	OOBitMap	*BitMap;
	UWORD		XOffs, YOffs;			// En pixels, offset dans la grande bitmap commune du 3DSolid
	UWORD		Width, Height;			// En pixels, dimension dans la grande bitmap commune du 3DSolid
	FLOAT		RealWidh, RealHeight;	// En mètres, dimension dans le polygone
	UWORD		Flags;					// OOF_TEXTURE_STRETCH, OOF_TEXTURE_TRANSPARENT
	}
	OO3DTexture;

typedef struct _OO3DMaterial {
	OONameNode	Node;
	ULONG		FillColor;
	OO3DTexture	*DisplayTexture;
	OO3DTexture	*TranspTexture;
	OO3DTexture	*BumpTexture;
	UWORD		Reflection;
	UWORD		Flags;
	}
	OO3DMaterial;

#define MAXVERTICES 4

typedef struct _OO3DPolygon {
	OO3DMaterial	*Material;		// couleurs, textures
	UWORD			Flags;			// OOF_HIDE, OOF_FILL, OOF_ELLIPSE
	UWORD			NumVertices;	// nombre de sommets
	UWORD			VerticesIndex[MAXVERTICES];	// tableau des numéros de sommets
	}
	OO3DPolygon;

typedef struct _OO3DType {
	OONameNode	Node;
	UWORD		NumVertices;
	UWORD		NumPolygons;
	UWORD		DimVertPols; // Dimension de chaque ligne de la table : nbr, pol1, pol2, ...
	//UWORD		NumEdges;
	UWORD		Pad;
    OO3DPoint	Size;	// Largeur, hauteur, profondeur en mètres
    OO3DPoint	Center; // Centre du rayon par rapport au centre des sommets
    FLOAT		R;		// Dimensions en mètres du rayon englobant pour aproximation en sphère
	OO3DPoint	*Vertices;	// Points-sommets de l'objet
	OO3DPolygon	*Polygons;	// Faces
	UWORD		*VertPols;	// Correspondance sommets -> polygones
	// UWORD		*Edges;		// Lignes : index polygone, index 1er point, index 2ème point
	}
	OO3DType;

typedef struct _OO3DPlan {
	OO3DPolygon	*Polygon;
    OO3DPoint   Min,Max;
    OO3DPoint   N;          // Normale au plan
    FLOAT       D;          // Paramètre d du plan
	UWORD		NumSpacePoints;
	UWORD		NumScreenPoints;
	OO3DPoint	*SpacePoints[MAXVERTICES*2];
	OO3DPoint	*ScreenPoints[MAXVERTICES*2];
	}
	OO3DPlan;

 typedef struct _OOGLSupport {
	HWND	Wnd;
	HDC		DC;
	HGLRC	RC;
	} 
	OOGLSupport;

//======================== Handle3D ========================================

struct _OOInst_Handle3D {
    ULONG           Elapsed;
    ULONG           MaxElapsed;
    ULONG           MaxPermit;
    ULONG           AddElapsed;
    ULONG           NumLoops;
    ULONG           Flags;
    ULONG           TimeOffset;     // si != 0, ne tient pas compte de Elapsed
    OObject         **ScreenAddr;
    OORastPort      *RastPort;
#ifdef AMIGA
    OOBitMap        *BitMap;
    struct Interrupt    IVBL;
    //--- Drawing
    struct AreaInfo AreaInfo;
    struct TmpRas   TmpRas;
#define AREANUMVECS 500
    UWORD           AreaBuffer[(AREANUMVECS*5)/2];
    APTR            AreaMask;
    ULONG           AreaMaskSize;
    struct BitMap   *MaskBitMap;
#endif
#ifdef _WIN32
    //OOBitMap      *OldBitMap;
    ULONG           LastTime;
    //--- Drawing
#endif
    };

// Flags
#define D3F_H3D_VBLON       0x0001
#define D3F_H3D_DOUBLEBUF   0x0002
#define D3F_H3D_3DSTARTED   0x0004
#define D3F_H3D_SCRCLEAR    0x0008
#define D3F_H3D_WAITVBLANK  0x0010


//======================== 3D Space ========================================

#define OOV_DISP_BLOCKSIZE      30
#define OOV_HEAR_BLOCKSIZE      4
#define OOV_DEAD_BLOCKSIZE      4
#define OOV_ACTORS_BLOCKSIZE    20
#define OOV_SCREENS_BLOCKSIZE   4
#define OOV_SPEAKERS_BLOCKSIZE  2

struct _OOInst_3DSpace {
    OObjArray       DispList;       // Liste des objets à afficher
    OObjArray       HearList;       // Liste des objets à entendre
    OObjArray       DeadList;       // Liste des objets à deleter en fin de tour
    OObjArray       ActorsList;     // Liste des objets acteurs
    OObjArray       ScreensList;    // Liste des écrans
    OObjArray       SpeakersList;   // Liste des haut-parleurs
    OOList          LoadList;
    };


//======================== 3D Solid ========================================

struct D3Rot {
    //---CENTER_ABS/CENTER_OBJ---
    FLOAT   Cx,Cy,Cz;   // ABS : centre de la rotation en mètres
    FLOAT   Ix,Iy,Iz;   // Incrément (vitesse) autour de chaque axe en radians
    FLOAT   Ax,Ay,Az;   // Angle sur chaque axe en radians
    //---CENTER_SELF---
    FLOAT   SIx,SIy,SIz; // Incrément (vitesse) autour de chaque axe en radians
    FLOAT   SAx,SAy,SAz; // Angle sur chaque axe (calculé) en radians
    FLOAT   OldSAx,OldSAy,OldSAz;   // Pour détecter le recalcul des sommets
    };

struct D3Speed {
    FLOAT   Vx,Vy,Vz;   // Vitesse objet sur chaque axe (modifiée par ActionFunc) en mètres/tick
    FLOAT   Ax,Ay,Az;   // Accélération sur chaque axe en mètres/tick/tick
    FLOAT   nx,ny,nz;   // Nouvelles coordonnées sur chaque axe en mètres
    };

struct _OOInst_3DSolid {
    FLOAT           X,Y,Z;      // Position du centre de l'objet dans Space en mètres
    OO3DType		*Type;		// Coordonnées et description géométrique/graphique de l'objet
	UBYTE			*Name;		// Nom de cette instance
    FLOAT           Profile;    // Pour profil aéro/hydro dynamisme (utilisé avec densité du milieu pour calcul de la trainée)
    FLOAT           Masse;      // Masse en kg

    struct D3Rot    Rot;
    struct D3Speed  Speed;
	OO3DPoint		*RotVertices;
    ULONG           Sensitivity;
    ULONG           Actions;
    ULONG           Flags;
    OObject         *AttachObj; // Objet auquel on est attaché pour translation ou rotation
    OO3DPoint       AttachVec;  // Si rotation : vecteur objet en rotation moins objet centre ou centre ABS
    OObject         *ColObject; // Objet entré en collision

    OOImage         *Image;     // Représentations de l'objet : plusieurs dans la même bitmap
    OObject         *Sound;     // Son : pris par les micros quand != NULL

    //--- Fonctions Callback
    void            (*CBFunc_Speed)( OOInst_3DSolid * );
    };

//------ Macros
#define D3SOL(o)    ((OOInst_3DSolid*)(((UBYTE*)o)+Class_3DSolid.InstOffset))

//------ Flags
#define D3F_SOLID_CENTER_ABS    0x00000001
#define D3F_SOLID_CENTER_OBJ    0x00000002
#define D3F_SOLID_CENTER_SELF   0x00000004
#define D3F_SOLID_RUBBERY       0x00000008
#define D3F_SOLID_COLLIDE       0x00000010
#define D3F_SOLID_VISIBLE       0x00000020
#define D3F_SOLID_AUDIBLE       0x00000040
#define D3F_SOLID_SPHERE        0x00000080

#define D3F_COL_LEFT            0x00010000
#define D3F_COL_RIGHT           0x00020000
#define D3F_COL_FORW            0x00040000
#define D3F_COL_BACK            0x00080000
#define D3F_COL_TOP             0x00100000
#define D3F_COL_BOT             0x00200000
#define D3F_UNDER_COLLISION     (D3F_COL_LEFT|D3F_COL_RIGHT|D3F_COL_FORW|D3F_COL_BACK|D3F_COL_TOP|D3F_COL_BOT)
#define D3F_BOUM                0x00400000
#define D3F_SOLID_ISMOVING      0x00800000
#define D3F_SOLID_ISVISIBLE     0x01000000
#define D3F_SOLID_ISAUDIBLE     0x02000000
#define D3F_LOADALLOC           0x04000000
#define D3F_DO_ABSROTATION      0x08000000
#define D3F_DO_ABSATTACH        0x10000000
#define D3F_DO_SELFROTATE		0x20000000
#define D3F_SOLID_TYPECHANGED	0x40000000


//======================== 3D Screen =======================================

struct _OOInst_3DScreen {
    OORastPort  *MemRP;
    // Bufferisation pour rapidité
    OORastPort  *DestRP;
    SLONG       DestOffsX, DestOffsY;
    ULONG       DestSizeX, DestSizeY;
    };


//======================== 3D Camera =======================================

/* Le centre contenu dans la partie 3DSolid de l'instance est la
   position de l'observateur.
 * Le hublot est à la distance ObserverDist du centre de
   l'objet.
 * La rotation contenue dans la partie 3DSolid de l'instance est
   la rotation de l'axe d'observation passant par la position de
   l'observateur et le milieu du hublot.
 * TgPsi et TgPhi sont les angles d'observation entre les bords
   du hublot et l'axe d'observation. Les coordonnées des centres
   des objets doivent être ramenées au repère 3D de l'observateur
   avant l'utilisation de TgPsi et TgPhi.
 */

typedef struct _OODispTrans {
    SLONG   AreaW2, AreaH2; // Laisser en signed pour éviter erreurs
    FLOAT   DispCoefX, DispCoefY;
    }
	OODispTrans;

struct _OOInst_3DCamera {
    // Tables des objets visibles
    OOList	VisList1;
    OOList	VisList2;
    OOList	VisWaitList;
	OOList	*VisCurList;
	OOList	*VisOldList;
    // Caractéristiques du hublot de visualisation
    FLOAT   WindowWidth2, WindowHeight2; // Demi-tailles
    FLOAT   ObserverDist;
    FLOAT   TgPsi, TgPhi;
    OODispTrans Trans;
    };


//======================== 3D Speaker ======================================

struct _OOInst_3DSpeaker {
    OObject     *HandleSound;
    };


//======================== 3D Mike =========================================

#define OOV_SOUND_BLOCKSIZE     4

struct _OOInst_3DMike {
    // Table des objets audibles
    OObjArray   SoundList;
    };


//======================== 3D Planets ======================================

/*
struct _OOInst_3DPlanet {
    ULONG tag_Id;   ULONG Id;
    ULONG tag_Name; UBYTE*Name;
    ULONG tag_e;    FLOAT e;
    ULONG tag_i;    FLOAT i;
    ULONG tag_a;    FLOAT a;
    ULONG tag_L;    FLOAT L;
    ULONG tag_P;    FLOAT P;
    ULONG tag_O;    FLOAT O;
    ULONG tag_v;    FLOAT v;
    ULONG tag_b;    FLOAT b;
    ULONG tag_l;    FLOAT l;
    ULONG tag_r;    FLOAT r;
    ULONG tag_end;  FLOAT end;
    };
*/

//======================== Private funcs ===================================

// void    OO3DMath_ProjectTexture( OOPlan *plan, OOTexture *texture );

void		*OO3DType_SearchType( UBYTE *name );
OO3DType	*OO3DType_AddType	( OO3DefType *def );
void		OO3DType_RemoveType	( OO3DType *node, BOOL force );


#endif
