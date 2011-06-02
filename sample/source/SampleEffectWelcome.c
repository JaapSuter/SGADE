// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSampleEffectWelcome.c
	\author		Jaap Suter
	\date		Mar 3 2002
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
#include "sgadeImage.dat"
#include "sgadePalette.dat"

#include "SoDebug.h"
// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Does the welcome effect;	
*/
// ----------------------------------------------------------------------------
void SoSampleEffectWelcome( void )
{
	// Enable the clock on timer two;
//	SoTimerClockEnable( 2 );

	// Set the screen palette to the image palette;
	SoPaletteSetPalette( SO_SCREEN_PALETTE, sgadePalette, true );
	
	// Go to mode 4;
	SoMode4RendererEnable();

	// Do the effect until we press the start key;
	do
	{	
		// Wait for VBlank;
		SoDisplayWaitForVBlankStart();

		// Update the keys;
		SoKeysUpdate();

		// Display the SGADE image;
		SoMode4RendererDrawImage( &sgadeImage );
		
		// Print the clock;
		SoMode4RendererDrawNumber( 10, 230, 150, SoTimerClockGetMinutes(), SoFontGetDefaultFontImage() );
		SoMode4RendererDrawString( 18, 230, 150, ":", SoFontGetDefaultFontImage() );
		SoMode4RendererDrawNumber( 26, 230, 150, SoTimerClockGetSeconds(), SoFontGetDefaultFontImage() );
		
		// Flip;
		SoMode4RendererFlip();

		if(SoKeysPressed(SO_KEY_SELECT))
		{
			SO_ASSERT(false || (true && false) || (!true),
				"This is a test of the tile mode assertion system. If you want to continue past "
				"this point you'll have to recompile the sample after commenting out this line (for "
				"location, see the 'file' and 'line' identifiers above)" );
		}
	
	} while ( ! SoKeysPressed( SO_KEY_START ) );


	// Clear the buffers;
	SoMode4RendererClearFrontAndBackBuffer();
//	SoTimerClockDisable();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
