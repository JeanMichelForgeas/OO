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

static HDC SpareDC = NULL;


/***************************************************************************
 *
 *      Init / Cleanup
 *
 ***************************************************************************/

ULONG OOGfx_FontInit( void )
{
	if ((SpareDC = CreateCompatibleDC(NULL)) == NULL) return OOV_ERROR;
    return OOV_OK;
}

void OOGfx_FontCleanup( void )
{
	if (SpareDC != NULL) DeleteObject( SpareDC );
}


/***************************************************************************
 *
 *		Fonts
 *
 ***************************************************************************/

void OOGfx_GetSizeOfString( OOFont *font, UBYTE *str, ULONG lenstr, OOSize *size )
{
	SIZE _S;
	OOASSERT( font != NULL && str != NULL && size != NULL );
	font = SelectObject( SpareDC, font );
	if (lenstr == -1) lenstr = strlen( str );
	GetTextExtentPoint32( SpareDC, str, lenstr, &_S );
	SelectObject( SpareDC, font );
	size->W = (UWORD)_S.cx;
	size->H = (UWORD)_S.cy;
}

//------------------------------------------------

OOFont *OOGfx_OpenFontV( OOFont *model, ULONG attr, ... )
{
	return OOGfx_OpenFont( model, (OOTagList*)&attr );  
}

// Always try to open a font. Obtain missing info from system font
OOFont *OOGfx_OpenFont( OOFont *model, OOTagList *taglist )
{
	LOGFONT _LF, _CMP;
	OOFont *font = NULL;
	BOOL variable = TRUE;

	//------ Needed info first
	if (taglist != NULL)
		{
		memset( &_CMP, 0, sizeof(LOGFONT) );
		OOGfx_TagListToInfo( taglist, &_CMP );
		if (_CMP.lfPitchAndFamily & FIXED_PITCH) variable = FALSE;
		}

	//------ Get default font info
	if (model == NULL)
		if ((model = GetStockObject( (variable == TRUE) ? ANSI_VAR_FONT : ANSI_FIXED_FONT )) == NULL) goto END_ERROR;
	if (OOGfx_FontToInfo( model, &_LF ) == FALSE) goto END_ERROR;

	//------ Change info with taglist parameters
	if (taglist != NULL)
		{
		// Examine if open needed
		if ((_CMP.lfFaceName[0] == 0 || strcmp( _LF.lfFaceName, _CMP.lfFaceName ) == 0)
		 && (_CMP.lfHeight == 0 || _LF.lfHeight == _CMP.lfHeight)
		 && (_CMP.lfWeight == 0 || _LF.lfWeight == _CMP.lfWeight)
		 && _LF.lfItalic == _CMP.lfItalic
		 && _LF.lfUnderline == _CMP.lfUnderline
		 && (_CMP.lfPitchAndFamily == 0 || _LF.lfPitchAndFamily == _CMP.lfPitchAndFamily))
			return model;
		// Yes open needed, update fontinfo to open new font
		OOGfx_TagListToInfo( taglist, &_LF );
		}

	//------ Open the required font if necessary
	font = CreateFontIndirect( &_LF );
	return font;
END_ERROR:
	return model;
}

void OOGfx_CloseFont( OOFont *font )
{
	if (font != NULL) DeleteObject( font );
}

