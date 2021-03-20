// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	System interface for sound.
//
//-----------------------------------------------------------------------------

//typedef signed short sint16;
#include "mixer.h"
#include "mixer_thread.h"

static const char
rcsid[] = "$Id: i_unix.c,v 1.5 1997/02/03 22:45:10 b1 Exp $";

#include <math.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_mutex.h>
#include <SDL_byteorder.h>
#include <SDL_version.h>

#include "z_zone.h"

#include "m_swap.h"
#include "i_system.h"

//#include "i_sound.h"		// cosmito
#include "l_sound_sdl.h"

#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
//#include "lprintf.h"

#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"

#include "d_main.h"

#include <SDL_mixer.h>

// The number of internal mixing channels,
//  the samples calculated for each mixing step,
//  the size of the 16bit, 2 hardware channel (stereo)
//  mixing buffer, and the samplerate of the raw data.

#define PIPE_CHECK(fh) if (broken_pipe) { fclose(fh); fh = NULL; broken_pipe = 0; }

// Variables used by Boom from Allegro
// created here to avoid changes to core Boom files
int snd_card = 1;
int mus_card = 1;
int detect_voices = 0; // God knows

// Needed for calling the actual sound output.
//static int SAMPLECOUNT =		512;
int SAMPLECOUNT =		512;
#define NUM_CHANNELS		8

//#define SAMPLERATE		48000/4 //11025	// Hz
int SAMPLERATE	=	48000/2; //11025	// Hz               // TBD

// The actual lengths of all sound effects.
int 		lengths[NUMSFX];

// The actual output device.
int	audio_fd;

// Time/gametic that the channel started playing,
//  used to determine oldest, which automatically
//  has lowest priority.
// In case number of active sounds exceeds
//  available channels.
int		channelstart[NUM_CHANNELS];

// SFX id of the playing sound effect.
// Used to catch duplicates (like chainsaw).
int		channelids[NUM_CHANNELS];


// cosmito : 
///
Mix_Chunk *Mix_QuickLoad_RAW_custom(Uint8 *mem, Uint32 len)
{
	Mix_Chunk *chunk;

	/* Make sure audio has been opened */
	//if ( ! audio_opened ) {
	//	SDL_SetError("Audio device hasn't been opened");
	//	return(NULL);
	//}

	/* Allocate the chunk memory */
	chunk = (Mix_Chunk *)malloc(sizeof(Mix_Chunk));
	if ( chunk == NULL ) {
		SDL_SetError("Out of memory");
		return(NULL);
	}

	/* Essentially just point at the audio data (no error checking - fast) */
	chunk->allocated = 0;
	chunk->alen = len;
	chunk->abuf = mem;
	chunk->volume = MIX_MAX_VOLUME;

	return(chunk);
}

int *__errno()
{
	extern int errno;
	return &errno;
}

