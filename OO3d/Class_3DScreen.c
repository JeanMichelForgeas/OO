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

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_gui_prv.h"
#include "oo_3d_prv.h"
#include "oo_pool.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#define	INDEX_OOA_OnOff			0
#define	INDEX_D3A_ConnectObj	1
#define	INDEX_OOA_AreaAddr		2
#define	INDEX_D3A_FillColor		3
#define	INDEX_D3A_DrawnBy		4

#define OOGLCOLORBITS	8


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG add_screen( OObject *cam );
static ULONG remove_screen( OObject *cam );

static ULONG PrepareScreen( OObject *obj, OOTagList *attrlist );
static void CleanupScreen( OObject *obj );
static OOGLSupport* CreateGLWindow( OObject *obj, OOInst_3DScreen *inst, OORenderInfo *ri );
static void KillGLWindow( OOGLSupport **suppaddr );
static void ClearScreen( OObject *obj, OOTagList *attrlist );
static void SyncRefresh( OOInst_3DScreen *inst, OOTagList *attrlist );


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_3DScreen( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //---------------
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
                OOInst_3DScreen *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				inst->MemRP = NULL;
				// Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
            OOInst_3DScreen *inst = OOINST_DATA(cl,obj);
			CleanupScreen( obj );
			}
			return OO_DoBaseMethod( cl, obj, method, attrlist );

        //---------------
        case OOM_GET:
			// Do not call base method
            return OOPrv_MGet( cl, obj, attrlist );

        case OOM_SET:
			// Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

        //---------------
		case D3M_START3D:
			OO_DoBaseMethod( cl, obj, method, attrlist );
			if (PrepareScreen( obj, attrlist ) == OOV_ERROR) return OOV_ERROR;
			add_screen( obj );
			break;

		case D3M_STOP3D:
			remove_screen( obj );
			OO_DoBaseMethod( cl, obj, method, attrlist );
			break;

        //---------------
		case D3M_CLEARSCREEN:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort );
			if (OOINST_ATTR(cl,obj,INDEX_OOA_OnOff) == TRUE)
				{
				ClearScreen( obj, attrlist );
				}
			break;

		case D3M_OBJTOSCREEN:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort && attrlist[1].ti_Tag == D3A_ObjTable && attrlist[2].ti_Tag == D3A_ObjCount );
			if (OOINST_ATTR(cl,obj,INDEX_OOA_OnOff) == TRUE)
				{
				OOInst_3DScreen *inst = OOINST_DATA(cl,obj);
				OObject **connectobj = (OObject**) OOINST_ATTR(cl,obj,INDEX_D3A_ConnectObj);
				void *save_rastport = attrlist[0].ti_Ptr; 
				attrlist[0].ti_Ptr = inst->MemRP; 
				if (connectobj != NULL && *connectobj != NULL)
					OO_DoMethod( *connectobj, D3M_OBJTOCAMERA, attrlist ); // Seules les cameras ont cette méthode
				attrlist[0].ti_Ptr = save_rastport; 
				}
			break;

		case D3M_DRAWSCREEN:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort );
			if (OOINST_ATTR(cl,obj,INDEX_OOA_OnOff) == TRUE)
				{
				OOInst_3DScreen *inst = OOINST_DATA(cl,obj);
				OOTagList TL[3];
				OObject **connectobj = (OObject**) OOINST_ATTR(cl,obj,INDEX_D3A_ConnectObj);
				if (connectobj != NULL && *connectobj != NULL)
					{
					TL[0].ti_Tag32 = D3A_RastPort;		TL[0].ti_Ptr = inst->MemRP;
					TL[1].ti_Tag32 = OOA_WidthHeight;	TL[1].ti_Data = W2L(inst->DestSizeX,inst->DestSizeY);
					TL[3].ti_Tag32 = OOV_TAG_END;		TL[2].ti_Ptr  = NULL;
					OO_DoMethod( *connectobj, D3M_DRAW, TL );
					}
				}
			break;

		case D3M_SYNCSCREEN:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == D3A_RastPort );
			if (OOINST_ATTR(cl,obj,INDEX_OOA_OnOff) == TRUE)
				{
				SyncRefresh( OOINST_DATA(cl,obj), attrlist );
				}
			break;

		//---------------
        default:
            return OO_DoBaseMethod( cl, obj, method, attrlist );
        }
    return OOV_OK;
}

