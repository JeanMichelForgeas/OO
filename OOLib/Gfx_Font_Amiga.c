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

#include <clib/diskfont_protos.h>


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/


/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

ULONG OOGfx_FontInit( void )
{
    return OOV_OK;
}

void OOGfx_FontCleanup( void )
{
}


/***************************************************************************
 *
 *		Fonts
 *
 ***************************************************************************/

ULONG OOGfx_CharWidth( OOFont *font, UBYTE c )
{
  UWORD *wptr;
  ULONG width;

	OOASSERT( font != NULL && font->Name != NULL );
    if ((c > font->Font->tf_HiChar) || ((c -= font->Font->tf_LoChar) < 0))
        c = font->Font->tf_HiChar - font->Font->tf_LoChar + 1;

    if (wptr = font->Font->tf_CharSpace) width = wptr[c]; else width = font->Font->tf_XSize;
    if (wptr = font->Font->tf_CharKern) { if (wptr[c] != 0xffff) width += wptr[c]; }
    return width;
}

ULONG OOGfx_StringWidth( OOFont *font, UBYTE *str )
{
  ULONG width=0;

	OOASSERT( font != NULL && font->Name != NULL );
    if (str)
        {
        if (font->Font->tf_Flags & FPF_PROPORTIONAL)
            { while (*str) width += OOGfx_CharWidth( font, *str++ ); }
        else width = strlen( str ) * font->Font->tf_XSize;
        }
    return width;
}

//------------------------------------------------

OOFont * OOGfx_OpenFontV( OOFont *model, ULONG attr, ... )
{
	return OOGfx_OpenFont( model, (OOTagList*)&attr );  
}

OOFont * OOGfx_OpenFontInfo( OOFont *font )
{
	struct TextFont *f;

	OOASSERT( font != NULL );

	... mettre à jour font->_TTA.tta_Style avec les flags OOF_FONT_...

    if (f = OpenFont( &font->_TTA ))
        {
        if (f->tf_YSize != tta->tta_YSize)
            { CloseFont( f ); f = NULL; }
        }
    if (f == NULL)
        {
        font->_TTA.tta_Flags &= ~FPF_ROMFONT;
        font->_TTA.tta_Flags |= FPF_DISKFONT|FPF_PROPORTIONAL;
        f = OpenDiskFont( &font->_TTA );
        }
	if (f == NULL)
		return NULL;

	if (font->Font != NULL && font->Flags & OOF_FONT_OPENED)
		CloseFont( font->Font );
	font->Font = f;
	font->Flags |= OOF_FONT_OPENED;
    return font;
}

void OOGfx_CloseFont( OOFont *font )
{
	if (font->Font != NULL && font->Flags & OOF_FONT_OPENED)
		{
		CloseFont( font->Font );
		font->Flags &= ~OOF_FONT_OPENED;
		}
	font->Font = NULL;
	if (font->_TTA.tta_Name != NULL && font->Flags & OOF_FONT_NAMEALLOCATED)
		{
		OOPool_Free( OOGlobalPool, font->_TTA.tta_Name );
		font->Flags &= OOF_FONT_NAMEALLOCATED;
		}
	font->_TTA.tta_Name = NULL;
}

BOOL OOGfx_GetFontInfo( OOFont *font )
{
	OOASSERT( font != NULL && font->Font != NULL );

    font->_TTA.tta_Name  = font->Font->tf_Message.mn_Node.ln_Name;
    font->_TTA.tta_YSize = font->Font->tf_YSize;
    font->_TTA.tta_Style = font->Font->tf_Style;
    font->_TTA.tta_Flags = font->Font->tf_Flags;
    font->_TTA.tta_Tags  = 0;

	font->Flags &= OOF_FONT_OPENED; // On enlève tout sauf l'indication d'ouverture du font
	if (font->Font->tf_Flags & FPF_PROPORTIONAL) 
		 font->Flags |= OOF_FONT_VARIABLE;
	else font->Flags |= OOF_FONT_FIXED;

    if (font->_TTA.tta_Flags & FSF_EXTENDED)
        font->_TTA.tta_Tags = ((struct TextFontExtension *)font->Font->tf_Extension)->tfe_Tags;
	return TRUE;
}

