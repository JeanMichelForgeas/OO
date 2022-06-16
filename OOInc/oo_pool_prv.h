#ifndef OO_MEM_PRV_H
#define OO_MEM_PRV_H

#include "oo_list.h"

struct _OOChunk {
    ULONG	OffsNext;	// pointer to next chunk 
    ULONG	Bytes;		// chunk byte size  
};
typedef struct _OOChunk OOChunk;

#define LASTFREECHUNK 0xffffffffL

struct _OOPuddle {     // C'est une struct MemHeader 
    struct _OOPuddle *ln_Succ;
    struct _OOPuddle *ln_Pred;
    ULONG		TotalSize;
    UWORD		padw;
    UWORD		DebugFlags;
    ULONG		mh_OffsFirstChunk;	// $10 first free region (OOChunk) 
    ULONG		mh_OffsLower;		// $14 lower memory bound       
    ULONG		mh_OffsUpper;		// $18 upper memory bound+1 
    ULONG		mh_Free;			// $1C total number of free bytes   
	};
typedef struct _OOPuddle OOPuddle;

// Flags pour le pool
#define OOF_MEMPOOL_DISABLEFREE	0x01

struct _OOPoolHeader {
    OOList	List;
    ULONG   PuddleSize;
	ULONG	AllocCount;
    UBYTE	DebugFlags;
    UBYTE	Flags;
	};
typedef struct _OOPoolHeader OOPool;

#define OOIDOFFSET	0	// Pas de rangement de Id dans l'allocation

#endif // OO_MEM_PRV_H
