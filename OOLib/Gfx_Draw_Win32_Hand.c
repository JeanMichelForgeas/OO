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

//#include "oo_3d_prv.h" // pour OOGfxHand_ScanConvert_Texture() : structure OOPlan, OO3DPoint, ...

struct minmax_info {
    SWORD minx, maxx, minx_ind, maxx_ind;
    SWORD miny, maxy, miny_ind, maxy_ind;
    };


/******* Imported ************************************************/


/******* Statics *************************************************/

static ULONG		OOGfxHand_DrawInit( void );
static void			OOGfxHand_DrawCleanup( void );
static void			OOGfxHand_FreePalette( OOPalette *oopalette );
static OOPalette *	OOGfxHand_AllocPalette( void *rgbtable, ULONG numcolors );
static void			OOGfxHand_SetDrawColor( OORastPort *rp, ULONG colornum );
static void			OOGfxHand_Move( OORastPort *rp, SLONG x, SLONG y );
static void			OOGfxHand_DrawPixel( OORastPort *rp, SLONG x, SLONG y );
static void			OOGfxHand_DrawLine( OORastPort *rp, SLONG x, SLONG y );
static void			OOGfxHand_DrawLine2( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
static void			OOGfxHand_DrawRect( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 );
static void			OOGfxHand_DrawBox( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2, ULONG type );
static void			OOGfxHand_DrawPolygon( OORastPort *rp, OOPolygon *pg, UBYTE *info_tab );

static void OOGfxHand_BuildLineTable( SWORD x1, SWORD y1, SWORD x2, SWORD y2, SWORD *table, ULONG index );
static void OOGfxHand_ScanConvert_Color( OORastPort *pg, OOPolygon *rp, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi );
//static void OOGfxHand_ScanConvert_Texture( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi );

static SLONG MoveX;
static SLONG MoveY;
static SWORD *XTable[2];
static SWORD *XMinTbl;
static SWORD *XMaxTbl;


/******* Exported ************************************************/

OOGfxFuncs GfxHandFuncs = {
	OOGfxHand_DrawInit,
	OOGfxHand_DrawCleanup,
	OOGfxHand_FreePalette,
	OOGfxHand_AllocPalette,
	OOGfxHand_SetDrawColor,
	OOGfxHand_Move,
	OOGfxHand_DrawPixel,
	OOGfxHand_DrawLine,
	OOGfxHand_DrawLine2,
	OOGfxHand_DrawRect,
	OOGfxHand_DrawBox,
	OOGfxHand_DrawPolygon,
};


/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

static ULONG OOGfxHand_DrawInit( void )
{
    RECT rect;
    ULONG maxrows;

    SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
    maxrows = rect.bottom - rect.top;
    XMinTbl = (SWORD*) OOPool_Alloc( OOGlobalPool, sizeof(SWORD)*maxrows );
    XMaxTbl = (SWORD*) OOPool_Alloc( OOGlobalPool, sizeof(SWORD)*maxrows );
    if (XMinTbl == NULL || XMaxTbl == NULL)
        return OOV_ERROR;
    XTable[0] = XMinTbl;
    XTable[1] = XMaxTbl;

    return OOV_OK;
}

static void OOGfxHand_DrawCleanup( void )
{
    if (XMinTbl != NULL) OOPool_Free( OOGlobalPool, XMinTbl );
    if (XMaxTbl != NULL) OOPool_Free( OOGlobalPool, XMaxTbl );
}


/***************************************************************************
 *
 *		Palettes		
 *
 ***************************************************************************/

static void OOGfxHand_FreePalette( OOPalette *oopalette )
{
	OOASSERT( FALSE ); // Pas utilisée
}

static OOPalette * OOGfxHand_AllocPalette( void *rgbtable, ULONG numcolors )
{
	OOASSERT( FALSE ); // Pas utilisée
	return NULL;
}


/***************************************************************************
 *
 *      Drawing lines & pixels
 *
 ***************************************************************************/

static void OOGfxHand_SetDrawColor( OORastPort *rp, ULONG colornum )
{
	OOASSERT( FALSE ); // Pas utilisée
}

static void OOGfxHand_Move( OORastPort *rp, SLONG x, SLONG y )
{
    MoveX = x;
    MoveY = y;
}

static void OOGfxHand_DrawPixel( OORastPort *rp, SLONG x, SLONG y )
{
    OOBitMap *bm = rp->BitMap;
    UBYTE *ptr = (UBYTE*)bm->Planes + ((bm->Height-1-y) * bm->BytesPerRow) + x;
    *ptr = rp->DrawPen;
}

static void OOGfxHand_DrawLine( OORastPort *rp, SLONG x, SLONG y )
{
    OOGfxHand_DrawLine2( rp, MoveX, MoveY, x, y );
    MoveX = x;
    MoveY = y;
}

static void OOGfxHand_DrawLine2( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
    SWORD dx, dy;
    UWORD i, absdx, absdy, count;
    ULONG lx, ly, coefx, coefy;
    SLONG oldy;
    OOBitMap *bm = rp->BitMap;
    UBYTE *lineptr, *plane = bm->Planes;

    dx = (SWORD)(x2 - x1);
    dy = (SWORD)(y2 - y1);
    absdx = abs(dx);
    absdy = abs(dy);

    if (absdx > absdy)
        {
        OOASSERT (dx != 0); // Pas de division par 0
        if (dx > 0) coefx = 0x10000; else coefx = 0xffff0000;
        coefy = (((dy << 16) >> 2) / absdx) << 2;
        count = absdx;
        }
    else{
        OOASSERT (dy != 0); // Pas de division par 0
        if (dy > 0) coefy = 0x10000; else coefy = 0xffff0000;
        coefx = (((dx << 16) >> 2) / absdy) << 2;
        count = absdy;
        }

    lx = x1 << 16;
    ly = y1 << 16;
    lineptr = plane + ((bm->Height-1-y1) * bm->BytesPerRow);
    oldy = y1;
    for (i=0; i < count; i++)
        {
        x1 = (SWORD)((lx + 0x00008000) >> 16);
        y1 = (SWORD)((ly + 0x00008000) >> 16);
        if (y1 != oldy) { oldy = y1; lineptr = plane + ((bm->Height-1-y1) * bm->BytesPerRow); }
        *(lineptr+x1) = rp->DrawPen;
        lx += coefx;
        ly += coefy;
        }
}

static void OOGfxHand_DrawBox( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2, ULONG type )
{
	OOASSERT( FALSE ); // Pas utilisée
}


/***************************************************************************
 *
 *      Drawing polygons
 *
 ***************************************************************************/

static void OOGfxHand_DrawRect( OORastPort *rp, SLONG x1, SLONG y1, SLONG x2, SLONG y2 )
{
	OOPoint pt[5] = { x1,y1, x1,y2, x2,y2, x2,y1, x1,y1 };
	OOPolygon P = { 
		4, 			// NumPoints
		OOF_FILL, 	// Flags   
		0, 			// LineColor
		rp->DrawPen,// FillColor
		NULL, 		// Texture
		pt 			// Points
		};
	OOGfxHand_DrawPolygon( rp, &P, NULL );
}

static void OOGfxHand_DrawPolygon( OORastPort *rp, OOPolygon *pg, UBYTE *info_tab )
{
    struct minmax_info MMI;
    UWORD x, y, i, j, inext;
    LONG orient=0;
    ULONG index, numpoints = pg->NumPoints;
    OOPoint *pt = pg->Points;
    OOPoint Tmp2DPoints[2];

//    if (! (pg->Flags & (OOF_FILL|OOF_MAP|OOF_LINES))) return;

    if (numpoints > 2)
        {
        //------------- Find out if polygon is clockwise or counterclockwise
        for (i=0; i < numpoints; i++) // Use the Newell method
            {
            if (i == (numpoints-1)) j = 0; else j = i+1;
            orient += (pt[i].x - pt[j].x)*(pt[i].y + pt[j].y);
            }
        // Elimine les erreurs dues au manque de précision dans les calculs
        if (orient < 0 && (pg->Flags & OOF_BACKORIENTED)) orient = 0;
        else if (orient > 0 && !(pg->Flags & OOF_BACKORIENTED)) orient = 0;

        //------------- Find miny,maxy & indexes ------
        for (MMI.minx=MMI.miny=MAXSWORD, MMI.maxx=MMI.maxy=MINSWORD, i=0; i < numpoints; i++)
            {
            x = (SWORD)pt[i].x;
            if (x < MMI.minx) { MMI.minx = x; MMI.minx_ind = i; }
            if (x >= MMI.maxx) { MMI.maxx = x; MMI.maxx_ind = i; }
            y = (SWORD)pt[i].y;
            if (y < MMI.miny) { MMI.miny = y; MMI.miny_ind = i; }
            if (y >= MMI.maxy) { MMI.maxy = y; MMI.maxy_ind = i; }
            }

        //------------- On trace une ligne ou un polygone
        if (orient != 0)
            {
            //------ Prépare les tables
            index = (orient < 0) ? 0 : 1;
            i = MMI.miny_ind;   // Start at top, fill to bottom, filling lines from top	to bottom
            do  {
                inext = i + 1;
                if (inext >= numpoints) inext = 0;      // Wrap around
                OOGfxHand_BuildLineTable( (SWORD)pt[i].x, (SWORD)pt[i].y, (SWORD)pt[inext].x, (SWORD)pt[inext].y, XTable[index], index );
                if (inext == MMI.maxy_ind) index ^= 1;  // Toggle min/max to keep lines
                i++;                                    // being filled from top to bottom
                if (i >= numpoints) i = 0;              // Wrap around
                } while (inext != MMI.miny_ind);        // Come full circle, done
            }
        else{
            //------ Simule une ligne
            if (MMI.miny_ind == MMI.maxy_ind) // En cas de ligne horizontale, faut pas pousser mémère dans les orties
                {                     //    alors on cherche sur les x
                if (MMI.minx_ind == MMI.maxx_ind) OOTRACE( "OOGfxHand_DrawPolygon : Alors c'est un point Ha Ha Ha\n" );
                Tmp2DPoints[0] = pt[MMI.minx_ind];
                Tmp2DPoints[1] = pt[MMI.maxx_ind];
                }
            else{
                Tmp2DPoints[0] = pt[MMI.miny_ind];
                Tmp2DPoints[1] = pt[MMI.maxy_ind];
                }
            pt = Tmp2DPoints;
            numpoints = 2;
            }
        }

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
            if (pg->Flags & OOF_FILL)
                {
                OOGfxHand_ScanConvert_Color( rp, pg, XMinTbl, XMaxTbl, &MMI );
                }
//            if ((pg->Flags & OOF_MAP) && pg->TextureInfo != NULL && pg->TextureInfo->BitMap != NULL)
                {
//                OOGfxHand_ScanConvert_Texture( rp, pg, XMinTbl, XMaxTbl, &MMI );
                }
//            if (pg->Flags & OOF_LINES)
                {
                ULONG i;
                OOGfx_SetDrawColor( rp, pg->LineColor );
                if (info_tab == NULL)
                    {
                    for (i=0; i < numpoints; i++)
                        OOGfx_DrawLine2( rp, pt[i].x, pt[i].y, pt[i+1].x, pt[i+1].y );
                    }
                else{
                    ULONG i;
                    for (i=0; i < numpoints; i++)
                        if (*info_tab++ != 0) OOGfx_DrawLine2( rp, pt[i].x, pt[i].y, pt[i+1].x, pt[i+1].y );
                    }
                }
            break;
        }
}

