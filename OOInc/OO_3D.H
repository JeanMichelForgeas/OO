#ifndef OO_3DFLT_H
#define OO_3DFLT_H 1

//==================================================================
#ifdef AMIGA
#include <libraries/mathieeesp.h>
#include <clib/mathieeesingbas_protos.h>
#include <clib/mathieeesingtrans_protos.h>
#include <pragmas/mathieeesingbas_pragmas.h>
#include <pragmas/mathieeesingtrans_pragmas.h>

extern struct Library *MathIeeeSingBasBase;
extern struct Library *MathIeeeSingTransBase;

#define F2FLT		IEEESPFlt
#define F2LONG		IEEESPFix
#define FMUL		IEEESPMul
#define FDIV		IEEESPDiv
#define FADD		IEEESPAdd
#define FSUB		IEEESPSub
#define FNEG		IEEESPNeg
#define FABS		IEEESPAbs
#define FMOD		IEEESPMod
#define FFLOOR		IEEESPFloor
#define FCEIL		IEEESPCeil
#define FCMP		IEEESPCmp
#define FTST		IEEESPTst
#define FZERO(a)	(IEEESPTst(a)==0)

#define FTAN		IEEESPTan
#define FATAN		IEEESPAtan
#define FCOS		IEEESPCos
#define FACOS		IEEESPAcos
#define FSIN		IEEESPSin
#define FASIN		IEEESPAsin
#define FEXP		IEEESPExp
#define FPOW(a,b)	IEEESPPow((b),(a))
#define FSQRT		IEEESPSqrt

#define FMAX(a,b)	(FCMP((a),(b))>0?(a):(b))
#define FMIN(a,b)	(FCMP((a),(b))<0?(a):(b))

#define OO3DMath_DotProduct(a,b) (FADD(FADD(FMUL((a)->X, (b)->X), FMUL((a)->Y, (b)->Y)), FMUL((a)->Z, (b)->Z)))
#define OO3DMath_CrossProduct(b,c,a) { (a)->X = FSUB(FMUL((b)->Y, (c)->Z), FMUL((b)->Z, (c)->Y)); (a)->Y = FSUB(FMUL((b)->Z, (c)->X), FMUL((b)->X, (c)->Z)); (a)->Z = FSUB(FMUL((b)->X, (c)->Y), FMUL((b)->Y, (c)->X)); }
#define OO3DMath_VecMul(a,b) { (a)->X = FMUL((a)->X,(b)); (a)->Y = FMUL((a)->Y,(b)); (a)->Z = FMUL((a)->Z,(b)); }
#define OO3DMath_VecDiv(a,b) { (a)->X = FDIV((a)->X,(b)); (a)->Y = FDIV((a)->Y,(b)); (a)->Z = FDIV((a)->Z,(b)); }
#define OO3DMath_VecAdd(a,b) { (a)->X = FADD((a)->X,(b)); (a)->Y = FADD((a)->Y,(b)); (a)->Z = FADD((a)->Z,(b)); }
#define OO3DMath_VecSub(a,b) { (a)->X = FSUB((a)->X,(b)); (a)->Y = FSUB((a)->Y,(b)); (a)->Z = FSUB((a)->Z,(b)); }
#endif
//==================================================================
#ifdef _WIN32
#include <math.h>
#include <float.h>

#define PI	3.141592653589793F

#define F2FLT(a)	((FLOAT)(a))
#define F2LONG(a)	((SLONG)(a))
#define FMUL(a,b)	((a)*(b))
#define FDIV(a,b)	((a)/(b))
#define FADD(a,b)	((a)+(b))
#define FSUB(a,b)	((a)-(b))
#define FNEG(a) 	((FLOAT)-(a))  // ((FLOAT)_chgsign(a))
#define FABS(a) 	((FLOAT)fabs(a))
#define FMOD(a,b)	((FLOAT)fmod(a,b))
#define FFLOOR(a)	((FLOAT)floor(a))
#define FCEIL(a)	((FLOAT)ceil(a))
#define FCMP(a,b)	(((a)==(b))?0L:(((a)>(b))?1L:-1L))
#define FTST(a) 	(((a)==(0.0F))?0L:(((a)>(0.0F))?1L:-1L))
#define FZERO(a)	((a)==(0.0F))

