/*****************************************************************
 *
 *		 Project:	 OO
 *		 Function:	 
 *
 *		 Created:		 Jean-Michel Forgeas
 *
 *		 This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_sound_prv.h"

#define INDEX_OOA_OnOff			0
#define INDEX_OOA_Orphan		1
#define INDEX_OOA_SoundFileName	2
#define INDEX_OOA_SoundVolume	3
#define INDEX_OOA_SoundLoop		4
#define INDEX_OOA_SoundStreamed	5
#define INDEX_OOA_SoundPreLoad	6


/******* Imported ************************************************/


/******* Exported ************************************************/


/******* Statics *************************************************/

static ULONG	OOSound_StartPlay	( OOClass *cl, OObject *obj, OObject *handlesound );
static void		OOSound_StopPlay	( OOClass *cl, OObject *obj );
static ULONG	OOSound_LoadNext	( OOClass *cl, OObject *obj );

static ULONG	OOSound_AllocBuffer	( OOClass *cl, OObject *obj );
static void		OOSound_FreeBuffer	( OOInst_Sound *inst );
static ULONG	OOSound_PrepareBuffer( OOClass *cl, OObject *obj );
static ULONG	OOSound_ReadBuffer	( OOClass *cl, OObject *obj, ULONG start, ULONG length );

static ULONG	OOSound_LoadFileFormat( OOClass *cl, OObject *obj, OOTagList *attrlist );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_Sound( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
				{
				OOInst_Sound *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				inst->DataSize = 0;
				if (OOSound_LoadFileFormat( cl, obj, attrlist ) == OOV_ERROR)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); return (ULONG)NULL; }
				// Init default attrs & change defaults with new values
				if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR)
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			return (ULONG)obj;

		case OOM_DELETE:
			OOSound_StopPlay( cl, obj );
			OOSound_FreeBuffer( OOINST_DATA(cl,obj) ); // Utile si PreLoad
			return OO_DoBaseMethod( cl, obj, method, attrlist );

		//---------------
		case OOM_GET:
			// Do not call base method
			return OOPrv_MGet( cl, obj, attrlist );

		case OOM_SET:
			// Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

		//---------------
		case OOM_STARTPLAY:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_HandleSound );
			if (OOSound_StartPlay( cl, obj, attrlist[0].ti_Ptr ) == OOV_ERROR)	return OOV_ERROR;
			OO_SetAttr( obj, OOA_OnOff, TRUE );
			break;

		case OOM_STOPPLAY:
			OOSound_StopPlay( cl, obj );
			OO_SetAttr( obj, OOA_OnOff, FALSE );
			break;

		//---------------
		case OOM_LOADNEXT:
			return OOSound_LoadNext( cl, obj );

		//---------------
		default:
			return OO_DoBaseMethod( cl, obj, method, attrlist );
		}
	return OOV_OK;
}


/***************************************************************************
 *
 *		Utilities
 *
 ***************************************************************************/

static ULONG OOSound_StartPlay( OOClass *cl, OObject *obj, OObject *handlesound )
{
	OOInst_Sound *inst = OOINST_DATA(cl,obj);
	ULONG playflags = 0;

	if (inst->Flags & OOF_SOUND_STARTED) return OOV_ERROR;

	// OOTRACE( "++++++ STARTING SOUND\n" );
	if (OOINST_ATTR(cl,obj,INDEX_OOA_SoundPreLoad) == FALSE)
		{
		if (OOSound_AllocBuffer( cl, obj ) == OOV_ERROR) goto END_ERROR;
		if (OOSound_PrepareBuffer( cl, obj ) == OOV_ERROR) goto END_ERROR;
		}
	if (OOINST_ATTR(cl,obj,INDEX_OOA_SoundLoop) == TRUE || (inst->Flags & OOF_SOUND_STREAM)) playflags = DSBPLAY_LOOPING;
	inst->CurrVolume = OOINST_ATTR(cl,obj,INDEX_OOA_SoundVolume);
	IDirectSoundBuffer_SetVolume( inst->SoundBuf, inst->CurrVolume );
	if (IDirectSoundBuffer_Play( inst->SoundBuf, 0, 0, playflags ) != DS_OK) goto END_ERROR;

	// Pour jouer en lecture continue depuis le fichier
	if (OO_AttachParent( obj, handlesound ) == OOV_ERROR) goto END_ERROR;

	inst->Flags |= OOF_SOUND_STARTED;
	return OOV_OK;

END_ERROR:
	OOTRACE( "SOUND ERROR\n" );
	if (OOINST_ATTR(cl,obj,INDEX_OOA_SoundPreLoad) == FALSE)
		{
		OOSound_FreeBuffer( inst );
		inst->RestLength = 0;
		}
	return OOV_ERROR;
}

