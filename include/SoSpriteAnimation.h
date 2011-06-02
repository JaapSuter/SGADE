// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSpriteAnimation.h
	\author		Jaap Suter
	\date		Mar 14 2002
	\ingroup	SoSpriteAnimation

	See the \a SoSpriteAnimation module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_SPRITEANIMATION_H
#define SO_SPRITEANIMATION_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoSpriteAnimation SoSpriteAnimation
	\brief	  Module for handling sprite frame sets.

	You can use the SoConverter tool to convert animations to Socrates compatible
	header files.

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
	\brief	Sprite animation structure. 

	A single \a SoSpriteAnimation instance represents a collection of sprite
	frames.

	All the members are private. Use the methods instead.
*/
typedef struct
{	
	bool	m_16Colors;		//!< \internal True if the animation is 16 colors, false if 256 colors.
	u8		m_Width;		//!< \internal Width of a single frame (in pixels);
	u8		m_Height;		//!< \internal Height of a single frame (in pixels);
	u8		m_NumFrames;	//!< \internal Number of frames in the tileset.
	u8*		m_Data;			//!< \internal Actual frame data (palette indices).

} SoSpriteAnimation;

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------
u32	 SoSpriteAnimationGetSize(				 const SoSpriteAnimation* a_This );
u32  SoSpriteAnimationGetNumIndicesPerFrame( const SoSpriteAnimation* a_This );

// u32  SoSpriteAnimationGetFrameWidth(  const SoSpriteAnimation* a_This );
// u32  SoSpriteAnimationGetFrameHeight( const SoSpriteAnimation* a_This );
// u32  SoSpriteAnimationGetNumFrames(	  const SoSpriteAnimation* a_This );
// bool SoSpriteAnimationIs16Colors(	  const SoSpriteAnimation* a_This );

// u8*  SoSpriteAnimationGetData(		  const SoSpriteAnimation* a_This );

//! Returns width of animation frame in pixels
#define SoSpriteAnimationGetFrameWidth(a_This) ((const u8)((a_This)->m_Width))

//! Returns height of animation frame in pixels
#define SoSpriteAnimationGetFrameHeight(a_This)  ((const u8)((a_This)->m_Height))

//! Returns number of frames in the animation
#define SoSpriteAnimationGetNumFrames(a_This)  ((const u8)((a_This)->m_NumFrames))

//! Returns true if the animation is 16 colors, false if it is 256 colors
#define SoSpriteAnimationIs16Colors(a_This)  ((const bool)((a_This)->m_16Colors))

//! Returns actual animation frame data (grahpics bytes).  The frame data is
//! arranged in 8x8 blocks, one frame after another.  You shouldn't need this
//! function -- it is used by the \a SoSpriteMemManager module.
#define SoSpriteAnimationGetData(a_This)  ((a_This)->m_Data)


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
