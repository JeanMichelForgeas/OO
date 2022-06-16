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


/******* Includes ***********************************************/

#include "ootypes.h"
#include "oo_pool.h"
#include "oo_gfx_prv.h"


/******* Imported ***********************************************/


/******* Exported ***********************************************/


/******* Statics ************************************************/


/***************************************************************************
 *
 *      Loading images
 *
 ***************************************************************************/

OOBitMap *OOGfx_LoadPicture( UBYTE *filename )
{
    BITMAPFILEHEADER hdr;
    BITMAPINFOHEADER bih;
    UBYTE   *p;
    UWORD   i,dx,dy,Width,Height;
    FILE    *fd;
	OOPalette *palette = NULL;
    OOBitMap *bm = NULL;

    if ((fd = fopen( filename, "rb" )) != NULL)
		{
		//------ Lecture de l'entête
		if (fread( &hdr.bfType, sizeof(UWORD), 1, fd ) == 0) goto END_ERROR;
		if (fread( &hdr.bfSize, sizeof(LONG), 1, fd ) == 0) goto END_ERROR;
		if (fread( &hdr.bfReserved1, sizeof(UWORD), 1, fd ) == 0) goto END_ERROR;
		if (fread( &hdr.bfReserved2, sizeof(UWORD), 1, fd ) == 0) goto END_ERROR;
		if (fread( &hdr.bfOffBits, sizeof(LONG), 1, fd ) == 0) goto END_ERROR;

		if (fread( &bih, sizeof(BITMAPINFOHEADER), 1, fd ) == 0) goto END_ERROR;

		dx = (UWORD)bih.biWidth;
		dy = (UWORD)bih.biHeight;

		if (bih.biCompression)
			{ OOTRACE( "*** OOGfx_LoadPicture() : ne sait pas charger une image compressée !!!\n" ); goto END_ERROR; }

		Width = dx;
		Height = dy;
		dx = 4*( (dx+3)/4 );

		if (bih.biClrUsed == 0)
			if (bih.biBitCount == 8)
				bih.biClrUsed = 256;

		if (bih.biBitCount != 8)
			{ OOTRACE( "*** OOGfx_LoadPicture() : ne sait pas charger une image autre que 256 couleurs !!!\n" ); goto END_ERROR; }

		//------ Lecture de la palette
		if ((p = (UBYTE*)OOPool_Alloc( OOGlobalPool, 256*4 )) == NULL) goto END_ERROR;
		if (fread( p, bih.biClrUsed*4, 1, fd ) == 0) goto END_ERROR;

		// Crée la palette et recopie les rgb
		if ((palette = OOGfx_AllocPalette( p, bih.biClrUsed )) == NULL) goto END_ERROR;
		
		OOPool_Free( OOGlobalPool, (UBYTE*)p );

		//------ Lecture de l'image
		if ((bm = OOGfx_AllocBMap( palette, 8, Width, Height, 0 )) == NULL) 
			{ OOTRACE( "*** OOGfx_LoadPicture() : ne peut pas allouer une bitmap de %dx%d !!!\n", Width, Height ); goto END_ERROR; }

		p = (UBYTE*)bm->Planes + (bm->Height-1) * bm->BytesPerRow;
		for (i=0; i < dy; i++)
			{
			if (fread( p, Width, 1, fd ) == 0) goto END_ERROR;
			if (dx-Width > 0) fseek( fd, dx-Width, SEEK_CUR );
			p -= bm->BytesPerRow;
			}

END_ERROR:
		if (bm == NULL && palette != NULL) OOGfx_FreePalette( palette );
		fclose( fd );
		}
    return bm;
}

