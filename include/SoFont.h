// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoFont.h
	\author		Jaap Suter
	\date		Jan 2 2002	
	\ingroup	SoFont

	See the \a SoFont module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_FONT_H
#define SO_FONT_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoFont SoFont
	\brief	  Font functionality
	
	This module is for all font handling, it's rather primitive at the moment,
	but in the future this will change.

	A default font is also implemented in this file, but this does not mean
	that you should define other fonts here too. You can define them anywhere,
	just include this file (\a SoFont.h) and off you go.
	
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoImage.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

//! This is the first character in the char set;
#define SO_FONT_FIRST_CHAR '!'		

//! This is the last character in the char set;
#define SO_FONT_LAST_CHAR  '~'

//! Number of characters in a font.
#define SO_FONT_NUM_CHARACTERS	(SO_FONT_LAST_CHAR - SO_FONT_FIRST_CHAR)

//! Number of pixels in between text lines;
#define SO_FONT_VERTICAL_SPACING 1

//! Number of pixels in between words;
//! \warning Must be an even number, in order to be 
//!			 compatible with mode 4.
#define SO_FONT_HORIZONTAL_SPACING 4

// ----------------------------------------------------------------------------
// Functions;
// ----------------------------------------------------------------------------

const SoImage* SoFontGetDefaultFontImage();

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
