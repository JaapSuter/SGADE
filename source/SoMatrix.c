// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMatrix.c
	\author		Jaap Suter, Gabriele Scibilia
	\date		Jun 26 2001
	\ingroup	SoMatrix

	See the \a SoMatrix module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoMatrix.h"
#include "SoTables.h"
#include "SoMode4Renderer.h"
#include "SoMath.h"

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Makes the given matrix an identity matrix.
	
	\param a_This	This pointer
*/
// ----------------------------------------------------------------------------
void SoMatrixMakeIdentity( SoMatrix* a_This )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Do it;
	cel[  0 ] = SO_FIXED_FROM_WHOLE( 1 ); 
	cel[  1 ] = 0; 
	cel[  2 ] = 0; 
	cel[  3 ] = 0;
	
	cel[  4 ] = 0; 
	cel[  5 ] = SO_FIXED_FROM_WHOLE( 1 ); 
	cel[  6 ] = 0; 
	cel[  7 ] = 0;
	
	cel[  8 ] = 0; 
	cel[  9 ] = 0; 
	cel[ 10 ] = SO_FIXED_FROM_WHOLE( 1 ); 
	cel[ 11 ] = 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Makes the given matrix an rotational matrix around the X-axis.
	
	\param	a_This		This pointer
	\param	a_Angle		Angle. A full circle is 256 degrees.
*/
// ----------------------------------------------------------------------------
void SoMatrixMakeRotationX( SoMatrix* a_This, s32 a_Angle )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Do it;
	cel[  0 ] = SO_FIXED_FROM_WHOLE( 1 ); 
	cel[  1 ] = 0; 
	cel[  2 ] = 0; 
	cel[  3 ] = 0;
	
	cel[  4 ] = 0; 
	cel[  5 ] = SO_COSINE( a_Angle ); 
	cel[  6 ] = SO_SINE( a_Angle ); 
	cel[  7 ] = 0;
	
	cel[  8 ] = 0; 
	cel[  9 ] = -SO_SINE(  a_Angle ); 
	cel[ 10 ] = SO_COSINE( a_Angle ); 
	cel[ 11 ] = 0;	
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Makes the given matrix an rotational matrix around the Y-axis.
	
	\param	a_This		This pointer
	\param	a_Angle		Angle. A full circle is 256 degrees.
*/
// ----------------------------------------------------------------------------
void SoMatrixMakeRotationY(  SoMatrix* a_This, s32 a_Angle )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Do it;
	cel[  0 ] = SO_COSINE( a_Angle );
	cel[  1 ] = 0; 
	cel[  2 ] = -SO_SINE( a_Angle );
	cel[  3 ] = 0;
	
	cel[  4 ] = 0; 
	cel[  5 ] = SO_FIXED_FROM_WHOLE( 1 ); 
	cel[  6 ] = 0; 
	cel[  7 ] = 0;
	
	cel[  8 ] = SO_SINE( a_Angle ); 
	cel[  9 ] = 0; 
	cel[ 10 ] = SO_COSINE( a_Angle );
	cel[ 11 ] = 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Makes the given matrix an rotational matrix around the Z-axis.
	
	\param	a_This		This pointer
	\param	a_Angle		Angle. A full circle is 256 degrees.
*/
// ----------------------------------------------------------------------------
void SoMatrixMakeRotationZ(  SoMatrix* a_This, s32 a_Angle )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Do it;
	cel[  0 ] = SO_COSINE( a_Angle );
	cel[  1 ] = SO_SINE( a_Angle );
	cel[  2 ] = 0; 
	cel[  3 ] = 0;
	
	cel[  4 ] = -SO_SINE( a_Angle );
	cel[  5 ] = SO_COSINE( a_Angle );
	cel[  6 ] = 0; 
	cel[  7 ] = 0;
	
	cel[  8 ] = 0; 
	cel[  9 ] = 0; 
	cel[ 10 ] = SO_FIXED_FROM_WHOLE( 1 ); 
	cel[ 11 ] = 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the translational part of the matrix.
	
	\param	a_This		This pointer
	\param	a_X			X part of the translation
	\param	a_Y			Y part of the translation
	\param	a_Z			Z part of the translation

	This function only sets the last column of the matrix (which represents the
	translation). It leaves the rest of the matrix intact.