static void OOSound_StopPlay( OOClass *cl, OObject *obj )
{
	OOInst_Sound *inst = OOINST_DATA(cl,obj);
	ULONG status;

	if (! (inst->Flags & OOF_SOUND_STARTED)) return;

	OO_DetachParents( obj );

	if (IDirectSoundBuffer_GetStatus( inst->SoundBuf, &status ) != DS_OK || (status & DSBSTATUS_PLAYING))
		IDirectSoundBuffer_Stop( inst->SoundBuf );

	if (OOINST_ATTR(cl,obj,INDEX_OOA_SoundPreLoad) == FALSE)
		{
		OOSound_FreeBuffer( inst );
		inst->RestLength = 0;
		}
	inst->Flags &= ~OOF_SOUND_STARTED;

	if (OOINST_ATTR(cl,obj,INDEX_OOA_Orphan) == TRUE) OO_DoBaseMethod( cl, obj, OOM_DELETE, NULL );
}

static ULONG OOSound_LoadNext( OOClass *cl, OObject *obj )
{
	OOInst_Sound *inst = OOINST_DATA(cl,obj);
	ULONG volume, status;

	OOASSERT( (inst->Flags & OOF_SOUND_STARTED) );
	OOASSERT( (inst->SoundBuf != NULL) );

	//----- Modify real-time volume
	volume = OOINST_ATTR(cl,obj,INDEX_OOA_SoundVolume);
	if (volume != inst->CurrVolume) 
		{
		// OOTRACE( "LOADEDNEXT -- VOLUME : old=%d new=%d\n", inst->CurrVolume, volume );
		IDirectSoundBuffer_SetVolume( inst->SoundBuf, inst->CurrVolume = volume );
		}

	if (IDirectSoundBuffer_GetStatus( inst->SoundBuf, &status ) != DS_OK) return OOV_ERROR;

	if (inst->Flags & OOF_SOUND_STREAM)
		{
		//------ STREAMED SOUND : LOAD NEXT BUFFER ------------------
		// Si le buffer n'est pas en train d'être joué C'est qu'il vient de se terminer
		if (! (status & DSBSTATUS_PLAYING))
			{
			OOTRACE( "LOADEDNEXT -- STREAMED : synchronize because not playing !!!!!!!!\n" );
			OOASSERT (FALSE); // On ne devrait jamais arriver ici...
			OOSound_StopPlay( cl, obj );
			}
		else{
			ULONG playpos, writepos, playbuf, otherbuf, start, len;
			IDirectSoundBuffer_GetCurrentPosition( inst->SoundBuf, &playpos, &writepos );
			playbuf = (playpos < inst->BufSize/2) ? 0 : 1;
			otherbuf = (playpos < inst->BufSize/2) ? 1 : 0;

			len = 0;
			switch (inst->BufStatus[playbuf])
				{
				case OOV_SOUND_BUFEMPTY:
					start = playpos;
					len = (playbuf == 0) ? inst->BufSize/2 - start : inst->BufSize - start;
					break;
				case OOV_SOUND_BUFLOADED:
					inst->BufStatus[playbuf] = OOV_SOUND_BUFPLAYING;
					break;
				case OOV_SOUND_BUFPLAYING:
					if (playpos >= inst->EndPos)
						{
						// OOTRACE( "\n------ SOUND FINISHED\n" );
						if (OOINST_ATTR(cl,obj,INDEX_OOA_SoundLoop) == FALSE) 
							{
							OOSound_StopPlay( cl, obj );
							goto END_ERROR;
							}
						}
					break;
				}
			if (len != 0)
				{
				if (OOSound_ReadBuffer( cl, obj, start, len ) == OOV_ERROR) goto END_ERROR;
				inst->BufStatus[playbuf] = OOV_SOUND_BUFLOADED;
				// OOTRACE( "LOADEDNEXT -- STREAMED (%d) : 1 start=%d, len=%ld\n", playbuf, start, len );
				}

			len = 0;
			switch (inst->BufStatus[otherbuf])
				{
				case OOV_SOUND_BUFEMPTY:
					start = (otherbuf == 0) ? 0 : inst->BufSize/2;
					len = inst->BufSize/2;
					break;
				case OOV_SOUND_BUFLOADED:
					break;
				case OOV_SOUND_BUFPLAYING:
					inst->BufStatus[otherbuf] = OOV_SOUND_BUFEMPTY;
					break;
				}
			if (len != 0)
				{
				if (OOSound_ReadBuffer( cl, obj, start, len ) == OOV_ERROR) goto END_ERROR;
				inst->BufStatus[otherbuf] = OOV_SOUND_BUFLOADED;
				// OOTRACE( "LOADEDNEXT -- STREAMED (%d) : 2 start=%d, len=%ld\n", otherbuf, start, len );
				// Si la fin du fichier vient d'être lue, on prépare pour la prochaine lecture
				if (inst->EndPos != MAXULONG) 
					{
					if (OOINST_ATTR(cl,obj,INDEX_OOA_SoundLoop) == TRUE)
						{
						if (inst->File != NULL) fseek( inst->File, inst->DataOffset, SEEK_SET );
						inst->RestLength = inst->DataSize;
						start = inst->EndPos;
						len = (otherbuf == 0) ? inst->BufSize/2 - inst->EndPos : inst->BufSize - inst->EndPos;
						if (len != 0)
							{
							if (OOSound_ReadBuffer( cl, obj, start, len ) == OOV_ERROR) goto END_ERROR;
							// OOTRACE( "LOADEDNEXT -- RESTART STREAM (%d) : 2 start=%d, len=%ld\n", otherbuf, start, len );
							}
						inst->EndPos = MAXULONG;
						}
					}
				}
			}
		}
	else{
		//------ STATIC SOUND : NOTHING TO LOAD ---------------------
		// Si le buffer n'est pas en train d'être joué C'est qu'il vient de se terminer
		if (! (status & DSBSTATUS_PLAYING))
			{
			// OOTRACE( "LOADEDNEXT -- STATIC : STOPPING...\n" );
			OOSound_StopPlay( cl, obj );
			}
		}

END_ERROR:
	return OOV_OK;
}

