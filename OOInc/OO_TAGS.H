#ifndef OO_TAGS_H
#define OO_TAGS_H

typedef	struct _OOTagItem OOTagItem;
typedef	struct _OOTagItem OOTagList;
typedef	struct _OOFltTagItem OOFltTagItem;
typedef	struct _OOFltTagItem OOFltTagList;

typedef union
	{
	ULONG	t32;
	struct {
		ULONG	tagval		:29;	// ti.id + ti.owner + ti.type + ti.typeinfo + ti.dispinfo + ti.dispflag
		ULONG	reserved	: 3;
		}	tv;
	struct {
		// Forment la valeur unique ti_Tag
		ULONG	id			:16;
		ULONG	owner		: 2;	// 4 propriétaires de tags : 1 public et 3 privés
		ULONG	sign		: 1;	// 1 signé ou non
		ULONG	type		: 3;	// 3 bits pour 8 types
		ULONG	typeinfo	: 2;	// 4 variations de type
		ULONG	dispinfo	: 2;	// 4 types de controles d'affichage pour le type
		ULONG	dispflag	: 3;	// variations
		// Informations complémentaires
		ULONG	isbuf		: 1;	// Indique si c'est un OOBuf (qui indique sa longueur, et qui est retaillable)
		ULONG	ignore		: 1;	// Le tag doit être ignoré
		ULONG	tmp			: 1;
		}	ti;
	} OOTag;

//#define TAGM_TAGBITS	0x1fffffff	// 2 + 3 + 2 + 16 == typeinfo + type + owner + id
#define TAGM_TAGBITS	(TAGMASK_ID | TAGMASK_OWNER | TAGMASK_SIGNED | TAGMASK_TYPE | TAGMASK_TINFO | TAGMASK_DINFO | TAGMASK_DFLAGS)

typedef union
	{
	ULONG		dat	;	
	BOOL		boo	;	
	UBYTE		byt	;	  
	SWORD		wrd	;
	SLONG		lng	;  
	FLOAT		flt ;
	char *		str	;
	void *		ptr	;
	void	  (*fnc)();
	OOTagList*	mor ;
	} OOData;

// For initializing
struct _OOFltTagItem {
	OOTag	Tag;
	FLOAT	Data;
    };
// For general use
struct _OOTagItem {
	OOTag	Tag;
	OOData	Data;
    };

// ti_Tag : useful names to use
#define ti_Tag32	Tag.t32
#define ti_Tag		Tag.tv.tagval
#define ti_Id		Tag.ti.id
#define ti_Owner	Tag.ti.owner
#define ti_Type		Tag.ti.type
#define ti_TypeInfo	Tag.ti.typeinfo
#define ti_DispInfo	Tag.ti.dispinfo
#define ti_DispFlag	Tag.ti.dispflag
#define ti_IsBuf	Tag.ti.isbuf
#define ti_Ignore	Tag.ti.ignore
#define ti_Tmp		Tag.ti.tmp
// ti_Data : useful names to use
#define ti_Data		Data.dat		
#define ti_Bool		Data.boo		
#define ti_Byte		Data.byt	
#define ti_Word		Data.wrd	
#define ti_Long		Data.lng	
#define ti_Float	Data.flt	
#define ti_String	Data.str	
#define ti_Ptr		Data.ptr	
#define ti_Func		Data.fnc		
#define ti_More		Data.mor		

/*
0x00020003 | TAGT_OO | TAGT_OBJ  | TAGT_PTR)
0000   0001   1111 1010
*/

#define TAGMASK_ID	(0xffffL)	 

// Tag Owner : 2 bits donnent 4 domaines de collision possibles (OO == 0)
#define TAGMASK_OWNER	(0x3L << (16))	 
#define TAGT_LOC	(0L << (16))	// Local au programe
#define TAGT_PUB	(1L << (16))	// Public
#define TAGT_OO		(2L << (16))	// Réservé à OO
#define TAGT_OWN	(3L << (16))	// 

