#ifndef OO_GFX_PRV_H
#define OO_GFX_PRV_H 1

#define OOD_DRAW_BY_SYSTEM
//#define OOD_DRAW_BY_HAND

#include "oo_gfx.h"

//=================================================================
#ifdef AMIGA

#include <exec/memory.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>

#include <clib/graphics_protos.h>

#define OO2BM(ubm) ((struct BitMap *)(((ULONG)ubm)+8))
#define BM2OO(bm) ((OOBitMap *)(((ULONG)bm)-8))

struct _OOBitMap {
	OOPalette *Palette;
    void	*Mask;	
	UBYTE	UFlags;
    UBYTE	Type;    
	UWORD	Width;
    // BitMap Amiga standard
    UWORD	BytesPerRow;    
	UWORD	Height;
    UBYTE   Flags;
    UBYTE   Depth;
    UWORD   pad;	// Utilisé par Amiga OS
    void    *Planes[8];
	};

struct _OOPalette {
	ULONG Reserved;
	};

//------ Class Picture 

#endif // AMIGA

//=================================================================

#ifdef _WIN32

typedef struct {
	ULONG		(*DrawInit)( void );
	void		(*DrawCleanup)( void );
	void		(*FreePalette)( OOPalette *oopalette );
	OOPalette * (*AllocPalette)( void *rgbtable, ULONG numcolors );
	void		(*SetDrawColor)( OORastPort *rp, ULONG colornum );
	void		(*Move)( OORastPort *rp, SLONG x, SLONG y );
	void		(*DrawPixel)( OORastPort *rp, SLONG x, SLONG y );
	void		(*DrawLine)( OORastPort *rp, SLONG x, SLONG y );
	void		(*DrawLine2)( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
	void		(*DrawRect)( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
	void		(*DrawBox)( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2, ULONG type );
	void		(*DrawPolygon)( OORastPort *rp, OOPolygon *pg, UBYTE *info_tab );
} OOGfxFuncs;

struct _OORastPort {	// Doit rester petit pour déclaration sur la pile
	HDC			DC;
	OOBitMap	*BitMap;
	OOPalette	*Palette;	// Palette propre au rastport
	void		*Context;	// Context propre à un moteur graphique : OpenGL...
	HBITMAP		OldHBitMap;
	HPALETTE	OldHPalette;
	UBYTE		Flags:5;
	UBYTE		DrawnBy:3;
	UBYTE		DrawPen;
	UBYTE		PenSet;
	UBYTE		BrushSet;
	};
// Flags
#define OOF_RP_ALLOCATED	0x01
#define OOF_RP_DC_ALLOCATED	0x02

#define OO2BM(ubm) ((struct BitMap *)(((ULONG)ubm)+12))
#define BM2OO(bm) ((OOBitMap *)(((ULONG)bm)-12))

struct _OOBitMap {
	OOPalette	*Palette;	// Palette propre à la bitmap
	HBITMAP		HBitMap;	// Peut être NULL
    HBITMAP		Mask;	
	UBYTE		UFlags;
    UBYTE		Type;    
	UWORD		Width;
    // BitMap Amiga standard
    UWORD		BytesPerRow;    
	UWORD		Height;
    UBYTE		Flags;
    UBYTE		Depth;	// == BitsPerPixel si biPlanes == 1
	UWORD		pad;	// Utilisé par Amiga OS
    void		*Planes;	
	};

struct _OOPalette {	// Peut être castée en (LOGPALETTE*)
    UWORD           Version;
    UWORD           NumEntries;
    PALETTEENTRY    PaletteEntries[256];
	HPALETTE		Palette;
	HPEN			*PensTable;
	HBRUSH			*BrushesTable;
	UBYTE			Flags;
	UBYTE			PreDefColors[OOV_COLOR_COUNT];
	};
// Flags
#define OOF_PAL_ALLOCATED	0x01
#define OOF_PAL_CREATED		0x02

HBRUSH OOPrv_GetPaletteBrush( OOPalette *oopalette, ULONG index );

#endif // _WIN32

//=================================================================

//------
struct _OOTPInfo {
	FLOAT	truc;
	};

#endif // OO_GFX_PRV_H 