/*
//---------------------------------------------------
//
// load and decode a 256 color 1 plane version 5 PCX
// format file. You pass in a file name and pointers
// to fill in with the header, the palette, and the
// image.
//
#define PALETTESIZE 256

BOOL LPolygon::loadPcx(char *file, int *imgWidth,int *imgHeight,color *Pal)
{
    int pcx;
    tPCXHdr header;
    struct stat stats;

    int count;
    char pal[3 * 256];
    int r, g, b;
    unsigned char *next;

    int width;
    int paddedWidth;
    int height;

    int rawSize;
    int imgSize;

    char *raw, *src;
    char *img = NULL;

    int repeat;
    int byte;

    int i, j, k;

    //
    // first open the file
    //
    if (-1 == (pcx = open(file, O_RDONLY | O_BINARY)))
    {
        printf("couldn't open file \"%s\"\n", file);
        return FALSE;
    }

    //
    // get the file stats
    //
    fstat(pcx, &stats);
    stat(file, &stats);

    //
    // read the header of a pcx file
    //
    if (sizeof(tPCXHdr) != (count = read(pcx, &header, sizeof(tPCXHdr))))
    {
        printf("error reading header of file \"%s\"\n", file);
        close(pcx);
        return FALSE;
    }

    //
    // make sure that it is the kind of
    // file that we want. We're being
    // pretty picky here. But, that makes
    // life simpler.
    //
    if (header.zSoftFlag != 0x0a ||
        header.version != 5 ||
        header.bitsPerPixel != 8 ||
        header.planes != 1)
    {
        printf("This program can only read version 5 pcx files\n");
        printf("with 1 plane and 8 bits/pixel\n");
        close(pcx);
        return FALSE;
    }

    //
    // Now go pick up the palette
    //
    // Could do this later, but what the heck
    //
    if (-1 == lseek(pcx, stats.st_size - sizeof(pal), SEEK_SET))
    {
        printf("error seeking to palette of file \"%s\"\n", file);
        close(pcx);
        return FALSE;
    }

    if (sizeof(pal) != (count = read(pcx, &pal, sizeof(pal))))
    {
        printf("error reading palette of file \"%s\" wanted %d got %d\n",
                file, sizeof(pal), count);

        close(pcx);
        return FALSE;
    }

    //
    // convert the palette to the form I want
    //
    next =(unsigned char *) &pal[0];
    for (i = 0; i < 256; i++)
    {
        r = *next++;
        g = *next++;
        b = *next++;

        Pal[i].red = r >> 2;
        Pal[i].green = g >> 2;
        Pal[i].blue = b >> 2;
    }

    LOGPALETTE *Palette;
    if ( (Palette = (LOGPALETTE *)LocalAlloc(LPTR, 0x404)) == NULL )
        return FALSE;
    // Reset everything in the system palette to black
    Palette->palVersion = 0x300;
    Palette->palNumEntries = 0x100;
    for (i=0; i<256; i++)
    {
        Palette->palPalEntry[i].peRed =Pal[i].red;
        Palette->palPalEntry[i].peGreen =Pal[i].green;
        Palette->palPalEntry[i].peBlue =Pal[i].blue;
        Palette->palPalEntry[i].peFlags = 0;
    }

/// L3DWorld::hpal256 = CreatePalette( Palette );
    //
    // compute the size of the raw image,
    // get space for it, and read it in
    //
    *imgWidth = width = header.xMax - header.xMin + 1;
    paddedWidth = width + (width & 1);

    *imgHeight = height = header.yMax - header.yMin + 1;

    rawSize = stats.st_size - sizeof(tPCXHdr) - sizeof(pal);
    raw = (char*) OOPool_Alloc( OOGlobalPool, rawSize );
    if (raw == NULL)
    {
        printf("unable to allocate %d bytes\n", rawSize);
        close(pcx);
        return FALSE;
    }

    if (-1 == lseek(pcx, sizeof(tPCXHdr), SEEK_SET))
    {
        printf("error seeking to raw image if file \"%s\"\n", file);
        OOPool_Free( OOGlobalPool, raw );
        close(pcx);
        return FALSE;
    }

    if (rawSize != (count = read(pcx, raw, rawSize)))
    {
        printf("error reading raw image from file \"%s\" wanted %d got %d\n",
                file, rawSize, count);
        OOPool_Free( OOGlobalPool, raw );
        close(pcx);
        return FALSE;
    }
    //
    // get space for the uncompressed image
    //
    imgSize = paddedWidth * height;
    img = (byte*) OOPool_Alloc( OOGlobalPool, imgSize );

    if (img == NULL)
    {
        printf("unable to allocate %d bytes\n", imgSize);
        OOPool_Free( OOGlobalPool, raw );
        close(pcx);
        return FALSE;
    }

    //
    // decompress the raw image
    //

    i = 0;
    k = 0;
    src = raw;
    while (i < rawSize && k < imgSize)
    {
        if ((*src & 0xc0) == 0xc0)
        {
            repeat = *src & 0x3f;
            src++;
            byte = *src++;
            i += 2;

            for (j = 0; j < repeat; j++)
            {
                *img++ = byte;
                k++;
            }
        }
        else
        {
            *img++ = *src++;
            i++;
            k++;
        }
    }

    OOPool_Free( OOGlobalPool, raw );
    close(pcx);
    return TRUE;
}
*/
