// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSampleEffect3DScene.c
	\author		Jaap Suter
	\date		Mar 3 2002
	\ingroup	SoSampleEffects

	See the \a SoSampleEffects module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Sample application includes;
#include "SampleEffects.h"


// ----------------------------------------------------------------------------
/*!
*/
// ----------------------------------------------------------------------------
void SoSampleEffect3DScene( void )
{
	// Constants;
	const u32 SO_NUM_CUBES = 24;

	// Variables;
	u32 i, j;
	SoMesh cube[ SO_NUM_CUBES ];
	SoCamera camera;

	// Go to mode 4;
	SoMode4RendererEnable();

	// Initialize the mode 4 renderer;
	SoMode4PolygonRasterizerInitialize();

	// Clear the screen;
	SoMode4RendererClear();

	// Set the palette to some random value;
	SoPaletteSetRandom( SO_SCREEN_PALETTE, true );

	// Initalize the camera;
	SoCameraInitialize( &camera );

	// Place the camera back somewhat;
	SoCameraForward( &camera, SO_FIXED_FROM_WHOLE( -256 ) );

	// Initialize the cubes;
	for ( j = 0; j < SO_NUM_CUBES; j++ )
	{
		// Make it a default cube;
		SoMeshMakeDefaultCube( cube + j );	

		// It has no texture;
		SoMeshSetTexture( cube + j, NULL );

		// Give the cube's polygons random colors;
		for ( i = 0; i < SoMeshGetNumPolygons( cube + j ); i++ )
		{
			SoPolygonSetPaletteIndex( SoMeshGetPolygon( cube + j, i ), (SoMathRand() & SO_6_BITS) + 127 );
		}

		// Give the cube a pseudo random position;
		SoTransformSetTranslation( SoMeshGetTransform( cube + j ), SO_FIXED_FROM_WHOLE( 128 - (SoMathRand() >> 24) ),
																   SO_FIXED_FROM_WHOLE( 128 - (SoMathRand() >> 24) ),
																   SO_FIXED_FROM_WHOLE(  64 - (SoMathRand() >> 26) ) );

	}

	// Loop until we press the start key;
	do 
	{
		// Update the keys;
		SoKeysUpdate();

		// Clear the screen;
		SoMode4RendererClear();	

		// Rotate and draw the cubes
		for ( i = 0; i < SO_NUM_CUBES; i++ )
		{
			SoTransformRotateX( SoMeshGetTransform( cube + i), SoMathRand() >> 29 );
			SoTransformRotateY( SoMeshGetTransform( cube + i), SoMathRand() >> 29 );
			SoTransformRotateZ( SoMeshGetTransform( cube + i), SoMathRand() >> 29 );

			// Draw the cube;
			SoCameraDrawMesh( &camera, cube + i );
		}
		
		// Control the camera;
		if ( SoKeysDown( SO_KEY_RIGHT ) ) SoCameraRight( &camera, SO_FIXED_FROM_WHOLE( 8 ) );
		if ( SoKeysDown( SO_KEY_LEFT  ) ) SoCameraRight( &camera, -SO_FIXED_FROM_WHOLE( 8 ) );
		if ( SoKeysDown( SO_KEY_UP    ) ) SoCameraUp( &camera, SO_FIXED_FROM_WHOLE( 8 ) );
		if ( SoKeysDown( SO_KEY_DOWN  ) ) SoCameraUp( &camera, -SO_FIXED_FROM_WHOLE( 8 ) );
		if ( SoKeysDown( SO_KEY_A     ) ) SoCameraForward( &camera,  SO_FIXED_FROM_WHOLE( 10 ) );
		if ( SoKeysDown( SO_KEY_B     ) ) SoCameraForward( &camera, -SO_FIXED_FROM_WHOLE( 10 ) );
		
		// Wait for a vblank;
		SoDisplayWaitForVBlankStart();

		// Flip the screen;
		SoMode4RendererFlip();

	} while ( ! SoKeysPressed( SO_KEY_START ) );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
