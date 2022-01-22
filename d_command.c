
#include "include/s_sound.h"
#include "include/d_main.h"


/****************************************************************************
** ps2doom cheat support - code typing and command line
*****************************************************************************/
void ps2_do_cheat(int cheat)
{
    event_t event;
    char *str;
    int i;

    switch (cheat)
    {
           case 1: // God Mode
	   str = "iddqd";		
	   break;		
           case 2: // Good fucking arsenal
	   str = "idfa";		
	   break;
	   case 3: // Key Full Ammo	
	   str = "idkfa";		
	   break;
	   case 4: // No Clipping		str = "idclip";		
	   break;
	   case 5: // Toggle Map         
	   str = "iddt";		
	   break;
	   case 6: // Invincible with Chainsaw 	         
	   str = "idchoppers";
	   break;
	   case 7:  // Berserker Strength Power-up    	 
	   str = "idbeholds";		
	   break;
	   case 8:  //Invincibility Power-up
	   str = "idbeholdv";		
	   break;		
	   case 9:  // Invisibility Power-Up
	   str = "idbeholdi";		
	   break;
           case 10: //Automap Power-up      
	   str = "idbeholda";		
	   break;
	   case 11: //Anti-Radiation Suit Power-up     
	   str = "idbeholdr";
	   break;
	   case 12: // Light-Amplification Visor Power-up
	   str = "idbeholdl";
	   break;
	   case 13: 
	   str = "no_sound"; //Deactivates thesound
          // wip SjPCM_Pause()
	   return;
           break;
	   
	   case 14: 
	   str = "no_music";
           S_StopMusic();
	   return;
	   break;
	   

       }

    for (i=0; i<strlen(str); i++) 
    { 
      event.type = ev_keydown; 
      event.data1 = str[i]; 
      D_PostEvent (&event); 
      event.type = ev_keyup; 
      event.data1 = str[i]; 
      D_PostEvent (&event);	
    }
}


