// ----------------------------------------------------------------------------
/*! 
Copyright (C) 2002 by the SGADE authors
For conditions of distribution and use, see copyright notice in SoLicense.txt

\file		SampleEffectMap.c
\author		Mark Price
\date		April 3 2002
\ingroup	SoSampleEffects

See the \a SoSampleEffect module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Sample application includes;
#include "SampleEffects.h"

// ----------------------------------------------------------------------------
// Data includes;
// ----------------------------------------------------------------------------
#include "maze.dat"

// ----------------------------------------------------------------------------
// Global variables;
// ----------------------------------------------------------------------------

static SoBkgMap mazeMap;

// ----------------------------------------------------------------------------
// Configuration definitions;
// ----------------------------------------------------------------------------

#define INFO_BKG  0
#define MAZE_BKG  1

#define INFO_CHARBASE 1
#define MAZE_CHARBASE 0


// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

//! \brief displays the current scroll position of the map
//! 
//! The scroll position is defined as the pixel offset of the upper-left
//! corner of the screen within the off-screen virtual map.
//!
static void showScrollPosition(void)
{
	// trailing blanks in format string ensure closing parens are always erased when
	// the length of the scroll position goes down.
	SoBkgSetTextCursor(INFO_BKG, 0, 0);
	SoBkgPrintf(INFO_BKG, "Scroll Position = (%d, %d)  ", mazeMap.m_XPos, mazeMap.m_YPos);
}

// ----------------------------------------------------------------------------
/*!
\brief Displays a very large scrolling tiled background
*/
// ----------------------------------------------------------------------------

void SoSampleEffectMap( void )
{
	// Set the screen palette to the tile set palette;
	SoPaletteSetPalette(SO_SCREEN_PALETTE, palMaze_Palette, true);

	// Set Mode 0 (tile mode)
	SoDisplaySetMode(0);

	// We have two tile sets in this demo, one for the maze and one for the font.
	// Both images were created using the same palette, which is required by the hardware.
	//
	// Both of the tile sets are loaded directly in to VRAM

#define INFO_BKG_OPTIONS (SO_BKG_PRIORITY_0|SO_BKG_CHARBASE(INFO_CHARBASE)|SO_BKG_MOSAIC_DISABLE|SO_BKG_CLRMODE_256|SO_BKG_SCRBASE(0x1e)|SO_BKG_TEXTSIZE_256x256)
#define MAZE_BKG_OPTIONS (SO_BKG_PRIORITY_3|SO_BKG_CHARBASE(MAZE_CHARBASE)|SO_BKG_MOSAIC_DISABLE|SO_BKG_CLRMODE_256|SO_BKG_SCRBASE(0x1f)|SO_BKG_TEXTSIZE_256x256)

	// pre-load both tile sets
	SoTileSetBkgLoad(&tsFont_TileSet, INFO_CHARBASE, 0);
	SoTileSetBkgLoad(&tsMaze_TileSet, MAZE_CHARBASE, 0);

	// Setup BKG
	SoBkgSetup(INFO_BKG, INFO_BKG_OPTIONS);
	SoBkgSetup(MAZE_BKG, MAZE_BKG_OPTIONS);

	// setup map management
	SoBkgMapInit(&mazeMap, SO_BKG_MAP_MODE_RAW);
	SoBkgMapSetBackground(&mazeMap, MAZE_BKG);
	SoBkgMapSetScroll(&mazeMap, 0, 0);
	SoBkgMapSetTileMap(&mazeMap, (SoTileMap*)&tmMaze_TileMap);

	// turn on the BKGs
	SoBkgEnable(INFO_BKG, true);
	SoBkgEnable(MAZE_BKG, true);


	// show initial scroll position
	showScrollPosition();


	// Do the effect until the start key is pressed
	do
	{	
		// Wait for VBlank;
		SoDisplayWaitForVBlankEnd();
		SoDisplayWaitForVBlankStart();

		// update the GBA button status variables
		SoKeysUpdate();

		bool bUpdate = false;
		u16 x = mazeMap.m_XPos;
		u16 y = mazeMap.m_YPos;

		if(SoKeysDown(SO_KEY_LEFT))
		{
			if(x >= 1)
				--x, bUpdate = true;
		}
		else if(SoKeysDown(SO_KEY_RIGHT))
		{
			if(x < (tmMaze_TileMap.m_SizeX*8 - SO_SCREEN_WIDTH))
				++x, bUpdate = true;
		}
		if(SoKeysDown(SO_KEY_UP))
		{
			if(y >= 1)
				--y, bUpdate = true;
		}
		else if(SoKeysDown(SO_KEY_DOWN))
		{
			if(y < (tmMaze_TileMap.m_SizeY*8 - SO_SCREEN_HEIGHT))
				++y, bUpdate = true;
		}

		if(SoKeysDown(SO_KEY_A))
		{
			bUpdate = true;
			x = y = 0;
		}

		if(bUpdate)
		{
			SoBkgMapSetScroll(&mazeMap, x, y);
			showScrollPosition();
		}
	} while (!SoKeysPressed(SO_KEY_START));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
