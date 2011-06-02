// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSprite.h
	\author		Jaap Suter
	\date		Jun 27 2001
	\ingroup	SoSprite

	See the \a SoSprite module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_SPRITE_H
#define SO_SPRITE_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoSprite SoSprite
	\brief	  To represent single sprites.
	
	This module contains all single sprite functionality. It works together
	with the \a SoSpriteManager and \a SoSpriteMemManager to create a complete
	sprite system.  So before you start using \a SoSprite objects read about
	these other two modules too.

	One other thing. Sometimes sprites are referred to as objects or OBJ's.
	This is all the same.

*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoSpriteAnimation.h"
#include "SoDisplay.h"

// ----------------------------------------------------------------------------
// Type definitions.
// ----------------------------------------------------------------------------

/*!
	\brief Structure representing a single sprite.

	You should never instantiate a SoSprite object yourself as a sprite created
	in this fasion is useless.  You should create a pointer to a SoSprite, and
	use the \a SoSpriteManagerRequestSprite function to make this pointer point
	to a valid sprite.
*/
typedef struct
{
	/*! 
	    \brief		Sprite attributes
		
	    \internal
		
	    Actually you will only ever use the first 3 attributes. But because we 
	    use these structures as overlays on OAM, we have to have a fourth
	    attribute to fill it up. This fourth attribute is actually part of one
	    of the 32 rotation and scale settings available. See the
	    \a SoSpriteManager.c file for more information on this OAM (and shadow
	    OAM, which is interesting).

		But you never work with these attribute directly anyway, you should use
	    the \a SoSprite functions instead. So what do you care? :) hehe...
	*/
	u16  m_Attribute[ 4 ];	

} SoSprite;

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

void SoSpriteDisable( SoSprite* a_This );

void SoSpriteFlipHorizontal( SoSprite* a_This );
void SoSpriteFlipVertical(	  SoSprite* a_This );

u32  SoSpriteGetWidth( SoSprite* a_This );
u32  SoSpriteGetHeight( SoSprite* a_This );

void SoSpriteSetTranslate( SoSprite* a_This, s32 a_X, s32 a_Y );

void SoSpriteSetMosaicEnable( SoSprite* a_This, bool a_Enable );
void SoSpriteSetSizeDoubleEnable( SoSprite* a_This, bool a_Enable );

void SoSpriteSetRotationAndScaleEnable( SoSprite* a_This, bool a_Enable );
void SoSpriteSetRotationAndScaleIndex( SoSprite* a_This, u32 a_Index );

void SoSpriteSet256ColorMode( SoSprite* a_This, bool a_Enable );

void SoSpriteSetAnimationIndex( SoSprite* a_This, u32 a_Index );

u32  SoSpriteGetAnimationIndex( SoSprite* a_This );

void SoSpriteSetSize( SoSprite* a_This, u32 a_Width, u32 a_Height );

void SoSpriteSetPriority(SoSprite* a_This, u32 a_Priority);

void SoSpriteCopyPropertiesFromAnimation( SoSprite* a_This, const SoSpriteAnimation* a_Animation );

void SoSpriteSet16ColorPaletteIndex(SoSprite* a_This, u16 a_Index);

   
// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
