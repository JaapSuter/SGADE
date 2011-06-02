// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMatrix.h
	\author		Jaap Suter, Gabriele Scibilia
	\date		Jun 26 2001
	\ingroup	SoMatrix

	See the \a SoMatrix module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_MATRIX_H
#define SO_MATRIX_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoMatrix SoMatrix
	\brief	  Matrix structure definition
	
	This module contains the matrix definition and all its methods.
	
	To save memory we use a 3 row, 4 column matrix system. The missing 4th row
	is assumed to contain [0, 0, 0, 1]. This works because we only support
	homogeneous transforms (rotations, translations and uniform scales).

	All cells in a matrix are in fixed point format.

	Note that you rarely need to work with matrices. You should use the
	\a SoTransform structure instead, because it is better in dealing with
	accumulating precision problems.  Even in the best case, if you multiply
	two orthogonal matrices a few hundred times, you can be sure they won't be
	orthogonal anymore.  This is even worse with a fixed-point number system as
	the errors crop up much faster.
	
*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

/*!
	\brief Matrix definition

	This structure represents a 3 by 4 matrix consisting of fixed point
	elements.

	See the \a SoMatrix module for more information.
*/
typedef struct
{
	// Private cels;
	sofixedpoint m_C[ 12 ];		//!< \internal	Array of cells of the matrix.

} SoMatrix;

// --------------------------------------------------------------------------
// Public methods;
// --------------------------------------------------------------------------
void SoMatrixMakeIdentity(SoMatrix* a_This );
void SoMatrixMakeRotationX(SoMatrix* a_This, s32 a_Angle );
void SoMatrixMakeRotationY(SoMatrix* a_This, s32 a_Angle );
void SoMatrixMakeRotationZ(SoMatrix* a_This, s32 a_Angle );

void SoMatrixSetTranslation(SoMatrix* a_This, sofixedpoint a_X, sofixedpoint a_Y, sofixedpoint a_Z );

void SoMatrixScale(SoMatrix* a_This, sofixedpoint a_ScaleX, sofixedpoint a_ScaleY, sofixedpoint a_ScaleZ );

void SoMatrixMultiplyBy(SoMatrix* a_This, SoMatrix* a_Matrix );
void SoMatrixMultiply(SoMatrix* a_This, SoMatrix* a_A, SoMatrix* a_B );

void SoMatrixMode4DebugDraw(SoMatrix* a_This );

// --------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
