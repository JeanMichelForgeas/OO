/*****************************************************************
 *
 *		 Project		handling Windows DIB
 *		 Function		handling Windows DIB
 *
 *		 Created		Jean-Michel Forgeas
 *
 ****************************************************************/


//******* Includes ***********************************************

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <tchar.h>

#include "OODIB.h"


//******* Imported ***********************************************


//******* Exported ***********************************************


//******* Statics ************************************************


// ***************************************************************
// *
// *	General utilities
// *
// ***************************************************************

ULONG OOGfx_GetPaletteEntriesCount( HDC hdc )
{
	ULONG numcolors = (1 << (GetDeviceCaps( hdc, BITSPIXEL ) * GetDeviceCaps( hdc, PLANES )));
	assert( numcolors );
	return numcolors;
}

HPALETTE OOGfx_GetSystemPalette( void )
{
	HDC hdc;
	HPALETTE hpalette = NULL;
	LOGPALETTE *logpal;
	ULONG numcolors;

	if ((hdc = GetDC( NULL )) != NULL)
		{
		numcolors = OOGfx_GetPaletteEntriesCount( hdc );
		if ((logpal = (LOGPALETTE*) malloc( sizeof(LOGPALETTE) + numcolors * sizeof(PALETTEENTRY) )) != NULL)
			{
			logpal->palVersion = PALVERSION;
			logpal->palNumEntries = (UWORD)numcolors;
			GetSystemPaletteEntries( hdc, 0, numcolors, (PALETTEENTRY*)logpal->palPalEntry );
			hpalette = CreatePalette( logpal );
			free( logpal );
			}
		ReleaseDC( NULL, hdc );
		}

    if (hpalette == NULL) 
		hpalette = (HPALETTE) GetStockObject( DEFAULT_PALETTE );
	
	return hpalette;
}


// ***************************************************************
// *
// *	DIB - Get functions
// *
// ***************************************************************

ULONG OOGfx_DIB_GetWidth( DIB *dib )
{
	return ISWIN30DIB(dib) ? ((BITMAPINFOHEADER*)dib)->biWidth : (ULONG)((BITMAPCOREHEADER*)dib)->bcWidth;
}

ULONG OOGfx_DIB_GetHeight( DIB *dib )
{
	SLONG height = ISWIN30DIB(dib) ? ((BITMAPINFOHEADER*)dib)->biHeight : (ULONG)((BITMAPCOREHEADER*)dib)->bcHeight;
	return ABS(height);
}

ULONG OOGfx_DIB_GetBitsPerPixel( DIB *dib )
{
	return ISWIN30DIB(dib) ? ((BITMAPINFOHEADER*)dib)->biBitCount : (ULONG)((BITMAPCOREHEADER*)dib)->bcBitCount;
}

ULONG OOGfx_DIB_GetBytesPerRow( DIB *dib )
{
	return BYTESPERROW(OOGfx_DIB_GetWidth(dib) * OOGfx_DIB_GetBitsPerPixel(dib));
}

ULONG OOGfx_DIB_GetImageSize( DIB *dib )
{
	return dib->bmiHeader.biSizeImage ? dib->bmiHeader.biSizeImage : OOGfx_DIB_GetBytesPerRow(dib) * OOGfx_DIB_GetHeight(dib);
}

ULONG OOGfx_DIB_GetPlanesCount( DIB *dib )
{
	return ISWIN30DIB(dib) ? ((BITMAPINFOHEADER*)dib)->biPlanes : (ULONG)((BITMAPCOREHEADER*)dib)->bcPlanes;
}

UBYTE *OOGfx_DIB_GetPlanes( DIB *dib )
{
	return (UBYTE*) ((ULONG)dib + dib->bmiHeader.biSize + OOGfx_DIB_GetRGBSize( dib ));
}

ULONG OOGfx_DIB_GetDPIX( DIB *dib, ULONG defaultvalue )
{
	ULONG dpix = ISWIN30DIB(dib) ? (dib->bmiHeader.biXPelsPerMeter * 100) / 3937 : 0;	// 39,37 = pouces par mètre

	return dpix != 0 ? dpix : defaultvalue;
}

ULONG OOGfx_DIB_GetDPIY( DIB *dib, ULONG defaultvalue )
{
	ULONG dpiy = ISWIN30DIB(dib) ? (dib->bmiHeader.biYPelsPerMeter * 100) / 3937 : 0;	// 39,37 = pouces par mètre

	return dpiy != 0 ? dpiy : defaultvalue;
}

ULONG OOGfx_DIB_GetCompression( DIB *dib )
{
	return ISWIN30DIB(dib) ? dib->bmiHeader.biCompression : BI_RGB;
}

