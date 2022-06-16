#ifndef OO_GFX_H
#define OO_GFX_H 1

#include "oo_gui.h"

typedef struct _OORect		OORect;
typedef struct _OOPoint		OOPoint;
typedef struct _OOImage		OOImage;
typedef struct _OOTexture	OOTexture;
typedef struct _OOPolygon	OOPolygon;
typedef struct _OOTPInfo	OOTPInfo;


//-----------------------------------------------------

struct _OORect { // X et Y sont supérieurs de 1 à la dimension
	SLONG	x, y, X, Y;
	};

struct _OOPoint {
	SLONG	x, y;
	};

//=================================================================
#ifdef AMIGA

typedef struct RastPort		OORastPort;
typedef struct _OOBitMap	OOBitMap;
typedef struct _OOPalette	OOPalette;
typedef struct TextFont		OOFont;

#define OOA_Image           (0x010A|TAG_OO|TAG_APTR)
#define OOA_PicObj          (0x0111|TAG_OO|TAG_APTR)
#define OOA_PicName         (0x0112|TAG_OO|TAG_STRING)

//------ Class Picture 

#define OOM_PIC_LOAD        0x804200FD
#define OOM_PIC_UNLOAD      0x804200FE
#define OOM_PIC_PUTCOLORS   0x804200FF

#define OOA_PIC_Width       (0x0100|TAG_OO|TAG_NUMBER)
#define OOA_PIC_Height      (0x0101|TAG_OO|TAG_NUMBER)
#define OOA_PIC_NumColors   (0x0102|TAG_OO|TAG_NUMBER)
#define OOA_PIC_Depth       (0x0103|TAG_OO|TAG_NUMBER)
#define OOA_PIC_XAspect     (0x0104|TAG_OO|TAG_NUMBER)
#define OOA_PIC_YAspect     (0x0105|TAG_OO|TAG_NUMBER)
#define OOA_PIC_BMap        (0x0106|TAG_OO|TAG_APTR)
#define OOA_PIC_Error       (0x0107|TAG_OO|TAG_NUMBER)
#define OOA_PIC_X           (0x0108|TAG_OO|TAG_NUMBER|TAG_SIGNED)
#define OOA_PIC_Y           (0x0109|TAG_OO|TAG_NUMBER|TAG_SIGNED)
#define OOA_PIC_Colors      (0x010B|TAG_OO|TAG_APTR)
#define OOA_PIC_Handle      (0x010C|TAG_OO|TAG_APTR)

typedef struct _OOMsg_PicLoad       OOMsg_PicLoad;
typedef struct _OOMsg_PicUnload     OOMsg_PicUnload;
typedef struct _OOMsg_PicPutCols    OOMsg_PicPutCols;
typedef struct _OOMsg_PicGet        OOMsg_PicGet;

struct _OOMsg_PicLoad       { ULONG MethodId; OOTagItem *AttrList; UBYTE *PicName; APTR Func; APTR BMap; ULONG BMFlags; };
struct _OOMsg_PicUnload     { ULONG MethodId; OOTagItem *AttrList; APTR PicHandle; };
struct _OOMsg_PicPutCols    { ULONG MethodId; OOTagItem *AttrList; APTR PicHandle; OObject *Area; };

extern OOClass Class_Pic;

#endif

//=================================================================

#ifdef _WIN32

typedef struct _OORastPort	OORastPort;
typedef struct _OOBitMap	OOBitMap;
typedef struct _OOPalette	OOPalette;
typedef void				OOFont;

#define OOGFX_BYTESPERROW(bits)		(((bits) + 31) / 32 * 4)
#define OOGFX_DIB_PLANE(dib)		((void*) ((ULONG)dib + *(ULONG*)dib + OOGfx_DIB_PaletteSize(dib)))
#define OOGFX_DIB_WIDTH(dib)		((((BITMAPINFOHEADER*)dib)->biSize == sizeof(BITMAPINFOHEADER)) ? ((BITMAPINFOHEADER*)dib)->biWidth : (ULONG)((BITMAPCOREHEADER*)dib)->bcWidth)
#define OOGFX_DIB_HEIGHT(dib)		((((BITMAPINFOHEADER*)dib)->biSize == sizeof(BITMAPINFOHEADER)) ? ((BITMAPINFOHEADER*)dib)->biHeight : (ULONG)((BITMAPCOREHEADER*)dib)->bcHeight)
#define OOGFX_DIB_BITSPERPIXEL(dib)	((((BITMAPINFOHEADER*)dib)->biSize == sizeof(BITMAPINFOHEADER)) ? ((BITMAPINFOHEADER*)dib)->biBitCount : (ULONG)((BITMAPCOREHEADER*)dib)->bcBitCount)
#define OOGFX_DIB_ISWIN30(bi)		((*(ULONG*)(bi)) == sizeof(BITMAPINFOHEADER))

