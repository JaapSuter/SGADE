// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoFlashMem.h
	\author		Gabriele Scibilia
	\date		Dec 9 2001	
	\ingroup	SoFlashMem

	See the \a SoFlashMem module for more information.
*/
// ----------------------------------------------------------------------------


#ifndef SO_FLASHMEM_H
#define SO_FLASHMEM_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoFlashMem SoFlashMem
	\brief	  SRam handling functionality.

	Singleton

	This module is for saving and loading data in SRAM (flash memory) on GBA 
	Rom packs. For example, you can use it for savegames, highscores, etc. etc.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

u16 SoFlashMemSaveByte( u16 a_Offset, u8 a_Value );
u16 SoFlashMemSaveHalfword( u16 a_Offset, u16 a_Value );
u16 SoFlashMemSaveWord( u16 a_Offset, u32 a_Value );
u16 SoFlashMemSaveBuffer( u16 a_Offset, u16 a_Length, u8* a_Buffer );

u8  SoFlashMemLoadByte( u16 a_Offset );
u16 SoFlashMemLoadHalfword( u16 a_Offset );
u32 SoFlashMemLoadWord( u16 a_Offset );
u16 SoFlashMemLoadBuffer( u16 a_Offset, u16 a_Length, u8* a_Buffer );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
