/*****************************************************************
 *
 *		 Project:	 OO
 *		 Function:
 *
 *		 Created:		 Jean-Michel Forgeas
 *
 *		 This code is CopyRight © 1991-1998 Jean-Michel Forgeas
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
 *		BitMaps & RastPorts
 *
 ***************************************************************************/

// Inits nécessaires pour un rastport réservé sur la pile, avec un DC donné
void OOGfx_InitRPort( OORastPort *rport )
{
	rport->DC = NULL;
	rport->Palette = NULL;
	rport->BitMap = NULL;
	rport->OldHPalette = NULL;
	rport->OldHBitMap = NULL;
	rport->Flags = 0;
	rport->DrawnBy = OOV_RP_DRAWNBY_SYS;
	rport->DrawPen = 0;
	rport->PenSet = rport->BrushSet = 1; // Different de DrawPen
	rport->Context = NULL;
}

void OOGfx_FreeRPort( OORastPort *rport )
{
	if (rport != NULL)
		{
		if (rport->DC != NULL && (rport->Flags & OOF_RP_DC_ALLOCATED)) 
			DeleteDC( rport->DC );
		if (rport->Flags & OOF_RP_ALLOCATED)
			OOPool_Free( OOGlobalPool, rport );
		}
}

OORastPort * OOGfx_AllocRPort( void )
{
	OORastPort *rport;
	HDC dc;

	if ((rport = (OORastPort *) OOPool_Alloc( OOGlobalPool, sizeof(OORastPort) )) != NULL)
		{
		if (dc = CreateCompatibleDC( NULL ))
			{
			OOGfx_InitRPort( rport );
			rport->DC = dc;
			rport->Flags |= OOF_RP_ALLOCATED;
			rport->Flags |= OOF_RP_DC_ALLOCATED;
			return rport;
			}
		OOPool_Free( OOGlobalPool, rport );
		}
	return NULL;
}

//----------------------------------

void OOGfx_FreeBMap( OOBitMap *bm )
{
	if (bm != NULL)
		{
		if (! (bm->UFlags & OOBMAP_NOPLANE))
			{
			//+++ if (bm->UFlags & OOBMAP_CONTIG)
			if (bm->HBitMap != NULL) DeleteObject( bm->HBitMap );
			}
		if (bm->Mask != NULL) DeleteObject( bm->Mask );
		if (bm->Palette != NULL) OOGfx_FreePalette( bm->Palette );

		OOPool_Free( OOGlobalPool, bm );
		}
}

OOBitMap *OOGfx_AllocBMap( OOPalette *pal, UBYTE depth, UWORD width, UWORD height, ULONG uflags )
{
	OOBitMap *bm;

	if ((bm = (OOBitMap *) OOPool_Alloc( OOGlobalPool, sizeof(OOBitMap) )) != NULL)
		{
		bm->Palette		= pal;
		bm->UFlags		= (UBYTE)uflags;
		bm->Width		= width;
		bm->Height		= height;
		bm->Type		= 0;
		bm->BytesPerRow = OOGFX_BYTESPERROW(width*depth);
		bm->Depth		= depth;

		if (bm->UFlags & OOBMAP_NOPLANE)
			{
			return bm;
			}
		else{
			BITMAPINFOHEADER *header;
			RGBQUAD *rgb;
			PALETTEENTRY *pe;
			HDC screen_dc;
			ULONG i, allocsize = sizeof(BITMAPINFOHEADER) + (256*sizeof(RGBQUAD)) /*+ (widthbytes*height)*/;

			if ((header = OOPool_Alloc( OOGlobalPool, allocsize )) != NULL )
				{
				memset( header, 0, allocsize );
				header->biSize			= sizeof(BITMAPINFOHEADER);
				header->biWidth 		= width;
				header->biHeight		= height;
				header->biPlanes		= 1;
				header->biBitCount		= depth;
				header->biCompression	= BI_RGB;
				header->biSizeImage 	= bm->BytesPerRow * height;

				rgb = (RGBQUAD*)((UBYTE*)header+header->biSize);
				pe = pal->PaletteEntries;
				for (i=0; i < 256; i++, rgb++, pe++)
					{
					rgb->rgbBlue	 = pe->peBlue;
					rgb->rgbGreen	 = pe->peGreen;
					rgb->rgbRed 	 = pe->peRed;
					rgb->rgbReserved = 0; //PC_NOCOLLAPSE;
					}

				if (bm->UFlags & OOBMAP_MASKED) bm->Mask = CreateBitmap( width, height, 1, 1, NULL );
				else bm->Mask = NULL;

				if ((screen_dc = GetDC( NULL )) != NULL)
					{
					//+++ if (bm->UFlags & OOBMAP_CONTIG)
					if ((bm->HBitMap = CreateDIBSection( screen_dc, (BITMAPINFO*)header, DIB_RGB_COLORS, &bm->Planes, NULL, 0 )) != NULL )
						{
						ReleaseDC( NULL, screen_dc );
						OOPool_Free( OOGlobalPool, header );
						return bm;
						}
					ReleaseDC( NULL, screen_dc );
					}
				OOPool_Free( OOGlobalPool, header );
				}
			}
		OOPool_Free( OOGlobalPool, bm );
		}
	return NULL;
}

