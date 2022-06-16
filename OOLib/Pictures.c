/*****************************************************************
 *
 *       Project:    OO
 *       Function:   Text processing
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1997 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_text.h"
#include "oo_pool.h"
#include "oo_pictures.h"

#include "oo_gfx_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static void free_pic( OOPicItem *pic );

static OOPicItem	defaultpics[] = { NULL,0,0,NULL, NULL,0,0,(void*)OOV_ENDTABLE };
static OOPicItem *	DefaultPics = defaultpics;

static ULONG NumPics;


/***************************************************************************
 *
 *      Catalog
 *
 ***************************************************************************/

void OOPic_SetTable( OOPicItem *table )
{
	OOPicItem *p = DefaultPics;

	for (; p->Picture != (char*)OOV_ENDTABLE; p++)
		free_pic( p );

	if (table == NULL)
		{
		DefaultPics = defaultpics;
		NumPics = 0;
		}
	else{
		DefaultPics = table;
		for (NumPics=0, p=&DefaultPics[1]; p->Picture != (char*)OOV_ENDTABLE; p++)
			{
			NumPics++;
			if (p->Flags & OOF_PIC_PRELOAD)
				OOPic_Get( NumPics );
			}
		}
}

void *OOPic_Get( ULONG index )
{
	OOPicItem *p = &DefaultPics[index];

	OOASSERT( index <= NumPics );
	if (p->Picture == NULL && (p->Flags & (OOF_PIC_LOADFILE | OOF_PIC_RESOURCE)))
		{
#ifdef _WIN32
		switch (p->Flags & (OOF_PIC_ICON|OOF_PIC_POINTER|OOF_PIC_BITMAP))
			{
			case OOF_PIC_ICON:
				if ((p->Flags & OOF_PIC_RESOURCE) && (p->Picture = LoadIcon( OOInstance, p->Name )) != NULL)
					p->Flags |= OOF_PIC_ALLOCATED;
				break;

			case OOF_PIC_POINTER:
				if ((p->Flags & OOF_PIC_RESOURCE) && (p->Picture = LoadCursor( OOInstance, p->Name )) != NULL)
					p->Flags |= OOF_PIC_ALLOCATED;
				break;

			case OOF_PIC_BITMAP:
				if ((p->Flags & OOF_PIC_RESOURCE) && (p->Picture = LoadBitmap( OOInstance, p->Name )) != NULL)
					p->Flags |= OOF_PIC_ALLOCATED;
				break;
			}
#endif
		}
    return p->Picture;
}

void *OOPic_GetSized( ULONG index, ULONG width, ULONG height )
{
	OOPicItem *p = &DefaultPics[index];

	OOASSERT( index <= NumPics );
	if (p->Picture == NULL && (p->Flags & (OOF_PIC_LOADFILE | OOF_PIC_RESOURCE)))
		{
#ifdef _WIN32
		switch (p->Flags & (OOF_PIC_ICON|OOF_PIC_POINTER|OOF_PIC_BITMAP))
			{
			case OOF_PIC_ICON:
				if ((p->Flags & OOF_PIC_RESOURCE) && (p->Picture = LoadImage( OOInstance, p->Name, IMAGE_ICON, width, height, LR_LOADTRANSPARENT )) != NULL)
					p->Flags |= OOF_PIC_ALLOCATED;
				break;

			case OOF_PIC_POINTER:
				if ((p->Flags & OOF_PIC_RESOURCE) && (p->Picture = LoadImage( OOInstance, p->Name, IMAGE_CURSOR, width, height, LR_LOADTRANSPARENT )) != NULL)
					p->Flags |= OOF_PIC_ALLOCATED;
				break;

			case OOF_PIC_BITMAP:
				if ((p->Flags & OOF_PIC_RESOURCE) && (p->Picture = LoadImage( OOInstance, p->Name, IMAGE_BITMAP, width, height, 0 )) != NULL)
					p->Flags |= OOF_PIC_ALLOCATED;
				break;
			}
#endif
		}
    return p->Picture;
}

