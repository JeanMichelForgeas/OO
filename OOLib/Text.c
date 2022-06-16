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
#include "oo_gui.h"

#include "oo_gfx_prv.h"
#include "oo_text_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static void free_string( OOTextItem *text );

// static OOCatalog *	Catalog;
static OOTextItem       defaulttext[] = { 0,0,0,NULL, 0,0,0,(char*)OOV_ENDTABLE };
static OOTextItem *     DefaultText = defaulttext;

static ULONG NumStrings;


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

void OOStr_SetTable( OOTextItem *table )
{
	OOTextItem *t = DefaultText;
	for (; t->Text != (char*)OOV_ENDTABLE; t++)
		free_string( t );

	if (table == NULL)
		{
		DefaultText = defaulttext;
		NumStrings = 0;
		}
	else{
		DefaultText = table;
		for (NumStrings=0, t=&DefaultText[1]; t->Text != (char*)OOV_ENDTABLE; t++)
			NumStrings++;
		}
}

char *OOStr_Get( ULONG index )
{
	OOTextItem *t = &DefaultText[index];

	OOASSERT( index <= NumStrings );
	if (t->Text == NULL)
		{
#ifdef _WIN32
		if (t->Flags & OOF_TEXT_RESOURCE)
			{
			HRSRC	hs;
			HGLOBAL	hres = NULL;
			LPVOID	res = NULL;
			if ((hs = FindResource( NULL, MAKEINTRESOURCE(t->CodeId), RT_STRING )) != NULL)
				{
				if ((hres = LoadResource( NULL, hs )) != NULL)
					{
					if ((res = LockResource( hres )) != NULL)
						{
						UWORD i, len = *((UWORD*)res+1);
						UWORD *wc ;
						char *str, *p;
						OOASSERT( OOGlobalPool != NULL );
						if ((p = str = (char*) OOPool_Alloc( OOGlobalPool, len + 1 )) != NULL)
							{
							for (i=0, wc=((UWORD*)res+2); i < len; i++)
								*p++ = (char)(*wc++);
							str[len] = 0;
							t->Text = str;
							t->Flags |= OOF_TEXT_ALLOCATED;
							}
						UnlockResource( hres );
						}
					FreeResource( hres );
					}
				}
			}
#endif
		}
    return t->Text;
}

ULONG OOStr_GetSize( ULONG index, void *vfont, void *vsize )
{
	OOFont *font = (OOFont*)vfont;
	OOSize *size = (OOSize*)vsize;
	OOSize _S;
	OOTextItem *t = &DefaultText[index];
	UBYTE *p, *str;
	ULONG num;

	OOASSERT( index <= NumStrings );
	if (t->Text == NULL) OOStr_Get( index );

	size->W = size->H = 0;
	str = t->Text;
	if (str == NULL || *str == 0) str = " ";

	for (p=str; ; p++)
		{
		if (*p == '\n' || *p == 0)
			{
			num = p - str;
			if (num == 0) { num = 1; str = " ";	}

			OOGfx_GetSizeOfString( font, str, num, &_S );
			if (_S.W > size->W)	size->W = _S.W;
			size->H += _S.H;

			if (*p == 0) break; else str = p+1;
			}
		}
    return OOV_OK;
}

char *OOStr_Draw( ULONG index, void *vrp, void *vbox, ULONG place )
{
	OORastPort *rp = (OORastPort*)vrp;
	OOBox *b = (OOBox*)vbox;
	OOTextItem *t = &DefaultText[index];

	OOASSERT( index <= NumStrings );
	if (t->Text == NULL) OOStr_Get( index );

	if (t->Text != NULL)
		{
#ifdef _WIN32
		ULONG flags = DT_NOCLIP;
		RECT _R;
		if (place & OOV_CENTER)	flags |= DT_CENTER|DT_VCENTER;
		if (place & OOV_LEFT)	{ flags |= DT_LEFT; flags &= ~DT_CENTER; }
		if (place & OOV_TOP)	{ flags |= DT_TOP; flags &= ~DT_VCENTER; }
		if (place & OOV_RIGHT)	{ flags |= DT_RIGHT; flags &= ~DT_CENTER; }
		if (place & OOV_BOTTOM)	{ flags |= DT_BOTTOM; flags &= ~DT_VCENTER; }
		_R.left = b->X;
		_R.top = b->Y;
		_R.right = b->X + b->W;
		_R.bottom = b->Y + b->H;
		DrawText( rp->DC, t->Text, -1, &_R, flags );
#endif
		/*
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
		*/
		}
    return t->Text;
}

char *OOStr_Set( ULONG index, char *newstr, BOOL alloc )
{
  OOTextItem *t = &DefaultText[index];

	OOASSERT( index <= NumStrings );
	if (! (t->Flags & OOF_TEXT_FORCEUSE))
		{
		if (alloc == FALSE || newstr == NULL)
			{
			free_string( t );
			t->Text = newstr;
			}
		else{
			ULONG len = strlen( (char*)newstr );
			char *buf = (char*) OOPool_Alloc( OOGlobalPool, len+1 );
			if (buf != NULL)
				{
				free_string( t );
				strcpy( buf, newstr );
				t->Text = buf;
				t->Flags |= OOF_TEXT_ALLOCATED;
				}
			}
		}
	return t->Text;
}

BOOL OOStr_IsEqual( UBYTE *s1, UBYTE *s2, BOOL case_dependant )
{
	if (s1 == NULL || s2 == NULL)
		return (s1 == s2) ? TRUE : FALSE;

	return (case_dependant == TRUE) ? (strcmp( s1, s2 ) == 0 ? TRUE : FALSE) : (stricmp( s1, s2 ) == 0 ? TRUE : FALSE);
}

//---------------------------------------------------

static void free_string( OOTextItem *t )
{
	if (t->Text != NULL && (t->Flags & OOF_TEXT_ALLOCATED))
		{ 
		OOPool_Free( OOGlobalPool, t->Text );
		t->Flags &= ~OOF_TEXT_ALLOCATED;
		t->Text = NULL; 
		}
}

