#ifndef OO_LIST_H
#define OO_LIST_H

struct _OONode {
    struct _OONode *ln_Succ;
    struct _OONode *ln_Pred;
};
typedef	struct _OONode OONode;

struct _OOList {
    struct _OONode *lh_Head;
    struct _OONode *lh_Tail;
    struct _OONode *lh_TailPred;
};
typedef	struct _OOList OOList;

#define OOList_Empty(x) (((x)->lh_TailPred) == (OONode*)(x) ? TRUE : FALSE)
#define DECLARE_OOLIST(x) static OOList (x) = { (OONode*)&(x).lh_Tail, NULL, (OONode*)&(x).lh_Head };

//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

#ifdef __cplusplus
extern "C" {
#endif

void			OOList_Init			( OOList *list );
void			OOList_AddHead		( OOList *list, OONode *node );
void			OOList_AddTail		( OOList *list, OONode *node );
void			OOList_Remove		( OONode *node );
void			OOList_Insert		( OOList *list, OONode *node, OONode *bef );
void			OOList_InsertAfter	( OOList *list, OONode *node, OONode *after );
OONode *		OOList_RemHead		( OOList *list );
OONode *		OOList_RemTail		( OOList *list );
OONode *		OOList_GetHead		( OOList *list );

#ifdef __cplusplus
	}
#endif

#endif // OO_LIST_H
