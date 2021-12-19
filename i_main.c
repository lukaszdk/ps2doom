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


#include <SDL/SDL.h>

#include "include/doomdef.h"

#include "include/m_argv.h"
#include "include/d_main.h"
#include "include/doomdef.h"
#include "include/doomstat.h"
#include "include/dstrings.h"
#include "include/sounds.h"
#include "include/z_zone.h"
#include "include/w_wad.h"
#include "include/s_sound.h"
#include "include/v_video.h"
#include "include/f_finale.h"
#include "include/f_wipe.h"
#include "include/m_argv.h"
#include "include/m_misc.h"
#include "include/m_menu.h"
#include "include/i_system.h"
//#include "include/i_sound.h"
#include "include/l_sound_sdl.h"
#include "include/g_game.h"
#include "include/hu_stuff.h"
#include "include/wi_stuff.h"
#include "include/st_stuff.h"
#include "include/am_map.h"
#include "include/p_setup.h"
#include "include/r_local.h"
#include "include/d_main.h"

#include <tamtypes.h>
#include <sifrpc.h>
#include <kernel.h>     //for GetThreadId 
#include <libmc.h>
#include <fcntl.h>
#include <usbhdfsd-common.h>
int main(int argc, char** argv) 
{ 
    myargc = argc; 
    myargv = argv; 
    s32 main_thread_id;
    main_thread_id = GetThreadId ();

	SifInitRpc(0); 

    init_scr();
    
    scr_printf("--==== PS2DOOM v1.0.6.0 ====--\n\n\n");
    scr_printf("A Doom PS2 port started by Lukasz Bruun, improved by cosmito and modified by wolf3s\n\n\n");
    scr_printf ("thanks to Wally modder, Dirsors, fjtrujy and Howling Wolf & Chelsea");
    
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
    SifExecModuleBuffer(0, NULL, &ret);

    // USB mass support
    SifExecModuleBuffer(0, NULL, &ret);
    SifExecModuleBuffer(0, NULL, &ret);

    // MC support   (from ps2sdk mc_example.c)
    int mc_Type, mc_Free, mc_Format;
	ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: SIO2MAN");
        scr_printf("Failed to load module: SIO2MAN");
		SleepThread();
	}
	ret = SifLoadModule("rom0:XMCMAN", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: MCMAN");
		scr_printf("Failed to load module: MCMAN");
		SleepThread();
	}
	ret = SifLoadModule("rom0:XMCSERV", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: MCSERV");
		SleepThread();
	}
	if(mcInit(MC_TYPE_XMC) < 0) {
		printf("Failed to initialise memcard server!\n");
		SleepThread();
	}
	// Since this is the first call, -1 should be returned.
	mcGetInfo(0, 0, &mc_Type, &mc_Free, &mc_Format); 
	mcSync(0, NULL, &ret);
	printf("mcGetInfo returned %d\n",ret);
	printf("Type: %d Free: %d Format: %d\n\n", mc_Type, mc_Free, mc_Format);

	// Assuming that the same memory card is connected, this should return 0
	mcGetInfo(0,0,&mc_Type,&mc_Free,&mc_Format);
	mcSync(0, NULL, &ret);
	printf("mcGetInfo returned %d\n",ret);
	printf("Type: %d Free: %d Format: %d\n\n", mc_Type, mc_Free, mc_Format);
    if (ret != 0)
    printf("mc0 trouble... should save to other device... To implement\n");  /// TBD

       // create save/load dir (mc0:PS2DOOM)
    int handle = fioOpen ("mc0:PS2DOOM/doomsav0.dsg", O_RDONLY);
    if (handle < 0)
        fioMkdir("mc0:PS2DOOM"); // Make sure it exists
    else
        fioClose(handle);


    SjPCM_Init(1);		// sync mode
	//SjPCM_Init(0);

	printf("SjPCM_Setvol ...\n");
	SjPCM_Setvol(0x3fff);

	ChangeThreadPriority ( GetThreadId (), 42 );
    Mixer_Init();       // TBD : arg number channels

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