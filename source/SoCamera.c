// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoCamera.c
	\author		Jaap Suter
	\date		Jun 26 2001
	\ingroup	SoCamera

	Implementation of SoCamera.h    
*/

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoCamera.h"
#include "SoSystem.h"
#include "SoMode4PolygonRasterizer.h"
#include "SoMode4Renderer.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

// The following defines must always be negative. See the SoCameraSafeProject method
// for more information;
#define SO_CAMERA_FRUSTUM_NEAR_PLANE	-1	//!< \internal Constant to uniquely identify the frustum near plane.
#define SO_CAMERA_FRUSTUM_FAR_PLANE		-2	//!< \internal Constant to uniquely identify the frustum far plane.
#define SO_CAMERA_FRUSTUM_LEFT_PLANE	-3	//!< \internal Constant to uniquely identify the frustum left plane.
#define SO_CAMERA_FRUSTUM_RIGHT_PLANE	-4	//!< \internal Constant to uniquely identify the frustum right plane.
#define SO_CAMERA_FRUSTUM_TOP_PLANE		-5	//!< \internal Constant to uniquely identify the frustum top plane.
#define SO_CAMERA_FRUSTUM_BOTTOM_PLANE	-6	//!< \internal Constant to uniquely identify the frustum bottom plane.

// ----------------------------------------------------------------------------
// Static variables
// ----------------------------------------------------------------------------

// Important note:
// These variables are shared by every instance of
// a camera. This means there are no guarantees about
// their contents inbetween public function calls.
// In other words, this means that they are only used inside 
// the SoCameraDrawMesh function and its entire child call graph.

//! \internal Buffer that is able to contain all of a mesh's vertices
//! transformed from object to camera space.
static SoVector3	s_CameraSpaceVertexBuffer[ SO_MESH_MAX_NUM_VERTICES ];


//! \internal Buffer that is able to contain all of a mesh's vertices
//! transformed from camera space to screen space.
static SoVector2	s_ScreenSpaceVertexBuffer[ SO_MESH_MAX_NUM_VERTICES ];

/*!

  \brief		Describes the current polygon that is about to be drawn.

  \internal 

  This struct is used to describe the polygon that is about to be rendered. It contains
  all the information to clip and render it.

  We are actually able to store two polygons in this struct. An original one, and 
  a clipped one. But after each clipping phase (against one frustumplane) we swap the
  data members so that the ones without the \a Clipped prefix are always the result. So the 
  members with the \a Clipped prefix are actually only used as intermediates.

*/
static struct 
{
	bool		m_HasTexture;					//!< Whether or not the polygon has a texture.
	
	u32			m_NumVertices;					//!< Number of vertices in the polygon. 
	
	SoVector2*	m_ScreenSpaceVertices;			//!< Screenspace coordinates of the polygon.
	SoVector3*	m_CameraSpaceVertices;			//!< Camera space coordinates of the polygon.
	SoVector2*	m_TextureCoordinates;			//!< Texture coordinates of the polygon.

	u32			m_ClippedNumVertices;			//!< \internal 

	SoVector2*	m_ClippedScreenSpaceVertices;	//!< \internal 
	SoVector3*	m_ClippedCameraSpaceVertices;	//!< \internal 
	SoVector2*	m_ClippedTextureCoordinates;	//!< \internal 

	SoVector2	m_ScreenSpaceArray0[ SO_POLYGON_MAX_NUM_VERTICES ]; //!< \internal 
	SoVector3	m_CameraSpaceArray0[ SO_POLYGON_MAX_NUM_VERTICES ]; //!< \internal 
	SoVector2	m_TextureCrdsArray0[ SO_POLYGON_MAX_NUM_VERTICES ]; //!< \internal 

	SoVector2	m_ScreenSpaceArray1[ SO_POLYGON_MAX_NUM_VERTICES ]; //!< \internal 
	SoVector3	m_CameraSpaceArray1[ SO_POLYGON_MAX_NUM_VERTICES ]; //!< \internal 
	SoVector2	m_TextureCrdsArray1[ SO_POLYGON_MAX_NUM_VERTICES ];	//!< \internal 
	
} s_CurrentPolygon;


// ----------------------------------------------------------------------------
// Forward declarations of private functions
// ----------------------------------------------------------------------------
void SoCameraTransformMesh( SoCamera* a_This, SoMesh* a_Mesh );

void SoCameraSafeProject( SoCamera*  a_This, 
						  SoVector3* a_CameraSpaceCoordinate, 
						  SoVector2* a_ScreenSpaceCoordinate );

void SoCameraProject( SoCamera*  a_This, 
					  SoVector3* a_CameraSpaceCoordinate, 
					  SoVector2* a_ScreenSpaceCoordinate );

void SoCameraClipPolygon( SoCamera*	a_This, SoPolygon* a_Polygon );

void SoCameraClipTexturedPolygonAgainstFrustumPlane( SoCamera* a_This, s32 a_WhichFrustumPlane );
void SoCameraClipSolidPolygonAgainstFrustumPlane(	 SoCamera* a_This, s32 a_WhichFrustumPlane );

void SoCameraProjectUnProjectedVertices( SoCamera* a_This );

sofixedpoint SoCameraDistanceToFrustumPlane( SoCamera* a_This, SoVector3* a_CameraSpaceVertex, 
									 s32 a_WhichFrustumPlane );

bool SoCameraClockwise( SoVector2* a_AtLeastThreeVertices );

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Initializes a camera
	
 	\param	a_This This Pointer

	\todo	Fix hardcoded projection plane distance. Make this adjustable.

	Initializes the given camera. It resets its transform. It enables near- and 
	far-plane clipping. Enables frustum side-plane clipping. Initializes some internal 
	structures.
