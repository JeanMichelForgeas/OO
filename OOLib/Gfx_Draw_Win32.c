/*****************************************************************
 *
 *       Project:    OO
 *       Function:
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "oo_gui_prv.h"
#include "oo_pool.h"
#include "oo_gfx_prv.h"


/******* Imported ************************************************/

extern OOGfxFuncs GfxSysFuncs;
extern OOGfxFuncs GfxHandFuncs;


/******* Exported ************************************************/


/******* Statics *************************************************/

static void set_palette_predef( OOPalette *oopalette );

static OOGfxFuncs* GfxFuncs[8] = {
	&GfxSysFuncs,
	&GfxHandFuncs,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
	};


/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

ULONG OOGfx_DrawInit( void )
{
    HDC dc;
	HPALETTE old;

    dc = GetDC( NULL );
    // Get Palette colors if possible. May fail.
	OOPalette256.Flags = 0;
	GetSystemPaletteEntries( dc, 0, 256, OOPalette256.PaletteEntries );
	// Create a système palette
	if ((OOPalette256.Palette = CreatePalette( (LOGPALETTE*)&OOPalette256 )) == NULL)
		return OOV_ERROR;
	OOPalette256.Flags |= OOF_PAL_CREATED;
	// Get standard colors indexes
	set_palette_predef( &OOPalette256 );
	// Eventually map palette colors to the system colors
	old = SelectPalette( dc, OOPalette256.Palette, TRUE );
	RealizePalette( dc );
	GetPaletteEntries( OOPalette256.Palette, 0, 256, OOPalette256.PaletteEntries );
	SelectPalette( dc, old, TRUE );
	RealizePalette( dc );
    ReleaseDC( NULL, dc );


	// Initialise les sous-systèmes
	if (GfxSysFuncs.DrawInit() != OOV_OK) return OOV_ERROR;
	if (GfxHandFuncs.DrawInit() != OOV_OK) return OOV_ERROR;

    return OOV_OK;
}

void OOGfx_DrawCleanup( void )
{
	GfxSysFuncs.DrawCleanup();
	GfxHandFuncs.DrawCleanup();
	OOGfx_FreePalette( &OOPalette256 );
}


/***************************************************************************
 *
 *		Palettes		
 *
 ***************************************************************************/

void OOGfx_FreePalette( OOPalette *oopalette )
{
	if (oopalette != NULL)
		{
		ULONG i;

		if (oopalette->PensTable != NULL) 
			{
			for (i=0; i < 256; i++) if (oopalette->PensTable[i]	!= NULL) DeleteObject( oopalette->PensTable[i] );
			OOPool_Free( OOGlobalPool, oopalette->PensTable );
			oopalette->PensTable = NULL;
			}
		if (oopalette->BrushesTable != NULL) 
			{
			for (i=0; i < 256; i++) if (oopalette->BrushesTable[i]	!= NULL) DeleteObject( oopalette->BrushesTable[i] );
			OOPool_Free( OOGlobalPool, oopalette->BrushesTable );
			oopalette->BrushesTable = NULL;
			}
		if (oopalette->Palette != NULL && (oopalette->Flags & OOF_PAL_CREATED)) 
			{ 
			DeleteObject( oopalette->Palette ); 
			oopalette->Palette = NULL; 
			}

		if (oopalette->Flags & OOF_PAL_ALLOCATED) OOPool_Free( OOGlobalPool, oopalette );
		}
}

