#ifndef __OODIB_H_
#define __OODIB_H_

#include "oo.h"

//-------------------------------------------------------------------------------------
//--------------------------- Définition des formats ----------------------------------
//-------------------------------------------------------------------------------------

DECLARE_HANDLE(HDIB);
typedef BITMAPINFO DIB;

#define BYTESPERROW(bits)	(((bits) + 31) / 32 * 4)

#define PALVERSION   0x300

#define RECTWIDTH(r)		((r)->right - (r)->left)
#define RECTHEIGHT(r)		((r)->bottom - (r)->top)

#define ISWIN30DIB(bi)		(((*(ULONG*)(bi)) >= sizeof(BITMAPINFOHEADER)) ? TRUE : FALSE)

#define OOF_DIB_DRAWSHRINK		0x0001
#define OOF_DIB_DRAWEXPAND		0x0002
#define OOF_DIB_DRAWCENTER		0x0004

typedef struct tagBITMAPINFO256 {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[256];
} BITMAPINFO256, FAR *LPBITMAPINFO256, *PBITMAPINFO256;

//--------------------------- BMP

#define DIB_HEADER_MARKER   ((UWORD) ('M'<<8)|'B')

//--------------------------- TIFF

#define TIFF_INTEL		0x4949	// II = Intel
#define TIFF_MOTOROLA	0x4D4D	// MM = Motorola

#define TIFF_BYTE		1
#define TIFF_ASCII		2
#define TIFF_SHORT		3
#define TIFF_LONG		4
#define TIFF_RATIONAL	5

// Tags necessaires au TIFF CLASS B (Bilevel TIFF)

#define TAG_NEWSUBFILETYPE				254
#define TAG_SUBFILETYPE					255
#define TAG_IMAGEWIDTH					256
#define TAG_IMAGELENGTH					257
#define TAG_BITSPERSAMPLES				258
#define TAG_COMPRESSION					259
#define TAG_PHOTOMETRICINTERPRETATION	262
#define TAG_STRIPOFFSETS				273
#define TAG_ORIENTATION					274
#define TAG_SAMPLESPERPIXEL				277
#define TAG_ROWSPERSTRIP				278
#define TAG_STRIPBYTECOUNTS				279
#define TAG_XRESOLUTION					282
#define TAG_YRESOLUTION					283
#define TAG_PLANARCONFIGURATION			284
#define TAG_RESOLUTIONUNIT				296

#define TAGF_NEWSUBFILETYPE				0x0001
#define TAGF_IMAGEWIDTH					0x0002
#define TAGF_IMAGELENGTH				0x0004
#define TAGF_BITSPERSAMPLES				0x0008
#define TAGF_COMPRESSION				0x0010
#define TAGF_PHOTOMETRICINTERPRETATION	0x0020
#define TAGF_STRIPOFFSETS				0x0040
#define TAGF_ORIENTATION				0x0080
#define TAGF_SAMPLESPERPIXEL			0x0100
#define TAGF_ROWSPERSTRIP				0x0200
#define TAGF_STRIPBYTECOUNTS			0x0400
#define TAGF_XRESOLUTION				0x0800
#define TAGF_YRESOLUTION				0x1000
#define TAGF_PLANARCONFIGURATION		0x2000
#define TAGF_RESOLUTIONUNIT				0x4000

typedef struct {
	UWORD	ByteOrder; // II ou MM
	UWORD	Version;
	ULONG	IFDOffset; // Offset vers premier IFD
} TIFF_Header;

typedef struct {
	UWORD	Tag;	// Etiquette
	UWORD	Type;	// Type des donnees : SHORT, LONG, ...
	ULONG	Count;	// Nombre de "Value"
	ULONG	Value;	// Valeur, ou offset de la valeur
} TIFF_IFDEntry;


//-------------------------------------------------------------------------------------
//--------------------------- Prototypes ----------------------------------------------
//-------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// General utilities
ULONG		OOGfx_GetPaletteEntriesCount( HDC hdc );
HPALETTE	OOGfx_GetSystemPalette( void );
// DIB - Get functions
ULONG		OOGfx_DIB_GetWidth( DIB *dib );
ULONG		OOGfx_DIB_GetHeight( DIB *dib );
ULONG		OOGfx_DIB_GetBitsPerPixel( DIB *dib );
ULONG		OOGfx_DIB_GetBytesPerRow( DIB *dib );
ULONG		OOGfx_DIB_GetPlanesCount( DIB *dib );
UBYTE *		OOGfx_DIB_GetPlanes( DIB *dib );
ULONG		OOGfx_DIB_GetDPIX( DIB *dib, ULONG defaultvalue );
ULONG		OOGfx_DIB_GetDPIY( DIB *dib, ULONG defaultvalue );
ULONG		OOGfx_DIB_GetCompression( DIB *dib );
ULONG		OOGfx_DIB_GetColorsCount( DIB *dib );
ULONG		OOGfx_DIB_GetRGBCount( DIB *dib );
ULONG		OOGfx_DIB_GetRGBSize( DIB *dib );
HPALETTE	OOGfx_DIB_GetPalette( DIB *dib );
// DIB - Draw functions
BOOL		OOGfx_DIB_Draw( HDC hdc, LPRECT maxrect, ULONG flags, HDIB hdib, LPRECT srcrect, HPALETTE hpalette );
// DIB - Conversion functions
HBITMAP		OOGfx_DIB_ToBitmap( HDIB dib, HPALETTE hpalette );
HDIB		OOGfx_DIB_FromDC( HDC dc );
HDIB		OOGfx_DIB_FromBitmap( HBITMAP hbitmap, HPALETTE hpalette );
// DIB - Alloc/Free functions
HDIB		OOGfx_DIB_Alloc( ULONG width, ULONG height, UWORD bitsperpixel, HPALETTE hpalette );
void		OOGfx_DIB_Free( HDIB hdib );
HDIB		OOGfx_DIB_Clone( HDIB hdib );
// DIB - Load/Save file functions
BOOL		OOGfx_DIB_LoadBMI256( LPCTSTR filename, BITMAPINFO256 *bmi256 );
HDIB		OOGfx_DIB_Load( LPCTSTR filename );
BOOL		OOGfx_DIB_Save( HDIB hdib, LPCTSTR filename );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