//--------------------------------------------------------------------------

static ULONG OOSound_AllocBuffer( OOClass *cl, OObject *obj )
{
	OOInst_Sound *inst = OOINST_DATA(cl,obj);
	DSBUFFERDESC BD;
	ULONG rc = OOV_ERROR;

	OOASSERT (inst->SoundBuf == NULL);
	OOASSERT (inst->DataSize != 0);

	OOTRACE( "ALLOC BUFFER\n" );
	if (inst->DataSize <= OOV_SOUND_BUFFERSIZE || (OOINST_ATTR(cl,obj,INDEX_OOA_SoundStreamed) == FALSE))
		{
		inst->BufSize = inst->DataSize;
		}
	else{
		inst->Flags |= OOF_SOUND_STREAM;
		inst->BufSize = OOV_SOUND_BUFFERSIZE;
		}

	inst->File = NULL;

	memset( &BD, 0, sizeof(DSBUFFERDESC) );
	BD.dwSize			= sizeof(BD);
	BD.dwFlags			= DSBCAPS_CTRLDEFAULT | ((inst->Flags & OOF_SOUND_STREAM) ? 0 : DSBCAPS_STATIC);
	BD.dwBufferBytes	= inst->BufSize; 
	BD.lpwfxFormat		= (WAVEFORMATEX*)inst->Format;
	if (IDirectSound_CreateSoundBuffer( DirectSoundObject, &BD, &inst->SoundBuf, NULL ) == DS_OK)
		rc = OOV_OK;

	return rc;
}

static void OOSound_FreeBuffer( OOInst_Sound *inst )
{
	OOTRACE( "FREE BUFFER\n" );
    if (inst->File != NULL) { fclose( inst->File ); inst->File = NULL; }
	if (inst->SoundBuf != NULL) { IDirectSoundBuffer_Release( inst->SoundBuf ); inst->SoundBuf = NULL; }
}

static ULONG OOSound_PrepareBuffer( OOClass *cl, OObject *obj )
{
	OOInst_Sound *inst = OOINST_DATA(cl,obj);

	OOASSERT (inst->File == NULL);

	if ((inst->File = fopen( (UBYTE*)OOINST_ATTR(cl,obj,INDEX_OOA_SoundFileName), "rb" )) == NULL) goto END_ERROR;
	if (fseek( inst->File, inst->DataOffset, SEEK_SET ) != 0) goto END_ERROR;
	inst->RestLength = inst->DataSize;
	inst->BufStatus[0] = OOV_SOUND_BUFEMPTY;
	inst->BufStatus[1] = OOV_SOUND_BUFEMPTY;
	inst->EndPos = MAXULONG;

	if (inst->Flags & OOF_SOUND_STREAM)
		{
		if (OOSound_ReadBuffer( cl, obj, 0, inst->BufSize / 2 ) == OOV_ERROR) goto END_ERROR;
		inst->BufStatus[0] = OOV_SOUND_BUFLOADED;
		}
	else{
		if (OOSound_ReadBuffer( cl, obj, 0, inst->BufSize ) == OOV_ERROR) goto END_ERROR;
		fclose( inst->File );
		inst->File = NULL;
		}

	return OOV_OK;

END_ERROR:
    if (inst->File != NULL) { fclose( inst->File ); inst->File = NULL; }
	return OOV_ERROR;
}

