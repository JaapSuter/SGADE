// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoPolygon.h
	\author		Jaap Suter
	\date		June 20 2001
	\ingroup	SoPolygon

	See the \a SoPolygon module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_POLYGON_H
#define SO_POLYGON_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoPolygon SoPolygon
	\brief	  Module representing a polygon

	This module contains a structure and methods for dealing with polygons. The 
	\a SoMesh structure contains a list of these polygons.
	
*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoVector.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

/*!
	\brief Maximum number of vertices in a polygon.

	This is used by the \a SoCamera module in the clipping routine. We use a
	fixed size array to hold clipped vertices, and make sure we never exceed
	this maximum number. Which shouldn't be much of a problem, since you should
	be using triangles anyway.
*/
#define SO_POLYGON_MAX_NUM_VERTICES 8

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------


/*!
	\brief Polygon structure definition.

	Note that all its attributes are private. Use the \a SoPolygon methods to
	act upon these attributes.
*/
typedef struct 
{
	u8          m_NumVertices;			//!< \internal Number of vertices in the polygon
	u16*        m_VertexIndices;		//!< \internal Pointer to an array of indices into an array 
										//!<		   of vertices (often located in the \a SoMesh object).
	u8          m_PaletteIndex;			//!< \internal Palette index of the polygon.
	SoVector2*  m_TextureCoordinates;	//!< \internal Array of texture coordinates of the polygon.

} SoPolygon;

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

SoVector2* SoPolygonGetTextureCoordinates( SoPolygon* a_This );
u32		   SoPolygonGetVertexIndex(		   SoPolygon* a_This, u32 a_Index );
u32		   SoPolygonGetNumVertices(		   SoPolygon* a_This );
u32		   SoPolygonGetPaletteIndex(	   SoPolygon* a_This );

void	   SoPolygonSetPaletteIndex(	   SoPolygon* a_This, u32 a_PaletteIndex );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
