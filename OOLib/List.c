/*****************************************************************
 *
 *       Project:    OO
 *       Function:   List processing
 *
 *       Created:        Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1991-1997 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_list.h"

#include "oo_list_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/


/***************************************************************************
 *
 *      Code
 *
 ***************************************************************************/

void OOList_Init( OOList *list )
{
    list->lh_Head     = (OONode*)&list->lh_Tail;
    list->lh_Tail     = NULL;
    list->lh_TailPred = (OONode*)&list->lh_Head;
}

void OOList_AddHead( OOList *list, OONode *node )
{
  OONode *succ;

    succ = list->lh_Head;
    list->lh_Head = node;
    node->ln_Succ = succ;
    node->ln_Pred = (OONode*)list;
    succ->ln_Pred = node;
}

void OOList_AddTail( OOList *list, OONode *node )
{
  OONode *last, *pred;

    last = (OONode *)&list->lh_Tail;
    pred = last->ln_Pred;
    pred->ln_Succ = node;
    node->ln_Pred = pred;
    node->ln_Succ = last;
    last->ln_Pred = node;
}

void OOList_Remove( OONode *node )
{
  OONode *succ, *pred;

    succ = node->ln_Succ;
    pred = node->ln_Pred;
    pred->ln_Succ = succ;
    succ->ln_Pred = pred;
    node->ln_Succ = node->ln_Pred = (OONode*)0x0BAD0BAD;
}

void OOList_Insert( OOList *list, OONode *node, OONode *bef )
{
  OONode *pred;

    pred = bef->ln_Pred;
    node->ln_Succ = bef;
    node->ln_Pred = pred;
    pred->ln_Succ = node;
    bef->ln_Pred = node;
}

void OOList_InsertAfter( OOList *list, OONode *node, OONode *after )
{
  OONode *succ, *pred;

    if (after)
        {
        if (succ = after->ln_Succ)
            {
            node->ln_Succ = succ;
            node->ln_Pred = after;
            after->ln_Succ = node;
            succ->ln_Pred = node;
            }
        else{
            pred = after->ln_Pred;  /* The "Successor" field of the listNode was zero, i.e. listnode */
            node->ln_Succ = after;  /* is really the end of list marker.  In this case, insert just */
            node->ln_Pred = pred;   /* before listNode, i.e. at the end of the list (undocumented). */
            pred->ln_Succ = node;
            after->ln_Pred = node;
            }
        }
    else OOList_AddHead( list, node );
}

OONode *OOList_RemHead( OOList *list )
{
  OONode *succ, *node;

    node = list->lh_Head;
    succ = node->ln_Succ;
    if (succ == NULL) return NULL;
    list->lh_Head = succ;
    succ->ln_Pred = (OONode*)list;
    node->ln_Succ = node->ln_Pred = (OONode*)0x0BAD0BAD;
    return node;
}

OONode *OOList_RemTail( OOList *list )
{
  OONode *pred, *node;

    node = list->lh_TailPred;
    pred = node->ln_Pred;
    if (pred == NULL) return NULL;
    list->lh_TailPred = pred;
    pred->ln_Succ = (OONode*)&list->lh_Tail;
    node->ln_Succ = node->ln_Pred = (OONode*)0x0BAD0BAD;
    return node;
}

OONode *OOList_GetHead( OOList *list )
{
  OONode *node;

    node = list->lh_Head;
    if (node->ln_Succ == NULL) return NULL;
    return node;
}
