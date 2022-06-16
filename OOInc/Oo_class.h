#ifndef OO_CLASS_H
#define OO_CLASS_H

#define	OO_ALL_ALLOC_POOLED	// Want all allocs done into OOGlobalPool instead of malloc()

typedef struct _OOClass			OOClass;
typedef struct _OOAttrDesc		OOAttrDesc;
typedef struct _OOFltAttrDesc	OOFltAttrDesc;
typedef struct _OOSetGetParms	OOSetGetParms;
typedef struct _OObjArray		OObjArray;
typedef struct _OONotifyItem	OONotifyItem;
typedef struct _OOTreeItem		OOTreeItem;


#include "ootypes.h"
#include "ooprotos.h"
#include "oo_tags.h"

typedef	struct _OOInst_Root		OObject;
typedef	struct _OOInst_Root		OOInst_Root;

/*
        Root
        \--Appl
        \--Pic
        \--DebugEvents
        \--SysEvent
        :  \--HandleGUI       Gestion d'interface utilisateur
        :  \--Handle2D        Gestion d'objets 2D dans un espace 2D : calculs sur des entiers (espace maxi : 32000 pixels
        :  \--Handle3D        Gestion d'objets 3D dans un espace 3D : calculs sur des flottants simple précision (espace maxi : 10^20 années-lumières, soit beaucoup plus que la taille de l'univers! Plusieurs univers peuvent cohabiter simultanément)
        :  \--HandleRexx
        :  \--HandleSound
        :  \--HandleAnim
        \--Area
        :  \--AreaGroup
        :  :  \--AreaScreen
        :  :  \--AreaWindow
        :  :  \--AreaRequest     (pas encore fait)
        :  :  \--AreaGroupBox
        :  \--Boopsi          (Privé Amiga: BOOPSI)
        :  \--GadTools        (Privé Amiga: MerdouilleGad)
        :     \--GadButton       (BUTTON_KIND  )
        :     \--GadChecker      (CHECKBOX_KIND)
        :     \--GadPopup        (CYCLE_KIND   )
        :     \--GadEditNum      (INTEGER_KIND )
        :     \--GadEditStr      (STRING_KIND  )
        :     \--GadDispNum      (NUMBER_KIND  )
        :     \--GadDispStr      (TEXT_KIND    )
        :     \--GadListView     (LISTVIEW_KIND)
        :     \--GadChoice       (MX_KIND      )
        :     \--GadPalette      (PALETTE_KIND )
        :     \--GadSlider       (SLIDER_KIND  )


                         Appl
                        /    \
                HandleGUI    Handle...
                /       \         \
             Screen    Screen
            /      \         \
       Window      Window
       /    \          \
   Gad...  Gad...
*/

//---------------------- Simple & fast dynamic table -------------------------

struct _OObjArray {
	UWORD	Count;
	UWORD	Max;
	OObject **Table;
	};

//---------------------- Methods (0x0001-0x001F) -----------------------------
// For general use, by several classes

//---------------------- Attributes (0x0001-0x001F) --------------------------
// For general use, parameter in taglist, etc...

