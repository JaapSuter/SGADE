// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMemManager.h
	\author		Mark T. Price
	\date		Aug 9 2001
	\ingroup	SoMemManager

	See the \a SoMemManager module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_MEMORY_MANAGER_H
#define SO_MEMORY_MANAGER_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoMemManager SoMemManager
	\brief    Fast / low-overhead general memory manager (alloc/free)

	Singleton

	This file contains an implementation of a simple general purpose memory
	manager suitable for use in embedded applications.  It is based loosely on
	the GDC 2001 talk "AGB Resource Management" by Rafael Baptista.  Credit
	goes to him for the basic algorithm used herein.

	These functions were designed with the intention of balancing simplicity
	with speed and space efficiency.  Features that would protect against
	improper use but which would add a significant computational cost were not
	included.  Here is a list of known limitations:
	* You must call free with the same pointer as was used to alloc (there is
	  an #ifdef'ed check to ensure this is done).  If you do not, free may
	  only release a portion of the originally allocated buffer.
	* The basic allocation block size must be a power of two. Other sizes are
	  truncated to the nearest power of two *less than* the requested size. The
	  minimum allowable block size is 8 bytes (in order to fit the free tree
	  node).
	* The number of blocks in the free pool is the largest multiple of 8 that
	  will fit within the buffer.

	This is my first implementation of a memory manager.  There are undoubtedly
	areas that could be improved.  If you have suggestions for improvements --
	preferably with code, or you find anything that is obviously wrong or
	grossly inefficient drop me a line.

	Configuration

    This implementation supports allocation block sizes equal to any power of
	two.  The default (and minimum) block size is the size of the free node
	structure (8 bytes).  To allow varying block sizes, uncomment the
	\a SO_MEM_MANAGER_BLOCK_BITS #define in \a SoMemManager.h

    This implementation supports multiple active memory pools.  To force a
    single memory pool (and simplify the API slightly) uncomment the
    SO_MEM_MANAGER_SINGLE_POOL_* #defines in \a SoMemManager.h
  
*/

//! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------

#include "SoSystem.h"


// ----------------------------------------------------------------------------
// Configuration Defines
// ----------------------------------------------------------------------------

#ifdef SO_DEBUG
//! Configuration -- If this symbol is defined, the library will verify
//! arguments at RUN TIME.  This is only done for debug builds.
#define SO_MEM_MANAGER_VERIFY                           
#endif

//! Configuration - starting address of the managed memory pool. If this symbol
//! is not #defined, then multiple free pools will be supported and each call
//! to every SoMemManager function must include the pool to be used.
#define SO_MEM_MANAGER_SINGLE_POOL_BASE &g_u32MemPool[0]

//! Configuration - the size of the managed memory pool in bytes.  If
//! \a SO_MEM_MANAGER_SINGLE_POOL_BASE is defined, then this symbol must be
//! defined as well.
#define SO_MEM_MANAGER_SINGLE_POOL_SIZE 0x00001000

//! Configuration - the number of address bits in each allocation block
//! (Allocation block size is equal to 1<<SO_MEM_MANAGER_BLOCK_BITS).  If this
//! symbol is not #defined, then different sizes will be allowed for each
//! free pool.
//! \note  This symbol must be defined if \a SO_MEM_MANAGER_SINGLE_POOL_BASE is defined
#define SO_MEM_MANAGER_BLOCK_BITS   3

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
//! The single managed memory pool (must be provided by library user).
//! Only used if \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #defined
extern u32 g_u32MemPool[];
#endif


// ----------------------------------------------------------------------------
// Functions;
// ----------------------------------------------------------------------------

#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE

void  SoMemManagerInit(void);
void* SoMemManagerAlloc(u16 iSize);
void  SoMemManagerFree(void* pBuf);

#else

#ifdef SO_MEM_MANAGER_BLOCK_BITS
void  SoMemManagerInit(u32* pMemPool, u32 iBufSize);
#else
void  SoMemManagerInit(u32* pMemPool, u32 iBufSize, u16 iBlockSize);
#endif
void* SoMemManagerAlloc(u32* pMemPool, u16 iSize);
void  SoMemManagerFree(u32* pMemPool, void* pBuf);

#endif


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