*/
// ----------------------------------------------------------------------------
void SoMatrixSetTranslation( SoMatrix* a_This, sofixedpoint a_X, sofixedpoint a_Y, sofixedpoint a_Z )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Do it;
	cel[  3 ] = a_X; 
	cel[  7 ] = a_Y;
	cel[ 11 ] = a_Z;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Non-uniformly scales the matrix
	
	\param	a_This		This pointer
	\param	a_ScaleX	Fixed point scale value. X component.
	\param	a_ScaleY	Fixed point scale value. Y component.
	\param	a_ScaleZ	Fixed point scale value. Z component.
	
	This function scales the given matrix. It leaves any existing rotation and 
	translation in tact.

	\todo	Can this even work this simple? Test whether this works.
*/
// ----------------------------------------------------------------------------
void SoMatrixScale( SoMatrix* a_This, sofixedpoint a_ScaleX, sofixedpoint a_ScaleY, sofixedpoint a_ScaleZ )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Multiply the diagonal with the scale values;
	cel[  0 ] = SO_FIXED_MULTIPLY( cel[  0 ], a_ScaleX );
	cel[  5 ] = SO_FIXED_MULTIPLY( cel[  5 ], a_ScaleY );
	cel[ 10 ] = SO_FIXED_MULTIPLY( cel[ 10 ], a_ScaleZ );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Multiplies the matrix by another matrix
	
	\param	a_This		This pointer
	\param	a_Matrix	Matrix to multiply by.

	Performs the calculation \a a_This = \a a_This * \a a_Matrix.
	
	\todo Probably worth optimizing in assembly.
