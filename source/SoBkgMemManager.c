// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgMemManager.c
	\author		Mark T. Price
	\date		April 3, 2003
	\ingroup	SoBkgMemManager

	See the \a SoBkgMemManager module for more information.

	\todo  Add support for rot/scale tiles (from tile modes 2 and 3)

	\todo  Add support for allocation of tile sets with charbase values other than 0

	\todo  We could instead split the memory up by assigning ranges to several
	       Tile caches.  Each would allocate tiles from their area, and maps
	       would be assigned to a specific tile cache.  In this case, the
	       SoBkgMemManager would also be called to allocate memory for map
	       data, instead of hard-coding specific ranges for each map.  Map
	       memory would be marked as used with another key value in the
	       RamToRom map (which would be shared among all active Tile caches).
	       This even allows the user to manage the tiles directly if they so
	       choose.

	\todo  if all tiles fit into memory, we could allow for them to be
	       pre-loaded instead of demand-loaded.  This would have to be a flag
	       on the tile cache setup call.  It would probably be a better bet to
	       just let the user call into \a SoTileSet to pre-load the tiles.

*/
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoBkgMemManager.h"
#include "SoBkg.h"
#include "SoMath.h"
#include "SoDMA.h"

// A single text mode BKG screen may refer to up to 1024 different tiles,
// all of which must be contiguous off of a single base address.  The base
// address is a multiple of 16K inside of the BKG Video Ram.  The screen
// maps themselves are allocated on 2K boundaries in BKG Video Ram.
//
// 1024 256 color tiles would take up all of BKG Video Ram, but 1024
// 16 color tiles only take up half (or 32K).
//
// In its current state, this module only supports allocating tiles relative
// to char base address 0.
//
// In order to maximize the number of accessable 16 color tiles, we
// choose to base all tiles at address 0 in BKG Video Ram.  The maps
// are allocated at the end of Video Ram: 4*2K or the last 8K (each
// map will be 256x256 pixels).
// 
// This results in the following BKG memory map:
// 
// +------------------------+  06000000
// |                        |
// |                        |
// |                        |
// +  16 & 256 color tiles  +  06004000
// |                        |
// |                        |
// |                        |
// | - - - - - - - - - - - -|  06008000
// |                        |
// |                        |
// |    256 color tiles     |
// +                        +  0600C000
// |                        |
// | - - - - - - - - - - - -|  0600E000,
// | BKG0  BKG1  BKG2  BKG3 |      0600E800, 0600F000, 0600F800
// +------------------------+  06010000
//
// Note that the BKGs are allocated at the end of BG Video Ram.  This
// is a requirement of the current implementation of this module.  If BKGs
// are located at other addresses in Video Ram, they may be overwritten
// by dynamically loaded tiles.
//
// In this layout, the maximum number of 16 color tiles is 1024, the
// maximum number of 256 color tiles is 896.  The largest possible number
// of tiles overall is 1408 tiles (1024 16 color tiles plus 384 256 color
// tiles).  Note, however, that the SO_BKG_MAX_TILES below is not 1408.
// This is because we are using one pair of entries in the table to refer
// to a single 256 color tile, and just one to refer to a 16 color tile.
// This results in the new pseudo-max number of 896*2 or 1792.
//
// For convenience, tile 0 is always assumed to be a fully transparent
// tile in both 16 and 256 color modes.  Because of this, we never load
// or release tile 0 and never change its reference count.  Make sure you
// don't put data in tile 0 of your tile set or it will never be displayed.

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

//! \internal	this is BKG Video Ram where the ram tiles are stored for display
#define SO_BKG_DATA     ((volatile u16*)(0x06000000))

//! \internal	Maximum number of bkg tiles in VRAM.
#define SO_BKG_MAX_TILES 2048

//! \internal	Maximum valid tile index for text mode tiles
#define SO_BKG_MAX_TILE_IDX 1024

//! \internal	Magic number used to identify tiles that have not been loaded into VRAM
#define SO_BKG_TILE_NOT_LOADED 0xffff

//! \internal	Magic number used to identify VRAM that is used for the 2nd half of a
//!				256 color tile
#define SO_BKG_TILE_USED_256   0xfffe