#define OOGFX_RECTWIDTH(r)		((r)->right - (r)->left)
#define OOGFX_RECTHEIGHT(r)		((r)->bottom - (r)->top)

#endif

//=================================================================

//------ Box types

#define OOV_BOX_NULL		0
#define OOV_BOX_BUTTONUP	1
#define OOV_BOX_BUTTONDOWN	2
#define OOV_BOX_HEAVYUP		3
#define OOV_BOX_HEAVYDOWN	4
#define OOV_BOX_LIGHTUP		5
#define OOV_BOX_LIGHTDOWN	6
#define OOV_BOX_BORDERUP	7
#define OOV_BOX_BORDERDOWN	8
#define OOV_BOX_GROOVEUP	9
#define OOV_BOX_GROOVEDOWN	10

//------ RastPorts

#define OOV_RP_DRAWNBY_SYS		0
#define OOV_RP_DRAWNBY_HAND		1
#define OOV_RP_DRAWNBY_DIRECTX	2
#define OOV_RP_DRAWNBY_OPENGL	3

//------ Bitmaps

#define OOBMAP_CONTIG       0x0001
#define OOBMAP_MASKED       0x0002
#define OOBMAP_PUBLIC       0x0004
#define OOBMAP_NOPLANE      0x0008
#define OOBMAP_FORCE_OOBMAP 0x0010 // Force l'allocation par OO_AllocBMap() même sous V39 ou plus 

//------ Palettes

#define OOV_COLOR_PREDEFBACK		0
#define OOV_COLOR_PREDEFDARK		1
#define OOV_COLOR_PREDEFMEDDARK		2 
#define OOV_COLOR_PREDEFMEDLIGHT	3 
#define OOV_COLOR_PREDEFLIGHT		4 
#define OOV_COLOR_PREDEFBLACK		5 
#define OOV_COLOR_PREDEFWHITE		6 
#define OOV_COLOR_PREDEFRED			7 
#define OOV_COLOR_PREDEFGREEN		8 
#define OOV_COLOR_PREDEFBLUE		9 
#define OOV_COLOR_PREDEFCYAN		10
#define OOV_COLOR_PREDEFMAGENTA		11
#define OOV_COLOR_PREDEFYELLOW		12
#define OOV_COLOR_BACK		0x10000+OOV_COLOR_PREDEFBACK
#define OOV_COLOR_DARK		0x10000+OOV_COLOR_PREDEFDARK
#define OOV_COLOR_MEDDARK	0x10000+OOV_COLOR_PREDEFMEDDARK
#define OOV_COLOR_MEDLIGHT	0x10000+OOV_COLOR_PREDEFMEDLIGHT
#define OOV_COLOR_LIGHT		0x10000+OOV_COLOR_PREDEFLIGHT
#define OOV_COLOR_BLACK		0x10000+OOV_COLOR_PREDEFBLACK	
#define OOV_COLOR_WHITE		0x10000+OOV_COLOR_PREDEFWHITE	
#define OOV_COLOR_RED		0x10000+OOV_COLOR_PREDEFRED		
#define OOV_COLOR_GREEN		0x10000+OOV_COLOR_PREDEFGREEN	
#define OOV_COLOR_BLUE		0x10000+OOV_COLOR_PREDEFBLUE	
#define OOV_COLOR_CYAN		0x10000+OOV_COLOR_PREDEFCYAN	
#define OOV_COLOR_MAGENTA	0x10000+OOV_COLOR_PREDEFMAGENTA	
#define OOV_COLOR_YELLOW	0x10000+OOV_COLOR_PREDEFYELLOW
#define OOV_COLOR_COUNT	13

#define OOCOLORINDEX(p,c) ((p)->PreDefColors[(c)-0x10000])

//------ Images

struct _OOImage { // Si SubBox != NULL, on n'affiche pas la BitMap entière 
    OOImage		*Next;		// Prochaine composante de cette imagerie 
    OOBitMap	*BitMap;	// BitMap contenant les pixels à afficher 
    OOBox		*SubBox;	// Si != NULL, table de OOBox : active = SubBox[SubBoxIndex]
    UWORD		SubBoxIndex;// Numéro de la SubBox à prendre en compte (permet animation) 
    UWORD		SubBoxCount;// Nombre total de subboxes
    SWORD		DestXOffs;	// Valeur à ajouter au x de destination 
    SWORD		DestYOffs;	// Valeur à ajouter au y de destination 
    };