*/
// ----------------------------------------------------------------------------
void SoMatrixMultiplyBy( SoMatrix* a_This, SoMatrix* a_Matrix )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Use a temporary matrix to store the end result;
	SoMatrix m;

	// Pointer to second cel array;
	sofixedpoint* cel2 = a_Matrix->m_C;
		
	// Perform the calculation;

	// First row;
	m.m_C[  0 ] = SO_FIXED_MULTIPLY( cel[  0 ], cel2[  0 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  1 ], cel2[  4 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  2 ], cel2[  8 ] );
	m.m_C[  1 ] = SO_FIXED_MULTIPLY( cel[  0 ], cel2[  1 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  1 ], cel2[  5 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  2 ], cel2[  9 ] );
	m.m_C[  2 ] = SO_FIXED_MULTIPLY( cel[  0 ], cel2[  2 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  1 ], cel2[  6 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  2 ], cel2[ 10 ] );
	m.m_C[  3 ] = SO_FIXED_MULTIPLY( cel[  0 ], cel2[  3 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  1 ], cel2[  7 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  2 ], cel2[ 11 ] ) +
				  cel[  3 ];
	// Second row;
	m.m_C[  4 ] = SO_FIXED_MULTIPLY( cel[  4 ], cel2[  0 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  5 ], cel2[  4 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  6 ], cel2[  8 ] );
	m.m_C[  5 ] = SO_FIXED_MULTIPLY( cel[  4 ], cel2[  1 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  5 ], cel2[  5 ] ) +
				  SO_FIXED_MULTIPLY( cel[  6 ], cel2[  9 ] );
	m.m_C[  6 ] = SO_FIXED_MULTIPLY( cel[  4 ], cel2[  2 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  5 ], cel2[  6 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  6 ], cel2[ 10 ] );
	m.m_C[  7 ] = SO_FIXED_MULTIPLY( cel[  4 ], cel2[  3 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  5 ], cel2[  7 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  6 ], cel2[ 11 ] ) +
				  cel[  7 ];
	// Third row;
	m.m_C[  8 ] = SO_FIXED_MULTIPLY( cel[  8 ], cel2[  0 ] ) + 
			 	  SO_FIXED_MULTIPLY( cel[  9 ], cel2[  4 ] ) + 
		 		  SO_FIXED_MULTIPLY( cel[ 10 ], cel2[  8 ] );
	m.m_C[  9 ] = SO_FIXED_MULTIPLY( cel[  8 ], cel2[  1 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  9 ], cel2[  5 ] ) + 
				  SO_FIXED_MULTIPLY( cel[ 10 ], cel2[  9 ] );
	m.m_C[ 10 ] = SO_FIXED_MULTIPLY( cel[  8 ], cel2[  2 ] ) + 
				  SO_FIXED_MULTIPLY( cel[  9 ], cel2[  6 ] ) + 
				  SO_FIXED_MULTIPLY( cel[ 10 ], cel2[ 10 ] );
	m.m_C[ 11 ] = SO_FIXED_MULTIPLY( cel[  8 ], cel2[  3 ] ) + 
			 	  SO_FIXED_MULTIPLY( cel[  9 ], cel2[  7 ] ) + 
				  SO_FIXED_MULTIPLY( cel[ 10 ], cel2[ 11 ] ) +
				  cel[ 11 ];

	// Copy it back into this;
	cel[  0 ] = m.m_C[  0 ];
	cel[  1 ] = m.m_C[  1 ];
	cel[  2 ] = m.m_C[  2 ];
	cel[  3 ] = m.m_C[  3 ];
	cel[  4 ] = m.m_C[  4 ];
	cel[  5 ] = m.m_C[  5 ];
	cel[  6 ] = m.m_C[  6 ];
	cel[  7 ] = m.m_C[  7 ];
	cel[  8 ] = m.m_C[  8 ];
	cel[  9 ] = m.m_C[  9 ];
	cel[ 10 ] = m.m_C[ 10 ];
	cel[ 11 ] = m.m_C[ 11 ];
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Multiplies two matrices, placing the result into this.

	\param	a_This		This pointer
	\param	a_A			Matrix A
	\param	a_B			Matrix B

	Performs the calculation \a a_This = \a a_A * \a a_B.

	\todo Probably worth optimizing in assembly.