//! \internal	Magic number used to identify VRAM that has been reserved.  Reserved
//!				tiles will never be allocated
#define SO_BKG_TILE_USED_RESERVED   0xfffd

//! \internal	Magic number used to identify the end of the VRAM free-tile list
#define SO_BKG_TILE_LIST_END   0xffff

// Macros ---------------------------------------------------------------------

//! \internal	This macro converts the ram index of a 16-color tile to the tile offset.
//!             Note: the offset is in words, not bytes.
#define SO_BKG_TILEIDX_TO_OFFSET16(x) (x<<4)

//! \internal	This macro converts the ram index of a 256-color tile to the tile offset.
//!             Note: the offset is in words, not bytes.
#define SO_BKG_TILEIDX_TO_OFFSET256(x) (x<<5)

//! \internal	This helper macro returns TRUE if the tile slot in VRAM is unused
#define SO_BKG_RAM_SLOT_IS_FREE(x)      (s_RamToTilesetIdx[x]    == SO_BKG_TILE_NOT_LOADED)

//! \internal	This helper macro returns TRUE if the 256 color tile has been mapped in RAM
#define SO_BKG_ROM_TILE_IS_LOADED(x)    (s_TilesetToRamIdx256[x] != SO_BKG_TILE_NOT_LOADED)

//! \internal	This helper macro returns TRUE if the 16 color tile has been mapped in RAM
#define SO_BKG_ROM_TILE16_IS_LOADED(x)  (s_TilesetToRamIdx16[x]  != SO_BKG_TILE_NOT_LOADED)


// ----------------------------------------------------------------------------
// Static variables
// ----------------------------------------------------------------------------

//! \internal  VRAM tile number to TileSet index translation table.  This
//!            table is used to lookup the tile number in the TileSet given
//!            the tile number in VRAM.  If the tile is not in use, the entry
//!            will be SO_BKG_TILE_NOT_LOADED.  If the tile is an 8-bit tile
//!				then the second entry is SO_BKG_TILE_USED_256;
static u16 s_RamToTilesetIdx[SO_BKG_MAX_TILES];

//! \internal  VRAM tile reference count.  For each tile in VRAM, this table
//!            contains the number of times the tile is currently being used
//!            (e.g. the number of times it appears in the displayed portion
//!            of an active map).  If the tile is not currently in use, the
//!            entry is instead used as part of a free list and points to the
//!            next in available tile.  If there is no next available tile,
//!            the value will be SO_BKG_TILE_LIST_END. If the tile is an 8-bit
//!				tile then the odd numbered entry is SO_BKG_TILE_USED_256
static u16 s_TileRefCount[SO_BKG_MAX_TILES];

// Free tile lists.  These are the heads of the three free tile lists.
//! \internal  16 color only tile free list
static u16 s_FreeTile16;
//! \internal  16 and 256 shared color tile free list
static u16 s_FreeTile256and16;
//! \internal  256 color only tile free list
static u16 s_FreeTile256;

//! \internal  16 color tile cache
static const SoTileSet *s_TileCache16 = NULL;

//! \internal  256 color tile cache
static const SoTileSet *s_TileCache256 = NULL;

//! \internal  TileSet to VRAM tile number translation tables for 16-color
//!            tiles.  This tables is used to lookup the tile number in VRAM
//!            given the tile number in the TileSet.  If the tile is not loaded
//!            into VRAM, the entry will be SO_BKG_TILE_NOT_LOADED.
static u16  *s_TilesetToRamIdx16 = NULL;
//! \internal  TileSet to VRAM tile number translation tables for 256-color
//!            tiles.  This tables is used to lookup the tile number in VRAM
//!            given the tile number in the TileSet.  If the tile is not loaded
//!            into VRAM, the entry will be SO_BKG_TILE_NOT_LOADED.
//!            the entry will be SO_BKG_TILE_NOT_LOADED.
static u16 *s_TilesetToRamIdx256 = NULL;


// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

