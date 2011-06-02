// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgManager.c
	\author		Jaap Suter
	\date		Feb 5 2002
	\ingroup	SoBkgManager

	See the \a SoBkgManager module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoBkgManager.h"
#include "SoDisplay.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the background mosaic values
	\param	a_HorizontalSize Horizontal Mosaic value, 0 is no mosaic value, 
							 15 is maximum value.
	\param	a_VerticalSize	 Vertical Mosaic value, 0 is no mosaic value, 
							 15 is maximum value.
*/
// ----------------------------------------------------------------------------
void SoBkgManagerSetMosaic( u32 a_HorizontalSize, u32 a_VerticalSize )
{
	// Assert the input range;
	SO_ASSERT( a_HorizontalSize < 16, "Mosaic size can't be bigger than 15" );
	SO_ASSERT( a_VerticalSize   < 16, "Mosaic size can't be bigger than 15" );

	// Clear any previous value;
	SO_REG_DISP_MOSAIC_RW &= ~(SO_8_BITS);

	// Set the new value;
	SO_REG_DISP_MOSAIC_RW |= (a_HorizontalSize | (a_VerticalSize << 4));

	// Copy it into the write only hardware register;
	SO_REG_DISP_MOSAIC_W = SO_REG_DISP_MOSAIC_RW;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