/***************************************************************************
 *
 *      Utilities
 *
 ***************************************************************************/

static ULONG add_screen( OObject *obj )
{
	OObject *space = OO_GetParentOfClass( obj, &Class_3DSpace );
	if (space != NULL) 
		return OO_DoMethodV( space, D3M_ADDSCREEN, D3A_ObjScreen, obj, TAG_END );
	return OOV_ERROR;
}

static ULONG remove_screen( OObject *obj )
{
	OObject *space = OO_GetParentOfClass( obj, &Class_3DSpace );
	if (space != NULL) 
		return OO_DoMethodV( space, D3M_REMOVESCREEN, D3A_ObjScreen, obj, TAG_END );
	return OOV_ERROR;
}

//-------------------------------------------------------------- 

static ULONG PrepareScreen( OObject *obj, OOTagList *attrlist )
{
	OOInst_3DScreen *inst = OOINST_DATA(&Class_3DScreen,obj);
	OOTagList *attrs = OOINST_ATTRS(&Class_3DScreen,obj);
	OObject **areaaddr = attrs[INDEX_OOA_AreaAddr].ti_Ptr;

	if (areaaddr != NULL && *areaaddr != NULL)
		{
		OORenderInfo *ri=0;
		OOBox _B;
		if (OO_GetAttrsV( *areaaddr, OOA_LeftTop, &_B.X, OOA_WidthHeight, (ULONG)&_B.W, OOA_RenderInfo, &ri, TAG_END ))
			if (ri != NULL && ri->rp != NULL)
				{
				OObject *win = (OObject*) GetWindowLong( ri->wi, GWL_USERDATA );
				OOInst_Area *ar = _DATA_AREA(win);

				inst->DestOffsX = _B.X + ar->_Border.Left; // Coordonnées dans la fenêtre 
				inst->DestOffsY = _B.Y + ar->_Border.Top;
				inst->DestSizeX = _B.W;		// Dimensions de l'area où blitter 
				inst->DestSizeY = _B.H;

				// Réalloue une nouvelle surface de dessin
				CleanupScreen( obj );

				if ((inst->MemRP = OOGfx_AllocBMapRPort( ri->rp->Palette, OOGLCOLORBITS, _B.W, _B.H, 0 )) == NULL) 
					return OOV_ERROR;

				switch (OOINST_ATTR(&Class_3DScreen,obj,INDEX_D3A_DrawnBy))
					{
					case OOV_RP_DRAWNBY_SYS		:
					case OOV_RP_DRAWNBY_HAND	:
						inst->DestRP = ri->rp; 		// RastPort de la fenêtre
						break;

					case OOV_RP_DRAWNBY_DIRECTX	:
						break;

					case OOV_RP_DRAWNBY_OPENGL	:
						if ((inst->MemRP->Context = CreateGLWindow( obj, inst, ri )) != NULL)
							{
							glViewport( 0, 0, inst->DestSizeX, inst->DestSizeY );	// Reset The Current Viewport

							glMatrixMode( GL_PROJECTION );	// Select The Projection Matrix
							glLoadIdentity();				// Reset The Projection Matrix
							// Calculate The Aspect Ratio Of The Window
							gluPerspective( 45.0f, (GLfloat)inst->DestSizeX/(GLfloat)inst->DestSizeY, 0.1f, 100.0f );

							glMatrixMode( GL_MODELVIEW );	// Select The Modelview Matrix
							glLoadIdentity();				// Reset The Modelview Matrix

							glShadeModel( GL_FLAT );				// Enable Smooth Shading
							glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );	// Black Background
							glClearDepth( 1.0f );					// Depth Buffer Setup
							glEnable( GL_DEPTH_TEST );				// Enables Depth Testing
							glDepthFunc( GL_LEQUAL );				// The Type Of Depth Testing To Do
							glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );	// Really Nice Perspective Calculations
							}
						break;
					}
				inst->MemRP->DrawnBy = (UBYTE) OOINST_ATTR(&Class_3DScreen,obj,INDEX_D3A_DrawnBy);
				}
		}
	return OOV_OK;
}