//! \brief  Performs first stage setup for the BKG memory manager
//!
//! This function should be called before any other functions in the
//! \a SoBkgMemManager, \a SoBkg or \a SoBkgManager.  It clears all
//! BKG memory usage tracking information.  Do not call this routine
//! if backgrounds are currently enabled!
//!
void SoBkgMemManagerPreInitialize(void)
{
	SO_DMA_MEMSET(&s_RamToTilesetIdx[0], SO_BKG_MAX_TILES>>1,
	  (SO_BKG_TILE_NOT_LOADED<<16)|SO_BKG_TILE_NOT_LOADED);

	// clear out the RAM to TileSet map (Tileset to RAM map is handled in SoBkgSetTileCache()
	s_TileRefCount[0] = s_TileRefCount[1] = SO_BKG_TILE_USED_RESERVED;

	SO_DMA_MEMSET(&s_TileRefCount[0], SO_BKG_MAX_TILES>>1,
	  (SO_BKG_TILE_NOT_LOADED<<16)|SO_BKG_TILE_NOT_LOADED);

	// set the tile data for the transparent tile (tile 0)
	// NOTE: we clear 256 words of data here (the size of a 256 color tile)
	SO_DMA_MEMSET(&SO_BKG_DATA[0], SO_BKG_TILEIDX_TO_OFFSET256(1)>>1, 0);

	// clear the tile caches for 16 and 256 color tiles
	s_TileCache16 = s_TileCache256 = NULL;
}


//! \brief	reserves a range of tile indexes
//!
//! \param	a_FirstIdx	the first index to be reserved
//!	\param	a_LastIdx	the last index to be reserved + 1
//!
//! This routine marks as a specified range of tiles as 'Reserved'.  These
//! tiles will never be allocated by the allocator.  This function will
//! assert if any of the requested tiles are already in use.
//!
static void SoBkgMemManagerAllocRange(u32 a_FirstIdx, u32 a_LastIdx)
{
	SO_ASSERT(a_FirstIdx < SO_BKG_MAX_TILES, "Invalid starting index");
	SO_ASSERT(a_LastIdx <= SO_BKG_MAX_TILES, "Invalid ending index");
	SO_ASSERT(a_FirstIdx < a_LastIdx, "Invalid reservation range");

	// first mark 'em all as reserved
	int i;
	for(i = a_FirstIdx; i < a_LastIdx; ++i)
	{
		SO_ASSERT(SO_BKG_RAM_SLOT_IS_FREE(i), "Can't reserve already allocated tile");
		s_TileRefCount[i] = SO_BKG_TILE_USED_RESERVED;
	}
}


//! \brief	Loads tile date into background VRAM
//!
//! \param	a_This		this pointer
//! \param	a_CharBase	charbase bank (0-3)
//! \param	a_BaseIdx	index value of first tile to copy from SoTileSet
//!
//! This routine reserves space in BKG VRAM used by a user managed Tile Set.  This ensures
//! that the allocator will not try to allocate tile data in the memory occupied by the
//! screen.  This routine takes the same arguments as the \a SoTileSetBkgLoad routine.
//!
void SoBkgMemManagerAllocTileSet(const SoTileSet* a_This, u32 a_CharBase, u32 a_BaseIdx)
{
	// calculate range of indexes to reserve
	u32 first, last;

	first = last = (a_CharBase << 9);
	if(SoTileSetIs16Colors(a_This))
	{
		first +=  a_BaseIdx;
		last  += SoTileSetGetNumTiles(a_This);
	}
	else
	{
		first += a_BaseIdx << 1;
		last  += SoTileSetGetNumTiles(a_This) << 1;
	}

	//{
	//	char buf[80], *dp = &buf[0];
	//	dp = dstrcpy(dp, "AllocTileSet(cnt=");
	//	dp = dstrcpynum(dp, SoTileSetGetNumTiles(a_This), 10);
	//	dp = dstrcpy(dp, ", base=");
	//	dp = dstrcpynum(dp, a_CharBase, 10);
	//	dp = dstrcpy(dp, ", first=");
	//	dp = dstrcpynum(dp, a_BaseIdx, 10);
	//	dp = dstrcpy(dp, ")  alloc=[");
	//	dp = dstrcpynum(dp, first, 10);
	//	dp = dstrcpy(dp, ", ");
	//	dp = dstrcpynum(dp, last-1, 10);
	//	dp = dstrcpy(dp, "]");
	//	SoDebugPrint(buf);
	//}
	SoBkgMemManagerAllocRange(first, last);
}