OOPalette * OOGfx_AllocPalette( void *rgbtable, ULONG numcolors )
{
	OOPalette *oopalette = NULL;

	if (numcolors == 0 || numcolors > 256) rgbtable = NULL;
	if (rgbtable == NULL) numcolors = 256;

	if ((oopalette = (OOPalette*) OOPool_AllocClear( OOGlobalPool, sizeof(OOPalette) )) != NULL)
		{
		oopalette->Version = OOPalette256.Version;
		oopalette->Flags |= OOF_PAL_ALLOCATED;

		if (rgbtable == NULL) 
			{
			oopalette->NumEntries = OOPalette256.NumEntries;
			*oopalette->PaletteEntries = *OOPalette256.PaletteEntries;
			}
		else{
			ULONG i;
			RGBQUAD *rgb = (RGBQUAD*)rgbtable;

			oopalette->NumEntries = (UWORD)numcolors;
			for (i=0; i < numcolors; i++)
				{
				oopalette->PaletteEntries[i].peRed	= rgb[i].rgbRed;
				oopalette->PaletteEntries[i].peGreen= rgb[i].rgbGreen;
				oopalette->PaletteEntries[i].peBlue	= rgb[i].rgbBlue;
				oopalette->PaletteEntries[i].peFlags= (UBYTE)0;
				}
			}

		if ((oopalette->Palette = CreatePalette( (LOGPALETTE*)oopalette )) != NULL)
			{
			oopalette->Flags |= OOF_PAL_CREATED;
			set_palette_predef( oopalette );
			}
		else{
			OOPool_Free( OOGlobalPool, oopalette );
			oopalette = NULL;
			}
		}

	return oopalette;
}

static UBYTE reset_palette_color( OOPalette *oopalette, COLORREF color )
{
	UBYTE ind = GetNearestPaletteIndex( oopalette->Palette, color );
	PALETTEENTRY *pe = &oopalette->PaletteEntries[ind];
	pe->peRed	= GetRValue(color);
	pe->peGreen	= GetGValue(color);
	pe->peBlue	= GetBValue(color);
	SetPaletteEntries( oopalette->Palette, ind, 1, pe );
	return ind;
}

static void set_palette_predef( OOPalette *oopalette )
{
	oopalette->PreDefColors[OOV_COLOR_PREDEFBACK	] = reset_palette_color( oopalette, GetSysColor(COLOR_3DFACE)   );
	oopalette->PreDefColors[OOV_COLOR_PREDEFDARK	] = reset_palette_color( oopalette, GetSysColor(COLOR_3DDKSHADOW));
	oopalette->PreDefColors[OOV_COLOR_PREDEFMEDDARK	] = reset_palette_color( oopalette, GetSysColor(COLOR_3DSHADOW) );
	oopalette->PreDefColors[OOV_COLOR_PREDEFMEDLIGHT] = reset_palette_color( oopalette, GetSysColor(COLOR_3DLIGHT)	);
	oopalette->PreDefColors[OOV_COLOR_PREDEFLIGHT	] = reset_palette_color( oopalette, GetSysColor(COLOR_3DHILIGHT));
	oopalette->PreDefColors[OOV_COLOR_PREDEFBLACK	] = GetNearestPaletteIndex( oopalette->Palette, RGB(0,0,0)		);
	oopalette->PreDefColors[OOV_COLOR_PREDEFWHITE	] = GetNearestPaletteIndex( oopalette->Palette, RGB(255,255,255));
	oopalette->PreDefColors[OOV_COLOR_PREDEFRED		] = GetNearestPaletteIndex( oopalette->Palette, RGB(255,0,0)	);
	oopalette->PreDefColors[OOV_COLOR_PREDEFGREEN	] = GetNearestPaletteIndex( oopalette->Palette, RGB(0,255,0)	);
	oopalette->PreDefColors[OOV_COLOR_PREDEFBLUE	] = GetNearestPaletteIndex( oopalette->Palette, RGB(0,0,255)	);
	oopalette->PreDefColors[OOV_COLOR_PREDEFCYAN	] = GetNearestPaletteIndex( oopalette->Palette, RGB(0,255,255)	);
	oopalette->PreDefColors[OOV_COLOR_PREDEFMAGENTA	] = GetNearestPaletteIndex( oopalette->Palette, RGB(255,0,255)	);
	oopalette->PreDefColors[OOV_COLOR_PREDEFYELLOW	] = GetNearestPaletteIndex( oopalette->Palette, RGB(255,255,0)	);
}


/***************************************************************************
 *
 *      Drawing lines & pixels
 *
 ***************************************************************************/