static void CleanupScreen( OObject *obj )
{
	OOInst_3DScreen *inst = OOINST_DATA(&Class_3DScreen,obj);

	if (inst->MemRP != NULL)
		{
		switch (inst->MemRP->DrawnBy)
			{
			case OOV_RP_DRAWNBY_SYS		:
			case OOV_RP_DRAWNBY_HAND	:
				break;

			case OOV_RP_DRAWNBY_DIRECTX	:
				break;

			case OOV_RP_DRAWNBY_OPENGL	:
				{
				OOTagList *attrs = OOINST_ATTRS(&Class_3DScreen,obj);
				OObject **areaaddr = attrs[INDEX_OOA_AreaAddr].ti_Ptr;

				if (areaaddr != NULL && *areaaddr != NULL)
					KillGLWindow( (OOGLSupport**)&inst->MemRP->Context );
				}
				break;
			}

		OOGfx_FreeBMapRPort( inst->MemRP );
		inst->MemRP = NULL;
		}
}

static OOGLSupport* CreateGLWindow( OObject *obj, OOInst_3DScreen *inst, OORenderInfo *ri )
{
	static PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
		{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		OOGLCOLORBITS,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
		};

	GLuint PixelFormat;
	OOGLSupport *supp;

	if ((supp = OOPool_Alloc( OOGlobalPool, sizeof(OOGLSupport) )) != NULL)
		{
		supp->Wnd = NULL;
		supp->RC = NULL;
		supp->DC = NULL;

		if ((supp->Wnd = CreateWindowEx( 0,			// Extended Style For The Window
								OOWINCLASSNAME,		// Class Name
								NULL,				// Window Title
								WS_CHILD,			// Required Window Style
								inst->DestOffsX, inst->DestOffsY, inst->DestSizeX, inst->DestSizeY,// Selected Width And Height
								ri->wi,				// No Parent Window
								NULL,				// No Menu
								OOInstance,			// Instance
								NULL )) != NULL)	// Dont Pass Anything To WM_CREATE
			{
			if ((supp->DC = GetDC( supp->Wnd )) == NULL) // Did We Get A Device Context?
				MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			else{
				if ((PixelFormat = ChoosePixelFormat( supp->DC, &pfd )) == 0)	// Did Windows Find A Matching Pixel Format?
					MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
				else{
					if (SetPixelFormat( supp->DC, PixelFormat, &pfd ) == FALSE)		// Are We Able To Set The Pixel Format?
						MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
					else{
						if ((supp->RC = wglCreateContext( supp->DC )) == NULL)	// Are We Able To Get A Rendering Context?
							MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
						else{
							if (wglMakeCurrent( supp->DC, supp->RC ) == FALSE)	// Try To Activate The Rendering Context
								MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
							else{
								ShowWindow( supp->Wnd, SW_SHOW );
								DeleteDC( inst->MemRP->DC );
								inst->MemRP->Flags -= OOF_RP_DC_ALLOCATED;
								inst->MemRP->DC = supp->DC;
								inst->DestRP = inst->MemRP; // RastPort de la fenêtre

								return supp;
								}
							}
						}
					}
				}
			}		
		}
	KillGLWindow( &supp );
		
	return supp;
}

