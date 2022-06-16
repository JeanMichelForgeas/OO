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

#include "oo_class_prv.h"

#define	INDEX_OOA_EventPort		0
#define	INDEX_OOA_EventMask		1


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/


/***************************************************************************
 *
 *      Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_SysEvent( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
				OOInst_SysEvent *inst = OOINST_DATA(cl,obj);
				// Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
                    { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				else{
					ULONG mask = OOINST_ATTR(cl,obj,INDEX_OOA_EventMask); // Look for preallocated signal
					if (mask == 0) 
						{
#ifdef AMIGA
						struct MsgPort *port = CreateMsgPort();
						if (port == NULL)
							{ OO_DoMethod( obj, OOM_DELETE, NULL ); return (ULONG)NULL; }
						OOINST_ATTR(cl,obj,INDEX_OOA_EventPort) = (ULONG)port;
						mask = (1L << port->mp_SigBit);
#endif
#ifdef _WIN32
						if (OO_DoClassMethod( &Class_Application, NULL, OOM_ALLOCEVENT, NULL ) == OOV_ERROR)
							{ OO_DoMethod( obj, OOM_DELETE, NULL ); return (ULONG)NULL; }
						mask = (ULONG) OOResult;
#endif
						inst->Flags |= OOF_SYSEVENT_ALLOCATED;
						OOINST_ATTR(cl,obj,INDEX_OOA_EventMask) = mask;
						}
					if (OO_DoClassMethodV( &Class_Application, NULL, OOM_ADDEVENTHANDLER, 
											OOA_Object,		obj, 
											OOA_EventMask,	mask, 
											TAG_END ) == OOV_ERROR)
						{ OO_DoMethod( obj, OOM_DELETE, NULL ); return (ULONG)NULL; }
	                }
                }
            return (ULONG)obj;

        //---------------
        case OOM_DELETE:
			{
			OOInst_SysEvent *inst = OOINST_DATA(cl,obj);

			OO_DoClassMethodV( &Class_Application, NULL, OOM_REMEVENTHANDLER, OOA_Object, obj, TAG_END );
			if (inst->Flags & OOF_SYSEVENT_ALLOCATED)
				{
#ifdef AMIGA
				struct MsgPort *port = (struct MsgPort*) OOINST_ATTR(cl,obj,INDEX_OOA_EventPort);
				if (port != NULL) 
					DeleteMsgPort( port );
#endif
#ifdef _WIN32
				OO_DoClassMethodV( &Class_Application, NULL, OOM_FREEEVENT, OOA_EventMask, OOINST_ATTR(cl,obj,INDEX_OOA_EventMask), TAG_END );
#endif
				}
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
        case OOM_HANDLEEVENT:
#ifdef AMIGA
            OOResult = (OORESULT) GetMsg( OOINST_ATTR(cl,obj,INDEX_OOA_EventPort );
#endif
#ifdef _WIN32
#endif
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


/***************************************************************************
 *
 *      Attributes setting
 *
 ***************************************************************************/


/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_EventPort, (ULONG)NULL, "EventPort", NULL, OOF_MNEW | OOF_MGET },
    { OOA_EventMask, 0			, "EventMask", NULL, OOF_MNEW | OOF_MGET },
    { TAG_END }
	};

OOClass Class_SysEvent = { 
	Dispatcher_SysEvent, 
	&Class_Root,			// Base class
	"Class_SysEvent",		// Class Name
	0,						// InstOffset
	sizeof(OOInst_SysEvent),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