//! \brief  reserves space for a BKG screen
//!
//! \param  a_Options   the option values used to set up the BKG (see \a SoBkgSetup)
//! 
//! This routine reserves space in BKG VRAM used by a BKG screen.  This ensures that
//! the allocator will not try to allocate tile data in the memory occupied by the
//! screen
//!
void SoBkgMemManagerAllocBkg(u32 a_Options)
{
	// calculate range of indexes to reserve
	u32 first, last;
	first = (a_Options & SO_BKG_SCRBASE_MASK) >> 2;
	last = (a_Options & SO_BIT_14) ? 128 : 64;
	if(a_Options & SO_BIT_15)
		last <<=1;
	last += first;

	//{
	//	char buf[80], *dp = &buf[0];
	//	dp = dstrcpy(dp, "AllocBkg(scrbase=");
	//	dp = dstrcpynum(dp, (a_Options&SO_BKG_SCRBASE_MASK)>>8, 10);
	//	dp = dstrcpy(dp, ", size=");
	//	dp = dstrcpynum(dp, (a_Options&SO_BIT_14) ? 512 : 256, 10);
	//	dp = dstrcpy(dp, "x");
	//	dp = dstrcpynum(dp, (a_Options&SO_BIT_15) ? 512 : 256, 10);
	//	dp = dstrcpy(dp, ")  alloc=[");
	//	dp = dstrcpynum(dp, first, 10);
	//	dp = dstrcpy(dp, ", ");
	//	dp = dstrcpynum(dp, last-1, 10);
	//	dp = dstrcpy(dp, "]");
	//	SoDebugPrint(buf);
	//}
	SoBkgMemManagerAllocRange(first, last);
}


//! \brief  Final setup for the BKG memory manager
//!
//! This function should be called after \a SoBkgMemManagerPreInitialize
//! and all calls to \a SoBkgMemManagerAllocBkg and \a SoBkgMemManagerAllocTileSet
//! It sets up the BKG memory usage free lists. Do not call this routine
//! if backgrounds are currently enabled!
//!
void SoBkgMemManagerInitialize(void)
{
//int cnt;
	u32 i;

	// the 16-color only list starts out empty
	s_FreeTile16 = SO_BKG_TILE_LIST_END;

	// the s_FreeTile256and16 list gets all of the tiles that can be
	// shared between 16 and 256 color tiles
	s_FreeTile256and16 = SO_BKG_TILE_LIST_END;

	// the s_FreeTile256 list gets the tiles that can only be used
	// by 256 color tiles
	s_FreeTile256 = SO_BKG_TILE_LIST_END;

	// NOTE: tile 0 (taking indices 0 and 1) is already allocated for the
	//       transparent tile.

	// insert all unreserved tiles into the free lists
//cnt =0;
	for(i = 0; i < SO_BKG_MAX_TILE_IDX; i += 2)
		if(s_TileRefCount[i] == SO_BKG_TILE_NOT_LOADED)
		{
			s_TileRefCount[i] = s_FreeTile256and16;
			s_FreeTile256and16 = i;
//++cnt;
		}
	//{
	//	char buf[80], *dp = &buf[0];
	//	dp = dstrcpy(dp, "s_FreeTile256and16 contains ");
	//	dp = dstrcpynum(dp, cnt, 10);
	//	dp = dstrcpy(dp, " entries");
	//	SoDebugPrint(buf);
	//}

//cnt =0;
	for(; i < SO_BKG_MAX_TILES; i += 2)
		if(s_TileRefCount[i] == SO_BKG_TILE_NOT_LOADED)
		{
			s_TileRefCount[i] = s_FreeTile256;
			s_FreeTile256 = i;
//++cnt;
		}
	//{
	//	char buf[80], *dp = &buf[0];
	//	dp = dstrcpy(dp, "s_FreeTile256 contains ");
	//	dp = dstrcpynum(dp, cnt, 10);
	//	dp = dstrcpy(dp, " entries");
	//	SoDebugPrint(buf);
	//}
}

