#ifndef _MIXER_H
#define _MIXER_H

#define _MIXER_MAXCHANNELS 8    // TBD
#define _MIXER_BUFSIZE  960

typedef signed short sint16;
typedef signed int sint32;

/// Section Init
///
void Mixer_SetMode();
void Mixer_Init();
void Mixer_Terminate();

/// Section A
///
void PlaySample(sint16 * sampleAddress, int sampleLenght, int vol, int stereo);
int  PlaySampleAtChannel(int selected_channel, sint16 * sampleAddress, int sampleLenght, int vol, int stereo);
int  StopSampleAtChannel(int selected_channel);
int  IsPlayingAtChannel(int chan);

// se fizer um resume, testar se o channel.counter > 0 ... assim distingue-se entre um stop (.counter == 0) e um pause.

/// Section B
///
void Mixer_Tick();

typedef struct
{
    int active;             // 1 if playing, 0 if not playing
    sint16 * currentSample;
    int counter;
    int sampleLenght;       // redundant
	int volume;			    // 0 to 128
    int stereo;			    // 1 if playing stereo sample, 0 if mono

} channel;

// uso : array channels : channel channels[]
#endif
