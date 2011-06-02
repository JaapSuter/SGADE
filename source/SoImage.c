// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoImage.c
	\author		Jaap Suter, Gabriele Scibilia
	\date		June 28, 2003
	\ingroup	SoImage

	See the \a SoImage module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoImage.h"
#include "SoTileSet.h"
#include "SoBkg.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! \internal	this is BKG Video Ram where the ram tiles are stored for display
#define SO_BKG_DATA     ((volatile u16*)(0x06000000))

#if 0

// ----------------------------------------------------------------------------
/*! 
	\brief	To get the width of the texture.
	\param	a_This	This pointer
	\return The width of the texture.
*/
// ----------------------------------------------------------------------------
u32 SoImageGetWidth( const SoImage* a_This ) 
{ 
	return a_This->m_Width; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*! 
	\brief	To get the height of the texture.
	\param	a_This	This pointer
	\return The height of the texture.
*/
// ----------------------------------------------------------------------------
u32 SoImageGetHeight(	const SoImage* a_This ) 
{ 
	return a_This->m_Height; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*! 
	\brief	To get the actual texture data.
	\param	a_This	This pointer
	\return Returns the array of texture data (array of palette indices).
*/
// ----------------------------------------------------------------------------
u16* SoImageGetData( const SoImage* a_This ) 
{ 
	return a_This->m_Data; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Tests whether an image is palettized or not;
	\param a_This This pointer
	\return \a True if the image is palettized, \a false otherwise.
*/
// ----------------------------------------------------------------------------
bool SoImageIsPalettized( const SoImage* a_This )
{
	return a_This->m_Palettized;
}
// ----------------------------------------------------------------------------

#endif // 0

//! \brief	Loads image data into background VRAM
//!
//! \param	a_This		this pointer
//! \param	a_CharBase	charbase bank (0-3)
//! \param	a_BaseIdx	index value of first tile to copy from SoImage
//!
void SoImageBkgLoad( const SoImage* a_This, u32 a_CharBase, u32 a_BaseIdx )
{
	SO_ASSERT( a_CharBase < SO_BKG_MAX_NUM_BACKGROUNDS, "Invalid background index" );

	// Remember: you must supply a palettized image while copying to background VRAM;
	SO_ASSERT( SoImageIsPalettized( a_This ), "Image must be palettized." );

	a_BaseIdx <<= 1;	// this is a palettized image
	a_BaseIdx <<= 4;	// multiply idx by 32/2 bytes
	a_CharBase <<= 13;		// multiply bank by 16k/2

	// Copy the image data to background VRAM, block-by-block;
	SoTileSetCopyFromLinearBuffer( (u32 *) SoImageGetData( a_This ), (u32 *) &SO_BKG_DATA[a_BaseIdx+a_CharBase], 
		SoImageGetWidth( a_This ), SoImageGetHeight( a_This ) );
}

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
