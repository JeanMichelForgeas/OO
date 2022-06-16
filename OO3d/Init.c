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
#include "oo_3d_prv.h"


/******* Imported ************************************************/

extern void OOMath_SqrtInit();
extern BOOL OO3DType_Init( void );
extern void OO3DType_Cleanup( void );


/******* Exported ************************************************/


/******* Statics *************************************************/

static BOOL InitDone = FALSE;

static OOClass *class_tab[] = {
	&Class_Handle3D,
	&Class_3DSpace,
	&Class_3DSolid,
	&Class_3DScreen,
	&Class_3DCamera,
	&Class_3DSpeaker,
	&Class_3DMike,
	//&Class_3DSoleil,
	//&Class_3DLight,
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

ULONG OO_Init3D( OOTagList *tl )
{
	OOClass **tab;

	if (InitDone == TRUE) return OOV_OK;
	
	OOMath_SqrtInit();

	// Initialize OO classes
	for (tab=class_tab; *tab != NULL; tab++)
		OO_InitClass( *tab );

	// Init de la gestion des types d'objets
	if (OO3DType_Init() == FALSE) return OOV_ERROR;

	InitDone = TRUE;

	return OOV_OK;
}

void OO_Cleanup3D( void )
{
	if (InitDone == TRUE)
		{
		OO3DType_Cleanup();
		InitDone = FALSE;
		}
}

#endif // _WIN32
//==========================================================================
//==========================================================================
//==========================================================================
//								AMIGA
//==========================================================================
//==========================================================================
//==========================================================================
#ifdef AMIGA

ULONG OO_Init3D( OOTagList *tl )
{
	OOClass **tab;

	if (InitDone == TRUE) return OOV_OK;
	
	// Initialize OO classes
	for (tab=class_tab; *tab != NULL; tab++)
		OO_InitClasses( *tab );

	// Init de la gestion des types d'objets
	if (OO3DType_Init() == FALSE) return OOV_ERROR;

	InitDone = TRUE;

	return OOV_OK;
}

void OO_Cleanup3D( void )
{
	if (InitDone == TRUE)
		{
		OO3DType_Cleanup();
		InitDone = FALSE;
		}
}

#endif // AMIGA
