#ifndef _MIXER_H
#define _MIXER_H

#define _MIXER_MAXCHANNELS 8    // TBD: add SDL2 Mixer
#define _MIXER_BUFSIZE  960

typedef signed short sint16;
typedef signed int sint32;

///
/// Section Init
///
void Mixer_SetMode();
void Mixer_Init();
void Mixer_Terminate();

//
/// Section A
///
void PlaySample(sint16 * sampleAddress, int sampleLenght, int vol, int stereo);
int  PlaySampleAtChannel(int selected_channel, sint16 * sampleAddress, int sampleLenght, int vol, int stereo);
int  StopSampleAtChannel(int selected_channel);
int  IsPlayingAtChannel(int chan);

/*
PT:BR se fizer um resume, ele vai testar se o channel.counter > 0 ... assim ele mesmo se distinguirá entre um stop (.counter == 0) e um pause.

EN-US: if he makes an resmume, he will be testing if the channel.counter > 0 ... then he will choose between an stop (.counter > 0) and a pause  
void Mixer(char *file)
{
   channel->counter;
}

*/

///
/// Section B
///
void Mixer_Tick();

//Thread functions
void Mixer_AddVBlankHandler();
void Mixer_RemoveVBlankHandler();
void Mixer_StartThread();
void Mixer_StopThread();
int Mixer_Tick_IntHandler(int cause);
void MixerThread_Play(void *arg);
typedef struct
{
    int active;             // 1 if playing, 0 if not playing
    sint16 * currentSample;
    int counter;
    int sampleLenght;       // redundant
	int volume;			    // 0 to 128
    int stereo;			    // 1 if playing stereo sample, 0 if mono
} channel;

// use : array channels : channel channels[]
#endif