ULONG OOGfx_DIB_GetColorsCount( DIB *dib )
{
	ULONG numcolors;

	if (ISWIN30DIB(dib))
		if (((BITMAPINFOHEADER*)dib)->biClrUsed != 0 && OOGfx_DIB_GetBitsPerPixel( dib ) != 24) 
			return ((BITMAPINFOHEADER*)dib)->biClrUsed;

	numcolors = 1 << OOGfx_DIB_GetBitsPerPixel( dib );
	assert( numcolors );
	return numcolors;
}


static ULONG get_rgb_count( ULONG bitsperpixel, ULONG compression )
{
	ULONG rgbcount;

	switch (bitsperpixel)
		{
		case 1:
		case 4:
		case 8:
			rgbcount = 1 << bitsperpixel;
			break;

		case 16:
		case 32:
			rgbcount = (compression == BI_BITFIELDS) ? 3 : 0;
			break;

		case 24:
		default:
			rgbcount = 0;
			break;
		}

	return rgbcount;
}

ULONG OOGfx_DIB_GetRGBCount( DIB *dib )
{
	return get_rgb_count( OOGfx_DIB_GetBitsPerPixel( dib ), OOGfx_DIB_GetCompression( dib) );
}


static ULONG get_rgb_size( ULONG bitsperpixel, ULONG compression, BOOL isv3dib )
{
	ULONG rgbsize;

	switch (bitsperpixel)
		{
		case 1:
		case 4:
		case 8:
			rgbsize = get_rgb_count( bitsperpixel, compression ) * ((isv3dib == TRUE) ? sizeof(RGBQUAD) : sizeof(RGBTRIPLE));
			break;

		case 16:
		case 32:
			rgbsize = (compression == BI_BITFIELDS) ? 3 * sizeof(ULONG) : 0;
			break;

		case 24:
		default:
			rgbsize = 0;
			break;
		}

	return rgbsize;
}

ULONG OOGfx_DIB_GetRGBSize( DIB *dib )
{
	return get_rgb_size( OOGfx_DIB_GetBitsPerPixel( dib ), OOGfx_DIB_GetCompression( dib), ISWIN30DIB(dib) );
}


HPALETTE OOGfx_DIB_GetPalette( DIB *dib )
{
	BITMAPINFO		*bmi = (BITMAPINFO*)dib;		// Win3.0
	BITMAPCOREINFO	*bmc = (BITMAPCOREINFO*)dib;	// OS/2
	LOGPALETTE		*logpal;
	HPALETTE		hpalette = NULL;
	LONG 			i, rgbcount;

	if (dib != NULL && (rgbcount = OOGfx_DIB_GetRGBCount( dib )))
		{
		if ((logpal = (LOGPALETTE*) malloc( sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * rgbcount) )) != NULL)
			{
			logpal->palVersion = PALVERSION;
			logpal->palNumEntries = (UWORD)rgbcount;
			if (ISWIN30DIB(dib))
				{
				for (i = 0; i < rgbcount; i++)
					{
					logpal->palPalEntry[i].peRed = bmi->bmiColors[i].rgbRed;
					logpal->palPalEntry[i].peGreen = bmi->bmiColors[i].rgbGreen;
					logpal->palPalEntry[i].peBlue = bmi->bmiColors[i].rgbBlue;
					logpal->palPalEntry[i].peFlags = 0;
					}
				}
			else{
				for (i = 0; i < rgbcount; i++)
					{
					logpal->palPalEntry[i].peRed = bmc->bmciColors[i].rgbtRed;
					logpal->palPalEntry[i].peGreen = bmc->bmciColors[i].rgbtGreen;
					logpal->palPalEntry[i].peBlue = bmc->bmciColors[i].rgbtBlue;
					logpal->palPalEntry[i].peFlags = 0;
					}
				}
			hpalette = CreatePalette( logpal );
			free( logpal );
			}
		}

	return hpalette;
}


// ***************************************************************
// *
// *	DIB - Draw functions
// *
// ***************************************************************

#if defined _DEBUG			
static draw_rect( HDC hdc, LPRECT r )
{
	POINT pt;
	GetCurrentPositionEx( hdc, &pt ); // On lit l'abscisse de départ pour y revenir

	MoveToEx( hdc, r->left, r->top, NULL );
	LineTo( hdc, r->right, r->top );
	LineTo( hdc, r->right, r->bottom );
	LineTo( hdc, r->left, r->bottom );
	LineTo( hdc, r->left, r->top );

	SetTextAlign( hdc, TA_NOUPDATECP );
	MoveToEx( hdc, pt.x, pt.y, NULL );
	SetTextAlign( hdc, TA_UPDATECP );
}
#endif