//----------------------------------

void OOGfx_FreeBMapRPort( OORastPort *rport )
{
	if (rport != NULL)
		{
		if (rport->OldHBitMap != NULL) SelectObject( rport->DC, rport->OldHBitMap );
		if (rport->OldHPalette != NULL) SelectPalette( rport->DC, rport->OldHPalette, TRUE );
		OOGfx_FreeBMap( rport->BitMap );
		OOGfx_FreeRPort( rport );
		}
}

OORastPort * OOGfx_AllocBMapRPort( OOPalette *pal, UBYTE depth, UWORD width, UWORD height, ULONG uflags )
{
  OORastPort *rport=0;

	if ((rport = OOGfx_AllocRPort()) != NULL)
		{
		if ((rport->BitMap = OOGfx_AllocBMap( pal, depth, width, height, uflags )) != NULL)
			{
			if ((rport->Palette = pal) != NULL) 
				rport->OldHPalette = SelectPalette( rport->DC, pal->Palette, TRUE );
			if (rport->BitMap->HBitMap != NULL) // Donc si !(uflags & OOBMAP_NOPLANE)
				rport->OldHBitMap = SelectObject( rport->DC, rport->BitMap->HBitMap );
			return rport;
			}
		OOGfx_FreeRPort( rport );
		}
	return NULL;
}


/***************************************************************************
 *
 *		
 *
 ***************************************************************************/

void * OOGfx_DIB_Alloc( ULONG width, ULONG height, UWORD bitsperpixel, BOOL allocplane )
{
	BITMAPINFOHEADER *bi;
	ULONG allocsize, tablesize, numcolors;

	if (bitsperpixel <= 1) bitsperpixel = 1;
	else if (bitsperpixel <= 4) bitsperpixel = 4;
	else if (bitsperpixel <= 8) bitsperpixel = 8;
	else if (bitsperpixel <= 24) bitsperpixel = 24;
	else bitsperpixel = 8;	// set default value to 8 if parameter is bogus

	switch (bitsperpixel)
		{
		case 1:		numcolors = 2;	break;
		case 4:		numcolors = 16;	break;
		case 8:		numcolors = 256;break;
		default:	numcolors = 0;	break;
		}
	tablesize = numcolors *	sizeof(RGBQUAD);

	allocsize = sizeof(BITMAPINFOHEADER) + tablesize;
	if (allocplane == TRUE) allocsize += (OOGFX_BYTESPERROW(bitsperpixel*width) * height);

	if ((bi = (BITMAPINFOHEADER*) OOPool_Alloc( OOGlobalPool, allocsize )) != NULL) 
		{
		bi->biSize = sizeof(BITMAPINFOHEADER);
		bi->biWidth = width; 
		bi->biHeight = height;
		bi->biPlanes = 1;	
		bi->biBitCount = bitsperpixel;
		bi->biCompression = BI_RGB;	  
		bi->biSizeImage = 0;
		bi->biXPelsPerMeter = 0;
		bi->biYPelsPerMeter = 0;
		bi->biClrUsed = 0;
		bi->biClrImportant = 0;
		}
	return bi;
}


//************************************************************************
// This function gets the size required to store the DIB's palette by
// multiplying the number of colors by the size of an RGBQUAD (for a
// Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an OS/2-
// style DIB).

ULONG OOGfx_DIB_PaletteSize( void *dib )
{
	if (OOGFX_DIB_ISWIN30(dib)) return (OOGfx_DIB_NumTableColors(dib) * sizeof(RGBQUAD));
	else return (OOGfx_DIB_NumTableColors(dib) * sizeof(RGBTRIPLE));
}


//*************************************************************************
// This function calculates the number of colors in the DIB's color table.
// If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256.
// If 24, no colors in color table.

ULONG OOGfx_DIB_NumTableColors( void *dib )
{
	UWORD bitsperpixel;

	if (OOGFX_DIB_ISWIN30(dib))
		{
		if (((BITMAPINFOHEADER*)dib)->biClrUsed) return ((BITMAPINFOHEADER*)dib)->biClrUsed;
		else bitsperpixel = ((BITMAPINFOHEADER*)dib)->biBitCount;
		}
	else bitsperpixel = ((BITMAPCOREHEADER*)dib)->bcBitCount;

	switch (bitsperpixel)
		{
		case 1:		return 2;
		case 4:		return 16;
		case 8:		return 256;
		default:	return 0;
		}
}


//***************************************************************************
// This function creates a palette from a DIB by allocating memory for the
// logical palette, reading and storing the colors from the DIB's color table
// into the logical palette, creating a palette from this logical palette,
// and then returning the palette's handle. This allows the DIB to be
// displayed using the best possible colors (important for DIBs with 256 or
// more colors).

