// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoPalette.h
	\author		Jaap Suter
	\date		August 9 2001
	\ingroup	SoPalette

	See the \a SoPalette module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_PALETTE_H
#define SO_PALETTE_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoPalette SoPalette
	\brief	  All palette handling

	This module takes care of everything that has to do with palettes.	

	A palette is just an array of 15 bit BGR color values, each in a 16 bit
	word.  So we're just using \a u16*'s for palettes.
	
	A palette contains either 16 or 256 colors. This information is not stored
	inside the palette. Most of the time you know it when using it anyway. 
	
	Because a palette is already a pointer in itself, we can directly use the
	\a SoPalette object as a this pointer.
	
	You can use the SoPaletteConverter.exe tool in the /tools/ directory to
	convert Jasc palettes to a Socrates compatible header file. Ready to be
	included in a socrates project.	
	
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------

// Start of sprite palette;
#define SO_SPRITE_PALETTE		((u16*)0x5000200)		//!< Sprite palette

// Start of screen palette;			
#define SO_SCREEN_PALETTE		((u16*)0x5000000)		//!< Screen palette
		
// For use with 16-color Sprite palettes
// These defines point to each of the 16 sprite palettes available when using 16-color palettes
// Can be used as follows: SoPaletteSetColor(SO_SPRITE_PALETTE_3, &myPalette, true);
// to set a 16-color palette defined in myPalette to slot 3 (fourth slot)
#define SO_SPRITE_PALETTE_0		((u16*)0x5000200)		//!< Sprite palette 0 (16 color mode)
#define SO_SPRITE_PALETTE_1		((u16*)0x5000220)		//!< Sprite palette 1 (16 color mode)
#define SO_SPRITE_PALETTE_2		((u16*)0x5000240)		//!< Sprite palette 2 (16 color mode)
#define SO_SPRITE_PALETTE_3		((u16*)0x5000260)		//!< Sprite palette 3 (16 color mode)
#define SO_SPRITE_PALETTE_4		((u16*)0x5000280)		//!< Sprite palette 4 (16 color mode)
#define SO_SPRITE_PALETTE_5		((u16*)0x50002A0)		//!< Sprite palette 5 (16 color mode)
#define SO_SPRITE_PALETTE_6		((u16*)0x50002C0)		//!< Sprite palette 6 (16 color mode)
#define SO_SPRITE_PALETTE_7		((u16*)0x50002E0)		//!< Sprite palette 7 (16 color mode)
#define SO_SPRITE_PALETTE_8		((u16*)0x5000300)		//!< Sprite palette 8 (16 color mode)
#define SO_SPRITE_PALETTE_9		((u16*)0x5000320)		//!< Sprite palette 9 (16 color mode)
#define SO_SPRITE_PALETTE_10	((u16*)0x5000340)		//!< Sprite palette 10 (16 color mode)
#define SO_SPRITE_PALETTE_11	((u16*)0x5000360)		//!< Sprite palette 11 (16 color mode)
#define SO_SPRITE_PALETTE_12	((u16*)0x5000380)		//!< Sprite palette 12 (16 color mode)
#define SO_SPRITE_PALETTE_13	((u16*)0x50003A0)		//!< Sprite palette 13 (16 color mode)
#define SO_SPRITE_PALETTE_14	((u16*)0x50003C0)		//!< Sprite palette 14 (16 color mode)
#define SO_SPRITE_PALETTE_15	((u16*)0x50003E0)		//!< Sprite palette 15 (16 color mode)
#define SO_SPRITE_PALETTE_GET(_n_) SO_SPRITE_PALETTE_##_n_	//!< Select the sprite palette n (16 color mode)

