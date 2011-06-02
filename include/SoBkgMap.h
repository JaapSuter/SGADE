// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2003 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgMap.h
	\author		Mark Price
	\date		Mar 31 2003
	\ingroup	SoBkgMap

	See the \a SoBkgMap module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_BKG_MAP_H
#define SO_BKG_MAP_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoBkgMap SoBkgMap
	\brief	  Background map handling specific functions

	Singleton

	This module provides a simple interface to efficiently scroll a large
	virtual playfield on a Background.  It works together with the \a SoBkg
	and \a SoTileMap modules.

	When using a \a SoBkgMap, it is useful to remember that it assumes that associated
	\a SoBkg's are set to 256x256 pixels.  Using larger \a SoBkg's will cause strange
	artifacts to occur.

	Here is the minimum code required to associate a SoBkg, SoTileMap and SoBkgMap.

	\code
		SoBkgMapSetTileMap(a_This, a_TileMap);
		SoBkgMapSetScroll(a_This, a_XPos, a_YPos);
		SoBkgMapSetBackground(a_This, iBkg);
	\endcode

	
	Here is the fastest code to change the SoTileMap associated with a SoBkgMap and SoBkg.

	\code
		SoBkgDisable(a_Bkg);						// turn off BKG (optional: to avoid nasty artifacts)

		SoBkgMapSetBkg(a_This, SO_BKG_NONE);		// disassociate from the BKG
		SoBkgMapSetTileMap(a_This, NULL);			// disassociate the old tile map
		SoBkgMapSetTileMap(a_This, a_TileMap);		// change the associated tile map
		SoBkgMapSetScroll(a_This, a_XPos, a_YPos);	// pre-load scroll position
		SoBkgMapSetBkg(a_This, a_Bkg);              // reassociate the BKG (reloads tiles)

		SoBkgEnable(a_Bkg, a_Bkg);					// turn on BKG
	\endcode

	This code is faster than the shorter, and potentially more obvious method of
	just associating the new tile map and setting the new scroll position because 
	it minimizes the amount of data written to the BKG.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoBkg.h"
#include "SoDisplay.h"
#include "SoTileMap.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

#define SO_BKG_MAP_MODE_RAW     0x0000	//!< flag - raw mode map
#define SO_BKG_MAP_MODE_DYNAMIC 0x0001  //!< flag - dynamic mode map

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

//! \brief	Background Map structure
//!
//! A single \a SoBkgMap instance represents a virtual playfield for a single
//! BKG layer.  All the members are private.  Use the methods instead.
//!
typedef struct
{
	u16        m_Mode;			//!< \internal  operation mode
	u16        m_Bkg;			//!< \internal	Background associated with this map
	u16	       m_XPos;			//!< \internal current horizontal scroll position (in pixels)
	u16	       m_YPos;			//!< \internal current vertical scroll position (in pixels)
	SoTileMap *m_TileMap;		//!< \internal	actual tile data for this map
} SoBkgMap;


// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

//! \brief Checks to see if the map is uses demand loaded tiles
#define SoBkgMapIsDynamic(a_This)      ((a_This)->m_Mode & SO_BKG_MAP_MODE_DYNAMIC)

//! \brief Returns the background used by the map
#define SoBkgMapGetBackground(a_This)  ((const u16)((a_This)->m_Bkg))

void SoBkgMapInit(SoBkgMap *a_This, u16 a_Mode);
void SoBkgMapSetBackground(SoBkgMap *a_This, u32 a_Index);
void SoBkgMapSetTileMap(SoBkgMap *a_This, SoTileMap *a_TileMap);
void SoBkgMapSetScroll(SoBkgMap *a_This, u16 a_XPos, u16 a_YPos);

//void SoBkgMapWrite(SoBkgMap *a_This, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer);
//void SoBkgMapWrite(SoBkgMap *a_This, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer);
//void SoBkgMapWriteV(SoBkgMap *a_This, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer);
//void SoBkgMapWriteBlock(SoBkgMap *a_This, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height,
//					u16 a_Pitch, u16 *a_buffer);
//const SoBkgFont* SoBkgMapSetFont(SoBkgMap *a_This, const SoBkgFont *a_Font);


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_BKG_MAP_H
