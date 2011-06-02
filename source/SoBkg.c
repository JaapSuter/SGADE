// ----------------------------------------------------------------------------
/*!
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkg.c
	\author		Mark T. Price, Jaap Suter, Raleigh Cross
	\date		June 16, 2003
	\ingroup	SoBkg

	See the \a SoBkg module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"
#include "SoBkg.h"
#include "SoBkgMap.h"
#include "SoBkgFont.h"
#include "SoBkgMemManager.h"
#include "SoDisplay.h"
#include "SoMath.h"
#include "SoDebug.h"
#include "SoVector.h"
#include "SoDMA.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

//! \internal	Array of the four background control registers. Use
//!				like SO_REG_BKG_CNT[ 1 ], to access the control register
//!				for background number one.
#define SO_REG_BKG_CNT		((volatile u16*)(0x04000008))

//! \internal	this is BKG Video Ram where the screens and tiles are stored for display
#define SO_BKG_DATA     ((volatile u16*)(0x06000000))

// Background control register bits
#define SO_BKG_PRIORITY_MASK  0x0003                  //!< \internal
#define SO_BKG_CHARBASE_MASK  0x000c                  //!< \internal

// horizontal scroll position for BG0-3 (9 bits)
#define SO_REG_BG0HOFS ((volatile u16*)(0x04000010))  //!< \internal
#define SO_REG_BG1HOFS ((volatile u16*)(0x04000014))  //!< \internal
#define SO_REG_BG2HOFS ((volatile u16*)(0x04000018))  //!< \internal
#define SO_REG_BG3HOFS ((volatile u16*)(0x0400001c))  //!< \internal

// vertical scroll position for BG0-3 (9 bits)
#define SO_REG_BG0VOFS ((volatile u16*)(0x04000012))  //!< \internal
#define SO_REG_BG1VOFS ((volatile u16*)(0x04000016))  //!< \internal
#define SO_REG_BG2VOFS ((volatile u16*)(0x0400001a))  //!< \internal
#define SO_REG_BG3VOFS ((volatile u16*)(0x0400001e))  //!< \internal

// ----------------------------------------------------------------------------
// Static variables
// ----------------------------------------------------------------------------

//! \brief		Holds current state of each background
//!
//! \internal 
//!
//!  This array is used to hold the current state information of each
//!  background.  Most of these values could be calculated by reading the
//!  GBA control registers, but it's faster and more convenient to keep them
//!  here.
//!
static struct {
	const SoBkgFont *m_Font;	//!< \internal	active font for this background
	SoVector2 m_TextCursor;		//!< \internal	the location that the next print will occur
	bool m_TextWrapping;		//!< \internal	flag: text wrapping is enabled
	u16  m_TextWindowLeft;
	u16  m_TextWindowRight;
	u16  m_TextWindowTop;
	u16  m_TextWindowBottom;
	u16 *m_Screen;				//!< \internal	points to the BG's screen memory (here for convenience only)
	u16 m_Width;				//!< \internal	width of the BG's screen memory, in tiles
	u16 m_Height;				//!< \internal	height of the BG's screen memory, in tiles
} s_Bkg[SO_BKG_MAX_NUM_BACKGROUNDS];


// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

//!	\brief Prepares a background for use.
//!
//!	\param a_Index		Background index, 0, 1, 2 or 3.
//!	\param a_Options	Combination of BG priority (SO_BKG_PRIORITY_* --
//!						bits 0-1), tile/char base (SO_BKG_CHARBASE -- bits
//!						2-3), mosaic enable (SO_BKG_MOSAIC_ENABLE -- bit 6),
//!						tile color mode (SO_BKG_CLRMODE_* -- bit 7), screen
//!						base (SO_BKG_SCRBASE -- bits 8-12), and BG size
//!						(SO_BKG_TEXTSIZE_* or SO_BKG_ROTSIZE_* -- bits 14-15)
//!
//! This function sets up all of the internal variables needed to work with a
//! background.  This function must be called prior to calling \a SoBkgEnable
//! to turn on the background.  If you are going to be using the
//! \a SoBkgMemManager to dynamically load your tiles, you should call its
//! \a SoBkgMemManagerAllocScreen call to ensure it doesn't try to allocate
//! tiles in the memory used by your screen.
//!
//! \todo	extend to handle mode 1 & 2 screens
//! \todo	extend to handle 512 pixel screen widths (which use rather odd tile
//!         ordering)
//!
void SoBkgSetup( u32 a_Index, u32 a_Options )
{
//! \internal
#define OPTION_BITS (SO_BKG_PRIORITY_MASK|SO_BKG_CHARBASE_MASK|SO_BKG_MOSAIC_ENABLE|SO_BKG_CLRMODE_MASK|SO_BKG_SCRBASE_MASK|SO_BKG_SIZE_MASK)

	// Assert the input;
	SO_ASSERT( a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index" );
	SO_ASSERT((a_Options & ~OPTION_BITS)==0, "Bad option selection");

	// save pointer to screen memory
	//   each increment of the scrbase represents 2048 bytes offset from the start of \a SO_BKG_DATA
	//   since the scrbase takes up bits 8-12 in a_Options, we shift by 3 bits to get a 2048 multiple.
	s_Bkg[a_Index].m_Screen = (u16*)(((u8*)SO_BKG_DATA) + ((a_Options & SO_BKG_SCRBASE_MASK)<<3));

	// save Width/Height of screen memory
	SO_ASSERT((a_Options & SO_BIT_14)==0, "BKG widths of 512 are not currently supported");
	s_Bkg[a_Index].m_Width  = (a_Options & SO_BIT_14) ? 512/8 : 256/8;
	s_Bkg[a_Index].m_Height = (a_Options & SO_BIT_15) ? 512/8 : 256/8;

	// clear the background to all 0's
	SoBkgFill(a_Index, 0);

	// Set background control bits
	SO_REG_BKG_CNT[ a_Index ] = (SO_REG_BKG_CNT[a_Index] & ~OPTION_BITS) | a_Options;

	SoBkgSetOffset(a_Index, 0, 0);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
// \brief Turns off the given background.
//
// \param a_Index	Background index, 0, 1, 2 or 3.
// \param a_Enable	true to enable the background, false to disable it
//
*/
// ----------------------------------------------------------------------------
void SoBkgEnable(u32 a_Index, bool a_Enable)
{
	// Assert the input;
	SO_ASSERT( a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index" );

	// Set it;
	if(a_Enable)
		SO_REG_DISP_CNT	|= (SO_BIT_8 << a_Index);
	else
		SO_REG_DISP_CNT &= ~(SO_BIT_8 << a_Index);
}

//! \brief Tests to see if a background is enabled
//!
//! \param a_Index  Background index 0, 1, 2, or 3.
//!
bool SoBkgIsEnabled(u32 a_Index)
{
	// Assert the input;
	SO_ASSERT( a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index" );

	if(SO_REG_DISP_CNT	& (SO_BIT_8 << a_Index))
		return true;
	else
		return false;
}


// ----------------------------------------------------------------------------
/*!
	\brief	Enables or disables the mosaic effect for a particular background.
	
	\param	a_Index		Identifier for the background, 0, 1, 2 or 3.
	\param  a_Enable	True if you want mosaic enabled, false otherwise.

	\note	If the mosaic values set with \a SoBkgManagerSetMosaic are still zero
			you won't notice a thing of course.
*/
// ----------------------------------------------------------------------------
void SoBkgSetMosaicEnable( u32 a_Index, bool a_Enable )
{
	// Assert the input;
	SO_ASSERT( a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid Background Index" );

	// Set the enable;
	if ( a_Enable )
	{
		SO_REG_BKG_CNT[ a_Index ] |= SO_BKG_MOSAIC_ENABLE;		
	}
	else
	{
		SO_REG_BKG_CNT[ a_Index ] &= ~SO_BKG_MOSAIC_ENABLE;
	}
}

//! \brief	changes the display priority for a particular background
//!
//!	\param	a_Index		Identifier for the background, 0, 1, 2 or 3.
//!	\param  a_Priority	Display priority for the background
//!
//! This function updates the display priority for a given background.
//! Backgrounds with lower priority are displayed in front of backgrounds
//! or objects with highter priority.
//!
//! When the priority of two backgrounds is the same, the background with the
//! lower index number is displayed on top.  When the priority of a sprite is
//! the same as the priority of a background, the sprite is displayed on top.
//!
void SoBkgSetPriority(u32 a_Index, u32 a_Priority)
{
	// validate the input
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_Priority < 4, "Invalid display priority");

	SO_REG_BKG_CNT[a_Index] = (SO_REG_BKG_CNT[a_Index] & SO_BKG_PRIORITY_MASK)
	  | a_Priority;
}

//! \brief	Sets the display offset of a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XPos	X pixel position -- positive distance from the left-most edge of the Screen
//! \param a_YPos	Y pixel position -- positive distance from the top-most edge of the Screen
//!
//! This function changes the current display offset position of a background
//! screen.  Each offset value may be a maximum of 9 bits (0-511).
//!
//! \todo	consider validating the X/Y offsets against the actual screen size
//!         instead of a fixed 512 max
//!
void SoBkgSetOffset(u16 a_Index, u16 a_XPos, u16 a_YPos)
{
	// Validate the input
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XPos < 512, "Invalid X screen offset");
	SO_ASSERT(a_YPos < 512, "Invalid Y screen offset");

	// set offset registers
	SO_REG_BG0HOFS[a_Index<<1] = a_XPos;
	SO_REG_BG0VOFS[a_Index<<1] = a_YPos;
}

//! brief	Gets a pointer to the BKG data in VRAM
//!
//! \param	a_Index	Background index: 0, 1, 2, or 3
//!
//!	\return	a pointer to the BKG's data in VRAM
//!
//! I'm not sure that anyone will ever need this function, but here it is just
//! in case.
//!
//! I should point out that modifying the contents of a background that was
//! created using the *Indirect calls is a very big no-no.  Doing this will
//! result in tiles being loaded and unloaded at the wrong times since their
//! use counts in \a SoBkgMemManager will be incorrect.
//!
u16 *SoBkgGetData(u32 a_Index)
{
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	return s_Bkg[a_Index].m_Screen;
}

//! \brief	Loads data for a single solid color tile into background VRAM
//!
//! \param	a_Index		Background index: 0, 1, 2, or 3
//! \param	a_Tilenum	index value of the tile to write
//! \param  a_ColorIndex The color value to fill the tile
//!
//! This function is used to manually load a solid color tile into the
//! tile memory for a given background.  This could also have been done using
//! a SoTileSet instance containing the data for the solid-color tile, but
//! this function should be smaller.
//!
void SoBkgLoadSolidColorTile(u16 a_Index, u32 a_Tilenum, u16 a_ColorIndex)
{
	u16 charbase = (SO_REG_BKG_CNT[a_Index] & SO_BKG_CHARBASE_MASK) >> 2;
	u32 tileSize;

	// Is this a 16 color background?
	if((SO_REG_BKG_CNT[a_Index] & SO_BKG_CLRMODE_MASK) == SO_BKG_CLRMODE_16)
		tileSize = 8;	// 8 dwords (32 bytes)
	else
	{
		tileSize = 16;	// 16 dwords (64 bytes)
		a_Tilenum <<= 1;
	}

	a_Tilenum <<= 4;	// multiply idx by 32/2 bytes
	charbase <<= 13;		// multiply bank by 16k/2

	SO_DMA_MEMSET(&SO_BKG_DATA[a_Tilenum+charbase], tileSize, 
		((u32)a_ColorIndex << 24) |
		((u32)a_ColorIndex << 16) |
		((u32)a_ColorIndex << 8) |
		a_ColorIndex);
}


// ============================================================================
// ============================================================================
// =====  RAW MODE SCREEN WRITING FUNCTIONS
// ============================================================================
// ============================================================================

//! \brief	Fills the contents of a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_Data	value to use to fill background
//!
//! This function fills an entire background with a single tile value
//!
void SoBkgFill(u16 a_Index, u16 a_Data)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
//	SO_ASSERT(a_Index < 1024, "Invalid tile value");

	// fill the background with a_Data
	SO_DMA_MEMSET(&s_Bkg[a_Index].m_Screen[0],
		(s_Bkg[a_Index].m_Width * s_Bkg[a_Index].m_Height)>>1, 
		((u32)a_Data)<<16 | a_Data);
}

