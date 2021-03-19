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
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";


#include "SDL.h"

#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"

#include <sifrpc.h>
//#include <kernel.h>

//Declare usbd module //
extern unsigned char usbd[];
extern unsigned int size_usbd;
//Declare usbhdfsd module //
extern unsigned char usbhdfsd[];
extern unsigned int size_usbhdfsd;

extern unsigned char SJPCM[];
extern unsigned int size_SJPCM;

static char s_pUDNL   [] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "rom0:UDNL rom0:EELOADCNF";


#include "sjpcm.h"

// cosmitoMixer
#include <sifrpc.h>
#include "mixer.h"
#include "mixer_thread.h"

#include <kernel.h>     //for GetThreadId 

extern int SAMPLECOUNT;

int getDisplayModeFromELFName(char **argv);


int main( int argc, char**	argv ) 
{ 
    int forceDisplayMode = -1;
    myargc = argc; 
    myargv = argv; 

    s32 main_thread_id;
    main_thread_id = GetThreadId ();

	SifInitRpc(0); 

    int ret;
    printf("sample: kicking IRXs\n");
	ret = SifLoadModule("rom0:LIBSD", 0, NULL);
	printf("libsd loadmodule %d\n", ret);

	//ret = SifLoadModule("host:SJPCM.IRX", 0, NULL);
	//if (ret < 0) 
	//{
	//	printf("Failed to load module: SJPCM.IRX\n");
	//	SleepThread();
	//}

    /*
    SifInitRpc(0);
    SifExitIopHeap();
    SifLoadFileExit();
    SifExitRpc();
    SifResetIop();
    SifIopReset(s_pUDNL, 0);
    SifInitRpc(0);      
    */
    
    //Load embedded modules

    // SJPCM
    SifExecModuleBuffer(SJPCM, size_SJPCM, 0, NULL, &ret);
    
    // USB mass support
    SifExecModuleBuffer(usbd, size_usbd, 0, NULL, &ret);
    SifExecModuleBuffer(usbhdfsd, size_usbhdfsd, 0, NULL, &ret);

    SjPCM_Init(1);		// sync mode
	//SjPCM_Init(0);

	printf("SjPCM_Setvol ...\n");
	SjPCM_Setvol(0x3fff);

	ChangeThreadPriority ( GetThreadId (), 42 );
    Mixer_Init();       // TBD : arg number channels

    // Until sdl isn't fixed
    int PAL = detect_signal();
    if (PAL == 1)
        PS2SDL_ForceSignal(0);
    else
        PS2SDL_ForceSignal(1);

    // Changes accordingly to filename
    forceDisplayMode = getDisplayModeFromELFName(argv);
    if (forceDisplayMode != -1)
        PS2SDL_ForceSignal(forceDisplayMode);

    // Sets SAMPLECOUNT accordingly to system
    if (PAL == 1)
        SAMPLECOUNT = 960;
    else
        SAMPLECOUNT = 800;


    D_DoomMain (); 

    return 0;
} 


// Check if ELF name ends with PAL, pal, NTSC, ntsc.
int getDisplayModeFromELFName(char **argv)
{
    char tmp[10];
    int ln = 0,i,j;

    ln = strlen(argv[0]);
    if (ln>=8)
    {
        // check for PAL
        j=0;
        for(i=ln-7; i<=ln-5; i++)
            tmp[j++] = argv[0][i];
        tmp[j] = 0;

        if (strcmp(tmp, "PAL") == 0 || strcmp(tmp, "pal") == 0)
            return 0;
        
        // check for NTSC
        j=0;
        for(i=ln-8; i<=ln-5; i++)
            tmp[j++] = argv[0][i];
        tmp[j] = 0;

        if (strcmp(tmp, "NTSC") == 0 || strcmp(tmp, "ntsc") == 0)
            return 1;
    }

    return -1;
}