// For use with 16-color Background palettes
// These defines point to each of the 16 background palettes available when using 16-color palettes
// Can be used as follows: SoPaletteSetColor(SO_BKG_PALETTE_3, &myBkgPalette, true);
// to set a 16-color palette defined in myBkgPalette to slot 3 (fourth slot)
#define SO_BKG_PALETTE			((u16*)0x5000000)		//!< Background palette (same as screen palette)
#define SO_BKG_PALETTE_0		((u16*)0x5000000)		//!< Background palette 0 (16 color mode)
#define SO_BKG_PALETTE_1		((u16*)0x5000020)		//!< Background palette 1 (16 color mode)
#define SO_BKG_PALETTE_2		((u16*)0x5000040)		//!< Background palette 2 (16 color mode)
#define SO_BKG_PALETTE_3		((u16*)0x5000060)		//!< Background palette 3 (16 color mode)
#define SO_BKG_PALETTE_4		((u16*)0x5000080)		//!< Background palette 4 (16 color mode)
#define SO_BKG_PALETTE_5		((u16*)0x50000A0)		//!< Background palette 5 (16 color mode)
#define SO_BKG_PALETTE_6		((u16*)0x50000C0)		//!< Background palette 6 (16 color mode)
#define SO_BKG_PALETTE_7		((u16*)0x50000E0)		//!< Background palette 7 (16 color mode)
#define SO_BKG_PALETTE_8		((u16*)0x5000100)		//!< Background palette 8 (16 color mode)
#define SO_BKG_PALETTE_9		((u16*)0x5000120)		//!< Background palette 9 (16 color mode)
#define SO_BKG_PALETTE_10		((u16*)0x5000140)		//!< Background palette 10 (16 color mode)
#define SO_BKG_PALETTE_11		((u16*)0x5000160)		//!< Background palette 11 (16 color mode)
#define SO_BKG_PALETTE_12		((u16*)0x5000180)		//!< Background palette 12 (16 color mode)
#define SO_BKG_PALETTE_13		((u16*)0x50001A0)		//!< Background palette 13 (16 color mode)
#define SO_BKG_PALETTE_14		((u16*)0x50001C0)		//!< Background palette 14 (16 color mode)
#define SO_BKG_PALETTE_15		((u16*)0x50001E0)		//!< Background palette 15 (16 color mode)
#define SO_BKG_PALETTE_GET(_n_)		SO_BKG_PALETTE_##_n_		//!< Select the background palette n (16 color mode)
		
#define SoColor(r,g,b)	((r)+(g<<5)+(b<<10))            //!< construct 15-bit BGR color value

//you can have 0-31 levels of each red,green,blue component.
#define SoColorGetRed(rgb)    (rgb &0x1f)               //!< Extract 5-bit red component of color value
#define SoColorGetGreen(rgb)  ((rgb>>5) & 0x1f)         //!< Extract 5-bit green component of color value
#define SoColorGetBlue(rgb)	  ((rgb>>10) & 0x1f)        //!< Extract 5-bit blue component of color value


// --------------------------------------------------------------------------
// Public methods;
// --------------------------------------------------------------------------

void SoPaletteSetColor( u16* a_This, u32 a_PalIndex, u32 a_16BitColor );

void SoPaletteSetBlack(		u16* a_This, bool a_256ColorPalette );
void SoPaletteSetGreyScale(	u16* a_This, bool a_256ColorPalette );
void SoPaletteSetRandom(	u16* a_This, bool a_256ColorPalette );
void SoPaletteSetPalette(	u16* a_This, const u16* a_Palette, bool a_256ColorPalette );

void SoPaletteFadeGreyScale( u16* a_This, u32 a_Fade, bool a_256ColorPalette );
void SoPaletteFadePalette(	 u16* a_This, const u16* a_Palette, u32 a_Fade, bool a_256ColorPalette );

void SoPaletteAddToPalette(			u16* a_This, u32 a_Red, u32 a_Green, u32 a_Blue, bool a_256ColorPalette );
void SoPaletteSubtractFromPalette(  u16* a_This, u32 a_Red, u32 a_Green, u32 a_Blue, bool a_256ColorPalette );

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
