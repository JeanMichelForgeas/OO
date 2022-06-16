#ifndef OO_SOUND_PRV_H
#define OO_SOUND_PRV_H 1

#include "oo_sound.h"


typedef struct _OOSoundFileHeader_WAVE	OOSoundFileHeader_WAVE;


//=================================================================
#ifdef AMIGA


#endif // AMIGA
//=================================================================
#ifdef _WIN32
#ifdef OOV_WANTED_DIRECTSOUND

#include <dsound.h>

extern LPDIRECTSOUND DirectSoundObject;

#endif // OOV_WANTED_DIRECTSOUND
#endif // _WIN32


//======================== HandleSound ============================

struct _OOInst_HandleSound {
	ULONG	Flags;
	};


//======================== Sound ==================================

#define OOV_SOUND_BUFFERSIZE 10000
#define OOV_SOUND_MAXFORMAT		max( sizeof(PCMWAVEFORMAT), 0 )

struct _OOInst_Sound {
	//------ LoadFileFormat	(initialisé par class dérivée)
	UBYTE			Format[OOV_SOUND_MAXFORMAT];	// Wave : PCMWAVEFORMAT
	ULONG			DataOffset;		// Wave : sizeof(OOSoundFileHeader_WAVE)
	ULONG			DataSize;		// Wave : OOSoundFileHeader_WAVE.DataSize
	//------ PrepareBuffer
	OOSoundBuffer	*SoundBuf;
	ULONG			BufSize;
	//------ Play
	FILE			*File;
	ULONG			RestLength;
	ULONG			CurrVolume;
	UWORD			Flags;
	UBYTE			BufStatus[2];
	ULONG			EndPos;
    };
//------ Buffer status
#define OOV_SOUND_BUFEMPTY		0
#define OOV_SOUND_BUFLOADED		1
#define OOV_SOUND_BUFPLAYING	2
//------ Flags
#define OOF_SOUND_STREAM	0x0001
#define OOF_SOUND_STARTED	0x0002
//------ Macros 
#define OOSND(o)		((OOInst_Sound*)(((UBYTE*)o)+Class_Sound.InstOffset))


//======================== Sound Wave =============================

#pragma pack(1)
struct _OOSoundFileHeader_WAVE
{
	ULONG	Str_RIFF;		// "RIFF"
	ULONG	ChunkSize;		// Size of data to follow
	ULONG	Str_WAVE;		// "WAVE"
	ULONG	Str_fmt_;		// "fmt "
	ULONG	Value16;		// 16
	UWORD	Value1;			// 1
	UWORD	ChannelCount;	// Number of Channels
	ULONG	SampleRate;		// Sample Rate
	ULONG	BytesPerSec;	// Sample Rate
	UWORD	BlockAlign;		// 1
	UWORD	BitsPerSample;	// Sample size
	ULONG	Str_data;		// "data"
	ULONG	DataSize;		// Size of samples part
};
#pragma pack()

//=================================================================

#endif // OO_SOUND_PRV_H 
