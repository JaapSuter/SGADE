// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMode4PolygonRasterizer.c
	\author		Jaap Suter
	\date		Jun 5 2001
	\ingroup	SoMode4PolygonRasterizer

	See the \a SoMode4PolygonRasterizer module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------
#include "SoSystem.h"
#include "SoMode4Renderer.h"
#include "SoTables.h"
#include "SoMath.h"
#include "SoVector.h"
#include "SoMode4PolygonRasterizer.h"
#include "SoDisplay.h"
#include "SoFont.h"
#include "SoDebug.h"
#include "SoPalette.h"


// ----------------------------------------------------------------------------
// Typedefs 
// ----------------------------------------------------------------------------

/*!
	\brief Structure to contain starting or ending values for
		   a horizontal scanline slice of a polygon.

	\internal
*/
typedef struct
{
	s32 m_X;	//!< \internal Start or ending X value of the scanline.
	s32 m_U;	//!< \internal Start or ending U (texture coordinate component) of the scanline.
	s32 m_V;	//!< \internal Start or ending V (texture coordinate component) of the scanline.

} SoEdgeTableEntry;

// ----------------------------------------------------------------------------
// Static variables
// ----------------------------------------------------------------------------

static u8*				s_TextureData = NULL;	//!< \internal Current texture data used to draw a polygon
static s32				s_TextureUShift;		//!< \internal 2log( texture width )
static s32				s_TextureVShift;		//!< \internal 2log( texture height )
static s32				s_TextureUMask;			//!< \internal Used for texture coord wrapping;
static s32				s_TextureVMask;			//!< \internal Used for texture coord wrapping;

static s32				s_PolygonMinY;	//!< \internal Maximum screen Y of a polygon
static s32				s_PolygonMaxY;	//!< \internal Minimum screen Y of a polygon

//! \internal Table containing the info of the left side 
//			  of the polygon for every horizontal scanline	.
static SoEdgeTableEntry s_EdgeTableL[ SO_SCREEN_HEIGHT ];	

//! \internal Table containing the info of the right side of 
//!			  the polygon for every horizontal scanline.
static SoEdgeTableEntry s_EdgeTableR[ SO_SCREEN_HEIGHT ];

//! True once \a SoMode4PolygonRasterizerInitialize is called.
static bool				s_Initialized = false;


// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

u16*	g_SoMode4PolygonRasterizerBuffer;	//!< \internal Buffer the polygon rasterizer renders to;

// ----------------------------------------------------------------------------
// Forward declarations
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerDrawTexturedPolygonFromEdgeTables( void );
void SoMode4PolygonRasterizerDrawSolidPolygonFromEdgeTables( u32 a_PaletteIndex );


// ----------------------------------------------------------------------------
// Forward declaration that is implemented in assembly and need to be documented here
// by their declaration, because Doxygen can't handle .s files.
// ----------------------------------------------------------------------------

/*!
	\brief Draws a 4 by 4 triangle from the given 10 bit code.

	\internal	Only called from within \a SoMode4PolygonRasterizerDraw4x4Triangle
		
	\param	a_4x4WordAddress		Word (32 bits aligned) address of the top left corner of
									the 4 by 4 pixel rectangle.
	\param	a_WordOffset			Offset (0, 1, 2 or 3) in the above 32 bits word of the top 
									left corder of the rectangle.
	\param	a_QuadPaletteIndex		Quadrupled palette index of the color of the triangle.
									This means that a palette index of 0xAB becomes 0xABABABAB.
	\param	a_TriangleCode			10 bits code of the triangle you want to draw.

	Check the documentation for the \a SoMode4PolygonRasterizerDraw4x4Triangle function
	for more information.
*/
void SoMode4PolygonRasterizerDraw4x4TriangleFromCode(	void* a_4x4WordAddress, u32 a_WordOffset, 
														u32 a_QuadPaletteIndex, u32 a_TriangleCode );

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the backbuffer the rasterizer will render to.
	
	\param	a_Buffer	Pointer to the start of a 240 x 160 x 8 bit buffer.
	
	Note that this buffer (for perfomance reasons) always is a 240 by 160 pixel buffer, 
	where each pixel is 8 bit. I could change this in the future (to support other sizes)
	even without a performance loss, but for now I have other priorities.