static void OOGfxHand_BuildLineTable( SWORD x1, SWORD y1, SWORD x2, SWORD y2, SWORD *table, ULONG index )
{
    SWORD tmp, dx, dy, oldy;
    UWORD i, absdx, absdy, count;
    ULONG lx, ly, coefx, coefy;
    SWORD xneg;

    // Toujours haut vers bas
    if (y1 > y2) { tmp = y2; y2 = y1; y1 = tmp; tmp = x2; x2 = x1; x1 = tmp; }

    // Signe de x
    if (index == 0) { if (x1 < x2) xneg = 0; else xneg = 1; }
    else { if (x1 > x2) xneg = 0; else xneg = 1; }

    table[y1] = x1;
    table[y2] = x2;
    dx = x2-x1;
    dy = y2-y1;
    absdx = abs(dx);
    absdy = abs(dy);

    if (absdx == 0)
        {
        for (y1++; y1 < y2; y1++) table[y1] = x1;
        return;
        }
    if (absdy == 0)
        {
        table[y1] = x1;
        return;
        }

    if (absdx > absdy)
        {
        OOASSERT (dx != 0); // Pas de division par 0
        if (dx > 0) coefx = 0x10000; else coefx = 0xffff0000;
        coefy = (((dy << 16) >> 2) / absdx) << 2;
        count = absdx + 1;
        }
    else{
        OOASSERT (dy != 0); // Pas de division par 0
        if (dy > 0) coefy = 0x10000; else coefy = 0xffff0000;
        coefx = (((dx << 16) >> 2) / absdy) << 2;
        count = absdy + 1;
        }

    lx = x1 << 16;
    ly = y1 << 16;
    if (xneg == 0)
        {
        oldy = y1 - 1;
        for (i=0; i < count; i++)
            {
            y1 = (SWORD)((ly + 0x00008000) >> 16);
            if (y1 != oldy) { oldy = y1; table[y1] = (SWORD)((lx + 0x00008000) >> 16); }
            lx += coefx;
            ly += coefy;
            }
        }
    else{
        for (i=0; i < count; i++)
            {
            y1 = (SWORD)((ly + 0x00008000) >> 16);
            table[y1] = (SWORD)((lx + 0x00008000) >> 16);
            lx += coefx;
            ly += coefy;
            }
        }
}

