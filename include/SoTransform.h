// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTransform.h
	\author		Jaap Suter, Gabriele Scibilia
	\date		June 28 2001
	\ingroup	SoTransform
*/
// ----------------------------------------------------------------------------

#ifndef SO_TRANSFORM_H
#define SO_TRANSFORM_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoTransform SoTransform 
	\brief	  Structure to represent transforms.

  	This module contains the definition of a transformation. A transformation
	can be represented by a matrix as well, but a matrix is more sensitive
	for accumulating errors, especially because we are using a low-precision
	fixed point format. That's why you should use this transformation class and
	not convert it to a matrix until the very last moment.

	Note that the transform separately stores its translation, scaling, and
	rotation and they do not interact with each other.  When constructing a
	matrix from a transform, we always composite them in the order X rotation,
	Y rotation, Z rotation, translation, scale.  For this reason, transforms
	are not ideal for use in hierarchical scene graphs.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoVector.h"
#include "SoMatrix.h"

// ----------------------------------------------------------------------------
// Type definition;
// ----------------------------------------------------------------------------

/*!
	\brief Transformation class definition

	This structure is used to define transformations. It contains a translation
	part, a rotation part and a non-uniform scaling part.

	See the \a SoTransform module for more information.
*/
typedef struct
{
	//! 3 dimensional translational part
	SoVector3	m_Translate;
	
	s32			m_AngleX;	//!< Angle around the X-axis. A full circle is 256 degrees.
	s32			m_AngleY;	//!< Angle around the Y-axis. A full circle is 256 degrees.
	s32			m_AngleZ;	//!< Angle around the Z-axis. A full circle is 256 degrees.
	
	//! Fixed point non-uniform scaling vector.
	SoVector3	m_Scale;
	
} SoTransform;

// --------------------------------------------------------------------------
// Public methods;
// --------------------------------------------------------------------------

void SoTransformMakeIdentity(	 SoTransform* a_This );

void SoTransformSetRotateX(		 SoTransform* a_This, s32 a_Angle );
void SoTransformSetRotateY(		 SoTransform* a_This, s32 a_Angle );
void SoTransformSetRotateZ(		 SoTransform* a_This, s32 a_Angle );

void SoTransformSetTranslation(	 SoTransform* a_This, sofixedpoint a_X, sofixedpoint a_Y, sofixedpoint a_Z );

void SoTransformRotateX(		 SoTransform* a_This, s32 a_Angle );
void SoTransformRotateY(		 SoTransform* a_This, s32 a_Angle );
void SoTransformRotateZ(		 SoTransform* a_This, s32 a_Angle );

void SoTransformTranslate(		 SoTransform* a_This, sofixedpoint a_X, sofixedpoint a_Y, sofixedpoint a_Z );

void SoTransformToMatrix(		 SoTransform* a_This, SoMatrix* a_Matrix );
void SoTransformToInverseMatrix( SoTransform* a_This, SoMatrix* a_Matrix );

void SoTransformSetScale(		 SoTransform* a_This, sofixedpoint a_ScaleX, sofixedpoint a_ScaleY, sofixedpoint a_ScaleZ );

// --------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