BOOL OOGfx_DIB_Draw( HDC hdc, LPRECT maxrect, ULONG flags, HDIB hdib, LPRECT srcrect, HPALETTE hpalette )
{
	DIB	*dib;
	BOOL success = FALSE;

	assert( hdib != NULL );
    if (hdib != NULL && (dib = GlobalLock( hdib )) != NULL)
		{
		HPALETTE	oldpalette = NULL;
		HPALETTE	dibpalette = NULL;
		RECT		destrect;
		SLONG		dpix, dpiy, srcdpix, srcdpiy;

		//------ Crée une palette si besoin

		if (hpalette == NULL)
			{
			if ((dibpalette = OOGfx_DIB_GetPalette( dib )) == NULL)
				dibpalette = OOGfx_GetSystemPalette();
			hpalette = dibpalette;
			}
		if (hpalette != NULL)
			{
			oldpalette = SelectPalette( hdc, hpalette, TRUE );
			RealizePalette( hdc );
			}

		//------ Retaille le rectangle de destination si besoin

		// On zoome l'image source en fonction de la différence entre la résolution
		//	de numérisation de la source, et celle de l'affichage
		dpix = GetDeviceCaps( hdc, LOGPIXELSX );
		dpiy = GetDeviceCaps( hdc, LOGPIXELSY );
		srcdpix = OOGfx_DIB_GetDPIX( dib, 200);	// 200 = default value
		srcdpiy = OOGfx_DIB_GetDPIY( dib, 200);	// 200 = default value
		SetRect( &destrect, 0, 0,	((OOGfx_DIB_GetWidth( dib ) * dpix) / srcdpix), 
									((OOGfx_DIB_GetHeight( dib ) * dpiy) / srcdpiy) );

		// On arrange la destination dans le cadre donné
		if (maxrect != NULL)
			{
			SIZE srcsize  = { RECTWIDTH(srcrect), RECTHEIGHT(srcrect) };
			SIZE destsize = { RECTWIDTH(&destrect), RECTHEIGHT(&destrect) };
			SIZE maxsize  = { RECTWIDTH(maxrect), RECTHEIGHT(maxrect) };

			// On essaye de rentrer dans maxrect, d'abord droite puis gauche, et bas puis haut
			if (destrect.right > maxrect->right)
				OffsetRect( &destrect, maxrect->right - destrect.right, 0 );
			if (destrect.bottom > maxrect->bottom)
				OffsetRect( &destrect, 0, maxrect->bottom - destrect.bottom );
			if (destrect.left < maxrect->left)
				OffsetRect( &destrect, maxrect->left - destrect.left, 0 );
			if (destrect.top < maxrect->top)
				OffsetRect( &destrect, 0, maxrect->top - destrect.top );

			// Si l'image est plus petite que le cadre max, on agrandit (si demandé)
			if ((flags & OOF_DIB_DRAWEXPAND) && (destsize.cy < maxsize.cy && destsize.cx < maxsize.cx))
				{
				destsize.cx = maxsize.cx + 1; // Agrandi n'importe comment
				destsize.cy = maxsize.cy + 1; // puis
				flags |= OOF_DIB_DRAWSHRINK;  // Ramène à l'intérieur en fonction du ratio source
				}

			// On réduit le tout en gardant le ratio source si un côté dépasse la taille maximum
			if (flags & OOF_DIB_DRAWSHRINK)
				{
				if (destsize.cy > maxsize.cy)
					{
					destsize.cy = maxsize.cy;
					destsize.cx = (destsize.cy * srcsize.cx) / srcsize.cy;
					}
				if (destsize.cx > maxsize.cx)
					{
					destsize.cx = maxsize.cx;
					destsize.cy = (destsize.cx * srcsize.cy) / srcsize.cx;
					}

				// Si SHRINK ou EXPAND
				destrect.bottom = destrect.top + destsize.cy;
				destrect.right = destrect.left + destsize.cx;
				}

			// Si l'image est plus petite que le max on centre
			if (flags & OOF_DIB_DRAWCENTER)
				{
				if (destsize.cx < maxsize.cx)
					OffsetRect( &destrect, (maxsize.cx - destsize.cx)/2, 0 );
				if (destsize.cy < maxsize.cy)
					OffsetRect( &destrect, 0, (maxsize.cy - destsize.cy)/2 );
				}
			}

		//------ Copie le rectangle

		SetStretchBltMode( hdc, COLORONCOLOR );

		if (StretchDIBits( hdc, destrect.left, destrect.top, RECTWIDTH(&destrect), RECTHEIGHT(&destrect), 
						srcrect->left, srcrect->top, RECTWIDTH(srcrect), RECTHEIGHT(srcrect),
						OOGfx_DIB_GetPlanes(dib), (BITMAPINFO*)dib, DIB_RGB_COLORS, SRCCOPY ) != GDI_ERROR)
			success = TRUE;

#if defined _DEBUG
		draw_rect( hdc, maxrect );
		draw_rect( hdc, &destrect );
#endif

		//------ Libère la palette

		if (hpalette != NULL)
			SelectPalette( hdc, oldpalette, FALSE );
		if (dibpalette != NULL)
			DeleteObject( dibpalette );

		GlobalUnlock( hdib );
		}

	return success;
}


