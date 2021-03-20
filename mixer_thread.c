/// Based on modplayer_thread.c

#include <tamtypes.h>
#include <ps2lib_err.h>
#include <kernel.h>
#include <sifrpc.h>

#include "mixer_thread.h"
#include "mixer.h"

#define THREAD_STACK_SIZE	(8 * 1024)

static int         thread_waitsema = -1;
static u8          thread_stack[THREAD_STACK_SIZE] ALIGNED(16);
static ee_thread_t thread_thread;
static ee_sema_t   thread_playsema;
static int         thread_threadid;

extern s32 main_thread_id;

///
void Mixer_StartThread()
{
	extern void *_gp;

	// Create a temporary semaphore that we'll use to wait for the RPC
	// thread to finish initializing.  */
	thread_playsema.init_count = 0;
	thread_playsema.max_count = 1;
	if ((thread_waitsema = CreateSema(&thread_playsema)) < 0)
	{
		printf("CREATE SEMA ERROR!!!\n");
		return;
	}

	thread_thread.func = MixerThread_Play;
	thread_thread.stack = thread_stack;
	thread_thread.stack_size = THREAD_STACK_SIZE;
	thread_thread.gp_reg = _gp;
	thread_thread.initial_priority = 42;

	if ((thread_threadid = CreateThread(&thread_thread)) < 0) 
	{
		printf("CREATE THREAD ERROR!!!\n");
		return;
	}

	StartThread(thread_threadid, NULL);
}

s32 mixer_VRstartID = 0;

//  DO NOT CALL THIS - this is the vblank handler function
int Mixer_Tick_IntHandler(int cause)
{
	//iChangeThreadPriority(main_thread_id, 42);
	iSignalSema(thread_waitsema);
	//iRotateThreadReadyQueue(42);

	// Must do at the end
	asm ("sync.l ; ei");
	return 0;
}


//  This adds the vblank handler and inits it
void Mixer_AddVBlankHandler()
{
	DI();
	mixer_VRstartID = AddIntcHandler(2, Mixer_Tick_IntHandler, 0);
	EnableIntc(2);
	EI();
}

//  DO NOT CALL THIS, this is for the thread for playing
//  this is triggered by : Mixer_Tick_IntHandler
void MixerThread_Play(void *arg)
{
	// DO THE MEAT IN HERE
	while (1)
	{
		//  Wait to be triggered from the Vblank
		WaitSema(thread_waitsema);
		/* Do Modplay tick */
		Mixer_Tick();
	}
}

//  This stops and removes the player thread
void Mixer_StopThread()
{
	TerminateThread(thread_threadid);
	DeleteThread(thread_threadid);
}

//  This disables and removes the vblank handler
void Mixer_RemoveVBlankHandler()
{
	DisableIntc(2);
	RemoveIntcHandler(2, mixer_VRstartID);
}