HPALETTE OOGfx_GetDIBPalette( void *dib )
{
	LOGPALETTE		*logpal;
	HPALETTE		palette = NULL;
	LONG 			i, numtablecolors;
	BITMAPINFO		*bmi = (BITMAPINFO*)dib;		// Win3.0
	BITMAPCOREINFO	*bmc = (BITMAPCOREINFO*)dib;	// OS/2

	if (dib != NULL && (numtablecolors = OOGfx_DIB_NumTableColors(dib)))
		{
		if ((logpal = (LOGPALETTE*) OOPool_Alloc( OOGlobalPool, sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * numtablecolors) )) != NULL)
			{
			logpal->palVersion = 0x300;
			logpal->palNumEntries = (UWORD)numtablecolors;
			// store RGB triples (if Win 3.0 DIB) or RGB quads (if OS/2 DIB)
			if (OOGFX_DIB_ISWIN30(dib))
				{
				for (i = 0; i < numtablecolors; i++)
					{
					logpal->palPalEntry[i].peRed = bmi->bmiColors[i].rgbRed;
					logpal->palPalEntry[i].peGreen = bmi->bmiColors[i].rgbGreen;
					logpal->palPalEntry[i].peBlue = bmi->bmiColors[i].rgbBlue;
					logpal->palPalEntry[i].peFlags = 0;
					}
				}
			else{
				for (i = 0; i < numtablecolors; i++)
					{
					logpal->palPalEntry[i].peRed = bmc->bmciColors[i].rgbtRed;
					logpal->palPalEntry[i].peGreen = bmc->bmciColors[i].rgbtGreen;
					logpal->palPalEntry[i].peBlue = bmc->bmciColors[i].rgbtBlue;
					logpal->palPalEntry[i].peFlags = 0;
					}
				}
			palette = CreatePalette( logpal );

			OOPool_Free( OOGlobalPool, logpal );
			}
		}

	return palette;
}

//***************************************************************************
// Gets the number of palette entries on the specified rastport

ULONG OOGfx_GetPaletteEntriesCount( HDC dc )
{
	ULONG numcolors = (1 << (GetDeviceCaps( dc, BITSPIXEL ) * GetDeviceCaps( dc, PLANES )));
	OOASSERT( numcolors );
	return numcolors;
}

//**************************************************************************
// This function returns a handle to a palette which represents the system
// palette. The system RGB values are copied into our logical palette using
// the GetSystemPaletteEntries function.  

HPALETTE OOGfx_GetSystemPalette( void )
{
	HDC dc;
	static HPALETTE palette = NULL;
	LOGPALETTE *logpal;
	ULONG numcolors;

	if ((dc = GetDC(NULL)) != NULL)
		{
		numcolors = OOGfx_GetPaletteEntriesCount( dc );	 // Number of palette entries
		if ((logpal = OOPool_Alloc( OOGlobalPool, sizeof(LOGPALETTE) + numcolors * sizeof(PALETTEENTRY) )) != NULL)
			{
			logpal->palVersion = 0x300;
			logpal->palNumEntries = (UWORD)numcolors;
			GetSystemPaletteEntries( dc, 0, numcolors, (PALETTEENTRY*)logpal->palPalEntry );
			palette = CreatePalette( logpal );

			OOPool_Free( OOGlobalPool, logpal );
			}
		ReleaseDC( NULL, dc );
		}

    if (palette == NULL) palette = GetStockObject( DEFAULT_PALETTE );
	
	return palette;
}


//***********************************************************************
// The bitmap returned from this function can be a bitmap compatible
// with the screen (e.g. same bits/pixel and color planes) or
// a bitmap with the same attributes as the DIB.

HBITMAP OOGfx_DIB_ToBitmap( void *dib, BOOL screen_compatible )
{
	void		*plane;
	HBITMAP 	bitmap = NULL;
	HDC 		dc;

	if (dib != NULL)
		{
		plane = OOGFX_DIB_PLANE(dib);

		if (screen_compatible == TRUE)
			{
			if ((dc = GetDC( NULL )) != NULL)
				{
				HPALETTE oldpalette, palette = OOGfx_GetDIBPalette( dib );
				if (palette == NULL) palette = OOGfx_GetSystemPalette();
				if (palette != NULL) { oldpalette = SelectPalette( dc, palette, FALSE ); RealizePalette( dc ); }

				bitmap = CreateDIBitmap( dc, (BITMAPINFOHEADER*)dib, CBM_INIT, plane, (BITMAPINFO*)dib, DIB_RGB_COLORS );
				
				if (oldpalette != NULL) { SelectPalette( dc, oldpalette, FALSE ); DeleteObject( palette ); }
				ReleaseDC( NULL, dc );
				}
			}
		else{
			bitmap = CreateBitmap( OOGFX_DIB_WIDTH(dib), OOGFX_DIB_HEIGHT(dib), 1, OOGFX_DIB_BITSPERPIXEL(dib), plane );
			}
		}
	return bitmap;
}


//************************************************************************
// This function creates a DIB from a bitmap using the specified palette.