// ***************************************************************
// *
// *	DIB - Conversion functions
// *
// ***************************************************************

HBITMAP OOGfx_DIB_ToBitmap( HDIB hdib, HPALETTE hpalette )
{
    DIB *		dib;
    HPALETTE	oldhpalette = NULL;
    HDC			hdc;
    HBITMAP		hbitmap = NULL;

	assert( hdib != NULL );
    if (hdib != NULL && (dib = GlobalLock( hdib )) != NULL)
		{
		if ((hdc = GetDC(NULL)) != NULL)
			{
			if (hpalette != NULL)
				{
				oldhpalette = SelectPalette( hdc, hpalette, FALSE );
				RealizePalette( hdc );
				}

			hbitmap = CreateDIBitmap( hdc, (BITMAPINFOHEADER*)dib, CBM_INIT, OOGfx_DIB_GetPlanes( dib ), dib, DIB_RGB_COLORS );

			if (hpalette != NULL)
				SelectPalette( hdc, oldhpalette, FALSE );

			ReleaseDC( NULL, hdc );
			}
		GlobalUnlock( hdib );
		}

    return hbitmap;
}

HDIB OOGfx_DIB_FromDC( HDC dc )
{
	return OOGfx_DIB_FromBitmap( (HBITMAP)GetCurrentObject( dc, OBJ_BITMAP ), (HPALETTE)GetCurrentObject( dc, OBJ_PAL ) );
}

HDIB OOGfx_DIB_FromBitmap( HBITMAP hbitmap, HPALETTE hpalette )
{
	BITMAPINFO256	bi;
	HPALETTE		dibpalette = NULL;
	BITMAP			bitmap;
	HDC				hdc;
	DIB				*dib = NULL;
	HDIB			hdib;
	ULONG			allocsize, rgbsize;

	assert( hbitmap != NULL );
	if (hbitmap != NULL)
		{
		if (hpalette == NULL)
			{
			if ((dibpalette = OOGfx_DIB_GetPalette( dib )) == NULL)
				dibpalette = OOGfx_GetSystemPalette();
			hpalette = dibpalette;
			}

		if (GetObject( hbitmap, sizeof(BITMAP), (void*)&bitmap ))
			{
			bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
			bi.bmiHeader.biWidth		= bitmap.bmWidth; 
			bi.bmiHeader.biHeight		= bitmap.bmHeight;
			bi.bmiHeader.biPlanes		= 1;	
			bi.bmiHeader.biBitCount		= bitmap.bmBitsPixel * bitmap.bmPlanes;
			bi.bmiHeader.biCompression	= BI_RGB;	  
			bi.bmiHeader.biSizeImage	= 0;
			bi.bmiHeader.biXPelsPerMeter= 0;
			bi.bmiHeader.biYPelsPerMeter= 0;
			bi.bmiHeader.biClrUsed		= 0;
			bi.bmiHeader.biClrImportant	= 0;

			if ((hdc = GetDC( NULL )) != NULL)
				{
				HPALETTE oldhpalette = SelectPalette( hdc, hpalette, FALSE );
				RealizePalette( hdc );

				// call GetDIBits with a NULL lpBits param, so get biSizeImage, color table, ...
				GetDIBits( hdc, hbitmap, 0, (UINT)bi.bmiHeader.biHeight, NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS );
				// if the driver did not fill in the biSizeImage field, make one up 
				if (bi.bmiHeader.biSizeImage == 0) 
					bi.bmiHeader.biSizeImage = OOGfx_DIB_GetImageSize( (DIB*)&bi );

				rgbsize = OOGfx_DIB_GetRGBSize( (DIB*)&bi );
				allocsize = bi.bmiHeader.biSize + rgbsize + bi.bmiHeader.biSizeImage;
				if ((hdib = GlobalAlloc( GHND, allocsize )) != NULL)
					{
					dib = (DIB*) GlobalLock( hdib );

					memcpy( dib, &bi, bi.bmiHeader.biSize + rgbsize );

					// call GetDIBits with a NON-NULL lpBits param, and actualy get the bits this time
					if (GetDIBits( hdc, hbitmap, 0, dib->bmiHeader.biHeight, OOGfx_DIB_GetPlanes( dib ), (BITMAPINFO*)dib, DIB_RGB_COLORS ) == 0)
						dib = NULL;

					GlobalUnlock( hdib );

					// Si GetDIBits() mal passé
					if (dib == NULL)
						{ GlobalFree( hdib ); hdib = NULL; }
					}

				SelectPalette( hdc, oldhpalette, FALSE );
				ReleaseDC( NULL, hdc );
				}
			}

		if (dibpalette != NULL)
			DeleteObject( dibpalette );
		}
	return hdib;
}