*/
// ----------------------------------------------------------------------------
void SoCameraInitialize( SoCamera* a_This )
{
	// Set the members;
	a_This->m_NearPlaneDistance		  = 10;
	a_This->m_ProjectionPlaneDistance = 200;
	a_This->m_FarPlaneDistance		  = 1400;

	// Reset the transform;
	SoTransformMakeIdentity( &(a_This->m_Transform) );

	// Set the clip flags;
	a_This->m_ClipAgainstFarAndNearPlane = true;
	a_This->m_ClipAgainstFrustumSidePlanes = true;

	// Calculate the frustumplane normals, leaving a 2-pixel
	// boundary around the screen to avoid accuracy problems;

	//! \todo Fix hardcoded values for a projection plane distance of 200;
	// For now, we assert that the value still is at 200.
	SO_ASSERT( a_This->m_ProjectionPlaneDistance == 200, "Hardcoded constant no longer at correct value" );
	a_This->m_LeftFrustumPlaneNormal.m_X = SO_FIXED_FROM_FLOAT( 0.861269258); // = 56444;
	a_This->m_LeftFrustumPlaneNormal.m_Y = SO_FIXED_FROM_FLOAT( 0.000000000); // = 0;
	a_This->m_LeftFrustumPlaneNormal.m_Z = SO_FIXED_FROM_FLOAT( 0.508148862); // = 33302;
	a_This->m_TopFrustumPlaneNormal.m_X  = SO_FIXED_FROM_FLOAT( 0.000000000); // = 0;
	a_This->m_TopFrustumPlaneNormal.m_Y  = SO_FIXED_FROM_FLOAT(-0.931654555); // = -61057;
	a_This->m_TopFrustumPlaneNormal.m_Z  = SO_FIXED_FROM_FLOAT( 0.363345276); // = 23812;

		
	// Set the polygon array pointers;
	s_CurrentPolygon.m_ScreenSpaceVertices			= s_CurrentPolygon.m_ScreenSpaceArray0;
	s_CurrentPolygon.m_ClippedScreenSpaceVertices	= s_CurrentPolygon.m_ScreenSpaceArray1;
	s_CurrentPolygon.m_CameraSpaceVertices			= s_CurrentPolygon.m_CameraSpaceArray0;
	s_CurrentPolygon.m_ClippedCameraSpaceVertices	= s_CurrentPolygon.m_CameraSpaceArray1;
	s_CurrentPolygon.m_TextureCoordinates			= s_CurrentPolygon.m_TextureCrdsArray0;
	s_CurrentPolygon.m_ClippedTextureCoordinates	= s_CurrentPolygon.m_TextureCrdsArray1;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the pitch of the camera. A full circle is 256 degrees.

	\param	a_This		This pointer
	\param	a_Angle		Angle
*/
// ----------------------------------------------------------------------------
void SoCameraSetPitch( SoCamera* a_This, s32 a_Angle ) 
{ 
	SoTransformSetRotateY( &a_This->m_Transform, a_Angle ); 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the yaw of the camera. A full circle is 256 degrees.	
	
	\param	a_This		This pointer
	\param	a_Angle		Angle
*/
// ----------------------------------------------------------------------------
void SoCameraSetYaw( SoCamera* a_This, s32 a_Angle ) 
{ 
	SoTransformSetRotateX( &a_This->m_Transform, a_Angle ); 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the roll of the camera. A full circle is 256 degrees.

	\param	a_This		This pointer
	\param	a_Angle		Angle
*/
// ----------------------------------------------------------------------------
void SoCameraSetRoll( SoCamera* a_This, s32 a_Angle ) 
{ 
	SoTransformSetRotateZ( &a_This->m_Transform, a_Angle ); 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the translate of the camera.

	\param a_This	This pointer
	\param a_X		Fixed point X-part of the translate
	\param a_Y		Fixed point Y-part of the translate
	\param a_Z		Fixed point Z-part of the translate
*/
// ----------------------------------------------------------------------------
void SoCameraSetTranslation( SoCamera* a_This, sofixedpoint a_X, sofixedpoint a_Y, sofixedpoint a_Z ) 
{ 
	SoTransformSetTranslation( &a_This->m_Transform, a_X, a_Y, a_Z ); 
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/*!
	\brief Enables or disables clipping of polygons against the far and near planes.

	\param	a_This		This pointer
	\param	a_Enable	\a true to enable, \a false to disable

	By default far and near plane clipping is enabled. However, if you know that the mesh
	you are about to render is fully in between those two planes (and hence doesn't need 
	clipping) you should disable this clipping to get a speedup.
*/
// ----------------------------------------------------------------------------
void SoCameraSetFarAndNearPlaneClippingEnable( SoCamera* a_This, bool a_Enable ) 
{ 
	a_This->m_ClipAgainstFarAndNearPlane = a_Enable; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\ Enables or disables clipping of polygons against the frustum side planes.

  	\param	a_This		This pointer
	\param	a_Enable	\a true to enable, \a false to disable

	By default frustum side plane clipping is enabled. However, if you know that the mesh
	you are about to render is fully in between those four planes (and hence doesn't need 
	clipping) you should disable this clipping to get a speedup.
*/
// --------------------------------------------------------------------------------------
void SoCameraSetFrustumSidePlanesClippingEnable(  SoCamera* a_This,	bool a_Enable ) 
{ 
	a_This->m_ClipAgainstFrustumSidePlanes = a_Enable;
}
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
/*! 
	\brief Pitches the camera.

	\param	a_This	This pointer
	\param	a_Angle	Angle, a full circle is 256 degrees

	The difference between this function and the \a SoCameraSetPitch function is that
	this function adds the given pitch to the existing pitch.
*/
// --------------------------------------------------------------------------------------
void SoCameraPitch( SoCamera* a_This, s32 a_Angle )
{
	SoTransformRotateY( &a_This->m_Transform, a_Angle );
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*! 
	\brief Yaws the camera.

	\param	a_This	This pointer
	\param	a_Angle	Angle, a full circle is 256 degrees

	  The difference between this function and the \a SoCameraSetYaw function is that
	this function adds the given yaw to the existing yaw.
*/
// --------------------------------------------------------------------------------------
void SoCameraYaw( SoCamera* a_This,	s32 a_Angle	)
{
	SoTransformRotateX( &a_This->m_Transform, a_Angle );
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*! 
	\brief Rolls the camera.

  	\param	a_This	This pointer
	\param	a_Angle	Angle, a full circle is 256 degrees

	The difference between this function and the \a SoCameraSetRoll function is that
	this function adds the given roll to the existing roll.
*/
// --------------------------------------------------------------------------------------
void SoCameraRoll( SoCamera* a_This, s32 a_Angle )
{
	SoTransformRotateZ( &a_This->m_Transform, a_Angle );
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Forwards the camera by the given fixed point amount.

	\param	a_This		This pointer
	\param	a_Amount	Fixed point amount to move forward with

	If you need backwards movement just give a negative amount.
*/
// --------------------------------------------------------------------------------------
void SoCameraForward( SoCamera* a_This,  sofixedpoint a_Amount )
{
	SoTransformTranslate( &a_This->m_Transform, 0, 0, a_Amount );
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Moves the camera to the right by the given fixed point amount.

	\param	a_This		This pointer
	\param	a_Amount	Fixed point amount to move right with

	If you need movement to the left just give a negative amount.
*/
// --------------------------------------------------------------------------------------
void SoCameraRight( SoCamera* a_This,  sofixedpoint a_Amount )
{
	SoTransformTranslate( &a_This->m_Transform, a_Amount, 0, 0 );
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Moves the camera up by the given fixed point amount.

	\param	a_This		This pointer
    \param	a_Amount	Fixed point amount to move up with

	If you need downwards movement just give a negative amount.
*/
// --------------------------------------------------------------------------------------
void SoCameraUp( SoCamera* a_This,  sofixedpoint a_Amount )
{
	SoTransformTranslate( &a_This->m_Transform, 0, a_Amount, 0 );
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief	Draws the given mesh into the current buffer

	\param	a_This		This pointer
	\param	a_Mesh		Mesh that should be drawn

	\todo	Make this method independent from the SoMode4Rasterizer

	This function transforms the given mesh from object space to camera space, performs
	clipping if neccesary and requested, then transforms it to screenspace, and then
	draws it using the correct functions (texture, non-textured, etc.).
*/
// --------------------------------------------------------------------------------------
void SoCameraDrawMesh( SoCamera* a_This, SoMesh* a_Mesh )
{
	// Dummy counter;
	u32 i;

	// Does the mesh have a texture;
	if (  SoMeshGetTexture( a_Mesh ) != NULL )
	{
		// Set the texture the rasterizer should use;
		SoMode4PolygonRasterizerSetTexture( SoMeshGetTexture( a_Mesh ) );

		// The polygon we are about to draw does has a texture.
		s_CurrentPolygon.m_HasTexture = true;
	}
	else
	{
		// The polygon we are about to draw does not have a texture.
		s_CurrentPolygon.m_HasTexture = false;
	}

	// Transform the mesh, this fills the screen- and 
	// cameraspace vertex buffers;
	SoCameraTransformMesh( a_This, a_Mesh );

	// Draw each polygon of the mesh;
	for ( i = 0; i < SoMeshGetNumPolygons( a_Mesh ); i++ )
	{
		// Clip the polygon;
		SoCameraClipPolygon( a_This, SoMeshGetPolygon( a_Mesh, i ) );

		// Is there a polygon left after clipping;
		if ( s_CurrentPolygon.m_NumVertices != 0 )
		{
			// Is the polygon clockwise ordered (not backface culled);
			if (SoCameraClockwise( s_CurrentPolygon.m_ScreenSpaceVertices ) )
			{
				// Draw the polygon;
				if ( s_CurrentPolygon.m_HasTexture )
				{
					SoMode4PolygonRasterizerDrawTexturedPolygon( s_CurrentPolygon.m_NumVertices,
																 s_CurrentPolygon.m_ScreenSpaceVertices, 
																 s_CurrentPolygon.m_TextureCoordinates
																);		
				}
				else
				{
					// Maybe it's a triangle;
					if ( s_CurrentPolygon.m_NumVertices == 3 )
					{
						SoMode4PolygonRasterizerDrawSolidTriangle( s_CurrentPolygon.m_ScreenSpaceVertices, 
																   SoPolygonGetPaletteIndex( SoMeshGetPolygon( a_Mesh, i ) ) );
					}
					else
					{
						SoMode4PolygonRasterizerDrawSolidPolygon( s_CurrentPolygon.m_NumVertices,
															 s_CurrentPolygon.m_ScreenSpaceVertices, 
															 SoPolygonGetPaletteIndex( SoMeshGetPolygon( a_Mesh, i ) )
														   );		
					}
				}
			}
		}
	}
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Transforms the given mesh filling the camera- and screenspace vertex buffers.

	\internal Used by \a SoCameraDrawMesh only.

	\param	a_This	This pointer
	\param	a_Mesh	Mesh to be transformed

	This method calculates the objectspace to cameraspace (via worldspace) matrix and 
	transforms the mesh by that matrix, filling the cameraspace vertex buffer. It then uses
	a safe project method to project the vertices to screenspace. This safeproject marks any
	vertex that couldn't be projected because it was outside of the frustum.	
*/
// --------------------------------------------------------------------------------------
void SoCameraTransformMesh( SoCamera* a_This, SoMesh* a_Mesh )
{
	// Dummy counter;
	u32 i;
	
	// Matrices;
	SoMatrix worldToCameraMatrix;
	SoMatrix objectToWorldMatrix;
	SoMatrix objectToCameraMatrix;

	// Create the object- to worldspace matrix;
	SoTransformToMatrix( SoMeshGetTransform( a_Mesh ), &objectToWorldMatrix );

	// Create the world- to cameraspace matrix;
	SoTransformToInverseMatrix( &a_This->m_Transform, &worldToCameraMatrix );

	// Multiply the two to create the object- to cameraspace matrix;
	SoMatrixMultiply( &objectToCameraMatrix, &worldToCameraMatrix, &objectToWorldMatrix );

	// Transform all the vertices in the mesh to camera space;
	// And then project them to screen space;
	for ( i = 0; i < SoMeshGetNumVertices( a_Mesh ); i++ )
	{
		SoVector3TransformInto( SoMeshGetVertex( a_Mesh, i ), 
								&s_CameraSpaceVertexBuffer[ i ], 
								&objectToCameraMatrix );

		SoCameraSafeProject( a_This, &s_CameraSpaceVertexBuffer[ i ],
							 &s_ScreenSpaceVertexBuffer[ i ] );

	}
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*! 
	\brief		Clips the given polygon against the camera's frustum.

	\internal	Only called from within \a SoCameraDrawMesh.
	
	\pre		This method expects that the camera's has correctly filled 
				screen- and cameraspace vertex buffers. This means that \a SoTransformMesh has 
				been called previously for the mesh containing this polygon.

	\post		The \a s_CurrentPolygon contains the polygon correctly clipped against the frustum.

	\param	a_This		This pointer
	\param	a_Polygon	Polygon to be clipped

	Note that this function only clips the polygon if near- and farplane clipping and 
	frustum sideplane clipping are enabled. If these are both disabled all this function does
	is copy the polygon from the given \a SoPolygon object into the \a s_CurrentPolygon
	object.
*/
// --------------------------------------------------------------------------------------
void SoCameraClipPolygon( SoCamera* a_This, SoPolygon* a_Polygon )
{
	// Dummy counter;
	u32 i;

	// Booleans for different polygon situations;
	bool polygonNeedsClipping = false;
	
	// Texture coordinate array;
	SoVector2* textureCoordinates = SoPolygonGetTextureCoordinates( a_Polygon );

	// Pointer to vertices;
	SoVector3* cameraSpaceVertex;
	SoVector2* screenSpaceVertex;

	// Number of vertices in the polygon;
	s_CurrentPolygon.m_NumVertices = SoPolygonGetNumVertices( a_Polygon );
	
	// Iterate over every vertex;
	for ( i = 0; i < s_CurrentPolygon.m_NumVertices; i++ )
	{
		// Get pointers to the vertices;
		screenSpaceVertex = &s_ScreenSpaceVertexBuffer[ SoPolygonGetVertexIndex( a_Polygon, i ) ];
		cameraSpaceVertex = &s_CameraSpaceVertexBuffer[ SoPolygonGetVertexIndex( a_Polygon, i ) ];

		// Fill the camera space vertex array;
		s_CurrentPolygon.m_CameraSpaceVertices[ i ].m_X = cameraSpaceVertex->m_X;
		s_CurrentPolygon.m_CameraSpaceVertices[ i ].m_Y = cameraSpaceVertex->m_Y;
		s_CurrentPolygon.m_CameraSpaceVertices[ i ].m_Z = cameraSpaceVertex->m_Z;

		// Set the texture coordinates, if neccesary;
		if ( s_CurrentPolygon.m_HasTexture )
		{
			s_CurrentPolygon.m_TextureCoordinates[ i ].m_X = textureCoordinates[ i ].m_X;
			s_CurrentPolygon.m_TextureCoordinates[ i ].m_Y = textureCoordinates[ i ].m_Y;
		}

		
		// Set the screen space vertices;
		s_CurrentPolygon.m_ScreenSpaceVertices[ i ].m_X = screenSpaceVertex->m_X;
		s_CurrentPolygon.m_ScreenSpaceVertices[ i ].m_Y = screenSpaceVertex->m_Y;

		// Is this vertex outside of the frustum;
		if ( screenSpaceVertex->m_X < 0 )
		{
			// There is at least one vertex of this polygon
			// that is not in the frustum. We need to clip
			// the entire polygon;
			polygonNeedsClipping = true;
		}
	}
	
	// Do we need to clip the polygon;
	if ( polygonNeedsClipping )
	{
		// Do we need to clip texture coordinates;
		if ( s_CurrentPolygon.m_HasTexture )
		{
			// Clip against the far and near plane if requested;
			if ( a_This->m_ClipAgainstFarAndNearPlane )
			{
				SoCameraClipTexturedPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_NEAR_PLANE	);
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipTexturedPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_FAR_PLANE		);
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
			}
			// Clip against the frustum side planes if requested;
			if ( a_This->m_ClipAgainstFrustumSidePlanes )
			{
				SoCameraClipTexturedPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_LEFT_PLANE	);
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipTexturedPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_RIGHT_PLANE	);
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipTexturedPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_TOP_PLANE		);
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipTexturedPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_BOTTOM_PLANE	);
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
			}
		}
		else
		{
			// Clip against the far and near plane if requested;
			if ( a_This->m_ClipAgainstFarAndNearPlane )
			{
				SoCameraClipSolidPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_NEAR_PLANE );
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipSolidPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_FAR_PLANE );
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
			}
			// Clip against the frustum side planes if requested;
			if ( a_This->m_ClipAgainstFrustumSidePlanes )
			{
				SoCameraClipSolidPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_LEFT_PLANE );
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipSolidPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_RIGHT_PLANE );
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipSolidPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_TOP_PLANE );
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
				SoCameraClipSolidPolygonAgainstFrustumPlane( a_This, SO_CAMERA_FRUSTUM_BOTTOM_PLANE	);
				if ( s_CurrentPolygon.m_NumVertices == 0 ) return;
			}
		}

		// Project the unprojected vertices;
		SoCameraProjectUnProjectedVertices( a_This );
	}
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Returns the fixed point distance from a cameraspace vertex to a frustum plane.

	\internal 

	\param	a_This					This pointer
	\param	a_CameraSpaceVertex		Vertex in camera space
	\param	a_WhichFrustumPlane		Identifies the frustum to which you want to know the distance.
									This is one of the SO_CAMERA_FRUSTUM_ * _PLANE constants.
*/
// --------------------------------------------------------------------------------------
sofixedpoint  SoCameraDistanceToFrustumPlane( SoCamera* a_This, SoVector3* a_CameraSpaceVertex, 
									 s32 a_WhichFrustumPlane )
								  	
{
	switch( a_WhichFrustumPlane )
	{
		// ---------
		case SO_CAMERA_FRUSTUM_NEAR_PLANE:
			
			// We add two to the near-plane-distance to avoid accuracy troubles;
			return (a_CameraSpaceVertex->m_Z - SO_FIXED_FROM_WHOLE( a_This->m_NearPlaneDistance + 2 ));

		break;
		// ---------
		case SO_CAMERA_FRUSTUM_FAR_PLANE:

			// We substract two from the far-plane-distance to avoid accuracy troubles;
			return (SO_FIXED_FROM_WHOLE( a_This->m_FarPlaneDistance - 2 ) - a_CameraSpaceVertex->m_Z);

		break;

		// The distance for the rest the planes is calculated using the dot product
		// of the point with the surface normal.  The resulting value is the shortest
		// possible distance from the point to the plane.
		//
		// Since this calculation is being done in camera space, we've taken advantage
		// of the symmetry between the LEFT/RIGHT and TOP/BOTTOM planes, and the fact
		// that some of the surface normal components are '0' and the
		//
		// ---------
		case SO_CAMERA_FRUSTUM_LEFT_PLANE:

			// Y component of normal is 0
			return SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_X, a_This->m_LeftFrustumPlaneNormal.m_X) +
				SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_Z, a_This->m_LeftFrustumPlaneNormal.m_Z);
		break;
		// ---------
		case SO_CAMERA_FRUSTUM_RIGHT_PLANE:

			// Y component of normal is 0
			return SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_X, -a_This->m_LeftFrustumPlaneNormal.m_X) +
				SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_Z, a_This->m_LeftFrustumPlaneNormal.m_Z);
		break;
		// ---------
		case SO_CAMERA_FRUSTUM_TOP_PLANE:

			// X component of normal is 0
			return SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_Y, a_This->m_TopFrustumPlaneNormal.m_Y) +
				SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_Z, a_This->m_TopFrustumPlaneNormal.m_Z);
		break;
		// ---------
		case SO_CAMERA_FRUSTUM_BOTTOM_PLANE:

			// X component of normal is 0
			return SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_Y, -a_This->m_TopFrustumPlaneNormal.m_Y) +
				SoMathFixedMultiplyByFraction(a_CameraSpaceVertex->m_Z,  a_This->m_TopFrustumPlaneNormal.m_Z);
		break;
		// ---------
		default:
			
			// This shouldn't be reachable.
			SO_ASSERT( false, "Illegal default switch case reached." );
			
			// For release builds;
			return SO_FIXED_FROM_WHOLE( 1 );
	}
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief	Clips a textured \a s_CurrentPolygon against a single specific frustum plane.

	\internal Only called from within \a SoCameraClipPolygon

	\pre	\a s_CurrentPolygon correctly filled.
	
	\post	\a The s_CurrentPolygon but now clipped against the given plane.

	\param	a_This					This pointer
	\param	a_WhichFrustumPlane		Identifies the frustum to which you want to know clip.
									This is one of the SO_CAMERA_FRUSTUM_ * _PLANE constants.

	This function clips the texture coordinates as well as the vertices. It uses the \a m_Clipped
	prefixed members of the \a s_CurrentPolygon structure as an intermediate result to store the
	clipped polygon, and swaps the arrays at the end so it becomes normal again.

	It that didn't make sense then tell me.
