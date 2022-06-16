#ifndef OO_SOUND_H
#define OO_SOUND_H 1

#include "ootypes.h"
#include "oo_class.h"


#ifdef _WIN32
#define OOV_WANTED_DIRECTSOUND // <<<<<<<<<<<<<<<
#endif


typedef	struct _OOInst_HandleSound	OOInst_HandleSound;
typedef struct _OOInst_Sound		OOInst_Sound;

//=================================================================
#ifdef AMIGA

typedef void OOSoundBuffer;

#endif

//=================================================================

#ifdef _WIN32
#ifdef OOV_WANTED_DIRECTSOUND

typedef struct IDirectSoundBuffer OOSoundBuffer;

#endif // OOV_WANTED_DIRECTSOUND
#endif

//=================================================================

//------------- General use attributes (0x4000-0x41FF) ------------

#define OOA_HandleSound		(0x4000|TAGT_OO|TAGT_OBJ |TAGT_PTR)
#define OOA_SoundFileName	(0x4001|TAGT_OO|TAGT_BYTE|TAGT_STR|TAGT_EDIT)	// Nom du fichier de son
#define OOA_SoundVolume		(0x4002|TAGT_OO|TAGT_LONG|TAGT_NUM|TAGT_EDIT)	// Volume du son quand il est tout près du micro
#define OOA_SoundFormat		(0x4003|TAGT_OO|TAGT_BYTE|TAGT_PTR)				// Format du son, rempli d'après l'entête du fichier

//-----------------------------------------------------------------


//=================================================================

// For Speed these methods expect some tags in the order below :

//======================== HandleSound ============================

//------------------------ Methods (0x4200-0x421F) ----------------

#define OOM_STARTPLAY		0x4200  // 
#define OOM_STOPPLAY		0x4201  // 

//------------------------ Attributes (0x4200-0x421F) -------------

// OOA_OnOff : Allumé / éteint


//======================== Sound ==================================

//------------------------ Methods (0x4220-0x423F) ----------------

#define	OOM_LOADNEXT	0x4220

//------------------------ Attributes (0x4220-0x423F) -------------

// OOA_OnOff : Allumé / éteint
#define OOA_Orphan 			(0x4220|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK) // Pour deleter OOSound automatiquement quand le son se fini
#define OOA_SoundLoop		(0x4221|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_SoundStreamed	(0x4222|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)
#define OOA_SoundPreLoad	(0x4223|TAGT_OO|TAGT_BOOL|TAGT_NUM|TAGT_CHECK)


//======================== Sound Wave =============================

//------------------------ Methods (0x4240-0x425F) ----------------

//------------------------ Attributes (0x4240-0x425F) -------------

// OOA_OnOff : Allumé / éteint


//=================================================================

ULONG	OO_InitSound		( OOTagList * );
void	OO_CleanupSound		( void );

#ifdef _WIN32
#ifdef OOV_WANTED_DIRECTSOUND
ULONG OO_InitSillyDirectSound( void );
#endif // OOV_WANTED_DIRECTSOUND
#endif // _WIN32

/*
OOSound *	OOSound_NewFromFile	( UBYTE *filename );
OOSound *	OOSound_NewFromMem	( void *mem );
void		OOSound_Delete		( OOSound *snd );
ULONG		OOSound_Play		( OOSound *snd, ULONG volume, ULONG span );
ULONG		OOSound_Stop		( OOSound *snd );
ULONG		OOSound_Update		( OOSound *snd, ULONG flags );
*/
extern OOClass Class_HandleSound;
extern OOClass Class_Sound;

#endif // OO_SOUND_H 