// ***************************************************************
// *
// *	DIB - Alloc/Free functions
// *
// ***************************************************************

HDIB OOGfx_DIB_Alloc( ULONG width, ULONG height, UWORD bitsperpixel, HPALETTE hpalette )
{
	ULONG	rgbsize = get_rgb_size( bitsperpixel, BI_RGB, TRUE );
	ULONG	imgsize = BYTESPERROW(width*bitsperpixel) * height;
	ULONG	totsize = sizeof(BITMAPINFOHEADER) + rgbsize + imgsize;
	HDIB	hdib;

	assert( width != 0 && height != 0 && bitsperpixel != 0 );
	if ((hdib = GlobalAlloc( GHND, totsize )) != NULL)
		{
		DIB *dib = (DIB*) GlobalLock( hdib );

		dib->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		dib->bmiHeader.biWidth			= width; 
		dib->bmiHeader.biHeight			= height;
		dib->bmiHeader.biPlanes			= 1;	
		dib->bmiHeader.biBitCount		= bitsperpixel;
		dib->bmiHeader.biCompression	= BI_RGB;	  
		dib->bmiHeader.biSizeImage		= imgsize;
		dib->bmiHeader.biXPelsPerMeter	= 0;
		dib->bmiHeader.biYPelsPerMeter	= 0;
		dib->bmiHeader.biClrUsed		= 0;

		if (hpalette != NULL)
			{
			HDC hdc;
			if ((hdc = GetDC( NULL )) != NULL)
				{
				HPALETTE oldhpalette = SelectPalette( hdc, hpalette, FALSE );
				RealizePalette( hdc );

				GetDIBColorTable( hdc, 0, OOGfx_DIB_GetRGBCount( dib ), dib->bmiColors );

				SelectPalette( hdc, oldhpalette, FALSE );
				ReleaseDC( NULL, hdc );
				}
			}

		GlobalUnlock( hdib );
		}

	return hdib;
}

void OOGfx_DIB_Free( HDIB hdib )
{
	if (hdib != NULL)
		GlobalFree( hdib );
}

HDIB OOGfx_DIB_Clone( HDIB hdib )
{
	HGLOBAL hcopy = NULL;
	DIB *	dib;

	assert( hdib != NULL );
    if (hdib != NULL && (dib = GlobalLock( hdib )) != NULL)
		{
		ULONG dibsize = GlobalSize( hdib );
		if ((hcopy = GlobalAlloc( GHND, dibsize )) != NULL)
			{
			memcpy( GlobalLock( hcopy ), dib, dibsize );
			GlobalUnlock( hcopy );
			}
		GlobalUnlock( hdib );
		}

	return hcopy;
}


// ***************************************************************
// *
// *	DIB - Create/Load/Save functions
// *
// ***************************************************************

static ULONG get_value( FILE *fd, TIFF_IFDEntry *ent, ULONG *pvalue2 )
{
	ULONG value;

	switch (ent->Type)
		{
		case TIFF_BYTE:		value = (ULONG) *((UBYTE*)&ent->Value); break;
		case TIFF_SHORT:	value = (ULONG) *((UWORD*)&ent->Value); break;
		case TIFF_LONG:		value = (ULONG) *((ULONG*)&ent->Value); break;
		case TIFF_RATIONAL:	// On prend ici seulement le premier !!!
			{
			SLONG pos;
			ULONG nom, denom;
			value = (ULONG) *((ULONG*)&ent->Value); // Offset to 2 values
			if ((pos = ftell( fd )) == -1) return 0;
			if (fseek( fd, value, SEEK_SET ) != 0) return 0;
			if (fread( &nom, sizeof(ULONG), 1, fd ) == 0) return 0;
			if (fread( &denom, sizeof(ULONG), 1, fd ) == 0) return 0;
			if (fseek( fd, pos, SEEK_SET ) != 0) return 0;
			value = nom;
			*pvalue2 = denom;
			}
			break; 
		default: value = 0; break;
		}
	return value;
}

