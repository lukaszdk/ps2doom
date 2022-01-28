/*
** p_openmap.cpp
**
** creates the data structures needed to load a map from the resource files.
**
**---------------------------------------------------------------------------
** Copyright 2005-2018 Christoph Oelckers
** Copyright 2005-2016 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**-------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>

#ifndef MAKE_ID
#ifndef __BIG_ENDIAN__
#define MAKE_ID(a,b,c,d)	((uint32_t)((a)|((b)<<8)|((c)<<16)|((d)<<24)))
#else
#define MAKE_ID(a,b,c,d)	((uint32_t)((d)|((c)<<8)|((b)<<16)|((a)<<24)))
#endif
#endif

#define PWAD_ID	MAKE_ID('P','W','A','D')

typedef struct
{ 
  char identification[4];  
  int numlumps; 
  int infotableofs;
} pwadinfo_t;

typedef struct
{ 
   int filepos; 
   int size; 
   char name[8];
} filelump_t;

typedef enum 
{ 
  // CPhipps - define elements in order of 'how new/unusual' 
  source_pre, // predefined lump 
  source_auto_load, // lump auto-loaded by config file 
  source_pwad, // pwad file load 
  source_lmp, // lmp file load 
  source_net // CPhipps 
  //e6y//  
  ,source_deh 
  ,source_err
} pwad_source_t;

// CPhipps - changed wad init// We _must_ have the wadfiles[] the same as those actually loaded, so there // is no point having these separate entities. This belongs here.

typedef struct 
{ 
  char* name; 
  pwad_source_t src; 
  int handle;
} pwadfile_info_t;

extern pwadfile_info_t *pwadfiles;

extern size_t numwadfiles; // CPhipps - size of the wadfiles array


typedef enum
{ 
  ns_global=0, 
  ns_sprites, 
  ns_flats, 
  ns_colormaps, 
  ns_ps2doom, 
  ns_demos, 
  ns_hires //e6y
} li_namespace_e; // haleyjd 05/21/02: renamed from "namespace"

typedef struct
{
  // WARNING: order of some fields important (see info.c).

  char  name[9];
  int   size;

  // killough 1/31/98: hash table fields, used for ultra-fast hash table lookup
  int index, next;

  // killough 4/17/98: namespace tags, to prevent conflicts between resources
  li_namespace_e li_namespace; // haleyjd 05/21/02: renamed from "namespace"
 pwadfile_info_t *pwadfile; 
 int position; 
 pwad_source_t source; 
 int flags; //e6y
} lumpinfo_t;

#define LUMP_STATIC 0x00000001 /* assigned gltexture should be static */

extern lumpinfo_t *lumpinfo;
extern int numlumps;
