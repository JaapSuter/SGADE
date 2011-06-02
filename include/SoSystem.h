// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSystem.h
	\author		Jaap Suter
	\date		June 5 2001
	\ingroup	SoSystem

	See the \a SoSystem module for more information. 
*/
// ----------------------------------------------------------------------------

#ifndef SO_SYSTEM_H
#define SO_SYSTEM_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoSystem SoSystem
	\brief	  Generic and important Socrates stuff.

	Singleton

	This module contains all important and most generic Socrates stuff. It is
	included by most other Socrates files.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

typedef unsigned char	u8;			//!< Unsigned  8 bit data type.
typedef unsigned short	u16;		//!< Unsigned 16 bit data type.
typedef unsigned long	u32;		//!< Unsigned 32 bit data type.

typedef signed char		s8;			//!< Signed  8 bit data type.
typedef signed short	s16;		//!< Signed 16 bit data type.
typedef signed long		s32;		//!< Signed 32 bit data type.

typedef signed long     sofixedpoint; //!< 32 bit signed fixed point data type.

// Only define this if we are not using C++
#ifndef __cplusplus
	typedef u8				bool;	//!< Boolean value, can be true (not zero) or false (zero). 
#endif

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Functions;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

// Only if not defined yet;
#ifndef NULL
	#define NULL						(0)			//!< This is what NULL-pointers point to.
#endif

// Only define this if we are not using C++
#ifndef __cplusplus
	#define true						(1)			//!< Used for boolean values.
	#define false						(0)			//!< Used for boolean values.
#endif

// Define some hardware specifics;
#define SO_GBA_CLOCKCYCLES_PER_SECOND	(16*1024*1024)	//!< The GBA's clockspeed; 16 Mhz 


// ---------------------------------------
/*!
	\name Single bit defines

	The SO_BIT_ * macros (where * is a number from 0 to 31) can be used to get
	a number in which specific bits are set. For example if you want a number
	in which bit 2 and bit 14 are set, you could use SO_BIT_2 | SO_BIT_14. This
	is the same as using (1<<2)|(1<<14) but it is a lot more readable.

	Note that SO_BIT_0 is the least significant bit of a word, and SO_BIT_31 is
	the most significant bit.

	If you need more than one consequetive bits then check out the SO_ * _BITS
	macros too.
*///@{   
// ---------------------------------------
#define SO_BIT_0					(1)				
#define SO_BIT_1					(1<<1)			
#define SO_BIT_2					(1<<2)			
#define SO_BIT_3					(1<<3)			
#define SO_BIT_4					(1<<4)			
#define SO_BIT_5					(1<<5)			
#define SO_BIT_6					(1<<6)			
#define SO_BIT_7					(1<<7)			
#define SO_BIT_8					(1<<8)			
#define SO_BIT_9					(1<<9)			
#define SO_BIT_10					(1<<10)			
#define SO_BIT_11					(1<<11)			
#define SO_BIT_12					(1<<12)			
#define SO_BIT_13					(1<<13)			
#define SO_BIT_14					(1<<14)			
#define SO_BIT_15					(1<<15)			
#define SO_BIT_16					(1<<16)			
#define SO_BIT_17					(1<<17)			
#define SO_BIT_18					(1<<18)			
#define SO_BIT_19					(1<<19)			
#define SO_BIT_20					(1<<20)			
#define SO_BIT_21					(1<<21)			
#define SO_BIT_22					(1<<22)			
#define SO_BIT_23					(1<<23)			
#define SO_BIT_24					(1<<24)			
#define SO_BIT_25					(1<<25)			
#define SO_BIT_26					(1<<26)			
#define SO_BIT_27					(1<<27)			
#define SO_BIT_28					(1<<28)			
#define SO_BIT_29					(1<<29)			
#define SO_BIT_30					(1<<30)			
#define SO_BIT_31					(1<<31)			
//@}
// ---------------------------------------

// ---------------------------------------
/*!
	\name Multiple bit defines

	The SO_ * _BITS macros (where * is a number from 2 to 32) can be used to
	get a number in which the first * bits are set. For example the binary
	number 000111 is equal to SO_3_BITS. The binary number 0001111000 is equal
	to SO_4_BITS << 3.

	Ofcourse there is no SO_1_BITS definition cause this one would be the same 
	as the SO_BIT_1 definition.

*///@{   
// ---------------------------------------
#define SO_2_BITS					(SO_BIT_2 - 1)			
#define SO_3_BITS					(SO_BIT_3 - 1)				
#define SO_4_BITS					(SO_BIT_4 - 1)				
#define SO_5_BITS					(SO_BIT_5 - 1)				
#define SO_6_BITS					(SO_BIT_6 - 1)				
#define SO_7_BITS					(SO_BIT_7 - 1)				
#define SO_8_BITS					(SO_BIT_8 - 1)				
#define SO_9_BITS					(SO_BIT_9 - 1)				
#define SO_10_BITS					(SO_BIT_10 - 1)				
#define SO_11_BITS					(SO_BIT_11 - 1)				
#define SO_12_BITS					(SO_BIT_12 - 1)				
#define SO_13_BITS					(SO_BIT_13 - 1)				
#define SO_14_BITS					(SO_BIT_14 - 1)				
#define SO_15_BITS					(SO_BIT_15 - 1)				
#define SO_16_BITS					(SO_BIT_16 - 1)				
#define SO_17_BITS					(SO_BIT_17 - 1)				
#define SO_18_BITS					(SO_BIT_18 - 1)				
#define SO_19_BITS					(SO_BIT_19 - 1)				
#define SO_20_BITS					(SO_BIT_20 - 1)				
#define SO_21_BITS					(SO_BIT_21 - 1)				
#define SO_22_BITS					(SO_BIT_22 - 1)				
#define SO_23_BITS					(SO_BIT_23 - 1)				
#define SO_24_BITS					(SO_BIT_24 - 1)				
#define SO_25_BITS					(SO_BIT_25 - 1)				
#define SO_26_BITS					(SO_BIT_26 - 1)				
#define SO_27_BITS					(SO_BIT_27 - 1)				
#define SO_28_BITS					(SO_BIT_28 - 1)				
#define SO_29_BITS					(SO_BIT_29 - 1)				
#define SO_30_BITS					(SO_BIT_30 - 1)				
#define SO_31_BITS					(SO_BIT_31 - 1)				
#define SO_32_BITS					(SO_31_BITS | SO_BIT_31)	
//@}
// ---------------------------------------


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------


//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
