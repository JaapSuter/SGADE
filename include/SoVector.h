// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoVector.h
	\author		Jaap Suter
	\date		June 5 2001
	\ingroup	SoVector
*/
// ----------------------------------------------------------------------------

#ifndef SO_VECTOR_H
#define SO_VECTOR_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoVector SoVector
	\brief	  Structures that represent 2D and 3D vectors.

	This module contains the definitions of the 2D and 3D vector 
	classes. Components of these vectors are in a fixed point format. 

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoMatrix.h"

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

/*!
	\brief		2D vector definition

	This structure is used to define 2D vectors. Its members are 
	public.
	
	The members can be used for either fixed point or integer whole 
	mathematics, depending on the context.

	See the \a SoVector module for more information.
*/
typedef struct 
{

	s32 m_X;	//!< X component.
	s32 m_Y;	//!< Y component.

} SoVector2;


/*!
	\brief		3D vector definition

	This structure is used to define 3D vectors.

    The members can be used for either fixed point or integer whole 
	mathematics, depending on the context.

	See the \a SoVector module for more information.
*/
typedef struct 
{
	s32 m_X;	//!< X component.
	s32 m_Y;	//!< Y component.
	s32 m_Z;	//!< Z component.

} SoVector3;

// ----------------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------------

void SoVector3TransformInto( SoVector3* a_This, SoVector3* a_Target, SoMatrix* a_Matrix );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