//! \brief  Specifies the tile data source for demand-loaded tiles
//!
//! \param  a_TileSet		the tile set to use
//! \param	a_TileMapBuffer	buffer used to to keep track of tile allocation
//!							the length of the buffer must be at least as large
//!							as the number of tiles in a_Tileset.  If you're running
//!							low on iwram, you should allocate this buffer in ewram.
//! 
//! This routine is identifies the ROM tile set that is to be the
//! source of demand-loaded tile data for a particular tile color
//! depth.  To use both 256 and 16 color tiles in the same display,
//! call this routine twice -- once with the 256 color tile set, and
//! once with the 16 color tile set.  This routine must be called
//! prior to calling \a SoBkgWrite*Indirect.
//!
void SoBkgMemManagerSetTileSet(const SoTileSet *a_TileSet, u16 *a_TileMapBuffer)
{
	// clear out tilemap buffer
	SO_DMA_MEMSET(&a_TileMapBuffer[0], SoTileSetGetNumTiles(a_TileSet)>>1,
	  (SO_BKG_TILE_NOT_LOADED<<16)|SO_BKG_TILE_NOT_LOADED);

	if(SO_NUMBER_IS_UNEVEN(SoTileSetGetNumTiles(a_TileSet)))
	{
		a_TileMapBuffer[SoTileSetGetNumTiles(a_TileSet)-1] = SO_BKG_TILE_NOT_LOADED;
	}

	if(SoTileSetIs16Colors(a_TileSet))
	{
		s_TileCache16 = a_TileSet;
		s_TilesetToRamIdx16 = a_TileMapBuffer;
	}
	else
	{
		s_TileCache256 = a_TileSet;
		s_TilesetToRamIdx256 = a_TileMapBuffer;
	}
}

//! \brief  maps a 256 color tile from active TileCache
//!
//! \param  a_TilesetIdx   the index of the tile to load (0 is transparent)
//! 
//! This routine causes a tile to be loaded from the active 256 color
//! TileCache to BKG VRAM.  If the tile is already loaded, the pre-existing
//! index is returned and the reference count is incremented.
//!
u16 SoBkgMemManagerAllocTile(u32 a_TilesetIdx)
{
	u16 ramtileIdx;

//{
//	char buf[80], *dp = &buf[0];
//	dp = dstrcpy(dp, "Request for alloc of 256 color tile ");
//	dp = dstrcpynum(dp, a_TilesetIdx, 10);
//	SoDebugPrint(buf);
//}

	// tile 0 is the always mapped transparent tile
	if(a_TilesetIdx == 0) 
		return 0;

	// a_TilesetIdx is an index into the tile in ROM
	SO_ASSERT(a_TilesetIdx < SoTileSetGetNumTiles(s_TileCache256), "Bad ROM tile index");

	if(SO_BKG_ROM_TILE_IS_LOADED(a_TilesetIdx))
	{
		// tile has been loaded just increment ref count
		ramtileIdx = s_TilesetToRamIdx256[a_TilesetIdx];
		s_TileRefCount[ramtileIdx]++;

//{
//	char buf[80], *dp = &buf[0];
//	dp = dstrcpy(dp, "Tile already loaded ");
//	dp = dstrcpy(dp, ", s_RamToTilesetIdx[");
//	dp = dstrcpynum(dp, ramtileIdx, 10);
//	dp = dstrcpy(dp, "] = ");
//	dp = dstrcpynum(dp, s_RamToTilesetIdx[ramtileIdx], 10);
//	SoDebugPrint(buf);
//}

		// assert that the ram index is occupied and that the tile is 
		// properly allocated
		SO_ASSERT((ramtileIdx>>1) < SO_BKG_MAX_TILE_IDX, "Allocated tile index is too large");
		SO_ASSERT(SO_NUMBER_IS_EVEN(ramtileIdx), "Illegal odd 256 color RAM tile index");
		SO_ASSERT(!SO_BKG_RAM_SLOT_IS_FREE(ramtileIdx), "Illegal unused RAM tile index");
		SO_ASSERT(!SO_BKG_RAM_SLOT_IS_FREE(ramtileIdx + 1), "Illegal unused RAM tile index2");
		SO_ASSERT(s_TilesetToRamIdx256[s_RamToTilesetIdx[ramtileIdx]] == ramtileIdx,
			"Mismatched RAM to ROM mapping");
		SO_ASSERT(s_TileRefCount[ramtileIdx] >= 2, "Incorrect RAM tile ref count");
	}
	else
	{
		// 1st try to allocate from 256-color only list
		if( s_FreeTile256 != SO_BKG_TILE_LIST_END) 
		{
			ramtileIdx = s_FreeTile256;
			s_FreeTile256 = s_TileRefCount[ramtileIdx];
		}
		// otherwise, try to allocate from 16 & 256 color shared list
		else if ( s_FreeTile256and16 != SO_BKG_TILE_LIST_END )
		{
			ramtileIdx = s_FreeTile256and16;
			s_FreeTile256and16 = s_TileRefCount[ramtileIdx];
		}
		else
		{ 
			SO_ASSERT(0, "Could not allocate tile");
			return 0;
		}

		// links in the main linked list should be for even tiles only.
		// and the following odd tile must be free
		SO_ASSERT((ramtileIdx>>1) < SO_BKG_MAX_TILE_IDX, "Allocated tile index is too large");
		SO_ASSERT(SO_NUMBER_IS_EVEN(ramtileIdx), "Illegal odd 256 color RAM tile index");
		SO_ASSERT(SO_BKG_RAM_SLOT_IS_FREE(ramtileIdx), "Illegal used RAM tile index");
		SO_ASSERT(SO_BKG_RAM_SLOT_IS_FREE(ramtileIdx + 1), "Illegal used RAM tile index2");

		// set its ref count to 1
		s_TileRefCount[ramtileIdx] = 1;
		s_TileRefCount[ramtileIdx+1] = SO_BKG_TILE_USED_256;

		// link up the ram and rom tile maps
		s_TilesetToRamIdx256[a_TilesetIdx] = ramtileIdx;
		s_RamToTilesetIdx[ramtileIdx] = a_TilesetIdx;
		s_RamToTilesetIdx[ramtileIdx+1] = SO_BKG_TILE_USED_256;

		// copy the actual data into the tile memory
		SO_DMA_MEMCPY(&SoTileSetGetData(s_TileCache256)
		  [SO_BKG_TILEIDX_TO_OFFSET256(a_TilesetIdx)],
		  &SO_BKG_DATA[SO_BKG_TILEIDX_TO_OFFSET16(ramtileIdx)],
		  SO_BKG_TILEIDX_TO_OFFSET256(1)>>1);
//{
//	char buf[80], *dp = &buf[0];
//	dp = dstrcpy(dp, "Allocated 256 color tile at full index ");
//	dp = dstrcpynum(dp, ramtileIdx, 10);
//	SoDebugPrint(buf);
//}
	}

	// convert 16 color index to 256 color index
	return ramtileIdx>>1;
}


