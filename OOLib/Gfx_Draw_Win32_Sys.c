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


/******* Statics *************************************************/

static ULONG		OOGfxSys_DrawInit( void );
static void			OOGfxSys_DrawCleanup( void );
static void			OOGfxSys_FreePalette( OOPalette *oopalette );
static OOPalette *	OOGfxSys_AllocPalette( void *rgbtable, ULONG numcolors );
static void			OOGfxSys_SetDrawColor( OORastPort *rp, ULONG colornum );
static void			OOGfxSys_Move( OORastPort *rp, SLONG x, SLONG y );
static void			OOGfxSys_DrawPixel( OORastPort *rp, SLONG x, SLONG y );
static void			OOGfxSys_DrawLine( OORastPort *rp, SLONG x, SLONG y );
static void			OOGfxSys_DrawLine2( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
static void			OOGfxSys_DrawRect( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
static void			OOGfxSys_DrawBox( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2, ULONG type );
static void			OOGfxSys_DrawPolygon( OORastPort *rp, OOPolygon *pg, UBYTE *info_tab );


/******* Exported ************************************************/

OOGfxFuncs GfxSysFuncs = {
	OOGfxSys_DrawInit,
	OOGfxSys_DrawCleanup,
	OOGfxSys_FreePalette,
	OOGfxSys_AllocPalette,
	OOGfxSys_SetDrawColor,
	OOGfxSys_Move,
	OOGfxSys_DrawPixel,
	OOGfxSys_DrawLine,
	OOGfxSys_DrawLine2,
	OOGfxSys_DrawRect,
	OOGfxSys_DrawBox,
	OOGfxSys_DrawPolygon,
};


/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

static ULONG OOGfxSys_DrawInit( void )
{
    return OOV_OK;
}

static void OOGfxSys_DrawCleanup( void )
{
}


/***************************************************************************
 *
 *		Palettes		
 *
 ***************************************************************************/

static void OOGfxSys_FreePalette( OOPalette *oopalette )
{
	OOASSERT( FALSE ); // Pas utilisée
}

static OOPalette * OOGfxSys_AllocPalette( void *rgbtable, ULONG numcolors )
{
	OOASSERT( FALSE ); // Pas utilisée
	return NULL;
}

static void set_palette_pen( OORastPort *rp )
{
	if (rp->DrawPen != rp->PenSet)
		{
		OOPalette *oopalette = rp->Palette;
		HPEN pen;

		if (oopalette->PensTable == NULL)
			if ((oopalette->PensTable = OOPool_AllocClear( OOGlobalPool, sizeof(HPEN)*256 )) == NULL)
				return;

		if ((pen = oopalette->PensTable[rp->DrawPen]) == NULL)
			{
			PALETTEENTRY *pe = &oopalette->PaletteEntries[rp->DrawPen];
			pen = oopalette->PensTable[rp->DrawPen] = CreatePen( PS_SOLID, 0, RGB(pe->peRed,pe->peGreen,pe->peBlue) );
			}

		if (pen != NULL)
			{
			SelectObject( rp->DC, pen );
			rp->PenSet = rp->DrawPen;
			}
		}
}

static void set_palette_brush( OORastPort *rp )
{
	OOASSERT( FALSE ); // Fonction pas utilisée
	if (rp->DrawPen != rp->PenSet)
		{
		OOPalette *oopalette = rp->Palette;
	    HBRUSH brush;

		if (oopalette->BrushesTable == NULL)
			if ((oopalette->BrushesTable = OOPool_AllocClear( OOGlobalPool, sizeof(HBRUSH)*256 )) == NULL)
				return;

		if ((brush = oopalette->BrushesTable[rp->DrawPen]) == NULL)
			{
			PALETTEENTRY *pe = &oopalette->PaletteEntries[rp->DrawPen];
			brush = oopalette->BrushesTable[rp->DrawPen] = CreateSolidBrush( RGB(pe->peRed,pe->peGreen,pe->peBlue) );
			}

		if (brush != NULL)
			{
			SelectObject( rp->DC, brush );
			rp->BrushSet = rp->DrawPen;
			}
		}
}

HBRUSH OOPrv_GetPaletteBrush( OOPalette *oopalette, ULONG index )
{
	HBRUSH brush;

	if (index >= 0x10000) index = oopalette->PreDefColors[index-0x10000];

	if (oopalette->BrushesTable == NULL)
		if ((oopalette->BrushesTable = OOPool_AllocClear( OOGlobalPool, sizeof(HBRUSH)*256 )) == NULL)
			return NULL;

	if ((brush = oopalette->BrushesTable[index]) == NULL)
		{
		PALETTEENTRY *pe = &oopalette->PaletteEntries[index];
		brush = oopalette->BrushesTable[index] = CreateSolidBrush( RGB(pe->peRed,pe->peGreen,pe->peBlue) );
		}

	return brush;
}

static HBRUSH get_palette_brush( OORastPort *rp )
{
	return OOPrv_GetPaletteBrush( rp->Palette, rp->DrawPen );
}


/***************************************************************************
 *
 *      Drawing lines & pixels
 *
 ***************************************************************************/

static void OOGfxSys_SetDrawColor( OORastPort *rp, ULONG colornum )
{
	OOASSERT( FALSE ); // Pas utilisée
}

static void OOGfxSys_Move( OORastPort *rp, SLONG x, SLONG y )
{
    MoveToEx( rp->DC, x, y, NULL );
}

static void OOGfxSys_DrawPixel( OORastPort *rp, SLONG x, SLONG y )
{
    PALETTEENTRY *pe = &rp->Palette->PaletteEntries[rp->DrawPen];
    SetPixelV( rp->DC, x, y, RGB(pe->peRed,pe->peGreen,pe->peBlue) );
}

static void OOGfxSys_DrawLine( OORastPort *rp, SLONG x, SLONG y )
{
	set_palette_pen( rp );
    LineTo( rp->DC, x, y );
}

static void OOGfxSys_DrawLine2( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
	set_palette_pen( rp );
    MoveToEx( rp->DC, x1, y1, NULL );
    LineTo( rp->DC, x2, y2 );
}

static void OOGfxSys_DrawBox( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2, ULONG type )
{
	OOASSERT( FALSE ); // Pas utilisée
}


/***************************************************************************
 *
 *      Drawing polygons
 *
 ***************************************************************************/

static void OOGfxSys_DrawRect( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
	RECT _R = { x1,y1, x2,y2 };
	HBRUSH brush = get_palette_brush( rp );
	FillRect( rp->DC, &_R, brush );
}

static void OOGfxSys_DrawPolygon( OORastPort *rp, OOPolygon *pg, UBYTE *info_tab )
{
    ULONG numpoints = pg->NumPoints;
    OOPoint *pt = pg->Points;

//    if (! (pg->Flags & (OOF_FILL|OOF_MAP|OOF_LINES))) return;

    switch (numpoints)
        {
        case 1:
//            OOGfx_SetDrawColor( rp, (pg->Flags & OOF_LINES) ? pg->LineColor : pg->FillColor );
            OOGfx_DrawPixel( rp, pt[0].x, pt[0].y );
            break;

        case 2:
//            OOGfx_SetDrawColor( rp, (pg->Flags & OOF_LINES) ? pg->LineColor : pg->FillColor );
			OOGfx_DrawLine2( rp, pt[0].x, pt[0].y, pt[1].x, pt[1].y );
            break;

        default:
//            if (pg->Flags & (OOF_FILL|OOF_MAP))
                {
                HRGN region;
				HBRUSH brush;
                if ((region = CreatePolygonRgn( (POINT*)pt, numpoints, ALTERNATE )) != NULL)
                    {
					OOGfx_SetDrawColor( rp, pg->FillColor );
					if ((brush = get_palette_brush( rp )) != NULL)
						FillRgn( rp->DC, region, brush );
                    DeleteObject( region );
                    }
                }
//            if (pg->Flags & OOF_LINES)
                {
                OOGfx_SetDrawColor( rp, pg->LineColor );
				set_palette_pen( rp );
                if (info_tab == NULL)
                    Polyline( rp->DC, (POINT*)pt, numpoints+1 );
                else{
                    ULONG i;
                    MoveToEx( rp->DC, pt[0].x, pt[0].y, NULL );
                    for (i=1; i <= numpoints; i++)
                        {
                        if (info_tab[i-1] != 0) LineTo( rp->DC, pt[i].x, pt[i].y );
                        else MoveToEx( rp->DC, pt[i].x, pt[i].y, NULL );
                        }
                    }
                }
            break;
        }
}
