/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file typedefs.h
   \brief common types and includes
   \author Jens Thiele
*/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>

#else

#ifdef __sun__ 
#warning using sun hack 
// hack because sun headers suck 
// sun headers define int8_t to be char and not signed char 
// you can't even work around it 
#include <inttypes.h> 
#define int8_t signed char 
#else 
 
// todo we should not assume that we use SDL 
// #warning you must define int8_t int16_t ... 
// #error you must define int8_t int16_t ... 
#include <SDL_types.h> 
 
typedef Uint8 uint8_t; 
typedef Sint8 int8_t; 
typedef Uint16 uint16_t; 
typedef Sint16 int16_t; 
typedef Uint32 uint32_t; 
typedef Sint32 int32_t; 
typedef Uint64 uint64_t;
typedef Sint64 int64_t;
#endif 
#endif 

typedef float R;
typedef uint16_t PlayerID;
typedef uint16_t TeamID;
typedef int64_t Frame;

#ifndef M_PI
#define M_PI  3.1415926535f
#endif

#ifdef main
#undef main
#define ADIC_NEED_SDLMAIN
#endif

/*
// std::streambuf
#include <iostream>
// layer 2 mini xml output stream
#include <dope/minixml.h>
// layer 2 xml sax input
#include <dope/xmlsaxinstream.h>
// DOPE_CLASS
#include <dope/typenames.h>
// simple for STL types
#include <dope/dopestl.h>
#include <sigc++/signal_system.h>
*/

#define SIMPLE(d) simple(d,#d)

//#include "genericpointer.h"

// all win conditional code should use WINDOOF
#if defined(__WIN32__) || defined(WIN32)
#define WINDOOF 1
#endif


// default is to dlopen opengl
#define DLOPEN_OPENGL
#ifdef WINDOOF
// do not dynamically load opengl on win
// this is because I only cross-compile for windows and then i have to link
// against the mingw opengl version which is a wrapper around the real opengl
// i think
#undef DLOPEN_OPENGL
#endif

#endif

