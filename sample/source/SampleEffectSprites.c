// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SampleEffectSprites.c
	\author		Jaap Suter
	\date		Mar 3 2002
	\ingroup	SoSampleEffects

	See the \a SoSampleEffect module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------

// Sample application includes;
#include "SampleEffects.h"

// ----------------------------------------------------------------------------
// Data includes
// ----------------------------------------------------------------------------
#include "fighterSpriteAnimation.dat"
#include "fighterPalette.dat"
#include "televisionImage.dat"
#include "televisionPalette.dat"


// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Does the sprite effect;	
*/
// ----------------------------------------------------------------------------
void SoSampleEffectSprites( void )
{
	// Variables;
	SoSprite* sprite	  = NULL;
	SoVector2 spritePos   = { 20, 20 };
	u8		  spriteFrame = 0;
	u8		  spriteAngle = 0;
	
	u32		  animBaseIndex		= 0;
	u32		  animCurrentIndex  = 0;
	
	s32		  mosaic	  = 15;
	s32		  deltaMosaic = -1;
	

	// Initialize;
	SoSpriteManagerInitialize();
	SoSpriteMemManagerInitialize();

	// Set the palettes;
	SoPaletteSetPalette( SO_SCREEN_PALETTE, televisionPalette, true );
	SoPaletteSetPalette( SO_SPRITE_PALETTE, fighterPalette, true );

	// Set the 255th color of the screen palette to a funky color;
	SoPaletteSetColor( SO_SCREEN_PALETTE, 255, SO_YELLOW );
	
	// Go to mode 4;
	SoMode4RendererEnable();

	// We are now in bitmapped mode;
	SoSpriteMemManagerSetInBitmappedMode( true );

	// Request the sprite;
	sprite = SoSpriteManagerRequestSprite();

	// Load the animation, remembering the index, and initializing the 
	// sprite with it;
	animBaseIndex = animCurrentIndex = SoSpriteMemManagerLoad( &fighterSpriteAnimation );
	SoSpriteSetAnimationIndex( sprite,  animBaseIndex );

	// Copy the animation properties;
	SoSpriteCopyPropertiesFromAnimation( sprite, &fighterSpriteAnimation );
	
	// Initialize the sprite position;
	SoSpriteSetTranslate( sprite, SO_FIXED_TO_WHOLE( spritePos.m_X ), SO_FIXED_TO_WHOLE( spritePos.m_Y ) );	

	// Set the scale and rotation settings this sprite should use.
	SoSpriteSetRotationAndScaleIndex( sprite, 0 );
	SoSpriteSetRotationAndScaleEnable( sprite, true );

	// Enable sprite rendering;
	SoSpriteManagerEnableSprites();	

	// Test a window;
	SoWindow0SetPosition( 10, 10, 185, 137 );
	SoWindow0BG2InsideEnable(  true  );
	SoWindow0BG2OutsideEnable( true  );
	SoWindow0OBJInsideEnable(  true  );
	SoWindow0OBJOutsideEnable( false );
	SoWindowSetWindow0Enable(  true  );

	// Enable mosaic rendering;
	SoBkgSetMosaicEnable(2, true);
	SoSpriteSetMosaicEnable( sprite, true );

	// Do the effect until the mosaic becomes too big;
	while ( mosaic < 16 )
	{	
		// Wait for VBlank;
		SoDisplayWaitForVBlankStart();

		// Update the sprites;
		SoSpriteManagerUpdate();

		// Update the keys;
		SoKeysUpdate();

		// Handle keypresses
		if ( SoKeysDown( SO_KEY_LEFT ) )  if ( spritePos.m_X > -(s32)SoSpriteGetWidth( sprite )) spritePos.m_X -= 1;
		if ( SoKeysDown( SO_KEY_RIGHT ) ) if ( spritePos.m_X < SO_SCREEN_WIDTH )			   spritePos.m_X += 1;
		if ( SoKeysDown( SO_KEY_UP ) )	  if ( spritePos.m_Y > -(s32)SoSpriteGetHeight( sprite )) spritePos.m_Y -= 1;		
		if ( SoKeysDown( SO_KEY_DOWN ) )  if ( spritePos.m_Y < SO_SCREEN_HEIGHT ) 		   spritePos.m_Y += 1;
		if ( SoKeysDown( SO_KEY_L ) )	  spriteAngle--;
		if ( SoKeysDown( SO_KEY_R ) )	  spriteAngle++;

		// If the start key is pressed, then we start the fade out;
		if ( SoKeysPressed( SO_KEY_START ) )
		{
			mosaic		= 0;
			deltaMosaic = 1;
		}

		// Update the mosaic;
		if ( mosaic > -1 )
		{
			// Set the mosaic value;
			SoBkgManagerSetMosaic((u32)mosaic, (u32)mosaic);
			SoSpriteManagerSetMosaic(	  (u32) mosaic, (u32) mosaic );

			// Update the mosaic value;
			mosaic += deltaMosaic;
		}

		// Update the sprite frame;
		// TODO: create a better animation system;
		spriteFrame++;
		if ( spriteFrame == SoSpriteAnimationGetNumFrames( &fighterSpriteAnimation ) ) 
		{
			spriteFrame = 0;
			animCurrentIndex = animBaseIndex;
		}
		else
		{
			animCurrentIndex += SoSpriteAnimationGetNumIndicesPerFrame( &fighterSpriteAnimation );
		}
		SoSpriteSetAnimationIndex( sprite, animCurrentIndex );
			
		// Set the sprite translate;
		SoSpriteSetTranslate( sprite, spritePos.m_X, spritePos.m_Y );	

		// Set the sprite scale and rotation setting;
		SoSpriteManagerSetRotationAndScale( 0, spriteAngle, SO_FIXED_FROM_WHOLE( 1 ), SO_FIXED_FROM_WHOLE( 1 ) );

		// Draw the image;
		SoMode4RendererDrawImage( &televisionImage );

		// Flip;
		SoMode4RendererFlip();			
	}

	// Clear the buffers;
	SoMode4RendererClearFrontAndBackBuffer();

	// Disable the sprite;
	SoSpriteDisable( sprite );

	// Release the tileset of the sprite;
	SoSpriteMemManagerRelease( SoSpriteGetAnimationIndex( sprite ) -
							   spriteFrame * SoSpriteAnimationGetNumIndicesPerFrame( &fighterSpriteAnimation ) );

	// Release the sprite;
	SoSpriteManagerRelease( sprite );

	// Disable sprites display;
	SoSpriteManagerDisableSprites();

	// If somebody else is going to bitmapped mode 
	// again, he has to say so himself;
	SoSpriteMemManagerSetInBitmappedMode( false );

	// Disable the window;
	SoWindowSetWindow0Enable( false );

	// Disable mosaic rendering;
	SoBkgSetMosaicEnable( 2, false );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