BOOL OOGfx_DIB_LoadBMI256( LPCTSTR filename, BITMAPINFO256 *bmi256 )
{
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bih;
	RGBQUAD colors[256];
    FILE *fd;
	BOOL rc = FALSE;
	ULONG palsize;

	long pathlen = _tcslen( filename );
	if (pathlen < 4 || ((_tcsicmp( &filename[pathlen-4], _T(".bmp") ) != 0) && (_tcsicmp( &filename[pathlen-4], _T(".tif") ) != 0)))
		return FALSE;

    if ((fd = fopen( filename, "rb" )) == NULL)
		return FALSE;

	if (_tcsicmp( &filename[pathlen-4], _T(".bmp") ) == 0)
		{
		//======================================== BMP
		// Lecture de l'entête
		if (fread( &hdr, sizeof(BITMAPFILEHEADER), 1, fd ) == 0) goto END_ERROR;
		if (hdr.bfType != DIB_HEADER_MARKER) goto END_ERROR;
		if (fread( &bih, sizeof(BITMAPINFOHEADER), 1, fd ) == 0) goto END_ERROR;

		// Mise à jour de l'entête
		if (ISWIN30DIB((DIB*)&bih))
			{
			bmi256->bmiHeader = bih;
			}
		else{
			bmi256->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
			bmi256->bmiHeader.biWidth			= ((BITMAPCOREHEADER*)&bih)->bcWidth;
			bmi256->bmiHeader.biHeight			= ((BITMAPCOREHEADER*)&bih)->bcHeight;
			bmi256->bmiHeader.biPlanes			= ((BITMAPCOREHEADER*)&bih)->bcPlanes;
			bmi256->bmiHeader.biBitCount		= ((BITMAPCOREHEADER*)&bih)->bcBitCount;
			bmi256->bmiHeader.biCompression		= BI_RGB;
			bmi256->bmiHeader.biSizeImage		= OOGfx_DIB_GetImageSize( (DIB*)&bih );
			bmi256->bmiHeader.biXPelsPerMeter	= 0;
			bmi256->bmiHeader.biYPelsPerMeter	= 0;
			bmi256->bmiHeader.biClrUsed			= 0;
			bmi256->bmiHeader.biClrImportant	= 0;
			}
		assert( (OOGfx_DIB_GetCompression( (DIB*)bmi256 ) == BI_RGB || OOGfx_DIB_GetCompression( (DIB*)bmi256 ) == BI_BITFIELDS) && OOGfx_DIB_GetImageSize( (DIB*)bmi256 ) == hdr.bfSize - hdr.bfOffBits );
		assert( OOGfx_DIB_GetImageSize( (DIB*)bmi256 ) == OOGfx_DIB_GetBytesPerRow( (DIB*)bmi256 ) * OOGfx_DIB_GetHeight( (DIB*)bmi256 ) );

		assert( bmi256->bmiHeader.biClrUsed <= 256 );
		if (bmi256->bmiHeader.biClrUsed > 256)
			goto END_ERROR;

		// Taille de la palette	(on lit depuis le fichier, donc bih)
		if ((palsize = OOGfx_DIB_GetRGBSize( (DIB*)&bih )) == 0) goto END_ERROR;
		assert( palsize <= sizeof(colors) );

		// Lecture de la palette
		if (fread( &colors, palsize, 1, fd ) == 0) goto END_ERROR;
		if (ISWIN30DIB((DIB*)&bih))
			{
			memcpy( bmi256->bmiColors, colors, palsize );
			}
		else{
			ULONG i, numcolors = OOGfx_DIB_GetRGBCount( (DIB*)&bih );
			RGBQUAD *rgb = bmi256->bmiColors;
			RGBTRIPLE *rgbt = (RGBTRIPLE*)colors;
			for (i = 0; i < numcolors; i++, rgb++, rgbt++)
				{
				rgb->rgbBlue	= rgbt->rgbtBlue;
				rgb->rgbGreen	= rgbt->rgbtGreen;
				rgb->rgbRed		= rgbt->rgbtRed;
				rgb->rgbReserved = 0;
				}
			}
		rc = TRUE;
		}
	else if (_tcsicmp( &filename[pathlen-4], _T(".tif") ) == 0)
		{
		//======================================== TIFF
		TIFF_Header		hdr;
		TIFF_IFDEntry	ifd_entry;
		ULONG	ifd_offset;
		ULONG	wanted_tags = TAGF_NEWSUBFILETYPE|TAGF_IMAGEWIDTH|TAGF_IMAGELENGTH|TAGF_SAMPLESPERPIXEL|TAGF_BITSPERSAMPLES|TAGF_XRESOLUTION|TAGF_YRESOLUTION|TAGF_RESOLUTIONUNIT;
		ULONG	found_tags = 0;

		bmi256->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		bmi256->bmiHeader.biWidth			= 0;
		bmi256->bmiHeader.biHeight			= 0;
		bmi256->bmiHeader.biPlanes			= 1;
		bmi256->bmiHeader.biBitCount		= 1;
		bmi256->bmiHeader.biCompression		= BI_RGB;
		bmi256->bmiHeader.biSizeImage		= 0;
		bmi256->bmiHeader.biXPelsPerMeter	= 0;
		bmi256->bmiHeader.biYPelsPerMeter	= 0;
		bmi256->bmiHeader.biClrUsed			= 0;
		bmi256->bmiHeader.biClrImportant	= 0;
		// Pas de table de couleurs pour l'instant
		memset( bmi256->bmiColors, 0, sizeof(RGBQUAD)*256 );

		// Lecture de l'entête
		if (fread( &hdr, sizeof(TIFF_Header), 1, fd ) == 0) goto END_ERROR;
		if (hdr.ByteOrder != TIFF_INTEL) goto END_ERROR;
		ifd_offset = hdr.IFDOffset;

		while (ifd_offset != 0 && found_tags != wanted_tags)
			{
			UWORD entry_count;
			ULONG bits = 0;
			ULONG samps = 0;
			ULONG resunit = 0;
			ULONG resx = 0, resy = 0, divx = 0, divy = 0;
			ULONG val1 = 0, val2 = 0;

			// Positionnement sur l'IFD suivant
			if (fseek( fd, ifd_offset, SEEK_SET ) != 0) goto END_ERROR;
			// Nombre d'entrées
			if (fread( &entry_count, sizeof(UWORD), 1, fd ) == 0) goto END_ERROR;

			// Lit toutes les entrées une par une
			for (; entry_count != 0 && found_tags != wanted_tags; entry_count--)
				{
				if (fread( &ifd_entry, sizeof(TIFF_IFDEntry), 1, fd ) == 0) goto END_ERROR;
				switch (ifd_entry.Tag)
					{
					case TAG_SUBFILETYPE:
					case TAG_NEWSUBFILETYPE:
						val1 = get_value( fd, &ifd_entry, &val2 );
						// if ((val1 & 0x01) || (val1 & 0x04)) goto END_ERROR; // Si pas pleine image
						found_tags |= TAGF_NEWSUBFILETYPE;
						break;

					case TAG_IMAGEWIDTH:
						bmi256->bmiHeader.biWidth = get_value( fd, &ifd_entry, &val2 );
						found_tags |= TAGF_IMAGEWIDTH;
						break;
					case TAG_IMAGELENGTH:
						bmi256->bmiHeader.biHeight = get_value( fd, &ifd_entry, &val2 );
						found_tags |= TAGF_IMAGELENGTH;
						break;

					case TAG_SAMPLESPERPIXEL:
						samps = get_value( fd, &ifd_entry, &val2 );
						found_tags |= TAGF_SAMPLESPERPIXEL;
						break;
					case TAG_BITSPERSAMPLES:
						bits = get_value( fd, &ifd_entry, &val2 );
						found_tags |= TAGF_BITSPERSAMPLES;
						break;

					case TAG_RESOLUTIONUNIT:
						resunit = get_value( fd, &ifd_entry, &val2 );
						found_tags |= TAGF_RESOLUTIONUNIT;
						break;
					case TAG_XRESOLUTION:
						if ((resx = get_value( fd, &ifd_entry, &divx )) == 0) goto END_ERROR;
						found_tags |= TAGF_XRESOLUTION;
						break;
					case TAG_YRESOLUTION:
						if ((resy = get_value( fd, &ifd_entry, &divy )) == 0) goto END_ERROR;
						found_tags |= TAGF_YRESOLUTION;
						break;

					default:
						break;
					}
				}

			// Remplissage nécessitant plusieurs tags
			if (bits && samps)
				bmi256->bmiHeader.biBitCount = (UWORD) (bits * samps);
			switch (resunit)
				{
				case 1: // Indéfinie
					bmi256->bmiHeader.biXPelsPerMeter = resx / divx;
					bmi256->bmiHeader.biYPelsPerMeter = resy / divy;
					break;
				default:
				case 2: // Pouce
					bmi256->bmiHeader.biXPelsPerMeter = (((resx * 3937) / divx) + 50) / 100; // 39,37 = pouces par mètre
					bmi256->bmiHeader.biYPelsPerMeter = (((resy * 3937) / divy) + 50) / 100; // 39,37 = pouces par mètre
					break;
				case 3: // cm
					bmi256->bmiHeader.biXPelsPerMeter = (resx * 100) / divx;
					bmi256->bmiHeader.biYPelsPerMeter = (resy * 100) / divy;
					break;
				}

			// On a le minimum ?
			if (bmi256->bmiHeader.biWidth && bmi256->bmiHeader.biHeight)
				rc = TRUE;

			// Lecture de l'offset vers l'IFD suivant
			// if (fread( &ifd_offset, sizeof(ULONG), 1, fd ) == 0) goto END_ERROR;
			ifd_offset = 0; // On ne lit que la première image
			}
		}

END_ERROR:
	fclose( fd );
	return rc;
}

