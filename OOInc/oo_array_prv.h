#ifndef OO_ARRAY_PRV_H
#define OO_ARRAY_ RP_VH
#include "oo_pool_prv.h"

struct _OOArray {
    OOPool	*Pool;
    ULONG	TotEntries;
    ULONG	CurEntries;
    ULONG	BlkEntries;
	};
typedef struct _OOArray OOArray;

#define A2O(p) (((OOArray*)p)-1)
#define O2A(o) ((ULONG*)(o+1))

#endif // OO_ARRAY_PRV_H
