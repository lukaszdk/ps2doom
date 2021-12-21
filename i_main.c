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

#include <SDL.h>

#include "include/doomdef.h"

#include "include/m_argv.h"
#include <libconfig.h>

#include "include/d_main.h"

#include <sifrpc.h>

//#ifdef PS2HDD
#include <debug.h>
#include <libhdd.h>
#include <libpwroff.h>
#include <loadfile.h>
#include <tamtypes.h> 
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PARTITIONS   100
int FILEXIO_MOUNT;
#define O_RDONLY	     00
//#endif


static char s_pUDNL   [] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "rom0:UDNL rom0:EELOADCNF";

#include <sjpcm.h>

// cosmitoMixer
#include <sifrpc.h>
#include "include/mixer.h"
#include "include/mixer_thread.h"
#include <kernel.h>     //for GetThreadId 
#include <libmc.h>

#include "include/cosmitoFileIO.h"


extern int SAMPLECOUNT =		512;

int getDisplayModeFromELFName(char **argv);

/// ------------------------- por em .h
//typedef enum
//{
//    square,
//    cross,
//    circle,
//    triangle,
//    select,
//    start,
//    l1,
//    r1,
//    l2,
//    r2,
//    l3,
//    r3,
//    analog1left,
//    analog1right,
//    analog2left,
//    analog2right
//
//} config_buttons;

/// these two are pairs
typedef struct
{
    char* name;
    char* defaultaction;
} config_buttons_element;

config_buttons_element config_buttons[] = 
{
    // button name, default action
    // both button names and action names as appearing at the libconfig config file
    {"square", "previous weapon"},
    {"cross", "run"},
    {"circle", "next weapon"},
    {"triangle", "escape"},
    {"select", "map"},
    {"start", "enter"},
    {"l1", "n"},
    {"r1", "fire"},
    {"l2", "y"},
    {"r2", "open"},
    {"l3", ""},
    {"r3", "gamma"},
    {"dpadleft", "leftarrow"},
    {"dpadright", "rightarrow"},
    {"dpadup", "uparrow"},
    {"dpaddown", "downarrow"},
    {"analog1left", ""},                /// TBD : yet not used
    {"analog1right", ""},               /// TBD : yet not used
    {"analog2left", "strafe left"},     // index 18
    {"analog2right", "strafe right"}
};

// Store place to be built at config loading. Have the key codes for each button.

// KEY_RIGHTARROW	0xae
// KEY_LEFTARROW	0xac
// KEY_UPARROW	    0xad
// KEY_DOWNARROW	0xaf

///

typedef struct
{
    char*	name;
    int		value;
} config_actions_element;

config_actions_element	config_actions[] =
{
    // actually the key mapping for the actions :
    // action name, mapped key 
    {"previous weapon", SDLK_o},
    {"run", KEY_RSHIFT},
    {"next weapon", SDLK_p},
    {"escape", KEY_ESCAPE},
    {"map", KEY_TAB},
    {"enter", KEY_ENTER},
    {"n", SDLK_n},
    {"fire", KEY_RCTRL},
    {"y", SDLK_y},
    {"open", SDLK_SPACE},
    {"gamma", KEY_F11},
    {"leftarrow", KEY_LEFTARROW},
    {"rightarrow", KEY_RIGHTARROW},
    {"uparrow", KEY_UPARROW},
    {"downarrow", KEY_DOWNARROW},
    
    {"strafe left", STRAFELEFT},
    {"strafe right", STRAFERIGHT},

    /// to add a new : add entry here with default
};

char config_probestring[200];

/// -------------------------

#define DEBUG_LIBCONFIG

const char *hdd_wads_folder;

