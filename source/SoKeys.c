// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoKeys.c
	\author		Jaap Suter, Erik Rounds
	\date		Jan 21, 2003
	\ingroup	SoKeys

	See the \a SoKeys module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoKeys.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

//! \internal	GBA keypress register
#define SO_REG_KEY					(*(volatile u16*)0x4000130) 

// ----------------------------------------------------------------------------
// Statics
// ----------------------------------------------------------------------------
static u16  s_KeysDown		= 0;	//!< \internal	Contains which keys are down at the moment
static u16	s_KeysPressed	= 0;	//!< \internal	Contains which keys have been pressed
static u16  s_KeysReleased	= 0;	//!< \internal	Contains which keys have been released

static u16	s_nRepeat		= 0;	//!< \internal  Key repeat count
static u16 	s_nRepeatDelay	= 12;	//!< \internal  Key repeat delay
static u16  s_nRepeatSpeed	= 4;	//!< \internal  Key repeat speed

// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Updates the key status. 
	
	This function should be called once each frame in order to maintain
	correct key status bitmaps. The less often this function is called, the
	more likely that some keypresses will be missed.  Calling the function
	more often than once a frame provides no benefit and reduces the value
	of the key transition functions \a SoKeysPressed and \a SoKeysReleased.
*/
// ----------------------------------------------------------------------------
void SoKeysUpdate( void )
{
	// Key down status of previous call to SoKeysUpdate;
	u16 prevKeysDown     = s_KeysDown;

	// Set the new key status;
	s_KeysDown		= ((~SO_REG_KEY) &   SO_10_BITS   );
	s_KeysPressed	= (  s_KeysDown  & (~prevKeysDown));
	s_KeysReleased	= ((~s_KeysDown) &   prevKeysDown );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the current key status bitmap
	
	This function returns a bitmap containing the current up/down status
	of each key.
*/
// ----------------------------------------------------------------------------
u16 SoKeysGetState( void )
{
	return s_KeysDown;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns true if the given keys are down at the moment;

	\param a_Keys	Bitwise OR'ed \a SO_KEY_* defines. One for each key you want to test.

	\return \a TRUE if the key is or keys are down at the moment

	Example usage:

	\code

	if ( SoKeysDown( SO_KEY_A | SO_KEY_B ) ) 
	{
		// Key A and/or B are down;
    }

	\endcode
*/
// ----------------------------------------------------------------------------
bool SoKeysDown( u16 a_Keys )
{
	return ((s_KeysDown & a_Keys) != 0);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Tests the given keys have been pressed in between the last two calls
		   to \a SoKeysUpdate. 

	\param a_Keys		Bitwise OR'ed \a SO_KEY_* defines. One for each key you want to test.
	
	\return \a TRUE if the key has or keys have been pressed.

	A key press means that the key has changed from a non-pressed situation (up
	position) to a pressed situation (down position).
*/
// ----------------------------------------------------------------------------
bool SoKeysPressed(  u16 a_Keys )
{
	return ((s_KeysPressed & a_Keys) != 0);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Tests the given keys have been released in between the last two calls
		   to \a SoKeysUpdate. 

	\param a_Keys	Bitwise OR'ed \a SO_KEY_* defines. One for each key you want to test.
	
	\return \a TRUE if the key has or keys have been released.

	A key release means that the key has changed from a pressed situation (down
	position) to a non-pressed situation (up position).
*/
// ----------------------------------------------------------------------------
bool SoKeysReleased( u16 a_Keys )
{
	return ((s_KeysReleased & a_Keys) != 0);
}
// ----------------------------------------------------------------------------

//! \brief  Not yet supported
bool SoKeysPressedRepeat( u16 a_Keys )
{
	return ( s_nRepeat == 0 || s_nRepeat == s_nRepeatDelay ) &&
	       (s_KeysDown & a_Keys) != 0;
}

//! \brief  Not yet supported
void SoKeysSetRepeatDelay( u16 a_nDelay )
{
	s_nRepeatDelay = a_nDelay;
}

//! \brief  Not yet supported
void SoKeysSetRepeatSpeed( u16 a_nSpeed )
{
	s_nRepeatSpeed = a_nSpeed;
}


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
