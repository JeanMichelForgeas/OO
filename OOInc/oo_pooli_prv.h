#ifndef OO_POOLI_PRV_H
#define OO_POOLI_PRV_H

#include "oo_list.h"

#define MEMPOOL_USE_ID_TABLE

struct _OOChunk {
    ULONG	OffsNext;	// pointer to next chunk 
    ULONG	Bytes;		// chunk byte size  
};
typedef struct _OOChunk OOChunk;

#define LASTFREECHUNK 0xffffffff

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

#define OOV_MEMPOOL_MAXCACHE	100		
			 
struct _OOPoolHeader {
    OOList	List;
    ULONG   PuddleSize;
	ULONG	AllocCount;
    UBYTE	DebugFlags;
    UBYTE	Flags;

	UBYTE	FreeSlotsCached;
	UBYTE	MaxCacheUsed;
    ULONG *	IdTable; // id 0 is invalid and always NULL 
    ULONG	LastId;  // Valid ids are from 1 to m_LastId included. 0 is an invalid id.
    ULONG	NumFreeSlots; // Number of free slots into the array of pointers
	ULONG	SlotsCache[OOV_MEMPOOL_MAXCACHE];
	};
typedef struct _OOPoolHeader OOPool;

#define OOIDOFFSET	4	// Rangement du Id dans l'allocation

#endif // OO_POOLI_PRV_H