void OOGfx_SetDrawColor( OORastPort *rp, ULONG colornum )
{
	OOPalette *oopalette;

	OOASSERT( rp->Palette != NULL );
	oopalette = rp->Palette;
	if (colornum >= 0x10000) colornum = oopalette->PreDefColors[colornum-0x10000];

	rp->DrawPen = (UBYTE)colornum;
}

void OOGfx_Move( OORastPort *rp, SLONG x, SLONG y )
{
	GfxFuncs[rp->DrawnBy]->Move( rp, x, y );
}

void OOGfx_DrawPixel( OORastPort *rp, SLONG x, SLONG y )
{
	OOASSERT( rp->Palette != NULL );
	GfxFuncs[rp->DrawnBy]->DrawPixel( rp, x, y );
}

void OOGfx_DrawLine( OORastPort *rp, SLONG x, SLONG y )
{
	GfxFuncs[rp->DrawnBy]->DrawLine( rp, x, y );
}

void OOGfx_DrawLine2( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
	GfxFuncs[rp->DrawnBy]->DrawLine2( rp, x1, y1, x2, y2 );
}

void OOGfx_DrawBox( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2, ULONG type )
{
	ULONG black, dark, light, white;

	switch (type)
		{
		case OOV_BOX_BUTTONUP:
		case OOV_BOX_HEAVYUP:
		case OOV_BOX_LIGHTUP:
		case OOV_BOX_BORDERUP:
		case OOV_BOX_GROOVEUP:
			black = OOV_COLOR_DARK;
			dark  = OOV_COLOR_MEDDARK; 
			light = OOV_COLOR_MEDLIGHT;
			white = OOV_COLOR_LIGHT;
			break;

		case OOV_BOX_BUTTONDOWN:
		case OOV_BOX_HEAVYDOWN:
		case OOV_BOX_LIGHTDOWN:
		case OOV_BOX_BORDERDOWN:
		case OOV_BOX_GROOVEDOWN:
			black = OOV_COLOR_LIGHT;
			dark  = OOV_COLOR_MEDLIGHT; 
			light = OOV_COLOR_MEDDARK;
			white = OOV_COLOR_DARK;
			break;

		case OOV_BOX_NULL:
		default:
			return;
		}

	x2--; y2--;
	switch (type)
		{
		case OOV_BOX_BUTTONUP:
		case OOV_BOX_BUTTONDOWN:
			OOGfx_SetDrawColor( rp, white );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2, y1 );
			OOGfx_SetDrawColor( rp, black );
			OOGfx_DrawLine( rp, x2, y2 ); OOGfx_DrawLine( rp, x1-1, y2 );
			x1++; y1++; x2--; y2--;
			OOGfx_SetDrawColor( rp, light );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2, y1 );
			OOGfx_SetDrawColor( rp, dark );
			OOGfx_DrawLine( rp, x2, y2 ); OOGfx_DrawLine( rp, x1-1, y2 );
			break;

		case OOV_BOX_HEAVYUP:
		case OOV_BOX_HEAVYDOWN:
			OOGfx_SetDrawColor( rp, light );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2, y1 );
			OOGfx_SetDrawColor( rp, black );
			OOGfx_DrawLine( rp, x2, y2 ); OOGfx_DrawLine( rp, x1-1, y2 );
			x1++; y1++; x2--; y2--;
			OOGfx_SetDrawColor( rp, white );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2, y1 );
			OOGfx_SetDrawColor( rp, dark );
			OOGfx_DrawLine( rp, x2, y2 ); OOGfx_DrawLine( rp, x1-1, y2 );
			break;

		case OOV_BOX_LIGHTUP:
			OOGfx_SetDrawColor( rp, OOV_COLOR_LIGHT );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2, y1 );
			OOGfx_SetDrawColor( rp, OOV_COLOR_MEDDARK );
			OOGfx_DrawLine( rp, x2, y2 ); OOGfx_DrawLine( rp, x1-1, y2 );
			break;
		case OOV_BOX_LIGHTDOWN:
			OOGfx_SetDrawColor( rp, OOV_COLOR_MEDDARK );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2, y1 );
			OOGfx_SetDrawColor( rp, OOV_COLOR_LIGHT );
			OOGfx_DrawLine( rp, x2, y2 ); OOGfx_DrawLine( rp, x1-1, y2 );
			break;

		case OOV_BOX_BORDERUP:
			OOGfx_DrawBox( rp, x1, y1, x2, y2, OOV_BOX_LIGHTUP );
			OOGfx_DrawBox( rp, x1+2, y1+2, x2-2, y2-2, OOV_BOX_LIGHTDOWN );
			break;
		case OOV_BOX_BORDERDOWN:
			OOGfx_DrawBox( rp, x1, y1, x2, y2, OOV_BOX_LIGHTDOWN );
			OOGfx_DrawBox( rp, x1+2, y1+2, x2-2, y2-2, OOV_BOX_LIGHTUP );
			break;

		case OOV_BOX_GROOVEUP:
			OOGfx_SetDrawColor( rp, OOV_COLOR_LIGHT );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2-1, y1 ); OOGfx_DrawLine( rp, x2-1, y2-1 );  OOGfx_DrawLine( rp, x1-1, y2-1 );
			OOGfx_SetDrawColor( rp, OOV_COLOR_MEDDARK );
			OOGfx_DrawLine2( rp, x1+1, y2-2, x1+1, y1+1 ); OOGfx_DrawLine( rp, x2-1, y1+1 );  
			OOGfx_DrawLine2( rp, x2, y1, x2, y2 );  OOGfx_DrawLine( rp, x1-1, y2 );
			break;
		case OOV_BOX_GROOVEDOWN:
			OOGfx_SetDrawColor( rp, OOV_COLOR_MEDDARK );
			OOGfx_DrawLine2( rp, x1, y2-1, x1, y1 ); OOGfx_DrawLine( rp, x2-1, y1 ); OOGfx_DrawLine( rp, x2-1, y2-1 );  OOGfx_DrawLine( rp, x1-1, y2-1 );
			OOGfx_SetDrawColor( rp, OOV_COLOR_LIGHT );
			OOGfx_DrawLine2( rp, x1+1, y2-2, x1+1, y1+1 ); OOGfx_DrawLine( rp, x2-1, y1+1 );  
			OOGfx_DrawLine2( rp, x2, y1, x2, y2 );  OOGfx_DrawLine( rp, x1-1, y2 );
			break;
		}
}