OOFont * OOGfx_OpenFont( OOFont *font, OOTagList *taglist )
{
	OOFont _NF;
	OOFont *nf, *newfont = NULL;

	if (font == NULL)
		{
		if (nf = newfont = (OOFont*) OOPool_Alloc( OOGlobalPool, sizeof(OOFONT) )) == NULL) return NULL;
		nf->Font			= NULL;
		nf->_TTA.tta_Name	= NULL;
		nf->_TTA.tta_YSize	= 0;
		nf->_TTA.tta_Style	= 0;
		nf->_TTA.tta_Flags	= 0;
		nf->_TTA.tta_Tags	= NULL;
		nf->Flags			= 0;
		}
	else{
		nf = &NF;
		nf->Font	= font->Font;
		nf->_TTA	= font->_TTA;
		nf->Flags	= font->Flags;
		}			

	if (taglist != NULL)
		{
		ULONG pc = 0;
		OOTAG_FOR_EACH_ITEM(taglist,ti)
			switch (ti->ti_Tag)
				{
				case OOA_FontName		: nf->_TTA.tta_Name = ti->ti_String; nf->Flags &= ~OOF_FONT_NAMEALLOCATED~; break;
				case OOA_FontHeight		: nf->_TTA.tta_YSize = ti->ti_Long; break;
				case OOA_FontHeightP100	: pc = ti->ti_Long; break;
				case OOA_FontBold		: if (ti->ti_Bool == TRUE) nf->Flags |= OOF_FONT_BOLD		; else nf->Flags &= ~OOF_FONT_BOLD		; break;
				case OOA_FontItalic		: if (ti->ti_Bool == TRUE) nf->Flags |= OOF_FONT_ITALIC		; else nf->Flags &= ~OOF_FONT_ITALIC	; break;
				case OOA_FontUnderline	: if (ti->ti_Bool == TRUE) nf->Flags |= OOF_FONT_UNDERLINE	; else nf->Flags &= ~OOF_FONT_UNDERLINE	; break;
				case OOA_FontFixed		: if (ti->ti_Bool == TRUE) nf->Flags |= OOF_FONT_FIXED		; else nf->Flags &= ~OOF_FONT_FIXED		; break;
				case OOA_FontVariable	: if (ti->ti_Bool == TRUE) nf->Flags |= OOF_FONT_VARIABLE	; else nf->Flags &= ~OOF_FONT_VARIABLE	; break;
				}
		OOTAG_FOR_EACH_ITEM_END
		if (pc != 0) nf->_TTA.tta_YSize = (nf->_TTA.tta_YSize * pc) / 100;
		if (nf->Flags & OOF_FONT_VARIABLE) font->_TTA.tta_Flags |= FPF_PROPORTIONAL; else font->_TTA.tta_Flags &= ~FPF_PROPORTIONAL;
		}

	if (nf->Name == NULL)
		{
		if ((nf->Font = ... GetStockObject( nf->Flags & OOF_FONT_VARIABLE ? SYSTEM_FONT : SYSTEM_FIXED_FONT )) == NULL) goto END_ERROR;
		if (OOGfx_GetFontInfo( nf ) == NULL) goto END_ERROR;
		*font = *nf;
		return font; 
		//______
		}

	if (nf->Height == 0) goto END_ERROR;
	if (nf->_TTA.tta_Name == NULL || nf->_TTA.tta_YSize == 0) goto END_ERROR;

	if (font == NULL)
		{
		if (OOGfx_OpenFontInfo( newfont ) == NULL) goto END_ERROR;
		font = newfont;
		}
	else{
		if ((nf->_TTA.tta_YSize != font->_TTA.tta_YSize)
		|| ((nf->Flags & OOM_FONT_ATTRIBUTES) != (font->Flags & OOM_FONT_ATTRIBUTES))
		|| (font->_TTA.tta_Name == NULL)
		|| (strcmp( nf->_TTA.tta_Name, font->_TTA.tta_Name ) != 0))
			{
			if (OOGfx_OpenFontInfo( nf ) == NULL) goto END_ERROR;
			// Si plus besoin du nom alloué qui va être perdu, le libérer avant
			if ((nf->_TTA.tta_Name != font->_TTA.tta_Name) && (font->_TTA.tta_Name != NULL) && (font->Flags & OOF_FONT_NAMEALLOCATED))
				OOPool_Free( OOGlobalPool, font->_TTA.tta_Name );
			*font = *nf;
			}
		}
	return font;

END_ERROR:
	if (newfont != NULL) OOPool_Free( OOGlobalPool, newfont );
    return NULL;
}