//! \brief  maps a 16 color tile from active TileCache
//!
//! \param  a_TilesetIdx   the index of the tile to load (0 is transparent)
//! 
//! This routine causes a tile to be loaded from the active 16 color
//! TileCache to BKG VRAM.  If the tile is already loaded, the pre-existing
//! index is returned and the reference count is incremented.
//!
u16 SoBkgMemManagerAllocTile16(u32 a_TilesetIdx)
{
	u16 ramtileIdx;

	// tile 0 is the always mapped transparent tile
	if(a_TilesetIdx == 0)
		return 0;

	// a_TilesetIdx is an index into the tile in ROM
	SO_ASSERT(a_TilesetIdx < SoTileSetGetNumTiles(s_TileCache16), "Bad ROM tile index");

	if(SO_BKG_ROM_TILE16_IS_LOADED(a_TilesetIdx))
	{
		// just increase the ref count
		ramtileIdx = s_TilesetToRamIdx16[a_TilesetIdx];
		s_TileRefCount[ramtileIdx]++;

		// make sure the tile is properly loaded
		SO_ASSERT(ramtileIdx < SO_BKG_MAX_TILE_IDX, "Allocated tile index is too large");
		SO_ASSERT(!SO_BKG_RAM_SLOT_IS_FREE(ramtileIdx), "Illegal unused RAM tile index");
		SO_ASSERT(s_TilesetToRamIdx16[s_RamToTilesetIdx[ramtileIdx]] == ramtileIdx,
			"Mismatched RAM to ROM mapping");
		SO_ASSERT(s_TileRefCount[ramtileIdx] >= 2, "Incorrect RAM tile ref count");
	}
	else
	{
		// load a new tile
		if ( s_FreeTile16 != SO_BKG_TILE_LIST_END )
		{
			// try getting the tile from the 16 color tile list
			ramtileIdx = s_FreeTile16;
			s_FreeTile16 = s_TileRefCount[ramtileIdx];

			// ramtileIdx should be a free RAM slot
			SO_ASSERT(SO_BKG_RAM_SLOT_IS_FREE(ramtileIdx), "Illegal used RAM tile index");
		}
		else if ( s_FreeTile256and16 != SO_BKG_TILE_LIST_END )
		{
			// otherwise get it from the 256 color tile list
			ramtileIdx = s_FreeTile256and16;
			s_FreeTile256and16 = s_TileRefCount[ramtileIdx];

			// any tile allocated from the 256 list must be even.
			SO_ASSERT(SO_NUMBER_IS_EVEN(ramtileIdx), "Illegal odd 256 color RAM tile index");

			// if we got a tile from the 256 color tile list, add the second 
			// tile to the 16 color tile list. 
			s_TileRefCount[ramtileIdx+1] = s_FreeTile16;
			s_FreeTile16 = ramtileIdx+1;
		}
		else
		{
			// otherwise no tiles are available....
			SO_ASSERT(0, "Could not allocate tile");
			return 0;
		}

		SO_ASSERT(ramtileIdx < SO_BKG_MAX_TILE_IDX, "Allocated tile index is too large");

		// set its ref count to 1
		s_TileRefCount[ramtileIdx] = 1;

		// link up the ram and rom tile maps
		s_TilesetToRamIdx16[a_TilesetIdx] = ramtileIdx;
		s_RamToTilesetIdx[ramtileIdx] = a_TilesetIdx;

		// now copy the actual data into the tile memory
		SO_DMA_MEMCPY(&SoTileSetGetData(s_TileCache16)
		  [SO_BKG_TILEIDX_TO_OFFSET16(a_TilesetIdx)],
		  &SO_BKG_DATA[SO_BKG_TILEIDX_TO_OFFSET16(ramtileIdx)],
		  SO_BKG_TILEIDX_TO_OFFSET256(1)>>1);
	}

	return ramtileIdx;
}