// Tag Type : premier bit indique si nombre signé
#define TAGMASK_SIGNED	( 1L << (16+2))	 
#define TAGT_SIGNED	( 1L << (16+2))	 
// Tag Type : les 3 bits suivants donnent 8 types possibles
#define TAGMASK_TYPE	(0x7L << (16+2+1))	 
#define TAGT_DATA	( 0L << (16+2+1)) // défaut : considéré comme binaire quelconque
#define TAGT_BOOL	( 1L << (16+2+1)) // Booléen : TRUE ou FALSE 
#define TAGT_BYTE	( 2L << (16+2+1)) // Un nombre
#define TAGT_WORD	( 3L << (16+2+1)) // Un nombre
#define TAGT_LONG	( 4L << (16+2+1)) // Un nombre
#define TAGT_FLOAT	( 5L << (16+2+1)) // Un nombre flottant sur 32 bits 
#define TAGT_DATE	( 6L << (16+2+1)) // Une date
#define TAGT_OBJ	( 7L << (16+2+1)) // Un objet
// Tag TypeInfo : 2 bits donnent 4 variations pour le type
#define TAGMASK_TINFO	(0x3L << (16+2+4))	 
#define TAGT_NUM	(0L << (16+2+4)) // On manipule un simple nombre
#define TAGT_STR	(1L << (16+2+4)) // On a affaire à une chaîne du type finie par 0 : UBYTE ou UWORD pour Unicode 
#define TAGT_LST	(2L << (16+2+4)) // On a affaire à une liste de chaînes du type séparées par 0 et finie par 2 0
#define TAGT_PTR	(3L << (16+2+4)) // Pointeur sur un des types

// Tag DispInfo : 2 bits 
#define TAGMASK_DINFO	(0x3L << (16+2+4+2))	 
#define TAGT_EDIT	(0L << (16+2+4+2)) // Une zone d'édition ou d'affichage (si readonly)
#define TAGT_COMB	(1L << (16+2+4+2)) // Une combo
#define TAGT_LIST	(2L << (16+2+4+2)) // Une liste
#define TAGT_BUTN	(3L << (16+2+4+2)) // Un bouton : check si MULT, radio si MUEX, ou plein
// Tag DispFlags : 3 bits
#define TAGMASK_DFLAGS (0x7L << (16+2+4+2+2))	 
#define TAGF_MULT	((1L<<0) << (16+2+4+2+2)) // Multi sélection pour liste ou bien check si bouton
#define TAGF_READ	((1L<<1) << (16+2+4+2+2)) // Read only pour un edit, ou disable pour un autre controle
#define TAGF_MUEX	((1L<<2) << (16+2+4+2+2)) // Mutual exclude
// Equivalences
#define TAGT_STATIC		(TAGT_EDIT|TAGF_READ)
#define TAGT_BUTTON		TAGT_BUTN
#define TAGT_RADIO		(TAGT_BUTN|TAGF_MUEX)	// TAGT_LIST pour le titre
#define TAGT_CHECK		(TAGT_BUTN|TAGF_MULT)	// TAGT_LIST pour le titre
#define TAGF_DOUBLEWORD	(TAGF_MULT)

// Tag Ignore : 1 bits
#define TAGMASK_IGNORE	(1L << (16+2+4+2+2+3+1))	 
									  
// Tag Tmp : 1 bits					  
#define TAGMASK_TMP		(1L << (16+2+4+2+2+3+1+1))	 

// Valeurs spéciales pour les fonctions de TagList 
#define OOV_TAG_END     0 // terminates array of TagItems. ti_Data used as continuation if != NULL
#define OOV_TAG_MORE	OOV_TAG_END
#define TAG_END			OOV_TAG_END,(ULONG)NULL
#define TAGFLT_END		OOV_TAG_END,(FLOAT)(ULONG)NULL
#define OOV_FILTER_AND			0	// exclude everything but filter hits 
#define OOV_FILTER_NOT			1	// exclude only filter hits    
#define OOV_FILTER_KEEP			0	// do not update original list
#define OOV_FILTER_UPDATE		1	// update original list
#define OOV_MAP_REMOVE_NOTFOUND	0	// remove tags that aren't in mapList 
#define OOV_MAP_KEEP_NOTFOUND	1	// keep tags that aren't in mapList   