static ULONG OOSound_ReadBuffer( OOClass *cl, OObject *obj, ULONG start, ULONG length )
{
	OOInst_Sound *inst = OOINST_DATA(cl,obj);
	BOOL locked = FALSE;
	HRESULT result;
	ULONG readlen=0, lentoread, size1, size2;
	void *data1, *data2;
	ULONG rc = OOV_ERROR;

	if (length >= inst->RestLength)
		{
		length = inst->RestLength;
		inst->EndPos = start + length; // Fin du sample dans le buffer
		if (length == 0)
			{
			// OOTRACE( "READ BUFFER : RestLength=0, plus rien à charger\n" );
			return OOV_OK;
			}
		}

	result = IDirectSoundBuffer_Lock( inst->SoundBuf, start, length, &data1, &size1, &data2, &size2, 0 );
	if (result == DSERR_BUFFERLOST)
		{
		IDirectSoundBuffer_Restore(	inst->SoundBuf );
		result = IDirectSoundBuffer_Lock( inst->SoundBuf, start, length, &data1, &size1, &data2, &size2, 0 );
		}
	if (result != DS_OK) goto END_ERROR;
	locked = TRUE;

	if (size1 > 0)
		{
		lentoread = (size1 < length) ? size1 : length;
		readlen += fread( data1, 1, lentoread, inst->File );
		}
	if (size2 > 0)
		{
		lentoread = (size2 < length) ? size2 : length;
		readlen += fread( data2, 1, lentoread, inst->File );
		}
	if (readlen != length) // Erreur. Ne pas faire de Play() si rien n'est lu
		{ inst->RestLength = 0; goto END_ERROR; }

	inst->RestLength -= readlen;
	// OOTRACE( "READ BUFFER : start=%d len=%d,  read=%d -> RestLength=%d\n", start, length, readlen, inst->RestLength );

	rc = OOV_OK;

END_ERROR:
    if (locked == TRUE) IDirectSoundBuffer_Unlock( inst->SoundBuf, data1, size1, data2, size2 );
	return rc;
}

/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

/*
static ULONG SetGet_OnOff( OOSetGetParms *sgp ) // OOA_OnOff
{
	ULONG change = OOV_NOCHANGE;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		BOOL data = sgp->ExtAttr->ti_Bool;
		if (sgp->Attr->ti_Bool != data) 
			{
			//------ Start sound
			if (data == TRUE)
				{
				// If not started (by method OOM_STARTPLAY)	start it
				if (! (((OOInst_Sound*)sgp->Instance)->Flags & OOF_SOUND_STARTED)) // Test avant pour éviter le code erreur
					if (OOSound_StartPlay( sgp->Class, sgp->Obj ) == OOV_ERROR)
						return OOV_NOCHANGE;
				}
			//------ Stop sound
			else{
				OOSound_StopPlay( sgp->Class, sgp->Obj );
				}
			change = OOV_CHANGED;
			sgp->Attr->ti_Bool = data;
			}
		}
	else{
		*(ULONG*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Data;
		}
	return change;
}
*/

static ULONG SetGet_Preload( OOSetGetParms *sgp ) // OOA_OnOff
{
	ULONG change = OOV_NOCHANGE;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		BOOL data = sgp->ExtAttr->ti_Bool;
		if (sgp->Attr->ti_Bool != data) 
			{
			//------ Preload the sound
			if (data == TRUE)
				{
				// Le nom de fichier peut être donné après, sinon on prend celui donné avant
				UBYTE *filename = (UBYTE*) OOTag_GetData( sgp->ExtAttr, OOA_SoundFileName, OOINST_ATTR(sgp->Class,sgp->Obj,INDEX_OOA_SoundFileName) );
				OOInst_Sound *inst = sgp->Instance;
				// OK essayer de charger
				if (filename != NULL && inst->SoundBuf == NULL && OOSound_AllocBuffer( sgp->Class, sgp->Obj ) == OOV_OK)
					{
					if (OOSound_PrepareBuffer( sgp->Class, sgp->Obj ) == OOV_OK)
						{
						OOINST_ATTR(sgp->Class,sgp->Obj,INDEX_OOA_SoundStreamed) = FALSE;
						change = OOV_CHANGED;
						}
					else OOSound_FreeBuffer( inst );
					}
				}
			//------ Free the sound buffer
			else{
				OOSound_FreeBuffer( sgp->Instance );
				change = OOV_CHANGED;
				}

			if (change == OOV_CHANGED)
				sgp->Attr->ti_Bool = data;
			}
		}
	else{
		*(ULONG*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Data;
		}
	return change;
}