#define FTAN(a) 	((FLOAT)tan(a))
#define FATAN(a)	((FLOAT)atan(a))
#define FCOS(a) 	((FLOAT)cos(a))
#define FACOS(a)	((FLOAT)acos(a))
#define FSIN(a) 	((FLOAT)sin(a))
#define FASIN(a)	((FLOAT)asin(a))
#define FEXP(a) 	((FLOAT)exp(a))
#define FPOW(a,b)	((FLOAT)pow((a),(b)))
#define FSQRT(a)	((FLOAT)OOMath_Sqrt(a))

#define FMAX(a,b)	(__max((a),(b)))
#define FMIN(a,b)	(__min((a),(b)))

#define OO3DMath_DotProduct(a,b) ((a)->X * (b)->X + (a)->Y * (b)->Y + (a)->Z * (b)->Z)
#define OO3DMath_CrossProduct(b,c,a) { (a)->X = (b)->Y * (c)->Z - (b)->Z * (c)->Y; (a)->Y = (b)->Z * (c)->X - (b)->X * (c)->Z; (a)->Z = (b)->X * (c)->Y - (b)->Y * (c)->X; }
#define OO3DMath_VecMul(a,b) { (a)->X *= (b); (a)->Y *= (b); (a)->Z *= (b); }
#define OO3DMath_VecDiv(a,b) { (a)->X /= (b); (a)->Y /= (b); (a)->Z /= (b); }
#define OO3DMath_VecAdd(a,b) { (a)->X += (b); (a)->Y += (b); (a)->Z += (b); }
#define OO3DMath_VecSub(a,b) { (a)->X -= (b); (a)->Y -= (b); (a)->Z -= (b); }
#endif
//==================================================================

#define FKP(a)		F2LONG(FMUL(a,(FLOAT)10000F))
#define MAXFLT		((FLOAT)+1e+37F)
#define MINFLT		((FLOAT)-1e+37F)

/*
		Root
		 \--Appl
		 \--SysEvent
		 :	 \--HandleGUI
		 :	 \--Handle3D
		 \--3DSpace
		 \--3DSolid
			 \--3DLight
			 \--3DScreen
			 \--3DCamera
			 \--3DHuman
			 \--3DPlanet

						 Appl
						/	 \
				Handle3D
				/		\
			 Space	   Space
			/	  \ 		\
	   Solid	   Solid	Solid
*/

typedef struct _OOInst_Handle3D		OOInst_Handle3D;
typedef struct _OOInst_3DSpace		OOInst_3DSpace;
typedef struct _OOInst_3DSolid		OOInst_3DSolid;
typedef struct _OOInst_3DScreen		OOInst_3DScreen;
typedef struct _OOInst_3DCamera		OOInst_3DCamera;
typedef struct _OOInst_3DSpeaker	OOInst_3DSpeaker;
typedef struct _OOInst_3DMike		OOInst_3DMike;
typedef struct _OOInst_3DScreen		OOInst_3DScreen;
typedef struct _OOInst_3DSoleil		OOInst_3DSoleil;
/*
typedef struct _OOInst_3DHuman		OOInst_3DHuman;
typedef struct _OOInst_3DPlanet		OOInst_3DPlanet;
*/

//------------- General use attributes (0x3000-0x31FF) --------------

#define D3A_Handle3D		(0x3000|TAGT_OO | TAGT_OBJ |TAGT_PTR)
#define D3A_ObjSpace		(0x3001|TAGT_OO | TAGT_OBJ |TAGT_PTR)
#define D3A_Actions 		(0x3002|TAGT_OO | TAGT_LONG|TAGT_NUM) // Voir Sensitivity
#define D3A_ObjCamera		(0x3003|TAGT_OO | TAGT_OBJ |TAGT_PTR)
#define D3A_ObjTable		(0x3004|TAGT_OO | TAGT_OBJ |TAGT_STR)
#define D3A_ObjCount		(0x3005|TAGT_OO | TAGT_LONG|TAGT_NUM)
#define D3A_ElapsedTime 	(0x3006|TAGT_OO | TAGT_LONG|TAGT_NUM)
#define D3A_RastPort		(0x3007|TAGT_OO | TAGT_BYTE|TAGT_PTR)
#define D3A_ObjScreen		(0x3008|TAGT_OO | TAGT_OBJ |TAGT_PTR)
#define D3A_ObjSpeaker		(0x3009|TAGT_OO | TAGT_OBJ |TAGT_PTR)
#define D3A_ConnectObj		(0x300B|TAGT_OO | TAGT_LONG|TAGT_PTR) // Objet connecté : camera, micro...
#define D3A_FillColor		(0x300C|TAGT_OO | TAGT_LONG|TAGT_NUM)
#define D3A_SceneFileName	(0x300D|TAGT_OO | TAGT_BYTE|TAGT_STR)


