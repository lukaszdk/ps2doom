/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 * Copyright 2022 by André Guilherme 
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *  mmus2mid.c supports conversion of MUS format music in memory
 *  to MIDI format 1 music in memory.
 */

#if !defined( MMUS2MID_H )
#define MMUS2MID_H

// error codes

typedef enum
{
  MUSDATACOR,    // MUS data corrupt
  TOOMCHAN,      // Too many channels
  MEMALLOC,      // Memory allocation error
  MUSDATAMT,     // MUS file empty
  BADMUSCTL,     // MUS event 5 or 7 found
  BADSYSEVT,     // MUS system event not in 10-14 range
  BADCTLCHG,     // MUS control change larger than 9
  TRACKOVF,      // MIDI track exceeds allocation
  BADMIDHDR,     // bad midi header detected
} error_code_t;

// some names for integers of various sizes, all unsigned
typedef unsigned char UBYTE;  // a one-byte int
typedef unsigned short UWORD; // a two-byte int
// proff: changed from unsigned int to unsigned long to avoid warning
typedef unsigned long ULONG;   // a four-byte int (assumes int 4 bytes)

#ifndef MSDOS /* proff: This is from allegro.h */
#define MIDI_TRACKS           32

typedef struct MIDI                    /* a midi file */
{
   int divisions;                      /* number of ticks per quarter note */
   struct {
      unsigned char *data;             /* MIDI message stream */
      int len;                         /* length of the track data */
   } track[MIDI_TRACKS];
} MIDI;
#endif /* !MSDOS */

//MUS format header structure

typedef struct
{
	char        ID[4];            // identifier "MUS"0x1A
	UWORD       ScoreLength;      // length of music portion
	UWORD       ScoreStart;       // offset of music portion
	UWORD       channels;         // count of primary channels
	UWORD       SecChannels;      // count of secondary channels
	UWORD       InstrCnt;         // number of instruments
} MUSheader;

// to keep track of information in a MIDI track

typedef struct Track
{
	char  velocity;
	long  deltaT;
	UBYTE lastEvt;
	long  alloced;
} TrackInfo;

// array of info about tracks

static TrackInfo track[MIDI_TRACKS];

// initial track size allocation
#define TRACKBUFFERSIZE 1024

// lookup table MUS -> MID controls
static UBYTE MUS2MIDcontrol[15] =
{
	0,         // Program change - not a MIDI control change
	0x00,      // Bank select
	0x01,      // Modulation pot
	0x07,      // Volume
	0x0A,      // Pan pot
	0x0B,      // Expression pot
	0x5B,      // Reverb depth
	0x5D,      // Chorus depth
	0x40,      // Sustain pedal
	0x43,      // Soft pedal
	0x78,      // All sounds off
	0x7B,      // All notes off
	0x7E,      // Mono
	0x7F,      // Poly
	0x79       // Reset all controllers
};

// some strings of bytes used in the midi format

static UBYTE midikey[] ={0x00,0xff,0x59,0x02,0x00,0x00}; // C major
static UBYTE miditempo[] ={0x00,0xff,0x51,0x03,0x09,0xa3,0x1a}; // uS/qnote
static UBYTE midihdr[] ={'M','T','h','d',0,0,0,6,0,1,0,0,0,0}; // header length 6, format 1
static UBYTE trackhdr[] ={'M','T','r','k'}; //track header


static int TWriteByte(MIDI *mididata, int MIDItrack, UBYTE byte);
static int TWriteVarLen(MIDI *mididata, int MIDItrack, register ULONG value);
static ULONG ReadTime(const UBYTE **musptrp);
static int FirstChannelAvailable(int MUS2MIDchannel[]);
static UBYTE MidiEvent(MIDI *mididata,UBYTE midicode,UBYTE MIDIchannel, UBYTE MIDItrack, int nocomp);


extern int mmus2mid(const UBYTE *mus,MIDI *mid, UWORD division, int nocomp); 
extern void free_mididata(MIDI *mid);
extern int MIDIToMidi(MIDI *mididata,UBYTE **mid,int *midlen);
extern int MidiToMIDI(UBYTE *mid,MIDI *mididata);

#endif
