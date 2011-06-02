// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgMap.c
	\author		Mark T. Price
	\date		Mar 31, 2003
	\ingroup	SoBkgMap

	See the \a SoBkgMap module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoBkgMap.h"
#include "SoMath.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

//! \brief	Initializes the SoBkgMap for use
//!
//! \param a_This	this pointer
//! \param a_Mode	operation mode (SO_BKG_MAP_MODE_RAW or SO_BKG_MAP_MODE_DYNAMIC)
//!
//! \todo	add optional lazy/aggressive release mode when using dynamic mode
//!
void SoBkgMapInit(SoBkgMap *a_This, u16 a_Mode)
{
	SO_ASSERT(a_Mode == SO_BKG_MAP_MODE_RAW || a_Mode == SO_BKG_MAP_MODE_DYNAMIC, "Invalid mode");

	a_This->m_Mode    = a_Mode;
	a_This->m_Bkg     = SO_BKG_NONE;
	a_This->m_XPos    = a_This->m_YPos = 0;
	a_This->m_TileMap = NULL;
}


//! \brief	copy a newly exposed block of tiles from the map to the screen
//!
//! \param a_This	this pointer
//! \param startx	starting x index of rectangle
//! \param starty	starting y index of rectangle
//! \param endx		ending x index of rectangle (inclusive)
//! \param endy		ending y index of rectangle (inclusive)
//!
//! This helper function copies tile information from the SoBkgMap to the SoBkg.
//!
//! \internal
static void SoBkgMapCopyData(SoBkgMap *a_This, u16 startx, u16 starty, u16 endx, u16 endy)
{
	// calculate map source offset
	u16 *srcptr = SoTileMapGetData(a_This->m_TileMap) + starty * SoTileMapGetNumCols(a_This->m_TileMap) + startx;

	// convert end position to width/height
	endx = (endx - startx) + 1;
	endy = (endy - starty) + 1;

	// convert start position to relative offset in 32x32 tile BKG
	starty &= 0x1f;
	startx &= 0x1f;

	if(SoBkgMapIsDynamic(a_This))
		SoBkgWriteBlockIndirect(a_This->m_Bkg, startx, starty, endx, endy, SoTileMapGetNumCols(a_This->m_TileMap), srcptr);
	else
		SoBkgWriteBlock(a_This->m_Bkg, startx, starty, endx, endy, SoTileMapGetNumCols(a_This->m_TileMap), srcptr);
}

//! \brief	Associates the map with a background
//!
//! \param a_This	this pointer
//! \param a_Bkg	The background to associate
//!
//! This function associates or disassociates a background with the background
//! map.  To disassociate, call this function with a_Bkg set to SO_BKG_NONE.
//!
void SoBkgMapSetBackground(SoBkgMap *a_This, u32 a_Bkg)
{
	SO_ASSERT(a_Bkg < SO_BKG_MAX_NUM_BACKGROUNDS && a_Bkg != SO_BKG_NONE, "Invalid background index");

	if(a_This->m_Bkg == a_Bkg)
		return;

	if(a_This->m_Bkg != SO_BKG_NONE && SoBkgMapIsDynamic(a_This))
	{
		SoBkgClearIndirect(a_This->m_Bkg);
	}

	a_This->m_Bkg = a_Bkg;

	if(a_Bkg != SO_BKG_NONE && a_This->m_TileMap != NULL)
	{
		SoBkgMapCopyData(a_This, a_This->m_XPos >> 3, a_This->m_YPos >> 3, (a_This->m_XPos + SO_SCREEN_WIDTH - 1) >> 3,
			(a_This->m_YPos + SO_SCREEN_HEIGHT - 1) >> 3);
	}
}


//! \brief	Associates a TileMap with the BkgMap
//!
//! \param a_This	this pointer
//! \param a_TileMap	The tile map to associate
//!
void SoBkgMapSetTileMap(SoBkgMap *a_This, SoTileMap *a_TileMap)
{
	if(a_This->m_TileMap == a_TileMap)
		return;

	// Assert the input;
	SO_ASSERT(a_This->m_XPos <= SoTileMapGetNumCols(a_TileMap)*8-SO_SCREEN_WIDTH, "TileMap too narrow");
	SO_ASSERT(a_This->m_YPos <= SoTileMapGetNumRows(a_TileMap)*8-SO_SCREEN_HEIGHT,"TileMap too short");

	a_This->m_TileMap = a_TileMap;

	if(a_This->m_Bkg != SO_BKG_NONE)
	{
		if(a_TileMap)
		{
			SoBkgMapCopyData(a_This, a_This->m_XPos >> 3, a_This->m_YPos >> 3, (a_This->m_XPos + SO_SCREEN_WIDTH - 1) >> 3,
				(a_This->m_YPos + SO_SCREEN_HEIGHT - 1) >> 3);
		}
		else
		{
			// clear old screen contents
			if(SoBkgMapIsDynamic(a_This))
				SoBkgClearIndirect(a_This->m_Bkg);
			else
				SoBkgFill(a_This->m_Bkg, 0);
		}
	}
}

