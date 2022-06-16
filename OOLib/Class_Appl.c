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
#include "oo_gui.h"
#include "oo_text.h"
#include "oo_buf.h"

#include "oo_class_prv.h"

#define INDEX_OOA_ImageIndex		0
#define INDEX_OOA_Name 			1
#define INDEX_OOA_NameIndex		2
#define INDEX_OOA_Compagny 			3
#define INDEX_OOA_CompagnyIndex		4
#define INDEX_OOA_Author 			5
#define INDEX_OOA_AuthorIndex		6
#define INDEX_OOA_ConfigTagList		7


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

#ifdef _WIN32
static ULONG ReceivedSignals = 0;
static ULONG AllocatedSignals = OOMASK_EVENTS_PREALLOCATED;
#endif

#ifdef _WIN32
static HKEY	ConfigKey;
#endif
static UBYTE *SectionName = NULL;
static OOBuf *ConfigName = NULL;
static ULONG ConfigBaseLen = 0;

static ULONG WaitersSigMask = 0;
struct _OOApplWaiter WaitersTable[OOV_APPL_MAX_WAITERS] = { NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0, NULL,0 };

//----------------------------------------------------

static OORESULT	init_saved( OOClass *cl, OObject *obj, OOTagList *attrlist );
static OORESULT	cleanup_saved( OOClass *cl, OObject *obj, OOTagList *attrlist );
static OORESULT reset_saved_section( UBYTE *newsectionname, BOOL force );
static OORESULT	set_saved_attrs( OOClass *cl, OObject *obj, OOTagList *attrlist );
static OORESULT	get_saved_attrs( OOClass *cl, OObject *obj, OOTagList *attrlist );

static ULONG	add_waiter( OObject *obj, ULONG sigmask );
static void		rem_waiter( OObject *obj );
static ULONG	do_events( ULONG events );

static void		app_setsignal( ULONG sigmask );
static void		app_clearsignal( ULONG sigmask );
static ULONG	app_allocsignal( void );
static void		app_freesignal( ULONG sigmask );
static ULONG	app_get_received_events( void );
static ULONG	app_wait_events( ULONG sigmask );


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_Application( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        case OOM_NEW:
		//------------------------------------------
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
				OOInst_Application *inst = OOINST_DATA(cl,obj);
				UWORD i;

				// Init static datas
				struct _OOApplWaiter aw = { NULL, 0 };
				for (i=0; i < OOV_APPL_MAX_WAITERS; i++)
					WaitersTable[i] = aw;
				WaitersSigMask = 0;
#ifdef _WIN32
				ReceivedSignals = 0;
				AllocatedSignals = OOMASK_EVENTS_PREALLOCATED;				
#endif
				// Init instance datas

				// Init default attrs & change defaults with new values
				if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }

				init_saved( cl, obj, attrlist );
                }
            return (ULONG)obj;

        case OOM_DELETE:
            {
			OOTagList *attrs = OOINST_ATTRS(cl,obj);
			OOTagList *configtaglist = attrs[INDEX_OOA_ConfigTagList].ti_Ptr;
			set_saved_attrs( cl, obj, configtaglist );
			}
			cleanup_saved( cl, obj, attrlist );
			return OO_DoBaseMethod( cl, obj, method, attrlist );

        //---------------
        case OOM_GET:
			// Do not call base method
            return OOPrv_MGet( cl, obj, attrlist );

        case OOM_SET:
			// Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

        //---------------
        case OOM_GETSAVED:
            return get_saved_attrs( cl, obj, attrlist );

        case OOM_SETSAVED:
			return set_saved_attrs( cl, obj, attrlist );

		//------------------------------------------
        case OOM_BREAK_C:
			app_setsignal( OOF_BREAK_C );
            break;
        case OOM_BREAK_D:
			app_setsignal( OOF_BREAK_D );
            break;
        case OOM_BREAK_E:
			app_setsignal( OOF_BREAK_E );
            break;
        case OOM_BREAK_F:
			app_setsignal( OOF_BREAK_F );
            break;

		//------------------------------------------

		case OOM_SETEVENT:
			app_setsignal( OOTag_GetData( attrlist, OOA_EventMask, 0 ) );
            break;
		case OOM_CLEAREVENT:
			app_clearsignal( OOTag_GetData( attrlist, OOA_EventMask, 0 ) );
            break;
		case OOM_ALLOCEVENT:
			{ 
			ULONG mask = app_allocsignal();
			if (mask == 0) return OOV_ERROR;
			OOResult = (OORESULT) mask;
            break;
			}
		case OOM_FREEEVENT:
			app_freesignal( OOTag_GetData( attrlist, OOA_EventMask, 0 ) );
            break;

		//------------------------------------------

		case OOM_ADDEVENTHANDLER:
			return add_waiter( (OObject*)OOTag_GetData( attrlist, OOA_Object, (ULONG)NULL ), OOTag_GetData( attrlist, OOA_EventMask, 0 ) );

		case OOM_REMEVENTHANDLER:
			rem_waiter( (OObject*)OOTag_GetData( attrlist, OOA_Object, (ULONG)NULL ) );
			break;

		//------------------------------------------

        case OOM_DOEVENTS:
        case OOM_WAIT1EVENT:
        case OOM_WAITEVENTS:
            {
            ULONG events, extevents=0;
			OOTagItem *ti = attrlist;
			for (; ; ti++)
				{
				OOTAG_GETITEM(ti);
				if (ti == NULL) break;
				switch (ti->ti_Tag)
					{
					case OOA_StoreAddr: *(ULONG*)ti->ti_Ptr = WaitersSigMask; break;
					case OOA_EventMask: extevents = ti->ti_Data; break;
					}
				}
			//---------------
			if (method == OOM_DOEVENTS)
				{
				events = (app_get_received_events() & (WaitersSigMask | extevents));
				if (! (events & extevents ))
					do_events( events );
				return events;
				}
			//---------------
            do  {
                events = app_wait_events( WaitersSigMask | extevents );
                if (events & extevents ) break;
                else do_events( events );
                }
                while (method == OOM_WAITEVENTS);
            return events;
            }

		default:
			return OO_DoBaseMethod( cl, obj, method, attrlist );
        }
    return OOV_OK;
}

