// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSample.c
	\author		Jaap Suter, Mark T. Price
	\date		April 3, 2003
	\ingroup	SoSample

	See the \a SoSample module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoSample SoSample
	\brief	  Module containing some demonstrations of what Socrates can do.
		
*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Socrates includes;
#include "Socrates.h"

// Sample application includes;
#include "SampleEffects.h"

// ----------------------------------------------------------------------------
/*! 
	\brief Comment this away if you are targeting ROM only.
	
	This global variable tells Jeff's linkerscript that you're linking for a multiboot 
	program. This global is never used in any way, so it's value doens't matter, but it's 
	there to avoid some compiler warnings. 

	Note that even if you do compile for multiboot (not commented away), it will still 
	run from ROM too.
*/	
// ----------------------------------------------------------------------------
//int __gba_multiboot = 1234567890;

// ----------------------------------------------------------------------------
/*!
	\brief	Application entry point
	
	\return	I have no idea what this return value can be used for, and it doesn't matter 
			anyway. I return 0, but this routine is never left anyway. Unless you switch
			the GBA off ofcourse :) hehe.
*/
// ----------------------------------------------------------------------------
int AgbMain( void )
{
	// Initialize some Socrates modules;
	SoDisplayInitialize();
	SoIntManagerInitialize();
	SoSpriteManagerInitialize();
	SoSpriteMemManagerInitialize();
	SoMode4PolygonRasterizerInitialize();	

	// Seed the random number generator, with just a random number :).
	SoMathRandSeed( 34329046 );

	// Enable the clock on timer two;
	SoTimerClockEnable( 2 );

	// Go to the main infinite loop;
	for ( ;; )
	{	
		// Do the effects in order;
		SoSampleEffectWelcome();		
		SoSampleEffectTiles();
		SoSampleEffectMap();
		SoSampleEffectSprites();
		SoSampleEffectMultiPlayer();
		SoSampleEffectTriangle();
		SoSampleEffect3DScene();
		SoSampleEffectCredits();		
	}

	// Return zero for release builds;
	return 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
