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

#include "w_wad.h"
#include "doomtype.h"
#include <stdbool.h>
#include <stdio.h>

#ifndef MAKE_ID
#ifndef __BIG_ENDIAN__
#define MAKE_ID(a,b,c,d)	((uint32_t)((a)|((b)<<8)|((c)<<16)|((d)<<24)))
#else
#define MAKE_ID(a,b,c,d)	((uint32_t)((d)|((c)<<8)|((b)<<16)|((a)<<24)))
#endif
#endif

#define IWAD_ID		MAKE_ID('I','W','A','D')
#define PWAD_ID		MAKE_ID('P','W','A','D')


inline bool P_IsBuildMap(MapData *map)
{
	return false;
}

//===========================================================================
//
// GetMapIndex
//
// Gets the type of map lump or -1 if invalid or -2 if required and not found.
//
//===========================================================================

struct checkstruct
{
	const char lumpname[9];
	bool  required;
};

static const struct checkstruct check[] = 
	{
		{"",		 true},
		{"THINGS",	 true},
		{"LINEDEFS", true},
		{"SIDEDEFS", true},
		{"VERTEXES", true},
		{"SEGS",	 false},
		{"SSECTORS", false},
		{"NODES",	 false},
		{"SECTORS",	 true},
		{"REJECT",	 false},
		{"BLOCKMAP", false},
		{"BEHAVIOR", false},
		//{"SCRIPTS",	 false},
	};

struct MapData
{
   struct ResourceHolder	
   {		
	int *data = NULL;
   
     const char ResourceDel()
     {
	 delete data;
     }
     //TBD: Resource file
     //ResourceHolder &operator=(FResourceFile *other) { data = other; return *this; }
		//FResourceFile *operator->() { return data; }
		//operator FResourceFile *() const { return data; }

   } 
   
   // The order of members here is important
	// Resource should be destructed after MapLumps as readers may share FResourceLump objects
	// For example, this is the case when map .wad is loaded from .pk3 file
     	ResourceHolder resource;

      struct MapLump
	{
	  char Name[8] = { 0 };
	  FileReader Reader;
	} MapLumps[ML_MAX];
	FileReader nofile;

	bool HasBehavior = false;
	bool isText = false;
	bool InWad = false;
	int lumpnum = -1;

        /*Todo:	
void Seek(unsigned int lumpindex)	{		if (lumpindex<countof(MapLumps))		{			file = &MapLumps[lumpindex].Reader;			file->Seek(0, FileReader::SeekSet);		}	}
	 
FileReader &Reader(unsigned int lumpindex)
	{
		if (lumpindex < countof(MapLumps))
		{
			auto &file = MapLumps[lumpindex].Reader;
			file.Seek(0, FileReader::SeekSet);
			return file;
		}
		return nofile;
	}

        FileReader &Reader(unsigned int lumpindex)	{		if (lumpindex < countof(MapLumps))		{			auto &file = MapLumps[lumpindex].Reader;			file.Seek(0, FileReader::SeekSet);			return file;		}		return nofile;	}

        TArray<uint8_t> Read(unsigned lumpindex)	{		TArray<uint8_t> buffer(Size(lumpindex), true); 		Read(lumpindex, buffer.Data(), (int)buffer.Size());		return buffer;	} 	uint32_t Size(unsigned int lumpindex)	{		if (lumpindex<countof(MapLumps) && MapLumps[lumpindex].Reader.isOpen())		{			return (uint32_t)MapLumps[lumpindex].Reader.GetLength();		}		return 0;	} 	bool CheckName(unsigned int lumpindex, const char *name)	{		if (lumpindex < countof(MapLumps))		{			return !strnicmp(MapLumps[lumpindex].Name, name, 8);		}		return false;	}

void GetChecksum(uint8_t cksum[16];
friend class MapLoader;	
friend MapData *P_OpenMapData(const char * mapname, bool justcheck);
*/

}
  



static int GetMapIndex(const char *mapname, int lastindex, const char *lumpname, bool needrequired);

//===========================================================================
//
// Opens a map for reading
//
//===========================================================================

MapData *P_OpenMapData(const char * mapname, bool justcheck);

bool P_CheckMapData(const char *mapname);

//===========================================================================
//
// GetChecksum
//
// Hashes a map based on its header, THINGS, LINEDEFS, SIDEDEFS, SECTORS,
// and BEHAVIOR lumps. Node-builder generated lumps are not included.
//
//===========================================================================

void GetChecksum(uint8_t cksum[16]);