/***************************************************************************
 *
 *      Saved attrs functions
 *
 ***************************************************************************/

static OORESULT init_saved( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
	UBYTE		*compname = attrs[INDEX_OOA_Compagny].ti_String;
	UBYTE		*applname = attrs[INDEX_OOA_Name].ti_String;
	OOTagList	*configtaglist = attrs[INDEX_OOA_ConfigTagList].ti_Ptr;

#ifdef _WIN32
	ConfigKey = NULL;
#endif
	ConfigName = NULL;
	SectionName = NULL;
	if (configtaglist != NULL && compname != NULL && applname != NULL)
		{
#ifdef _WIN32
		if (OOBuf_FormatV( &ConfigName, 0, "SOFTWARE\\%s\\%s", compname, applname ) == NULL)
			return OOV_ERROR;
#endif
		ConfigBaseLen = strlen( ConfigName );

		if (reset_saved_section( NULL, TRUE ) != OOV_OK)
			return OOV_ERROR;

		get_saved_attrs( cl, obj, configtaglist );
		}
	return OOV_OK;
}

static OORESULT cleanup_saved( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
#ifdef _WIN32
	if (ConfigKey != NULL) { RegCloseKey( ConfigKey ); ConfigKey = NULL; }
#endif
	OOBuf_Free( &ConfigName );
	return OOV_OK;
}

static OORESULT reset_saved_section( UBYTE *newsectionname, BOOL force )
{
	OOASSERT( ConfigName != NULL );
	if (OOStr_IsEqual( SectionName, newsectionname, FALSE ) == FALSE || force == TRUE)
		{
	    ULONG result;

		OOBuf_Truncate(	ConfigName, ConfigBaseLen );
		if (newsectionname != NULL && *newsectionname != '\0')
			if (OOBuf_FormatV( &ConfigName, ConfigBaseLen, "\\%s", newsectionname ) == NULL)
				return OOV_ERROR;
#ifdef _WIN32
		if (RegCreateKeyEx( HKEY_LOCAL_MACHINE, ConfigName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &ConfigKey, &result) != ERROR_SUCCESS)
			{ ConfigKey = NULL; return OOV_ERROR; }
#endif
		SectionName = newsectionname;
		}
	return OOV_OK;
}

