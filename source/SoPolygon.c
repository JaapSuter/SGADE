// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoPolygon.c
	\author		Jaap Suter
	\date		June 20 2001
	\ingroup	SoPolygon

	See the \a SoPolygon module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoPolygon.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief To get the array of texture coordinates of the polygon.

	\param	a_This This pointer
	\return	Array of texture coordinates of the polygon

	This method can return NULL if the polygon has no texture coordinates.
*/
// ----------------------------------------------------------------------------
SoVector2* SoPolygonGetTextureCoordinates( SoPolygon* a_This ) 
{ 
	return a_This->m_TextureCoordinates; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief To get a single vertex from the polygon.

	\param	a_This	This pointer
	\param  a_Index Index of the requested vertex, in the range [0..\a SoPolygonGetNumVertices - 1].
	\return The requested vertex.

*/
// ----------------------------------------------------------------------------
u32	SoPolygonGetVertexIndex( SoPolygon* a_This, u32 a_Index ) 
{ 
	// Assert the input.
	SO_ASSERT( a_Index < a_This->m_NumVertices, "Index out of bounds." );

	// Assert the object state.
	SO_ASSERT( a_This->m_VertexIndices != NULL, "This polygon contains no vertices." );

	// Return;
	return a_This->m_VertexIndices[ a_Index ]; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief To get the number of vertices in the polygon.

	\param	a_This	This pointer
	\return Number of vertices in this polygon
*/
// ----------------------------------------------------------------------------
u32 SoPolygonGetNumVertices( SoPolygon* a_This ) 
{ 
	return (u32) a_This->m_NumVertices; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief To get the color of the polygon.

	\param	a_This	This pointer

	\return The palette index of the polygon.
*/
// ----------------------------------------------------------------------------
u32 SoPolygonGetPaletteIndex( SoPolygon* a_This ) 
{ 
	return a_This->m_PaletteIndex; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief To set the color of the polygon.
	
	\param	a_This			This pointer
	\param	a_PaletteIndex	Palette index you want the polygon to use.

*/
// ----------------------------------------------------------------------------
void SoPolygonSetPaletteIndex( SoPolygon* a_This, u32 a_PaletteIndex )
{ 
	a_This->m_PaletteIndex = a_PaletteIndex; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