// Fills all present tags with font information
BOOL OOGfx_FontToTagList( OOFont *font, OOTagList *taglist )
{
	OOTagItem *tiname=NULL, *tiheight=NULL, *tiheightp100=NULL, *tibold=NULL, *tiitalic=NULL, *tiunder=NULL, *tifixed=NULL, *tivariable=NULL;
	UBYTE *mem=NULL;
	LOGFONT _LF;

	OOASSERT( taglist != NULL && font != NULL);
	OOTAG_FOR_EACH_ITEM(taglist,ti)
		switch (ti->ti_Tag)
			{
			case OOA_FontName		: tiname		= ti; break;
			case OOA_FontHeight		: tiheight		= ti; break;
			case OOA_FontHeightP100	: tiheightp100	= ti; break;
			case OOA_FontBold		: tibold		= ti; break;
			case OOA_FontItalic		: tiitalic		= ti; break;
			case OOA_FontUnderline	: tiunder		= ti; break;
			case OOA_FontFixed		: tifixed		= ti; break;
			case OOA_FontVariable	: tivariable	= ti; break;
			}
	OOTAG_FOR_EACH_ITEM_END

	if (OOGfx_FontToInfo( font, &_LF ) == FALSE) goto END_ERROR;

	if (tiname != NULL)
		{
		if (OOTag_SetString( tiname, _LF.lfFaceName ) == NULL) goto END_ERROR;
		}
	if (tiheight != NULL)
		{
		ULONG pc = (tiheightp100 == 0) ? 100 : tiheightp100->ti_Data;
		tiheight->ti_Data = (_LF.lfHeight * 100) / pc;
		}
	if (tibold		!= NULL) tibold		->ti_Bool = (_LF.lfWeight		 >= FW_BOLD		  ) ? TRUE : FALSE;	
	if (tiitalic	!= NULL) tiitalic	->ti_Bool = (_LF.lfItalic		 != 0			  ) ? TRUE : FALSE;
	if (tiunder		!= NULL) tiunder	->ti_Bool = (_LF.lfUnderline	 != 0			  ) ? TRUE : FALSE;
	if (tifixed		!= NULL) tifixed	->ti_Bool = (_LF.lfPitchAndFamily & FIXED_PITCH	  ) ? TRUE : FALSE;
	if (tivariable	!= NULL) tivariable	->ti_Bool = (_LF.lfPitchAndFamily & VARIABLE_PITCH) ? TRUE : FALSE;

	return TRUE;

END_ERROR:
    return FALSE;
}

BOOL OOGfx_FontToInfo( OOFont *font, void *fontinfo )
{
	OOASSERT( font != NULL && fontinfo != NULL);
	if (GetObject( font, sizeof(LOGFONT), fontinfo ) == 0) return FALSE;
	return TRUE;
}

// Met à jour seulement les éléments pour lesquels un Tag existe
void OOGfx_TagListToInfo( OOTagList *taglist, void *fontinfo )
{
	LOGFONT *lf = fontinfo;
	ULONG pc = 100;

	OOASSERT( taglist != NULL && fontinfo != NULL);
	OOTAG_FOR_EACH_ITEM(taglist,ti)
		switch (ti->ti_Tag)
			{
			case OOA_FontName		: if (ti->ti_String != NULL) strcpy( lf->lfFaceName, ti->ti_String ); break;
			case OOA_FontHeight		: if (ti->ti_Data != 0) lf->lfHeight = ti->ti_Data; break;
			case OOA_FontHeightP100	: pc = ti->ti_Data; break;
			case OOA_FontBold		: lf->lfWeight = (ti->ti_Bool == TRUE) ? FW_BOLD : FW_NORMAL; break;
			case OOA_FontItalic		: lf->lfItalic = (ti->ti_Bool == TRUE) ? TRUE : FALSE; break;
			case OOA_FontUnderline	: lf->lfUnderline = (ti->ti_Bool == TRUE) ? TRUE : FALSE; break;
			case OOA_FontFixed		: lf->lfPitchAndFamily = (ti->ti_Bool == TRUE) ? ((lf->lfPitchAndFamily|FIXED_PITCH) & ~VARIABLE_PITCH) : (lf->lfPitchAndFamily & ~VARIABLE_PITCH); break;
			case OOA_FontVariable	: lf->lfPitchAndFamily = (ti->ti_Bool == TRUE) ? ((lf->lfPitchAndFamily|VARIABLE_PITCH) & ~FIXED_PITCH) : (lf->lfPitchAndFamily & ~FIXED_PITCH); break;
			}
	OOTAG_FOR_EACH_ITEM_END
	if (pc != 0) lf->lfHeight = (lf->lfHeight * pc) / 100;
}