static OORESULT set_saved_attrs( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOTagList	*attrs = OOINST_ATTRS(cl,obj);
	UBYTE		*valuename = "Default";
	OOTagItem	*ti;
	UBYTE *dataptr;
	ULONG datatype;
	ULONG datasize;

	if (ConfigName == NULL) return OOV_ERROR;
	if (reset_saved_section( NULL, TRUE ) == OOV_ERROR) return OOV_ERROR;

	for (ti=attrlist; ; ti++)
		{
		OOTAG_GETITEM(ti);
        if (ti == NULL) break;

		switch (ti->ti_Tag)
			{
			case OOA_SectionNameIndex:
				if (ti->ti_Data != 0) reset_saved_section( OOStr_Get(ti->ti_Data), FALSE ); // ConfigKey
				break;
			case OOA_SectionName:
				if (ti->ti_String != NULL) reset_saved_section( ti->ti_String, FALSE ); // ConfigKey
				break;

			case OOA_ValueNameIndex:
				if (ti->ti_Data != 0) valuename = OOStr_Get(ti->ti_Data);
				break;
			case OOA_ValueName:
				if (ti->ti_String != NULL) valuename = ti->ti_String;
				break;

			default:
#ifdef _WIN32
				if (ConfigKey != NULL) // reset_saved_section()
					{
					datatype = REG_NONE;
					if (OOTAGISBIN(ti))
						{
						if (OOTAGISBUF(ti))
							{ if ((dataptr = *((UBYTE**)ti->ti_Ptr)) != NULL) { datatype = REG_BINARY; datasize = OOBuf_Length( dataptr ); } }
						}
					else{
						if (OOTAGISNUM(ti))
							{ dataptr = (UBYTE*)ti->ti_Data; datatype = REG_DWORD; datasize = sizeof(ULONG); }
						else if (OOTAGISSTR(ti))
							{ if ((dataptr = *((UBYTE**)ti->ti_Ptr)) != NULL) { datatype = REG_SZ; datasize = strlen( dataptr ); } }
						}

					if (datatype != REG_NONE)
						RegSetValueEx( ConfigKey, valuename, 0, datatype, dataptr, datasize );
					}
				else OOASSERT (FALSE);
#endif
				break;
			}
		}
	
	return OOV_OK;
}

static OORESULT get_saved_attrs( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOTagList	*attrs = OOINST_ATTRS(cl,obj);
	UBYTE		*valuename = "Default";
	OOTagItem	*ti;
	UBYTE *dataptr;
	ULONG datatype;
	ULONG readtype;
	ULONG readsize;

	if (ConfigName == NULL) return OOV_ERROR;
	if (reset_saved_section( NULL, TRUE ) == OOV_ERROR) return OOV_ERROR;

	for (ti=attrlist; ; ti++)
		{
		OOTAG_GETITEM(ti);
        if (ti == NULL) break;

		switch (ti->ti_Tag)
			{
			case OOA_SectionNameIndex:
				if (ti->ti_Data != 0) reset_saved_section( OOStr_Get(ti->ti_Data), FALSE ); // ConfigKey
				break;
			case OOA_SectionName:
				if (ti->ti_String != NULL) reset_saved_section( ti->ti_String, FALSE ); // ConfigKey
				break;

			case OOA_ValueNameIndex:
				if (ti->ti_Data != 0) valuename = OOStr_Get(ti->ti_Data);
				break;
			case OOA_ValueName:
				if (ti->ti_String != NULL) valuename = ti->ti_String;
				break;

			default:
#ifdef _WIN32
				if (ConfigKey != NULL) // reset_saved_section()
					{
					datatype = REG_NONE;
					if ((dataptr = ti->ti_Ptr) != NULL)
						{
						if (OOTAGISBIN(ti))
							{
							if (OOTAGISBUF(ti)) datatype = REG_BINARY;
							}
						else{
							if (OOTAGISNUM(ti)) datatype = REG_DWORD;
							else if (OOTAGISSTR(ti)) datatype = REG_SZ;
							}
						}

					if (datatype != REG_NONE)
						{
						if (RegQueryValueEx( ConfigKey, valuename, NULL, &readtype, NULL, &readsize ) == ERROR_SUCCESS)
							{
							if (readtype != datatype)
								OOASSERT( FALSE );
							else{
								if (datatype != REG_DWORD)
									{
									OOBuf **bufaddr = (OOBuf**)dataptr;
									dataptr = *bufaddr;
									if (dataptr == NULL || !OOTAGISBUF(ti)) 
										{ if ((dataptr = *bufaddr = OOBuf_Alloc( OOGlobalPool, readsize, 4 )) != NULL) ti->ti_IsBuf = 1; }
									else dataptr = OOBuf_Resize( bufaddr, readsize, FALSE );
									}
								}
							}

						if (dataptr != NULL)
							if (RegQueryValueEx( ConfigKey, valuename, NULL, NULL, dataptr, &readsize ) == ERROR_MORE_DATA)
								RegQueryValueEx( ConfigKey, valuename, NULL, NULL, dataptr, &readsize );
						}
					}
#endif
				break;
			}
		}

	return OOV_OK;
}


