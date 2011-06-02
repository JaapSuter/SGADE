// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoWindow.h
	\author		Gabriele Scibilia
	\date		February 3 2002
	\ingroup	SoWindow

	See the \a SoWindow module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_WINDOW_H
#define SO_WINDOW_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoWindow SoWindow
	\brief	  All window control handling

	Singleton

	The GBA can display 2 windows simultaneously allowing you to select a
	rectangular portion of the screen. The selected portions can be used to
	control which parts of the screen colors special effects and scrolling are
	applied and which layers can be seen through the windows.

	\note Window 0 has a higher display priority than window 1.

	Make sure you enable a window by using \a SoWindowSet0Enable or
	\a SoWindowSet1Enable cause otherwise you can do all you want but you
	won't see a difference.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

void SoWindowSetWindow0Enable( bool a_Enable );
void SoWindowSetWindow1Enable( bool a_Enable );

void SoWindow0SetPosition( u16 a_UpperLeftX, u16 a_UpperLeftY,
						   u16 a_LowerRightX, u16 a_LowerRightY );
void SoWindow1SetPosition( u16 a_UpperLeftX, u16 a_UpperLeftY,
						   u16 a_LowerRightX, u16 a_LowerRightY );

void SoWindow0BG0InsideEnable( bool a_Enable );
void SoWindow0BG1InsideEnable( bool a_Enable );
void SoWindow0BG2InsideEnable( bool a_Enable );
void SoWindow0BG3InsideEnable( bool a_Enable );
void SoWindow0OBJInsideEnable( bool a_Enable );
void SoWindow0InsideEffectsEnable( bool a_Enable );

void SoWindow1BG0InsideEnable( bool a_Enable );
void SoWindow1BG1InsideEnable( bool a_Enable );
void SoWindow1BG2InsideEnable( bool a_Enable );
void SoWindow1BG3InsideEnable( bool a_Enable );
void SoWindow1OBJInsideEnable( bool a_Enable );
void SoWindow1InsideEffectsEnable( bool a_Enable );

void SoWindow0BG0OutsideEnable( bool a_Enable );
void SoWindow0BG1OutsideEnable( bool a_Enable );
void SoWindow0BG2OutsideEnable( bool a_Enable );
void SoWindow0BG3OutsideEnable( bool a_Enable );
void SoWindow0OBJOutsideEnable( bool a_Enable );
void SoWindow0OutsideEffectsEnable( bool a_Enable );

void SoWindow1BG0OutsideEnable( bool a_Enable );
void SoWindow1BG1OutsideEnable( bool a_Enable );
void SoWindow1BG2OutsideEnable( bool a_Enable );
void SoWindow1BG3OutsideEnable( bool a_Enable );
void SoWindow1OBJOutsideEnable( bool a_Enable );
void SoWindow1OutsideEffectsEnable( bool a_Enable );

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
