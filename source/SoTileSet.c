// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTileSet.c
	\author		Jaap Suter, Mark T. Price
	\date		December 29, 2002
	\ingroup	SoTileSet

	See the \a SoTileSet module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoTileSet.h"
#include "SoDMA.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! \internal	this is BKG Video Ram where the ram tiles are stored for display
#define SO_BKG_DATA     ((volatile u16*)(0x06000000))

// ----------------------------------------------------------------------------
/*!
	\brief	To get the size of the entire tileset. 
	\param	a_This This pointer
	\return	The size of the entire tileset in bytes.

	Because a single tile is 8x8, an entire tileset is the number of tiles
	times 8 times 8 bytes big. Unless they are 4-bit palettized (16 color tiles),
	cause then this value needs to be divided by two, cause there's two pixels in each 
	byte.
*/
// ----------------------------------------------------------------------------
u32 SoTileSetGetSize( const SoTileSet* a_This )
{
	// Is this a 16 color tile;
	if ( a_This->m_16Colors )
	{
		// Multiply by 32 (8*8/2);
		return (a_This->m_NumTiles << 5);
	}
	else
	{
		// Multiply by 64 (8*8);
		return (a_This->m_NumTiles << 6);
	}
}
// ----------------------------------------------------------------------------

#if 0

// ----------------------------------------------------------------------------
/*!
	\brief	Tells you whether the tileset is 16 colors or 256 colors;
	
	\param	a_This This pointer
	
	\return	True if 16 color tile, false if 256 color tile;
*/
// ----------------------------------------------------------------------------
bool SoTileSetIs16Colors( const SoTileSet* a_This )
{
	return a_This->m_16Colors;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/*!
	\brief	To get the number of tiles in this set.
	\param	a_This This pointer
	\return	The number of tiles in the set.
*/
// ----------------------------------------------------------------------------
u32  SoTileSetGetNumTiles( 	const SoTileSet* a_This ) 
{ 
	return a_This->m_NumTiles; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	To get the tile data itself.
	\param	a_This This pointer
	\return	The tiledata in an \a u16* format so you can easily copy it to 
			VRAM directly.
*/
// ----------------------------------------------------------------------------
u16* SoTileSetGetData( const SoTileSet* a_This ) 
{ 
	return (u16*) a_This->m_Data; 
}
// ----------------------------------------------------------------------------

#endif // 0

//! \brief	Loads tile data into background VRAM
//!
//! \param	a_This		this pointer
//! \param	a_CharBase	charbase bank (0-3)
//! \param	a_BaseIdx	index value of first tile to copy from SoTileSet
//!
void SoTileSetBkgLoad(const SoTileSet* a_This, u32 a_CharBase, u32 a_BaseIdx)
{
	u32 tilesetSize = SoTileSetGetSize(a_This) >>1;

	SO_ASSERT(a_CharBase < 4, "Bad char bank");
	SO_ASSERT(a_BaseIdx < a_This->m_NumTiles, "Bad starting tile");

	if(!a_This->m_16Colors)
	{
		a_BaseIdx <<= 1;
	}
	a_BaseIdx <<= 4;	// multiply idx by 32/2 bytes
	a_CharBase <<= 13;		// multiply bank by 16k/2

	SO_DMA_MEMCPY(&a_This->m_Data[a_BaseIdx],
	  &SO_BKG_DATA[a_BaseIdx+a_CharBase], tilesetSize>>1);
}

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
