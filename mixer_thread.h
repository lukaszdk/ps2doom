#ifndef _MIXER_THREAD_H
#define _MIXER_THREAD_H

void Mixer_AddVBlankHandler();
void Mixer_RemoveVBlankHandler();
void Mixer_StartThread();
void Mixer_StopThread();
int Mixer_Tick_IntHandler(int cause);
void MixerThread_Play(void *arg);

#endif