*/
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerSetBuffer( void* a_Buffer ) 
{ 
	g_SoMode4PolygonRasterizerBuffer = a_Buffer; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the texture the rasterizer should use for textured polygons.

	\param a_Texture	Texture you want to use.
 */
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerSetTexture( SoImage* a_Texture ) 
{ 
	// Variables;
	s32 textureWidth;
	s32 textureHeight;

	// Maybe we're just disabling texturing;
	if ( a_Texture == NULL )
	{
		s_TextureData = NULL;
		return;
	}

	// Get the texture width and height;
	textureWidth  = SoImageGetWidth(  a_Texture );
	textureHeight = SoImageGetHeight( a_Texture );

	// Assert that the width and height of the image are powers of two, using
	// a dirty 2's-complement trick;	
	SO_ASSERT( (textureHeight & -textureHeight) == textureHeight, 
			   "SoImage structure used as textures should have power of two dimensions." );
	SO_ASSERT( (textureWidth & -textureWidth) == textureWidth, 
			   "SoImage structure used as textures should have power of two dimensions." );

	// Assert that the texture is palettized;
	SO_ASSERT( SoImageIsPalettized( a_Texture ), "Only palettized images can be used as textures." );

	// Calculate the wrapper masks;
	s_TextureUMask = textureWidth  - 1;
	s_TextureVMask = textureHeight - 1;

	// Calculate the texture shifts;
	s_TextureUShift  = 0; while ( textureWidth  >>= 1 ) ++s_TextureUShift; --s_TextureUShift;
	s_TextureVShift  = 0; while ( textureWidth  >>= 1 ) ++s_TextureVShift; --s_TextureVShift;
	
	// Set the texture;
	s_TextureData = (u8*) SoImageGetData( a_Texture ); 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Initializes the rasterizer.

	Make this the first function you call before any other \a SoMode4Rasterizer functions.
	Returns immediately if this method has been called before.

	Make sure you have already called \a SoMode4RendererEnable too.
*/
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerInitialize( void )
{
	// Variables;
	u32 i; // Dummy counter;

	// Return if we are already initialized;
	if ( s_Initialized ) return;

	// Initialize the edge tables;
	for ( i = 0; i < SO_SCREEN_HEIGHT; i++ )
	{
		s_EdgeTableL[ i ].m_X = SO_FIXED_FROM_WHOLE( SO_SCREEN_WIDTH );
		s_EdgeTableR[ i ].m_X = 0;
	}

	// From now on we are initialized;
	s_Initialized = true;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Draws a textured polygon

	\param a_NumVertices			Number of vertices in the polygon.
	\param a_ScreenCoordinates		Array of \a a_NumVertices \a SoVector2 objects, which are the vertices								
									points of the triangle. All three need to be onscreen, in a fixed
									of the polygon. These need to be in fixed point format and 
									on screen. This routine does not do any clipping.
	\param	a_TextureCoordinates	Array of \a a_NumVertices \a SoVector2 objects that represent 
									the texture coordinates of the polygon.

	\warning	This function is is rather slow. It was the first thing I've ever written
				for the GBA and some things just need to be done better befor you can use this
				in an a commercial or otherwise important product. For example, you shouldn't be
				drawing polygons, but triangles. This routine uses edge tables, which means slow 
				nasty memory reads everywhere. Besides, it's not affine, thus slow. 
				
				If you really want a fast texturemapper, keep bugging me and I'll write you one.
				Or even better, write it yourself (in assembly ofcourse) and submit it for the
				SGADE.
*/
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerDrawTexturedPolygon( u32 a_NumVertices, 
												  SoVector2* a_ScreenCoordinates, 
												  SoVector2* a_TextureCoordinates
												)
{
	// Variables to hold the start and end of one edge;
	// These are initialized to the edge that has vertex 0 as
	// the end;
	SoVector2* vertA = &a_ScreenCoordinates[ a_NumVertices - 1 ];
	SoVector2* vertB = &a_ScreenCoordinates[ 0 ];
	SoVector2* textA = &a_TextureCoordinates[ a_NumVertices - 1 ];
	SoVector2* textB = &a_TextureCoordinates[ 0 ];

	// Variables to hold the bottom and top of an edge;
	SoVector2 *topVert;
	SoVector2 *botVert;
	SoVector2 *topText;
	SoVector2 *botText;

	// Pointers to left and right entries in the edge tables;
	SoEdgeTableEntry *edgeL;
	SoEdgeTableEntry *edgeR;
		
	// Other variables;
	u32 i;						// Dummy counter;
	s32	rX, rU, rV;				// Real values;
	s32	tX, tU, tV;				// Tangent values;
	s32	dY, oodY;				// Delta Y and one-over-delta Y value;
	s32 topY, botY;				// Whole top and bot Y values of one edge;

	// Reset the min and max y values of this polygon
	s_PolygonMinY = SO_SCREEN_HEIGHT; 
	s_PolygonMaxY = 0;

	// Trace each edge into the edge tables;
	for ( i = 1; i < (a_NumVertices + 1); i++ )
	{
		// Find out what top and bottom of the edge is;
		if ( vertA->m_Y > vertB->m_Y )
		{
			topVert = vertB;
			botVert = vertA;
			topText = textB;
			botText = textA;
		}
		else
		{
			topVert = vertA;
			botVert = vertB;
			topText = textA;
			botText = textB;
		}

		// Calculate the dY;
		dY = botVert->m_Y - topVert->m_Y;

		// If dY is smaller then fixed one we can skip this edge;
		if ( dY < SO_FIXED_FROM_WHOLE( 1 ) ) 
		{
			// Go to the next edge;
			vertA = vertB;
			textA = textB;
			vertB = &a_ScreenCoordinates[ i ];
			textB = &a_TextureCoordinates[ i ];		

			// Skip the rest of this iteration;
			continue;
		}

		// Calculate one-over-dY;
		oodY = SO_FIXED_ONE_OVER_FAST_INACCURATE( dY );

		// Calculate the tangents;
		tX = SO_FIXED_MULTIPLY_BIG_SMALL( (botVert->m_X - topVert->m_X), oodY );
		tU = SO_FIXED_MULTIPLY_BIG_SMALL( (botText->m_X - topText->m_X), oodY );
		tV = SO_FIXED_MULTIPLY_BIG_SMALL( (botText->m_Y - topText->m_Y), oodY );

		// Set initial X, U and V;
		rX = topVert->m_X;
		rU = topText->m_X;
		rV = topText->m_Y;

		// Calculate whole top and bot Y;
		topY = SO_FIXED_TO_WHOLE( topVert->m_Y );
		botY = SO_FIXED_TO_WHOLE( botVert->m_Y );

		// Are these top and bot Y values the maximum and minimum?
		if ( topY < s_PolygonMinY ) s_PolygonMinY = topY;
		if ( botY > s_PolygonMaxY ) s_PolygonMaxY = botY;

		// Calculate whole delta Y;
		dY = botY - topY;

		// Multiply the texture values by the texture width and height
		// to go from [0..1] range to [0..width or height] range;
		tU <<= s_TextureUShift;
		rU <<= s_TextureUShift;
		tV <<= s_TextureVShift;
		rV <<= s_TextureVShift;

		// Get a pointer to the the start of the edge tables;
		edgeL = &s_EdgeTableL[ topY ];
		edgeR = &s_EdgeTableR[ topY ];

		// Trace this single edge;
		for ( ; dY != 0; dY-- )
		{
			// Add them in the edge-tables if neccesary;
			if ( rX > edgeR->m_X ) 
			{
				edgeR->m_X = rX;
				edgeR->m_U = rU;
				edgeR->m_V = rV;
			}
			if ( rX < edgeL->m_X ) 
			{
				edgeL->m_X = rX;
				edgeL->m_U = rU;
				edgeL->m_V = rV;
			}

			// Add the tangents;
			rX += tX;
			rU += tU;
			rV += tV;

			// Go to the next entry in the edge-table;
			edgeL++;
			edgeR++;
		}			
		
		// Go to the next edge;
		vertA = vertB;
		textA = textB;
		vertB = &a_ScreenCoordinates[ i ];
		textB = &a_TextureCoordinates[ i ];		
	}

	// If there is a polygon;
	if ( s_PolygonMinY < s_PolygonMaxY )
	{
		// Draw it from the filled edge tables;	
		SoMode4PolygonRasterizerDrawTexturedPolygonFromEdgeTables();
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Draws a textured polygon from the information in the edge tables.

	\internal

	This function is called by \a SoMode4PoygonRasterizerDrawTexturedPolygon after that 
	function has filled the edge tables.
*/
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerDrawTexturedPolygonFromEdgeTables( void )
{
	// Pointers;
	u16				 *pixels;
	u8				 *texture		= s_TextureData;
	u16				 *scanline		= g_SoMode4PolygonRasterizerBuffer 
									+ SO_SCREEN_HALF_WIDTH_MULTIPLY( s_PolygonMinY );
	SoEdgeTableEntry *edgeL			= &s_EdgeTableL[ s_PolygonMinY ];
	SoEdgeTableEntry *edgeR			= &s_EdgeTableR[ s_PolygonMinY ];
	
	// Other variables;
	s32 dX, oodX;			// Delta X and one-over-delta X value;
	s32 tU, tV;				// Tangent values;
	s32	iU, iV;				// Whole U and V;
	s32 rU, rV, rY;			// Real values;		
	s32 lX, rX;				// Values that hold the whole left and right of a line;
	u16	paletteIndices;		// Value that can hold two palette indices;

	// Draw each horizontal line of the polygon;
	for ( rY = s_PolygonMaxY - s_PolygonMinY; rY != 0; rY-- )
	{
		// Get the left and right whole values;
		lX = SO_FIXED_TO_WHOLE( edgeL->m_X );
		rX = SO_FIXED_TO_WHOLE( edgeR->m_X );

		// Calculate the fixed length of the line;
		dX = (edgeR->m_X - edgeL->m_X);

		// If the length is at least something;
		if ( dX > SO_FIXED_FROM_WHOLE( 1 ) )
		{
			// Get a pointer to the first two pixels;
			pixels = scanline + (lX >> 1);
		
			// Calculate one over deltaX;
			oodX = SO_FIXED_ONE_OVER_FAST_INACCURATE( dX );

			// Calculate the tangents;
			tU = SO_FIXED_MULTIPLY_BIG_SMALL( (edgeR->m_U - edgeL->m_U), oodX );
			tV = SO_FIXED_MULTIPLY_BIG_SMALL( (edgeR->m_V - edgeL->m_V), oodX );

			// Set the initial u and v values;
			rU = edgeL->m_U;
			rV = edgeL->m_V;

			// If the line starts at uneven pixel, we
			// need to take care of it;
			if ( SO_NUMBER_IS_UNEVEN( lX ) )
			{
				// Convert to whole texture coordinates;
				iU = SO_FIXED_TO_WHOLE( rU );
				iV = SO_FIXED_TO_WHOLE( rV );

				// Wrap the texture coordinates;
				iU &= s_TextureUMask;
				iV &= s_TextureVMask;

				// Create the second palette index of the two-pixel value;
				paletteIndices = (u16)texture[ (iV << s_TextureUShift) + iU ] << 8;

				// Mask out the second pixel;
				*pixels &= 0x00FF;

				// Set the second pixel;
				*pixels |= paletteIndices;

				// Go to the next pixel;
				pixels++;

				// Add the tangents;
				rU += tU;
				rV += tV;

				// Advance the left X value one pixel;
				lX++;
			}

			// Calculate the length in the number of pixel-duos;
			dX = (rX -lX) >> 1;
			
			// Iterate over the line;
			while ( dX-- )
			{
				// Convert to whole texture coordinates;
				iU = SO_FIXED_TO_WHOLE( rU );
				iV = SO_FIXED_TO_WHOLE( rV );

				// Wrap the texture coordinates;
				iU &= s_TextureUMask;
				iV &= s_TextureVMask;

				// Create the first palette index of the two-pixel value;
				paletteIndices = (u16)texture[ (iV << s_TextureUShift) + iU ];

				// Add the tangents;
				rU += tU;
				rV += tV;

				// Convert to whole texture coordinates;
				iU = SO_FIXED_TO_WHOLE( rU );
				iV = SO_FIXED_TO_WHOLE( rV );

				// Wrap the texture coordinates;
				iU &= s_TextureUMask;
				iV &= s_TextureVMask;

				// Create the second palette index of the two-pixel value;
				paletteIndices |= (((u16)texture[ (iV << s_TextureUShift) + iU ]) << 8);

				// Put the palette-indices in the buffer;
				*pixels = paletteIndices;

				// Go to the next pixel;
				pixels++;

				// Add the tangents;
				rU += tU;
				rV += tV;				

			}
			
			// If the line ends at an uneven pixel we need to take 
			// care of the last pixel;
			if ( SO_NUMBER_IS_UNEVEN( rX ) )
			{
				// Convert to whole texture coordinates;
				iU = SO_FIXED_TO_WHOLE( rU );
				iV = SO_FIXED_TO_WHOLE( rV );

				// Wrap the texture coordinates;
				iU &= s_TextureUMask;
				iV &= s_TextureVMask;

				// Create the first palette index of the two-pixel value;
				paletteIndices = (u16)texture[ (iV << s_TextureUShift) + iU ];

				// Mask out the first pixel;
				*pixels &= 0xFF00;

				// Set the second pixel;
				*pixels |= paletteIndices;
			}
		}

		// Go to the next scanline;
		scanline += SO_SCREEN_HALF_WIDTH;
		
		// Restore the edge tables to their initial values;
		(edgeL++)->m_X = SO_FIXED_FROM_WHOLE( SO_SCREEN_WIDTH );
		(edgeR++)->m_X = 0;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Draws a solid single color polygon

	\param a_NumVertices			Number of vertices in the polygon.
	\param a_ScreenCoordinates		Array of \a a_NumVertices \a SoVector2 objects, which are the vertices								
									points of the triangle. All three need to be onscreen, in a fixed
									of the polygon. These need to be in fixed point format and 
									on screen. This routine does not do any clipping.
	\param	a_PaletteIndex			Index in the screen palette to the color you want to draw the
									polygon with.

	\warning	This function is is rather slow. It was the second thing I've ever written
				for the GBA and some things just need to be done better befor you can use this
				in an a commercial or otherwise important product. For example, you shouldn't be
				drawing polygons, but triangles. This routine uses edge tables, which means slow 
				nasty memory reads everywhere. 

				If you really want a fast polyfiller, subdivide your polygons in triangles
				and use \a SoMode4RasterizerDrawSolidTriangle instead.
*/
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerDrawSolidPolygon( u32 a_NumVertices, 
											   SoVector2* a_ScreenCoordinates, 
											   u32 a_PaletteIndex 
											 )	
{
	// Variables to hold the start and end of one edge;
	// These are initialized to the edge that has vertex 0 as
	// the end;
	SoVector2* vertA = &a_ScreenCoordinates[ a_NumVertices - 1 ];
	SoVector2* vertB = &a_ScreenCoordinates[ 0 ];
	
	// Variables to hold the bottom and top of an edge;
	SoVector2 *topVert;
	SoVector2 *botVert;
	
	// Pointers to left and right entries in the edge tables;
	SoEdgeTableEntry *edgeL;
	SoEdgeTableEntry *edgeR;
		
	// Other variables;
	u32 i;						// Dummy counter;
	s32	rX;						// Real values;
	s32	tX;						// Tangent values;
	s32	dY, oodY;				// Delta Y and one-over-delta Y value;
	s32 topY, botY;				// Whole top and bot Y values of one edge;

	// Reset the min and max y values of this polygon
	s_PolygonMinY = SO_SCREEN_HEIGHT; 
	s_PolygonMaxY = 0;

	// Trace each edge into the edge tables;
	for ( i = 1; i < (a_NumVertices + 1); i++ )
	{
		// Find out what top and bottom of the edge is;
		if ( vertA->m_Y > vertB->m_Y )
		{
			topVert = vertB;
			botVert = vertA;
		}
		else
		{
			topVert = vertA;
			botVert = vertB;
		}

		// Calculate the dY;
		dY = botVert->m_Y - topVert->m_Y;

		// If dY is smaller then fixed one we can skip this edge;
		if ( dY < SO_FIXED_FROM_WHOLE( 1 ) ) 
		{
			// Go to the next edge;
			vertA = vertB;
			vertB = &a_ScreenCoordinates[ i ];

			// Skip the rest of this iteration;
			continue;
		}

		// Calculate one-over-dY;
		oodY = SO_FIXED_ONE_OVER_FAST_INACCURATE( dY );

		// Calculate the X tangent;
		tX = SO_FIXED_MULTIPLY_BIG_SMALL( (botVert->m_X - topVert->m_X), oodY );

		// Set initial X;
		rX = topVert->m_X;
		
		// Calculate whole top and bot Y;
		topY = SO_FIXED_TO_WHOLE( topVert->m_Y );
		botY = SO_FIXED_TO_WHOLE( botVert->m_Y );

		// Are these top and bot Y values the maximum and minimum?
		if ( topY < s_PolygonMinY ) s_PolygonMinY = topY;
		if ( botY > s_PolygonMaxY ) s_PolygonMaxY = botY;

		// Calculate whole delta Y;
		dY = botY - topY;

		// Get a pointer to the the start of the edge tables;
		edgeL = &s_EdgeTableL[ topY ];
		edgeR = &s_EdgeTableR[ topY ];

		// Trace this single edge;
		while ( dY-- )
		{
			// Add the X in the edge-tables if neccesary;
			if ( rX > edgeR->m_X ) 
			{
				edgeR->m_X = rX;
			}
			if ( rX < edgeL->m_X ) 
			{
				edgeL->m_X = rX;
			}

			// Add the tangent;
			rX += tX;
			
			// Go to the next entry in the edge-table;
			edgeL++;
			edgeR++;
		}			
		
		// Go to the next edge;
		vertA = vertB;
		vertB = &a_ScreenCoordinates[ i ];
	}

	// If there is a polygon;
	if ( s_PolygonMinY < s_PolygonMaxY )
	{
		// Draw it;
		SoMode4PolygonRasterizerDrawSolidPolygonFromEdgeTables( a_PaletteIndex );
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Draws a solid polygon from the information in the edge tables.

	\internal

	This function is called by \a SoMode4PoygonRasterizerDrawSolidPolygon after that 
	function has filled the edge tables.
*/
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerDrawSolidPolygonFromEdgeTables( u32 a_PaletteIndex )
{
	// Pointers;
	u16				 *pixels;
	u16				 *scanline		= g_SoMode4PolygonRasterizerBuffer 
									+ SO_SCREEN_HALF_WIDTH_MULTIPLY( s_PolygonMinY );
	SoEdgeTableEntry *edgeL			= &s_EdgeTableL[ s_PolygonMinY ];
	SoEdgeTableEntry *edgeR			= &s_EdgeTableR[ s_PolygonMinY ];
	
	// Other variables;
	s32 dX;					// Delta X;
	s32 rY;					// Real values;		
	s32 lX, rX;				// Values that hold the whole left and right of a line;
	
	// Value that holds two pixels;
	u16	paletteIndices = a_PaletteIndex | (a_PaletteIndex << 8 );

	// Draw each horizontal line of the polygon;
	for ( rY = s_PolygonMaxY - s_PolygonMinY; rY != 0; rY-- )
	{
		// Get the left and right whole values;
		lX = SO_FIXED_TO_WHOLE( edgeL->m_X );
		rX = SO_FIXED_TO_WHOLE( edgeR->m_X );

		// Calculate the fixed length of the line;
		dX = (edgeR->m_X - edgeL->m_X);

		// If the length is at least something;
		if ( dX > SO_FIXED_FROM_WHOLE( 1 ) )
		{
			// Get a pointer to the first two pixels;
			pixels = scanline + (lX >> 1);
		
			// If the line starts at uneven pixel, we
			// need to take care of it;
			if ( SO_NUMBER_IS_UNEVEN( lX ) )
			{
				// Mask out the second pixel;
				*pixels &= 0x00FF;

				// Set the second pixel;
				*pixels |= a_PaletteIndex << 8;

				// Go to the next pixel;
				pixels++;

				// Advance the left X value one pixel;
				lX++;
			}

			// Calculate the number of pixel-duos, (half scanline length);
			dX = (rX -lX) >> 1;

			// Iterate over the line;
			while ( dX-- )
			{
				// Put the palette-indices in the buffer;
				*pixels = paletteIndices;
			
				// Go to the next pixel;
				pixels++;
			}
			
			// If the line ends at an uneven pixel we need to take 
			// care of the last pixel;
			if ( SO_NUMBER_IS_UNEVEN( rX ) )
			{
				// Mask out the first pixel;
				*pixels &= 0xFF00;

				// Set the second pixel;
				*pixels |= a_PaletteIndex;
			}
		}

		// Go to the next scanline;
		scanline += SO_SCREEN_HALF_WIDTH;
		
		// Restore the edge tables to their initial values;
		(edgeL++)->m_X = SO_FIXED_FROM_WHOLE( SO_SCREEN_WIDTH );
		(edgeR++)->m_X = 0;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief C version of the assembly solid triangle rasterizer;

	I actually implemented this one first, and then converted it to assembly;
*/
// ----------------------------------------------------------------------------
void SoMode4PolygonRasterizerDrawSolidTriangleC( SoVector2 a_Triangle[ 3 ], u32 a_PaletteIndex )
{
	// Variables;
	SoVector2* v0 = &a_Triangle[ 0 ]; 
	SoVector2* v1 = &a_Triangle[ 1 ];
	SoVector2* v2 = &a_Triangle[ 2 ];
	SoVector2* vT; // Temporary vector for swapping;

	s32 integerDeltaY;
	s32 integerLeftX;
	s32 integerRightX;
	sofixedpoint fixedLeftX, fixedRightX;
	
	s32 scanlineLength;

	s32 yShift;
	
	sofixedpoint tangentLeftX, tangentRightX;

	sofixedpoint ooQ;

	u16* scanline;
	u16* pixel;
	
	bool middleVertexRight = true;

	bool bottomPartDone = false;

	// Double the palette index (we're plotting double pixels);
	a_PaletteIndex |= a_PaletteIndex << 8;

	// Sort the vertices in top to bottom Y-order, also checking on what side 
	// the middle vertex lies;
	if ( v0->m_Y > v1->m_Y ) { vT = v0; v0 = v1; v1 = vT; middleVertexRight = !middleVertexRight; }
	if ( v1->m_Y > v2->m_Y ) { vT = v1; v1 = v2; v2 = vT; middleVertexRight = !middleVertexRight; }
	if ( v0->m_Y > v1->m_Y ) { vT = v0; v0 = v1; v1 = vT; middleVertexRight = !middleVertexRight; }

	// Calculate the delta Y, using a top left filling convention;
	integerDeltaY = SO_FIXED_CEIL_WHOLE( v1->m_Y ) - SO_FIXED_CEIL_WHOLE( v0->m_Y );

	// check for degenerate triangles
	if(!integerDeltaY)
	{
		if(SO_FIXED_CEIL_WHOLE( v2->m_Y ) - SO_FIXED_CEIL_WHOLE( v0->m_Y ) == 0)
			return;
	}

	// Calculate the tangents;
	if ( middleVertexRight )
	{
		         ooQ = SO_FIXED_ONE_OVER_FAST_INACCURATE(v2->m_Y - v0->m_Y);
		tangentLeftX = SO_FIXED_MULTIPLY( (v2->m_X - v0->m_X), ooQ );


		if ( integerDeltaY ) 
		{
			          ooQ = SO_FIXED_ONE_OVER_FAST_INACCURATE(v1->m_Y - v0->m_Y);
			tangentRightX = SO_FIXED_MULTIPLY( (v1->m_X - v0->m_X), ooQ );
		}			
	}
	else
	{
		if ( integerDeltaY ) 
		{
			         ooQ = SO_FIXED_ONE_OVER_FAST_INACCURATE(v1->m_Y - v0->m_Y);
			tangentLeftX = SO_FIXED_MULTIPLY( (v1->m_X - v0->m_X), ooQ );
		}

		          ooQ = SO_FIXED_ONE_OVER_FAST_INACCURATE(v2->m_Y - v0->m_Y);
		tangentRightX = SO_FIXED_MULTIPLY( (v2->m_X - v0->m_X), ooQ );
	}

	// Calculate the start X values;
	fixedLeftX = fixedRightX = v0->m_X;

	// Calculate the starting scanline pointer;
	scanline = g_SoMode4PolygonRasterizerBuffer + SO_SCREEN_HALF_WIDTH * SO_FIXED_CEIL_WHOLE( v0->m_Y );
	
	// Subpixel correct the starting X values;
	yShift = SO_FIXED_FROM_WHOLE( SO_FIXED_CEIL_WHOLE( v0->m_Y ) ) - v0->m_Y;
	fixedLeftX  += SO_FIXED_MULTIPLY( tangentLeftX,  yShift );
	fixedRightX += SO_FIXED_MULTIPLY( tangentRightX, yShift );

	// Break out after the second triangle half;
	while ( true )
	{		
		// Iterate over the triangle-half;
		while ( integerDeltaY-- )
		{
			// Get the scanline start and end;
			integerLeftX   = SO_FIXED_CEIL_WHOLE( fixedLeftX  );
			integerRightX  = SO_FIXED_CEIL_WHOLE( fixedRightX );

			// Get the length;
			scanlineLength = integerRightX - integerLeftX; 

			// Is there a scanline
			if ( scanlineLength > 0 )
			{		
				// Get the pixel pointer on a 16 bit boundary;
				pixel = scanline + (integerLeftX >> 1);
				
				// Is there a funky first pixel;
				if ( integerLeftX & 1 ) 
				{
					*pixel++ |= 0xFF00 & a_PaletteIndex;
					--scanlineLength;
				}

				// From now on, we plot two pixels at once;
				scanlineLength >>= 1;

				// Draw the line;
				while ( scanlineLength-- ) *pixel++ = a_PaletteIndex;

				// Is there a funky last pixel;
				if ( integerRightX & 1 ) *pixel |= 0x00FF & a_PaletteIndex;
			}

			// Go to the next line;
			scanline += SO_SCREEN_HALF_WIDTH;		

			// Add the tangents;
			fixedLeftX	+= tangentLeftX;
			fixedRightX	+= tangentRightX;
		}	

		// Is the bottom half done;
		if ( bottomPartDone ) return; else bottomPartDone = true;

		// Calculate the height of the bottom half;
		integerDeltaY = SO_FIXED_CEIL_WHOLE( v2->m_Y ) - SO_FIXED_CEIL_WHOLE( v1->m_Y );

		// Maybe we're done already;
		if ( ! integerDeltaY ) return;

		// Setup the bottom half;
		if ( middleVertexRight )
		{
			          ooQ = SO_FIXED_ONE_OVER_FAST_INACCURATE(v2->m_Y - v1->m_Y);
			tangentRightX = SO_FIXED_MULTIPLY( (v2->m_X - v1->m_X), ooQ );
			
			fixedRightX = v1->m_X;
	
			yShift = SO_FIXED_FROM_WHOLE( SO_FIXED_CEIL_WHOLE( v1->m_Y ) ) - v1->m_Y;
			fixedRightX += SO_FIXED_MULTIPLY( tangentRightX, yShift );
		}
		else
		{
			         ooQ = SO_FIXED_ONE_OVER_FAST_INACCURATE(v2->m_Y - v1->m_Y);
			tangentLeftX = SO_FIXED_MULTIPLY( (v2->m_X - v1->m_X), ooQ );

			fixedLeftX = v1->m_X;
		
			yShift = SO_FIXED_FROM_WHOLE( SO_FIXED_CEIL_WHOLE( v1->m_Y ) ) - v1->m_Y;
			fixedLeftX += SO_FIXED_MULTIPLY( tangentLeftX, yShift );
		}
	} 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