//---------------------------------------------------------------

typedef struct _OO3DPoint {
	FLOAT X;
	FLOAT Y;
	FLOAT Z;
	}
	OO3DPoint;

typedef struct _OO3DefTexture {
	UBYTE	*Name;
	UBYTE	*FileName;
	UWORD	XOffs, YOffs;			// En pixels, offset dans la grande bitmap commune du 3DSolid
	UWORD	Width, Height;			// En pixels, dimension dans la grande bitmap commune du 3DSolid
	FLOAT	RealWidh, RealHeight;	// En mètres, dimension dans le polygone
	UWORD	Flags;					// OOF_TEXTURE_STRETCH, OOF_TEXTURE_TRANSPARENT
	}
	OO3DefTexture;
// OOTextureInfo Flags
#define OOF_TEXTURE_STRETCH		0x0001	// Etire ou réduit la texture sans tenir compte de RealTruc
#define OOF_TEXTURE_TRANSPARENT	0x0002	// La couleur 0 n'est pas plaquée 

typedef struct _OO3DefMaterial {
	UBYTE			*Name;
	ULONG			FillColor;
	OO3DefTexture	*DisplayTexture;
	OO3DefTexture	*TranspTexture;
	OO3DefTexture	*BumpTexture;
	UWORD			Reflection;
	UWORD			Flags;
	}
	OO3DefMaterial;

typedef struct _OO3DefPolygon {
	OO3DefMaterial	*Material;		// couleurs, textures
	UWORD			Flags;			// OOF_HIDE, OOF_FILL, OOF_ELLIPSE
	UWORD			NumVertices;	// nombre de sommets
	UWORD			VerticesIndex[4];	// tableau des numéros de sommets
	}
	OO3DefPolygon;

typedef struct _OO3DefType {
	UBYTE			*Name;
	UWORD			NumVertices;
	UWORD			NumPolygons;
	OO3DPoint		*Vertices;		// Points-sommets de l'objet
	OO3DefPolygon	*Polygons;		// Table des polygones
	}
	OO3DefType;

// Flags en entrée à indiquer dans les OO3DDefPolygon, et copiés dans OOPolygon de OOFace
// OOF_HIDE 	 Ne pas tracer si face retournée (D3F_BACKORIENTED)
// OOF_FILL 	 Il faut remplir le polygone avec une couleur
// OOF_ELLIPSE	 C'est une ellipse ça c'est sûr

// Flags de travail mis à jour par les routines de calcul dans OOPolygon de OOFace
// OOM_CLIPMASK
// OOF_CLIPLEFT
// OOF_CLIPRIGHT
// OOF_CLIPTOP
// OOF_CLIPBOT
// OOF_CLIPBACK
// OOF_CLIPPED		Mis par les routines de clipping
// OOF_BACKORIENTED Mis par la routine de visibilité


//==========================================================================

