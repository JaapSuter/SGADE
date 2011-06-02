// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSound.h
	\author		Jaap Suter
	\date		Feb 2 2002	
	\ingroup	SoSound

	See the \a SoSound module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_SOUND_H
#define SO_SOUND_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoSound SoSound
	\brief	  Module for all sound handling

	Singleton
	
	This module is still in experimental phase. Only use when you're absolutely
	sure about what you are doing.
	
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Public Functions;
// ----------------------------------------------------------------------------

void SoSoundInitialize( void );
void SoSoundPlayDirectSoundA( s8* a_WaveData, u32 a_Timer, u32 a_DMAChannel, u32 a_Frequency );


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
