#ifndef OO_CLASS_PRV_H
#define OO_CLASS_PRV_H

#include "oo_class.h"
#include "oo_list.h"

typedef	struct _OOTarget	OOTarget;

#define OOINST_DATA(cl,o)	((APTR)(((UBYTE*)o)+(cl)->InstOffset)) // add offset for instance data to an object handle
#define OOINST_OBJ(cl,i)	((OObject*)(((UBYTE*)i)-(cl)->InstOffset)) // sub offset from instance for object handle
#define OOINST_ATTRS(cl,o)	((OOTagList*)(((UBYTE*)o)+(cl)->InstOffset+(cl)->InstSize)) // 
#define OOINST_ATTR(cl,o,a)	(OOINST_ATTRS(cl,o)[(a)].ti_Data)
#define OOGETPARENT(o)		((o)->ParentsTable.Table!=NULL?(OObject*)((o)->ParentsTable.Table[0]):NULL)

//------------------------ Class --------------------------------------

struct _OOClass {
    ULONG			(*Dispatcher)( void*, void*, ULONG, OOTagList* );
	OOClass			*BaseClass;
	UBYTE			*Name;			// Name of the class
    UWORD			 InstOffset;	// where within an object is the instance data for this class?
    UWORD			 InstSize;		// Size of the instance
	OOAttrDesc		*AttrsDesc;		// Table of descriptors for each attribute, end marker : Name==NULL
    UWORD			 AttrsSize;		// Size for instance attrs + 1
    };									

//------------------------ Instance --------------------------------------

struct _OOInst_Root {
    OOClass	*	Class;
	OObjArray	ParentsTable;
	OObjArray	ChildrenTable;
	OObjArray	SendersTable;	// Objects having this one as a notification target
	OObjArray	TargetsTable;	// Objects being notification targets
	UWORD		Flags;
	UWORD		NotUsed;
    };

#define OOF_NOTIFYING       0x0001	// Interne au processus de notification
#define OOF_BREAKNOTIFY     0x0002	// Interne au processus de notification
#define OOF_ISALLOCATED		0x0004	// Si oui l'instance a été allouée par OO_New()
#define OOF_DONOTDELETE		0x0008	// Interne au processus delete
#define OOF_DELETED			0x0010	// Interne au processus delete

#define OOV_CHILDLIST_BLOCKSIZE		8
#define OOV_PARENTLIST_BLOCKSIZE	2
#define OOV_TARGETLIST_BLOCKSIZE	2

// OOAttrDesc Method Flags
#define OOF_MNEW    0x01	// L'attribut est pris en compte par OOM_NEW
#define OOF_MSET    0x02	// L'attribut est pris en compte par OOM_SET
#define OOF_MGET    0x04	// L'attribut est pris en compte par OOM_GET
#define OOF_MNOT	0x08	// L'attribut provoque un OOM_NOTIFY si modifié
#define OOF_MAREA   0x10	// L'attribut provoque un recalcul d'area si modifié

//------------------------ Attributes --------------------------------

struct _OOAttrDesc {
	OOTagItem	TagItem;	// Pairs of tag	/ default value
    UBYTE *		Name;		// Nom de l'attribut : nom pour en faire une variable, ou titre pour un élément de dialogue
	ULONG		(*SetGet)( OOSetGetParms* );// Fonction à appeler
    UWORD		Flags;		// Flags de validité ou de déclenchement
	};
struct _OOFltAttrDesc {
	OOFltTagItem	TagItem;	// Pairs of tag	/ default value
    UBYTE *			Name;		// Nom de l'attribut : nom pour en faire une variable, ou titre pour un élément de dialogue
	ULONG			(*SetGet)( OOSetGetParms* );// Fonction à appeler
    UWORD			Flags;		// Flags de validité ou de déclenchement
	};

// Parameters for function (*SetGet)()
struct _OOSetGetParms {
	OOClass		*Class;
	OObject		*Obj;
	APTR		Instance;
	OOTagItem	*Attr;
	OOTagItem	*ExtAttr;
	ULONG		MethodFlag;
	};

//------------------------ Targets -----------------------------------

struct _OOTarget {
    UBYTE       TrigType;
    UBYTE       ActionType;
	UWORD			NotUsed;
    ULONG       SrcAttr;	// Used as a TagItem with following Min
    ULONG       Min;
    ULONG       Max;
    OObject		**PDest;
	ULONG		MethodId;
	OOTagItem	TagList[1]; // Minimum = size for TAG_END
    };

//==============================================================================
//==============================================================================
//============================                  ================================
//============================      CLASSES     ================================
//============================                  ================================
//==============================================================================
//==============================================================================

//================================== Application ===============================

#define OOV_APPL_PREALLOCATED 16	// Amiga OS leave only 16 signals free
#define OOV_APPL_MAX_WAITERS (32-OOV_APPL_PREALLOCATED)

#ifdef AMIGA
#define OOMASK_EVENTS_PREALLOCATED	(0x0000FFFFL)
#endif

#ifdef _WIN32
// See with other preallocated sigs in class_appl.h
#define OOF_EVENT_GUI	(1L << 4)
#define OOF_EVENT_TIMER	(1L << 5)
#define OOMASK_EVENTS_PREALLOCATED	(OOF_BREAK_C|OOF_BREAK_D|OOF_BREAK_E|OOF_BREAK_F|OOF_EVENT_GUI|OOF_EVENT_TIMER)

#define WM_OO_SIGNALED	(WM_APP+1)	// Used to transmit signals and to break waiting loop
#endif

struct _OOApplWaiter {
    OObject *Waiter;
    ULONG   Mask;
    };

struct _OOInst_Application {
#ifdef _WIN32
	ULONG	Timer;		// To simulate Amiga intuiticks
#endif
	};


//================================== SysEvent ==================================

struct _OOInst_SysEvent {
	ULONG	Flags;
	};

// Flags
#define OOF_SYSEVENT_ALLOCATED	0x0001


//================================== HandleTimer ===============================

struct _HandleTimerNode {
	OONode			Node;
	OOTimerCallback	Func;
	ULONG			Timer;
	ULONG			ClientData;
	};

struct _OOInst_HandleTimer {
	MSG				CurrentMsg;
	};


//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

ULONG	OOPrv_MNew			( OOClass *cl, OObject *obj, OOTagList *tl );
ULONG	OOPrv_MSet			( OOClass *cl, OObject *obj, OOTagList *tl, UWORD methodflag );
ULONG	OOPrv_MSetBounceV	( OOSetGetParms *sgp, ULONG attr, ... );
ULONG	OOPrv_MSetBounce	( OOSetGetParms *sgp, OOTagList *taglist );
ULONG	OOPrv_MGet			( OOClass *cl, OObject *obj, OOTagList *tl );
ULONG	OOPrv_MClone		( OOClass *cl, OObject *obj, OOTagList *tl );

#endif // OO_CLASS_PRV_H
