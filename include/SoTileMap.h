// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2003 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTileMap.h
	\author		Mark Price
	\date		Mar 1 2003
	\ingroup	SoBkgMap

	See the \a SoBkgMap module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_TILE_MAP_H
#define SO_TILE_MAP_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoTileMap SoTileMap
	\brief	  Background map handling specific functions

	Singleton

	This module encapsulates a tile map used to define a Background.  It works
	together with the \a SoBkg and \a SoBkgMap modules.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

//! \brief	Tile map structure
//!
//! A single \a SoTileMap instance represents a single playfield layout.  The
//! SoTileMap and it's Data may reside in ROM if no data will be written to it
//! (such as for a static scrolling background).  Conversely, the data must
//! reside in RAM if it is to be updated.
//!
//! All the members are private. Use the methods instead.
//!
typedef struct
{
	bool    m_16Colors;			//!< \internal	True if the map is 16 colors, false if 256 colors
	u16     m_PalBank;			//!< \internal	The bank of colors to use for this map if in
								//!<            16 color mode (0 - 15)
	u16     m_SizeX;			//!< \internal  number of columns in map
	u16     m_SizeY;			//!< \internal	number of rows in map
	u16*    m_Data;				//!< \internal	actual map data (tile indices)
} SoTileMap;


// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

//! \brief Checks to see if the map is in 16 color mode
#define SoTileMapIs16Colors(a_This)     ((a_This)->m_16Colors)
//! \brief Returns the 16-color palette bank associated with the map
#define SoTileMapGetPaletteBank(a_This) ((a_This)->m_PalBank)
//! \brief Returns the width of the map in columns
#define SoTileMapGetNumCols(a_This)     ((a_This)->m_SizeX)
//! \brief Returns the height of the map in rows
#define SoTileMapGetNumRows(a_This)     ((a_This)->m_SizeY)
//! \brief Returns a pointer to the map data
#define SoTileMapGetData(a_This)        ((a_This)->m_Data)


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_TILE_MAP_H
