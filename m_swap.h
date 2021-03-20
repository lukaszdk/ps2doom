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
// DESCRIPTION:
//	Endianess handling, swapping 16bit and 32bit.
//
//-----------------------------------------------------------------------------


#ifndef __M_SWAP__
#define __M_SWAP__


#ifdef __GNUG__
#pragma interface
#endif

// cosmito from lsdldoom
#define doom_swap_s(x) \
        ((short int)((((unsigned short int)(x) & 0x00ff) << 8) | \
                              (((unsigned short int)(x) & 0xff00) >> 8))) 

#if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
#define doom_wtohs(x) doom_swap_s(x)
#else
#define doom_wtohs(x) (short int)(x)
#endif


// Endianess handling.
// WAD files are stored little endian.
#ifdef sparc
#define __BIG_ENDIAN__
#endif
#ifdef __BEOS__
#include <byteorder.h>
#if B_HOST_IS_BENDIAN
#define __BIG_ENDIAN__
#endif
#endif
#ifdef __BIG_ENDIAN__
#define SHORT(x)	((short)SwapSHORT((unsigned short) (x)))
#define LONG(x)         ((long)SwapLONG((unsigned long) (x)))
#else
#define SHORT(x)	(x)
#define LONG(x)         (x)
#endif




#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
