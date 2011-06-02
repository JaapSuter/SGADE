// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoEffects.c
	\author		Gabriele Scibilia
	\date		February 4 2002
	\ingroup	SoEffects

	See the \a SoEffects module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoEffects.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! \internal Specifies types of color special effects and the target pixels
#define SO_REG_EFFECTS_BLD_CNT		(*(u16*)0x04000050)

//! \internal Specifies color special effects coefficient EVA and EVB
#define SO_REG_EFFECTS_BLD_ALPHA	(*(u16*)0x04000052)

//! \internal Specifies color special effects coefficient EVY
#define SO_REG_EFFECTS_BLD_Y		(*(u16*)0x04000054)


// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Changes color special effects mode and specifies the two target surfaces.

	\param	a_Mode		Color special effects mode.
	\param	a_Target1	1st target surface.
	\param	a_Target2	2nd target surface.

	\note	If doing a brightness increase or decrease (fade-in or -out) 
			the \a a_Target2 parameter can be zero.

	\note	Semi-transparent OBJs are individually specified in OAM;
			please refer to the \a SoSprite module documentation.
*/
// ----------------------------------------------------------------------------
void SoEffectsSetMode( u16 a_Mode, u16 a_Target1, u16 a_Target2 )
{
	SO_REG_EFFECTS_BLD_CNT = a_Mode | a_Target1 | a_Target2;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Specifies the coefficients used in alpha-blending processing.

	\param	a_CoeffEVA	EVA coefficient controls the 1st target surface.
	\param	a_CoeffEVB	EVB coefficient controls the 2nd target surface.

	They represent the fraction of the final result each surface contributes.
	Thus, if EVA coefficient is 12 and EVB is 4, the resulting image will appear
	to be 12/16 the color of 1st target and 4/16 the color of 2nd target.
*/
// ----------------------------------------------------------------------------
void SoEffectsSetBlend( u16 a_CoeffEVA, u16 a_CoeffEVB )
{
	// Assert the input;
	SO_ASSERT( a_CoeffEVA <= 16, "EVA out of bounds, valid range is [0..16]." );
	SO_ASSERT( a_CoeffEVB <= 16, "EVB out of bounds, valid range is [0..16]." );

	SO_REG_EFFECTS_BLD_ALPHA = a_CoeffEVA | ( a_CoeffEVB << 8 );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Specifies the coefficient used in brightness processing.

	\param	a_CoeffEVY	EVY coefficient controls the 1st target surface.

	This is the amount by which to fade into the pixel color specified
	in 1st target surface.
*/
// ----------------------------------------------------------------------------
void SoEffectsSetFade( u16 a_CoeffEVY )
{
	// Assert the input;
	SO_ASSERT( a_CoeffEVY <= 16, "EVY out of bounds, valid range is [0..16]." );

	SO_REG_EFFECTS_BLD_Y = a_CoeffEVY;
}
// ----------------------------------------------------------------------------
