// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoVector.c
	\author		Jaap Suter
	\date		June 27 2001
	\ingroup	SoVector

	See the documentation on SoVector.h for more information.  
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoVector.h"
#include "SoMath.h"

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief		Transforms a fixed point 3D vector into another through matrix multiplication.

	\param	a_This		Vector that is to be transformed.
	\param	a_Target	Vector that the result can be placed into.
	\param	a_Matrix	Matrix that is used to multiply.

	Matrix multiplication is done by using \a a_Target = \a a_Matrix * \a a_This and assuming
	that the 3D vectors are actually 4D vectors with their 4th component set to 1.0 (homogenous 
	coordinates).
*/
// ----------------------------------------------------------------------------
void SoVector3TransformInto( SoVector3* a_This,	SoVector3* a_Target, SoMatrix* a_Matrix	)
{
	// Perform the calculation;
	a_Target->m_X = SO_FIXED_MULTIPLY( a_This->m_X, a_Matrix->m_C[  0 ] ) + 
					SO_FIXED_MULTIPLY( a_This->m_Y, a_Matrix->m_C[  1 ] ) + 
					SO_FIXED_MULTIPLY( a_This->m_Z, a_Matrix->m_C[  2 ] ) +
					a_Matrix->m_C[  3 ];
	a_Target->m_Y = SO_FIXED_MULTIPLY( a_This->m_X, a_Matrix->m_C[  4 ] ) + 
					SO_FIXED_MULTIPLY( a_This->m_Y, a_Matrix->m_C[  5 ] ) + 
					SO_FIXED_MULTIPLY( a_This->m_Z, a_Matrix->m_C[  6 ] ) +
					a_Matrix->m_C[  7 ];
	a_Target->m_Z = SO_FIXED_MULTIPLY( a_This->m_X, a_Matrix->m_C[  8 ] ) + 
					SO_FIXED_MULTIPLY( a_This->m_Y, a_Matrix->m_C[  9 ] ) + 
					SO_FIXED_MULTIPLY( a_This->m_Z, a_Matrix->m_C[ 10 ] ) +
					a_Matrix->m_C[ 11 ];
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
