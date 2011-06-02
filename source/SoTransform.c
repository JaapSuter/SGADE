// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTransform.c
	\author		Jaap Suter, Gabriele Scibilia
	\date		June 28 2001
	\ingroup	SoTransform

	Implementation of SoTransform.h.
*/
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoTransform.h"
#include "SoTables.h"
#include "SoMath.h"

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Resets the transformation.

	Sets the transformation to the identity-transform. Sets the translation 
	part to zero. Sets the rotational part to zero, and sets the uniform scale to one. 
*/
// ----------------------------------------------------------------------------
void SoTransformMakeIdentity( 
					  SoTransform* a_This //!< This pointer
					  )
{
	a_This->m_AngleX = 0;
	a_This->m_AngleY = 0;
	a_This->m_AngleZ = 0;
	a_This->m_Translate.m_X = 0;
	a_This->m_Translate.m_Y = 0;
	a_This->m_Translate.m_Z = 0;
	a_This->m_Scale.m_X = SO_FIXED_FROM_WHOLE( 1 );
	a_This->m_Scale.m_Y = SO_FIXED_FROM_WHOLE( 1 );
	a_This->m_Scale.m_Z = SO_FIXED_FROM_WHOLE( 1 );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!

	\brief Sets the rotation angle around the X-axis.
*/
// ----------------------------------------------------------------------------
void SoTransformSetRotateX(	
						   SoTransform* a_This, //!< This pointer
						   s32 a_Angle			//!< Angle. A Full circle is 256 degrees
						   )
{
	// Set the angle;
	a_This->m_AngleX = a_Angle;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the rotation angle around the Y-axis.
*/
// ----------------------------------------------------------------------------
void SoTransformSetRotateY(	
						   SoTransform* a_This, //!< This pointer
						   s32 a_Angle			//!< Angle. A Full circle is 256 degrees
						   )
{
	// Set the angle;
	a_This->m_AngleY = a_Angle;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the rotation angle around the Z-axis.
*/
// ----------------------------------------------------------------------------
void SoTransformSetRotateZ(	
						   SoTransform* a_This, //!< This pointer
						   s32 a_Angle			//!< Angle. A Full circle is 256 degrees
						   )
{
	// Set the angle;
	a_This->m_AngleZ = a_Angle;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Rotates the transform around the X-axis.

	The difference between this one and \a SetTransformRotateX is that this one 
	adds the angle to the existing angle.
*/
// ----------------------------------------------------------------------------
void SoTransformRotateX(
						SoTransform* a_This,	//!< This pointer
						s32 a_Angle				//!< Angle. A Full circle is 256 degrees 
						)
{
	// Add the angle;
	a_This->m_AngleX += a_Angle;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Rotates the transform around the Y-axis.

 	The difference between this one and \a SetTransformRotateY is that this one 
	adds the angle to the existing angle.
*/
// ----------------------------------------------------------------------------
void SoTransformRotateY(	
						SoTransform* a_This, //!< This pointer
						s32 a_Angle			//!< Angle. A Full circle is 256 degrees
						)
{
	// Add the angle;
	a_This->m_AngleY += a_Angle;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Rotates the transform around the Z-axis.

	The difference between this one and \a SetTransformRotateZ is that this one 
	adds the angle to the existing angle.
*/
// ----------------------------------------------------------------------------
void SoTransformRotateZ( 
						SoTransform* a_This,	//!< This pointer
						s32 a_Angle				//!< Angle. A Full circle is 256 degrees
						)
{
	// Add the angle;
	a_This->m_AngleZ += a_Angle;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
  \brief Sets the translational part of the transform.
*/
// ----------------------------------------------------------------------------
void SoTransformSetTranslation( 
							 SoTransform* a_This,	//!< This pointer
							 sofixedpoint a_X,		//!< Fixed point X part of the translate
							 sofixedpoint a_Y,		//!< Fixed point Y part of the translate
							 sofixedpoint a_Z		//!< Fixed point Z part of the translate
							 )
{
	a_This->m_Translate.m_X = a_X;
	a_This->m_Translate.m_Y = a_Y;
	a_This->m_Translate.m_Z = a_Z;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Translates the transform

	The difference between this one and \a SoTransformSetTranslate is that in this
	Method the translation is added to the existing transformation.
*/
// ----------------------------------------------------------------------------
void SoTransformTranslate( 
						  SoTransform* a_This,	//!< This pointer
						  sofixedpoint a_X,		//!< Fixed point X part of the translate
						  sofixedpoint a_Y,		//!< Fixed point Y part of the translate
						  sofixedpoint a_Z		//!< Fixed point Z part of the translate
						  ) 
{
	a_This->m_Translate.m_X += a_X;
	a_This->m_Translate.m_Y += a_Y;
	a_This->m_Translate.m_Z += a_Z;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the uniform scale value
*/
// ----------------------------------------------------------------------------
void SoTransformSetScale( 
						 SoTransform* a_This,	//!< This pointer
						 sofixedpoint a_ScaleX,	//!< Fixed point scale factor. X component.
						 sofixedpoint a_ScaleY,	//!< Fixed point scale factor. Y component.
						 sofixedpoint a_ScaleZ	//!< Fixed point scale factor. Z component.
						 )
{
	a_This->m_Scale.m_X = a_ScaleX;
	a_This->m_Scale.m_Y = a_ScaleY;
	a_This->m_Scale.m_Z = a_ScaleZ;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns a 3 by 4 matrix representation of the transform.

	\todo	Optimize this method
*/
// ----------------------------------------------------------------------------
void SoTransformToMatrix( 
						 SoTransform* a_This,	//!< This pointer
						 SoMatrix* a_Matrix		//!< [OUT] Resulting matrix
						 )
{
	// Helper intermediate matrices;
	SoMatrix matrixRotateX;
	SoMatrix matrixRotateY;
	SoMatrix matrixRotateZ;

	// Create an x- and y-rotation matrix;
	SoMatrixMakeRotationX( &matrixRotateX, a_This->m_AngleX );
	SoMatrixMakeRotationY( &matrixRotateY, a_This->m_AngleY );

	// Multiply them into the return value;
	SoMatrixMultiply( a_Matrix, &matrixRotateX, &matrixRotateY );
	
	// Add the z-rotation;
	SoMatrixMakeRotationZ( &matrixRotateZ, a_This->m_AngleZ );
	SoMatrixMultiplyBy( a_Matrix, &matrixRotateZ );

	// Set the translate in it;
	SoMatrixSetTranslation( a_Matrix, a_This->m_Translate.m_X,
									  a_This->m_Translate.m_Y, 
									  a_This->m_Translate.m_Z );

	// Scale the matrix, even if the scale factor is one;
	SoMatrixScale( a_Matrix, a_This->m_Scale.m_X,
							 a_This->m_Scale.m_Y,
							 a_This->m_Scale.m_Z );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns a 3 by 4 matrix representation of the inverse of this transform.

	\todo	Optimize this method
*/
// ----------------------------------------------------------------------------
void SoTransformToInverseMatrix( 
								SoTransform* a_This,	//!< This pointer
								SoMatrix* a_Matrix		//!< [OUT] Resulting matrix
								)
{
	// Helper intermediate matrices;
	SoMatrix matrixRotateX;
	SoMatrix matrixRotateY;
	SoMatrix matrixRotateZ;

	// Reset the incoming matrix;
	SoMatrixMakeIdentity( a_Matrix );

	// Create the x-axis matrix;
	if ( a_This->m_AngleX != 0 )
		SoMatrixMakeRotationX( &matrixRotateX, 360 - a_This->m_AngleX );
	else
		SoMatrixMakeIdentity( &matrixRotateX );
	
	// Create the y-axis matrix;
	if ( a_This->m_AngleY != 0 )
		SoMatrixMakeRotationY( &matrixRotateY, 360 - a_This->m_AngleY );
	else
		SoMatrixMakeIdentity( &matrixRotateY );

	// Create the z-axis matrix;
	if ( a_This->m_AngleZ != 0 )
		SoMatrixMakeRotationZ( &matrixRotateZ, 360 - a_This->m_AngleZ );
	else
		SoMatrixMakeIdentity( &matrixRotateZ );	
	
	// Combine 'm all;
	SoMatrixMultiplyBy( a_Matrix, &matrixRotateX );
	SoMatrixMultiplyBy( a_Matrix, &matrixRotateY );
	SoMatrixMultiplyBy( a_Matrix, &matrixRotateZ );

	// Set the translate in it;
	SoMatrixSetTranslation( a_Matrix, -a_This->m_Translate.m_X,
									  -a_This->m_Translate.m_Y, 
									  -a_This->m_Translate.m_Z );

	// Inverse scale the matrix, even if the scale factor is one;
	SoMatrixScale( a_Matrix, SO_FIXED_ONE_OVER_SLOW_ACCURATE( a_This->m_Scale.m_X ),
							 SO_FIXED_ONE_OVER_SLOW_ACCURATE( a_This->m_Scale.m_Y ),
							 SO_FIXED_ONE_OVER_SLOW_ACCURATE( a_This->m_Scale.m_Z ) );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