// For Speed these methods expect some tags in the order below :
//		D3M_START3D 		:	D3A_Handle3D	D3A_ObjSpace
//		D3M_MOVEALL 		:	D3A_Handle3D	D3A_ElapsedTime
//		D3M_OBJTOSCREEN 	:	D3A_RastPort	D3A_ObjTable	D3A_ObjCount
//		D3M_OBJTOCAMERA 	:	D3A_RastPort	D3A_ObjTable	D3A_ObjCount
//		D3M_OBJTOSPEAKER	:	D3A_RastPort	D3A_ObjTable	D3A_ObjCount
//		D3M_OBJTOMIKE		:	D3A_ObjSpeaker	D3A_ObjTable	D3A_ObjCount
//		D3M_MIXSOUND		:	D3A_ObjSpeaker
//		D3M_CLEARSCREEN 	:	D3A_RastPort
//		D3M_DRAWSCREEN		:	D3A_RastPort
//		D3M_SYNCSCREEN		:	D3A_RastPort
//		D3M_DRAW			:	D3A_RastPort	OOA_WidthHeight
//		D3M_ADDACTOR		:	D3A_Object
//		D3M_REMOVEACTOR 	:	D3A_Object
//		D3M_ADDSCREEN		:	D3A_ObjScreen
//		D3M_REMOVESCREEN	:	D3A_ObjScreen
//		D3M_DEADCHILD		:	OOA_Object
//		D3M_OBJ2ACTOR		:	OOA_Object
//		D3M_DOSPACECOL		:	D3A_ObjSpace

//======================== Handle3D ========================================

//------------------------ Methods (0x3200-0x321F) -------------------------

#define D3M_STEP3D			0x3200
#define D3M_START3D 		0x3201
#define D3M_STOP3D			0x3202

//------------------------ Attributes (0x3200-0x321F) ----------------------

// OOA_OnOff : Allumé / éteint
#define D3A_MaxTime 	(0x3200|TAGT_OO | TAGT_LONG|TAGT_NUM)
#define D3A_TimeOffset	(0x3201|TAGT_OO | TAGT_LONG|TAGT_NUM)
#define D3A_ScreenAddr	(0x3202|TAGT_OO | TAGT_BYTE|TAGT_PTR)
#define D3A_ClearScreen (0x3203|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define D3A_WaitVBlank	(0x3204|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)


//======================== 3D Space ========================================

//------------------------ Methods (0x3220-0x323F) -------------------------

//#define D3M_START3D		0x3201 Handle3D
//#define D3M_STOP3D		0x3202 Handle3D
#define D3M_MOVEALL 		0x3220
#define D3M_DEADCHILD		0x3221
#define D3M_OBJTOVIEW		0x3222
#define D3M_CLEARVIEW		0x3223
#define D3M_REFRESHVIEW 	0x3224
#define D3M_SYNCVIEW		0x3225
#define D3M_ADDACTOR		0x3226
#define D3M_REMOVEACTOR 	0x3227
#define D3M_ADDSCREEN		0x3228
#define D3M_REMOVESCREEN	0x3229
#define D3M_ADDSPEAKER		0x322A
#define D3M_REMOVESPEAKER	0x322B
#define D3M_LOADSCENE		0x322C

//------------------------ Attributes (0x3220-0x323F) ----------------------

#define D3A_TimeAccel	(0x3220|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)// Accélaration du temps dans cet espace
#define D3A_Gravity 	(0x3221|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)//
#define D3A_Density 	(0x3222|TAGT_OO | TAGT_FLOAT|TAGT_NUM)			  // Densité du milieu : air, eau, espace : 1 == eau
//#define D3A_SizeX 	(0x3244|TAGT_OO | TAGT_FLOAT|TAGT_NUM)	3DSolid   // Dimension en mètres
//#define D3A_SizeY 	(0x3245|TAGT_OO | TAGT_FLOAT|TAGT_NUM)	3DSolid   // Dimension en mètres
//#define D3A_SizeZ 	(0x3246|TAGT_OO | TAGT_FLOAT|TAGT_NUM)	3DSolid   // Dimension en mètres


//======================== 3D Solid ========================================

//------------------------ Methods (0x3240-0x329F) -------------------------

#define D3M_SETSPEED		0x3240
#define D3M_SETIMAGE		0x3241
#define D3M_SETSOUND		0x3242
#define D3M_DOSPACECOL		0x3243
#define D3M_DOOBJECTCOL 	0x3244
#define D3M_DOACTION		0x3245
#define D3M_ADD 			0x3246
#define D3M_SUB 			0x3247
#define D3M_OBJ2ACTOR		0x3248
#define D3M_BUILDVERTICES	0x3249

//------------------------ Attributes (0x3240-0x329F) ----------------------

