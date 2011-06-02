// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSampleEffectTriangle.c
	\author		Jaap Suter
	\date		Apr 1 2002
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
void SoSampleEffectTriangle( void )
{
	// Variables;
	u32 i;							//!< Dummy counter;
	u32 angle = 0;					//!< Angle of each triangle;
	SoVector2 triangle[ 2 ][ 3 ];	//!< Two triangles;
	
	SoMode4RendererEnable();

	SoMode4PolygonRasterizerInitialize();

	SoPaletteSetRandom( SO_SCREEN_PALETTE, true );
	
	// Loop until we press the start key;
	do 
	{
		SoKeysUpdate();

		SoMode4RendererClear();	

		// Set the two triangles;
		triangle[ 0 ][ 0 ].m_X = SO_FIXED_FROM_WHOLE( 120 ) + (SO_COSINE( angle	      ) << 5);
		triangle[ 0 ][ 0 ].m_Y = SO_FIXED_FROM_WHOLE(  80 ) + (SO_SINE(   angle	      ) << 5);
		triangle[ 0 ][ 1 ].m_X = SO_FIXED_FROM_WHOLE( 120 ) + (SO_COSINE( angle +  64 ) << 5);
		triangle[ 0 ][ 1 ].m_Y = SO_FIXED_FROM_WHOLE(  80 ) + (SO_SINE(   angle +  64 ) << 5);
		triangle[ 0 ][ 2 ].m_X = SO_FIXED_FROM_WHOLE( 120 ) + (SO_COSINE( angle + 128 ) << 5);
		triangle[ 0 ][ 2 ].m_Y = SO_FIXED_FROM_WHOLE(  80 ) + (SO_SINE(   angle + 128 ) << 5);
		
		triangle[ 1 ][ 0 ].m_X = SO_FIXED_FROM_WHOLE( 120 ) + (SO_COSINE( angle	+ 196 ) << 5);
		triangle[ 1 ][ 0 ].m_Y = SO_FIXED_FROM_WHOLE(  80 ) + (SO_SINE(   angle	+ 196 ) << 5);
		triangle[ 1 ][ 1 ].m_X = SO_FIXED_FROM_WHOLE( 120 ) + (SO_COSINE( angle + 256 ) << 5);
		triangle[ 1 ][ 1 ].m_Y = SO_FIXED_FROM_WHOLE(  80 ) + (SO_SINE(   angle + 256 ) << 5);
		triangle[ 1 ][ 2 ].m_X = SO_FIXED_FROM_WHOLE( 120 ) + (SO_COSINE( angle + 128 ) << 5);
		triangle[ 1 ][ 2 ].m_Y = SO_FIXED_FROM_WHOLE(  80 ) + (SO_SINE(   angle + 128 ) << 5);
		
		// DEBUG TEST;
		/*
		triangle[ 1 ][ 0 ].m_X = SO_FIXED_FROM_WHOLE(  64 );// + (SO_FIXED_FROM_WHOLE( 1 ) >> 1);
		triangle[ 1 ][ 0 ].m_Y = SO_FIXED_FROM_WHOLE(   0 );// + (SO_FIXED_FROM_WHOLE( 1 ) >> 1);
		triangle[ 1 ][ 1 ].m_X = SO_FIXED_FROM_WHOLE(   0 );// + (SO_FIXED_FROM_WHOLE( 1 ) >> 1);
		triangle[ 1 ][ 1 ].m_Y = SO_FIXED_FROM_WHOLE(   0 );// + (SO_FIXED_FROM_WHOLE( 1 ) >> 1);
		triangle[ 1 ][ 2 ].m_X = SO_FIXED_FROM_WHOLE(  64 );// + (SO_FIXED_FROM_WHOLE( 1 ) >> 1);
		triangle[ 1 ][ 2 ].m_Y = SO_FIXED_FROM_WHOLE(  64 );// + (SO_FIXED_FROM_WHOLE( 1 ) >> 1);		
		*/
							
		for ( i = 50; i--; ) SoMode4PolygonRasterizerDrawSolidTriangle( triangle[ 0 ], 150 );
		for ( i = 50; i--; ) SoMode4PolygonRasterizerDrawSolidTriangle( triangle[ 1 ], 255 );

		SoDisplayWaitForVBlankStart();
		
		SoMode4RendererFlip();

		angle++;
		
	} while ( ! SoKeysPressed( SO_KEY_START ) );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