//! \brief	Fills a rectangular block of a background
//!
//!	\param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Width	width of the block of data to be written
//!	\param a_Height height of the block of data to be written
//! \param a_Data	value to use to fill
//!
//! This function fills a rectangular block of a background with a single tile value
//!
void SoBkgFillBlock(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height, u16 a_Data)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");

	// calculate horiz/vert wrap lengths (if any)
	if(a_Width > s_Bkg[a_Index].m_Width - a_XIndex)
	{
		a_Width = s_Bkg[a_Index].m_Width - a_XIndex;
	}
	if(a_Height > s_Bkg[a_Index].m_Height - a_YIndex)
	{
		a_Height = s_Bkg[a_Index].m_Height - a_YIndex;
	}

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	while(a_Height)
	{
		u16 *dstptr = ramMap;
		u16 width;

		// write a single scan line
		for(width = a_Width; width > 0; --width)
		{
			*dstptr++ = a_Data;
		}

		// setup for next line of block (handles vertical wrapping)
		--a_Height;
		ramMap += s_Bkg[a_Index].m_Width;
	}
}


//! \brief	Writes a horizontal strip of data to a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Len	length of the strip of data to be written
//! \param a_buffer	buffer containing the strip of data to be written
//!
//! This function writes a one-tile high strip of data to the screen.  Data is
//! written from left to right and is wrapped.  Data that is wrapped is written
//! to the same display row as unwrapped data.
//! The data does not have to be displayed on the screen for it to be written.
//!
void SoBkgWrite(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");

//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Width, "Block length is wider than the screen");

	// calculate wrap length (if any)
	u16 len2 = 0;
	if(a_Len > s_Bkg[a_Index].m_Width - a_XIndex)
	{
		len2 = a_Len - (s_Bkg[a_Index].m_Width - a_XIndex);
		a_Len -= len2;
//		a_Len = s_Bkg[a_Index].m_Width - a_XIndex;
	}

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	while(a_Len-- > 0)
	{
		*ramMap++ = *a_buffer++;
	}

	// draw wrapped portion (if any)
	if(len2)
	{
		ramMap -= s_Bkg[a_Index].m_Width;
		while(len2-- > 0)
		{
			*ramMap++ = *a_buffer++;
		}
	}
}