/***************************************************************************
 *
 *      Signals functions
 *
 ***************************************************************************/

static ULONG add_waiter( OObject *obj, ULONG mask )
{
  struct _OOApplWaiter *aw;
  ULONG i;

	if (mask)
		for (aw=WaitersTable, i=0; i < OOV_APPL_MAX_WAITERS; i++, aw++)
			{
			if (aw->Waiter == NULL)
				{
				aw->Waiter = obj;
				WaitersSigMask |= (aw->Mask = mask);
				return OOV_OK;
				}
			}
	return OOV_ERROR;
}

static void rem_waiter( OObject *obj )
{
  struct _OOApplWaiter *aw;
  ULONG i;

    WaitersSigMask = 0;
    for (aw=WaitersTable, i=0; i < OOV_APPL_MAX_WAITERS; i++, aw++)
        {
        if (aw->Waiter == obj) { aw->Waiter = NULL; aw->Mask = 0;  }
        else WaitersSigMask |= aw->Mask;
        }
}

static ULONG do_events( ULONG events )
{
  struct _OOApplWaiter *aw;
  ULONG i, rc=0;

	if (events & WaitersSigMask)
		{
		for (aw=WaitersTable, i=0; i < OOV_APPL_MAX_WAITERS; i++, aw++)
			{
			if (aw->Waiter)
				{
				if (events & aw->Mask) 
					rc = OO_DoMethodV( aw->Waiter, OOM_HANDLEEVENT, OOA_EventMask, events, TAG_END );
				}
			}
		}
    return rc;
}

//--------------------------------------------------------------------------------

static void app_setsignal( ULONG sigmask )
{
#ifdef AMIGA
	Signal( FindTask(0), sigmask );
#endif
#ifdef _WIN32
	ReceivedSignals |= sigmask;
	PostMessage( NULL, WM_OO_SIGNALED, 0, 0 );
#endif
}

static void app_clearsignal( ULONG sigmask )
{
#ifdef AMIGA
	SetSignal( 0, sigmask );
#endif
#ifdef _WIN32
	ReceivedSignals &= ~sigmask;
#endif
}

static ULONG app_allocsignal( void )
{
	SBYTE sigbit;
#ifdef AMIGA
	sigbit = AllocSignal( ? );
	if (sigbit != -1)
	return (1L<<sigbit);
#endif
#ifdef _WIN32
	ULONG sigmask;
	for (sigbit=31; sigbit >= OOV_APPL_PREALLOCATED; sigbit--)
		if ((AllocatedSignals & (sigmask=(1L<<sigbit))) == 0)
			{
			AllocatedSignals |= sigmask;
			return sigmask;
			}
#endif
	return 0;
}

static void app_freesignal( ULONG sigmask )
{
#ifdef AMIGA
	FreeSignal( ? );
#endif
#ifdef _WIN32
	if (! (sigmask & OOMASK_EVENTS_PREALLOCATED))
		AllocatedSignals &= ~sigmask;
#endif
}

static ULONG app_get_received_events( void )
{
#ifdef AMIGA
	return SetSignal(0,0);
#endif
#ifdef _WIN32
	MSG _MSG;
	ULONG return_events;

	// See if a non-window message
	while (PeekMessage( &_MSG, (HWND)NULL, 0, 0, PM_NOREMOVE ))
		{
		if (_MSG.hwnd == NULL)
			{
			// OOTRACE( "External Message : %x\n", _MSG.message );
			if (_MSG.message == WM_TIMER)
				{
				ReceivedSignals |= OOF_EVENT_TIMER;
				break; // Let handlers
				}

			switch (_MSG.message)
				{
				case WM_OO_SIGNALED: // For ctrl-C or events without messages
					break;
				case WM_QUIT:		 // Program termination asked from anywhere
					ReceivedSignals |= OOF_BREAK_C;
					break;
				default:
					OOTRACE( "*** Non-Window message received : %x\n", _MSG.message );
					break;
				}
			// Remove the message
			PeekMessage( &_MSG, (HWND)NULL, _MSG.message, _MSG.message, PM_REMOVE ); 
			}
		else{
			// OOTRACE( "GUI Message : %x, %d\n", _MSG.message, num++ );
			ReceivedSignals |= OOF_EVENT_GUI;
			break; // Let handlers
			}
		}
	return_events = ReceivedSignals;
	ReceivedSignals = 0;
	return return_events;
#endif
}

