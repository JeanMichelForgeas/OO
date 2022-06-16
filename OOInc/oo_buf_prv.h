#ifndef OO_BUF_PRV_H
#define OO_BUF_PRV_H

#include "oo_pool_prv.h"

struct _OOBuf {
    OOPool	*Pool;
    ULONG	TotSize;
    ULONG	CurSize;
    UWORD	BlkSize;
    UWORD	ClientWord;
    ULONG	ClientLong;
    };
typedef struct _OOBuf OOSBuf;

#define P2O(p) (((OOSBuf*)p)-1)  // L'utilisation de ces macros optimize beaucoup
#define O2P(o) ((char*)(o+1))  // le code généré par le SAS C sur Amiga

#endif // OO_BUF_PRV_H
