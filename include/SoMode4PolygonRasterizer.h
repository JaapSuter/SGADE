// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMode4PolygonRasterizer.h
	\author		Jaap Suter
	\date		Jun 5 2001
	\ingroup	SoMode4PolygonRasterizer

	See the \a SoMode4PolygonRasterizer module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_MODE_4_POLYGON_RASTERIZER_H
#define SO_MODE_4_POLYGON_RASTERIZER_H

#ifdef __cplusplus
extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoMode4PolygonRasterizer SoMode4PolygonRasterizer 
	\brief	  Functionality for all mode 4 polygon and triangle rendering

	Singleton
	
	Module containing all routines to draw textured and solid and special case
	polygons and triangles. If you want kick ass double buffered full
	resolution 3D graphics on your Gameboy Advance, use this module.
	
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoVector.h"
#include "SoImage.h"

// --------------------------------------------------------------------------
// Public functions;
// --------------------------------------------------------------------------

void  SoMode4PolygonRasterizerInitialize(			void );

void  SoMode4PolygonRasterizerSetBuffer(			void* a_Buffer );

void  SoMode4PolygonRasterizerSetTexture(			SoImage* a_Texture );

void SoMode4PolygonRasterizerDrawTexturedPolygon(	u32		   a_NumVertices, 
													SoVector2* a_ScreenCoordinates, 
													SoVector2* a_TextureCoordinates );

void SoMode4PolygonRasterizerDrawSolidPolygon(		u32 a_NumVertices, SoVector2* a_ScreenCoordinates, 
													u32 a_PaletteIndex );
void SoMode4PolygonRasterizerDrawSolidTriangleC( SoVector2 a_Triangle[ 3 ], u32 a_PaletteIndex );

// --------------------------------------------------------------------------
// Public functions implemented in assembly. These are only documented here
// only because doxygen can't handle documentation in .s files.
// --------------------------------------------------------------------------

/*!
	\brief Draws a solid filled 2D triangle.
	
	\param a_Triangle       Array of 3 \a SoVector2 objects, which are the
	                        three corner points of the triangle. All three need
	                        to be onscreen, in a fixed point format. This
	                        routine does not do any clipping.
	\param a_PaletteIndex	Color you want to draw the triangle with.

	This routine draws a filled (flat or solid or whatever you want to call it)
	triangle in the current \a SoMode4PolygonRasterizer backbuffer. This
	backbuffer is set by using the \a SoMode4PolygonRasterizerSetBuffer
	function.
*/
void SoMode4PolygonRasterizerDrawSolidTriangle(		SoVector2 a_Triangle[ 3 ], 
													u32		  a_PaletteIndex );

// --------------------------------------------------------------------------
// Private attributes;
// Sadly these need to be global cause we need them in the .c and in the .s
// implementation. I could put them in another .h file, but that would result 
// in more files which is even more ugly. Got any better ideas?
// --------------------------------------------------------------------------

/*!
	\brief Pointer the polygon routines will render to. 

	\internal
	
	Always equal to the current mode 4 backbuffer. This is actually set 
	by \a SoMode4Renderer when you do \a SoMode4RendererInitialize 
	or \a SoMode4RendererFlip.

	\warning This variable is only global because I need it in the .C and in
	         the .S file.  Otherwise I would have declared it in the .C file
	         instead. Sorry.
*/
extern u16* g_SoMode4PolygonRasterizerBuffer;


// --------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