#define OOA_StoreAddr			(0x0001 | TAGT_OO | TAGT_LONG | TAGT_PTR)
#define OOA_EventMask			(0x0002 | TAGT_OO | TAGT_LONG | TAGT_NUM)
#define OOA_Object				(0x0003 | TAGT_OO | TAGT_OBJ  | TAGT_PTR)
#define OOA_Flags				(0x0004 | TAGT_OO | TAGT_LONG | TAGT_NUM)
#define OOA_TagList				(0x0005 | TAGT_OO | TAGT_LONG | TAGT_PTR)
#define OOA_TagItem				(0x0006 | TAGT_OO | TAGT_BYTE | TAGT_PTR)
#define OOA_FileName			(0x0007 | TAGT_OO | TAGT_BYTE | TAGT_STR | TAGT_EDIT)
#define OOA_OnOff				(0x0008 | TAGT_OO | TAGT_BOOL | TAGT_NUM | TAGT_CHECK)	// Allumé/éteint
#define OOA_Console				(0x0009 | TAGT_OO | TAGT_BOOL | TAGT_NUM | TAGT_CHECK)
#define OOA_VerifyFunc			(0x000A | TAGT_OO | TAGT_LONG | TAGT_PTR)
#define OOA_Timer				(0x000B | TAGT_OO | TAGT_LONG | TAGT_NUM)
#define OOA_CallbackFunc		(0x000C | TAGT_OO | TAGT_LONG | TAGT_PTR)
#define OOA_Delay				(0x000D | TAGT_OO | TAGT_LONG | TAGT_NUM | TAGT_EDIT)
#define OOA_ClientData			(0x000E | TAGT_OO | TAGT_LONG | TAGT_NUM)
#define OOA_SectionName			(0x000F | TAGT_OO | TAGT_BYTE | TAGT_STR | TAGT_EDIT)	// Used by OOA_ConfigTagList from Class_Application
#define OOA_SectionNameIndex	(0x0010 | TAGT_OO | TAGT_BYTE | TAGT_STR | TAGT_EDIT)	// Used by OOA_ConfigTagList from Class_Application
#define OOA_ValueName			(0x0011 | TAGT_OO | TAGT_BYTE | TAGT_STR | TAGT_EDIT)
#define OOA_ValueNameIndex		(0x0012 | TAGT_OO | TAGT_BYTE | TAGT_STR | TAGT_EDIT)
#define OOA_ValueNumber			(0x0013 | TAGT_OO | TAGT_LONG | TAGT_NUM | TAGT_EDIT)
#define OOA_ValueString			(0x0014 | TAGT_OO | TAGT_BYTE | TAGT_STR | TAGT_EDIT)
#define OOA_ValueBinary			(0x0015 | TAGT_OO | TAGT_DATA | TAGT_PTR | TAGT_EDIT)
#define OOA_Name				(0x0016 | TAGT_OO | TAGT_BYTE | TAGT_STR | TAGT_EDIT)
#define OOA_NameIndex			(0x0017 | TAGT_OO | TAGT_LONG | TAGT_NUM | TAGT_EDIT)
#define OOA_ImageIndex 			(0x0018 | TAGT_OO | TAGT_LONG | TAGT_NUM | TAGT_EDIT)

//------------------ Fonction DoChildren() ---------------------

#define OOF_TESTCHILD		0x0001	// Teste la valeur de retour de DoMethod() pour s'arrêter si OOV_ERROR

//------------------------ Notify ------------------------------

//--- Conditions de déclenchement
//      OO_DoNotify() reçoit une taglist, et pour chaque paire (attribut,valeur)
//      il regarde si la valeur remplit la condition de déclenchement précisée (trigtype : OON_TRIG_...)
//      et si c'est le cas il effectue l'action demandée (actiontype : OON_ACT_...)
//      Si l'action est OON_ACT_DOMETHOD...et *pdest != 0, la methode est envoyée
//      à *pdest avec l'éventuelle taglist spécifiée.
//
//  La fonction appelée par OON_ACT_CALLFUNC ou la méthode appelée par OON_ACT_DOMETHOD... 
//      doit retourner une valeur : OOV_OK ou OOV_ERROR -> arrêt de la notification (erreur, 
//		refut de OOM_DELETE d'une fenêtre, etc...)
//
//  OO_AddNotify( OObject *obj, ULONG trigtype, ULONG attr, ULONG min, ULONG max,
//				  OObject **pdest, ULONG actiontype, ULONG mid, OOTagList *taglist );

#define OON_TRIG_EVERYVALUE     1   // Pour chaque valeur (pas de test) 
#define OON_TRIG_RANGE			2   // Test signé déclenche si : min <= valeur <= max 
#define OON_TRIG_EQUAL          3   // Déclenche si valeur == min, use strcmp() si tags & TAG_STRING
									  
//--- Actions déclenchées
#define OON_ACT_CALLFUNC        1   // Appel d'une fonction : ULONG mid( OObject *srcobj, ULONG attreçu, ULONG valreçue, ULONG min, ULONG max, OObject *destobj, OOTagItem *taglist )
#define OON_ACT_COPYSTR         2   // Copie de chaîne (addresse = argument 'tagattr') : strcpy( tagattr, valreçue ); 
#define OON_ACT_FORMATSTR       3   // Création d'une chaîne formatée : sprintf( tagattr, tagvalue, valreçue ); 
#define OON_ACT_COPYCONST       4   // Copie d'une constante ULONG : *((ULONG*)tagattr) = tagvalue; 
#define OON_ACT_COPYVALUE       5   // Copie de la valeur reçue : *((ULONG*)tagattr) = valreçue; 
#define OON_ACT_DOMETHOD        6   // Fait OO_DoMethodA( *pdest, msg ); msg->MethodId est rempli avec mid, et msg->AttrList avec : tagattr, tagvalue, tagattr2, tagvalue2 
#define OON_ACT_DOMETHODVALUE   7   // Pareil que OON_ACT_DOMETHOD, mais tagvalue est remplacée par la valeur reçue 