void * OOGfx_BMap_ToDIB( HBITMAP bitmap )
{
	BITMAP				BMP;
	BITMAPINFOHEADER	*bi, *dib = NULL;
	ULONG				allocsize;
	HDC 				dc;

	if (bitmap != NULL && GetObject( bitmap, sizeof(BMP), (void*)&BMP) && (dc = GetDC(NULL)) != NULL)
		{
		UWORD bitsperpixel = BMP.bmPlanes * BMP.bmBitsPixel;
		if ((bi = (BITMAPINFOHEADER*) OOGfx_DIB_Alloc( BMP.bmWidth, BMP.bmHeight, bitsperpixel, FALSE )) != NULL)
			{
			// call GetDIBits with a NULL lpBits param, so it will calculate the biSizeImage field for us
			GetDIBits( dc, bitmap, 0, (UINT)bi->biHeight, NULL, (BITMAPINFO*)bi, DIB_RGB_COLORS );
			// if the driver did not fill in the biSizeImage field, make one up 
			if (bi->biSizeImage == 0) bi->biSizeImage = OOGFX_BYTESPERROW(bi->biWidth*bi->biBitCount) * bi->biHeight;

			allocsize = bi->biSize + OOGfx_DIB_PaletteSize((void*)bi) + bi->biSizeImage;
			if ((dib = (BITMAPINFOHEADER*) OOPool_Alloc( OOGlobalPool, allocsize )) != NULL)
				{
				*dib = *bi;
				// call GetDIBits with a NON-NULL lpBits param, and actualy get the bits this time
				if (GetDIBits( dc, bitmap, 0, dib->biHeight, OOGFX_DIB_PLANE(dib), (BITMAPINFO*)dib, DIB_RGB_COLORS ) == 0)
					{ OOPool_Free( OOGlobalPool, dib ); dib = NULL; }
				}
			OOPool_Free( OOGlobalPool, bi ); 
			}
		ReleaseDC( NULL, dc );
		}

	return dib;
}

