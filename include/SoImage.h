// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoImage.h
	\author		Jaap Suter, Gabriele Scibilia
	\date		June 28, 2002
	\ingroup	SoImage

	See the \a SoImage module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_TEXTURE_H
#define SO_TEXTURE_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoImage SoImage
	\brief	  For all image handling
	
	This module represents 8bit palettized and 16bit images

	You can use the \a SoConverter.exe tool to convert image data to 
	Socrates compatible header files, ready to be included in the project.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

/*!
	\brief	Texture structure.
	
	This structure represents an image, and is used by \a SoMesh for example.
	All its members are private, and you should only use the \a SoImage methods
	to access these attributes.
*/
typedef struct
{
	u16		m_Width;		//!< \internal Width of the image;
	u16		m_Height;		//!< \internal Height of the image;
	
	bool    m_Palettized;   //!< \internal True if the image is palettized, false otherwise;
	
	u16*	m_Data;			//!< \internal Array of image data;
	
} SoImage;

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

// u32  SoImageGetWidth(     const SoImage* a_This );
// u32  SoImageGetHeight(    const SoImage* a_This );
// u16* SoImageGetData(	  const SoImage* a_This );
// bool SoImageIsPalettized( const SoImage* a_This );
// void SoImageInitialize( SoImage* a_This, u16 a_Width, u16 a_Height, bool a_Palettized, void* a_Data );

void SoImageBkgLoad( const SoImage* a_This, u32 a_CharBase, u32 a_BaseIdx );

//! Returns the width of the image (texture)
#define SoImageGetWidth(a_This)  ((const u16)((a_This)->m_Width))

//! Returns the height of the image (texture)
#define SoImageGetHeight(a_This)  ((const u16)((a_This)->m_Height))

//! Returns the actual texture data (graphics bytes)
#define SoImageGetData(a_This)  ((a_This)->m_Data)

//! Tests whether an image is palettized or not (returns true if palettized)
#define SoImageIsPalettized(a_This)  ((const bool)((a_This)->m_Palettized))

//! Creates an image from given attributes
#define SoImageInitialize(a_This, a_Width, a_Height, a_Palettized, a_Data)	\
		{																	\
			(a_This)->m_Width = a_Width;									\
			(a_This)->m_Height = a_Height;									\
			(a_This)->m_Palettized = a_Palettized;							\
			(a_This)->m_Data = a_Data;										\
		}


// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
