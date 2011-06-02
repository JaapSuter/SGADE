// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTables.h
	\author		Jaap Suter
	\date		Jun 25 2001	
	\ingroup	SoTables
*/
// ----------------------------------------------------------------------------

#ifndef SO_TABLES_H
#define SO_TABLES_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoTables SoTables 
	\brief	  All generic precalculated tables.

	Singleton
	
	Contains every precalculated table that is used in more than one place 
	in the SGADE. That is, the general ones. Those that belong to a specific 
	class are defined there.

*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

/*!
	\brief Range of the one-over-N table

	I use a table for fast (1 / N) calculations. Obviously we cannot make
	this table too big. Because we want a bit more precision we allow this
	indexed-division to have a couple of bits for the fraction. That is
	defined by the following value. The table is SO_ONE_OVER_N_MAX_N times
	2^SO_ONE_OVER_N_INDEX_Q times 4 bytes big.

	See \a g_OneOver for more information.
*/
#define SO_ONE_OVER_N_MAX_N   256

//! Number of bits for fraction of entries in the one-over-N table. (see \a g_OneOver)
#define SO_ONE_OVER_N_INDEX_Q 4

/*!
	\brief Number of bits allowed for fading-precision.

	We use a precalculated table for speedy-fading but because we 
	need to be carefull of speed and size we don't allow the full 
	8 bits range for fades. The following constants define the range allowed.
	The table is SO_FADE_STEPS times 256 bytes big.

	See \a g_Fade for more information.
*/
#define SO_FADE_BITS  (4)

//! Number of fade steps allowed (see \a g_Fade)
#define SO_FADE_STEPS (1<<SO_FADE_BITS)

//! Maximum fade value (see \a g_Fade)
#define SO_FADE_MAX   (SO_FADE_STEPS-1)	


// ----------------------------------------------------------------------------
// Globals, see the .c files for the documentation.
// ----------------------------------------------------------------------------

extern const sofixedpoint*  g_Sine;
extern const sofixedpoint*  g_Cosine;
extern const sofixedpoint   g_OneOver[ SO_ONE_OVER_N_MAX_N << SO_ONE_OVER_N_INDEX_Q ];
extern const u8             g_Fade[ SO_FADE_STEPS * 256 ];

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
