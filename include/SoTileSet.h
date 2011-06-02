// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTileSet.h
	\author		Jaap Suter, Mark T. Price, Gabriele Scibilia
	\date		June 28, 2003
	\ingroup	SoTileSet

	See the \a SoTileSet module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_TILESET_H
#define SO_TILESET_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoTileSet SoTileSet
	\brief	  Module for handling sets of 8 by 8 pixel tiles.

	A tile is an 8x8 square of palettized pixels. Either referring to a 
	16 color palette, or a 256 color palette.

	You can use the SoConverter tool to convert images to tilesets.

*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

/*!
	\brief	Tileset structure. 

	A single \a SoTileSet instance represents a collection of 8 by 8 pixel
	squares.

	All the members are private. Use the methods instead.
*/
typedef struct
{	
	bool	m_16Colors;		//!< \internal	True if the tileset is 16 colors, false if 256 colors.
	u16		m_NumTiles;		//!< \internal	Number of tiles in the tileset.
	u8*		m_Data;			//!< \internal	Actual tileset data (palette indices).
} SoTileSet;

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

u32	 SoTileSetGetSize(	   const SoTileSet* a_This );
// u32  SoTileSetGetNumTiles( const SoTileSet* a_This );
// bool SoTileSetIs16Colors(  const SoTileSet* a_This );
// u16* SoTileSetGetData(	   const SoTileSet* a_This );

//#define SoTileSetGetSize(a_This)     (a_This->m_NumTiles << (a_This->m_16Colors ? 5 : 6))

//! Returns the number of tiles in the TileSet
#define SoTileSetGetNumTiles(a_This) ((const u16)((a_This)->m_NumTiles))

//! Tests whether the tileset is palettized or not (returns true if palettized)
#define SoTileSetIs16Colors(a_This)  ((const bool)((a_This)->m_16Colors))

//! Returns the actual texture data of the tileset
#define SoTileSetGetData(a_This)     ((u16*)((a_This)->m_Data))

void SoTileSetBkgLoad(const SoTileSet* a_This, u32 a_CharBase, u32 a_BaseIdx);

// ----------------------------------------------------------------------------
// Functions implemented in asm;
// Sadly, these need to be documented in here, cause Doxygen can't handle
// .s files very well.
// ----------------------------------------------------------------------------
/*!
	\brief	Copy a linear buffer to sprite format, which is
			organised in a linear array of 8*8 blocks.

	\param	source	the start of the linear buffer in iwram
	\param	source	the start of the linear buffer in iwram
	\param	dest	the start of the sprite in vram
	\param	width	the width of the iwram buffer in pixels
	\param	height	the width of the iwram buffer in pixels

	This function is located in iwram

	For optimal performance, make sure the source buffer is in iwram
	If your destination buffer is in vram, make sure to only use this in vblank,
	since the accestimes to vram are undefined while refreshing the display

	This function takes ~ 2% cpu time for a 64*64 sprite
*/
// ----------------------------------------------------------------------------
void SoTileSetCopyFromLinearBuffer( u32* source, u32* dest, u32 width, u32 height );


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