//! \brief	Sets the scroll position of a map on a background
//!
//! \param a_This	this pointer
//! \param a_XPos	X pixel position -- positive distance from the left-most edge of the map
//! \param a_YPos	Y pixel position -- positive distance from the top-most edge of the map
//!
//! This function changes the current scroll position of a background.
//! If the map has an associated background, this routine will cause any tiles
//! that are hidden to be unloaded, and any tiles that are exposed to be
//! loaded.
//!
void SoBkgMapSetScroll(SoBkgMap *a_This, u16 a_XPos, u16 a_YPos)
{
	if(a_This->m_TileMap == NULL)
	{
		// just save new scroll position and return
		a_This->m_XPos = a_XPos;
		a_This->m_YPos = a_YPos;
	}

	// Assert the input;
	SO_ASSERT(a_XPos <= SoTileMapGetNumCols(a_This->m_TileMap)*8-SO_SCREEN_WIDTH,  "Invalid scroll position");
	SO_ASSERT(a_YPos <= SoTileMapGetNumRows(a_This->m_TileMap)*8-SO_SCREEN_HEIGHT, "Invalid scroll position");

	if(a_This->m_Bkg != SO_BKG_NONE)
	{
		// 1: calculate screen change

		u16 oldLeft, oldTop, oldRight, oldBottom;	// old x/y start/end
		u16 newLeft, newTop, newRight, newBottom;   // new x/y start/end

		oldLeft   = a_This->m_XPos >> 3;	// calculate old full-tile offset 
		oldTop    = a_This->m_YPos >> 3;
		oldRight  = (a_This->m_XPos + SO_SCREEN_WIDTH - 1) >> 3;
		oldBottom = (a_This->m_YPos + SO_SCREEN_HEIGHT - 1) >> 3;

		newLeft   = a_XPos >> 3;			// calculate new full-tile offset
		newTop    = a_YPos >> 3;
		newRight  = (a_XPos + SO_SCREEN_WIDTH - 1) >> 3;
		newBottom = (a_YPos + SO_SCREEN_HEIGHT - 1) >> 3;

		// 2: load up about-to-be-exposed tiles

		if((oldTop < newTop && oldBottom <= newTop) ||			// new entirely below old
			(oldTop >= newBottom && oldBottom > newBottom) ||	// new entirely above old
			(oldLeft < newLeft && oldRight <= newLeft) ||		// new entirely right of old
			(oldLeft >= newRight && oldRight > newRight))		// new entirely left of old
		{
			// there is no overlap, just reload everything
			SoBkgMapCopyData(a_This, newLeft, newTop, newRight, newBottom);
		}
		else
		{
			// The new and old data overlap.

			// This following code is a bit tricky...
			//
			// When copying tiles below, we always use the entire screen width
			// for the top/bottom rectangles.  Because of this, we limit the
			// left/right rectangles so as to not overlap the bottom/top.  If
			// we didn't do the adjustment we'd needlessly copy the same tile
			// twice, resulting wasted work.
			//
			// All in all, it's better to do the extra math here...

			if(newBottom > oldBottom)
				// exposed new tiles on the bottom
				SoBkgMapCopyData(a_This, newLeft, oldBottom+1, newRight, newBottom);
			else if(newTop < oldTop)
				// exposed new tiles on the top
				SoBkgMapCopyData(a_This, newLeft, newTop, newRight, oldTop-1);

			if(newRight > oldRight)
				// exposed new tiles on the right
				SoBkgMapCopyData(a_This, oldRight+1, SO_MAX(oldTop, newTop), newRight, SO_MIN(oldBottom, newBottom));
			else if(newLeft < oldLeft)
				// exposed new tiles on the left
				SoBkgMapCopyData(a_This, newLeft, SO_MAX(oldTop, newTop), oldLeft-1, SO_MIN(oldBottom, newBottom));
		}

		// 3: update the display offset to show the newly loaded tiles

		SoBkgSetOffset(a_This->m_Bkg, a_XPos & 0xff, a_YPos & 0xff);

		// 4: release newly hidden tiles (aggressively released dynamic mode only)
//		if(SoBkgMapIsDynamic(a_This))
//		{
//			// \todo replace the magic numbers '1' and '11' with expressions based on SO_SCREEN_WIDTH, etc
//			if(SO_ABS((int)newLeft - (int)oldLeft) > 1 || SO_ABS((int)newTop - (int)oldTop) > 11)
//			{
//				// unless I put in a bit of effort to track the actually hidden tiles, I cannot safely free
//				// newly hidden tiles since they may also have been newly loaded tiles.  To cover for this,
//				// I instead opt to just not release any newly hidden tiles.
//			}
//			else
//			{
//				// todo	stop being so lazy :-)  Give the user the option to aggressively release tiles
//			}
//		}
	}

	// save new scroll position

	a_This->m_XPos = a_XPos;
	a_This->m_YPos = a_YPos;
}


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
