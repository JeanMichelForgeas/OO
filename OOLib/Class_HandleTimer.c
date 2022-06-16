/*****************************************************************
 *
 *		 Project:	 OO
 *		 Function:	 
 *
 *		 Created:		 Jean-Michel Forgeas
 *
 *		 This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "oo_pool.h"

#include "oo_class_prv.h"

#define INDEX_OOA_OnOff	0


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG HandleTimerEvent( OObject *obj, ULONG method, OOTagList *attrlist );
static struct _HandleTimerNode *search_timer( ULONG timer );
static void free_timers( void );
static void free_timer( OONode *node );

/*
#ifdef _WIN32
static OObject *HTObj; // A cause de WinMerdouille
static VOID CALLBACK timer_proc( HWND hwnd, UINT msgid, UINT timerid, DWORD time );
#endif
*/

static OOList TimersList;


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_HandleTimer( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			{
			OOTagItem init_taglist[] = { OOA_EventMask, OOF_EVENT_TIMER, TAG_END };

			if (attrlist == NULL)
				obj = (OObject*) OO_DoBaseMethod( cl, obj, method, init_taglist );
			else{
				OOTagItem *join_tag = OOTag_Join( attrlist, init_taglist );
				obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist );
				OOTag_Split( join_tag );
				}
			if (obj != NULL)
				{
				OOInst_HandleTimer *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				OOList_Init( &TimersList );
				// Init default attrs & change defaults with new values
				if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				//HTObj = obj;
				}
			}
			return (ULONG)obj;

		case OOM_DELETE:
			//HTObj = NULL;
			free_timers();
			return OO_DoBaseMethod( cl, obj, method, attrlist );

		//---------------
		case OOM_GET:
			// Do not call base method
			return OOPrv_MGet( cl, obj, attrlist );

		case OOM_SET:
			// Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

		//---------------
		case OOM_HANDLEEVENT:
			if (OOINST_ATTR(cl,obj,INDEX_OOA_OnOff) == TRUE)
				HandleTimerEvent( obj, method, attrlist );
			return OOV_OK;

		//---------------
		case OOM_ADDTIMER:
			{
			struct _HandleTimerNode *node;
			ULONG delay = OOTag_GetData( attrlist, OOA_Delay, 0 );
			void *func = (void*) OOTag_GetData( attrlist, OOA_CallbackFunc, (ULONG)NULL );
			if (delay != 0 && func != NULL && (node = (struct _HandleTimerNode*) OOPool_Alloc( OOGlobalPool, sizeof(struct _HandleTimerNode) )) != NULL)
				{
#ifdef _WIN32
				if ((node->Timer = (ULONG) SetTimer( NULL, 0, delay, NULL/*(TIMERPROC)timer_proc*/ )) != 0)
#endif
					{
					node->Func = (OOTimerCallback)func;
					node->ClientData = OOTag_GetData( attrlist, OOA_ClientData, 0 );
					OOList_AddTail( &TimersList, (OONode*)node );
					return OOV_OK;
					}
				OOPool_Free( OOGlobalPool, (OONode*)node );
				}
			return OOV_ERROR;
			}

		case OOM_REMOVETIMER:
			{
			ULONG timer = OOTag_GetData( attrlist, OOA_Timer, 0 );
			struct _HandleTimerNode *node = search_timer( timer );
			if (node != NULL) free_timer( (OONode*)node );
			return OOV_OK;
			}

		//---------------
		default:
			return OO_DoBaseMethod( cl, obj, method, attrlist );
		}
	return OOV_OK;
}


/***************************************************************************
 *
 *		Utilities
 *
 ***************************************************************************/

static ULONG HandleTimerEvent( OObject *obj, ULONG method, OOTagList *attrlist )
{
	OOInst_HandleTimer *inst = OOINST_DATA(obj->Class,obj);
	struct _HandleTimerNode *timernode;

#ifdef _WIN32
	while (PeekMessage( &inst->CurrentMsg, (HWND)-1, WM_TIMER, WM_TIMER, PM_REMOVE ))
		{
		// process this message
		OOASSERT( inst->CurrentMsg.hwnd == NULL );
		OOASSERT( inst->CurrentMsg.message == WM_TIMER );
		// If the msg parameter points to a WM_TIMER message and the 
		//	lparam parameter of the WM_TIMER message is not NULL, 
		//	lparam points to a function that is called instead of 
		//	the window procedure.
		//DispatchMessage( &inst->CurrentMsg ); //  Calls window proc
		if ((timernode = search_timer( inst->CurrentMsg.wParam )) != NULL)
			timernode->Func( timernode->Timer, timernode->ClientData );
		}
#endif // _WIN32
	return OOV_OK;
}

/*
#ifdef _WIN32
static VOID CALLBACK timer_proc( HWND hwnd, UINT msgid, UINT timerid, DWORD time ) 
{
	if (OOINST_ATTR(HTObj->Class,HTObj,INDEX_OOA_OnOff) == TRUE && HTObj != NULL)
		{
		struct _HandleTimerNode *timernode;
		if ((timernode = search_timer( timerid )) != NULL)
			timernode->Func( timernode->Timer, timernode->ClientData );
		}
}
#endif
*/

static struct _HandleTimerNode *search_timer( ULONG timer )
{
	OONode *node, *succ;

    for (node=TimersList.lh_Head; succ = node->ln_Succ; node = succ)
        {
		if (((struct _HandleTimerNode*)node)->Timer == timer)
			return (struct _HandleTimerNode*)node;
        }
	return NULL;
}

static void free_timers( void )
{
	OONode *node, *succ;

    for (node=TimersList.lh_Head; succ = node->ln_Succ; node = succ)
        {
		free_timer( node );
        }
}

static void free_timer( OONode *node )
{
	OOList_Remove( node );
#ifdef _WIN32
	KillTimer( NULL, ((struct _HandleTimerNode*)node)->Timer );
#endif
	OOPool_Free( OOGlobalPool, node );
}


/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/


/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_OnOff , FALSE	, "OnOff", NULL, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }					
	};

OOClass Class_HandleTimer = { 
	Dispatcher_HandleTimer, 
	&Class_SysEvent,			// Base class
	"Class_HandleTimer",		// Class Name
	0,							// InstOffset
	sizeof(OOInst_HandleTimer),	// InstSize  : Taille de l'instance
	attrs,						// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