HDIB OOGfx_DIB_Load( LPCTSTR filename )
{
	BITMAPFILEHEADER hdr;
	BITMAPINFO256 bih;
	HDIB hdib = NULL;
	DIB *dib;
	ULONG allocsize, palsize;
	FILE *fd = NULL;
	UBYTE *destbits;

	long pathlen = _tcslen( filename );
	if (pathlen < 4 || _tcsicmp( &filename[pathlen-4], _T(".bmp") ) != 0)
		return FALSE;

	//------ Lit l'entête et la transforme en win30

	if (OOGfx_DIB_LoadBMI256( filename, &bih ) == FALSE)
		return NULL;

	//------ Alloue la mémoire totale

	palsize = OOGfx_DIB_GetRGBSize( (DIB*)&bih );
	allocsize = sizeof(BITMAPINFOHEADER) + palsize + OOGfx_DIB_GetImageSize( (DIB*)&bih );
	if ((hdib = (HDIB) GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, allocsize )) == NULL) goto END_ERROR;
	dib = (DIB*) GlobalLock( (HGLOBAL)hdib );

	//------ Recopie les valeurs déjà connues

	dib->bmiHeader = bih.bmiHeader;
	memcpy( dib->bmiColors, bih.bmiColors, palsize );

	//------ Lit les bits de l'image

	// Lecture de l'entête
    if ((fd = fopen( filename, "rb" )) == NULL) goto END_ERROR;
	if (fread( &hdr, sizeof(BITMAPFILEHEADER), 1, fd ) == 0) goto END_ERROR;

	// Positionnement dans le fichier
	if (fseek( fd, hdr.bfOffBits, SEEK_SET ) != 0) goto END_ERROR;

	// Lecture de l'image
	destbits = ((UBYTE*)dib) + sizeof(BITMAPINFOHEADER) + palsize;
	if (fread( destbits, bih.bmiHeader.biSizeImage, 1, fd ) == 0) goto END_ERROR;

	// Et ben c'est ok
	GlobalUnlock( (HGLOBAL)hdib );
	fclose( fd );
	return hdib;

