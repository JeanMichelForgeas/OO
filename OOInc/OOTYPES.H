#ifndef OO_TYPES_H
#define OO_TYPES_H

#include <stddef.h> // pour macro offset() 
#include <stdio.h>	// pour sprintf() 
#include <malloc.h>	// _msize()
#include <assert.h>

//============================================//
#ifdef AMIGA

typedef signed char		SBYTE;
typedef signed short	SWORD;
typedef signed long		SLONG;
typedef UWORD			IDW;
typedef ULONG			IDL;

#define MINSBYTE	((char)0x80)        
#define MAXSBYTE	0x7f        
#define MINUBYTE	0        
#define MAXUBYTE	0xff        
#define MINSWORD	((short)0x8000)
#define MAXSWORD	0x7fff
#define MINUWORD	0
#define MAXUWORD	0xffff
#define MINSLONG	((long)0x80000000)
#define MAXSLONG	0x7fffffff
#define MINULONG	0
#define MAXULONG	0xffffffff

#define W2L(a,b)        ((ULONG)(((ULONG)((UWORD)(a))<<16)|((UWORD)(b))))
#define B2L(a,b,c,d)    ((ULONG)(((ULONG)((UBYTE)(a))<<24)|((ULONG)((UBYTE)(b))<<16)|((ULONG)((UBYTE)(c))<<8)|(ULONG)((UBYTE)(d))))
#define B2W(a,b)        ((UWORD)(((UWORD)((UBYTE)(a))<<8)|((UBYTE)(b))))

#define L2W(x,a,b)      {ULONG v=x; a=v>>16; b=((UWORD)v); }
#define L2B(x,a,b,c,d)  {ULONG v=x; a=v>>24; b=((UBYTE)v>>16); c=((UBYTE)v>>8); d=((UBYTE)v); }
#define W2B(x,a,b)      {UWORD v=x; a=v>> 8; b=((UBYTE)v); }

#define OORGB(r,g,b)	((ULONG)(((UBYTE)(b)|((UWORD)((UBYTE)(g))<<8))|(((ULONG)(UBYTE)(r))<<16)))

#define MAX(a,b)    ((a)>(b)?(a):(b))
#define MIN(a,b)    ((a)<(b)?(a):(b))
#define ABS(x)      ((x<0)?(-(x)):(x))

#ifdef _DEBUG
#define OOASSERT(f)	assert(f)
#define OOVERIFY(f)	assert(f)
#define OOTRACE		kprintf
#else   // _DEBUG
#define OOASSERT(f)	((void)0)
#define OOVERIFY(f)	((void)(f))
#define OOTRACE		1 ? (void)0 : kprintf
#define OOPRINTF	1 ? (void)0 : printf
#endif // !_DEBUG

#endif
//============================================//
#ifdef _WIN32

#define	NOCRYPT
#define	NOIME
//#define	NOGDICAPMASKS     // - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
//#define	NOVIRTUALKEYCODES // - VK_*
//#define	NOWINMESSAGES     // - WM_*, EM_*, LB_*, CB_*
//#define	NOWINSTYLES       // - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
//#define	NOSYSMETRICS      // - SM_*
//#define	NOMENUS           // - MF_*
//#define	NOICONS           // - IDI_*
//#define	NOKEYSTATES       // - MK_*
//#define	NOSYSCOMMANDS     // - SC_*
//#define	NORASTEROPS       // - Binary and Tertiary raster ops
//#define	NOSHOWWINDOW      // - SW_*
//#define	OEMRESOURCE       // - OEM Resource values
//#define	NOATOM            // - Atom Manager routines
#define	NOCLIPBOARD       // - Clipboard routines
//#define	NOCOLOR           // - Screen colors
//#define	NOCTLMGR          // - Control and Dialog routines
//#define	NODRAWTEXT        // - DrawText() and DT_*
//#define	NOGDI             // - All GDI defines and routines
//#define	NOKERNEL          // - All KERNEL defines and routines
//#define	NOUSER            // - All USER defines and routines
#define	NONLS             // - All NLS defines and routines
//#define	NOMB              // - MB_* and MessageBox()
#define	NOMEMMGR          // - GMEM_*, LMEM_*, GHND, LHND, associated routines
#define	NOMETAFILE        // - typedef METAFILEPICT
//#define	NOMINMAX          // - Macros min(a,b) and max(a,b)
//#define	NOMSG             // - typedef MSG and associated routines
#define	NOOPENFILE        // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
//#define	NOSCROLL          // - SB_* and scrolling routines
#define	NOSERVICE         // - All Service Controller routines, SERVICE_ equates, etc.
#define	NOSOUND           // - Sound driver routines
//#define	NOTEXTMETRIC      // - typedef TEXTMETRIC and associated routines
//#define	NOWH              // - SetWindowsHook and WH_*
//#define	NOWINOFFSETS      // - GWL_*, GCL_*, associated routines
#define	NOCOMM            // - COMM driver routines
#define	NOKANJI           // - Kanji support stuff.
#define	NOHELP            // - Help engine interface.
#define	NOPROFILER        // - Profiler interface.
#define	NODEFERWINDOWPOS  // - DeferWindowPos routines
#define	NOMCX             // - Modem Configuration Extensions