/*
//------ Textures

struct _OOTexture {
	UWORD		XOffs, YOffs;			// En pixels, offset dans la grande bitmap commune du 3DSolid
	UWORD		Width, Height;			// En pixels, dimension dans la grande bitmap commune du 3DSolid
	FLOAT		RealWidh, RealHeight;	// En mètres, dimension dans le polygone
	UWORD		Flags;					// 
	UWORD		padw;
	OOBitMap	*BitMap;				// Bitmap contenant l'image
	};										  

// Flags
#define OOF_TEXTURE_STRETCH		0x0001	// Etire ou réduit la texture sans tenir compte de RealTruc
#define OOF_TEXTURE_TRANSPARENT	0x0002	// La couleur 0 n'est pas plaquée 
*/
//------ Polygones

struct _OOPolygon {
	UWORD		NumPoints;
	UWORD		Flags;
	UWORD		LineColor;
	UWORD		FillColor;
	OOTexture	*TextureInfo;
	OOPoint		*Points;
	void		*Plan;
	};

// Flags en entrée à indiquer dans OOPolygon pour la fonction
#define OOF_HIDE			0x0001 // Ne pas tracer si face retournée (D3F_BACKORIENTED)
#define OOF_FILL			0x0002 // Il faut remplir le polygone avec une couleur
#define OOF_ELLIPSE			0x0004 // Inutilisé pour l'instant (pris par l'objet Caméra)
// Flags de travail mis à jour par des routines de calcul
#define OOF_CLIPPED			0x0020
#define OOF_BACKORIENTED	0x0040
#define OOF_TEST2DPOINT		0x0080

//------------------------------------------------------------------

ULONG		OOGfx_Init			( void );
void		OOGfx_Cleanup		( void );
void		OOGfx_FreePalette	( OOPalette *oopalette );
OOPalette * OOGfx_AllocPalette	( void *rgbtable, ULONG numcolors );
void		OOGfx_SetDrawColor	( OORastPort *rp, ULONG colornum );
void		OOGfx_Move			( OORastPort *rp, SLONG x, SLONG y );
void		OOGfx_DrawPixel		( OORastPort *rp, SLONG x, SLONG y );
void		OOGfx_DrawLine		( OORastPort *rp, SLONG x, SLONG y );
void		OOGfx_DrawLine2		( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
void		OOGfx_DrawBox		( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2, ULONG type );
OOBorder *	OOGfx_GetBoxBorder	( ULONG type );
void		OOGfx_DrawRect		( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
void		OOGfx_DrawPolygon	( OORastPort *rp, OOPolygon *pg, UBYTE *lineinfo );

OOBitMap   *OOGfx_LoadPicture	( UBYTE *filename );

void		OOGfx_InitRPort		( OORastPort *rport );
void		OOGfx_FreeRPort		( OORastPort *rport );
OORastPort *OOGfx_AllocRPort	( void );
void		OOGfx_FreeBMap		( OOBitMap *bm );
OOBitMap   *OOGfx_AllocBMap		( OOPalette *pal, UBYTE depth, UWORD width, UWORD height, ULONG uflags );
void		OOGfx_FreeBMapRPort	( OORastPort *rport );
OORastPort *OOGfx_AllocBMapRPort( OOPalette *pal, UBYTE depth, UWORD width, UWORD height, ULONG uflags );

void *		OOGfx_DIB_Alloc			( ULONG width, ULONG height, UWORD bitsperpixel, BOOL allocplane );
ULONG		OOGfx_DIB_PaletteSize	( void *dib );
ULONG		OOGfx_DIB_NumTableColors( void *dib );
HBITMAP		OOGfx_DIB_ToBitmap		( void *dib, BOOL screen_compatible );
HPALETTE	OOGfx_GetDIBPalette		( void *dib );
HPALETTE	OOGfx_GetSystemPalette	( void );
ULONG		OOGfx_GetPaletteEntriesCount( HDC dc );

void		OOGfx_GetSizeOfString( OOFont *font, UBYTE *str, ULONG lenstr, OOSize *size );
OOFont *	OOGfx_OpenFontV		( OOFont *model, ULONG attr, ... );
OOFont *	OOGfx_OpenFont		( OOFont *model, OOTagList *taglist );
void		OOGfx_CloseFont		( OOFont *font );
BOOL		OOGfx_FontToTagList	( OOFont *font, OOTagList *taglist );
BOOL		OOGfx_FontToInfo	( OOFont *font, void *fontinfo );
void		OOGfx_TagListToInfo	( OOTagList *taglist, void *fontinfo );

#ifdef _WIN32
extern OOPalette OOPalette256;
#endif

#endif // OO_GFX_H 
