// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoEffects.h
	\author		Gabriele Scibilia
	\date		February 4 2002
	\ingroup	SoEffects

	See the \a SoEffects module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_EFFECTS_H
#define SO_EFFECTS_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoEffects SoEffects
	\brief	  All color special effects processing

	Singleton

	The GBA provides alpha blending, and fade-in/fade-out color special effects.
	The area where these effects are applied can be limited using a window.

	Alpha blending allows processing for 16 levels of semi-transparency
	performing arithmetic operations on 2 selected layers.

	Fade-in/fade-out allows processing for 16 levels of brightness performing
	arithmetic operations on 1 selected layer.

	The arithmetic expressions that explain the effects are:

	- alpha-blending color = ( 1/EVA * 1st target ) + ( 1/EVB * 2nd target )

	- brightness increase color = 1st target + ( 1 - 1st target ) * 1/EVY

	- brightness decrease color = 1st target - ( 1st target * 1/EVY )

	See the Pern Project (http://www.thepernproject.com) or other Nintendo
	references for more information on this system.

	\note	EVA, EVB, and EVY coefficients are only valid in the range [0..16].

	\note	The BD is the backdrop, which is the area you see behind all the
	        backgrounds and is just a solid screen of color zero.

	\note	In case of alpha blending, the 2 surfaces you're targeting must
	        have meaningfull priorities. 

	\warning To use effects, you have to specify the window usage of these
	         effects. If you simply want a fullscreen effect just use
			 \code
				SoWindow0OutsideEffectsEnable( true );
				SoWindow0InsideEffectsEnable( true );
			 \endcode
			 And everything will work. You don't even have to enable the window
	         itself. Just make sure you enable effects for at least one window.

	\todo	Provide some higher level functions too, or some examples.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------

#define SO_EFFECTS_MODE_NONE		0						//!< No special effects (default)
#define SO_EFFECTS_MODE_BLEND		(SO_BIT_6)				//!< Alpha-blending (semi-transparency)
#define SO_EFFECTS_MODE_FADE_IN		(SO_BIT_7)				//!< Fade-in (brightness increase)
#define SO_EFFECTS_MODE_FADE_OUT	(SO_BIT_6 | SO_BIT_7)	//!< Fade-out (brightness decrease)

#define SO_EFFECTS_TARGET1_BG0		(SO_BIT_0)				//!< 1st target pixel: BG0
#define SO_EFFECTS_TARGET1_BG1		(SO_BIT_1)				//!< 1st target pixel: BG1
#define SO_EFFECTS_TARGET1_BG2		(SO_BIT_2)				//!< 1st target pixel: BG2
#define SO_EFFECTS_TARGET1_BG3		(SO_BIT_3)				//!< 1st target pixel: BG3
#define SO_EFFECTS_TARGET1_OBJ		(SO_BIT_4)				//!< 1st target pixel: OBJ
#define SO_EFFECTS_TARGET1_BD		(SO_BIT_5)				//!< 1st target pixel: BD

//! 1st target pixel: everything
#define SO_EFFECTS_TARGET1_ALL		(SO_6_BITS)

#define SO_EFFECTS_TARGET2_BG0		(SO_BIT_8)				//!< 2nd target pixel: BG0
#define SO_EFFECTS_TARGET2_BG1		(SO_BIT_9)				//!< 2nd target pixel: BG1
#define SO_EFFECTS_TARGET2_BG2		(SO_BIT_10)				//!< 2nd target pixel: BG2
#define SO_EFFECTS_TARGET2_BG3		(SO_BIT_11)				//!< 2nd target pixel: BG3
#define SO_EFFECTS_TARGET2_OBJ		(SO_BIT_12)				//!< 2nd target pixel: OBJ
#define SO_EFFECTS_TARGET2_BD		(SO_BIT_13)				//!< 2nd target pixel: BD

//! 2nd target pixel: everything
#define SO_EFFECTS_TARGET2_ALL		(SO_BIT_8 | SO_BIT_9 | SO_BIT_10 | SO_BIT_11 | SO_BIT_12 | SO_BIT_13)

// --------------------------------------------------------------------------
// Public methods;
// --------------------------------------------------------------------------

void SoEffectsSetMode( u16 a_Mode, u16 a_Target1, u16 a_Target2 );
void SoEffectsSetBlend( u16 a_CoeffEVA, u16 a_CoeffEVB );
void SoEffectsSetFade( u16 a_CoeffEVY );

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
