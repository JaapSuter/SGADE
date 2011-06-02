// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMode4Renderer.h
	\author		Jaap Suter
	\date		June 24 2001
	\ingroup	SoMode4Renderer

	See the \a SoMode4Renderer module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_RENDERER_H
#define SO_RENDERER_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoMode4Renderer SoMode4Renderer
	\brief	  Handles all basic mode 4 rendering.

	Singleton
	
	For polygon rendering use the \a SoMode4PolygonRenderer
	
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoMode4PolygonRasterizer.h"
#include "SoFont.h"

// --------------------------------------------------------------------------
// Functions implemented in C (documentation in .C file).
// --------------------------------------------------------------------------

void SoMode4RendererEnable( void );

void SoMode4RendererFlip( void );

void SoMode4RendererClearFrontAndBackBuffer( void );

void* SoMode4RendererGetBuffer( void );

void SoMode4RendererDrawImage(			  const SoImage* a_Image );
void SoMode4RendererDrawTransparentImage( const SoImage* a_Image );
void SoMode4RendererDrawCrossFadeImage(	  const SoImage* a_Image0, const SoImage* a_Image1, u32 a_Fade );

void SoMode4RendererDrawPixel( u32 a_X, u32 a_Y, u32 a_PalIndex );

void SoMode4RendererDrawNumber( u32 a_LeftX, u32 a_RightX, u32 a_TopY, s32   a_Number, const SoImage* a_Font );
void SoMode4RendererDrawString( u32 a_LeftX, u32 a_RightX, u32 a_TopY, char* a_String, const SoImage* a_Font );


// --------------------------------------------------------------------------
// Functions implemented in assembly (documentation over here
// cause Doxygen doesn't like assembly).
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
/*!
	\brief Clears the current mode4 backbuffer to palette index zero.

	Remember, it's always better to just draw on the entire backbuffer, so you
	won't have to clear it.
*/
// --------------------------------------------------------------------------
void SoMode4RendererClear( void );

// --------------------------------------------------------------------------
// EOF;
// --------------------------------------------------------------------------


//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