//! \brief	Writes a vertical strip of data to a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Len	length of the strip of data to be written
//! \param a_buffer	buffer containing the strip of data to be written
//!
//! This function writes a one-tile wide strip of data to the screen.  Data is
//! written from top to bottom and is wrapped.  Data that is wrapped is written
//! to the same display column as unwrapped data.
//! The data does not have to be displayed on the screen for it to be written.
//!
void SoBkgWriteV(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");

//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Height, "Block length is taller than the screen");


	// calculate wrap length (if any)
	u16 len2 = 0;
	if(a_Len > s_Bkg[a_Index].m_Height - a_YIndex)
	{
//		a_Len = s_Bkg[a_Index].m_Height - a_YIndex;
		len2 = a_Len - (s_Bkg[a_Index].m_Height - a_YIndex);
		a_Len -= len2;
	}

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	while(a_Len-- > 0)
	{
		*ramMap = *a_buffer++;
		ramMap += s_Bkg[a_Index].m_Width;
	}

	// draw wrapped portion (if any)
	if(len2)
	{
		ramMap = s_Bkg[a_Index].m_Screen + a_XIndex;
		while(len2-- > 0)
		{
			*ramMap = *a_buffer++;
			ramMap += s_Bkg[a_Index].m_Width;
		}
	}
}