//--- Exemple d'ajout de notification sur un objet
//	OO_AddNotifyV(
//	------ Source -----------------------------------------------
//	SrcObj,                 : obj           objet recevant l'attribut déclencheur
//	OON_TRIG_RANGE,			: trigtype      condition de déclenchement
//	OOA_Pos,                : attr          attribut de déclenchement
//	10,                     : min           valeur de l'attribut ou minimum de déclenchement
//	2000,                   : max           selon trigtype, maximum de déclenchement
//	------ Destination ------------------------------------------
//	&DstObj,                : pdest         pointeur sur le pointeur de l'objet asservi
//	OON_ACT_DOMETHODVALUE,  : actiontype    type de l'action à déclencher
//	------ Method -----------------------------------------------
//	OOM_SET,				: mid           MethodId à envoyer, ou fonction à exécuter si OON_ACT_CALLFUNC
//	------ TagList ----------------------------------------------
//	OOA_Num,                : tagattr       1er attribut à donner à *pdest, ou bien addresse de copie
//	0,						: tagvalue      valeur de cet attribut, ou bien chaîne de formatage
//  ..., ...				:				autres paires de valeurs
//	TAG_END                 :				Terminer la taglist
//	)

struct _OONotifyItem {
    ULONG   TrigType;
    ULONG   SrcAttr;
    ULONG   Min;
    ULONG   Max;
    ULONG   DestVar;    // If OOV_NOOFFS take DestObj
    ULONG   ActionType;
    ULONG   MethodId;
    ULONG   Attr1;
    ULONG   Value1;
    ULONG   Attr2;
    ULONG   Value2;
    ULONG   Attr3;
    ULONG   Value3;
    ULONG   Tag_End;	// Put OOV_TAG_END here
    ULONG   Taglist;	// If you need more tags, put a pointer to a taglist here
    }; 

#define OOV_NTOBJITSELF MAXULONG

// Table de définition des notifications
// Valeur speciale : la table doit se terminer par OOV_ENDTABLE
#define NOTIFY( TrigType, SrcAttr, Min, Max, DestVar, ActionType, MethodId, Attr1, Value1, Attr2, Value2, Attr3, Value3, Tag_End, TagList ) \
			  { TrigType, SrcAttr, Min, Max, DestVar, ActionType, MethodId, Attr1, Value1, Attr2, Value2, Attr3, Value3, Tag_End, TagList }

//------------------------ Tree construction ---------------------------

struct _OOTreeItem {
    SWORD           Level;
    ULONG           Var;    // si -1 pas de sauvegarde du ptr
    OOClass         *Class;
    OOTagItem       *TagList;
    OONotifyItem    *NotifyTable;
    };

#define OOV_NOOFFS  MAXULONG

#define OOA_Tree    (0x0135	| TAGT_OO | TAGT_PTR)

#define OBJECT( xLevel, xVar, xClass, xTagList, xNotify ) { xLevel, xVar, xClass, xTagList, xNotify }
#define OBJECTEND ((SWORD)-1)


//==============================================================================
//==============================================================================
//============================                  ================================
//============================      CLASSES     ================================
//============================                  ================================
//==============================================================================
//==============================================================================

//================================== Root ======================================

extern OOClass Class_Root;

//------------------------ Methods (0x0020-0x003F) --------------------------------

#define OOM_NEW				0x0021
#define OOM_DELETE			0x0022
#define OOM_SET				0x0023
#define OOM_GET				0x0024
#define OOM_NOTIFY			0x0025
#define OOM_ADDNOTIFY		0x0026
#define OOM_ADDCHILD		0x0027
#define OOM_REMCHILD		0x0028
#define OOM_ADDPARENT		0x0029
#define OOM_REMPARENT		0x002A
#define OOM_DETACH			0x002B
#define OOM_COPY			0x002C

//---------------------- Attributes (0x0020-0x003F) --------------------------

#define OOA_Child		(0x0021 | TAGT_OO|TAGT_LONG |TAGT_OBJ |TAGT_PTR)	// Ajout dans la liste de l'objet de celui donné dans ti_Data 
#define OOA_Parent		(0x0022 | TAGT_OO|TAGT_LONG |TAGT_OBJ |TAGT_PTR)	// Ajout de l'objet dans la liste de l'objet mis dans ti_Data 