/*


void * OOGfx_BMap_ToDIB( HBITMAP bitmap )
{
	BITMAP				BMP;
	BITMAPINFOHEADER	*bi, *dib = NULL;
	ULONG				allocsize;
	HDC 				dc;

	if (bitmap != NULL && GetObject( bitmap, sizeof(BMP), (void*)&BMP) && (dc = GetDC(NULL)) != NULL)
		{
		bitsperpixel = BMP.bmPlanes * BMP.bmBitsPixel;
		if ((bi = (BITMAPINFOHEADER*) OOGfx_DIB_Alloc( BMP.bmWidth, BMP.bmHeight, bitsperpixel, FALSE ))
			{
			// call GetDIBits with a NULL lpBits param, so it will calculate the biSizeImage field for us
			GetDIBits( dc, bitmap, 0, (UINT)bi->biHeight, NULL, (BITMAPINFO*)bi, DIB_RGB_COLORS );
			// if the driver did not fill in the biSizeImage field, make one up 
			if (bi->biSizeImage == 0) bi->biSizeImage = OOGFX_BYTESPERROW(bi->biWidth*bi->biBitCount) * bi->biHeight;

			allocsize = bi->biSize + OOGfx_DIB_PaletteSize((void*)bi) + bi->biSizeImage;
			if ((dib = (BITMAPINFOHEADER*) OOPool_Alloc( OOGlobalPool, allocsize )) != NULL)
				{
				*dib = *bi
				// call GetDIBits with a NON-NULL lpBits param, and actualy get the bits this time
				if (GetDIBits( dc, bitmap, 0, dib->biHeight, OOGFX_DIB_PLANE(dib), (BITMAPINFO*)dib, DIB_RGB_COLORS ) == 0)
					{ OOPool_Free( OOGlobalPool, dib ); dib == NULL; }
				}
			OOPool_Free( OOGlobalPool, bi ); 
			}
		ReleaseDC( NULL, dc );
		}

	return dib;
}

HANDLE AllocRoomForDIB( ULONG width, ULONG height, ULONG bitsperpixel, ULONG compression, HBITMAP bitmap )
{
	ULONG				allocsize;
	HDC 				dc;
	BITMAPINFOHEADER	*bi, *dib = NULL;

	allocsize = header.biSize + OOGfx_DIB_PaletteSize((void*)&header);
	if ((bi = OOPool_Alloc( OOGlobalPool, allocsize )) != NULL)
		{
		*bi = header;

		// Figure out the size needed to hold the BITMAPINFO structure
		dc = GetDC( NULL );
		GetDIBits( dc, bitmap, 0, (UINT) header.biHeight, NULL, (BITMAPINFO*)bi, DIB_RGB_COLORS );
		ReleaseDC( NULL, dc );
		// If the driver did not fill in the biSizeImage field, NOTE: this is a bug in the driver!
		if (bi->biSizeImage == 0)
			bi->biSizeImage = OOGFX_BYTESPERROW((ULONG)bi->biWidth * bi->biBitCount) * bi->biHeight;

		allocsize = bi->biSize + OOGfx_DIB_PaletteSize((void*)&header) + bi->biSizeImage;
		dib = OOPool_Alloc( OOGlobalPool, allocsize );

		OOPool_Free( OOGlobalPool, bi );
		}
	return dib;
}


//*************************************************************************
// This function will convert the bits per pixel and/or the compression
// format of the specified DIB. Note: If the conversion was unsuccessful,
// we return NULL. The original DIB is left alone. Don't use code like the
// following:
//	  hMyDIB = ChangeDIBFormat(hMyDIB, 8, BI_RLE4);
// The conversion will fail, but hMyDIB will now be NULL and the original
// DIB will now hang around in memory. We could have returned the old
// DIB, but we wanted to allow the programmer to check whether this
// conversion succeeded or failed.

HDIB ChangeDIBFormat(HDIB HDIB, UWORD bitsperpixel, ULONG dwCompression)
{
	HDC 				dc; 			// Handle to DC
	HBITMAP 			bitmap; 		// Handle to bitmap
	BITMAP				BMP;			// BITMAP data structure
	BITMAPINFOHEADER	header;				// Bitmap info header
	BITMAPINFOHEADER	*bi;			// Pointer to bitmap info
	HDIB				hNewDIB = NULL;	// Handle to new DIB
	HPALETTE			palette, oldpalette;	// Handle to palette, prev pal
	UWORD				DIBBPP, NewBPP;  // DIB bits per pixel, new bpp
	ULONG				DIBComp, NewComp;// DIB compression, new compression

	// Check for a valid DIB handle

	if (!HDIB)
		return NULL;

	// Get the old DIB's bits per pixel and compression format

	bi = (BITMAPINFOHEADER*)GlobalLock(HDIB);
	DIBBPP = ((BITMAPINFOHEADER*)bi)->biBitCount;
	DIBComp = ((BITMAPINFOHEADER*)bi)->biCompression;
	GlobalUnlock(HDIB);

	// Validate bitsperpixel and dwCompression
	// They must match correctly (i.e., BI_RLE4 and 4 BPP or
	// BI_RLE8 and 8BPP, etc.) or we return failure
	if (bitsperpixel == 0)
	{
		NewBPP = DIBBPP;
		if ((dwCompression == BI_RLE4 && NewBPP == 4) ||
				(dwCompression == BI_RLE8 && NewBPP == 8) ||
				(dwCompression == BI_RGB))
			NewComp = dwCompression;
		else
			return NULL;
	}
	else if (bitsperpixel == 1 && dwCompression == BI_RGB)
	{
		NewBPP = bitsperpixel;
		NewComp = BI_RGB;
	}
	else if (bitsperpixel == 4)
	{
		NewBPP = bitsperpixel;
		if (dwCompression == BI_RGB || dwCompression == BI_RLE4)
			NewComp = dwCompression;
		else
			return NULL;
	}
	else if (bitsperpixel == 8)
	{
		NewBPP = bitsperpixel;
		if (dwCompression == BI_RGB || dwCompression == BI_RLE8)
			NewComp = dwCompression;
		else
			return NULL;
	}
	else if (bitsperpixel == 24 && dwCompression == BI_RGB)
	{
		NewBPP = bitsperpixel;
		NewComp = BI_RGB;
	}
	else
		return NULL;

	// Save the old DIB's palette

	palette = OOGfx_GetDIBPalette(HDIB);
	if (!palette)
		return NULL;

	// Convert old DIB to a bitmap

	bitmap = OOGfx_DIB_ToBitmap(HDIB, palette);
	if (!bitmap)
	{
		DeleteObject(palette);
		return NULL;
	}

	// Get info about the bitmap
	GetObject(bitmap, sizeof(BITMAP), (void*)&BMP);

	// Fill in the BITMAPINFOHEADER appropriately

	header.biSize				= sizeof(BITMAPINFOHEADER);
	header.biWidth				= BMP.bmWidth;
	header.biHeight 			= BMP.bmHeight;
	header.biPlanes 			= 1;
	header.biBitCount			= NewBPP;
	header.biCompression		= NewComp;
	header.biSizeImage			= 0;
	header.biXPelsPerMeter		= 0;
	header.biYPelsPerMeter		= 0;
	header.biClrUsed			= 0;
	header.biClrImportant		= 0;

	// Go allocate room for the new DIB

	hNewDIB = AllocRoomForDIB(header, bitmap);
	if (!hNewDIB)
		return NULL;

	// Get a pointer to the new DIB

	bi = (BITMAPINFOHEADER*)GlobalLock(hNewDIB);

	// Get a DC and select/realize our palette in it

	dc  = GetDC(NULL);
	oldpalette = SelectPalette(dc, palette, FALSE);
	RealizePalette(dc);

	// Call GetDIBits and get the new DIB bits

	if (!GetDIBits(dc, bitmap, 0, (UINT) bi->biHeight,
			(void*)bi + (UWORD)bi->biSize + OOGfx_DIB_PaletteSize((void*)bi),
			(BITMAPINFO*)bi, DIB_RGB_COLORS))
	{
		GlobalUnlock(hNewDIB);
		GlobalFree(hNewDIB);
		hNewDIB = NULL;
	}

	// Clean up and return

	SelectPalette(dc, oldpalette, TRUE);
	RealizePalette(dc);
	ReleaseDC(NULL, dc);

	// Unlock the new DIB's memory block
	if (hNewDIB)
		GlobalUnlock(hNewDIB);

	DeleteObject(bitmap);
	DeleteObject(palette);

	return hNewDIB;
}


//************************************************************************
// This function will convert a bitmap to the specified bits per pixel
// and compression format. The bitmap and it's palette will remain
// after calling this function.

HDIB ChangeBitmapFormat(HBITMAP bitmap, UWORD bitsperpixel, ULONG dwCompression,
		HPALETTE palette)
{
	HDC 				dc; 		 // Screen DC
	HDIB				hNewDIB=NULL; // Handle to new DIB
	BITMAP				BMP;		 // BITMAP data structure
	BITMAPINFOHEADER	header;			 // Bitmap info. header
	BITMAPINFOHEADER	* bi;		 // Pointer to bitmap header
	HPALETTE			oldpalette=NULL; // Handle to palette
	UWORD				NewBPP; 	  // New bits per pixel
	ULONG				NewComp; 	 // New compression format

	// Check for a valid bitmap handle

	if (!bitmap)
		return NULL;

	// Validate bitsperpixel and dwCompression
	// They must match correctly (i.e., BI_RLE4 and 4 BPP or
	// BI_RLE8 and 8BPP, etc.) or we return failure
	
	if (bitsperpixel == 0)
	{
		NewComp = dwCompression;
		if (NewComp == BI_RLE4)
			NewBPP = 4;
		else if (NewComp == BI_RLE8)
			NewBPP = 8;
		else // Not enough info
			return NULL;
	}
	else if (bitsperpixel == 1 && dwCompression == BI_RGB)
	{
		NewBPP = bitsperpixel;
		NewComp = BI_RGB;
	}
	else if (bitsperpixel == 4)
	{
		NewBPP = bitsperpixel;
		if (dwCompression == BI_RGB || dwCompression == BI_RLE4)
			NewComp = dwCompression;
		else
			return NULL;
	}
	else if (bitsperpixel == 8)
	{
		NewBPP = bitsperpixel;
		if (dwCompression == BI_RGB || dwCompression == BI_RLE8)
			NewComp = dwCompression;
		else
			return NULL;
	}
	else if (bitsperpixel == 24 && dwCompression == BI_RGB)
	{
		NewBPP = bitsperpixel;
		NewComp = BI_RGB;
	}
	else
		return NULL;

	// Get info about the bitmap

	GetObject(bitmap, sizeof(BITMAP), (void*)&BMP);

	// Fill in the BITMAPINFOHEADER appropriately

	header.biSize				= sizeof(BITMAPINFOHEADER);
	header.biWidth				= BMP.bmWidth;
	header.biHeight 			= BMP.bmHeight;
	header.biPlanes 			= 1;
	header.biBitCount			= NewBPP;
	header.biCompression		= NewComp;
	header.biSizeImage			= 0;
	header.biXPelsPerMeter		= 0;
	header.biYPelsPerMeter		= 0;
	header.biClrUsed			= 0;
	header.biClrImportant		= 0;

	// Go allocate room for the new DIB

	hNewDIB = AllocRoomForDIB(header, bitmap);
	if (!hNewDIB)
		return NULL;

	// Get a pointer to the new DIB

	bi = (BITMAPINFOHEADER*)GlobalLock(hNewDIB);

	// If we have a palette, get a DC and select/realize it

	if (palette)
	{
		dc  = GetDC(NULL);
		oldpalette = SelectPalette(dc, palette, FALSE);
		RealizePalette(dc);
	}

	// Call GetDIBits and get the new DIB bits

	if (!GetDIBits(dc, bitmap, 0, (UINT) bi->biHeight, (void*)bi +
			(UWORD)bi->biSize + OOGfx_DIB_PaletteSize((void*)bi), (BITMAPINFO*)bi,
			DIB_RGB_COLORS))
	{
		GlobalUnlock(hNewDIB);
		GlobalFree(hNewDIB);
		hNewDIB = NULL;
	}

	// Clean up and return

	if (oldpalette)
	{
		SelectPalette(dc, oldpalette, TRUE);
		RealizePalette(dc);
		ReleaseDC(NULL, dc);
	}

	// Unlock the new DIB's memory block

	if (hNewDIB)
		GlobalUnlock(hNewDIB);

	return hNewDIB;
}
*/
/***************************************************************************
 *
 *		
 *
 ***************************************************************************/