//! \brief	Writes a block of data to a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Width	width of the block of data to be written
//!	\param a_Height height of the block of data to be written
//! \param a_Pitch	number of tiles per line in the buffer (may be different from width)
//! \param a_buffer	buffer containing the block of data to be written
//!
//! This function writes a two dimensional block of data to the screen.
//! Data is written from left to right on successive lines is wrapped.
//! The data does not have to be displayed on the screen for it to be written.
//!
void SoBkgWriteBlock(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height, u16 a_Pitch, u16 *a_buffer)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");

//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Width, "Block length is wider than the screen");
//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Height, "Block length is taller than the screen");

	// calculate horiz/vert wrap lengths (if any)
	u16 width2  = 0;
	u16 height2 = 0;
	if(a_Width > s_Bkg[a_Index].m_Width - a_XIndex)
	{
//		a_Width = s_Bkg[a_Index].m_Width - a_XIndex;
		width2   = a_Width - (s_Bkg[a_Index].m_Width - a_XIndex);
		a_Width -= width2;
	}
	if(a_Height > s_Bkg[a_Index].m_Height - a_YIndex)
	{
//		a_Height = s_Bkg[a_Index].m_Height - a_YIndex;
		height2   = a_Height - (s_Bkg[a_Index].m_Height - a_YIndex);
		a_Height -= height2;
	}

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	while(a_Height)
	{
		u16 *dstptr = ramMap;
		u16 *srcptr = a_buffer;
		u16 width;

		// write a single scan line
		for(width = a_Width; width > 0; --width)
		{
			*dstptr++ = *srcptr++;
		}

		// draw wrapped portion (if any)
		if(width2)
		{
			dstptr -= s_Bkg[a_Index].m_Width;
			for(width = width2; width > 0; --width)
			{
				*dstptr++ = *srcptr++;
			}
		}

		// setup for next line of block (handles vertical wrapping)
		a_buffer += a_Pitch;
		if(--a_Height == 0)
		{
			if(height2)
			{
				// recalc position for top of screen
				ramMap = s_Bkg[a_Index].m_Screen + a_XIndex;
				a_Height = height2;
				height2 = 0;
			}
		}
		else
		{
			ramMap += s_Bkg[a_Index].m_Width;
		}
	}
}

//! \brief	Writes a block of data to a background
//!		while translating the data to use a specific 
//!		background palette
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Width	width of the block of data to be written
//!	\param a_Height height of the block of data to be written
//! \param a_Pitch	number of tiles per line in the buffer (may be different from width)
//! \param a_PalIndex	the palette slot that the data will use
//! \param a_buffer	buffer containing the block of data to be written
//!
//! This function writes a two dimensional block of data to the screen.
//! Data is written from left to right on successive lines is wrapped.
//! The data does not have to be displayed on the screen for it to be written.
//! (Same as SoBkgWriteBlock, except the data is translated to use a specific 16-color palette)
//! ----------------------------------------------------
//! This will be the first of more functions to come
//! that will deal with backgrounds in 16 color mode
//!
void SoBkgWriteBlock16Color(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height, u16 a_Pitch, u16 a_PalIndex, u16 *a_buffer)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");
	SO_ASSERT(a_PalIndex < 16, "Palette index greater than 16. valid palettes: 0-15");

