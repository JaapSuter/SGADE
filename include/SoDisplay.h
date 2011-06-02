// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoDisplay.h
	\author		Jaap Suter
	\date		Jan 1 2002	
	\ingroup	SoDisplay

	See the \a SoDisplay module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_DISPLAY_H
#define SO_DISPLAY_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoDisplay SoDisplay
	\brief	  All general display related stuff.

	Singleton

	This module contains and defines everything related to the display of the
	GBA. Very specific stuff gets it's own module like \a SoMode4Renderer and
	\a SoMode4PolygonRasterizer but this module contains the most general stuff.
		
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! GBA display control register, defined in the header cause it's used in multiple files.
#define SO_REG_DISP_CNT				(*(volatile u16*)0x4000000)	

//! GBA display statistics register, defined in the header cause it's used in multiple files.
#define SO_REG_DISP_STAT			(*(volatile u16*)0x4000004)	

//! GBA scanline counter register, defined in the header cause it's used in multiple files.
#define SO_REG_SCANLINE_COUNT		(*(volatile u16*)0x4000006)	

//! GBA display mosaic register, defined in the header cause it's used in multiple files.
//! \warning Write only register. Use the \a SO_REG_DISP_MOSAIC_RW register instead.
#define		SO_REG_DISP_MOSAIC_W 		(*(volatile u16*)0x400004C)	

//! GBA display mosaic register backup, so we can both read and write;
//! \warning This register needs to be copied into SO_REG_DISP_MOSAIC_W to have any effect.
extern u16 SO_REG_DISP_MOSAIC_RW;

// Screen dimensions;					
#define SO_SCREEN_WIDTH				240							//!< Width of the GBA screen
#define SO_SCREEN_HALF_WIDTH		(SO_SCREEN_WIDTH / 2)		//!< Half of the GBA screen width
#define SO_SCREEN_HEIGHT			160							//!< Height of the GBA screen
#define SO_SCREEN_HALF_HEIGHT		(SO_SCREEN_HEIGHT / 2)		//!< Half of the GBA screen height

// Colors
#define SO_RED						(SO_5_BITS)						//!< Pure red;
#define SO_GREEN					(SO_5_BITS << 5)				//!< Pure green;
#define SO_BLUE						(SO_5_BITS << 10)				//!< Pure blue;
#define SO_WHITE					(SO_RED | SO_GREEN | SO_BLUE)	//!< Pure white;
#define SO_BLACK					0								//!< Pure black;
#define SO_YELLOW					(SO_RED | SO_GREEN)				//!< Pure yellow;
#define SO_CYAN						(SO_BLUE | SO_GREEN)			//!< Pure cyan;
#define SO_MAGENTA					(SO_RED | SO_BLUE)				//!< Pure magenta (purple);
#define SO_PURPLE					SO_MAGENTA						//!< Pure purple (magenta);

// ----------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------

#define SO_RGB( r, g, b )		(((b)<<10)+((g)<<5)+(r))	//!< Converts three 5 bit values for RGB into one 16 bit RGB value.
#define SO_RGB_GET_R( c )		(((c)&(31    ))    )		//!< Extracts the 5 bit R value out of a 15 bit RGB value.
#define SO_RGB_GET_G( c )		(((c)&(31<< 5))>> 5)		//!< Extracts the 5 bit G value out of a 15 bit RGB value.
#define SO_RGB_GET_B( c )		(((c)&(31<<10))>>10)		//!< Extracts the 5 bit B value out of a 15 bit RGB value.

#define SO_SCREEN_HALF_WIDTH_MULTIPLY( y ) (((y)<<7)-((y)<<3))	//!< Multiplies the given number by the half width (120) of the GBA screen;

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

void SoDisplayInitialize(		  void );

void SoDisplayEnable(u32 a_Enable);

void SoDisplaySetMode(u32 a_Mode);
u32  SoDisplayGetMode(void);

u32	 SoDisplayGetVCounter(		  void );

bool SoDisplayIsInVBlank(		  void );
bool SoDisplayIsInHBlank(		  void );

void SoDisplayWaitForVBlankStart( void );
void SoDisplayWaitForVBlankEnd(	  void );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif
	
#endif