// Tag macros
#define OOTAGEQUAL(a,b)			(a->ti_Tag == b->ti_Tag)
#define OOTIDEQUAL(a,b)			(a->ti_Id == b->ti_Id)
#define OOTYPEQUAL(a,b)			((a->ti_Tag32 & (TAGMASK_TYPE|TAGMASK_TINFO)) == (b->ti_Tag32 & (TAGMASK_TYPE|TAGMASK_TINFO)))
#define OOTAGISBUF(a)			(a->ti_IsBuf == 1)
#define OOTAGISBIN(a)			((a->ti_Tag32 & TAGMASK_TYPE) == TAGT_DATA)
#define OOTAGISNUM(a)			((a->ti_Tag32 & TAGMASK_TINFO) == TAGT_NUM)
#define OOTAGISSTR(a)			((a->ti_Tag32 & TAGMASK_TINFO) == TAGT_STR || (a->ti_Tag32 & TAGMASK_TINFO) == TAGT_LST)
#define OOTAGISFLT(a)			((a->ti_Tag32 & TAGMASK_TYPE) == TAGT_FLOAT)
#define OOTAGDATAEQUAL(t1,t2)	((((t1)->ti_Tag32 & TAGMASK_TINFO) == TAGT_NUM && (t1)->ti_Data == (t2)->ti_Data) ? TRUE : OOTag_DataEqual( t1, t2 ))

#define OOTAG_GETITEM(ti)								\
	{ LOOP:												\
	if (ti->ti_Tag32 == OOV_TAG_END)					\
		{ if ((ti = ti->ti_More) != NULL) goto LOOP; }	\
	else if (ti->ti_Tag32 & TAGMASK_IGNORE)				\
		{ ti++;	goto LOOP; } }

#define OOTAG_GETITEM_TMP(ti)							\
	{ LOOP:												\
	if (ti->ti_Tag32 == OOV_TAG_END)					\
		{ if ((ti = ti->ti_More) != NULL) goto LOOP; }	\
	else if (ti->ti_Tag32 & (TAGMASK_IGNORE|TAGMASK_TMP))\
		{ ti++;	goto LOOP; } }

#define OOTAG_FOR_EACH_ITEM(a,b)		\
	{ OOTagItem *b;						\
	for (b=a; ; b++) {					\
		OOTAG_GETITEM(b);				\
	    if (b == NULL) break;
#define OOTAG_FOR_EACH_ITEM_END }}

//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

OOTagItem *		OOTag_NextItem		( OOTagItem **ti_addr );
BOOL			OOTag_InArray		( ULONG *tagarray, ULONG tag );
OOTagItem *		OOTag_FindTag		( OOTagList *ti, ULONG tag );
OOTagItem *		OOTag_FindTagIndex	( OOTagList *ti, ULONG tag, ULONG *indexaddr );
ULONG			OOTag_GetData		( OOTagList *ti, ULONG tag, ULONG defaultdata );
OOTagItem *		OOTag_SetData		( OOTagItem *tl, ULONG tag, ULONG newdata );
BOOL			OOTag_DataEqual		( OOTagItem *t1, OOTagItem *t2 );
ULONG			OOTag_FilterList	( OOTagList *ti, ULONG *tagarray, ULONG logic );
void			OOTag_FilterChanges	( OOTagList *change_tl, OOTagList *oldvalues, ULONG apply );
ULONG			OOTag_ApplyList		( OOTagList *tl, ULONG (*Func)(OOTagItem*,ULONG), ULONG funcparm, ULONG testrc, ULONG testvalue );
ULONG			OOTag_ApplyFind		( OOTagList *ti, OOTagList *find_tl, ULONG (*Perform)(OOTagList*,OOTagItem*,ULONG), ULONG parm );
ULONG			OOTag_PackBool		( OOTagList *tl, OOTagList *boolmap, ULONG initial_flags );
void			OOTag_ApplyChanges	( OOTagList *tl, OOTagList *change_tl );
void			OOTag_MapTags		( OOTagItem *tl, OOTagItem *maplist, ULONG include_miss );
void			OOTag_Validate		( OOTagList *ti );
OOTagList *		OOTag_Alloc			( ULONG numitems );
void			OOTag_Free			( OOTagList *taglist );
ULONG			OOTag_GetCount		( OOTagList *tl );
OOTagList *		OOTag_Clone			( OOTagList *tl_src );
void			OOTag_Copy			( OOTagList *ti, OOTagList *dest_tl );
OOTagList *		OOTag_Merge			( OOTagList *tl, OOTagList *Next );
OOTagItem *		OOTag_Join			( OOTagList *ti, OOTagList *tl2 );
OOTagList *		OOTag_Split			( OOTagItem *ti );
UBYTE *			OOTag_SetString		( OOTagItem *ti, UBYTE *str );
ULONG			OOTag_FreeBuffers	( OOTagList *tl );

#endif // OO_TAGS_H
