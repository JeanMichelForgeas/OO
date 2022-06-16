/*****************************************************************
 *
 *       Project:    OO
 *       Function:   Init / Cleanup
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "oo_class.h"
#include "oo_tags.h"
#include "oo_init.h"
#include "oo_pool.h"
#include "oo_gfx.h"
#include "oo_text.h"
#include "oo_pictures.h"
#include "oo_class_prv.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>


#define OOD_WANT_CONSOLE


/******* Imported ************************************************/

extern void main();

extern OORESULT		OOResult;
extern void *		OOGlobalPool;
extern HINSTANCE	OOInstance;
extern int			OOCmdShow;


/******* Exported ************************************************/


/******* Statics *************************************************/

static OOClass *class_tab[] = {
	&Class_Application,
	&Class_HandleTimer,
	&Class_HandleGUI,
	&Class_AreaScreen,
	&Class_AreaWindow,
	&Class_AreaGroupBox,
	&Class_CtrlButton,
	&Class_CtrlScroller,
	&Class_CtrlChecker,
	&Class_CtrlRadio,
	&Class_CtrlDisplayNum,
	&Class_CtrlDisplayText,
	&Class_CtrlDisplayImage,
	&Class_CtrlEditNum,
	&Class_CtrlEditText,
	&Class_CtrlList,
	NULL
	};

#ifdef OOD_WANT_CONSOLE
static BOOL Console;
static int save_stdin ;
static int save_stdout;
static int save_stderr;
#endif

static char ProgName[256] = "\0";
static char* ArgV[3];


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

ULONG OO_InitV( ULONG attr1, ... )
{
	return OO_Init( (OOTagList*)&attr1 );
}

ULONG OO_Init( OOTagList *tl )
{
	OOClass **tab;

#ifdef OOD_WANT_CONSOLE
	if (OOTag_GetData( tl, OOA_Console, FALSE ) == TRUE)
		{
		if ((Console = AllocConsole()) == TRUE)
			{
			save_stdin  = _fileno( stdin  );
			save_stdout = _fileno( stdout );
			save_stderr = _fileno( stderr );
			_fileno( stdin  ) = _open_osfhandle( (long)GetStdHandle( STD_INPUT_HANDLE  ), _O_RDONLY );
			_fileno( stdout ) = _open_osfhandle( (long)GetStdHandle( STD_OUTPUT_HANDLE ), _O_APPEND );
			_fileno( stderr ) = _open_osfhandle( (long)GetStdHandle( STD_ERROR_HANDLE  ), _O_APPEND );
			}
		}
#endif

	// Create the global pool
#ifdef _DEBUG
	if ((OOGlobalPool = OOPool_Create( 4000, 0xFF )) == NULL)
#else
	if ((OOGlobalPool = OOPool_Create( 4000, 0 )) == NULL)
#endif
		return OOV_ERROR;

	// Initialize OO classes
	for (tab=class_tab; *tab != NULL; tab++)
		OO_InitClass( *tab );

	if (tl != NULL)
		{
		// Initialize client classes
		for (; ; tl++)
			{
			OOTAG_GETITEM(tl);
			if (tl == NULL) break;
			switch (tl->ti_Tag)
				{
				case OOT_InitClass	: OO_InitClass( tl->ti_Ptr ); break;
				case OOT_InitText	: OOStr_SetTable( tl->ti_Ptr ); break;
				case OOT_InitImages	: OOPic_SetTable( tl->ti_Ptr ); break;
				}
			}
		}

	if (OOGfx_Init() == OOV_ERROR)
		return OOV_ERROR;

	return OOV_OK;
}

void OO_Cleanup( void )
{
	OOGfx_Cleanup();

	if (OOGlobalPool != NULL)
		{
		ULONG count = OOPool_GetCount( OOGlobalPool );
		if (count) 
			{
			OOTRACE( "OOCleanup Warning : Pool alloc count = %d\n", count );
			}
		OOGlobalPool = OOPool_Delete( OOGlobalPool );
		}

#ifdef OOD_WANT_CONSOLE
	if (Console == TRUE) 
		{
		printf( "------------ Press RETURN..." );
		fflush( stdout );
		getchar();
		_fileno( stdin  ) = save_stdin ;
		_fileno( stdout ) = save_stdout;
		_fileno( stderr ) = save_stderr;
		FreeConsole();
		}
#endif
}

//---------------------------------------------------------------------

void OO_InitClass( OOClass *cl )
{
	OOClass *base;
	cl->InstOffset = 0;
	for (; (base = cl->BaseClass) != NULL; cl=base)
		{
		if (base->InstOffset == 0)
			OO_InitClass( base );
		cl->InstOffset = base->InstOffset + base->InstSize + base->AttrsSize;
		}
}

//---------------------------------------------------------------------

#ifdef _WIN32
#ifndef _CONSOLE

int WINAPI WinMain( HINSTANCE instance, HINSTANCE previnstance, PSTR cmdline, int cmdshow )
{
	OOInstance = instance;
	OOCmdShow = cmdshow;

	GetModuleFileName( GetModuleHandle(NULL), ProgName, 255 );
	ArgV[0] = (char*)ProgName;
	ArgV[1] = (char*)cmdline;
	ArgV[2] = (char*)NULL;
	main( (cmdline == NULL || cmdline[0] == 0) ? 1 : 2, ArgV );

	return 0;
}

#endif // _CONSOLE
#endif // _WIN32
