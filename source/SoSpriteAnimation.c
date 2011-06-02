// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSpriteAnimation.c
	\author		Jaap Suter
	\date		Mar 14 2002
	\ingroup	SoSpriteAnimation

	See the \a SoSpriteAnimation module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSpriteAnimation.h"

// ----------------------------------------------------------------------------
// Implementation;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief  Gets the size of the entire frame set.
	\param	a_This	This pointer.
	\return The size in bytes.
*/
// ----------------------------------------------------------------------------
u32	 SoSpriteAnimationGetSize( const SoSpriteAnimation* a_This )
{
	// Calculate the 256 case size;
	u32 retVal = (u32)a_This->m_Height * (u32)a_This->m_Width * (u32)a_This->m_NumFrames;

	// Divide by two for the 16 color case;
	if ( a_This->m_16Colors ) retVal >>= 1;

	// Done;
	return retVal;
}
// ----------------------------------------------------------------------------

#if 0

// ----------------------------------------------------------------------------
/*!
	\brief  Single frame width
	\param	a_This	This pointer;
	\return The width of a single frame, in pixels.
*/
// ----------------------------------------------------------------------------
u32  SoSpriteAnimationGetFrameWidth(  const SoSpriteAnimation* a_This )
{
	return a_This->m_Width;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Single frame height
	\param	a_This	This pointer;
	\return The height of a single frame, in pixels.
*/
// ----------------------------------------------------------------------------
u32  SoSpriteAnimationGetFrameHeight( const SoSpriteAnimation* a_This )
{
	return a_This->m_Height;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Number of frames in the animation.
	\param	a_This	This pointer;
	\return What do you think?
*/
// ----------------------------------------------------------------------------
u32  SoSpriteAnimationGetNumFrames(	  const SoSpriteAnimation* a_This )
{
	return a_This->m_NumFrames;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief To get the Colordepth
	\param	a_This	This pointer;
	\return \a True if the sprite is 16 colors, false if 256 colors.
*/
// ----------------------------------------------------------------------------
bool SoSpriteAnimationIs16Colors( const SoSpriteAnimation* a_This )
{
	return a_This->m_16Colors;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	To obtain the actual frame data;
	\param	a_This	This pointer;
	\return Pointer to the start of the frame data;

	The frame data is arrranged in 8x8 blocks, every frame after the other. You shouldn't
	need this function. It's used by the \a SoSpriteMemManager module.
*/
// ----------------------------------------------------------------------------
u8*  SoSpriteAnimationGetData( const SoSpriteAnimation* a_This )
{
	return a_This->m_Data;
}
// ----------------------------------------------------------------------------

#endif // 0


// ----------------------------------------------------------------------------
/*!
	\brief	Returns the number of sprite memory indices (characters) a 
			single frame occupies. 
	
	\param	a_This	This pointer
	\return The number of character blocks a single frame takes;

	Basically this is the same as the amount of bytes in a frame divided by 32, cause 
	each character block is 32 bytes (smallest unit in sprite memory); 

	This function can be used for sprite animation. If you add this number to the current
	sprite tile index, it will use the next tile index. You can subtract it to get the 
	previous tile index. It is up to you to manage what tile of the tileset you are 
	currently displaying.
*/
// ----------------------------------------------------------------------------
u32 SoSpriteAnimationGetNumIndicesPerFrame( const SoSpriteAnimation* a_This )
{
	// Since each block in memory is 32 bytes, we can divide the byte size of
	// a single frame by 32 (>>5) and voila; the number of blocks a single tile takes.
	// Unless we are in 16 color mode, cause then it takes half of that;
	if ( a_This->m_16Colors )
	{
		return (((u32)a_This->m_Width * (u32)a_This->m_Height) >> 6);
	}
	else
	{
		return (((u32)a_This->m_Width * (u32)a_This->m_Height) >> 5);
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
