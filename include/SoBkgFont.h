// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgFont.h
	\author		Raleigh Cross, Mark Price
	\date		Oct 29, 2003
	\ingroup	SoBkgFont

	See the \a SoBkgFont module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_BKG_FONT_H
#define SO_BKG_FONT_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "SoTileSet.h"

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoBkgFont SoBkgFont
	\brief	  Module for handling 8 by 8 pixel fixed-width font data

	Singleton

	This module encapsulates a tile based font.  It works together with the
	\a SoBkg module.  A font is made up of a set of indices to 8 by 8 pixel
	characters.  The actual characters are stored in a shared \a SoTileSet.

	\todo  Merge with \a SoFont module

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"


// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

//! \brief	Background font structure
//!
//! A single \a SoBkgFont instance represents a mapping from ASCII characters
//! to tile indices in a TileSet.  Note that there is no requirement that the
//! tiles be either 16 or 256 color.  This allows the same font mapping to be
//! used with both 16 and 256 color tile sets -- though to do this care must be
//! taken to ensure that the font indices are valid in both tile sets.
//!
//! All the members are private. Use the methods instead.
//!
//! \todo	Create a variable size extension to this (width only or width/height)
//!
typedef struct  {
	u16  m_FirstChar;	//!< \internal	First valid character in the font
	u16  m_NumChars;	//!< \internal	Number of valid characters in the font
	u16  m_TileOffset;  //!< \internal	Offset added to each mapped index
	u16* m_CharMap;		//!< \internal	character remapping data
} SoBkgFont;



// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

//! \brief Returns the first character in the font character map
#define SoBkgFontGetFirstChar(a_This) ((const u16)((a_This)->m_FirstChar))

//! \brief Returns the number of characters in the font character map
#define SoBkgFontGetNumChars(a_This)  ((const u16)((a_This)->m_NumChars))

//! \brief Returns a pointer to the character map in the font
#define SoBkgFontGetCharMap(a_This)   ((a_This)->m_CharMap)


const SoTileSet *SoBkgGetDefaultFontTileSet();


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_BKG_FONT_H