// OOA_SoundFileName
// OOA_SoundVolume
#define D3A_R			(0x3240|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_X			(0x3241|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_Y			(0x3242|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_Z			(0x3243|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_SizeX		(0x3244|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_SizeY		(0x3245|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_SizeZ		(0x3246|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_SpeedVx 	(0x3247|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_SpeedVy 	(0x3248|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_SpeedVz 	(0x3249|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_SpeedAx 	(0x324A|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_SpeedAy 	(0x324B|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_SpeedAz 	(0x324C|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_AttachObj	(0x324D|TAGT_OO | TAGT_LONG |TAGT_PTR)
#define D3A_RotCx		(0x324E|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)	//		Centre
#define D3A_RotCy		(0x324F|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotCz		(0x3250|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotIx		(0x3251|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)	//		Vitesse
#define D3A_RotIy		(0x3252|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotIz		(0x3253|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotAx		(0x3254|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)	//		Angle
#define D3A_RotAy		(0x3255|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotAz		(0x3256|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotSIx		(0x3257|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)	// Rotation centrée sur lui-même
#define D3A_RotSIy		(0x3258|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)	//		Vitesse
#define D3A_RotSIz		(0x3259|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotSAx		(0x325A|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)	//		Angle
#define D3A_RotSAy		(0x325B|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_RotSAz		(0x325C|TAGT_OO | TAGT_FLOAT|TAGT_NUM|TAGT_SIGNED)
#define D3A_Masse		(0x325D|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Profile 	(0x325E|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Type 		(0x325F|TAGT_OO | TAGT_BYTE |TAGT_PTR)
#define D3A_TypeName	(0x3260|TAGT_OO | TAGT_BYTE |TAGT_PTR|TAGT_EDIT)
//#define D3A_	(0x3261|TAGT_OO | TAGT_BYTE |TAGT_PTR)
#define D3A_CBFuncSpeed (0x3262|TAGT_OO | TAGT_BYTE |TAGT_PTR)
#define D3A_CenterAbs	(0x3263|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Rotation absolue
#define D3A_CenterObj	(0x3264|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Rotation autour d'un objet
#define D3A_CenterSelf	(0x3265|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Rotation sur lui-même
#define D3A_Collide 	(0x3266|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Collision possible avec cet objet
#define D3A_Visible 	(0x3267|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // L'objet est visible pour les caméras
#define D3A_Audible 	(0x3268|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // L'objet est audible pour les micros
#define D3A_Sphere		(0x3269|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // C'est une sphère, ça c'est sûr
#define D3A_Rubbery 	(0x326A|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Collision caoutchouc (choc mou : pas de rebond)
#define D3A_Sensitivity 	(0x326B|TAGT_OO | TAGT_LONG|TAGT_NUM) // Mask des sensibilités
#define D3A_SensGravity 	(0x326C|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : aux champs de gravitation
#define D3A_SensMagnetism	(0x326D|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : aux champs magnétiques
#define D3A_SensElectricity (0x326E|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : aux champs électriques
#define D3A_SensLuminosity	(0x326F|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : à la luminosité
#define D3A_SensCharism 	(0x3260|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : au charisme
#define D3A_SensMagic		(0x3261|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : à la magie
#define D3A_Sens_7			(0x3272|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : à ... ?
#define D3A_Sens_8			(0x3273|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)  // Booléen de sensibilité : à ... ?

//------ Sensitivity
#define D3MASK_SENS_PREALLOCATED	(OOF_SENS_GRAVITY|OOF_SENS_MAGNETISM|OOF_SENS_ELECTRICITY|OOF_SENS_LUMINOSITY|OOF_SENS_CHARISM|OOF_SENS_MAGIC|OOF_SENS_7|OOF_SENS_8)
#define D3F_SENS_GRAVITY		0x0001
#define D3F_SENS_MAGNETISM		0x0002
#define D3F_SENS_ELECTRICITY	0x0004
#define D3F_SENS_LUMINOSITY 	0x0008
#define D3F_SENS_CHARISM		0x0010
#define D3F_SENS_MAGIC			0x0020
#define D3F_SENS_7				0x0040
#define D3F_SENS_8				0x0080

//======================== 3D Screen =======================================

//------------------------ Methods (0x32C0-0x32DF) -------------------------

#define D3M_CLEARSCREEN 	0x32C0
#define D3M_OBJTOSCREEN 	0x32C1
#define D3M_DRAWSCREEN		0x32C2
#define D3M_SYNCSCREEN		0x32C3