//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Width, "Block length is wider than the screen");
//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Height, "Block length is taller than the screen");

	// calculate horiz/vert wrap lengths (if any)
	u16 width2  = 0;
	u16 height2 = 0;
	if(a_Width > s_Bkg[a_Index].m_Width - a_XIndex)
	{
//		a_Width = s_Bkg[a_Index].m_Width - a_XIndex;
		width2   = a_Width - (s_Bkg[a_Index].m_Width - a_XIndex);
		a_Width -= width2;
	}
	if(a_Height > s_Bkg[a_Index].m_Height - a_YIndex)
	{
//		a_Height = s_Bkg[a_Index].m_Height - a_YIndex;
		height2   = a_Height - (s_Bkg[a_Index].m_Height - a_YIndex);
		a_Height -= height2;
	}

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	while(a_Height)
	{
		u16 *dstptr = ramMap;
		u16 *srcptr = a_buffer;
		u16 width;


		// write a single scan line
		for(width = a_Width; width > 0; --width)
		{
			// add the palette index data
			// this will be ignored by GBA hardware if in 256 color mode
			*dstptr++ = (*srcptr++) | (a_PalIndex << 12);
		}

		// draw wrapped portion (if any)
		if(width2)
		{
			dstptr -= s_Bkg[a_Index].m_Width;
			for(width = width2; width > 0; --width)
			{
				// add the palette index data
				// this will be ignored by GBA hardware if in 256 color mode
				*dstptr++ = (*srcptr++) | (a_PalIndex << 12);
			}
		}

		// setup for next line of block (handles vertical wrapping)
		a_buffer += a_Pitch;
		if(--a_Height == 0)
		{
			if(height2)
			{
				// recalc position for top of screen
				ramMap = s_Bkg[a_Index].m_Screen + a_XIndex;
				a_Height = height2;
				height2 = 0;
			}
		}
		else
		{
			ramMap += s_Bkg[a_Index].m_Width;
		}
	}
}

// ============================================================================
// ============================================================================
// =====  INDIRECT MODE SCREEN WRITING FUNCTIONS
// ============================================================================
// ============================================================================

//! \brief	Clears the contents of an indirectly loaded background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//!
//! This function releases all of the tiles that were dynamically loaded into
//! a background and clears the background to 0's.
//!
void SoBkgClearIndirect(u16 a_Index)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
//	SO_ASSERT(a_Index < 1024, "Invalid tile value");

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen;
	u32 count = s_Bkg[a_Index].m_Width * s_Bkg[a_Index].m_Height;

	for(; count > 0; --count)
	{
		SoBkgMemManagerFreeTile(*ramMap++);
	}
	SoBkgFill(a_Index, 0);
}

//! \brief	Writes a horizontal strip of data to a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Len	length of the strip of data to be written
//! \param a_buffer	buffer containing the strip of data to be written
//!
//! This function writes a one-tile high strip of data to the screen.  Data in
//! a_buffer represents logical tile numbers.  Actual tile numbers are demand
//! loaded using \a SoBkgMemManager. Data is written from left to right and is
//! wrapped.  The data does not have to be displayed on the screen for it to be
//! written.
//!
void SoBkgWriteIndirect(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");

//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Width, "Block length is wider than the screen");

	// calculate wrap length (if any)
	u16 len2 = 0;
	if(a_Len > s_Bkg[a_Index].m_Width - a_XIndex)
	{
//		a_Len = s_Bkg[a_Index].m_Width - a_XIndex;
		len2   = a_Len - (s_Bkg[a_Index].m_Width - a_XIndex);
		a_Len -= len2;
	}

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	if(SO_REG_BKG_CNT[a_Index] & SO_BKG_CLRMODE_256)
	{
		while(a_Len-- > 0)
		{
			SoBkgMemManagerFreeTile(*ramMap);
			*ramMap++ = SoBkgMemManagerAllocTile(*a_buffer++);
		}

		// draw wrapped portion (if any)
		if(len2)
		{
			ramMap -= s_Bkg[a_Index].m_Width;
			while(len2-- > 0)
			{
				SoBkgMemManagerFreeTile(*ramMap);
				*ramMap++ = SoBkgMemManagerAllocTile(*a_buffer++);
			}
		}
	}
	else
	{
		while(a_Len-- > 0)
		{
			SoBkgMemManagerFreeTile16(*ramMap);
			*ramMap++ = SoBkgMemManagerAllocTile16(*a_buffer++);
		}

		// draw wrapped portion (if any)
		if(len2)
		{
			ramMap -= s_Bkg[a_Index].m_Width;
			while(len2-- > 0)
			{
				SoBkgMemManagerFreeTile16(*ramMap);
				*ramMap++ = SoBkgMemManagerAllocTile16(*a_buffer++);
			}
		}
	}
}


//! \brief	Writes a vertical strip of data to a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Len	length of the strip of data to be written
//! \param a_buffer	buffer containing the strip of data to be written
//!
//! This function writes a one-tile wide strip of data to the screen.  Data in
//! a_buffer represents logical tile numbers.  Actual tile numbers are demand
//! loaded using \a SoBkgMemManager. Data is written from top to bottom and is
//! wrapped.  The data does not have to be displayed on the screen for it to be
//! written.
//!
void SoBkgWriteVIndirect(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");

//	SO_ASSERT(a_Len <= s_Bkg[a_Index].m_Height, "Block length is taller than the screen");

	// calculate wrap length (if any)
	u16 len2 = 0;
	if(a_Len > s_Bkg[a_Index].m_Height - a_YIndex)
	{
//		a_Len = s_Bkg[a_Index].m_Height - a_YIndex;
		len2 = a_Len - (s_Bkg[a_Index].m_Height - a_YIndex);
		a_Len -= len2;
	}

	// calculate starting address in screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	if(SO_REG_BKG_CNT[a_Index] & SO_BKG_CLRMODE_256)
	{
		while(a_Len-- > 0)
		{
			SoBkgMemManagerFreeTile(*ramMap);
			*ramMap = SoBkgMemManagerAllocTile(*a_buffer++);
			ramMap += s_Bkg[a_Index].m_Width;
		}

		// draw wrapped portion (if any)
		if(len2)
		{
			ramMap = s_Bkg[a_Index].m_Screen + a_XIndex;
			while(len2-- > 0)
			{
				SoBkgMemManagerFreeTile(*ramMap);
				*ramMap = SoBkgMemManagerAllocTile(*a_buffer++);
				ramMap += s_Bkg[a_Index].m_Width;
			}
		}
	}
	else
	{
		while(a_Len-- > 0)
		{
			SoBkgMemManagerFreeTile16(*ramMap);
			*ramMap = SoBkgMemManagerAllocTile16(*a_buffer++);
			ramMap += s_Bkg[a_Index].m_Width;
		}

		// draw wrapped portion (if any)
		if(len2)
		{
			ramMap = s_Bkg[a_Index].m_Screen + a_XIndex;
			while(len2-- > 0)
			{
				SoBkgMemManagerFreeTile16(*ramMap);
				*ramMap = SoBkgMemManagerAllocTile16(*a_buffer++);
				ramMap += s_Bkg[a_Index].m_Width;
			}
		}
	}
}