#include <windows.h>

#undef BYTE
#undef WORD
#undef LONG

typedef unsigned char	UBYTE;
typedef unsigned long	ULONG;
typedef unsigned short	UWORD;
typedef signed char		SBYTE;
typedef signed short	SWORD;
typedef signed long		SLONG;
typedef UWORD			IDW;
typedef ULONG			IDL;
typedef void *			APTR;

#define MINSBYTE	((char)0x80)        
#define MAXSBYTE	0x7f        
#define MINUBYTE	0        
#define MAXUBYTE	0xff        
#define MINSWORD	((short)0x8000)
#define MAXSWORD	0x7fff
#define MINUWORD	0
#define MAXUWORD	0xffff
#define MINSLONG	((long)0x80000000)
#define MAXSLONG	0x7fffffff
#define MINULONG	0
#define MAXULONG	0xffffffff

#define W2L(a,b)		((ULONG)(((UWORD)(a))|((DWORD)((UWORD)(b))<<16)))
#define B2L(a,b,c,d)    ((ULONG)(((ULONG)((UBYTE)(d))<<24)|((ULONG)((UBYTE)(c))<<16)|((ULONG)((UBYTE)(b))<<8)|(ULONG)((UBYTE)(a))))
#define B2W(a, b)		((UWORD)(((UBYTE)(a))|((UWORD)((UBYTE)(b))<<8)))

#define L2W(x,a,b)      {ULONG v=x; b=(UWORD)(v>>16); a=(UWORD)(v); }
#define L2B(x,a,b,c,d)  {ULONG v=x; d=(UBYTE)(v>>24); c=(UBYTE)(v>>16); b=(UBYTE)(v>>8); a=(UBYTE)(v); }
#define W2B(x,a,b)      {UWORD v=x; b=(UBYTE)(v>> 8); a=(UBYTE)(v); }

#define OORGB(r,g,b)	((ULONG)(((UBYTE)(r)|((UWORD)((UBYTE)(g))<<8))|(((ULONG)(UBYTE)(b))<<16)))

#define MAX(a,b)    ((a)>(b)?(a):(b))
#define MIN(a,b)    ((a)<(b)?(a):(b))
#define ABS(x)      ((x<0)?(-(x)):(x))

#ifdef _DEBUG
#define OOASSERT(f)	assert(f)
#define OOVERIFY(f)	assert(f)
#define OOTRACE		printf
#else   // _DEBUG
#define OOASSERT(f)	((void)0)
#define OOVERIFY(f)	((void)(f))
#define OOTRACE		1 ? (void)0 : printf
#endif // !_DEBUG

#endif
//============================================//
#ifdef _MAC

#endif
//============================================//

#define OF(s,v) (offsetof(struct s,v))

#define OOV_ENDTABLE	MAXULONG	// Fin des table de texte, notification, etc...

typedef UBYTE OOBuf;

//----------- Return codes

typedef ULONG	OORESULT;
// Return codes can be OK, ERROR, or values as pointers, etc...
#define	OOV_ERROR			0
#define	OOV_OK				MAXULONG
// Dialogs, etc...
#define OOV_CANCEL          1
// Functions can store their error codes or result values into the global variable OOResult
#define	OOV_ERR_NOMEMORY	2
// Set/Get attr
#define OOV_NOCHANGE		3
#define OOV_CHANGED			4

//----------- Global

#define	OO_ALL_ALLOC_POOLED	// Want objects allocs done into OOGlobalPool instead of malloc()

extern void *OOGlobalPool;
extern OORESULT OOResult;

#ifdef _WIN32
#ifndef _CONSOLE
extern HINSTANCE OOInstance;
extern int OOCmdShow;
#endif // _CONSOLE
#endif // _WIN32

#endif // OO_TYPES_H