/* General use attributes (defined into oo_class.h)
	OOA_StoreAddr	// Permet de mettre à jour le pointeur sur l'objet
*/

//================================== Application ===============================

extern OOClass Class_Application;

typedef	struct _OOInst_Application		OOInst_Application;

//------------------------ Methods (0x0040-0x005F) -----------------------------

#define OOM_DOEVENTS		0x0040	// Execute 1 event, n'attend pas
#define OOM_WAIT1EVENT		0x0041	// Execute 1 event, attend qu'il arrive
#define OOM_WAITEVENTS		0x0042	// Boucle d'attente d'évènements
#define OOM_BREAK_C			0x0043	// Envoie un Ctrl+C au programme
#define OOM_BREAK_D			0x0044	// Envoie un Ctrl+D au programme
#define OOM_BREAK_E			0x0045	// Envoie un Ctrl+E au programme
#define OOM_BREAK_F			0x0046	// Envoie un Ctrl+F au programme
#define OOM_SETEVENT		0x0047	// Provoque la réception d'un évènement
#define OOM_CLEAREVENT		0x0048	// Annule la réception d'un évènement
#define OOM_ALLOCEVENT		0x0049	// Alloue une ressource évènement
#define OOM_FREEEVENT		0x004A	// Libère une ressource évènement
#define OOM_ADDEVENTHANDLER	0x004B	// Register an object for an event mask
#define OOM_REMEVENTHANDLER	0x004C	// Unregister an object and its event mask
#define OOM_GETSAVED		0x004D	// Lit des données depuis un fichier de config (registry sous Win32)
#define OOM_SETSAVED		0x004E	// Ecrit des données danss un fichier de config (registry sous Win32)

//------------------------ Attributes (0x0040-0x005F) --------------------------

#define OOA_Compagny 		(0x0040|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)	// 
#define OOA_CompagnyIndex	(0x0041|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)	// 
#define OOA_Author 			(0x0042|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)	// 
#define OOA_AuthorIndex		(0x0043|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)	// 
#define OOA_ConfigTagList	(0x0044|TAGT_OO|TAGT_LONG|TAGT_PTR)	// Accept OOA_ConfigSection and any attrs

/* General use attributes (defined into oo_class.h)
	OOA_StoreAddr	// Address to store the received signals to
	OOA_EventMask	// Parameter for methods OOM_SETEVENT, etc...
	OOA_Title
	OOA_TitleIndex
	OOA_ImageIndex
*/

/* The OOA_ConfigTagList can use any tags and these ones :
	OOA_ConfigSection : special for the section name
	OOA_ValueName	  :	special for the variable name
	OOA_StringValue	OOA_NumberValue	OOA_BufferValue	: differents value types...	
*/

// Preallocated signals
#ifdef AMIGA
#define OOF_BREAK_C     SIGBREAKF_CTRL_C
#define OOF_BREAK_D     SIGBREAKF_CTRL_D
#define OOF_BREAK_E     SIGBREAKF_CTRL_E
#define OOF_BREAK_F     SIGBREAKF_CTRL_E
#endif
#ifdef _WIN32
#define OOF_BREAK_C     (1L << 0)
#define OOF_BREAK_D     (1L << 1)
#define OOF_BREAK_E     (1L << 2)
#define OOF_BREAK_F     (1L << 3)
#endif
#ifdef MAC
#endif


//================================== SysEvent ==================================

extern OOClass Class_SysEvent;

typedef	struct _OOInst_SysEvent	OOInst_SysEvent;

//------------------------ Methods (0x0060-0x007F) --------------------------------

#define OOM_HANDLEEVENT	0x0060  // Method sent by Class_Application waiting for the received event

//------------------------ Attributes (0x0060-0x007F) --------------------------------

#define OOA_EventPort	(0x0060 | TAGT_OO | TAGT_LONG |TAGT_NUM)	// Port associated with an event

/* General use attributes (defined into oo_class.h)
	OOA_EventMask	// Parameter for methods OOM_SETEVENT, etc...
*/

//================================== HandleTimer ===============================

extern OOClass Class_HandleTimer;

typedef	struct _OOInst_HandleTimer		OOInst_HandleTimer;

typedef ULONG (*OOTimerCallback)( ULONG timer, ULONG clientdata );