//
// This function loads the sound data from the WAD lump,
//  for single sound.
//
void* getsfx( const char* sfxname, int* len )
{
	unsigned char*      sfx;
	unsigned char*      paddedsfx;
	int                 i;
	int                 size;
	int                 paddedsize;
	char                name[20];
	int                 sfxlump;

	SDL_AudioCVT cvt;
	int outlen;

	// Get the sound data from the WAD, allocate lump
	//  in zone memory.
	sprintf(name, "ds%s", sfxname);

	// Now, there is a severe problem with the
	//  sound handling, in it is not (yet/anymore)
	//  gamemode aware. That means, sounds from
	//  DOOM II will be requested even with DOOM
	//  shareware.
	// The sound list is wired into sounds.c,
	//  which sets the external variable.
	// I do not do runtime patches to that
	//  variable. Instead, we will use a
	//  default sound for replacement.
	if ( W_CheckNumForName(name) == -1 )
	{	sfxlump = W_GetNumForName("dspistol");
//printf("*1*\n"); 
	}
	else
	{	sfxlump = W_GetNumForName(name);
//printf("*2*\n"); 
	}
	size = W_LumpLength( sfxlump );

//// debug
//printf("sfxname = %s\n", sfxname);
//printf("sfxlump = %d\n", sfxlump);
//printf("len = %d\n", *len);
//printf("size = %d\n", size);
//while(1)
//{};


	// Debug.
	// fprintf( stderr, "." );
	//fprintf( stderr, " -loading  %s (lump %d, %d bytes)\n",
	//	     sfxname, sfxlump, size );
	//fflush( stderr );

	//sfx = (unsigned char*)W_CacheLumpNum(sfxlump);
	sfx = (unsigned char*)W_CacheLumpNum(sfxlump, PU_STATIC);		// cosmito

			//		//debug
			//for(qq = 0; qq < 50; qq++)
			//	printf("sfx[qq] = %d\n", (unsigned char)sfx[qq]);
			//while(1)
			//{};

	// Pads the sound effect out to the mixing buffer size.
	// The original realloc would interfere with zone memory.
	paddedsize = ((size-8 + (SAMPLECOUNT-1)) / SAMPLECOUNT) * SAMPLECOUNT;

	// Allocate from zone memory.
	paddedsfx = (unsigned char*)Z_Malloc( paddedsize+8, PU_STATIC, 0 );
	// ddt: (unsigned char *) realloc(sfx, paddedsize+8);
	// This should interfere with zone memory handling,
	//  which does not kick in in the soundserver.

	// Now copy and pad.
	memcpy(  paddedsfx, sfx, size );
	for (i=size ; i<paddedsize+8 ; i++)
		paddedsfx[i] = 128;

	// upsample to fit ps2 specs
	SDL_BuildAudioCVT(&cvt, AUDIO_U8, 1, 5513, AUDIO_S16, 1, SAMPLERATE);
    
    //printf("%d\n", SAMPLERATE);

////debug
//printf("paddedsize = %d ,  cvt.len_mult = %d\n", paddedsize, cvt.len_mult);
//while(1) {};
	outlen = paddedsize * cvt.len_mult;
	cvt.len = paddedsize;
	cvt.buf = (Uint8 *)malloc(outlen);
	memcpy(cvt.buf, paddedsfx + 8, paddedsize);
//	//debug
//for(qq = 0; qq < 50; qq++)
//printf("cvt.buf[qq] = %d\n", cvt.buf[qq]);
//while(1)
//{};
	SDL_ConvertAudio(&cvt);

    /*
    /// cosmito : dump cvt.buf to file
    sprintf(strbuffer, "host:%s.raw", sfxname);
    printf(" - Dumping %s ...\n", strbuffer);
    dump = fopen(strbuffer, "wb+");
	fwrite(cvt.buf, 1, outlen, dump);
    fclose(dump);
    ///
    */

	*len = outlen;

    free(sfx);      // cosmito : needes testing

	return (void *) cvt.buf;
}




//
// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//
void I_SetChannels()
{
	//Mix_AllocateChannels(NUM_CHANNELS);
}	


