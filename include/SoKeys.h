// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoKeys.h
	\author		Jaap Suter, Erik Rounds
	\date		Jan 1 2002	
	\ingroup	SoKeys

	See the \a SoKeys module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_KEYS_H
#define SO_KEYS_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoKeys SoKeys
	\brief	  Keypress handling stuff.

	Singleton

	This module handles everything key related.	It contains macros for testing 
	whether or not a key is pressed, down or released. See the different
	functions for the difference between down, pressed and released.
		
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------
#define SO_KEY_A		(SO_BIT_0)		//!< A key identifier;
#define SO_KEY_B		(SO_BIT_1)		//!< B key identifier;
#define SO_KEY_SELECT	(SO_BIT_2)		//!< Select key identifier;
#define SO_KEY_START	(SO_BIT_3)		//!< Start key identifier;
#define SO_KEY_RIGHT	(SO_BIT_4)		//!< D-pad right key identifier;
#define SO_KEY_LEFT		(SO_BIT_5)		//!< D-pad left key identifier;
#define SO_KEY_UP		(SO_BIT_6)		//!< D-pad up key identifier;
#define SO_KEY_DOWN		(SO_BIT_7)		//!< D-pad down key identifier;
#define SO_KEY_R		(SO_BIT_8)		//!< R shoulder key identifier;
#define SO_KEY_L		(SO_BIT_9)		//!< L shoulder key identifier;
		
#define SO_KEY_ANY		(SO_10_BITS)	//!< All key identifiers, handy for any-key presses;

// ----------------------------------------------------------------------------
// Functions;
// ----------------------------------------------------------------------------
void SoKeysUpdate( void );

u16  SoKeysGetState(void);

bool SoKeysDown(	 u16 a_Keys );
bool SoKeysPressed(  u16 a_Keys );
bool SoKeysReleased( u16 a_Keys );

bool SoKeysPressedRepeat( u16 a_Keys );
void SoKeysSetRepeatDelay( u16 a_nDelay );
void SoKeysSetRepeatSpeed( u16 a_nSpeed );



// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