*/
// --------------------------------------------------------------------------------------
void SoCameraClipTexturedPolygonAgainstFrustumPlane( SoCamera* a_This, s32 a_WhichFrustumPlane )
{
	// Dummy counter;
	u32 i;

	// Variables to hold vertex-to-frustum-plane distances;
	sofixedpoint distanceVertexA;
	sofixedpoint distanceVertexB;

	// Parametric value used during clipping;
	sofixedpoint t;

	// Variables to hold the start and end of an edge;
	SoVector3* cameraSpaceVertexA = &s_CurrentPolygon.m_CameraSpaceVertices[ s_CurrentPolygon.m_NumVertices - 1 ];
	SoVector3* cameraSpaceVertexB = &s_CurrentPolygon.m_CameraSpaceVertices[ 0 ];
	SoVector2* screenSpaceVertexA = &s_CurrentPolygon.m_ScreenSpaceVertices[ s_CurrentPolygon.m_NumVertices - 1 ];
	SoVector2* screenSpaceVertexB = &s_CurrentPolygon.m_ScreenSpaceVertices[ 0 ];
	SoVector2* textureCoordinateA = &s_CurrentPolygon.m_TextureCoordinates[ s_CurrentPolygon.m_NumVertices - 1 ];
	SoVector2* textureCoordinateB = &s_CurrentPolygon.m_TextureCoordinates[ 0 ];

	// Variables to swap;
	u32			swpU32;
	SoVector2*	swpVector2Pointer;
	SoVector3*	swpVector3Pointer;

	// Reset the number of vertices in polygon B;
	s_CurrentPolygon.m_ClippedNumVertices = 0;

	// Calculate the distance of vertex A;
	distanceVertexA = SoCameraDistanceToFrustumPlane( a_This, cameraSpaceVertexA, a_WhichFrustumPlane ); 

	// Iterate over every edge;
	for ( i = 1; i < (s_CurrentPolygon.m_NumVertices + 1); i++ )
	{
		// Calculate the distance of vertex B;
		distanceVertexB = SoCameraDistanceToFrustumPlane( a_This, cameraSpaceVertexB, a_WhichFrustumPlane ); 

		// Is the edge in the front;
		if ( (distanceVertexA >= 0) && (distanceVertexB >= 0) )
		{
			// Add vertex B to the new polygon;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexB->m_Y;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexB->m_Z;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = screenSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = screenSpaceVertexB->m_Y;
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_X	= textureCoordinateB->m_X;	
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y	= textureCoordinateB->m_Y;	

			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;
		}
		else
		// Is the edge going in the plane;
		if ( (distanceVertexA >= 0) && (distanceVertexB < 0) )
		{
			// Calculate parametric t value of the clipped point;
			// We negate distanceVertexB because it is negative and we want the absolute distance now;
			t = SO_FIXED_DIVIDE( distanceVertexA, -distanceVertexB + distanceVertexA );

			// Set the clipped cameraspace point;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexA->m_X + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_X - cameraSpaceVertexA->m_X, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexA->m_Y + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Y - cameraSpaceVertexA->m_Y, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexA->m_Z + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Z - cameraSpaceVertexA->m_Z, t );

			// Set the clipped texture coord;
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = textureCoordinateA->m_X + SO_FIXED_MULTIPLY_BIG_SMALL( textureCoordinateB->m_X - textureCoordinateA->m_X, t );
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = textureCoordinateA->m_Y + SO_FIXED_MULTIPLY_BIG_SMALL( textureCoordinateB->m_Y - textureCoordinateA->m_Y, t );

			// Set the screen space point to a negative value;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = -1;
			
			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;
		}
		else
		// The edge is going out the plane;
		if ( (distanceVertexA < 0) && (distanceVertexB >= 0) )
		{
			// Calculate parametric t value of the clipped point;
			// We negate distanceVertexA because it is negative and we want
			// absolute distances;
			t = SO_FIXED_DIVIDE( -distanceVertexA, distanceVertexB - distanceVertexA );

			// Set the clipped cameraspace point;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexA->m_X + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_X - cameraSpaceVertexA->m_X, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexA->m_Y + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Y - cameraSpaceVertexA->m_Y, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexA->m_Z + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Z - cameraSpaceVertexA->m_Z, t );

			// Set the clipped texture coord;
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = textureCoordinateA->m_X + SO_FIXED_MULTIPLY_BIG_SMALL( textureCoordinateB->m_X - textureCoordinateA->m_X, t );
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = textureCoordinateA->m_Y + SO_FIXED_MULTIPLY_BIG_SMALL( textureCoordinateB->m_Y - textureCoordinateA->m_Y, t );

			// Set the screen space point to a negative value;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = -1;

			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;

			// Add vertex B to the new polygon as well;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexB->m_Y;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexB->m_Z;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = screenSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = screenSpaceVertexB->m_Y;
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_X	= textureCoordinateB->m_X;	
			s_CurrentPolygon.m_ClippedTextureCoordinates[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y	= textureCoordinateB->m_Y;	

			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;
		}
		// Else, the edge is in the back and nothing has to be done;

		// Go to the next edge;
		distanceVertexA	   = distanceVertexB;
		cameraSpaceVertexA = cameraSpaceVertexB;
		screenSpaceVertexA = screenSpaceVertexB;
		textureCoordinateA = textureCoordinateB;	
		cameraSpaceVertexB = &s_CurrentPolygon.m_CameraSpaceVertices[ i ];
		screenSpaceVertexB = &s_CurrentPolygon.m_ScreenSpaceVertices[ i ];
		textureCoordinateB = &s_CurrentPolygon.m_TextureCoordinates[ i ];
	}

	// Swap the number of vertices around;
	swpU32				   = s_CurrentPolygon.m_NumVertices; 
	s_CurrentPolygon.m_NumVertices = s_CurrentPolygon.m_ClippedNumVertices; 
	s_CurrentPolygon.m_ClippedNumVertices = swpU32;

	// Swap the polygon arrays around;
	swpVector3Pointer					= s_CurrentPolygon.m_CameraSpaceVertices;
	s_CurrentPolygon.m_CameraSpaceVertices	    = s_CurrentPolygon.m_ClippedCameraSpaceVertices;
	s_CurrentPolygon.m_ClippedCameraSpaceVertices = swpVector3Pointer;

	swpVector2Pointer					= s_CurrentPolygon.m_ScreenSpaceVertices;
	s_CurrentPolygon.m_ScreenSpaceVertices		= s_CurrentPolygon.m_ClippedScreenSpaceVertices;
	s_CurrentPolygon.m_ClippedScreenSpaceVertices = swpVector2Pointer;
	
	swpVector2Pointer					= s_CurrentPolygon.m_TextureCoordinates;
	s_CurrentPolygon.m_TextureCoordinates		= s_CurrentPolygon.m_ClippedTextureCoordinates;
	s_CurrentPolygon.m_ClippedTextureCoordinates = swpVector2Pointer;
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief	Clips a solid \a s_CurrentPolygon against a single specific frustum plane.

	\internal Only called from within \a SoCameraClipPolygon

	\pre	\a s_CurrentPolygon correctly filled.
	
	\post	\a The s_CurrentPolygon but now clipped against the given plane.

	\param	a_This					This pointer
	\param	a_WhichFrustumPlane		Identifies the frustum to which you want to know clip.
									This is one of the SO_CAMERA_FRUSTUM_ * _PLANE constants.

	This function only clips the vertices (no texture coordinates). It uses the \a m_Clipped
	prefixed members of the \a s_CurrentPolygon structure as an intermediate result to store the
	clipped polygon, and swaps the arrays at the end so it becomes normal again.

	It that didn't make sense then tell me.
*/
// --------------------------------------------------------------------------------------
void SoCameraClipSolidPolygonAgainstFrustumPlane( SoCamera* a_This, s32 a_WhichFrustumPlane )
{
	// Dummy counter;
	u32 i;

	// Variables to hold vertex-to-frustum-plane distances;
	sofixedpoint distanceVertexA;
	sofixedpoint distanceVertexB;

	// Parametric value used during clipping;
	sofixedpoint t;

	// Variables to hold the start and end of an edge;
	SoVector3* cameraSpaceVertexA = &s_CurrentPolygon.m_CameraSpaceVertices[ s_CurrentPolygon.m_NumVertices - 1 ];
	SoVector2* screenSpaceVertexA = &s_CurrentPolygon.m_ScreenSpaceVertices[ s_CurrentPolygon.m_NumVertices - 1 ];
	
	// Variables to swap;
	u32			swpU32;
	SoVector2*	swpVector2Pointer;
	SoVector3*	swpVector3Pointer;

	// Reset the number of vertices in clipped polygon
	s_CurrentPolygon.m_ClippedNumVertices = 0;

	// Calculate the distance of vertex A;
	distanceVertexA = SoCameraDistanceToFrustumPlane( a_This, cameraSpaceVertexA, a_WhichFrustumPlane ); 

	// Iterate over every edge;
	for(i = 0; i < s_CurrentPolygon.m_NumVertices; ++i)
	{
		SoVector3* cameraSpaceVertexB = &s_CurrentPolygon.m_CameraSpaceVertices[ i ];
		SoVector2* screenSpaceVertexB = &s_CurrentPolygon.m_ScreenSpaceVertices[ i ];

		// Calculate the distance of vertex B;
		distanceVertexB = SoCameraDistanceToFrustumPlane( a_This, cameraSpaceVertexB, a_WhichFrustumPlane ); 

		// Is the edge in the front;
		if ( (distanceVertexA >= 0) && (distanceVertexB >= 0) )
		{
			// Add vertex B to the new polygon;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexB->m_Y;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexB->m_Z;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = screenSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = screenSpaceVertexB->m_Y;
			
			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;
		}
		else
		// Is the edge going in the plane;
		if ( (distanceVertexA >= 0) && (distanceVertexB < 0) )
		{
			// Calculate parametric t value of the clipped point;
			// We negate distanceVertexB because it is negative and we want the absolute distance now;
			t = SO_FIXED_DIVIDE( distanceVertexA, -distanceVertexB + distanceVertexA );

			// Set the clipped cameraspace point;
			// V = A + t * (vec(AtoB))
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexA->m_X + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_X - cameraSpaceVertexA->m_X, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexA->m_Y + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Y - cameraSpaceVertexA->m_Y, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexA->m_Z + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Z - cameraSpaceVertexA->m_Z, t );

			// Set the screen space point to a negative value;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = -1;
			
			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;
		}
		else
		// The edge is going out the plane;
		if ( (distanceVertexA < 0) && (distanceVertexB >= 0) )
		{
			// Calculate parametric t value of the clipped point;
			// We negate distanceVertexA because it is negative and we want
			// absolute distances;
			t = SO_FIXED_DIVIDE( -distanceVertexA, distanceVertexB - distanceVertexA );

			// Set the clipped cameraspace point;
			// V = A + t * (vec(AtoB))
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexA->m_X + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_X - cameraSpaceVertexA->m_X, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexA->m_Y + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Y - cameraSpaceVertexA->m_Y, t );
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexA->m_Z + SO_FIXED_MULTIPLY_BIG_SMALL( cameraSpaceVertexB->m_Z - cameraSpaceVertexA->m_Z, t );

			// Set the screen space point to a negative value;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = -1;

			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;

			// Add vertex B to the new polygon as well;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = cameraSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = cameraSpaceVertexB->m_Y;
			s_CurrentPolygon.m_ClippedCameraSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Z = cameraSpaceVertexB->m_Z;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_X = screenSpaceVertexB->m_X;
			s_CurrentPolygon.m_ClippedScreenSpaceVertices[ s_CurrentPolygon.m_ClippedNumVertices ].m_Y = screenSpaceVertexB->m_Y;

			// Increase the number of vertices;
			s_CurrentPolygon.m_ClippedNumVertices++;
		}
		// Else, the edge is in the back and nothing has to be done;

		// Go to the next edge;
		distanceVertexA	   = distanceVertexB;
		cameraSpaceVertexA = cameraSpaceVertexB;
		screenSpaceVertexA = screenSpaceVertexB;
	}

	// Swap the number of vertices around;
	swpU32				 = s_CurrentPolygon.m_NumVertices; 
	s_CurrentPolygon.m_NumVertices		 = s_CurrentPolygon.m_ClippedNumVertices; 
	s_CurrentPolygon.m_ClippedNumVertices = swpU32;

	// Swap the polygon arrays around;
	swpVector3Pointer			= s_CurrentPolygon.m_CameraSpaceVertices;
	s_CurrentPolygon.m_CameraSpaceVertices	    = s_CurrentPolygon.m_ClippedCameraSpaceVertices;
	s_CurrentPolygon.m_ClippedCameraSpaceVertices = swpVector3Pointer;

	swpVector2Pointer			= s_CurrentPolygon.m_ScreenSpaceVertices;
	s_CurrentPolygon.m_ScreenSpaceVertices		= s_CurrentPolygon.m_ClippedScreenSpaceVertices;
	s_CurrentPolygon.m_ClippedScreenSpaceVertices = swpVector2Pointer;
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Cameraspace vertex projector that can handle vertices outside the frustum.

	\internal

	\param	a_This						This pointer
	\param	a_CameraSpaceCoordinate		Vertex in camera space
	
	\retval	a_ScreenSpaceCoordinate		Transformed vertex

	If the vertex cannot be projected because it is outside of the frustum a negative
	value is loaded in the X component. Since a negative X means it's offscreen we can use
	this to tell that this vertex couldn't be projected on screen.
	
	We use the SO_CAMERA_FRUSTUM_ * _PLANE constants for this so we can identify 
	which plane it was clipped by first. That's why these constants have to be negative.
*/
// --------------------------------------------------------------------------------------
void SoCameraSafeProject( SoCamera*	a_This, 
						  SoVector3* a_CameraSpaceCoordinate, 
						  SoVector2* a_ScreenSpaceCoordinate )
{
	// Is Z too near;
	if ( SO_FIXED_TO_WHOLE( a_CameraSpaceCoordinate->m_Z ) < a_This->m_NearPlaneDistance )
	{
		// Set negative X;
		a_ScreenSpaceCoordinate->m_X = SO_CAMERA_FRUSTUM_NEAR_PLANE;

		// Done;
		return;
	}

	// Is Z too far;
	if ( SO_FIXED_TO_WHOLE( a_CameraSpaceCoordinate->m_Z ) > a_This->m_FarPlaneDistance )
	{
		// Set negative X;
		a_ScreenSpaceCoordinate->m_X = SO_CAMERA_FRUSTUM_FAR_PLANE;

		// Done;
		return;
	}

	// Perform the project;
	SoCameraProject( a_This, a_CameraSpaceCoordinate, a_ScreenSpaceCoordinate );

	// Are the screen coordinates in the screen-range;
	if ( a_ScreenSpaceCoordinate->m_X < 0 )
	{
		// Set negative X;
		a_ScreenSpaceCoordinate->m_X = SO_CAMERA_FRUSTUM_LEFT_PLANE;

		// Done;
		return;
	}
	if ( a_ScreenSpaceCoordinate->m_Y < 0 )
	{
		// Set negative X;
		a_ScreenSpaceCoordinate->m_X = SO_CAMERA_FRUSTUM_TOP_PLANE;

		// Done;
		return;
	}
	if ( a_ScreenSpaceCoordinate->m_X > SO_FIXED_FROM_WHOLE( SO_SCREEN_WIDTH  ) )
	{
		// Set negative X;
		a_ScreenSpaceCoordinate->m_X = SO_CAMERA_FRUSTUM_RIGHT_PLANE;

		// Done;
		return;
	}
	if ( a_ScreenSpaceCoordinate->m_Y > SO_FIXED_FROM_WHOLE( SO_SCREEN_HEIGHT ) )
	{
		// Set negative X;
		a_ScreenSpaceCoordinate->m_X = SO_CAMERA_FRUSTUM_BOTTOM_PLANE;

		// Done;
		return;
	}
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Projects a vertex from cameraspace to screenspace.

	\internal

	\param	a_This						This pointer
	\param	a_CameraSpaceCoordinate		Vertex in camera space
	
	\retval	a_ScreenSpaceCoordinate		Transformed vertex

	\todo	Optimize

	\warning	This function does no frustum checks, so if you're not carefull you can get some
				weird results. You might even get a divide by zero. Use it only for vertices
				that are guaranteed to be inbetween the near and far clipping planes.

*/
// --------------------------------------------------------------------------------------
void SoCameraProject( SoCamera*	a_This, 
					  SoVector3* a_CameraSpaceCoordinate, 
					  SoVector2* a_ScreenSpaceCoordinate )
{
	// Value to hold the one-over-Z multiplier;
	sofixedpoint ooZ;

	// Calculate the projectionPlaneDistance / Z;
	ooZ = SO_FIXED_ONE_OVER_SLOW_ACCURATE( a_CameraSpaceCoordinate->m_Z );
	ooZ *= a_This->m_ProjectionPlaneDistance;

	// Project from cameraspace to screenspace;
	// The Y value is negated because Y is upside down on the screen;
	a_ScreenSpaceCoordinate->m_X =  SO_FIXED_MULTIPLY_BIG_SMALL( a_CameraSpaceCoordinate->m_X, ooZ );
	a_ScreenSpaceCoordinate->m_Y = -SO_FIXED_MULTIPLY_BIG_SMALL( a_CameraSpaceCoordinate->m_Y, ooZ );

	// Convert to center of screen;
	a_ScreenSpaceCoordinate->m_X += SO_FIXED_FROM_WHOLE( SO_SCREEN_HALF_WIDTH  );
	a_ScreenSpaceCoordinate->m_Y += SO_FIXED_FROM_WHOLE( SO_SCREEN_HALF_HEIGHT );
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\brief Projects the vertices that weren't projected before (i.e. vertices introduced
		   during clipping).

	\internal

	\param a_This This pointer

	Projects all unprojected vertices in the \a s_CurrentPolygon structure. These are the
	vertices introduced during clipping.
*/
// ----------------------------------------------------------------------------
void SoCameraProjectUnProjectedVertices( SoCamera* a_This )
{
	// Dummy counter;
	u32 i;

	// Iterate over every vertex;
	for ( i = 0; i < s_CurrentPolygon.m_NumVertices; i++ )
	{
		// If this screen-vertex is not already projected;
		if ( s_CurrentPolygon.m_ScreenSpaceVertices[ i ].m_X < 0 )
		{
			// Project it now;
			// We don't have to do a safe project cause these vertices are surely 
			// in the screen;
			SoCameraProject( a_This, &s_CurrentPolygon.m_CameraSpaceVertices[ i ], &s_CurrentPolygon.m_ScreenSpaceVertices[ i ] );
		}
	}

}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Tests whether the polygon's vertices are clockwise ordered.

	\internal

	\param a_AtLeastThreeVertices An array of at least three 2D vertices

	\todo	Move backface culling from 2D to 3D.

	Tests whether the given triangle (first three vertices of a polygon) are lying in a
	clockwise order. This can be used for backface culling.
*/
// ----------------------------------------------------------------------------
bool SoCameraClockwise( SoVector2* a_AtLeastThreeVertices )
{
	// Calculate the normal of the edge between vertex 0 and 1;
	sofixedpoint edgeNormalX = a_AtLeastThreeVertices[ 0 ].m_Y - a_AtLeastThreeVertices[ 1 ].m_Y;
	sofixedpoint edgeNormalY = a_AtLeastThreeVertices[ 1 ].m_X - a_AtLeastThreeVertices[ 0 ].m_X;

	// Calculate whole number vertex 0 and 2;
	s32 vertex0X = SO_FIXED_TO_WHOLE( a_AtLeastThreeVertices[ 0 ].m_X );
	s32 vertex0Y = SO_FIXED_TO_WHOLE( a_AtLeastThreeVertices[ 0 ].m_Y );
	s32 vertex2X = SO_FIXED_TO_WHOLE( a_AtLeastThreeVertices[ 2 ].m_X );
	s32 vertex2Y = SO_FIXED_TO_WHOLE( a_AtLeastThreeVertices[ 2 ].m_Y );
	
	// Calculate the D value of the edge-equation;
	sofixedpoint edgeD = edgeNormalX * vertex0X + edgeNormalY * vertex0Y;

	// Check what side of the edge vertex 2 is on;
	if ( (edgeNormalX * vertex2X + edgeNormalY * vertex2Y - edgeD) < 0 )
	{
		// The polygon is anti-clockwise;
		return false;
	}
	else
	{
		// The polygon is clockwise;
		return true;
	}
}
// ----------------------------------------------------------------------------
	
// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
