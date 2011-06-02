// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMode4Renderer.c
	\author		Jaap Suter, Gabriele Scibilia, Mark T. Price
	\date		June 28, 2003
	\ingroup	SoMode4Renderer

	See the \a SoMode4Renderer module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoMode4Renderer.h"
#include "SoTables.h"
#include "SoMath.h"
#include "SoDisplay.h"
#include "SoFont.h"
#include "SoDMA.h"
#include "SoBkgManager.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

//! \internal Pointer to the first mode 4 buffer
#define SO_SCREEN_BUFFER_0			((u16*)0x6000000)

//! \internal Pointer to the second mode 4 buffer
#define SO_SCREEN_BUFFER_1			((u16*)0x600A000)

// ----------------------------------------------------------------------------
// Variables;
// ----------------------------------------------------------------------------

//!	\internal Pointer to the current mode 4 backbuffer.
//! Not static because we need it in an assembly file too.
u16* s_SoMode4RendererBackBuffer;

// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Switches the Gameboy Advance to mode 4.

	Use this method before you are going to use any other mode 4 rendering function.
	This also applies to all \a SoMode4PolygonRasterizer functions.

	Make sure you have already called \a SoDisplayInitialize too.
*/
// ----------------------------------------------------------------------------
void SoMode4RendererEnable( void )
{
	// Set the backbuffer;
	s_SoMode4RendererBackBuffer  = SO_SCREEN_BUFFER_1;

	// Set mode 4 with backbuffer bit set & LCD enabled
	SoDisplaySetMode(4);

	// Tell the rasterizers about the backbuffer;
	SoMode4PolygonRasterizerSetBuffer( s_SoMode4RendererBackBuffer );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Flips the two mode 4 buffers.

	If buffer 0 is visible then buffer 1 becomes visible, and vice versa.
*/
// ----------------------------------------------------------------------------
void SoMode4RendererFlip( void )
{	
	// Let the gameboy display the other buffer;
	SO_REG_DISP_CNT ^= SO_BIT_4;

	// Set the new backbuffer;
	if ( SO_REG_DISP_CNT & SO_BIT_4 )
	{
		// Set the new backbuffer;
		s_SoMode4RendererBackBuffer  = SO_SCREEN_BUFFER_0;
	}
	else
	{
		// Set the new backbuffer;
		s_SoMode4RendererBackBuffer  = SO_SCREEN_BUFFER_1;
	}

	// Tell the rasterizers about the new backbuffer;
	SoMode4PolygonRasterizerSetBuffer( s_SoMode4RendererBackBuffer );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns a pointer to the current backbuffer;
		
	Note that if you call \a SoMode4RendererFlip after using this function, the 
	pointer you have suddenly becomes the frontbuffer.
*/
// ----------------------------------------------------------------------------
void* SoMode4RendererGetBuffer( void )
{
	return (void*) s_SoMode4RendererBackBuffer;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Plots a single pixel into the backbuffer.

	\param a_X			X position of the pixel in the range 0 to 239.
	\param a_Y			Y position of the pixel in the ragen 0 to 159
	\param a_PalIndex	Index into the screenpalette, containing the requested color.

	This is a rather slow routine, because of the strange way we have to acces
	mode 4 pixels (two pixels at a time). Do not use this routine if you need speed.
*/
// ----------------------------------------------------------------------------
void SoMode4RendererDrawPixel( u32 a_X, u32 a_Y, u32 a_PalIndex )
{
	// Assert the input;
	SO_ASSERT( a_PalIndex < 256, "Invalid pallete index" );
	SO_ASSERT( a_X < SO_SCREEN_WIDTH, "Invalid X coordinate (offscreen)" );
	SO_ASSERT( a_Y < SO_SCREEN_HEIGHT, "Invalid Y coordinate (offscreen)" );

	// Get a pointer to the pixel duo;
	u16* pointerToPixels = s_SoMode4RendererBackBuffer + a_Y * SO_SCREEN_HALF_WIDTH + (a_X >> 1);
	
	// Get the pixels themselves;
	u16 pixels = *pointerToPixels;

	// Is x even or uneven;
	if ( SO_NUMBER_IS_EVEN( a_X ) )
	{
		// Mask out the first pixel;
		pixels &= 0xFF00;
		
		// Set it;
		pixels |= a_PalIndex;
	}
	else
	{
		// Mask out the second pixel;
		pixels &= 0x00FF;
		
		// Set it;
		pixels |= (a_PalIndex << 8);
	}

	// Place them back in memory;
	*pointerToPixels = pixels;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!	
	\brief Draws the given image in the current backbuffer.

	\param a_Image	An 240 by 160 palettized SoImage instance.
*/
// ----------------------------------------------------------------------------
void SoMode4RendererDrawImage( const SoImage* a_Image )
{
	// Assert the input image;
	SO_ASSERT( SoImageGetWidth( a_Image )  == SO_SCREEN_WIDTH,  "Image width must be 240." );
	SO_ASSERT( SoImageGetHeight( a_Image ) == SO_SCREEN_HEIGHT, "Image width must be 160." );
	SO_ASSERT( SoImageIsPalettized( a_Image )				  , "Image must be palettized." );

	// Display the image;
	SoDMA3Transfer( (void*) SoImageGetData( a_Image ),
					SoMode4RendererGetBuffer(),
					SO_SCREEN_HALF_WIDTH * SO_SCREEN_HEIGHT,
					SO_DMA_16 | SO_DMA_START_NOW );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Draws a transparent image into the current backbuffer.

	\param a_Image	An 240 by 160 palettized SoImage instance.

	\warning Only pixelduo's are considered to be transparent. This means that only
			 if a single u16 value (two pixels) equals zero it is not drawn. 
*/
// ----------------------------------------------------------------------------
void SoMode4RendererDrawTransparentImage( const SoImage* a_Image )
{
	// Assert the input image;
	SO_ASSERT( SoImageGetWidth( a_Image )  == SO_SCREEN_WIDTH,  "Image width must be 240." );
	SO_ASSERT( SoImageGetHeight( a_Image ) == SO_SCREEN_HEIGHT, "Image width must be 160." );
	SO_ASSERT( SoImageIsPalettized( a_Image )				  , "Image must be palettized." );

	// Get a pointer to the buffer;
	u16* buffer = s_SoMode4RendererBackBuffer;

	// Get a poniter to the image;
	u16* image  = SoImageGetData( a_Image );
	
	// Display the image;
	u32 i;
	for ( i = (SO_SCREEN_HALF_WIDTH*SO_SCREEN_HEIGHT); i != 0; i-- ) 
	{
		if ( *image != 0 )
		{
			*buffer++ = *image++;
		}
		else
		{
			image++;
			buffer++;
		}		
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Draws a crossfaded version of two images into the backbuffer.

	\param	a_Image0	An 240 by 160 palettized SoImage instance.
	\param	a_Image1	An 240 by 160 palettized SoImage instance.
	\param	a_Fade		Value you want to crossfade with. Anything in 
						between 0 and (\a SO_FADE_MAX-1).

	Draws the the faded version of two images. The pixel is drawn with the following formula.

	\code
		destPixel = srcPixel0 *      (a_Fade / SO_FADE_MAX) 
				  + srcPixel1 * (1 - (a_Fade / SO_FADE_MAX));
	\endcode
*/
// ----------------------------------------------------------------------------
void SoMode4RendererDrawCrossFadeImage( const SoImage* a_Image0, const SoImage* a_Image1, u32 a_Fade )
{
	// Variables;
	u32 paletteIndex0, paletteIndex1;
	u32 paletteIndices;

	// Get a pointer to the buffer;
	u16* buffer = s_SoMode4RendererBackBuffer;

	// Get pointers into the images;
	u16* image0 = SoImageGetData( a_Image0 );
	u16* image1 = SoImageGetData( a_Image1 );

	// Dummy counter;
	u32 i;

	// Get a pointer to the crossfade table lines;
	const u8* crossFade	 = &g_Fade[ a_Fade << 8 ];
	const u8* invCrossFade = &g_Fade[ (SO_FADE_MAX - a_Fade) << 8 ];
	
	// Clear the buffer;
	for ( i = (SO_SCREEN_HALF_WIDTH*SO_SCREEN_HEIGHT); i != 0; i-- ) 
	{
		// First check whether both pixels are the same;
		if ( (*image0) == (*image1) )
		{
			// Then just plot;
			*buffer = *image0;
		}
		else
		{
			// Do the first pixel;
			paletteIndex0 = (*image0) >> 8;
			paletteIndex1 = (*image1) >> 8;

			paletteIndices = crossFade[ paletteIndex0 ] + invCrossFade[ paletteIndex1 ];
			paletteIndices <<= 8;

			// Do the second pixel;
			paletteIndex0 = (*image0) & 0xFF;
			paletteIndex1 = (*image1) & 0xFF;

			paletteIndices |= crossFade[ paletteIndex0 ] + invCrossFade[ paletteIndex1 ];
		
			// Plot the two pixels;
			*buffer = paletteIndices;
		}

		// Advance the pointers;
		image0++;
		image1++;
		buffer++;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*! 
	\brief Draws a string with the given font into the current backbuffer.

	\param a_LeftX		Left  X of the rectangle where the string is drawn, due to 
						hardware reasons this must be an even number.
	\param a_RightX		Right X of the rectangle where the string is drawn. Must also
						be an even number.
	\param a_TopY		Top Y position of the rectangle where the string is drawn.
	\param a_String		String you want to draw.
	\param a_Font		Font you want to use for the string.
*/
// ----------------------------------------------------------------------------
void SoMode4RendererDrawString( u32 a_LeftX, u32 a_RightX, u32 a_TopY, char* a_String, const SoImage* a_Font )
{
	// Get the start X and Y positions;
	u32	x = a_LeftX;
	u32	y = a_TopY;

	// Dummy counters;
	u32 i, j;

	// Calculate the character width and height;
	u32 charWidth  = SoImageGetWidth(  a_Font ) / SO_FONT_NUM_CHARACTERS;
	u32 charHeight = SoImageGetHeight( a_Font );

	// Pixel pointers;
	u16*		charPixel;
	u16*		screenPixel;

	// Variable to store the number of characters in a single word;
	u32			numCharsInWord = 0;

	// Assert the image is palettized;
	SO_ASSERT( SoImageIsPalettized( a_Font ), "Font image must be palettized." );
	
	// Assert the starting X coordinate;
	SO_ASSERT( (a_LeftX  & 1) == 0, "Parameter a_LeftX must be an even number. " );

	// Assert the character width is an even number;
	SO_ASSERT( (charWidth & 1) == 0, "Character width must be an even number" );
	
	// Iterate over each character in the string;
	while ( *a_String != '\0' )
	{	
		// Is this a non-drawable character;
		if ( (*a_String < SO_FONT_FIRST_CHAR) || (*a_String > SO_FONT_LAST_CHAR) )
		{
			// Is this a newline;
			if ( *a_String == '\n' )
			{
				// Go to the next line;
				y += charHeight + SO_FONT_VERTICAL_SPACING;

				// Reset X;
				x = a_LeftX;
			}
			else
			{
				// Add a space;
				x += SO_FONT_HORIZONTAL_SPACING;
			}
		}
		else
		{
			// Is this the start of a word;
			if ( numCharsInWord == 0 )
			{
				// Calculate the number of characters in the word;
				for ( ; (*(a_String + numCharsInWord) >= SO_FONT_FIRST_CHAR) 
					 && (*(a_String + numCharsInWord) <= SO_FONT_LAST_CHAR); 
					numCharsInWord++ );

				// If the word doesn't fit on this line;
				if ( (x + (numCharsInWord*charWidth)) > a_RightX )
				{
					// Go to the next line;
					y += charHeight + SO_FONT_VERTICAL_SPACING;

					// Reset X;
					x = a_LeftX;
				}
			}

			// Get a pointer to the character, dividing the character width by two cause
			// it's an u16 pointer, and substracting the space-character from the string
			// character to get the index;
			charPixel = SoImageGetData( a_Font ) + (*a_String - SO_FONT_FIRST_CHAR) * (charWidth >> 1);

			// Get a pointer to the screen pixel;
			screenPixel = s_SoMode4RendererBackBuffer + SO_SCREEN_HALF_WIDTH * y + (x >> 1);

			// Plot the character;
			i = charHeight; while ( i-- )
			{
				// Plot a single pixel row;
				j = charWidth >> 1; while ( j-- )
				{				
					screenPixel[ j ] = charPixel[ j ];
				}
				
				// Next row;
				screenPixel += SO_SCREEN_HALF_WIDTH;
				charPixel   += SoImageGetWidth( a_Font ) >> 1;
			}			

			// Next screen pixel;
			screenPixel += -SO_SCREEN_HALF_WIDTH*charHeight + (charWidth>>1);

			// We did another character;
			--numCharsInWord; 
			x += charWidth;
			
		}

		// Next character;
		a_String++;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Draws a number with the given font in the current backbuffer;

	\param a_LeftX		Left  X of the rectangle where the string is drawn, due to 
						hardware reasons this must be an even number.
	\param a_RightX		Right X of the rectangle where the string is drawn. Must also
						be an even number.
	\param a_TopY		Top Y position of the rectangle where the string is drawn.
	\param a_Number		Number you want to draw.
	\param a_Font		Font you want to use for the string.
*/
// ----------------------------------------------------------------------------
void SoMode4RendererDrawNumber( u32 a_LeftX, u32 a_RightX, u32 a_TopY, s32   a_Number, const SoImage* a_Font )
{
	// String big enough to handle any number plus a minus sign;
	char number[ 16 ];
	char *sz = ditoa(a_Number, &number[0], 16, 10);	// conver the number to a string

	// Draw the string;
	SoMode4RendererDrawString( a_LeftX, a_RightX, a_TopY, sz, a_Font );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief This function clears both the front and the back buffer
*/
// ----------------------------------------------------------------------------
void SoMode4RendererClearFrontAndBackBuffer( void )
{
	SoMode4RendererClear();
	SoMode4RendererFlip();
	SoMode4RendererClear();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