static void OOGfxHand_ScanConvert_Color( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi )
{
    OOBitMap *bm = rp->BitMap;
    UBYTE *plane = bm->Planes;
    UWORD rowcount;
    SWORD *tabminxptr, *tabmaxxptr, minx, maxx;
    UBYTE *lineptr;
    ULONG *xptr, leftmask, rightmask;
    ULONG colormask = B2L(pg->FillColor,pg->FillColor,pg->FillColor,pg->FillColor);

    tabminxptr = XMinTbl + mmi->miny;
    tabmaxxptr = XMaxTbl + mmi->miny;
    lineptr = (plane + ((bm->Height - 1 - mmi->miny) * bm->BytesPerRow));

    for (rowcount = mmi->maxy - mmi->miny + 1; rowcount > 0; rowcount--)
        {
        minx = *tabminxptr++;
        maxx = *tabmaxxptr++;

        if (minx <= maxx)
            {
            SWORD width4 = (maxx >> 2) - (minx >> 2) + 1; // largeur en paquets de 4 pixels
            xptr = (ULONG*) (lineptr + (minx & ~3));
            // Il faut construire les masks à l'envers à cause de l'inversion Intel
            leftmask = -1L << ((minx & 3) << 3); // minx de 0 à 3 -> décale de 0, 8, 16, 24
            rightmask = -1L << ((maxx & 3) << 3); // maxx de 0 à 3 -> décale de 24, 16, 8, 0
            rightmask = ~rightmask;
            if (width4 == 1)
                {
                *xptr = (*xptr & ~(leftmask&rightmask)) | ((leftmask&rightmask) & colormask);
                }
            else{
                *xptr++ = (*xptr & ~leftmask) | (leftmask & colormask);
                width4 -= 2;
                if (width4 > 50)
                    {
                    for (; width4 >= 50; width4-=50)
                        {
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask;
                        }
                    }
                if (width4 > 10)
                    {
                    for (; width4 >= 10; width4-=10)
                        {
                        *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask;
                        *xptr++ = colormask; *xptr++ = colormask;
                        }
                    }
                if (width4 != 0)
                    {
                    for (; width4 > 0; width4--) *xptr++ = colormask;
                    }
                *xptr = (*xptr & ~rightmask) | (rightmask & colormask);
                }
            }
        lineptr -= bm->BytesPerRow;
        }
}

