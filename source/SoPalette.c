// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoPalette.c
	\author		Jaap Suter, Gabriele Scibilia
	\date		August 9 2001
	\ingroup	SoPalette

	See the \a SoPalette module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------
#include "SoPalette.h"
#include "SoTables.h"
#include "SoDisplay.h"
#include "SoSystem.h"
#include "SoMath.h"
#include "SoDMA.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets an individual color into the given palette
	
	\param a_This		Palette you want to set the color for;
	\param a_PalIndex	Index into the palette.
	\param a_16BitColor	New color in 15 bit BGR format, for the given index;
*/
// ----------------------------------------------------------------------------
void SoPaletteSetColor( u16* a_This, u32 a_PalIndex, u32 a_16BitColor )
{	
	a_This[ a_PalIndex ] = a_16BitColor;	
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Makes the entire palette fully black (every color equals zero).
	
	\param a_This		Palette you want to blacken.
	\param a_256ColorPalette	True if there are 256 colors in the given palette, false 
								if there are 16.
*/
// ----------------------------------------------------------------------------
void SoPaletteSetBlack( u16* a_This, bool a_256ColorPalette )
{
	// Dummy counter;
	u32 i;

	// Black pixel;
	u16 blackPixel = SO_BLACK;

	// What kind of palette is this;
	if ( a_256ColorPalette ) i = 256; else i = 16;

	// Set the palette;
	SoDMA3Transfer( &blackPixel, a_This, i, SO_DMA_16 | SO_DMA_START_NOW | SO_DMA_SOURCE_FIX );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Greyscales the palette. 
	
	\param a_This				Makes a greyscale palette of the given palette;
	\param a_256ColorPalette	True if there are 256 colors in the given palette, false 
								if there are 16.
*/
// ----------------------------------------------------------------------------
void SoPaletteSetGreyScale(	u16* a_This, bool a_256ColorPalette )
{
	// Dummy counter;
	u32 i;

	// Greyscale value;
	u32 greyScale;

	// 256 color or 16 bit color palette ;
	if ( a_256ColorPalette )
	{
		for ( i = 0; i < 256; i++ )
		{
			// Calculate the greyscale value;
			greyScale = i >> 3;

			// Set the palette color and advance 
			// the palette pointer;
			*a_This++ = SO_RGB( greyScale, greyScale, greyScale );	
		}
	}
	else
	{
		for ( i = 0; i < 16; i++ )
		{
			// Calculate the greyscale value. We bitwise-or the value with i to
			// get the last bit of the value okay. Now it's one on fully white
			// (index = 15), and zero on fully black (index = 0);
			greyScale = (i << 1) | (i);

			// Set the palette color and advance 
			// the palette pointer;
			*a_This++ = SO_RGB( greyScale, greyScale, greyScale );	
		}
	}	
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Copies an entire palette into another one.
	
	\param a_This				Palette you want to set
	\param a_Palette			Palette that is the source
	\param a_256ColorPalette	True if there are 256 colors in the given palettes, false 
								if there are 16.
*/
// ----------------------------------------------------------------------------
void SoPaletteSetPalette( u16* a_This, const u16* a_Palette, bool a_256ColorPalette )
{
	// Dummy counter;
	u32 i;

	// What kind of palette is this;
	if ( a_256ColorPalette ) i = 256; else i = 16;

	// Set the palette;
	SoDMA3Transfer( a_Palette, a_This, i, SO_DMA_16 | SO_DMA_START_NOW );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the pallette a faded greyscale palette;
	
	\param	a_This				Palette you want to fade;
	\param	a_Fade				Value you want to fade with. Anything in 
								between 0 and (\a SO_FADE_MAX-1).
	\param	a_256ColorPalette	True if there are 256 colors in the given palettes, false 
								if there are 16.


	If the fade value equals zero then the entire palette will become black, if
	the fade value equals (\a SO_FADE_MAX - 1) then this function does the same
	as \a SoPaletteSetGreyScale, namely just making it a greyscale palette. If
	the fade value is something in between, the greyscale values are scaled
	accordingly.
*/
// ----------------------------------------------------------------------------
void SoPaletteFadeGreyScale( u16* a_This, u32 a_Fade, bool a_256ColorPalette )
{
	// Dummy counter;
	u32 i;

	// Get a pointer to correct line in the crossfade table;
	const u8* fade = &g_Fade[ a_Fade << 8 ];

	// Variable to hold a greyscale value;
	u32 greyScale;

	// Assert that the fade value is in the correct range;
	SO_ASSERT( a_Fade < (SO_FADE_MAX-1), "Fade value out of allowed range." );

	// Do 256 color palettes;
	if ( a_256ColorPalette )
	{
		for ( i = 0; i < 256; i++ )
		{
			// Get the greyscale value;
			greyScale = *fade++ >> 3;

			// Set and advance;
			*a_This++ = SO_RGB( greyScale, greyScale, greyScale );
		}
	}
	else
	{
		for ( i = 0; i < 16; i++ )
		{
			// Get the greyscale value;
			greyScale = *fade >> 3;

			// Set and advance;
			*a_This++ = SO_RGB( greyScale, greyScale, greyScale );

			// Advance the fade point;
			fade += 16;
		}
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the pallette a faded version of another palette.
	
	\param	a_This				Palette you want to fade
	\param  a_Palette			Source palette
	\param	a_Fade				Value you want to fade with. Anything in 
								between 0 and (\a SO_FADE_MAX-1).
	\param	a_256ColorPalette	True if there are 256 colors in the given palettes, false 
								if there are 16.

	If the fade value equals zero then the entire palette will become black, if
	the fade value equals (\a SO_FADE_MAX - 1) then this function does the same
	as \a SoPaletteSetPalette, namely just making it the same as the source
	palette. If the fade value is something in between, the color values are
	scaled accordingly.
*/
// ----------------------------------------------------------------------------
void SoPaletteFadePalette( u16* a_This, const u16* a_Palette, 
						   u32 a_Fade, bool a_256ColorPalette )
{
	// Dummy counter;
	u32 i;

	// Intermediate color-component variables;
	u32 r, g, b;

	// Pointer to start of source palette;
	const register u16* pal = a_Palette;

	// Get a pointer to correct line in the crossfade table;
	const u8* fade = &g_Fade[ a_Fade << 8 ];

	// Assert the fade value is in the correct range;
	SO_ASSERT( a_Fade < (SO_FADE_MAX-1), "Fade value out of range." );

	// What kind of palette is it;
	if ( a_256ColorPalette ) i = 256; else i = 16;

	// Iterate over the palette;
	for ( ; i != 0; i-- )
	{
		// Get each color component out of the current color;
		r = SO_RGB_GET_R( *pal );
		g = SO_RGB_GET_G( *pal );
		b = SO_RGB_GET_B( *pal );

		// Fade the values;
		r = fade[ r ];
		g = fade[ g ];
		b = fade[ b ];

		// Set the new color, increasing the palette
		// pointer to the next color afterwards;
		*a_This++ = SO_RGB( r, g, b );

		// Go to the next color in the given palette;
		pal++;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Saturated addition to each color of the given palette.
	
	\param	a_This				Palette you want to add values to.
	\param	a_Red				Value you want to add to red (0 to 31).
	\param	a_Green				Value you want to add to green (0 to 31).
	\param	a_Blue				Value you want to add to blue (0 to 31).
	\param	a_256ColorPalette	True if there are 256 colors in the given palettes, false 
								if there are 16.

	Does a saturated addition to each color in the palette. This means that if a 
	color component has reached its maximum value (31), it no longer increases.
*/
// ----------------------------------------------------------------------------
void SoPaletteAddToPalette( u16* a_This, u32 a_Red, u32 a_Green, 
						    u32 a_Blue, bool a_256ColorPalette )
{
	// Dummy counter;
	u32 i;

	// Intermediate color-component variables;
	u32 r, g, b;

	// What kind of palette is this;
	if ( a_256ColorPalette ) i = 256; else i = 16;

	// Iterate over the palette;
	for ( ; i != 0; i-- )
	{
		// Get each color component out of the current color;
		r = SO_RGB_GET_R( *a_This );
		g = SO_RGB_GET_G( *a_This );
		b = SO_RGB_GET_B( *a_This );

		// Add the values;
		if ( r < (31-a_Red)   ) r += a_Red;   else r = 31;
		if ( g < (31-a_Green) ) g += a_Green; else g = 31;
		if ( b < (31-a_Blue)  ) b += a_Blue;  else b = 31;

		// Set the new color, increasing the palette
		// pointer to the next color afterwards;
		*a_This++ = SO_RGB( r, g, b );
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Saturated subtraction from each color of the given palette.
	
	\param	a_This				Palette you want to subtract values from.
	\param	a_Red				Value you want to subtract from red (0 to 31).
	\param	a_Green				Value you want to subtract from green (0 to 31).
	\param	a_Blue				Value you want to subtract from blue (0 to 31).
	\param	a_256ColorPalette	True if there are 256 colors in the given palettes, false 
								if there are 16.

	Does a saturated subtraction from each color in the palette. This means
	that if a color component has reached its minimum value (31), it no longer
	decreases.
*/
// ----------------------------------------------------------------------------
void SoPaletteSubtractFromPalette( u16* a_This, u32 a_Red, u32 a_Green, 
									u32 a_Blue, bool a_256ColorPalette )
{
	// Dummy counter;
	u32 i;

	// Intermediate color-component variables;
	u32 r, g, b;

	// What kind of palette is this;
	if ( a_256ColorPalette ) i = 256; else i = 16;

	// Iterate over the palette;
	for ( ; i != 0; i-- )
	{
		// Get each color component out of the current color;
		r = SO_RGB_GET_R( *a_This );
		g = SO_RGB_GET_G( *a_This );
		b = SO_RGB_GET_B( *a_This );

		// Substract the values;
		if ( r > a_Red )   r -= a_Red;   else r = 0;
		if ( g > a_Green ) g -= a_Green; else g = 0;
		if ( b > a_Blue )  b -= a_Blue;  else b = 0;

		// Set the new color, increasing the palette
		// pointer to the next color afterwards;
		*a_This++ = SO_RGB( r, g, b );
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Just throws random colors in the palette;

	\param	a_This				Palette you want to subtract values from.
	\param	a_256ColorPalette	True if there are 256 colors in the given palettes, false 
								if there are 16.
	
	Keeps black as the zero color, and white as the 255th or 16th color though.
	Also, tries to avoid darkish colors. So it's not truly random... :). But it
	suits my purposes.
*/
// ----------------------------------------------------------------------------
void SoPaletteSetRandom( u16* a_This, bool a_256ColorPalette )
{
	// Variables;
	u32 i, r, g, b;
	
	// What kind of palette is this;
	if ( a_256ColorPalette ) i = 255; else i = 15;
	
	// Set black;
	*a_This++ = SO_BLACK;

	// Iterate over the palette;
	for ( ; i != 1; i-- )
	{
		// Get each color component out of the current color;
		r = ((SoMathRand() & SO_7_BITS) << 1);
		g = ((SoMathRand() & SO_7_BITS) << 1);
		b = ((SoMathRand() & SO_7_BITS) << 1);

		// Set the new color, increasing the palette
		// pointer to the next color afterwards;
		*a_This++ = SO_RGB( r, g, b );
	}
	
	// Set white;
	*a_This = SO_WHITE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
