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


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/


/***************************************************************************
 *
 *      Creating BitMaps & RastPorts
 *
 ***************************************************************************/

void OOGfx_InitRPort( OORastPort *rport )
{
	InitRastPort( rport );
}

void OOGfx_FreeRPort( OORastPort *rport )
{
    if (rport != NULL)
        OOPool_Free( OOGlobalPool, rport );
}

OORastPort * OOGfx_AllocRPort( void )
{
  OORastPort *rport;

    if ((rport = (OORastPort *) OOPool_AllocClear( OOGlobalPool, sizeof(OORastPort) )) != NULL)
        OOGfx_InitRPort( rport );
    return rport;
}

void OOGfx_FreeBMap( struct BitMap *bitmap )
{
  UBYTE i;
  OOBitMap *ubm;

    if (bitmap == NULL) return;
    ubm = (OOBitMap *) (((ULONG)bitmap) - offsetof(OOBitMap,BytesPerRow));
    if (! (ubm->UFlags & OOBMAP_NOPLANE))
        {
        if (ubm->UFlags & OOBMAP_CONTIG) { if (ubm->Planes[0] != NULL) FreeVec( ubm->Planes[0] ); }
        else for (i=0; i<ubm->Depth; i++) { if (ubm->Planes[i] != NULL) FreeVec( ubm->Planes[i] ); }
        if (ubm->Mask != NULL) FreeVec( ubm->Mask );
        }
    OOPool_Free( OOGlobalPool, ubm );
}

struct BitMap * OOGfx_AllocBMap( UBYTE depth, UWORD width, UWORD height, UWORD UFlags )
{
  OOBitMap *ubm;
  struct BitMap *bitmap=0;
  UWORD size, i;
  UBYTE *ptr;
  ULONG reqs = (UFlags & OOBMAP_PUBLIC ? MEMF_PUBLIC : MEMF_CHIP) | MEMF_CLEAR;

    if ((ubm = (OOBitMap *) OOPool_AllocClear( OOGlobalPool, sizeof(OOBitMap) )) != NULL)
        {
        ubm->UFlags = UFlags;
        ubm->Width = width;
        bitmap = (struct BitMap *) (((ULONG)ubm) + offsetof(OOBitMap,BytesPerRow));
        InitBitMap( bitmap, depth, width, height );
        if (! (ubm->UFlags & OOBMAP_NOPLANE))
            {
            size = bitmap->BytesPerRow * bitmap->Rows;
            if (UFlags & OOBMAP_CONTIG)
                {
                if (! (ptr = AllocVec( size * depth, reqs ))) return(0);
                for (i=0; i<depth; i++) { bitmap->Planes[i] = ptr; ptr += size; }
                }
            else{
                for (i=0; i<depth; i++)
                    {
                    if ((bitmap->Planes[i] = (PLANEPTR) AllocVec( size, reqs )) == NULL)
                        {
                        OOGfx_FreeBMap( bitmap );
                        return NULL;
                        }
                    }
                }
            if (UFlags & OOBMAP_MASKED)
                {
                if ((ubm->Mask = AllocVec( size, MEMF_CHIP | MEMF_CLEAR )) == NULL)
                    return NULL;
                }
            }
        }
    return bitmap;
}

void OOGfx_FreeBMapRPort( OORastPort *rport )
{
    if (rport != NULL)
        {
        OOGfx_FreeBMap( rport->BitMap );
        OOGfx_FreeRPort( rport );
        }
}

OORastPort * OOGfx_AllocBMapRPort( UBYTE depth, UWORD width, UWORD height, ULONG uflags )
{
  OORastPort *rport=0;

    if ((rport = OOGfx_AllocRPort()) != NULL)
        {
        if (! (rport->BitMap = OOGfx_AllocBMap( depth, width, height, UFlags )))
            {
            OOGfx_FreeBMapRPort( rport );
            rport = NULL;
            }
        }
    return( rport );
}