/*
static void OOGfxHand_ScanConvert_Texture( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi )
{
    // From John Di Camillo

    //------ Fill data
    OOBitMap *destbm = rp->BitMap;
    UBYTE *plane = destbm->Planes;
    SLONG line_minx, line_maxx;
    SLONG x, y, width; // Laisser en signed pour éviter erreurs
    UBYTE *lineptr;
    //------ Texture data
    OOPlan *plan = (OOPlan*)pg->Plan;
    OODispTrans *dt = plan->Trans;
    UBYTE *t = pg->TextureInfo->BitMap->Planes;
    OO3DPoint A, B, C;

    //------ Fill init
    lineptr = (plane + ((destbm->Height - 1 - mmi->miny) * destbm->BytesPerRow));

    //------ Texture init
    OO3DMath_CrossProduct( &plan->P1, &plan->P3, &A );
    OO3DMath_CrossProduct( &plan->P2, &plan->P1, &B );
    OO3DMath_CrossProduct( &plan->P3, &plan->P2, &C );

    //OO3DMath_VecMul( &A, F2FLT(pg->TextureInfo->Width) );
    //OO3DMath_VecMul( &B, F2FLT(pg->TextureInfo->Height) );

    //--- Coordonnées spatiales du point (minx,miny)

    for (y=mmi->miny; y <= mmi->maxy; y++)
        {
        line_minx = XMinTbl[y];
        line_maxx = XMaxTbl[y];

        if (line_minx <= line_maxx)
            {
            UBYTE *xptr = lineptr + line_minx;
            FLOAT U, V, a, b, c, cinv;
            SLONG u, v;

            U = F2FLT((line_minx - mmi->minx));
            V = F2FLT((y - mmi->miny));
            a = U * A.X + V * A.Y + A.Z;
            b = U * B.X + V * B.Y + B.Z;
            c = U * C.X + V * C.Y + C.Z;
            cinv = (FLOAT)128 / c;

            for (width=line_maxx-line_minx+1, x=line_minx; x <= line_maxx; x++)
                {
                u = (((SLONG)(a * cinv)) - 64) & 0x7f;
                v = (((SLONG)(b * cinv)) + 64) & 0x7f;
                *xptr++ = t[(v<<7) + u];
                a += A.X;
                b += B.X;
                c += C.X;
                }
            }
        lineptr -= destbm->BytesPerRow;
        }
}
*/