/***************************************************************************
 *
 *      Sound filters
 *
 ***************************************************************************/

static ULONG OOSound_Filter_Wave( OOClass *cl, OObject *obj, OOTagList *attrlist, UBYTE *filename )
{
	OOInst_Sound *inst = OOINST_DATA(cl,obj);
	FILE *file;
	ULONG rc = OOV_ERROR;

	if ((file = fopen( filename, "rb" )) != NULL)
		{
		OOSoundFileHeader_WAVE FHW;
		if (fread( &FHW, sizeof(FHW), 1, file ) == 1) 
			{
			if (FHW.Str_RIFF == B2L('R','I','F','F') && FHW.Str_WAVE == B2L('W','A','V','E') && FHW.Str_fmt_ == B2L('f','m','t',' ') && FHW.Str_data == B2L('d','a','t','a'))
				{
				PCMWAVEFORMAT *wfmt = (PCMWAVEFORMAT*)inst->Format;

				memset( wfmt, 0, sizeof(PCMWAVEFORMAT) );
				wfmt->wf.wFormatTag		= WAVE_FORMAT_PCM;      
				wfmt->wf.nChannels		= FHW.ChannelCount;
				wfmt->wf.nSamplesPerSec	= FHW.SampleRate;
				wfmt->wf.nBlockAlign	= FHW.BlockAlign;
				wfmt->wf.nAvgBytesPerSec= FHW.SampleRate * FHW.BlockAlign;
				wfmt->wBitsPerSample	= FHW.BitsPerSample;

				inst->DataOffset = sizeof(OOSoundFileHeader_WAVE);
				inst->DataSize	 = (ULONG)FHW.DataSize;
				rc = OOV_OK;
				}
			}
		fclose( file );
		}
	return rc;
}

//------------------- Load file format

struct _OOSoundFileFilter {
	UBYTE	*FileExt;
	ULONG	(*Func)( OOClass *cl, OObject *obj, OOTagList *attrlist, UBYTE *filename );
	}; 
static struct _OOSoundFileFilter Filters_Table[] = {
	{ ".wav", OOSound_Filter_Wave },
	NULL
	};

static ULONG OOSound_LoadFileFormat( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	UBYTE *found;
	struct _OOSoundFileFilter *tab;
	UBYTE *filename;

	if ((filename = (UBYTE*) OOTag_GetData( attrlist, OOA_SoundFileName, (ULONG)NULL )) != NULL)
		{
		for (tab=Filters_Table; tab->FileExt != NULL; tab++)
			if ((found = strrchr( filename, '.' )) != NULL)
				if (_stricmp( found, tab->FileExt ) == 0)
					return tab->Func( cl, obj, attrlist, filename );
		}
	return OOV_ERROR;
}

/***************************************************************************
 *
 *      Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
    { OOA_OnOff			, TRUE	, "OnOff"		 , NULL				, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_Orphan		, FALSE	, "Orphan"		 , NULL				, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_SoundFileName	, FALSE	, "SoundFileName", NULL				, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
	{ OOA_SoundVolume	, 0		, "SoundVolume"	 , NULL				, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT }, // 0 à -10000 decibels
	{ OOA_SoundLoop		, FALSE	, "SoundLoop"	 , NULL				, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT }, // 0 à -10000 decibels
	{ OOA_SoundStreamed	, FALSE	, "SoundStreamed", NULL				, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT }, // 0 à -10000 decibels
	{ OOA_SoundPreLoad	, FALSE	, "SoundPreLoad" , SetGet_Preload	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT }, // 0 à -10000 decibels
    { TAG_END }					
	};

OOClass Class_Sound = { 
	Dispatcher_Sound, 
	&Class_Root,			// Base class
	"Class_Sound",			// Class Name
	0,						// InstOffset
	sizeof(OOInst_Sound),	// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