static OOBorder BoxBorders[] = {
	{ 0, 0, 0, 0 }, // OOV_BOX_NULL	
	{ 2+2, 2+2, 2+1, 2+1 }, // OOV_BOX_BUTTONUP
	{ 2+2, 2+2, 2+1, 2+1 }, // OOV_BOX_BUTTONDOWN
	{ 2+2, 2+2, 2+1, 2+1 }, // OOV_BOX_HEAVYUP	
	{ 2+2, 2+2, 2+1, 2+1 }, // OOV_BOX_HEAVYDOWN
	{ 1+2, 1+2, 1+1, 1+1 }, // OOV_BOX_LIGHTUP	
	{ 1+2, 1+2, 1+1, 1+1 }, // OOV_BOX_LIGHTDOWN
	{ 3+2, 3+2, 3+1, 3+1 }, // OOV_BOX_BORDERUP
	{ 3+2, 3+2, 3+1, 3+1 }, // OOV_BOX_BORDERDOWN
	{ 2+2, 2+2, 2+1, 2+1 }, // OOV_BOX_GROOVEUP
	{ 2+2, 2+2, 2+1, 2+1 }, // OOV_BOX_GROOVEDOWN
	};

OOBorder *OOGfx_GetBoxBorder( ULONG type )
{
	return &BoxBorders[type];
}


/***************************************************************************
 *
 *      Drawing polygons
 *
 ***************************************************************************/

void OOGfx_DrawRect( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
	GfxFuncs[rp->DrawnBy]->DrawRect( rp, x1, y1, x2, y2 );
}

void OOGfx_DrawPolygon( OORastPort *rp, OOPolygon *pg, UBYTE *info_tab )
{
	GfxFuncs[rp->DrawnBy]->DrawPolygon( rp, pg, info_tab );
}
