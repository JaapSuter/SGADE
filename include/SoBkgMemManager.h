// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgMemManager.h
	\author		Mark Price
	\date		June 16 2002
	\ingroup	SoBkgMemManager

	See the \a SoBkgMemManager module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_BKG_MEM_MANAGER_H
#define SO_BKG_MEM_MANAGER_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoBkgMemManager SoBkgMemManager
	\brief	  Manages background memory.

	Singleton

	This module handles the character data for backgrounds. It loads data into
	the BKG VRAM area as needed by the \a SoBkg and \a SoBkgMap modules.
	So before you start using \a this module read about these other two modules
	too.

	This implementation is closely based on Rafael Baptista's GDC 2001 talk on
	GBA resource management.  Recordings of this session can be purchased from
	The Sound Of Knowledge (www.tsok.net - product code 2001-GDC-014) for $11
	plus shipping & handling (and tax if you live in California).

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoTileSet.h"

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

void SoBkgMemManagerPreInitialize(void);
void SoBkgMemManagerAllocTileSet(const SoTileSet* a_TileSet, u32 a_CharBase, u32 a_BaseIdx);
void SoBkgMemManagerAllocBkg(u32 a_Options);
void SoBkgMemManagerInitialize(void);

void SoBkgMemManagerSetTileSet(const SoTileSet *a_TileSet, u16 *a_TileMapBuffer);

u16  SoBkgMemManagerAllocTile(u32 a_TileIdx);
void SoBkgMemManagerFreeTile(u32 a_TileIdx);

u16  SoBkgMemManagerAllocTile16(u32 a_TileIdx);
void SoBkgMemManagerFreeTile16(u32 a_TileIdx);

/*
void SoBkgMemManagerAllocBkg(u32 a_Options);
void SoBkgMemManagerFreeBkg(u32 a_Options);

void SoBkgMemManagerAllocTileSet(const SoTileSet* a_This, u32 a_CharBase, u32 a_BaseIdx);
void SoBkgMemManagerFreeTileSet(const SoTileSet* a_This, u32 a_CharBase, u32 a_BaseIdx);
*/

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_BKG_MEM_MANAGER_H