/*
void OOGfxHand_ScanConvert_Texture( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi )
{
    //------ Fill data
    OOBitMap *destbm = rp->BitMap;
    UBYTE *plane = destbm->Planes;
    UWORD rowcount, width, height;
    SWORD *tabminxptr, *tabmaxxptr, line_minx, line_maxx;
    UBYTE *lineptr;
    //------ Texture data
    OOBitMap *textbm = pg->TextureInfo->BitMap;
    UBYTE *t = textbm->Planes;
    SLONG au, av, u, v, du, dv;

    //------ Fill init
    tabminxptr = XMinTbl + miny;
    tabmaxxptr = XMaxTbl + miny;
    lineptr = (plane + ((destbm->Height-1-miny) * destbm->BytesPerRow));


    av = 0;
    height = maxy - miny + 1;
    dv = (128 << 16) / (height); // Et ben non : -1 pour tomber sur le dernier à la fin de la boucle
    for (rowcount=0; rowcount < height; rowcount++)
        {
        line_minx = *tabminxptr++;
        line_maxx = *tabmaxxptr++;
        v = ((av+0x8000)>>16) & 127;

        if (line_minx <= line_maxx)
            {
            UBYTE *xptr = lineptr + line_minx;
            au = 0;
            width = line_maxx - line_minx + 1;
            du = (128 << 16) / (width); // Et ben non : -1 pour tomber sur le dernier à la fin de la boucle
            for (; width > 0; width--)
                {
                u = ((au+0x8000)>>16) & 127;
                *xptr++ = t[(v<<7)+u];
                au += du;
                }
            }
        av += dv;
        lineptr -= destbm->BytesPerRow;
        }
}
*/