END_ERROR:
	if (fd != NULL) 
		fclose( fd );
	if (hdib != NULL)
		{
		GlobalUnlock( (HGLOBAL)hdib );
		GlobalFree( (HGLOBAL)hdib );
		}
	return NULL;
}

BOOL OOGfx_DIB_Save( HDIB hdib, LPCTSTR filename )
{
    BITMAPFILEHEADER	hdr;
    DIB *				dib;
	FILE *				fd;
	BOOL				rc = FALSE;

	assert( hdib != NULL );
    if (hdib == NULL || (dib = GlobalLock( hdib )) == NULL) goto END_ERROR;

	if ((fd = fopen( filename, "wb" )) == NULL) goto END_ERROR;

    hdr.bfType          = DIB_HEADER_MARKER;
    hdr.bfSize          = sizeof(BITMAPFILEHEADER) + GlobalSize( hdib );
    hdr.bfReserved1     = 0;
    hdr.bfReserved2     = 0;
    hdr.bfOffBits       = sizeof(BITMAPFILEHEADER) + dib->bmiHeader.biSize + OOGfx_DIB_GetRGBSize( dib );

	if (fwrite( &hdr, sizeof(BITMAPFILEHEADER), 1, fd ) == 0) goto END_ERROR;
	if (fwrite( dib, GlobalSize( hdib ), 1, fd ) == 0) goto END_ERROR;

	rc = TRUE;

END_ERROR:
    if (fd != NULL)
		fclose( fd );
    GlobalUnlock (hdib);
    return rc;
}
