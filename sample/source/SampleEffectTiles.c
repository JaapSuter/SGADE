// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSampleEffectTiles.c
	\author		Mark Price
	\date		April 3, 2003
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
#include "clouds.dat"
#include "ruin.dat"

// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Displays an animated image using a tiled background
*/
// ----------------------------------------------------------------------------
u16 ruin_TileMapBuffer[502];	//!< tile index remapping buffer used by \a SoBkgMemManager

void SoSampleEffectTiles( void )
{
	// Set the screen palette to the image palette;
	SoPaletteSetPalette( SO_SCREEN_PALETTE, ruin_pal, true );
	
	// Go to mode 0;
	SoDisplaySetMode(0);

	// We've got two tile sets in this demo, one for the ruin and one for the clouds.
	// Both images were created using the same palette, which is required by the hardware.
	//
	// For the purposes of illustration, the clouds tileset is loaded directly in to VRAM
	// while the ruin tileset is dynamically loaded as used in SoBkg.
	//
	// NOTE: since the \a SoBkgMemManager is allocates tile data out of the same memory
	//       that is used for the screens (and our own user-managed clouds tile set) we
	//       must tell the \a SoBkgMemManager what memory we're using.  This is done with
	//       the \a SoBkgMemManagerAllocTileSet and \a SoBkgMemManagerAllocBkg calls.

#define CLOUD_BKG_OPTIONS (SO_BKG_PRIORITY_0|SO_BKG_CHARBASE(1)|SO_BKG_MOSAIC_DISABLE|SO_BKG_CLRMODE_256| \
	  SO_BKG_SCRBASE(0x1c)|SO_BKG_TEXTSIZE_256x256)
#define RUIN_BKG_OPTIONS  (SO_BKG_PRIORITY_1|SO_BKG_CHARBASE(0)|SO_BKG_MOSAIC_DISABLE|SO_BKG_CLRMODE_256| \
	  SO_BKG_SCRBASE(0x1d)|SO_BKG_TEXTSIZE_256x256)

	// initialize dynamic tile loader
	SoBkgMemManagerPreInitialize();
	SoBkgMemManagerAllocTileSet(&clouds_TileSet, 1, 0);		// reserve space for clouds tileset
	SoBkgMemManagerAllocBkg(CLOUD_BKG_OPTIONS);				// reserve space for clouds background
	SoBkgMemManagerAllocBkg(RUIN_BKG_OPTIONS);				// reserve space for ruin background
	SoBkgMemManagerInitialize();

	// hand-load cloud tiles
	SoTileSetBkgLoad(&clouds_TileSet, 1, 0);

	// setup demand loading of ruin tiles (requires use of SoBkg*Indirect calls)
	SoBkgMemManagerSetTileSet(&ruin_TileSet, &ruin_TileMapBuffer[0]);

	// setup BKG 0 (for clouds)
	SoBkgSetup(0, CLOUD_BKG_OPTIONS);
	{
		// write cloud bitmap 2 blocks high & 2 blocks wide (total 32x32 tiles)
		int xx, yy;
		for(yy = 0; yy < 2; ++yy)
			for(xx = 0; xx < 2; ++xx)
			{
				SoBkgWriteBlock(0, xx*SoTileMapGetNumCols(&clouds_map),
					yy*SoTileMapGetNumRows(&clouds_map), SoTileMapGetNumCols(&clouds_map),
					SoTileMapGetNumRows(&clouds_map), SoTileMapGetNumCols(&clouds_map),
					SoTileMapGetData(&clouds_map));
			}
	}

	// setup BKG 1 (for building ruin)
	SoBkgSetup(1, RUIN_BKG_OPTIONS);
	SoBkgWriteBlockIndirect(1, 0, 0, SoTileMapGetNumCols(&ruin_map), SoTileMapGetNumRows(&ruin_map),
		SoTileMapGetNumCols(&ruin_map), SoTileMapGetData(&ruin_map));

	// set alpha blending of clouds over ruin
	SoEffectsSetMode(SO_EFFECTS_MODE_BLEND, SO_EFFECTS_TARGET1_BG0, SO_EFFECTS_TARGET2_BG1);
	SoEffectsSetBlend(8, 8);

	// turn on BKGs
	SoBkgEnable(0, true);
	SoBkgEnable(1, true);

	// Do the effect until the start key is pressed
	int xoffset = 0;
	int yoffset = 0;
	bool autoscroll = true;
	do
	{	
		SoBkgSetOffset(0, xoffset, yoffset);

		// Wait for VBlank;
		SoDisplayWaitForVBlankEnd();
		SoDisplayWaitForVBlankStart();

		// Update the keys;
		SoKeysUpdate();

		// move clouds to right until user presses cursor keys, then move based on cursor direction
		if(SoKeysDown(SO_KEY_LEFT))  xoffset += 1, autoscroll = false;
		if(SoKeysDown(SO_KEY_RIGHT)) xoffset -= 1, autoscroll = false;
		if(SoKeysDown(SO_KEY_UP))	 yoffset += 1, autoscroll = false;		
		if(SoKeysDown(SO_KEY_DOWN))  yoffset -= 1, autoscroll = false;
		if(autoscroll)
		{
			xoffset += 1;
		}

		if(xoffset >= 16*8)
			xoffset -= 16*8;
		else if(xoffset < 0)
			xoffset += 16*8;
		if(yoffset >= 16*8)
			yoffset -= 16*8;
		else if(yoffset < 0)
			yoffset += 16*8;
	} while (!SoKeysPressed(SO_KEY_START));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