/*
extern UBYTE Index3D[]; // Dans Class-3DCamera, provisoire

struct OO3DInc {
    FLOAT   dXx, dYx, dZx;
    FLOAT   dXy, dYy, dZy;
    };

static void x_increments_per_pixel( OOPolygon *pg, struct minmax_info *mmi, struct 003DInc *inc, OO3DPoint *p )
{
    OO3DPoint *S1, *S2;
    OOPlan *plan = (OOPlan*)pg->Plan;
    FLOAT X1, X2, diff_X, diff_x, Z1, Z2, diff_Z;

    S1 = &plan->Points[Index3D[mmi->minx_ind]];
    X1 = FMUL( S1->X, S1->Z );  // X1 réel
    S2 = &plan->Points[Index3D[mmi->maxx_ind]];
    X2 = FMUL( S2->X, S2->Z );  // X2 réel

    diff_X = FSUB( X2, X1 );
    diff_x = F2FLT(pg->Points[mmi->maxx_ind].x - pg->Points[mmi->minx_ind].x);
    inc->dXx = FDIV( diff_X, diff_x );

    Z1 = S1->Z;
    Z2 = OO3DMath_PlanZ( plan, X2, S1->Y );

    diff_Z = FSUB( Z2, Z1 );
    inc->dZx = FDIV( diff_Z, diff_x );

    p->X = X1; // Tant qu'on y est, on récupère pour plus tard
}

static void y_increments_per_pixel( OOPolygon *pg, struct minmax_info *mmi, struct OO3DInc *inc, OO3DPoint *p )
{
    OO3DPoint *S1, *S2;
    OOPlan *plan = (OOPlan*)pg->Plan;
    FLOAT Y1, Y2, diff_Y, diff_y, Z1, Z2, diff_Z;

    S1 = &plan->Points[Index3D[mmi->miny_ind]];
    Y1 = FMUL( S1->Y, S1->Z );
    S2 = &plan->Points[Index3D[mmi->maxy_ind]];
    Y2 = FMUL( S2->Y, S2->Z );

    diff_Y = FSUB( Y2, Y1 );
    diff_y = F2FLT(pg->Points[mmi->maxy_ind].y - pg->Points[mmi->miny_ind].y);
    inc->dYy = FDIV( diff_Y, diff_y );

    Z1 = S1->Z;
    Z2 = OO3DMath_PlanZ( plan, S1->X, Y2 );

    diff_Z = FSUB( Z2, Z1 );
    inc->dZy = FDIV( diff_Z, diff_y );

    p->Y = Y1; // Tant qu'on y est, on récupère pour plus tard
}

#define OO3DMath_Project(s,d) { (d)->X = FDIV((s)->X,(s)->Z); (d)->Y = FDIV((s)->Y,(s)->Z); (d)->Z = (s)->Z; }

void OOGfxHand_ScanConvert_Texture( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi )
{
    //------ Fill data
    OOBitMap *destbm = rp->BitMap;
    UBYTE *plane = destbm->Planes;
    SLONG i, j, line_minx, line_maxx;
    SLONG width, height; // Laisser en signed pour éviter erreurs
    UBYTE *lineptr;
    //------ Texture data
    OOPlan *plan = (OOPlan*)pg->Plan;
    OODispTrans *dt = plan->Trans;
    UBYTE *t = pg->TextureInfo->BitMap->Planes;
    FLOAT ZY, X, Y, Z;
    struct OO3DInc _I;
    OO3DPoint PMin;

    //------ Fill init
    lineptr = (plane + ((destbm->Height - 1 - mmi->miny) * destbm->BytesPerRow));

    //------ Texture init
    //--- Incréments X,Y,Z sur x et y
    x_increments_per_pixel( pg, mmi, &_I, &PMin );
    y_increments_per_pixel( pg, mmi, &_I, &PMin );

    //--- Coordonnées spatiales du point (minx,miny)
    PMin.Z = OO3DMath_PlanZ( plan, PMin.X, PMin.Y );

    Y = PMin.Y;
    Z = ZY = PMin.Z;
    for (y=mmi->miny; y <= mmi->maxy; y++)
        {
        line_minx = XMinTbl[y];
        line_maxx = XMaxTbl[y];

        if (line_minx <= line_maxx)
            {
            UBYTE *xptr = lineptr + line_minx;
            OO3DPoint, P1, P2, p1, p2;
            FLOAT inc_minx = F2FLT(line_minx - mmi->minx);
            FLOAT inc_maxx = F2FLT(line_maxx - mmi->minx);
            SLONG u, v;
            FLOAT du, dv;

            P1.X = FADD( PMin.X, FMUL( _I.dXx, inc_minx ) );
            P1.Y = Y;
            P1.Z = FADD( Z, FMUL( _I.dZx, inc_minx ) );
            P2.X = FADD( PMin.X, FMUL( _I.dXx, inc_maxx ) );
            P2.Y = Y;
            P2.Z = FADD( Z, FMUL( _I.dZx, inc_maxx ) );

            u =
            du = P2.Z - ;
            dv = FNEG(FSUB( p2.,  ));

            for (width=line_maxx-line_minx+1, x=line_minx; x <= line_maxx; x++)
                {
                u = ((long)(X/Z * 128)) & 0x7f;
                v = ((long)(Y/Z * 128)) & 0x7f;
                *xptr++ = t[(v<<7) + u];
                X += dXx;
                }

            }
        Z = (ZY += _I.dZy);
        Y += _I.dYy;
        lineptr -= destbm->BytesPerRow;
        }
}
*/

