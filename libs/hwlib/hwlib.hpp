// ==========================================================================
//
// File      : hwlib.hpp
// Part of   : C++ hwlib library for close-to-the-hardware OO programming
// Copyright : wouter@voti.nl 2017-2019
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================

// An application defines the appropriate HWLIB_TARGET_* marco and 
// include this hwlib.hpp file.

// The bmptk build system will define the HWLIB_TARGET_* macro
// base on the TARGET specified in the makefile.

#ifndef HWLIB_H
#define HWLIB_H

#define HWLIB_QUOTE( FILE ) #FILE
#define HWLIB_INCLUDE( FILE ) HWLIB_QUOTE( FILE )

// Only one target here: esp32
#define HWLIB_TARGET
   #include HWLIB_INCLUDE ( targets/hwlib-arduino-ide-esp32.hpp )

#ifndef HWLIB_TARGET
   #define HWLIB_TARGET
   #include HWLIB_INCLUDE( targets/hwlib-none.hpp )
#endif

#endif // HWLIB_ALL_H