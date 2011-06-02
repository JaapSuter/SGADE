// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSpriteManager.h
	\author		Jaap Suter
	\date		Sep 19 2001
	\ingroup	SoSpriteManager

	See the \a SoSpriteManager module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_SPRITE_MANAGER_H
#define SO_SPRITE_MANAGER_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoSpriteManager SoSpriteManager
	\brief	  Manages sprite objects.
	
	Singleton

	This module manages all sprites.

	It handles the GBA's object attribute memory. This module does NOT manage
	the actual sprite memory (where characters are located). The
	\a SoSpriteMemManager module is for that. Of course these two modules do
	interact in some way. Upon studying this module, I suggest you take a look
	at the \a SoSprite and \a SoSpriteMemManager modules too.

	Most of the ideas behind this implementation came from Rafael Baptista's
	excellent article on GBA resource management that was originally posted on
	http://www.gamasutra.com Unfortunately the article was removed from the
	site due to legal reasons.

	Note that this module uses the interrupt manager. If the interrupt manager
	is not already initialized, it will initialize it in its own initialize
	routine.

	This module does more than just manage OAM; it also handles sprite mosaic
	values, rotation and scale settings, etc. etc. In short everything sprite
	related that doesn't belong to a particular sprite instance (because then
	it would be in \a SoSprite).
	
*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoSprite.h"

// ----------------------------------------------------------------------------
// Public functions
// ----------------------------------------------------------------------------

void	  SoSpriteManagerInitialize( void );

void	  SoSpriteManagerUpdate( void );

void	  SoSpriteManagerEnableSprites( void );
void	  SoSpriteManagerDisableSprites( void );

SoSprite* SoSpriteManagerRequestSprite( void );
void	  SoSpriteManagerRelease( SoSprite* a_Sprite );

void	  SoSpriteManagerSetMosaic( u32 a_HorizontalSize, u32 a_VerticalSize );
u32		  SoSpriteManagerGetHorizontalMosaic( void );
u32		  SoSpriteManagerGetVerticalMosaic( void );

void	  SoSpriteManagerSetRotationAndScale( u32 a_Index, s32 a_Angle, 
											  u32 a_FixedScaleX, u32 a_FixedScaleY );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
