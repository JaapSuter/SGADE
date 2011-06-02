// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSystem.c
	\author		Jaap Suter
	\date		June 21 2001
	\ingroup	SoSystem

	Implementation of the SoSystem module.
  
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"
#include "SoTables.h"
#include "SoMath.h"
#include "SoMode4Renderer.h"
#include "SoPalette.h"
#include "SoDisplay.h"
#include "SoKeys.h"
#include "SoDebug.h"
#include "SoIntManager.h"

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*
	(This comment is not parsed by Doxygen, because it's only important during compile time.)
	
	Here we do some assertions to make sure your SGADE compile is correct. This may not 
	seem important, but it is. We are using some hardcoded constants in SGADE 
	assembly code. This is because we cannot reference some SGADE #defines made in C from 
	the ASM code. Or maybe I'm just too lazy to find out how this can be done. Anyway, if 
	these C defines are changed in the future, we need to change the assembly code as well. 
	That's why I put some compile time assertions over here, so we are 
	reminded when we change the values of the defines. This also applies to some macros,
	especially the fixed point macros in \a SoMath. These might need changes too.

	So, if one of the following assertions fails, check all .S files, and check 
	all SoMath stuff to see whether there is a problem (which there surely is). Or 
	contact me, and I will fix it.
*/
// ----------------------------------------------------------------------------
SO_COMPILE_TIME_ASSERT( 16 == SO_FIXED_Q							, Fixed_Q_no_longer_the_same_as_in_hardcoded_assembly );
SO_COMPILE_TIME_ASSERT( 120 == SO_SCREEN_HALF_WIDTH				    , Screen_half_width_no_longer_the_same_as_in_hardcoded_assembly );
SO_COMPILE_TIME_ASSERT( 240 == SO_SCREEN_WIDTH						, Screen_width_no_longer_the_same_as_in_hardcoded_assembly );
SO_COMPILE_TIME_ASSERT( (SO_FIXED_Q - 12) == SO_ONE_OVER_N_INDEX_Q	, SO_ONE_OVER_N_INDEX_Q_no_longer_compatible_with_hardcoded_assembly );
SO_COMPILE_TIME_ASSERT( 4 == SO_ONE_OVER_N_INDEX_Q					, SO_ONE_OVER_N_INDEX_Q_no_longer_the_same_as_in_hardcoded_assembly );
SO_COMPILE_TIME_ASSERT( 14 == SO_NUM_INTERRUPT_TYPES				, SO_NUM_INTERRUPT_TYPES_no_longer_the_same_as_in_hardcoded_assembly );
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