/*
void OOGfxHand_ScanConvert_Texture( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi )
{
    //------ Fill data
    OOBitMap *destbm = rp->BitMap;
    UBYTE *plane = destbm->Planes;
    SLONG i, j, width, height, line_minx, line_maxx;
    UBYTE *lineptr;
    ULONG colormask;
    //------ Texture data
    OOPlan *plan = (OOPlan*)pg->Plan;
    UBYTE *t = pg->TextureInfo->BitMap->Planes;

    //------ Fill init
    lineptr = (plane + ((destbm->Height-1-miny) * destbm->BytesPerRow));

    for (height=maxy-miny+1, j=miny; j <= maxy; j++)
        {
        line_minx = XMinTbl[j];
        line_maxx = XMaxTbl[j];

        if (line_minx <= line_maxx)
            {
            UBYTE *xptr = lineptr + line_minx;
            SLONG u, v;
            OO3DPoint S;
            FLOAT a, b, c;

            a = (line_minx) * plan->A.x + (j) * plan->A.y + plan->A.z;
            b = (line_minx) * plan->A.x + (j) * plan->B.y + plan->B.z;
            c = (line_minx) * plan->A.x + (j) * plan->C.y + plan->C.z;

            for (width=line_maxx-line_minx+1, i=line_minx; i <= line_maxx; i++)
                {
                FLOAT cinv = (FLOAT)128 / c;
                u = (((SLONG)(a * cinv)));
                v = (((SLONG)(b * cinv)));
                u = u & 127;
                v = v & 127;
                *xptr++ = t[(v<<7) + u];
                a += plan->A.x;
                b += plan->B.x;
                c += plan->C.x;

            }
        lineptr -= destbm->BytesPerRow;
        }
}
*/