static void KillGLWindow( OOGLSupport **suppaddr )
{
	OOGLSupport *supp = (OOGLSupport*)*suppaddr;

	if (supp != NULL)
		{
		if (supp->Wnd != NULL)
			{
			if (supp->DC != NULL)
				{
				if (supp->RC != NULL)
					{
					if (wglMakeCurrent( NULL, NULL ))
						wglDeleteContext( supp->RC );
					}
				else
					ReleaseDC( supp->Wnd, supp->DC );
				}
			DestroyWindow( supp->Wnd );
			}
		OOPool_Free( OOGlobalPool, supp );
		*suppaddr = NULL;
		}
}

static void ClearScreen( OObject *obj, OOTagList *attrlist )
{
	OOInst_3DScreen *inst = OOINST_DATA(&Class_3DScreen,obj);
	OOTagList *attrs = OOINST_ATTRS(&Class_3DScreen,obj);
	OORastPort *rp = inst->MemRP;

	switch (inst->MemRP->DrawnBy)
		{
		case OOV_RP_DRAWNBY_SYS		:
		case OOV_RP_DRAWNBY_HAND	:
			{
			OOPoint pt[5] = { 
				0, 0,  
				0, inst->DestSizeY-1,
				inst->DestSizeX-1, inst->DestSizeY-1,
				inst->DestSizeX-1, 0,
				0, 0
				};
			OOPolygon P = { 
				4, 			// NumPoints
				OOF_FILL, 	// Flags   
				0, 			// LineColor
				(UWORD)attrs[INDEX_D3A_FillColor].ti_Data, // FillColor
				NULL, 		// Texture
				pt 			// Points
				};
			OOGfx_DrawPolygon( rp, &P, NULL );
			}
			break;

		case OOV_RP_DRAWNBY_DIRECTX	:
			break;

		case OOV_RP_DRAWNBY_OPENGL	:
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// Clear Screen And Depth Buffer
			break;
		}
}

static void SyncRefresh( OOInst_3DScreen *inst, OOTagList *attrlist )
{
	OORastPort *rp = inst->MemRP;

	switch (inst->MemRP->DrawnBy)
		{
		case OOV_RP_DRAWNBY_SYS		:
		case OOV_RP_DRAWNBY_HAND	:
			#ifdef AMIGA	
				BltBitMapRastPort( rp->BitMap, 0, 0,
								   inst->DestRP, inst->DestOffsX, inst->DestOffsY,
								   inst->DestSizeX, inst->DestSizeY, 0xC0 );
			#endif
			#ifdef _WIN32
				BitBlt( inst->DestRP->DC, inst->DestOffsX, inst->DestOffsY,
					inst->DestSizeX, inst->DestSizeY,
					rp->DC, 0, 0,
					SRCCOPY
					);
			#endif
			break;

		case OOV_RP_DRAWNBY_DIRECTX	:
			break;

		case OOV_RP_DRAWNBY_OPENGL	:
			SwapBuffers( ((OOGLSupport*)(inst->MemRP->Context))->DC );	// Swap Buffers (Double Buffering)
			break;
		}
}


/***************************************************************************
 *
 *      Attributes setting
 *
 ***************************************************************************/

// Trop tomatique


/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_OnOff		, TRUE		 ,	"OnOff"		, NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_ConnectObj, (ULONG)NULL,	"ConnectObj", NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_AreaAddr	, (ULONG)NULL,	"AreaAddr"	, NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_FillColor	, (ULONG)NULL,	"FillColor"	, NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { D3A_DrawnBy	, OOV_RP_DRAWNBY_OPENGL, "DrawnBy"	, NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }
	};

OOClass Class_3DScreen = { 
	Dispatcher_3DScreen, 
	&Class_3DSolid,			// Base class
	"Class_3DScreen",		// Class Name
	0,						// InstOffset
	sizeof(OOInst_3DScreen),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