int main( int argc, char**	argv ) 
{
    FILE *fp;
    int i, j, nj;
    const char *s;
    char configfile[256];
    
    char elfFilename[100];
    char deviceName[10];
    char fullPath[256];
    int use_hdd;
    int swap_analogsticks;
    int config_buttons_int[] = 
    {
    //-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,     0xac, 0xae, 0xad, 0xaf,    -1, -1, -1, -1
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };
    //Declare usbd module //
unsigned char usbd;
unsigned int size_usbd;

//Declare usbhdfsd module //
unsigned char usbhdfsd;
unsigned int size_usbhdfsd;

unsigned char SJPCM;
unsigned int size_SJPCM;

unsigned char freesd;
unsigned int size_freesd;

//#ifdef PS2HDD
/*Declare iomanX module*/
unsigned char iomanX;
unsigned int size_iomanX;
/*Declare fileXio module*/
unsigned char fileXio;
unsigned int size_fileXio;
/*Declare ps2dev9 module*/
unsigned char ps2dev9;
unsigned int size_ps2dev9;
/*Declare ps2atad module*/
unsigned char ps2atad;
unsigned int size_ps2atad;
/*Declare ps2hdd module*/
unsigned char ps2hdd;
unsigned int size_ps2hdd;
/*Declare ps2fsmodule*/
unsigned char ps2fs;
unsigned int size_ps2fs;
/*Declare poweroff module*/
unsigned char poweroff;
unsigned int size_poweroff;
/*Declare cdvd module*/
unsigned char cdvd;
unsigned int size_cdvd;
//#endif

    config_t cfg;       // libconfig
    const char *hdd_path_to_partition;
    use_hdd = CONFIG_FALSE;

    GetElfFilename(argv[0], deviceName, fullPath, elfFilename);

    int forceDisplayMode = -1;
    myargc = argc; 
    myargv = argv; 

    s32 main_thread_id;
    main_thread_id = GetThreadId ();

	SifInitRpc(0); 

    init_scr();
    scr_printf("--==== PS2DOOM v1.0.5.0 ====--\n\n\n");
    scr_printf("A Doom PS2 port started by Lukasz Bruun, improved by cosmito and modified by wolf3s\n\n\n");
    
    scr_printf ("thanks to Wally modder, Dirsors, fjtrujy and Howling Wolf & Chelsea");
    int ret;
    printf("sample: kicking IRXs\n");
	
    //ret = SifLoadModule("rom0:LIBSD", 0, NULL);
    ret = SifExecModuleBuffer(freesd, size_freesd, 0, NULL, &ret);
	printf("freesd loadmodule %d\n", ret);

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
    
//#ifdef PS2HDD
//	SifExecModuleBuffer(poweroff, size_poweroff, 0, NULL, &ret);
//    SifExecModuleBuffer(iomanX, size_iomanX, 0, NULL, &ret);
//	SifExecModuleBuffer(fileXio, size_fileXio, 0, NULL, &ret);
//	SifExecModuleBuffer(ps2dev9, size_ps2dev9, 0, NULL, &ret);
//	SifExecModuleBuffer(ps2atad, size_ps2atad, 0, NULL, &ret);
//	static char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
//    SifExecModuleBuffer(ps2hdd, size_ps2hdd, sizeof(hddarg), hddarg, &ret);
//	if (ret < 0)
//    {
//        printf("Failed to load module: PS2HDD.IRX");
//        scr_printf("Failed to load module: PS2HDD.IRX");
//    }
//	static char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40";
//	SifExecModuleBuffer(ps2fs, size_ps2fs, sizeof(pfsarg), pfsarg, &ret);
//	if (ret < 0)
//    {
//        scr_printf("Failed to load module: PS2FS.IRX");
//        printf("Failed to load module: PS2FS.IRX");
//    }
//#endif

    // USB mass support
    SifExecModuleBuffer(usbd, size_usbd, 0, NULL, &ret);
    SifExecModuleBuffer(usbhdfsd, size_usbhdfsd, 0, NULL, &ret);

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
    {
        fioMkdir("mc0:PS2DOOM"); // Make sure it exists
        printf(" ... created mc0:PS2DOOM ...\n");
    }
    else
        fioClose(handle);


    /// config
    sprintf(configfile, "%s%s", fullPath, "ps2doom.config");

    // First, try to load from localpath. If fails, try from 'mc0:'
    int configLoadSuccess = 0;
    fp = fopen(configfile, "rb");
    if(!fp)
    {
        printf("file '%s' not found. Going to try 'mc0:PS2DOOM/ps2doom.config'\n", configfile);
        sprintf(configfile, "%s", "mc0:PS2DOOM/ps2doom.config");

        fp = fopen(configfile, "rb");
        if(!fp)
        {
            // Using default actions for buttons
#ifdef DEBUG_LIBCONFIG
            printf("Unable to open '%s'. Using defaults\n", configfile);
#endif
            int nConfig_buttonsEntries = sizeof(config_buttons)/sizeof(config_buttons[0]);
            for (i=0; i<nConfig_buttonsEntries; i++)
            {
                nj = sizeof(config_actions)/sizeof(config_actions[0]);
                for(j=0; j<nj; j++)
                {
                    if(strcmp(config_actions[j].name, config_buttons[i].defaultaction) == 0)
                    {
                        int value = config_actions[j].value;
                        config_buttons_int[i] = value;
                    }
                }      
            }
        }
        else
            configLoadSuccess = 1;
    }
    else
        configLoadSuccess = 1;

    if(configLoadSuccess == 1)
    {
        // Get the actions for buttons from config
        int x;
        config_init(&cfg);
        x = config_read(&cfg, fp);
        fclose(fp);
        if(x)
        {
            // Process each ps2doom.controls config entries
            int nConfigEntries = sizeof(config_buttons)/sizeof(config_buttons[0]);
            for (i=0; i<nConfigEntries; i++)
            {
                sprintf(config_probestring, "%s%s", "ps2doom.controls.", config_buttons[i].name);
                if(! config_lookup_string(&cfg, config_probestring, &s))
                {
#ifdef DEBUG_LIBCONFIG
                    printf("NOT FOUND %s\n", config_probestring);
#endif
                }
                else
                {
#ifdef DEBUG_LIBCONFIG
                    printf("found: %s = %s\n", config_probestring, s);
#endif
                    nj = sizeof(config_actions)/sizeof(config_actions[0]);
                    for(j=0; j<nj; j++)
                    {
                        if(strcmp(config_actions[j].name, s) == 0)
                        {
                            int value = config_actions[j].value;
                            config_buttons_int[i] = value;
                        }
                    }      
                }
            }
        }
        else
            printf("error on line %d: %s\n", cfg.error_line, cfg.error_text);

        use_hdd = CONFIG_FALSE;
        sprintf(config_probestring, "%s", "ps2doom.hdd.use_hdd");
        if(!config_lookup_bool(&cfg, config_probestring, &use_hdd))
        {
            use_hdd = CONFIG_FALSE;
            printf("NOT FOUND %s\n", config_probestring);
        }
        else
        {
            printf("found: %s = %d\n", config_probestring, use_hdd);
        }


        if(use_hdd == CONFIG_TRUE)
        {
            sprintf(config_probestring, "%s", "ps2doom.hdd.path_to_partition");
            if(!config_lookup_string(&cfg, config_probestring, &hdd_path_to_partition))
            {
                printf("NOT FOUND %s\n", config_probestring);
                scr_printf("Error: Value '%s' at ps2doom.config not found\n", config_probestring);
                SleepThread();
            }
            else
            {
                printf("found: %s = %s\n", config_probestring, hdd_path_to_partition);
            }

            sprintf(config_probestring, "%s", "ps2doom.hdd.wads_folder");
            if(!config_lookup_string(&cfg, config_probestring, &hdd_wads_folder))
            {
                printf("NOT FOUND %s\n", config_probestring);
                scr_printf("Error: Value '%s' at ps2doom.config not found\n", config_probestring);
                SleepThread();
            }
            else
            {
                printf("found: %s = %s\n", config_probestring, hdd_wads_folder);
            }
        }

        //
        swap_analogsticks = CONFIG_FALSE;
        sprintf(config_probestring, "%s", "ps2doom.controls.switches.swap_analogsticks");
        if(!config_lookup_bool(&cfg, config_probestring, &swap_analogsticks))
        {
            swap_analogsticks = CONFIG_FALSE;
#ifdef DEBUG_LIBCONFIG
            printf("NOT FOUND %s\n", config_probestring);
#endif
        }
        else
        {
#ifdef DEBUG_LIBCONFIG
            printf("found: %s = %d\n", config_probestring, swap_analogsticks);
#endif
        }

        
    }

#ifdef DEBUG_LIBCONFIG
    for(i=0; i<sizeof(config_buttons)/sizeof(config_buttons[0]); i++)
    {
        printf("%d (%s)\n", config_buttons_int[i], config_buttons[i].name);
    }
#endif


    if(use_hdd == CONFIG_TRUE)
    {
        SifExecModuleBuffer(poweroff, size_poweroff, 0, NULL, &ret);
        SifExecModuleBuffer(iomanX, size_iomanX, 0, NULL, &ret);
        SifExecModuleBuffer(fileXio, size_fileXio, 0, NULL, &ret);
        SifExecModuleBuffer(ps2dev9, size_ps2dev9, 0, NULL, &ret);
        SifExecModuleBuffer(ps2atad, size_ps2atad, 0, NULL, &ret);
        static char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
        SifExecModuleBuffer(ps2hdd, size_ps2hdd, sizeof(hddarg), hddarg, &ret);
        if (ret < 0)
        {
            printf("Failed to load module: PS2HDD.IRX");
            scr_printf("Failed to load module: PS2HDD.IRX");
        }
        static char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40";
        SifExecModuleBuffer(ps2fs, size_ps2fs, sizeof(pfsarg), pfsarg, &ret);
        if (ret < 0)
        {
            scr_printf("Failed to load module: PS2FS.IRX");
            printf("Failed to load module: PS2FS.IRX");
        }
        //#endif


        //#ifdef PS2HDD
        //#ifdef PS2HDD
        //char ps2HDDPartition[] = "hdd0:+Test";                    // TBD : name of partition only
        //char folderpath[] = "abc";                                // The folder path without any '/'
        
        //sprintf(fullPath, "%s/%s/", hdd_path_to_partition, hdd_wads_folder);
        sprintf(fullPath, "pfs0:%s/", hdd_wads_folder);
        printf(">>>>>>>>%s\n", fullPath);
        //#endif
        if( hddCheckPresent() < 0 )
        {
            printf( "NO HDD FOUND!\n" );
            scr_printf( "NO HDD FOUND!\n" );
            return -1;
        }
        else
        {
            printf( "Found HDD!\n" );
        }

        if( hddCheckFormatted() < 0 )
        {
            printf( "HDD Not Formatted!\n" );
            scr_printf( "HDD Not Formatted!\n" );
            return -1;
        }
        else
        {
            printf( "HDD Is Formatted!\n" );
        }

        int mountErr = 0;
        mountErr = fileXioMount( "pfs0:", hdd_path_to_partition, FILEXIO_MOUNT );
        
        if( mountErr < 0 )
        {
            printf( "Mount Error: %d while trying to mount partition '%s'. Check if path is correct.\n", mountErr, hdd_path_to_partition);
            scr_printf( "Mount Error: %d while trying to mount partition '%s'. Check if path is correct.\n", mountErr, hdd_path_to_partition);
            SleepThread();
        }
        //#endif
    }


    SjPCM_Init(1);		// sync mode

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