void I_SetSfxVolume(int volume)
{
	// Identical to DOS.
	// Basically, this should propagate
	//  the menu/config file setting
	//  to the state variable used in
	//  the mixing.
	snd_SfxVolume = volume;
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
	char namebuf[9];
	sprintf(namebuf, "ds%s", sfx->name);
	return W_GetNumForName(namebuf);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
int I_StartSound( int		id,
 int		vol,
 int		sep,
 int		pitch,
 int		priority )
{
	int channel;

	int		oldest = gametic;
	int		slot = -1;
	int		i;

	// Chainsaw troubles.
	// Play these sound effects only one at a time.
	if ( id == sfx_sawup
		|| id == sfx_sawidl
		|| id == sfx_sawful
		|| id == sfx_sawhit
		|| id == sfx_stnmov
		|| id == sfx_pistol	 )
	{
		// Loop all channels, check.
		for (i=0 ; i<NUM_CHANNELS ; i++)
		{
			// Active, and using the same SFX?
			//if ( Mix_Playing(i) && (channelids[i] == id) )
            if ( IsPlayingAtChannel(i) && (channelids[i] == id) )
			{
				// Reset.
				//Mix_HaltChannel(i);
                StopSampleAtChannel(i);

				// We are sure that iff,
				//  there will only be one.
				break;
			}
		}
	}

	// Loop all channels to find oldest SFX.
	for (i=0; i<NUM_CHANNELS; i++)
	{
		if (channelstart[i] < oldest)
		{
			slot = i;
			oldest = channelstart[i];
		}
	}

	//Mix_VolumeChunk(S_sfx[id].data, vol * snd_SfxVolume);     // TBD later

    //channel = Mix_PlayChannel(slot, S_sfx[id].data, 0);     // S_sfx[id].data é um chunk (ver linha 446 : S_sfx[i].data = Mix_QuickLoad_RAW(data, lengths[i]);)

    Mix_Chunk *chunk;
    chunk = (Mix_Chunk *)S_sfx[id].data;
    channel = PlaySampleAtChannel(slot, (sint16*)chunk->abuf, chunk->alen, vol*snd_SfxVolume, 0);

//printf("> sample vol = %d\n", vol*snd_SfxVolume);

	//// debug
	//unsigned char * aux;
	//printf("channel = %d\n ", channel);
	////	aux = (void *)(S_sfx[id].data + 2);
	////printf("data[2] = %d\n ", (unsigned char*) S_sfx[id].data[2]);
	////printf("data[3] = %d\n ", S_sfx[id].data[3]);
	////printf("data[3] = %d\n ", (S_sfx[id].data[3]<<8)+S_sfx[id].data[2]);

	//printf("l_sound_sdl.c : I_StartSound()\n\n");
	//printf("id = %d\n\n", id);
	//for(i=0; i<100; i++)
	//{  
	//	aux = (void *)(S_sfx[id].data + i);
	//	printf("S_sfx[id].data[%d] = %d\n ", i, *aux);
	//}  
	////printf("(---)\n");
	////for(i=len-100; i<len; i++)
	//// printf("data[%d] = %d\n ", i, data[i]);
	//while(1)
	//{
	//};

	if ( channel > -1 )
	{
		//Mix_Pause(channel);
		//Mix_SetPanning(channel, sep, 254 - sep);      // TBD later
		//Mix_Resume(channel);
		channelstart[channel] = gametic;
		channelids[channel] = id;
	}

	return channel;
}



void I_StopSound (int handle)
{
	// You need the handle returned by StartSound.
	// Would be looping all channels,
	//  tracking down the handle,
	//  an setting the channel to zero.

	// UNUSED.
	//handle = 0;
	//Mix_HaltChannel(handle);
    StopSampleAtChannel(handle);
}


//boolean I_SoundIsPlaying(int handle)
int I_SoundIsPlaying(int handle)		// cosmito
{
	// Ouch.
	//return gametic < handle;
	//return Mix_Playing(handle);
    return IsPlayingAtChannel(handle);
}


//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the given
//  mixing buffer, and clamping it to the allowed
//  range.
//
// This function currently supports only 16bit.
//
void I_UpdateSound(void *unused, Uint8 *stream, int len)
{
	return;
}

void
I_UpdateSoundParams
( int	handle,
 int	vol,
 int	sep,
 int	pitch)
{
	// I fail too see that this is used.
	// Would be using the handle to identify
	//  on which channel the sound might be active,
	//  and resetting the channel parameters.

	// UNUSED.
	handle = vol = sep = pitch = 0;
}


void I_ShutdownSound(void)
{
	fprintf(stderr, "I_ShutdownSound: ");
	//Mix_CloseAudio();     // ptek : we don't want to do this, since it will result in a crash
	fprintf(stderr, "\n");
}

//static SDL_AudioSpec audio;

void I_InitSound()
{ 
	int i;	
	boolean nomusicparm = 1;		/// TBD : cosmito : ugly hardwired

	//// Secure and configure sound device first.
	//fprintf( stderr, "I_InitSound: ");

	//if ( SDL_InitSubSystem(SDL_INIT_AUDIO) < 0 ) {
	//	fprintf(stderr, "Couldn't initialize SDL Audio: %s\n",SDL_GetError());
	//	return;        
	//}
	//// Open the audio device
	//if (Mix_OpenAudio(SAMPLERATE, AUDIO_S16, 2, SAMPLECOUNT) < 0) {
	//	fprintf(stderr, "couldn't open audio with desired format\n");
	//	return;
	//}
	//fprintf(stderr, " configured audio device with %d samples/slice\n", SAMPLECOUNT);

	// Initialize external data (all sounds) at start, keep static.
	fprintf( stderr, "I_InitSound: ");

//printf("\n**** using SAMPLECOUNT = %d\n\n", SAMPLECOUNT);

int debug_memusage = 0;
	for (i=1 ; i<NUMSFX ; i++)
	{ 
		// Alias? Example is the chaingun sound linked to pistol.
		if (!S_sfx[i].link)
		{
			unsigned char *data;

			// Load data from WAD file.
			data = getsfx( S_sfx[i].name, &lengths[i] );

//			//debug
//for(qq = 0; qq < 100; qq++)
//	printf("data[%d] = %d\n", qq, data[qq]);
//while(1)
//{};
			//S_sfx[i].data = Mix_QuickLoad_RAW(data, lengths[i]);
            S_sfx[i].data = Mix_QuickLoad_RAW_custom(data, lengths[i]);

debug_memusage = debug_memusage + lengths[i];

		}
		else
		{
			// Previously loaded already?
			S_sfx[i].data = S_sfx[i].link->data;
			lengths[i] = lengths[(S_sfx[i].link - S_sfx)/sizeof(sfxinfo_t)];
		}
	}

			//	//debug
			//for(qq = 0; qq < 109; qq++)
			//	printf("lengths[i] = %d\n", lengths[qq]);
			//while(1)
			//{};

	fprintf( stderr, " pre-cached all sound data\n");

printf(" .... Used %d bytes for sounds\n\n", debug_memusage);

	atexit(I_ShutdownSound);

	if (!nomusicparm)
		I_InitMusic();

	// Finished initialization.
	fprintf(stderr, "I_InitSound: sound module ready\n");
	//Mix_Pause(-1);
}




//
// MUSIC API.
//

#ifdef HAVE_MIXER
//#include "SDL_mixer.h"
#include "mmus2mid.h"

static Mix_Music *music[2] = { NULL, NULL };
#endif

void I_ShutdownMusic(void) 
{
#ifdef HAVE_MIXER
	Mix_FreeMusic(music[0]);
#endif
	fprintf(stderr, "I_ShutdownMusic: shut down\n");
}

void I_InitMusic(void)
{
#ifdef HAVE_MIXER
	//fprintf(stderr, "I_InitMusic: music initialized\n");
#endif
	atexit(I_ShutdownMusic);
}

void I_PlaySong(int handle, int looping)
{
#ifdef HAVE_MIXER
	if ( music[handle] ) {
		Mix_FadeInMusic(music[handle], looping ? -1 : 0, 500);
	}
#endif
}

extern int mus_pause_opt; // From m_misc.c

void I_PauseSong (int handle)
{
	int mus_pause_opt = 0;	// 0 = kill music, 1 = pause, 2 = continue   // cosmito from lsdldoom
#ifdef HAVE_MIXER
	switch(mus_pause_opt) {
	case 0:
		I_StopSong(handle);
		break;
	case 1:
		Mix_PauseMusic();
		break;
	}
#endif
	// Default - let music continue
}

void I_ResumeSong (int handle)
{
#ifdef HAVE_MIXER
	//Mix_ResumeMusic();
#endif
}

void I_StopSong(int handle)
{
#ifdef HAVE_MIXER
	Mix_FadeOutMusic(500);
#endif
}

void I_UnRegisterSong(int handle)
{
#ifdef HAVE_MIXER
	if ( music[handle] ) {
		Mix_FreeMusic(music[handle]);
		music[handle] = NULL;
	}
#endif
}

int I_RegisterSong(void *data, size_t len)
{
#ifdef HAVE_MIXER
	SDL_RWops *rw;
	MIDI *mididata;
	UBYTE *mid;
	int midlen;

	//printf("Registering song {%c%c%c}\n", ((unsigned char *)data)[0],
	//                                      ((unsigned char *)data)[1],
	//                                      ((unsigned char *)data)[2]);
	if ( memcmp(data, "MUS", 3) == 0 )
	{
		mididata = Z_Malloc(sizeof(MIDI), PU_STATIC, 0);
		mmus2mid(data, mididata, 89, 0);
		if (MIDIToMidi(mididata, &mid, &midlen))
		{
			// Error occurred
			printf("Error converting MUS lump.\n");
		}
		rw = SDL_RWFromMem(mid, midlen);
		Z_Free(mididata);
	}
	else {
		rw = SDL_RWFromMem(data, len);
	}

	/* load and play music */
	music[0] = Mix_LoadMUS_RW(rw);

	if ( music[0] == NULL ) {
		printf("Couldn't load MIDI: %s\n", Mix_GetError());
	}

#endif
	return (0);
}

void I_SetMusicVolume(int volume)
{
#ifdef HAVE_MIXER
	Mix_VolumeMusic(volume*8);
#endif
}