ULONG OOPic_GetSize( ULONG index, void *vsize )
{
	OOSize *size = (OOSize*)vsize;
	OOPicItem *p = &DefaultPics[index];

	OOASSERT( index <= NumPics );
	size->W = size->H = 0;

	if (p->Picture == NULL) OOPic_Get( index );

	if (p->Picture != NULL)
		{
#ifdef _WIN32
		BITMAP _BM;
		switch (p->Flags & (OOF_PIC_ICON|OOF_PIC_POINTER|OOF_PIC_BITMAP))
			{
			case OOF_PIC_ICON:
			case OOF_PIC_POINTER:
				{
				ICONINFO _II;
				if (GetIconInfo( p->Picture, &_II ) && GetObject( _II.hbmColor, sizeof(BITMAP), &_BM ))
					{
					size->W = (UWORD)_BM.bmWidth;
					size->H = (UWORD)_BM.bmHeight;
					return OOV_OK;
					}
				}
				break;

			case OOF_PIC_BITMAP:
				if (GetObject( p->Picture, sizeof(BITMAP), &_BM ))
					{
					size->W = (UWORD)_BM.bmWidth;
					size->H = (UWORD)_BM.bmHeight;
					return OOV_OK;
					}
				break;
			}
#endif
		}
    return OOV_ERROR;
}

OOPicItem *OOPic_GetItem( ULONG index )
{
	OOASSERT( index <= NumPics );
	if (index > NumPics) return NULL;
	return &DefaultPics[index];
}

void *OOPic_Draw( ULONG index, void *vrp, void *vbox, ULONG place )
{
	OORastPort *rp = (OORastPort*)vrp;
	OOBox *b = (OOBox*)vbox;
	OOPicItem *p = &DefaultPics[index];

	OOASSERT( index <= NumPics );
	if (p->Picture == NULL) OOPic_Get( index );

	if (p->Picture != NULL)
		{
		OOSize _S;
		OOBox _B = *b;
		if (OOPic_GetSize( index, &_S ) == OOV_OK)
			{
			_B.W = _S.W;
			_B.H = _S.H;
			if (place & OOV_CENTER)
				{
				_B.X = b->X + (b->W - _S.W) / 2;
				_B.Y = b->Y + (b->H - _S.H) / 2;
				}
			if (place & OOV_LEFT) _B.X = b->X;
			if (place & OOV_TOP) _B.Y = b->Y;
			if (place & OOV_RIGHT) _B.X = b->X + b->W - _S.W;
			if (place & OOV_BOTTOM) _B.Y = b->Y + b->H - _S.H;
			}
#ifdef _WIN32
		switch (p->Flags & (OOF_PIC_ICON|OOF_PIC_POINTER|OOF_PIC_BITMAP))
			{
			case OOF_PIC_ICON:
			case OOF_PIC_POINTER:
				DrawIconEx( rp->DC, _B.X, _B.Y, p->Picture, _B.W, _B.H, 0, NULL, DI_NORMAL );
				break;

			case OOF_PIC_BITMAP:
				{
				HDC dc = CreateCompatibleDC(NULL);
				SelectObject( dc, p->Picture );
				BitBlt( rp->DC, _B.X, _B.Y, _B.W, _B.H, dc, 0, 0, SRCCOPY );
				DeleteDC( dc );
				}
				break;
			}
#endif
		}
    return p->Picture;
}

void *OOPic_Set( ULONG index, void *newpic, BOOL alloc )
{
	OOPicItem *p = &DefaultPics[index];

	OOASSERT( index <= NumPics );
	if (alloc == FALSE || newpic == NULL)
		{
		free_pic( p );
		p->Picture = newpic;
		}
	else{
		ULONG type;
		HANDLE pic;
		switch (p->Flags & (OOF_PIC_ICON|OOF_PIC_POINTER|OOF_PIC_BITMAP))
			{
			case OOF_PIC_ICON: type = IMAGE_ICON; break;
			case OOF_PIC_POINTER: type = IMAGE_CURSOR; break;
			case OOF_PIC_BITMAP: type = IMAGE_BITMAP; break;
			default: return NULL;
			}
		if ((pic = CopyImage( newpic, type, 0, 0, LR_COPYRETURNORG )) != NULL)
			{
			free_pic( p );
			p->Picture = pic;
			p->Flags |= OOF_PIC_ALLOCATED;
			}
		}
	return p->Picture;
}

//---------------------------------------------------

static void free_pic( OOPicItem *p )
{
	if (p->Picture != NULL && (p->Flags & OOF_PIC_ALLOCATED))
		{ 
		DeleteObject( p->Picture );
		p->Flags &= ~OOF_PIC_ALLOCATED;
		p->Picture = NULL; 
		}
}