//! \brief  releases a 256 color tile from active TileCache
//!
//! \param  a_RamtileIdx   the index of the tile to release
//! 
//! This routine causes a tile to be removed from the BKG VRAM.  The removal
//! is done based on a reference count, so this if the tile was allocated
//! multiple times, the effect of this routine is just to decrease the
//! reference count.
//!
void SoBkgMemManagerFreeTile(u32 a_RamtileIdx)
{
	// tile 0 is the always mapped transparent tile
	if(a_RamtileIdx == 0)
		return;

	// convert to 16 color tile multiple index
	a_RamtileIdx <<= 1;

	// a_RamtileIdx is an index into the tile in RAM.
	SO_ASSERT(a_RamtileIdx < SO_BKG_MAX_TILES, "Invalid tile index");
	SO_ASSERT(s_TileRefCount[a_RamtileIdx] > 0, "Incorrect RAM tile ref count");
	SO_ASSERT(!SO_BKG_RAM_SLOT_IS_FREE(a_RamtileIdx), "Attempt to free unallocated tile");
	SO_ASSERT(!SO_BKG_RAM_SLOT_IS_FREE(a_RamtileIdx + 1), "Attempt to free unallocated tile2");
	SO_ASSERT(s_TilesetToRamIdx256[s_RamToTilesetIdx[a_RamtileIdx]] == a_RamtileIdx,
		"Mismatched RAM to ROM mapping");
//	SO_ASSERT(SO_NUMBER_IS_EVEN(a_RamtileIdx), "Illegal odd 256 color RAM tile index");

	s_TileRefCount[a_RamtileIdx]--;

	// if no one is referencing the tile, link it into the appropriate unused tile list.
	// and clear out all references to it.
	if(s_TileRefCount[a_RamtileIdx] == 0)
	{
		// mark the tile as free in the RAM to ROM and ROM to RAM maps
		s_TilesetToRamIdx256[s_RamToTilesetIdx[a_RamtileIdx]] = SO_BKG_TILE_NOT_LOADED;
		s_RamToTilesetIdx[a_RamtileIdx]   = SO_BKG_TILE_NOT_LOADED;
		s_RamToTilesetIdx[a_RamtileIdx+1] = SO_BKG_TILE_NOT_LOADED;

		// link this tile back into the appropriate free list
		if(a_RamtileIdx < SO_BKG_MAX_TILE_IDX)
		{
			// shared area
			s_TileRefCount[a_RamtileIdx] = s_FreeTile256and16;
			s_FreeTile256and16 = a_RamtileIdx;
		}
		else
		{
			// 256 color only area
			s_TileRefCount[a_RamtileIdx] = s_FreeTile256;
			s_FreeTile256 = a_RamtileIdx;
		}
	}
}

