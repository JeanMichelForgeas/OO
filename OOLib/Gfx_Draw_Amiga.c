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


/******* Exported ************************************************/


/******* Statics *************************************************/


/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

ULONG OOGfx_DrawInit( void )
{
	OOASSERT( FALSE ); // Préparer palette par défaut
    return OOV_OK;
}

void OOGfx_DrawCleanup( void )
{
}


/***************************************************************************
 *
 *		Palettes		
 *
 ***************************************************************************/

void OOGfx_FreePalette( OOPalette *oopalette )
{
	OOASSERT( FALSE );
}

OOPalette * OOGfx_AllocPalette( void *rgbtable, ULONG numcolors )
{
	OOASSERT( FALSE );
}


/***************************************************************************
 *
 *      Drawing lines & pixels
 *
 ***************************************************************************/

void OOGfx_SetDrawColor( OORastPort *rp, ULONG colornum )
{
    SetAPen( rp, colornum );
}

void OOGfx_Move( OORastPort *rp, SLONG x, SLONG y )
{
    Move( rp, x, y );
}

void OOGfx_DrawPixel( OORastPort *rp, SLONG x, SLONG y )
{
    WritePixel( rp, x, y );
}

void OOGfx_DrawLine( OORastPort *rp, SLONG x, SLONG y )
{
    Draw( rp, x, y );
}

void OOGfx_DrawLine2( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
    Move( rp, x1, y1 );
    Draw( rp, x2, y2 );
}

void OOGfx_DrawRect( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
	RectFill( rp, x1, y1, x2, y2 );
}


/***************************************************************************
 *
 *      Drawing polygons
 *
 ***************************************************************************/

void OOGfx_DrawPolygon( OORastPort *rp, OOPolygon *pg, UBYTE *info_tab )
{
	OOASSERT( FALSE );
}
