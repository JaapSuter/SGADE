// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMesh.c
	\author		Jaap Suter
	\date		Jun 10 2001
	\ingroup	SoMesh

	See the \a SoMesh module for more information.
*/
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoMesh.h"
#include "SoMath.h"
#include "SoDebug.h"


// ----------------------------------------------------------------------------
/*!
	\brief Returns the texture this mesh is using.
	
	\param a_This	This pointer.

	\return Texture the mesh is using.

	Returns \a NULL if the mesh has no texture.
*/
// ----------------------------------------------------------------------------
SoImage* SoMeshGetTexture( SoMesh* a_This ) 
{ 
	return a_This->m_Texture; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the number of vertices in the mesh.

	\param	a_This This pointer
	
	\return Number of vertices in the mesh.

	\todo Inline this function.

	If you receive a value of 0, then you've got a problem. Or you got a mesh with 
	nothing in it. Or maybe you got some sort of disease, or maybe you got a whole 
	lot of money,or maybe I should shut up now and leave the humour out of this reference.
	Sorry, couldn't resist. I mean, documenting these self-evident is pretty annoying 
	anyway, but done for the sake of consistency.
*/
// ----------------------------------------------------------------------------
u32 SoMeshGetNumVertices(  SoMesh* a_This ) 
{ 
	return a_This->m_NumVertices; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returs the number of polygons in the mesh.

	\param a_This This pointer
	
	\return Number of polygons in the mesh.

	\todo Inline this function.
*/
// ----------------------------------------------------------------------------
u32 SoMeshGetNumPolygons(  SoMesh* a_This ) 
{ 
	return a_This->m_NumPolygons; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns a single vertex of the mesh.

	\param a_This	This pointer
	\param a_Index	Index of the vertex.

	\return pointer to the vertex you requested.

	\todo Inline this function.

	In release mode, no checks are made whether the given \a a_Index is a valid index.
	In debug mode (when \a SO_DEBUG is defined) this is asserted (using the \a SO_ASSERT macro).
*/
// ----------------------------------------------------------------------------
SoVector3* SoMeshGetVertex( SoMesh* a_This, u32 a_Index ) 
{ 
	SO_ASSERT( a_Index < a_This->m_NumVertices, "Index out of bounds." );
	return &a_This->m_Vertices[ a_Index ]; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns a single vertex of the mesh.

	\param a_This	This pointer
	\param a_Index	Index of the polygon.

	\return Pointer to the polygon you requested.

	\todo Inline this function.

	In release mode, no checks are made whether the given \a a_Index is a valid index.
	In debug mode (when \a SO_DEBUG is defined) this is asserted (using the \a SO_ASSERT macro).
*/
// ----------------------------------------------------------------------------
SoPolygon* SoMeshGetPolygon( SoMesh* a_This, u32 a_Index ) 
{ 
	SO_ASSERT( a_Index < a_This->m_NumPolygons, "Index out of bounds." );
	return &a_This->m_Polygons[ a_Index ]; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the transform of the mesh.

	\param a_This	This pointer

	\return Pointer to the transform the mesh uses.

	\todo Inline this function.

	If you want to transform (translate, rotate, scale) a mesh, use this method to
	obtain the transform, and manipulate that transform.
*/
// ----------------------------------------------------------------------------
SoTransform* SoMeshGetTransform( SoMesh* a_This ) 
{ 
	return &a_This->m_Transform; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the texture of the mesh.
	
	\param	a_This		This pointer
	\param	a_Texture	Texture you want this mesh to use. Use NULL for no texture.

	\todo Inline this function.
*/
// ----------------------------------------------------------------------------
void SoMeshSetTexture( SoMesh* a_This, SoImage* a_Texture ) 
{ 
	a_This->m_Texture = a_Texture; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