//! \brief	Writes data to a background
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_XIndex	destination X tile index -- counted from the left edge of the screen
//! \param a_YIndex destination Y tile index -- counted from the top edge of the screen
//! \param a_Width	width of the block of data to be written
//!	\param a_Height height of the block of data to be written
//! \param a_Pitch	number of tiles per line in the buffer (may be different from width)
//! \param a_buffer	buffer containing the block of data to be written
//!
//! This function writes a two dimensional block of data to the screen.  Data
//! in a_buffer represents logical tile numbers.  Actual tile numbers are
//! demand loaded using \a SoBkgMemManager.  Data is written from left to right
//! on successive lines and is wrapped. The data does not have to be displayed
//! on the screen for it to be written.
//!
void SoBkgWriteBlockIndirect(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height,
							 u16 a_Pitch, u16 *a_buffer)
{
	// validate inputs
	SO_ASSERT(a_Index < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index");
	SO_ASSERT(a_XIndex < s_Bkg[a_Index].m_Width, "Bad starting position (X)");
	SO_ASSERT(a_YIndex < s_Bkg[a_Index].m_Height, "Bad starting position (Y)");

	// calculate horiz/vert wrap lengths (if any)
	u16 width2  = 0;
	u16 height2 = 0;
	if(a_Width > s_Bkg[a_Index].m_Width - a_XIndex)
	{
//		a_Width = s_Bkg[a_Index].m_Width - a_XIndex;
		width2   = a_Width - (s_Bkg[a_Index].m_Width - a_XIndex);
		a_Width -= width2;
	}
	if(a_Height > s_Bkg[a_Index].m_Height - a_YIndex)
	{
//		a_Height = s_Bkg[a_Index].m_Height - a_YIndex;
		height2   = a_Height - (s_Bkg[a_Index].m_Height - a_YIndex);
		a_Height -= height2;
	}

/*
	// copy the data to the map
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	if(SO_REG_BKG_CNT[a_Index] & SO_BKG_CLRMODE_256)
	{
		while(a_Height--)
		{
			u16 *srcptr = a_buffer;  a_buffer += a_Pitch;
			u16 *dstptr = ramMap;    ramMap   += s_Bkg[a_Index].m_Width;
			int width;

			for(width = 0; width < a_Width; ++width)
			{
				SoBkgMemManagerFreeTile(*dstptr);
				*dstptr++ = SoBkgMemManagerAllocTile(*srcptr++);
			}
		}
	}
	else
	{
		while(a_Height--)
		{
			u16 *srcptr = a_buffer;  a_buffer += a_Pitch;
			u16 *dstptr = ramMap;    ramMap   += s_Bkg[a_Index].m_Width;
			int width;

			for(width = 0; width < a_Width; ++width)
			{
				SoBkgMemManagerFreeTile16(*dstptr);
				*dstptr++ = SoBkgMemManagerAllocTile16(*srcptr++);
			}
		}
	}
*/
	// calculate starting address in  screen & copy data
	u16 *ramMap = s_Bkg[a_Index].m_Screen + a_YIndex * s_Bkg[a_Index].m_Width + a_XIndex;
	while(a_Height)
	{
		u16 *dstptr = ramMap;
		u16 *srcptr = a_buffer;

		u16 width;
		
		if(SO_REG_BKG_CNT[a_Index] & SO_BKG_CLRMODE_256)
		{
			for(width = a_Width; width > 0; --width)
			{
				SoBkgMemManagerFreeTile(*dstptr);
				*dstptr++ = SoBkgMemManagerAllocTile(*srcptr++);
			}

			// draw wrapped portion (if any)
			if(width2)
			{
				dstptr -= s_Bkg[a_Index].m_Width;
				for(width = width2; width > 0; --width)
				{
					SoBkgMemManagerFreeTile(*dstptr);
					*dstptr++ = SoBkgMemManagerAllocTile(*srcptr++);
				}
			}
		}
		else
		{
			for(width = a_Width; width > 0; --width)
			{
				SoBkgMemManagerFreeTile16(*dstptr);
				*dstptr++ = SoBkgMemManagerAllocTile16(*srcptr++);
			}

			// draw wrapped portion (if any)
			if(width2)
			{
				dstptr -= s_Bkg[a_Index].m_Width;
				for(width = width2; width > 0; --width)
				{
					SoBkgMemManagerFreeTile16(*dstptr);
					*dstptr++ = SoBkgMemManagerAllocTile16(*srcptr++);
				}
			}
		}

		// setup for next line of block (handles vertical wrapping)
		a_buffer += a_Pitch;
		if(--a_Height == 0)
		{
			if(height2)
			{
				// recalc position for top of bkg
				ramMap = s_Bkg[a_Index].m_Screen + a_XIndex;
				a_Height = height2;
				height2 = 0;
			}
		}
		else
		{
			ramMap += s_Bkg[a_Index].m_Width;
		}
	}
}

// ============================================================================
// ============================================================================
// =====  TEXT / PRINTING FUNCTIONS
// ============================================================================
// ============================================================================

//!	\brief Sets the position that the next print will occur
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_X		X-coordinate of cursor
//! \param a_Y		Y-coordinate of cursor
//!
//! This function sets the current text cursor position for a given background.
//!
void SoBkgSetTextCursor(u16 a_Index, u16 a_X, u16 a_Y)
{
	s_Bkg[a_Index].m_TextCursor.m_X = a_X;
	s_Bkg[a_Index].m_TextCursor.m_Y = a_Y;
}

//!	\brief Gets the position that the next print will occur
//!
//! \param a_Index	 Background index: 0, 1, 2, or 3
//!
//! This function retrieves a pointer to the current text cursor position for
//! a given background.
//!
const SoVector2 *SoBkgGetTextCursor(u16 a_Index)
{
	return &s_Bkg[a_Index].m_TextCursor;
}

//!	\brief Sets the font associated with a background
//!
//! \param a_Index	 Background index: 0, 1, 2, or 3
//! \param a_Font    the Font to be associated
//!
//! This function sets the font to be used by a given background
//!
void SoBkgSetFont(u16 a_Index, const SoBkgFont *a_Font)
{
	s_Bkg[a_Index].m_Font = a_Font;
}

//!	\brief Gets the font associated with a background
//!
//! \param a_Index	 Background index: 0, 1, 2, or 3
//!
//! This function retrieves a pointer to the current font used by
//! a given background.
//!
const SoBkgFont *SoBkgGetFont(u16 a_Index)
{
	return s_Bkg[a_Index].m_Font;
}

//! \brief enables or disables text wrapping
//!
//! \param a_Index	 Background index: 0, 1, 2, or 3
//! \param a_DoWrapping flag: true to enable, false to disable wrapping
//! 
//! This function turns text wrapping on or off for a given background.
//! With text wrapping off, printed text is truncated when it goes off the
//! right or bottom edge of the BKG.
//! 
void SoBkgSetTextWrapping(u16 a_Index, bool a_DoWrapping)
{
	s_Bkg[a_Index].m_TextWrapping = a_DoWrapping;
}

//!	\brief Sets the window in which printing is restricted
//!
//! \param a_Index	 Background index: 0, 1, 2, or 3
//! \param a_UpperLeftX  left edge of wrapping window
//! \param a_UpperLeftY  top edge of wrapping window
//! \param a_LowerRightX right edge of wrapping window
//! \param a_LowerRightY bottom edge of wrapping window
//!
//! This function sets the boundaries in which text wrapping is done.
//! All edges are inclusive.
//!
void SoBkgSetTextWindow(u16 a_Index, u16 a_UpperLeftX, u16 a_UpperLeftY, u16 a_LowerRightX, u16 a_LowerRightY)
{
	s_Bkg[a_Index].m_TextWindowLeft   = a_UpperLeftX;
	s_Bkg[a_Index].m_TextWindowTop    = a_UpperLeftY;
	s_Bkg[a_Index].m_TextWindowRight  = a_LowerRightX;
	s_Bkg[a_Index].m_TextWindowBottom = a_LowerRightY;
}

//!	\brief Draws a string to a background
//!
//! \param a_Index	 Background index: 0, 1, 2, or 3
//!	\param a_String	 The string you want to write.
//!
//! This function causes a text string to be copied to the specified background.
//! Before calling this function, you must first load a font and associate it
//! with the background you are using.
//!
void SoBkgPrint(u16 a_Index, const char *a_String)
{
	// --------------------------------------------------------------------
	// This really ain't all that spiffy. All it does is figure how the
	// X and Y indices need to be changed, then it draws the 'character'
	// (tile) at those indices.
	// --------------------------------------------------------------------

	u16 curXindex = s_Bkg[a_Index].m_TextCursor.m_X;
	u16 curYindex = s_Bkg[a_Index].m_TextCursor.m_Y;
	u16 charsInWord = 0;

	const SoBkgFont *pFont = s_Bkg[a_Index].m_Font;
	u16 firstChar;
	u16 lastChar;
	if(pFont)
	{
		firstChar = pFont->m_FirstChar;
		lastChar  = pFont->m_FirstChar + pFont->m_NumChars - 1;
	}
	else
	{
		firstChar = ' ';	// first/last printable ASCII characters
		lastChar  = '~';
	}

	// Loop through a_String
	while( *a_String != '\0' )
	{
		// protect against going outside the BKG boundaries
		if(curXindex >= s_Bkg[a_Index].m_Width || curYindex >= s_Bkg[a_Index].m_Height)
			break;

		// Count charsInWord
		// ------------------------------------------------------------
		// If we aren't already in a word...
		if(charsInWord == 0 && s_Bkg[a_Index].m_TextWrapping)
		{
			// ...count the characters in the word.
			while(
				// The characters that are not part of a word.
				// In ASCII, all escape characters are below 
				// the space character
				*(a_String + charsInWord) > ' ' &&
				*(a_String + charsInWord) <= '~'
				)
			{
				// '-' is included in the word, but is enough for a word-break
				// (note side-effect charsInWord increment takes place after
				// string is dereferenced)
				if(*(a_String + charsInWord++) == '-')
					break;
			}

			// too long to fit on a single line?
			if(charsInWord > ((s_Bkg[a_Index].m_TextWindowRight - s_Bkg[a_Index].m_TextWindowLeft) + 1))
			{
				charsInWord = s_Bkg[a_Index].m_TextWindowRight + 1;
				// set size to remaining space
				if(curXindex < s_Bkg[a_Index].m_TextWindowRight)
					charsInWord -= curXindex;
				else
					charsInWord -= s_Bkg[a_Index].m_TextWindowLeft;
			}
		}

		// Newline if user requests
		if( *a_String == '\n' )
		{
			if(s_Bkg[a_Index].m_TextWrapping)
				curXindex = s_Bkg[a_Index].m_TextWindowLeft;
			else
				curXindex = 0;
			curYindex++;
		}
		else
		{
			// Newline if out of room
			if(s_Bkg[a_Index].m_TextWrapping && ((charsInWord + curXindex) > (s_Bkg[a_Index].m_TextWindowRight+1)))
			{
				curXindex = s_Bkg[a_Index].m_TextWindowLeft;
				curYindex++;
				if(curYindex > s_Bkg[a_Index].m_TextWindowBottom)
					break;
			}

			// Write the character to the screen
			//
			// The ASCII escape characters come before the printable 
			// characters. The first printable character in a SoBkgFont is
			// the space.
			u16 charCode = *a_String;
			if(charCode >= firstChar && charCode <= lastChar)
			{
				charCode -= firstChar;
				if(pFont)
				{
					charCode = pFont->m_CharMap[charCode];
					if(charCode != 0)
						charCode += pFont->m_TileOffset;
				}
			}
			else
				charCode = 0;
			SoBkgWrite( a_Index, curXindex++, curYindex, 1, &charCode );
		}

		if( charsInWord != 0 )
			--charsInWord;
		++a_String;
	}

	s_Bkg[a_Index].m_TextCursor.m_X = curXindex;
	s_Bkg[a_Index].m_TextCursor.m_Y = curYindex;
}

//!	\brief Print a string at a given location
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_X		X-coordinate of cursor
//! \param a_Y		Y-coordinate of cursor
//!
//! This function causes a text string to be copied to a specified location
//! on a background.  It is really just a convenience wrapper around
//! \a SoBkgSetTextCursor and \a SoBkgPrint
//! 
//!
//void SoBkgPrintAt(u16 a_Index, u16 a_X, u16 a_Y, const char *a_String)
//{
//	SoBkgSetTextCursor(a_Index, a_X, a_Y);
//	SoBkgPrint(a_Index, a_String);
//}


//! \brief Formatted print routine
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_Format	Format string (see your stdlib references)
//!
//! This function provides printf-style printing to backgrounds.  It uses the
//! currently active font, cursor position and text window.
//! 
void SoBkgPrintf(u16 a_Index, const char* a_Format, ...)
{
	va_list args;

	va_start(args, a_Format);
		dvsprintf(s_DebugBuffer, a_Format, args);
		SoBkgPrint(a_Index, s_DebugBuffer);
	va_end(args);
}


/* TO BE IMPLEMENTED:::


// read/write API 

SoBkgSetTile(u16 bkg, u16 xidx, u16 yidx, u16 tval)
SoBkgGetTile(u16 bkg, u16 xidx, u16 yidx)
// returns absolute tile at x/y

SoBkgRead(u16 bkg, u16 xidx, u16 yidx, u16 len, u16 *wbuf)
// retrieves absolute tile data to wbuf (left to right)

SoBkgReadV(u16 bkg, u16 xidx, u16 yidx, u16 len, u16 *wbuf)
// retrieves absolute tile data to wbuf (top-to-bottom)

SoBkgReadBlk(u16 bkg, u16 xidx, u16 yidx, u16 height, u16 width, u16 *wbuf)

*/

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