//! \brief  releases a 16 color tile from active TileCache
//!
//! \param  a_RamtileIdx   the index of the tile to release
//! 
//! This routine causes a tile to be removed from the BKG VRAM.  The removal
//! is done based on a reference count, so this if the tile was allocated
//! multiple times, the effect of this routine is just to decrease the
//! reference count.
//!
void SoBkgMemManagerFreeTile16(u32 a_RamtileIdx)
{
	// tile 0 is the always mapped transparent tile
	if(a_RamtileIdx == 0)
		return;

	// a_RamtileIdx is an index into the tile in RAM.
	SO_ASSERT(a_RamtileIdx < SO_BKG_MAX_TILES, "Invalid tile index");
	SO_ASSERT(s_TileRefCount[a_RamtileIdx] > 0, "Incorrect RAM tile ref count");
	SO_ASSERT(!SO_BKG_RAM_SLOT_IS_FREE(a_RamtileIdx), "Attempt to free unallocated tile");
	SO_ASSERT(s_TilesetToRamIdx16[s_RamToTilesetIdx[a_RamtileIdx]] == a_RamtileIdx,
		"Mismatched RAM to ROM mapping");

	s_TileRefCount[a_RamtileIdx]--;

	// if no one is referencing the tile, link it into the unused tile list.
	// and clear out all references to it.
	if(s_TileRefCount[a_RamtileIdx] == 0)
	{
		// release the 16 color tile
		s_TilesetToRamIdx16[s_RamToTilesetIdx[a_RamtileIdx]] = SO_BKG_TILE_NOT_LOADED;
		s_RamToTilesetIdx[a_RamtileIdx] = SO_BKG_TILE_NOT_LOADED;

		// check if the tile released is part of a pair that can be put into 
		// the 256 color free list
		if(s_RamToTilesetIdx[a_RamtileIdx ^ 0x1] == SO_BKG_TILE_NOT_LOADED)
		{
			// PROBLEM: don't have back pointer for this other tile. will have
			// to search entire list for tile w/ back pointer... has possible
			// bad scaling.

			// the tile to find has the last bit toggled
			u32 findTile = (a_RamtileIdx ^ 0x1);

			u32 prevTile = SO_BKG_TILE_LIST_END;
			u32 curTile  = s_FreeTile16;

			// search... will be slow if there are lots of 16 color tiles in
			// the free list and the one we are looking for is near the end.
			while(curTile != findTile)
			{
				prevTile = curTile;
				curTile = s_TileRefCount[curTile];

				// the tile we are looking for is guaranteed to be in the list.
				SO_ASSERT(curTile != SO_BKG_TILE_LIST_END, "Lost tile from free list");
			}

			// cur tile gets unlinked from the 16 color free list
			if ( prevTile == SO_BKG_TILE_LIST_END )
				s_FreeTile16 = s_TileRefCount[curTile];
			else 
				s_TileRefCount[prevTile] = s_TileRefCount[curTile];

			// a_RamtileIdx gets linked to the 256 color free list.
			s_TileRefCount[a_RamtileIdx & 0xFFFE] = s_FreeTile256and16;
			s_FreeTile256and16 = (a_RamtileIdx & 0xFFFE);
		}
		else
		{
			// link the tile into the 16 color free list.
			s_TileRefCount[a_RamtileIdx] = s_FreeTile16;
			s_FreeTile16 = a_RamtileIdx;
		}
	}
}
