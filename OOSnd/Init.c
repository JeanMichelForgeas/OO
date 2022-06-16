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

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_init.h"
#include "oo_sound_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static BOOL InitDone = FALSE;

static OOClass *class_tab[] = {
	&Class_HandleSound,
	&Class_Sound,
	NULL
	};


//==========================================================================
//==========================================================================
//==========================================================================
//							WIN32 - DIRECTSOUND
//==========================================================================
//==========================================================================
//==========================================================================
#ifdef _WIN32
#ifdef OOV_WANTED_DIRECTSOUND

#include <mmsystem.h>
#include "oo_gui_prv.h"

LPDIRECTSOUND DirectSoundObject = NULL;

static FARPROC		Func_DirectSoundCreate = NULL;
static HINSTANCE	DirectSoundDLL = NULL;
static OObject		**AreaAddr = NULL;
static BOOL			SillyDone = FALSE;

ULONG OO_InitSound( OOTagList *tl )
{
	OOClass **tab;
	ULONG errmode;

	if (InitDone == TRUE) return OOV_OK;
	
	// Initialize OO classes
	for (tab=class_tab; *tab != NULL; tab++)
		OO_InitClass( *tab );

	// Initialize DirectSound
	//DirectSoundCreate( NULL, &DirectSoundObject, NULL );
	//DirectSoundObject->SetCooperativeLevel( win, DSSCL_NORMAL ))

	errmode = SetErrorMode( SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS );
	DirectSoundDLL = LoadLibrary( "DSOUND.DLL" );
	SetErrorMode( errmode );
	if (DirectSoundDLL != NULL)
		{
		if ((Func_DirectSoundCreate = GetProcAddress( DirectSoundDLL, "DirectSoundCreate" )) != NULL)
			{
			if (Func_DirectSoundCreate( NULL, &DirectSoundObject, NULL ) == DS_OK)
				{
				AreaAddr = (OObject**) OOTag_GetData( tl, OOA_AreaAddr, (ULONG)NULL );
				SillyDone = FALSE;
				InitDone = TRUE;
				return OOV_OK;
				}
			}
		FreeLibrary( DirectSoundDLL );
		}
	return OOV_ERROR;
}

ULONG OO_InitSillyDirectSound( void )
{
	OORenderInfo *ri;
	OObject *area;

	if (SillyDone == TRUE) return OOV_OK;

	if ((area = *AreaAddr) != NULL && (ri = (void*)OO_GetAttr( area, OOA_RenderInfo )) != NULL)
		{
		IDirectSound_SetCooperativeLevel( DirectSoundObject, ri->wi, DSSCL_NORMAL );
		SillyDone = TRUE;
		return OOV_OK;
		}
	return OOV_ERROR;
}

void OO_CleanupSound( void )
{
	if (InitDone == TRUE)
		{
		if (DirectSoundDLL != NULL)
			{
			if (DirectSoundObject != NULL)
				{
				while (IDirectSound_Release( DirectSoundObject ));
				DirectSoundObject = NULL;
				}
			FreeLibrary( DirectSoundDLL );
			DirectSoundDLL = NULL;
			}
		InitDone = FALSE;
		}
}

#endif // OOV_WANTED_DIRECTSOUND
#endif // _WIN32
//==========================================================================
//==========================================================================
//==========================================================================
//								AMIGA
//==========================================================================
//==========================================================================
//==========================================================================
#ifdef AMIGA

ULONG OO_InitSound( OOTagList *tl )
{
	OOClass **tab;

	if (InitDone == TRUE) return OOV_OK;
	
	// Initialize OO classes
	for (tab=class_tab; *tab != NULL; tab++)
		OO_InitClasses( *tab );

	InitDone = TRUE;

	return OOV_OK;
}

void OO_CleanupSound( void )
{
	if (InitDone == TRUE)
		{
		InitDone = FALSE;
		}
}

#endif // AMIGA
