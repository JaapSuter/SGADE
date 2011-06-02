// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSprite.c
	\author		Jaap Suter
	\date		Jun 27 2001
	\ingroup	SoSprite

	See the \a SoSprite module for more information.

	Contains the non-inline functions of the \a SoSprite module.
*/
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------
#include "SoSprite.h"
#include "SoDisplay.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Disables rendering of this sprite.

	\param	a_This		This pointer.	
*/
// ----------------------------------------------------------------------------
void SoSpriteDisable( SoSprite* a_This )
{
	// Enable double size and switch off rotation (this disables the sprite);
	a_This->m_Attribute[ 0 ] &= ~SO_BIT_8;
	a_This->m_Attribute[ 0 ] |=  SO_BIT_9;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the screen position of the top left corner of the sprite.

	\param	a_This		This pointer.
	\param	a_X			Integer horizontal screen coordinate in the range [-sprite width .. 240]
	\param	a_Y			Integer horizontal screen coordinate in the range [-sprite height .. 120]

	This positions the sprite on the correct location on screen. Takes care of sprite clipping
	(negative coords) and sprite double sizing. So don't worry....
*/
// ----------------------------------------------------------------------------
void SoSpriteSetTranslate( SoSprite* a_This, s32 a_X, s32 a_Y )
{
	// Mask out previous X value;
	a_This->m_Attribute[ 1 ] &= ~SO_9_BITS;

	// Compensate for double size;
	if ( a_This->m_Attribute[ 0 ] & SO_BIT_9 )
	{
		// Sub half the size;
		a_X -= SoSpriteGetWidth( a_This ) >> 1;
		a_Y -= SoSpriteGetHeight( a_This ) >> 1;
	}
	
	// Set new value;
	if ( a_X < 0 )
	{
		a_This->m_Attribute[ 1 ] |= (SO_9_BITS + 1) + a_X;
	}
	else
	{
		a_This->m_Attribute[ 1 ] |= a_X;
	}

	// Mask out previous Y value;
	a_This->m_Attribute[ 0 ] &= ~SO_8_BITS;
	
	// Set new value;
	if ( a_Y < 0 )
	{
		a_This->m_Attribute[ 0 ] |= (SO_8_BITS + 1) + a_Y;
	}
	else
	{
		a_This->m_Attribute[ 0 ] |= a_Y;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the animation (character-data) this sprite should use.

	\param	a_This		This pointer.
	\param	a_Index		Index of the animation data as loaded by the 
						sprite memory manager.

	Often used like this:	
	\code
		
		SoSprite* sprite = SoSpriteManagerRequestSprite();
		SoSpriteSetAnimationIndex( someSprite, SoSpriteMemManagerLoad( someAnimation ) );

	\endcode

	See the \a SoSpriteMemManagerLoad function for more information. But basically it loads a 
	certain animation data in sprite character memory and returns the index in sprite 
	character memory. 
	
	If this is not clear, mail me. I'll update the docs.

*/
// ----------------------------------------------------------------------------
void SoSpriteSetAnimationIndex( SoSprite* a_This, u32 a_Index )
{
	// Assert that the index is valid;
	SO_ASSERT( a_Index < 1024, "Sprite animation index should be smaller then 1024" );

	// Mask out a previous index;
	a_This->m_Attribute[ 2 ] &= ~SO_10_BITS;

	// Set the new sprite index;
	a_This->m_Attribute[ 2 ] |= a_Index;	
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Returns the animation index the sprite is using.

	\param	a_This		This pointer.
*/
// ----------------------------------------------------------------------------
u32 SoSpriteGetAnimationIndex( SoSprite* a_This )
{
	return (a_This->m_Attribute[ 2 ] & SO_10_BITS);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the sprite to 256 color palette mode instead of 16 color palettes.

	\param	a_This		This pointer.
	\param	a_Enable	True if the sprite is 256 colors, false if 16 colors.	

	Note that you will rarely use this function, cause
	you can deduce this values from the animation you want the sprite to use. So when you've
	loaded an animation, and you have a certain sprite, you can do the following:

	\code
		SoSpriteCopyPropertiesFromAnimation( someSprite, someAnimation );
	\endcode

	And the sprite will have the correct color mode.
*/
// ----------------------------------------------------------------------------
void SoSpriteSet256ColorMode( SoSprite* a_This, bool a_Enable )
{
	if ( a_Enable )
	{
		a_This->m_Attribute[ 0 ] |= SO_BIT_13;
	}
	else
	{
		a_This->m_Attribute[ 0 ] &= ~SO_BIT_13;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Enabs the mosaic for this sprite.

	\param	a_This		This pointer.
	\param	a_Enable	True if you want mosaic enabled.

	Ofcourse you have to set the \a SO_REG_DISP_MOSAIC register with some values to 
	use mosaic for sprites.	
*/
// ----------------------------------------------------------------------------
void SoSpriteSetMosaicEnable( SoSprite* a_This, bool a_Enable )
{
	if ( a_Enable )
	{
		a_This->m_Attribute[ 0 ] |= SO_BIT_12;
	}
	else
	{
		a_This->m_Attribute[ 0 ] &= ~SO_BIT_12;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Enables sprite rotation and scaling.

	\param	a_This		This pointer.
	\param	a_Enable	True if you want to enable it. False if you want to disable it.
*/
// ----------------------------------------------------------------------------
void SoSpriteSetRotationAndScaleEnable( SoSprite* a_This, bool a_Enable )
{
	if ( a_Enable )
	{
		a_This->m_Attribute[ 0 ] |= SO_BIT_8;
	}
	else
	{
		a_This->m_Attribute[ 0 ] &= ~SO_BIT_8;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the index used for sprite scale and rotation settings.
	
	\param	a_This		This pointer.
	\param	a_Index		Index of the sprite scale and rotation settings (range [0..31]).

	You can set the sprite scale and rotation values with 
	the \a SoSpriteManagerSetRotationAndScale function. Use the same index as you use for 
	this one and the sprite will use it.

*/
// ----------------------------------------------------------------------------
void SoSpriteSetRotationAndScaleIndex( SoSprite* a_This, u32 a_Index )
{
	// Assert the input;
	SO_ASSERT( a_Index < 32, "Index out of bounds, valid range is [0..31]." );

	// Mask out the previous value;
	a_This->m_Attribute[ 1 ] &= ~(SO_3_BITS << 9);

	// Set;
	a_This->m_Attribute[ 1 ] |= a_Index << 9;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!	
	\brief	Enables double size for the sprite.

	\param	a_This		This pointer.
	\param	a_Enable	True if you want the sprite to double in size (of the canvas). 
	
	Usefull for rotating sprites. Cause a rotating sprite will exceed its canvas when
	rotated 45 degrees (for example). With this enabled it won't. 
*/
// ----------------------------------------------------------------------------
void SoSpriteSetSizeDoubleEnable( SoSprite* a_This, bool a_Enable )
{
	if ( a_Enable )
	{
		a_This->m_Attribute[ 0 ] |= SO_BIT_9;
	}
	else
	{
		a_This->m_Attribute[ 0 ] &= ~SO_BIT_9;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Flips the sprite horizontally.

	\param	a_This		This pointer.

	Call twice to bring back to the original setting. 	
*/
// ----------------------------------------------------------------------------
void SoSpriteFlipHorizontal( SoSprite* a_This ) 
{ 
	a_This->m_Attribute[ 1 ] ^= SO_BIT_12; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Flips the sprite vertically.

	\param	a_This		This pointer.

	Call twice to bring back to the original setting.	
*/
// ----------------------------------------------------------------------------
void SoSpriteFlipVertical(	  SoSprite* a_This ) 
{ 
	a_This->m_Attribute[ 1 ] ^= SO_BIT_13; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Returns the width of the sprite.

	\param	a_This		This pointer.	
*/
// ----------------------------------------------------------------------------
u32 SoSpriteGetWidth( SoSprite* a_This )
{
	// Are we tall;
	if ( a_This->m_Attribute[ 0 ] & SO_BIT_15 )
	{
		// Return the width;
		switch ( (a_This->m_Attribute[ 1 ] & (SO_2_BITS << 14)) >> 14 )
		{
			case 0: return 8;
			case 1: return 8;
			case 2: return 16;
			case 3: return 32;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				return 8; // We return the smallest sprite size for release builds;
		}
	}
	else
	// Are we wide;
	if ( a_This->m_Attribute[ 0 ] & SO_BIT_14 )
	{
		// Return the width;
		switch ( (a_This->m_Attribute[ 1 ] & (SO_2_BITS << 14)) >> 14 )
		{
			case 0: return 16;
			case 1: return 32;
			case 2: return 32;
			case 3: return 64;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				return 8; // We return the smallest sprite size for release builds;
		}
	}
	else
	// We are square;
	{
		// Return the width;
		switch ( (a_This->m_Attribute[ 1 ] & (SO_2_BITS << 14)) >> 14 )
		{
			case 0: return 8;
			case 1: return 16;
			case 2: return 32;
			case 3: return 64;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				return 8; // We return the smallest sprite size for release builds;
		}
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the height of the sprite.

	\param	a_This		This pointer.	
*/
// ----------------------------------------------------------------------------
u32 SoSpriteGetHeight( SoSprite* a_This )
{
	// Are we tall;
	if ( a_This->m_Attribute[ 0 ] & SO_BIT_15 )
	{
		// Return the height;
		switch ( (a_This->m_Attribute[ 1 ] & (SO_2_BITS << 14)) >> 14 )
		{
			case 0: return 16;
			case 1: return 16;
			case 2: return 32;
			case 3: return 64;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				return 8; // We return the smallest sprite size for release builds;
		}
	}
	else
	// Are we wide;
	if ( a_This->m_Attribute[ 0 ] & SO_BIT_14 )
	{
		// Return the height;
		switch ( (a_This->m_Attribute[ 1 ] & (SO_2_BITS << 14)) >> 14 )
		{
			case 0: return 8;
			case 1: return 8;
			case 2: return 16;
			case 3: return 32;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				return 8; // We return the smallest sprite size for release builds;
		}
	}
	else
	// We are square;
	{
		// Return the height;
		switch ( (a_This->m_Attribute[ 1 ] & (SO_2_BITS << 14)) >> 14 )
		{
			case 0: return 8;
			case 1: return 16;
			case 2: return 32;
			case 3: return 64;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				return 8; // We return the smallest sprite size for release builds;
		}
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the size of the sprite.

	\param	a_This		This pointer.	

	\param	a_Width		Width of the sprite.
	\param  a_Height	Height of the sprite.

	Not all widths and heights combinations are valid. The following can be used...
	  
		 8x8  16x16 32x32 64x64 
		 8x16  8x32 16x32 32x64 
		16x8  32x8  32x16 64x32 

	In debug mode these values are asserted. Note that you will rarely use this function, cause
	you can deduce these values from the animation you want the sprite to use. So when you've
	loaded an animation, and you have a certain sprite, you can do the following:

	\code
		SoSpriteCopyPropertiesFromAnimation( someSprite, someAnimation );
	\endcode

	And the sprite will have the correct size.
*/
// ----------------------------------------------------------------------------
void SoSpriteSetSize( SoSprite* a_This, u32 a_Width, u32 a_Height )
{
	// Variable to store bitmask;
	u16 spriteShape;
	u16 spriteSize;

	// Is the sprite square;
	if ( a_Width == a_Height )
	{
		// Square;
		spriteShape = 0;

		// What size is the sprite;
		switch ( a_Width )
		{
			case  8: spriteSize = 0;					 break;
			case 16: spriteSize = SO_BIT_14;			 break;
			case 32: spriteSize = SO_BIT_15;			 break;
			case 64: spriteSize = SO_BIT_14 | SO_BIT_15; break;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				spriteSize = 0;		// For release mode;
		}
	} 
	else 
	{
		// Is the sprite tall or wide;
		if ( a_Width < a_Height ) 
		{
			// The sprite is tall;
			// TODO: I thought that BIT_14 was for tall,
			// and 15 was for wide, but Mappy seems to agree otherwise;
			// is this a Mappy thing, my fault, or was my original thought wrong?
			spriteShape = SO_BIT_15;
		}
		else
		{
			// Wide;
			spriteShape = SO_BIT_14;
		}

		// What size is the sprite;
		switch ( a_Width * a_Height ) 
		{
			case ( 8 * 16): spriteSize = 0;						break;
			case ( 8 * 32): spriteSize = SO_BIT_14;			    break;
			case (16 * 32): spriteSize = SO_BIT_15;				break;
			case (32 * 64): spriteSize = SO_BIT_14 | SO_BIT_15; break;
			default: 
				SO_ASSERT( false, "Unreachable default switch clause reached" );
				spriteSize = 0;		// For release mode;
		}
	}

	// Mask out previous shape and size;
	a_This->m_Attribute[ 0 ] &= ~(SO_BIT_14 | SO_BIT_15);
	a_This->m_Attribute[ 1 ] &= ~(SO_BIT_14 | SO_BIT_15);
	
	// Set the shape;
	a_This->m_Attribute[ 0 ] |= spriteShape;

	// Set the size;
	a_This->m_Attribute[ 1 ] |= spriteSize;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/*!
	\brief	Copies the size and color attributes from a certain animation

	\param	a_This		This pointer.
	\param	a_Animation	Animation the sprite uses.

	This copies the size and the color (256 or 16) properties from the animation into the 
	sprite. The complete code for loading a sprite then becomes:

	\code

		// Include the animation data;
		#include "someAnimation.data";		

		// Request a location in OAM;
		SoSprite* sprite = SoSpriteManagerRequestSprite();			
		
		// Load the animation and assign it to the sprite;
		SoSpriteSetAnimationIndex( someSprite, SoSpriteMemManagerLoad( someAnimation ) );

		// Copy the animation's properties.
		SoSpriteCopyPropertiesFromAnimation( someSprite, someAnimation );
		
	\endcode
	
*/
// ----------------------------------------------------------------------------
void SoSpriteCopyPropertiesFromAnimation( SoSprite* a_This, const SoSpriteAnimation* a_Animation )
{
	SoSpriteSetSize( a_This, SoSpriteAnimationGetFrameWidth( a_Animation ), SoSpriteAnimationGetFrameHeight( a_Animation ) );
	SoSpriteSet256ColorMode( a_This, ! SoSpriteAnimationIs16Colors( a_Animation ) );
}
// ----------------------------------------------------------------------------


//! \brief	changes the display priority for a particular sprite
//!
//!	\param	a_This		Sprite's 'this' pointer
//!	\param  a_Priority	Display priority for the sprite (0 to 3)
//!
//! This function updates the display priority for a given
//! sprite.  Sprites with lower priority are displayed in front of
//! sprites or backgrounds with highter priority.
//!
//! When the priority of two sprites is the same, the sprite with the
//! lower index number is displayed on top.  When the priority of a
//! sprite is the same as the priority of a background, the sprite
//! is displayed on top.
//!
void SoSpriteSetPriority(SoSprite* a_This, u32 a_Priority)
{
	// validate the input
	SO_ASSERT(a_Priority < 4, "Invalid display priority");

	// Mask out the previous priority
	a_This->m_Attribute[2] &= ~(SO_2_BITS << 10);
	a_This->m_Attribute[2] |= (a_Priority << 10);
}

// ----------------------------------------------------------------------------
//! \brief	set the sprite to use one of the 16 palettes available in 16 color mode
//!
//!	\param	a_This		Sprite's 'this' pointer
//!	\param  a_Index		which 16 color palette (out of 16 avail) to use
//!
//! This function sets the index to the 16 color palette that the sprite
//! will use. In 16 color mode, the GBA has 16 different 16-Color palettes
//! available for the sprites to use.

void SoSpriteSet16ColorPaletteIndex(SoSprite* a_This, u16 a_Index) {

	// make sure sprite is 16 colors before we set the index.
	// if 256 colors, we do nothing.
	
	// ensure that the index is witin the range 0-15
	// validate the input
	SO_ASSERT(a_Index < 16, "Palette Index out of range. Valid 0-15.");
	
	// Mask out the previous priority
	a_This->m_Attribute[2] &= ~(SO_4_BITS << 12);
	// set the value
	a_This->m_Attribute[2] |= (a_Index << 12);
			
	
}

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