//------------------------ Attributes (0x32C0-0x32DF) ----------------------

// OOA_OnOff		 : Allumé / éteint
// D3A_ConnectObj : Caméra, magnétoscope, ...
// OOA_AreaAddr
// D3A_FillColor	 : Couleur de fond de l'écran


//======================== 3D Camera =======================================

//------------------------ Methods (0x32A0-0x32BF) -------------------------

#define D3M_OBJTOCAMERA 	0x32A0
#define D3M_DRAW			0x32A1


//------------------------ Attributes (0x32A0-0x32BF) ----------------------

// OOA_OnOff : Allumé / éteint
#define D3A_WindowWidth 	(0x32A0|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_WindowHeight	(0x32A1|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_ObserverDist	(0x32A2|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_DrawnBy			(0x32A3|TAGT_OO | TAGT_LONG |TAGT_NUM)
#define D3A_FullScreen		(0x32A4|TAGT_OO | TAGT_BOOL |TAGT_NUM)


//======================== 3D Speaker ======================================

//------------------------ Methods (0x32E0-0x32FF) -------------------------

#define D3M_OBJTOSPEAKER	0x32E0
#define D3M_PLAYSOUND		0x32E1


//------------------------ Attributes (0x32E0-0x32FF) ----------------------

// OOA_OnOff : Allumé / éteint
// D3A_ConnectObj : micro, mixer, ampli, ...


//======================== 3D Mike =========================================

//------------------------ Methods (0x3300-0x331F) -------------------------

#define D3M_OBJTOMIKE		0x30E0
#define D3M_MIXSOUND		0x30E1


//------------------------ Attributes (0x3300-0x331F) ----------------------

// OOA_OnOff : Allumé / éteint
#define D3A_Directional 	(0x3300|TAGT_OO | TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define D3A_Sensibility 	(0x3301|TAGT_OO | TAGT_FLOAT|TAGT_NUM)


//======================== 3D Planets =======================================

//---------------------------------------------------------------

/*
#define D3ID_MERCURE	0
#define D3ID_VENUS		1
#define D3ID_TERRE		2
#define D3ID_MARS		3
#define D3ID_JUPITER	4
#define D3ID_SATURNE	5
#define D3ID_URANUS 	6
#define D3ID_NEPTUNE	7
#define D3ID_PLUTON 	8

#define D3A_Planet_Id	(0x|TAGT_OO | )
#define D3A_Planet_Name (0x|TAGT_OO | )
#define D3A_Planet_e	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_i	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_a	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_L	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_P	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_O	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_v	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_b	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_l	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
#define D3A_Planet_r	(0x|TAGT_OO | TAGT_FLOAT|TAGT_NUM)
*/

//---------------------------------------------------------------

ULONG	OO_Init3D		( OOTagList * );
void	OO_Cleanup3D	( void );
void	OO3DMath_Rotate ( OO3DPoint *co, OO3DPoint *angle );
void	OO3DMath_RotateArray( OO3DPoint *co, OO3DPoint *angle, ULONG count );
void	OO3DMath_AddArray( OO3DPoint *co, OO3DPoint *add, ULONG count );
void	OO3DMath_SubArray( OO3DPoint *co, OO3DPoint *sub, ULONG count );
FLOAT	OO3DMath_PlanZ	( void *plan, FLOAT x, FLOAT y );
FLOAT	OO3DMath_Normale( OO3DPoint *p0, OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *n, BOOL normalize );
FLOAT	OO3DMath_Modulo ( OO3DPoint *p );
FLOAT	OO3DMath_Deg2Rad( FLOAT deg );
FLOAT	OO3DMath_Rad2Deg( FLOAT rad );

extern OOClass Class_Handle3D;
extern OOClass Class_3DSpace;
extern OOClass Class_3DSolid;
extern OOClass Class_3DScreen;
extern OOClass Class_3DCamera;
extern OOClass Class_3DSpeaker;
extern OOClass Class_3DMike;
extern OOClass Class_3DSoleil;
extern OOClass Class_3DLight;
//extern OOClass Class_3DPlanet;
//extern OOClass Class_3DHuman;

#endif
