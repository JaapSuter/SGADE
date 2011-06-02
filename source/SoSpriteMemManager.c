// ----------------------------------------------------------------------------
/*!
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSpriteMemManager.c
	\author		Jaap Suter, Gabriele Scibilia
	\date		June 28, 2003
	\ingroup	SoSpriteMemManager

	See the \a SoSpriteMemManager module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSpriteMemManager.h"
#include "SoTileSet.h"
#include "SoDMA.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! \internal Start of sprite (tile, character, whatever) memory.
#define SO_SPRITE_MEMORY			((u16*)0x6010000)

#define SO_SPRITE_MEMORY_SIZE		32768	//!< \internal Number of bytes available for sprite tiles;
#define SO_MIN_SPRITE_BLOCK_SIZE	32		//!< \internal An 8x8 4 bit palettized tile takes up 32 bytes;
#define SO_SPRITE_BLOCK_SIZE_SHIFT	5		//!< \internal 2^5 == 32, this value can be used to divide by the blocksize.
#define SO_MAX_NUM_SPRITE_BLOCKS	1024	//!< \internal SO_SPRITE_TILE_MEM_SIZE / MIN_SPRITE_BLOCK_SIZE == 1024;

#define SO_SPRITE_BLOCK_FREE		0		//!< \internal Value to mark a block as free
#define SO_SPRITE_BLOCK_USED		1		//!< \internal Value to mark a block as used
#define SO_SPRITE_BLOCK_CONTINUE	2		//!< \internal Value to mark a block as continued (same
											//!<		  as the one before that, either free or used)

// ----------------------------------------------------------------------------
// Statics
// ----------------------------------------------------------------------------

//! \internal This becomes true once the sprite memory manager is initialized;
static bool	s_Initialized = false;

//! \internal This is for smart sprite memory managment;
static u8	s_SpriteAllocationTable[ SO_MAX_NUM_SPRITE_BLOCKS ];

//! \internal This is true when the GBA is in bitmapped modes (3, 4 or 5). We
//!			  need to know this, because we can only use half of the available
//!			  character data in that case;
static bool	s_InBitmappedMode;

// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Initializes the sprite memory manager.

	This should be the first function you call before using any other functions
	of \a SoSpriteMemManager.
*/
// ----------------------------------------------------------------------------
void SoSpriteMemManagerInitialize( void )
{
	// Dummy counter;
	u32 i;

	// If we are already initialized we can return;
	if ( s_Initialized ) return;

	// Initialize the sprite allocation table;
	for ( i = 0; i < SO_MAX_NUM_SPRITE_BLOCKS; i++ )
	{
		s_SpriteAllocationTable[ i ] = SO_SPRITE_BLOCK_FREE;
	}

	// We're initialized;
	s_Initialized = true;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Searches in sprite memory for a free slot and returns it.

	Helper function internally used by the \a SoSpriteMemManagerLoad and
	\a SoSpriteMemManagerLoadFromImage public methods.

	\internal
*/
// ----------------------------------------------------------------------------
u32 SoSpriteMemManagerSearchFreeSlot( u32 numBlocksInSet )
{
	// Dummy counters;
	u32 i, j;

	// If we are in bitmapped mode we start halfway;
	if ( s_InBitmappedMode )
	{
		// Divide the maximum number of sprites by two to get halfway;
		i = (SO_MAX_NUM_SPRITE_BLOCKS >> 1);
	}
	else
	{
		// We start at the beginning;
		i = 0;
	}

	// Search for a free spot the size of the tileset;
	// We only want to place the tileset on boundaries that are equal to the
	// tile-size itself, that explains the add value;
	for ( ; i < SO_MAX_NUM_SPRITE_BLOCKS; i += numBlocksInSet )
	{
		// Is this block free;
		if ( s_SpriteAllocationTable[ i ] == SO_SPRITE_BLOCK_FREE )
		{
			// Yes so now we check whether this sprite will fit;
			for ( j = 0; j < numBlocksInSet; j++ )
			{
				// Is this block not out of bounds;
				if ( j + i > SO_MAX_NUM_SPRITE_BLOCKS ) break;

				// Is this block free;
				if ( s_SpriteAllocationTable[ j + i ] != SO_SPRITE_BLOCK_FREE )
				{
					// This block is not free, so we cannot
					// place the tileset here;
					break;
				}
			}

			// If the loop finished completely this block can be used;
			if ( j == numBlocksInSet )
			{
				// Mark the first block as used;
				s_SpriteAllocationTable[ i ] = SO_SPRITE_BLOCK_USED;

				// Mark the rest of the blocks as used-continued;
				for ( j = 1; j < numBlocksInSet; j++ )
				{
					s_SpriteAllocationTable[ j + i ] = SO_SPRITE_BLOCK_CONTINUE;
				}

				// Return the index of the sprite in sprite memory;
				return i;
			}
		}
	}

	// If we reach this place we didn't find a spot in the sprite memory
	// to place the tileset data. This should never happen;
	SO_ASSERT( false, "Unable to find a free memory location to place the sprite." );

	// For release build we simply return the start of the sprite memory,
	// but then again, this should never happen;
	return 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Loads a animation in sprite memory and returns the index in sprite memory.

	\param a_Animation		Animation you want to load.

	You can use the returned index as an index for a sprite object, and the sprite object
	will automatically use the given tileset. Like this:
	\code

		SoSprite* sprite = SoSpriteManagerRequestSprite();
		SoSpriteSetAnimationIndex( someSprite, SoSpriteMemManagerLoad( someAnimation ) );

	\endcode

	This function will never fail. If there is no more room in sprite memory, an assertion
	will be fired in debug mode and you should rearrange your animations, or lower the
	game requirements. In release mode this function will not load the tileset, and just
	return zero as an index. But since release mode is functionality-wise the same as debug
	mode this should never happen, cause you've made it work in debug mode first, right?

	Mhwuahuhahaha
*/
// ----------------------------------------------------------------------------
u32 SoSpriteMemManagerLoad( const SoSpriteAnimation* a_Animation )
{
	// The free slot;
	u32 a_FreeSlot;

	// Variable to store a pointer into sprite memory;
	u16* spriteMem;

	// Calculate the number of blocks in the tile set;
	u32 numBlocksInSet = SoSpriteAnimationGetSize( a_Animation ) >> SO_SPRITE_BLOCK_SIZE_SHIFT;

	// Calculate the number of 16 bit halfwords in the set;
	u32 tileSet16BitSize = (SoSpriteAnimationGetSize( a_Animation ) >> 1);

	// Search for a free slot to put the sprite into;
	a_FreeSlot = SoSpriteMemManagerSearchFreeSlot( numBlocksInSet );

				// Get the start of the location where we are going
				// to put the sprite;
				// We multiply by the number of bytes in the block, and
				// we divide by two (shift - 1) because this pointer is not in bytes
				// but in halfwords;
	spriteMem = &SO_SPRITE_MEMORY[ a_FreeSlot << (SO_SPRITE_BLOCK_SIZE_SHIFT - 1) ];

				// Copy the sprite data to sprite memory;
				SoDMATransfer( 3, SoSpriteAnimationGetData( a_Animation ), spriteMem, tileSet16BitSize,
					SO_DMA_DEST_INC | SO_DMA_SOURCE_INC | SO_DMA_16 | SO_DMA_START_NOW );

				// Return the index of the sprite in sprite memory;
	return a_FreeSlot;
		}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/** Load an individual animation frame into memory.

 This works the same as SoSpriteMemManagerLoad() except that only one frame
 is loaded into memory.

 \param a_Frame The zero-based index of the frame to load.
 \param a_Animation		Animation you want to load from
 \return The sprite block index.
*/
// ----------------------------------------------------------------------------
u32 SoSpriteMemManagerLoadFrame( u16 a_Frame, const SoSpriteAnimation* a_Animation)
{
	SO_ASSERT( a_Frame < SoSpriteAnimationGetNumFrames(a_Animation), "Invalid frame index");

	u32 numBlocksInSet = SoSpriteAnimationGetNumIndicesPerFrame(a_Animation);
	u32 freeSlot = SoSpriteMemManagerSearchFreeSlot(numBlocksInSet);
	u32 numHalfWordsInSet = numBlocksInSet << 4;

	u16* dst = &SO_SPRITE_MEMORY[freeSlot << 4];
	u8* src = SoSpriteAnimationGetData(a_Animation) + a_Frame * (numBlocksInSet << 5);
	SoDMATransfer(3, src, dst, numHalfWordsInSet, SO_DMA_DEST_INC|SO_DMA_SOURCE_INC|SO_DMA_16|SO_DMA_START_NOW);

	return freeSlot;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/** Copy an individual animation frame into memory.

 This works the same as SoSpriteMemManagerLoadFrame() except that 
 instead of loading the frame to a free slot,
 it overwrites the slot pointed to by a_Index

 \param a_Index The slot to overwrite with the animation frame.
 		This normally refers to the base sprite index
 \param a_Frame The zero-based index of the frame to copy from.
 \param a_Animation The animation form which to obtain the frame to copy.
 
 Used together with \a SoSpriteMemManagerLoadFrame(), DMA animation method can be used.
 for example:
 \code
 	// load the first frame and get the base sprite index
 	g_BaseSprite = SoSpriteMemManagerLoadFrame(0, &myAnimation);
 	
 	// more code below ....
 	
 	// ... when you require to switch to the next animation frame
 	nextFrame++;
 	SoSpriteMemManagerCopyFrame(g_BaseSprite, nextFrame, &myAnimation);
 	
 \endcode
 
 
*/
// ----------------------------------------------------------------------------
void SoSpriteMemManagerCopyFrame(u32 a_Index, u16 a_Frame, const SoSpriteAnimation* a_Animation)
{
	// check that it is within the valid number of frames for an animation
	SO_ASSERT( a_Frame < SoSpriteAnimationGetNumFrames(a_Animation), "Invalid frame index");
	u32 numBlocksInSet = SoSpriteAnimationGetNumIndicesPerFrame(a_Animation);
	u32 numHalfWordsInSet = numBlocksInSet << 4;
	// destination is set to the base sprite index
	u16* dst = &SO_SPRITE_MEMORY[ a_Index << (SO_SPRITE_BLOCK_SIZE_SHIFT - 1) ];
	u8* src = SoSpriteAnimationGetData(a_Animation) + a_Frame * (numBlocksInSet << 5);
	// DMA transfer the wanted frame 
	SoDMATransfer(3, src, dst, numHalfWordsInSet, SO_DMA_DEST_INC|SO_DMA_SOURCE_INC|SO_DMA_16|SO_DMA_START_NOW);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Copies an image in sprite memory.

	\param a_Index		Index in sprite memory to load data to.
	\param a_Image		Image you want to load.

	Note that image should be palettized (256 color palette mode) and its dimensions
	should be sprite-compatible.
*/
// ----------------------------------------------------------------------------
void SoSpriteMemManagerCopyFromImage( u32 a_Index, SoImage* a_Image )
{
	// Variable to store a pointer into sprite memory;
	u16* spriteMem;

	// Remember: you must supply a palettized image while copying to sprite memory;
	SO_ASSERT( SoImageIsPalettized( a_Image ), "Image must be palettized." );

	// \todo
	// Check for valid sprite size/dimensions, only the following are allowed:
	// 8,8,  16,16, 32,32, 64,64,      = square sprite
	// 16,8, 32,8,  32,16, 64,32,      = wide
	// 8,16, 8,32,  16,32, 32,64       = tall

	// Get the start of the location where we are going
	// to put the sprite;
	// We multiply by the number of bytes in the block, and
	// we divide by two (shift - 1) because this pointer is not in bytes
	// but in halfwords;
	spriteMem = &SO_SPRITE_MEMORY[ a_Index << (SO_SPRITE_BLOCK_SIZE_SHIFT - 1) ];

	// Copy the image data to sprite memory, block-by-block;
	SoTileSetCopyFromLinearBuffer( (u32 *) SoImageGetData( a_Image ), (u32 *) spriteMem,
		SoImageGetWidth( a_Image ), SoImageGetHeight( a_Image ) );
	}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Loads an image in sprite memory and returns the index in sprite memory.

	\param a_Image		Image you want to load.

	You can use the returned index as an index for a sprite object, just like
	the \a SoSpriteMemManagerLoad method, please refer to its documentation for
	further help.

	Note that image should be palettized (256 color palette mode) and its dimensions
	should be sprite-compatible.
*/
// ----------------------------------------------------------------------------
u32 SoSpriteMemManagerLoadFromImage( SoImage* a_Image )
{
	// The free slot;
	u32 a_FreeSlot;

	// Variable to store a pointer into sprite memory;
	u16* spriteMem;

	// Calculate the number of blocks in the image;
	u32 numBlocksInSet = ( SoImageGetWidth( a_Image ) * SoImageGetHeight( a_Image ) ) >> SO_SPRITE_BLOCK_SIZE_SHIFT;

	// Remember: you must supply a palettized image while copying to sprite memory;
	SO_ASSERT( SoImageIsPalettized( a_Image ), "Image must be palettized." );

	// \todo
	// Check for valid sprite size/dimensions, only the following are allowed:
	// 8,8,  16,16, 32,32, 64,64,      = square sprite
	// 16,8, 32,8,  32,16, 64,32,      = wide
	// 8,16, 8,32,  16,32, 32,64       = tall

	// Search for a free slot to put the sprite into;
	a_FreeSlot = SoSpriteMemManagerSearchFreeSlot( numBlocksInSet );

	// Copy the image data to sprite memory, block-by-block;
	// Get the start of the location where we are going
	// to put the sprite;
	// We multiply by the number of bytes in the block, and
	// we divide by two (shift - 1) because this pointer is not in bytes
	// but in halfwords;
	spriteMem = &SO_SPRITE_MEMORY[ a_FreeSlot << (SO_SPRITE_BLOCK_SIZE_SHIFT - 1) ];

	// Copy the image data to sprite memory, block-by-block;
	SoTileSetCopyFromLinearBuffer( (u32 *) SoImageGetData( a_Image ), (u32 *) spriteMem,
		SoImageGetWidth( a_Image ), SoImageGetHeight( a_Image ) );

	// Return the index of the sprite in sprite memory;
	return a_FreeSlot;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Releases a given tileset index from memory.

	\param  a_Index		Index of the animation (range is [0..1024]).

	Often when you no longer need a sprite and its animation data, you will use it like this:

	\code
		SoSpriteMemManagerRelease( SoSpriteGetAnimationIndex( someSprite ) );
		SoSpriteManagerRelease( someSprite );
	\endcode

	Do not use the index after releasing it, cause no garantuees are made about
	its contents.
*/
// ----------------------------------------------------------------------------
void SoSpriteMemManagerRelease( u32 a_Index )
{
	// Get a pointer to the start of this tileset in
	// the allocation table;
	u8* blockEntry = &s_SpriteAllocationTable[ a_Index ];

	// Clear the first entry, going to the next;
	*blockEntry++ = SO_SPRITE_BLOCK_FREE;

	// Clear the rest of the blocks
	while ( *blockEntry == SO_SPRITE_BLOCK_CONTINUE )
		*blockEntry++ = SO_SPRITE_BLOCK_FREE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Use when you are in mode 3, 4, or 5 when using sprites.

	Call this method if you're in bitmapped mode, to tell this manager that only
	half of the sprite memory is available. Call it before you request any tilesets.
	Forgetting to call this will result in sprites not showing up on the real thing and
	most emulators.
*/
// ----------------------------------------------------------------------------
void SoSpriteMemManagerSetInBitmappedMode( bool a_Enable )
{
	s_InBitmappedMode = true;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