static ULONG num;

static ULONG app_wait_events( ULONG sigmask )
{
#ifdef AMIGA
	return Wait( sigmask );
#endif
#ifdef _WIN32
	MSG _MSG;
	ULONG return_events;

	// It seems that WaitMessage Does not look if messages arrived at the queue
	if (! PeekMessage( &_MSG, (HWND)NULL, 0, 0, PM_NOREMOVE ))
		{
		WaitMessage();
		PeekMessage( &_MSG, (HWND)NULL, 0, 0, PM_NOREMOVE );
		}
	// See if a non-window message
	do	{
		if (_MSG.hwnd == NULL)
			{
			// OOTRACE( "External Message : %x, %d\n", _MSG.message, num++ );
			if (_MSG.message == WM_TIMER)
				{
				ReceivedSignals |= OOF_EVENT_TIMER;
				break; // Let handlers
				}

			switch (_MSG.message)
				{
				case WM_OO_SIGNALED: // For ctrl-C or events without messages
					break;
				case WM_QUIT:		 // Program termination asked from anywhere
					ReceivedSignals |= OOF_BREAK_C;
					break;
				default:
					OOTRACE( "*** Non-Window message received : %x, %d\n", _MSG.message, num++ );
					break;
				}
			// Remove the message
			PeekMessage( &_MSG, (HWND)NULL, _MSG.message, _MSG.message, PM_REMOVE ); 
			}
		else{
			// OOTRACE( "GUI Message : %x, %d\n", _MSG.message, num++ );
			ReceivedSignals |= OOF_EVENT_GUI;
			break; // Let handlers
			}
		} while (PeekMessage( &_MSG, (HWND)NULL, 0, 0, PM_NOREMOVE ));

	return_events = (ReceivedSignals & sigmask);	   //+++
	ReceivedSignals &= ~sigmask;
	return return_events;
#endif
}


/***************************************************************************
 *
 *      Attributes setting
 *
 ***************************************************************************/

static ULONG SetGet_All( OOSetGetParms *sgp )
{
	ULONG change = OOV_NOCHANGE;
	ULONG data = sgp->ExtAttr->ti_Data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_NameIndex:
				sgp->Attr->ti_Data = data;
				if (OOPrv_MSetBounceV( sgp, OOA_Name, OOStr_Get( data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_CompagnyIndex:
				sgp->Attr->ti_Data = data;
				if (OOPrv_MSetBounceV( sgp, OOA_Compagny, OOStr_Get( data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			case OOA_AuthorIndex:
				sgp->Attr->ti_Data = data;
				if (OOPrv_MSetBounceV( sgp, OOA_Author, OOStr_Get( data ), TAG_END ) == OOV_ERROR) return OOV_ERROR; // exécution de OOM_DELETE 
				break;
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return change;
}


/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_ImageIndex	, 0			 , "ImageIndex"		, NULL		, OOF_MNEW |		OOF_MGET },
	{ OOA_Name 			, (ULONG)NULL, "Name"			, NULL		, OOF_MNEW |		OOF_MGET },
	{ OOA_NameIndex		, 0			 , "NameIndex"		, SetGet_All, OOF_MNEW |		OOF_MGET },
	{ OOA_Compagny 		, (ULONG)NULL, "Compagny"		, NULL		, OOF_MNEW |		OOF_MGET },
	{ OOA_CompagnyIndex	, 0			 , "CompagnyIndex"	, SetGet_All, OOF_MNEW |		OOF_MGET },
	{ OOA_Author 		, (ULONG)NULL, "Author"			, NULL		, OOF_MNEW |		OOF_MGET },
	{ OOA_AuthorIndex	, 0			 , "AuthorIndex"	, SetGet_All, OOF_MNEW |		OOF_MGET },
	{ OOA_ConfigTagList	, (ULONG)NULL, "ConfigTagList"	, NULL		, OOF_MNEW					 },
	{ TAG_END }
	};

OOClass Class_Application = { 
	Dispatcher_Application, 
	&Class_Root,			// Base class
	"Class_Application",	// Class Name
	0,						// InstOffset
	sizeof(OOInst_Application),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
