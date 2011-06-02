
// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2003 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgConsole.h
	\author		Mark Price
	\date		Oct 26, 2003
	\ingroup	SoBkgConsole

	See the \a SoBkgConsole module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_BKG_CONSOLE_H
#define SO_BKG_CONSOLE_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoBkgConsole SoBkgConsole
	\brief	  Functions to debugging console display (output only)

	This special-purpose module provides a debug display window.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

//#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

void SoBkgConsoleInit(u16 a_Bkg);
void SoBkgConsolePrint(const char *a_String);
void SoBkgConsolePrintf(const char* a_Format, ...);

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_BKG_CONSOLE_H