*/
// ----------------------------------------------------------------------------
void SoMatrixMultiply( SoMatrix* a_This, SoMatrix* a_A, SoMatrix* a_B )
{
	// Pointer to cel array;
	sofixedpoint* cel = a_This->m_C;

	// Get pointers to the matrices;
	sofixedpoint* celA = a_A->m_C;
	sofixedpoint* celB = a_B->m_C;
		
	// Perform the calculation;

	// First row;
	cel[  0 ] = SO_FIXED_MULTIPLY( celA[  0 ], celB[  0 ] ) + 
				SO_FIXED_MULTIPLY( celA[  1 ], celB[  4 ] ) + 
				SO_FIXED_MULTIPLY( celA[  2 ], celB[  8 ] );
	cel[  1 ] = SO_FIXED_MULTIPLY( celA[  0 ], celB[  1 ] ) + 
				SO_FIXED_MULTIPLY( celA[  1 ], celB[  5 ] ) + 
				SO_FIXED_MULTIPLY( celA[  2 ], celB[  9 ] );
	cel[  2 ] = SO_FIXED_MULTIPLY( celA[  0 ], celB[  2 ] ) + 
				SO_FIXED_MULTIPLY( celA[  1 ], celB[  6 ] ) + 
				SO_FIXED_MULTIPLY( celA[  2 ], celB[ 10 ] );
	cel[  3 ] = SO_FIXED_MULTIPLY( celA[  0 ], celB[  3 ] ) + 
				SO_FIXED_MULTIPLY( celA[  1 ], celB[  7 ] ) + 
				SO_FIXED_MULTIPLY( celA[  2 ], celB[ 11 ] ) +
				celA[  3 ];
	// Second row;
	cel[  4 ] = SO_FIXED_MULTIPLY( celA[  4 ], celB[  0 ] ) + 
				SO_FIXED_MULTIPLY( celA[  5 ], celB[  4 ] ) + 
				SO_FIXED_MULTIPLY( celA[  6 ], celB[  8 ] );
	cel[  5 ] = SO_FIXED_MULTIPLY( celA[  4 ], celB[  1 ] ) + 
				SO_FIXED_MULTIPLY( celA[  5 ], celB[  5 ] ) + 
				SO_FIXED_MULTIPLY( celA[  6 ], celB[  9 ] );
	cel[  6 ] = SO_FIXED_MULTIPLY( celA[  4 ], celB[  2 ] ) + 
				SO_FIXED_MULTIPLY( celA[  5 ], celB[  6 ] ) + 
				SO_FIXED_MULTIPLY( celA[  6 ], celB[ 10 ] );
	cel[  7 ] = SO_FIXED_MULTIPLY( celA[  4 ], celB[  3 ] ) + 
				SO_FIXED_MULTIPLY( celA[  5 ], celB[  7 ] ) + 
				SO_FIXED_MULTIPLY( celA[  6 ], celB[ 11 ] ) +
				celA[  7 ];
	// Third row;
	cel[  8 ] = SO_FIXED_MULTIPLY( celA[  8 ], celB[  0 ] ) + 
			 	SO_FIXED_MULTIPLY( celA[  9 ], celB[  4 ] ) + 
		 		SO_FIXED_MULTIPLY( celA[ 10 ], celB[  8 ] );
	cel[  9 ] = SO_FIXED_MULTIPLY( celA[  8 ], celB[  1 ] ) + 
				SO_FIXED_MULTIPLY( celA[  9 ], celB[  5 ] ) + 
				SO_FIXED_MULTIPLY( celA[ 10 ], celB[  9 ] );
	cel[ 10 ] = SO_FIXED_MULTIPLY( celA[  8 ], celB[  2 ] ) + 
				SO_FIXED_MULTIPLY( celA[  9 ], celB[  6 ] ) + 
				SO_FIXED_MULTIPLY( celA[ 10 ], celB[ 10 ] );
	cel[ 11 ] = SO_FIXED_MULTIPLY( celA[  8 ], celB[  3 ] ) + 
			 	SO_FIXED_MULTIPLY( celA[  9 ], celB[  7 ] ) + 
				SO_FIXED_MULTIPLY( celA[ 10 ], celB[ 11 ] ) +
				celA[ 11 ];
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Prints the matrix elements on the screen for debugging 
		   purposes. Only works in mode 4.

	\param	a_This		This pointer

	Simply prints the 12 elements on the screen in fixed point format. You have to do
	a fixed to float conversion yourself (if you want any information about wholes 
	and fractions).
	
	\warning	You should already be in mode 4 for this to work.
*/
// ----------------------------------------------------------------------------
void SoMatrixMode4DebugDraw( SoMatrix* a_This )
{
	SoMode4RendererDrawNumber(  10, 240, 10, a_This->m_C[  0 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber(  70, 240, 10, a_This->m_C[  1 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber( 130, 240, 10, a_This->m_C[  2 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber( 190, 240, 10, a_This->m_C[  3 ], SoFontGetDefaultFontImage() );

	SoMode4RendererDrawNumber(  10, 240, 20, a_This->m_C[  4 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber(  70, 240, 20, a_This->m_C[  5 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber( 130, 240, 20, a_This->m_C[  6 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber( 190, 240, 20, a_This->m_C[  7 ], SoFontGetDefaultFontImage() );

	SoMode4RendererDrawNumber(  10, 240, 30, a_This->m_C[  8 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber(  70, 240, 30, a_This->m_C[  9 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber( 130, 240, 30, a_This->m_C[ 10 ], SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber( 190, 240, 30, a_This->m_C[ 11 ], SoFontGetDefaultFontImage() );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