//------------------------ Methods (0x0080-0x009F) --------------------------------

#define OOM_ADDTIMER		0x0080	// OOA_Delay, OOA_CallbackFunc, OOA_ClientData --> OOResult
#define OOM_REMOVETIMER		0x0081	// OOA_Timer

//------------------------ Attributes (0x0080-0x009F) --------------------------------

//#define OOA_	(0x0080 | TAGT_OO|)	// 

/* General use attributes (defined into oo_class.h)
	OOA_OnOff
*/

//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

OObject *	OO_NewV				( OOClass *cl, ULONG attr1, ... );
OObject *	OO_New				( OOClass *cl, OOTagList *attrlist );
ULONG		OO_Delete			( OObject *obj );
ULONG		OO_DoMethodV		( OObject *obj, ULONG mid, ULONG attr1, ... );
ULONG		OO_DoMethod			( OObject *obj, ULONG mid, OOTagList *tl );
ULONG		OO_DoClassMethodV	( OOClass *cl, OObject *obj, ULONG mid, ULONG attr1, ... );
ULONG		OO_DoClassMethod	( OOClass *cl, OObject *obj, ULONG mid, OOTagList *tl );
ULONG		OO_DoBaseMethodV	( OOClass *cl, OObject *obj, ULONG mid, ULONG attr1, ... );
ULONG		OO_DoBaseMethod		( OOClass *cl, OObject *obj, ULONG mid, OOTagList *tl );
ULONG		OO_DoChildrenMethodV( ULONG flags, OObject *obj, ULONG mid, ULONG attr1, ... );
ULONG		OO_DoChildrenMethod	( ULONG flags, OObject *obj, ULONG mid, OOTagList *tl );
ULONG		OO_DoNotifyV		( OObject *obj, ULONG attr1, ... );
ULONG		OO_DoNotify			( OObject *obj, OOTagList *taglist );
ULONG		OO_SetAttrsV		( OObject *obj, ULONG attr, ... );
ULONG		OO_SetAttrs			( OObject *obj, OOTagItem *taglist );
ULONG		OO_SetAttr			( OObject *obj, ULONG tag, ULONG data );
ULONG		OO_GetAttrsV		( OObject *obj, ULONG attr, ... );
ULONG		OO_GetAttrs			( OObject *obj, OOTagList *taglist );
ULONG		OO_GetAttr			( OObject *obj, ULONG tag );

OObject *   OO_NewTreeV			( OObject *parent, APTR ptrbase, ULONG itval, ... );
OObject *   OO_NewTree			( OObject *parent, APTR ptrbase, OOTreeItem *it );
ULONG       OO_ParseTree		( OObject *root, OObject *after, ULONG (*func)(void*,...), ULONG parm );
OObject *   OO_GetRootObject	( OObject *obj );
OOClass *   OO_GetRootClass		( OObject *obj );
OObject *	OO_GetParentOfClass	( OObject *obj, OOClass *cl );

ULONG		OO_AttachTarget		( OObject *obj, void *target );
void		OO_RemoveTarget		( OObject *obj, void *target );
void		OO_RemoveTargets	( OObject *obj );

ULONG		OO_AttachParent		( OObject *child, OObject *parent );
void		OO_DetachParent		( OObject *child, OObject *parent );
void		OO_DetachParents	( OObject *obj );
void		OO_DetachChildren	( OObject *obj );
void		OO_DetachAll		( OObject *obj );

void *	OO_AddNotify		( OObject *obj, ULONG trigtype, ULONG attr, ULONG min, ULONG max,
									OObject **pdest, ULONG actiontype, ULONG mid, OOTagList *taglist );
void *	OO_AddNotifyV		( OObject *obj, ULONG trigtype, ULONG attr, ULONG min, ULONG max,
									OObject **pdest, ULONG actiontype,ULONG mid, ULONG tagattr, ... );
void *	OO_AddNotifyItem	( OObject *obj, OONotifyItem *ni, APTR ptrbase );

void	OOAr_Init	( OObjArray *oa, BOOL table );
void	OOAr_Cleanup( OObjArray *oa );
ULONG	OOAr_Add	( OObjArray *oa, OObject *obj, ULONG blocksize );
void	OOAr_Remove	( OObjArray *oa, OObject *obj );
void	OOAr_Sweep	( OObjArray *oa, OObject *obj );

#endif // OO_CLASS_H
