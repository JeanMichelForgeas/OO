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

extern ULONG OOGfx_FontInit( void );
extern void  OOGfx_FontCleanup( void );
extern ULONG OOGfx_DrawInit( void );
extern void  OOGfx_DrawCleanup( void );


/******* Exported ************************************************/


/******* Statics *************************************************/


//==========================================================================
//==========================================================================
//==========================================================================
//                              WIN32
//==========================================================================
//==========================================================================
//==========================================================================

#ifdef _WIN32

/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

ULONG OOGfx_Init( void )
{
	if (OOGfx_FontInit() != OOV_OK) return OOV_ERROR;
	if (OOGfx_DrawInit() != OOV_OK) return OOV_ERROR;
    return OOV_OK;
}

void OOGfx_Cleanup( void )
{
	OOGfx_DrawCleanup();
	OOGfx_FontCleanup();
}

#endif // _WIN32


//==========================================================================
//==========================================================================
//==========================================================================
//                              AMIGA
//==========================================================================
//==========================================================================
//==========================================================================

#ifdef AMIGA

/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

ULONG OOGfx_Init( void )
{
    return OOV_OK;
}

void OOGfx_Cleanup( void )
{
}

#endif // AMIGA


//==========================================================================
//==========================================================================
//==========================================================================
//                              COMMON CODE
//==========================================================================
//==========================================================================
//==========================================================================

/***************************************************************************
 *
 *      Drawing images
 *
 ***************************************************************************/

/*
ULONG OOGfx_GetImageSize( OOImage *im )
{
  short maxx=MINWORD, maxy=MINWORD, minx=MAXWORD, miny=MAXWORD, X, Y;

    for ( ; im; im=im->Next)
        {
        if (im->SubBox)
            {
            X = im->DestXOffs + im->SubBox[im->BoxNum].W - 1;
            Y = im->DestYOffs + im->SubBox[im->BoxNum].H - 1;
            }
        else{
            X = im->DestXOffs + im->OOBitMap->Width - 1;
            Y = im->DestYOffs + im->OOBitMap->Height - 1;
            }
        if (im->DestXOffs < minx) minx = im->DestXOffs;
        if (im->DestYOffs < miny) miny = im->DestYOffs;
        if (X > maxx) maxx = X;
        if (Y > maxy) maxy = Y;
        }
    return( W2L((maxx-minx+1),(maxy-miny+1)) );
}

void OOGfx_PutImage( OORastPort *rp, OOImage *im, SLONG x, SLONG y, OORect *cliprect )
{
    for ( ; im; im=im->Next)
        {
#ifdef AMIGA
        if (im->OOBitMap->UFlags & OOBMAP_MASKED)
            BltMaskBitMapRastPort( OO2BM(im->OOBitMap), on->XOffs, on->YOffs,
                        rp, x, y, on->NewW, on->NewH, 0xe0, im->OOBitMap->Mask );
        else
            BltBitMapRastPort( OO2BM(im->OOBitMap), on->XOffs, on->YOffs,
                        rp, x, y, on->NewW, on->NewH, 0xc0 );
#endif
#ifdef _WIN32
#endif
        }
}
*/
