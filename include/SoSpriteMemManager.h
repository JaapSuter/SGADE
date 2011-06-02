// ----------------------------------------------------------------------------
/*!
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSpriteMemManager.h
	\author		Jaap Suter, Gabriele Scibilia
	\date		June 23, 2003
	\ingroup	SoSpriteMemManager

	See the \a SoSpriteMemManager module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_SPRITE_MEM_MANAGER_H
#define SO_SPRITE_MEM_MANAGER_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*!
	\defgroup SoSpriteMemManager SoSpriteMemManager
	\brief	  Handles sprite character memory

	Singleton.

	This module handles the character data for sprites. It uses the
	\a SoSpriteAnimation struct to load animations (tiles, characters,
	whatever you name it) into sprite memory.   This is distinct from OAM
	(object attribute memory) which is about sprite attributes.  We have the
	\a SoSpriteManager for that. This module works together with the
	\a SoSpriteManager and \a SoSprite instances to create a complete sprite
	system.  So before you start using \a this module read about these other
	two modules too.

	Most of the ideas behind this implementation came from Rafael Baptista's
	excellent article on GBA resource management that was originally posted on
	http://www.gamasutra.com Unfortunately the article was removed from the
	site due to legal reasons.

	\todo Implement a ref-count for sprite animations, so sprites can share
	      animations.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoSpriteAnimation.h"
#include "SoImage.h"

// ----------------------------------------------------------------------------
// Public functions;
// ----------------------------------------------------------------------------

void SoSpriteMemManagerInitialize( void );

void SoSpriteMemManagerSetInBitmappedMode( bool a_Enable );

void SoSpriteMemManagerCopyFromImage( u32 a_Index, SoImage* a_Image );
u32  SoSpriteMemManagerLoadFromImage( SoImage* a_Image );

u32  SoSpriteMemManagerLoad( const SoSpriteAnimation* a_Animation );
u32  SoSpriteMemManagerLoadFrame( u16 a_Frame, const SoSpriteAnimation* a_Animation);
void SoSpriteMemManagerCopyFrame(u32 a_Index, u16 a_Frame, const SoSpriteAnimation* a_Animation);
void SoSpriteMemManagerRelease( u32 a_Index );

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
