// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgManager.h
	\author		Jaap Suter
	\date		Feb 5 2002
	\ingroup	SoBkgManager

	See the \a SoBkgManager module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_BKG_MANAGER_H
#define SO_BKG_MANAGER_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoBkgManager SoBkgManager
	\brief	  Manages all backgrounds.

	Singleton

	This module handles all background handling logic that does not relate to a
	particular \a SoBkg instance. This module works together with the \a SoBkg module.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"


// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

void SoBkgManagerSetMosaic( u32 a_HorizontalSize, u32 a_VerticalSize );


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_BKG_MANAGER_H
