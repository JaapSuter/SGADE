// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMesh.h
	\author		Jaap Suter
	\date		Jun 10 2001
	\ingroup	SoMesh

	See the \a SoMesh module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_MESH_H
#define SO_MESH_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoMesh SoMesh
	\brief	  Mesh representation
	
	This module represents 3D meshes. A mesh is an object consisting of 
	several polygons, ready to be rendered by a camera.
	
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoImage.h"
#include "SoPolygon.h"
#include "SoTransform.h"

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------

/*! 
	\brief Defines the maximum number of vertices a mesh can contain. 
	
	If this is not enough, you might want to increase it, but remember this is
	a GBA we're talking about. You might even want to scale it down a bit if
	you only use fairly simple meshes.

	This value is used by the camera module to know the maximum size for the
	vertex buffers. 
*/
#define SO_MESH_MAX_NUM_VERTICES 512

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

/*!
	\brief Mesh definition

	Defines a mesh that can be given to a \a SoCamera instance for rendering.
	Check the attributes and the methods for more information. However, these
	attributes are private. Under normal circumstances you'll only need the
	\a SoMesh methods. 

	See the \a SoMesh module for more information.
*/
typedef struct 
{
	// Private Attributes;
	u32          m_NumVertices; //!< \internal Number of vertices in the mesh.
	SoPolygon*   m_Polygons;    //!< \internal Array of polygons in the mesh.
	u32          m_NumPolygons; //!< \internal Number of polygons in the mesh.
	SoImage*     m_Texture;     //!< \internal Pointer to texture. Null if the mesh has no texture.
	SoTransform  m_Transform;   //!< \internal Current transform of the mesh.

	/*! 
		\internal 
		
		\brief	Array of vertices in the mesh.

		Yes, I know I'm using \a SoVector3 to represent vertices. If 
		you have a problem with that, email me and we'll sort it out, :) hehe.
		Mathematical correctness is only so good as long as it's usefull.
		Use the following code if you really want it the mathematically correct 
		way. \code typedef SoVertex SoVector3; \endcode 
	*/
	SoVector3*		m_Vertices;			


} SoMesh;

// ----------------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------------

void		 SoMeshSetTexture(		SoMesh* a_This, SoImage* a_Texture );

SoImage*	 SoMeshGetTexture(		SoMesh* a_This );
u32			 SoMeshGetNumVertices(  SoMesh* a_This );
u32			 SoMeshGetNumPolygons(  SoMesh* a_This );
SoVector3*	 SoMeshGetVertex(		SoMesh* a_This, u32 a_Index );
SoPolygon*	 SoMeshGetPolygon(		SoMesh* a_This, u32 a_Index );

SoTransform* SoMeshGetTransform(	SoMesh* a_This );

void		 SoMeshMakeDefaultCube(	SoMesh* a_This );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