/*
#define GTP(a,b) (((((SLONG)((b+=hy) * divz)) << 7) & 127) << 7) + ((((SLONG)((a+=hx) * divz)) << 7) & 127)

ULONG gtp( SLONG a, SLONG b, SLONG divz )
{
    SLONG iu = (((SLONG)(a * divz)) << 7) & 127;
    SLONG iv = (((SLONG)(b * divz)) << 7) & 127;
    return (iv << 7) + iu;
}

void OOGfxHand_ScanConvert_Texture( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi )
{
    //------ Fill data
    OOBitMap *destbm = rp->BitMap;
    UBYTE *plane = destbm->Planes;
    SLONG i, j, width, height, line_minx, line_maxx;
    UBYTE *lineptr;
    ULONG colormask;
    //------ Texture data
    OOPlan *plan = (OOPlan*)pg->Plan;
    UBYTE *t = pg->TextureInfo->BitMap->Planes;

    //------ Fill init
    lineptr = (plane + ((destbm->Height-1-miny) * destbm->BytesPerRow));

    for (height=maxy-miny+1, j=miny; j <= maxy; j++)
        {
        line_minx = XMinTbl[j];
        line_maxx = XMaxTbl[j];

        if (line_minx <= line_maxx)
            {
            UBYTE *xptr = lineptr + line_minx;
            SWORD width4 = (maxx >> 2) - (minx >> 2) + 1; // largeur en paquets de 4 pixels
            ULONG leftmask, rightmask, *xptr = (ULONG*) (lineptr + (minx & ~3));
            // Il faut construire les masks à l'envers à cause de l'inversion Intel
            leftmask = -1L << ((minx & 3) << 3); // minx de 0 à 3 -> décale de 0, 8, 16, 24
            rightmask = -1L << ((maxx & 3) << 3); // maxx de 0 à 3 -> décale de 24, 16, 8, 0
            rightmask = ~rightmask;
            if (width4 == 1)
                {
                switch (minx & 3)
                    {
                    case 0: colormask = B2L( *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)) ); break;
                    case 1: colormask = B2L( 0            , *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)) ); break;
                    case 2: colormask = B2L( 0            , 0            , *(t+GTP(x,y)), *(t+GTP(x,y)) ); break;
                    case 3: colormask = B2L( 0            , 0            , 0            , *(t+GTP(x,y)) ); break;
                    }
                *xptr = (*xptr & ~(leftmask&rightmask)) | ((leftmask&rightmask) & colormask);
                }
            else{
                switch (minx & 3)
                    {
                    case 0: colormask = B2L( *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)) ); break;
                    case 1: colormask = B2L( 0            , *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)) ); break;
                    case 2: colormask = B2L( 0            , 0            , *(t+GTP(x,y)), *(t+GTP(x,y)) ); break;
                    case 3: colormask = B2L( 0            , 0            , 0            , *(t+GTP(x,y)) ); break;
                    }
                *xptr++ = (*xptr & ~leftmask) | (leftmask & colormask);
                width4 -= 2;
                for (; width4 > 0; width4--)
                    {
                    ULONG a, b, c, d;
                    // colormask = B2L( *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)) );
                    *xptr++ = colormask;
                    }
                // colormask = B2L( *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)), *(t+GTP(x,y)) );
                *xptr = (*xptr & ~rightmask) | (rightmask & colormask);
                }
            }
        lineptr -= destbm->BytesPerRow;
        }
}
*/

/*
void OOGfxHand_ScanConvert_Texture( OORastPort *rp, OOPolygon *pg, SWORD *XMinTbl, SWORD *XMaxTbl, struct minmax_info *mmi )
{
    OOBitMap *destbm = rp->BitMap;
    UBYTE *plane = destbm->Planes;
    UWORD rowcount;
    SWORD *tabminxptr, *tabmaxxptr, minx, maxx;
    UBYTE *lineptr;
    ULONG colormask = B2L(pg->FillColor,pg->FillColor,pg->FillColor,pg->FillColor);

    tabminxptr = XMinTbl + miny;
    tabmaxxptr = XMaxTbl + miny;
    lineptr = (plane + ((destbm->Height-1-miny) * destbm->BytesPerRow));

    for (rowcount = maxy - miny + 1; rowcount > 0; rowcount--)
        {
        minx = *tabminxptr++;
        maxx = *tabmaxxptr++;

        if (minx <= maxx)
            {
            SWORD width4 = (maxx >> 2) - (minx >> 2) + 1; // largeur en paquets de 4 pixels
            ULONG leftmask, rightmask, *xptr = (ULONG*) (lineptr + (minx & ~3));
            // Il faut construire les masks à l'envers à cause de l'inversion Intel
            leftmask = -1L << ((minx & 3) << 3); // minx de 0 à 3 -> décale de 0, 8, 16, 24
            rightmask = -1L << ((maxx & 3) << 3); // maxx de 0 à 3 -> décale de 24, 16, 8, 0
            rightmask = ~rightmask;
            if (width4 == 1)
                {
                *xptr = (*xptr & ~(leftmask&rightmask)) | ((leftmask&rightmask) & colormask);
                }
            else{
                *xptr++ = (*xptr & ~leftmask) | (leftmask & colormask);
                width4 -= 2;
                for (; width4 > 0; width4--) *xptr++ = colormask;
                *xptr = (*xptr & ~rightmask) | (rightmask & colormask);
                }
            }
        lineptr -= destbm->BytesPerRow;
        }
}
*/
