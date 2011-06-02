// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkg.h
	\author		Mark Price, Jaap Suter
	\date		April 3, 2003
	\ingroup	SoBkg

	See the \a SoBkg module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_BKG_H
#define SO_BKG_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoBkg SoBkg
	\brief	  Background specific functions

	Singleton

	This file handles everthing that has to do with a specific background
	instance. This module works together with the \a SoBkgManager,
	\a SoBkgMemManager and \a SoBkgMap modules.

	\todo Add support for screen sizes other than 256x256 pixels

	\todo Change to not do demand load for tiles that are not visible due to
	      windowing.

	\todo Add support for mode 1, 2 screens

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoBkgMap.h"
#include "SoBkgFont.h"
#include "SoVector.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! The number of valid backgrounds
#define SO_BKG_MAX_NUM_BACKGROUNDS  4

//! Invalid background index used in SoBkgMap to disassociate from any backgrounds
#define SO_BKG_NONE 0xffff

// Background option bits
#define SO_BKG_PRIORITY_0         0x0000		//!< screen priority 0 (highest)
#define SO_BKG_PRIORITY_1         0x0001		//!< screen priority 1
#define SO_BKG_PRIORITY_2         0x0002		//!< screen priority 2
#define SO_BKG_PRIORITY_3         0x0003		//!< screen priority 3 (lowest)

#define SO_BKG_CHARBASE(n)        (((n)&0x3)<<2)	//!< starting tile offset in VRAM (n*16384)

#define SO_BKG_MOSAIC_DISABLE     0x0000		//!< mosaic mode disabled
#define SO_BKG_MOSAIC_ENABLE      0x0040		//!< mosaic mode enabled

#define SO_BKG_CLRMODE_MASK       0x0080		//!< mode bits for the color mode
#define SO_BKG_CLRMODE_16         0x0000		//!< tile color mode (16 color)
#define SO_BKG_CLRMODE_256        0x0080		//!< tile color mode (256 color)

#define SO_BKG_SCRBASE_MASK       0x1f00		//!< mode bits for the screen base
#define SO_BKG_SCRBASE(n)         (((n)&0x1f)<<8)	//!< screen offset in VRAM (n*2048)

#define SO_BKG_SIZE_MASK          0xc000		//!< mode bits for the bkg size
#define SO_BKG_TEXTSIZE_256x256   0x0000		//!< text screen size 256x256 pixels (32x32 tiles)
#define SO_BKG_TEXTSIZE_512x256   0x4000		//!< text screen size 512x256 pixels (64x32 tiles)
#define SO_BKG_TEXTSIZE_256x512   0x8000		//!< text screen size 256x512 pixels (32x64 tiles)
#define SO_BKG_TEXTSIZE_512x512   0xc000		//!< text screen size 512x512 pixels (64x64 tiles)

#define SO_BKG_ROTSIZE_128x128    0x0000		//!< rot/scale screen size 128x128 pixels (16x16 tiles)
#define SO_BKG_ROTSIZE_256x256    0x4000		//!< rot/scale screen size 256x256 pixels (32x32 tiles)
#define SO_BKG_ROTSIZE_512x512    0x8000		//!< rot/scale screen size 512x512 pixels (64x64 tiles)
#define SO_BKG_ROTSIZE 1024x1024  0xc000		//!< rot/scale screen size 1024x1024 pixels (128x128 tiles)

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

void SoBkgSetup(u32 a_Index, u32 a_Options);
void SoBkgEnable(u32 a_Index, bool a_Enable);

void SoBkgSetMosaicEnable( u32 a_Index, bool a_Enable );
void SoBkgSetPriority(u32 a_Index, u32 a_Priority);

bool SoBkgIsEnabled(u32 a_Index);
//bool SoBkgGetMosaicEnable(u32 a_Index);
//u32  SoBkgGetPriority(u32 a_Index);

void SoBkgSetOffset(u16 a_Index, u16 a_XPos, u16 a_YPos);

u16 *SoBkgGetData(u32 a_Index);

void SoBkgLoadSolidColorTile(u16 a_Index, u32 a_Tilenum, u16 a_ColorIndex);


// Direct mode functions
void SoBkgFill(u16 a_Index, u16 a_Data);
void SoBkgFillBlock(u16 a_index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height, u16 a_Data);
void SoBkgWrite(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer);
void SoBkgWriteV(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer);
void SoBkgWriteBlock(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height, u16 a_Pitch,
					 u16 *a_buffer);
void SoBkgWriteBlock16Color(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height, u16 a_Pitch, u16 a_PalIndex, u16 *a_buffer);

// Indirect mode functions (dynamically loaded tiles)
void SoBkgClearIndirect(u16 a_Index);
void SoBkgWriteIndirect(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer);
void SoBkgWriteVIndirect(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Len, u16 *a_buffer);
void SoBkgWriteBlockIndirect(u16 a_Index, u16 a_XIndex, u16 a_YIndex, u16 a_Width, u16 a_Height,
							 u16 a_Pitch, u16 *a_buffer);

// Printing functions
void SoBkgPrint(u16 a_Index, const char *a_String);
void SoBkgPrintf(u16 a_Index, const char *a_Format, ...);

//!	\brief Print a string at a given location
//!
//! \param a_Index	Background index: 0, 1, 2, or 3
//! \param a_X		X-coordinate of target
//! \param a_Y		Y-coordinate of target
//!	\param a_String	 The string you want to write.
//!
//! This function causes a text string to be copied to a specified location
//! on a background.  It is really just a convenience wrapper around
//! \a SoBkgSetTextCursor and \a SoBkgPrint
//! 
//! void SoBkgPrintAt(u16 a_Index, u16 a_X, u16 a_Y, const char *a_String);
//!
#define SoBkgPrintAt(a_Index, a_X, a_Y, a_String) \
	(SoBkgSetTextCursor(a_Index, a_X, a_Y), SoBkgPrint(a_Index, a_String))

void SoBkgSetTextWindow(u16 a_Index, u16 a_UpperLeftX, u16 a_UpperLeftY, u16 a_LowerRightX, u16 a_LowerRightY);
void SoBkgSetTextWrapping(u16 a_Index, bool a_DoWrapping);

void SoBkgSetTextCursor(u16 a_Index, u16 a_X, u16 a_Y);
const SoVector2* SoBkgGetTextCursor(u16 a_Index);
void SoBkgSetFont(u16 a_Index, const SoBkgFont *a_Font);
const SoBkgFont *SoBkgGetFont(u16 a_Index);


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_BKG_H