/*
HDIB CopyWindowToDIB(HWND hWnd, UWORD fPrintArea)
{
   HDIB 	hDIB = NULL;  // handle to DIB

   // check for a valid window handle

	if (!hWnd)
		return NULL;

	switch (fPrintArea)
	{
		case PW_WINDOW: // copy entire window
		{
			RECT	rectWnd;

			// get the window rectangle

			GetWindowRect(hWnd, &rectWnd);

			// get the DIB of the window by calling
			// CopyScreenToDIB and passing it the window rect
			
			hDIB = CopyScreenToDIB(&rectWnd);
			break;
		}
	  
		case PW_CLIENT: // copy client area
		{
			RECT	rectClient;
			POINT	pt1, pt2;

			// get the client area dimensions

			GetClientRect(hWnd, &rectClient);

			// convert client coords to screen coords

			pt1.x = rectClient.left;
			pt1.y = rectClient.top;
			pt2.x = rectClient.right;
			pt2.y = rectClient.bottom;
			ClientToScreen(hWnd, &pt1);
			ClientToScreen(hWnd, &pt2);
			rectClient.left = pt1.x;
			rectClient.top = pt1.y;
			rectClient.right = pt2.x;
			rectClient.bottom = pt2.y;

			// get the DIB of the client area by calling
			// CopyScreenToDIB and passing it the client rect

			hDIB = CopyScreenToDIB(&rectClient);
			break;
		}
	  
		default:	// invalid print area
			return NULL;
	}

   // return the handle to the DIB

   return hDIB;
}

HDIB CopyScreenToDIB(LPRECT lpRect)
{
	HBITMAP 	bitmap;		// handle to device-dependent bitmap
	HPALETTE	hPalette;		// handle to palette
	HDIB		hDIB = NULL;	// handle to DIB

	// get the device-dependent bitmap in lpRect by calling
	//	CopyScreenToBitmap and passing it the rectangle to grab

	bitmap = CopyScreenToBitmap(lpRect);

	// check for a valid bitmap handle

	if (!bitmap)
	  return NULL;

	// get the current palette

	hPalette = GetSystemPalette();

	// convert the bitmap to a DIB

	hDIB = OOGfx_BMap_ToDIB(bitmap, hPalette);

	// clean up 

	DeleteObject(hPalette);
	DeleteObject(bitmap);

	// return handle to the packed-DIB
	return hDIB;
}


HBITMAP CopyWindowToBitmap(HWND hWnd, UWORD fPrintArea)
{
	HBITMAP 	bitmap = NULL;  // handle to device-dependent bitmap

	// check for a valid window handle

	if (!hWnd)
		return NULL;

	switch (fPrintArea)
	{
		case PW_WINDOW: // copy entire window
		{
			RECT	rectWnd;

			// get the window rectangle

			GetWindowRect(hWnd, &rectWnd);

			// get the bitmap of that window by calling
			// CopyScreenToBitmap and passing it the window rect
	   
			bitmap = CopyScreenToBitmap(&rectWnd);
			break;
		}
		

		case PW_CLIENT: // copy client area
		{
			RECT rectClient;
			POINT pt1, pt2;

			// get client dimensions

			GetClientRect(hWnd, &rectClient);

			// convert client coords to screen coords

			pt1.x = rectClient.left;
			pt1.y = rectClient.top;
			pt2.x = rectClient.right;
			pt2.y = rectClient.bottom;
			ClientToScreen(hWnd, &pt1);
			ClientToScreen(hWnd, &pt2);
			rectClient.left = pt1.x;
			rectClient.top = pt1.y;
			rectClient.right = pt2.x;
			rectClient.bottom = pt2.y;

			// get the bitmap of the client area by calling
			// CopyScreenToBitmap and passing it the client rect
			
			bitmap = CopyScreenToBitmap(&rectClient);
			break;
		}
	
		default:	// invalid print area
			return NULL;
	}

	// return handle to the bitmap

   return bitmap;
}


HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
	HDC 		hScrDC, hMemDC; 		// screen DC and memory DC
	HBITMAP 	bitmap, hOldBitmap;	// handles to deice-dependent bitmaps
	int 		nX, nY, nX2, nY2;		// coordinates of rectangle to grab
	int 		nWidth, nHeight;		// DIB width and height
	int 		xScrn, yScrn;			// screen resolution

	// check for an empty rectangle

	if (IsRectEmpty(lpRect))
	  return NULL;

	// create a DC for the screen and create
	// a memory DC compatible to screen DC
	
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);

	// get points of rectangle to grab

	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	// get screen resolution

	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);

	//make sure bitmap rectangle is visible

	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > xScrn)
		nX2 = xScrn;
	if (nY2 > yScrn)
		nY2 = yScrn;

	nWidth = nX2 - nX;
	nHeight = nY2 - nY;

	// create a bitmap compatible with the screen DC
	bitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

	// select new bitmap into memory DC
	hOldBitmap = SelectObject(hMemDC, bitmap);

	// bitblt screen DC to memory DC
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

	// select old bitmap back into memory DC and get handle to
	// bitmap of the screen
	
	bitmap = SelectObject(hMemDC, hOldBitmap);

	// clean up

	DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	// return handle to the bitmap

	return bitmap;
}


//************************************************************************
// Description:
//	 Painting routine for a DIB.  Calls StretchDIBits() or
//	 SetDIBitsToDevice() to paint the DIB.	The DIB is
//	 output to the specified DC, at the coordinates given
//	 in lpDCRect.  The area of the DIB to be output is
//	 given by lpDIBRect.
//
// NOTE: This function always selects the palette as background. Before
// calling this function, be sure your palette is selected to desired
// priority (foreground or background).

BOOL PaintDIB(HDC dc, LPRECT lpDCRect, HDIB hDIB, LPRECT lpDIBRect,
		HPALETTE palette)
{
	void*		 dib;			 // Pointer to BITMAPINFOHEADER
	void*		 plane; 		 // Pointer to DIB bits
	BOOL		 bSuccess=FALSE;	 // Success/fail flag
	HPALETTE	 oldpalette=NULL;		 // Previous palette

	// Check for valid DIB handle

	if (!hDIB)
		return FALSE;

	// Lock down the DIB, and get a pointer to the beginning of the bit
	// buffer
	
	dib  = GlobalLock(hDIB);
	plane = OOGFX_DIB_PLANE(dib);

	// Select and realize our palette as background

	if (palette)
	{
		oldpalette = SelectPalette(dc, palette, TRUE);
		RealizePalette(dc);
	}

	// Make sure to use the stretching mode best for color pictures

	SetStretchBltMode(dc, COLORONCOLOR);

	// Determine whether to call StretchDIBits() or SetDIBitsToDevice()

	if ((OOGFX_RECTWIDTH(lpDCRect)  == OOGFX_RECTWIDTH(lpDIBRect)) &&
			(OOGFX_RECTHEIGHT(lpDCRect) == OOGFX_RECTHEIGHT(lpDIBRect)))
	{
		bSuccess = SetDIBitsToDevice(dc, lpDCRect->left, lpDCRect->top,
				OOGFX_RECTWIDTH(lpDCRect), OOGFX_RECTHEIGHT(lpDCRect), lpDIBRect->left,
				(int)OOGFX_DIB_HEIGHT(dib) - lpDIBRect->top -
				OOGFX_RECTHEIGHT(lpDIBRect), 0, OOGFX_DIB_HEIGHT(dib), plane,
				(BITMAPINFO*)dib, DIB_RGB_COLORS);
	}
	else 
		bSuccess = StretchDIBits(dc, lpDCRect->left, lpDCRect->top,
				OOGFX_RECTWIDTH(lpDCRect), OOGFX_RECTHEIGHT(lpDCRect), lpDIBRect->left,
				lpDIBRect->top, OOGFX_RECTWIDTH(lpDIBRect), OOGFX_RECTHEIGHT(lpDIBRect),
				plane, (BITMAPINFO*)dib, DIB_RGB_COLORS, SRCCOPY);

	// Unlock the memory block

	GlobalUnlock(hDIB);

	// Reselect old palette

	if (oldpalette)
	 SelectPalette(dc, oldpalette, FALSE);

	// Return with success/fail flag
	return bSuccess;
}


//************************************************************************
// Painting routine for a DDB.	Calls BitBlt() or
// StretchBlt() to paint the DDB.  The DDB is
// output to the specified DC, at the coordinates given
// in lpDCRect.  The area of the DDB to be output is
// given by lpDDBRect.	The specified palette is used.
//
// NOTE: This function always selects the palette as background. Before
// calling this function, be sure your palette is selected to desired
// priority (foreground or background).

BOOL PaintBitmap( HDC dc, LPRECT lpDCRect, HBITMAP hDDB, LPRECT lpDDBRect, HPALETTE palette )
{
	HDC 		hMemDC; 		   // Handle to memory DC
	HBITMAP 	hOldBitmap; 	   // Handle to previous bitmap
	HPALETTE	hOldPal1 = NULL;   // Handle to previous palette
	HPALETTE	hOldPal2 = NULL;   // Handle to previous palette
	BOOL		bSuccess = FALSE;  // Success/fail flag

	// Create a memory DC

	hMemDC = CreateCompatibleDC (dc);

	// If this failed, return FALSE

	if (!hMemDC)
		return FALSE;

	// If we have a palette, select and realize it

	if (palette)
	{
		hOldPal1 = SelectPalette(hMemDC, palette, TRUE);
		hOldPal2 = SelectPalette(dc, palette, TRUE);
		RealizePalette(dc);
	}

	// Select bitmap into the memory DC

	hOldBitmap = SelectObject (hMemDC, hDDB);

	// Make sure to use the stretching mode best for color pictures

	SetStretchBltMode (dc, COLORONCOLOR);

	// Determine whether to call StretchBlt() or BitBlt()
	if ((OOGFX_RECTWIDTH(lpDCRect)  == OOGFX_RECTWIDTH(lpDDBRect)) &&
			(OOGFX_RECTHEIGHT(lpDCRect) == OOGFX_RECTHEIGHT(lpDDBRect)))
		bSuccess = BitBlt(dc, lpDCRect->left, lpDCRect->top,
				lpDCRect->right - lpDCRect->left,
				lpDCRect->bottom - lpDCRect->top, hMemDC, lpDDBRect->left,
				lpDDBRect->top, SRCCOPY);
	else
		bSuccess = StretchBlt(dc, lpDCRect->left,	lpDCRect->top, 
				lpDCRect->right - lpDCRect->left,
				lpDCRect->bottom - lpDCRect->top, hMemDC, lpDDBRect->left, 
				lpDDBRect->top,  lpDDBRect->right - lpDDBRect->left,
				lpDDBRect->bottom - lpDDBRect->top, SRCCOPY);

	// Clean up

	SelectObject(hMemDC, hOldBitmap);

	if (hOldPal1)
		SelectPalette (hMemDC, hOldPal1, FALSE);

	if (hOldPal2)
		SelectPalette (dc, hOldPal2, FALSE);

	DeleteDC (hMemDC);

	// Return with success/fail flag

	return bSuccess;
}
*/