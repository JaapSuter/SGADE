// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMeshCube.c
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
// Default cube definition;
// ----------------------------------------------------------------------------
/*!
	\name Default Cube Mesh

	The following constants define a default cube you could use for testing purposes.
	I used it because I hadn't written a 3d object importer yet, and I needed some test 
	meshes. I decided to leave it in here, cause you never know when it might come in 
	handy again.

	It was extracted into a separate source file to ensure that it won't be linked into
	your application unless you explicitly use it.

*///@{   
// ----------------------------------------------------------------------------


//! Size of the default cube divided by two.
#define SO_CUBE_HALF_SIZE		SO_FIXED_FROM_WHOLE( 10 )

//! Size of the default cubes texture coordinates.
#define SO_CUBE_TEX_COORD_SIZE	SO_FIXED_FROM_WHOLE( 1 )

//! Vertices of the default cube.
static SoVector3 s_CubeVertices[ 8 ] = {	
									{ -SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE },
									{  SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE },
									{  SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE },
									{ -SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE },
									{ -SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE },
									{  SO_CUBE_HALF_SIZE, -SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE },
									{  SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE },
									{ -SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE,  SO_CUBE_HALF_SIZE }
								};

//! Texture coordinates of each vertex.
static SoVector2 s_CubeTxCoords[ 8 ] = { 
										{ 0,					  0 },
										{ SO_CUBE_TEX_COORD_SIZE, 0 },
										{ SO_CUBE_TEX_COORD_SIZE, SO_CUBE_TEX_COORD_SIZE },
										{ 0,					  SO_CUBE_TEX_COORD_SIZE }
										
									};

static u16 s_CubeTriangleIndex00[ 3 ] = { 0, 3, 2 }; //!< Vertex-index array for triangle  0 of the default cube;
static u16 s_CubeTriangleIndex01[ 3 ] = { 0, 2, 1 }; //!< Vertex-index array for triangle  1 of the default cube;
static u16 s_CubeTriangleIndex02[ 3 ] = { 0, 1, 5 }; //!< Vertex-index array for triangle  2 of the default cube;
static u16 s_CubeTriangleIndex03[ 3 ] = { 0, 5, 4 }; //!< Vertex-index array for triangle  3 of the default cube;
static u16 s_CubeTriangleIndex04[ 3 ] = { 1, 2, 6 }; //!< Vertex-index array for triangle  4 of the default cube;
static u16 s_CubeTriangleIndex05[ 3 ] = { 1, 6, 5 }; //!< Vertex-index array for triangle  5 of the default cube;
static u16 s_CubeTriangleIndex06[ 3 ] = { 2, 3, 7 }; //!< Vertex-index array for triangle  6 of the default cube;
static u16 s_CubeTriangleIndex07[ 3 ] = { 2, 7, 6 }; //!< Vertex-index array for triangle  7 of the default cube;
static u16 s_CubeTriangleIndex08[ 3 ] = { 3, 0, 4 }; //!< Vertex-index array for triangle  8 of the default cube;
static u16 s_CubeTriangleIndex09[ 3 ] = { 3, 4, 7 }; //!< Vertex-index array for triangle  9 of the default cube;
static u16 s_CubeTriangleIndex10[ 3 ] = { 4, 5, 6 }; //!< Vertex-index array for triangle 10 of the default cube;
static u16 s_CubeTriangleIndex11[ 3 ] = { 4, 6, 7 }; //!< Vertex-index array for triangle 11 of the default cube;


//! Each triangle of the cube.
static SoPolygon s_CubeTriangles[ 12 ] = { 
									{ 3, (u16*) s_CubeTriangleIndex00, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex01, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex02, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex03, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex04, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex05, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex06, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex07, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex08, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex09, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex10, 0, (SoVector2*) s_CubeTxCoords },
									{ 3, (u16*) s_CubeTriangleIndex11, 0, (SoVector2*) s_CubeTxCoords },
								};
//@}
// ----------------------------------------------------------------------------
// End of default cube definition
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Makes the mesh equal to a default cube.
	
	\ingroup Default Cube Mesh

	\param a_This	This pointer.

	This makes the mesh represent a default cube, located at the origin (0, 0, 0) with
	size 2 times \a SO_CUBE_HALF_SIZE. It will consist of 12 triangles, 8 vertices, and 
	has texture coordinates too. 
	
	Check the documentation of the \a SoMeshCube.c file for more information on this default cube.

	Mainly used for testing purposes.
*/
// ----------------------------------------------------------------------------
void SoMeshMakeDefaultCube( SoMesh* a_This )
{
	a_This->m_NumVertices = 8;
	a_This->m_NumPolygons = 12;	
	a_This->m_Polygons = (SoPolygon*) s_CubeTriangles;
	a_This->m_Vertices = (SoVector3*) s_CubeVertices;

	SoTransformMakeIdentity( &a_This->m_Transform );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
