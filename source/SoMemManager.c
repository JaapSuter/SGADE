// ----------------------------------------------------------------------------
/*!
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMemManager.c
	\author		Mark T. Price
	\date		Aug 9 2001
	\ingroup	SoMemManager

	See the \a SoMemManager module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoMemManager.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! \internal  NULL node index ('pointer') value
#define SO_MEM_MANAGER_NULL_IDX 0xffff

#ifdef SO_MEM_MANAGER_BLOCK_BITS
//! \internal	basic allocation block size in bytes
#define SO_MEM_MANAGER_BLOCK_SIZE (1<<SO_MEM_MANAGER_BLOCK_BITS)
#endif

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

// LAYOUT OF MEMORY BUFFER
//   struct SoMemManagerHdr_t
//   [memory map bitmap] : two bits per block
//   [free pool tree]    : one node per free range (inside free memory)

//! \internal	memory manager header
struct SoMemManagerHdr_t {
	u8* pFreePool;		//!< pointer to start of memory pool data
	u16 iFreeRootIdx;	//!< 'pointer' to top of free node tree
	u16 iBlockCnt;		//!< total # of blocks being managed
#ifndef SO_MEM_MANAGER_BLOCK_BITS
	u16 iBlockBits;		//!< shift size
#endif
};

//! \internal	Free pool tree node
struct SoMemManagerNode_t {
	u16 iParentIdx;		//!< 'pointer' to parent node
	u16 iChildLessIdx;	//!< 'pointer' to smaller sized children subtree
	u16 iChildMoreIdx;	//!< 'pointer' to larger sized children subtree
	u16 iBlockCount;	//!< number of blocks in this node
};


// ----------------------------------------------------------------------------
// Module Local Variables
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Local (helper) macros and functions
// ----------------------------------------------------------------------------

// DATA STRUCTURES AND MACROS  - - - - - - - - - - - - - - - - - - - -

#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
//! \internal
#define SoMemManagerHdr()        ((struct SoMemManagerHdr_t*)SO_MEM_MANAGER_SINGLE_POOL_BASE)
//! \internal
#define SoMemManagerPoolArg()    
#else
//! \internal
#define SoMemManagerHdr()        ((struct SoMemManagerHdr_t*)pMemPool)
//! \internal
#define SoMemManagerPoolArg()    pMemPool,
#endif

#ifdef SO_MEM_MANAGER_BLOCK_BITS
//! \internal
#define SoMemManagerAddx(iBlock) ((struct SoMemManagerNode_t *)(SoMemManagerHdr()->pFreePool + ((u32)(iBlock) << SO_MEM_MANAGER_BLOCK_BITS)))
#else
//! \internal
#define SoMemManagerAddx(iBlock) ((struct SoMemManagerNode_t *)(SoMemManagerHdr()->pFreePool + ((u32)(iBlock) << SoMemManagerHdr()->iBlockBits)))
#endif

// FREE NODE TREE HANDLING ROUTINES  - - - - - - - - - - - - - - - - -

/*!
//  \brief	insert new node into (subtree of) free node tree
//
//  \param pMemPool     the memory pool (this argument is omitted if
//                      \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #define'd)
//  \param iParentNode	subtree index to insert into
//  \param pParentNode	subtree pointer to insert into
//  \param iInsNode		index of node to be inserted
// 
//  \internal
*/
#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
static void SoMemManagerInsertNode(u16 iParentNode, struct SoMemManagerNode_t *pParentNode, u16 iInsNode)
{
#else
static void SoMemManagerInsertNode(u32* pMemPool, u16 iParentNode, struct SoMemManagerNode_t *pParentNode, u16 iInsNode)
{
#endif
	struct SoMemManagerNode_t *pInsNode = SoMemManagerAddx(iInsNode);

	if(iParentNode == SO_MEM_MANAGER_NULL_IDX)
	{
		// insert at root, resolve to actual index
		iParentNode = SoMemManagerHdr()->iFreeRootIdx;
		if(iParentNode == SO_MEM_MANAGER_NULL_IDX)
		{
			// special case insert into null tree
			SoMemManagerHdr()->iFreeRootIdx = iInsNode;
			pInsNode->iParentIdx = SO_MEM_MANAGER_NULL_IDX;
			return;
		}

		pParentNode = SoMemManagerAddx(iParentNode);
	}

	for(;;)
	{
		if(pParentNode->iBlockCount > pInsNode->iBlockCount)
		{
			// insert into smaller tree
			if(pParentNode->iChildLessIdx == SO_MEM_MANAGER_NULL_IDX)
			{
				// no smaller tree - trivial insert
				pParentNode->iChildLessIdx = iInsNode;
				pInsNode->iParentIdx       = iParentNode;
				return;
			}

			// traverse to locate correct insertion point
			iParentNode = pParentNode->iChildLessIdx;
		}
		else // if(pParentNode->iBlockCount <= pInsNode->iBlockCount)
		{
			// insert into larger tree
			if(pParentNode->iChildMoreIdx == SO_MEM_MANAGER_NULL_IDX)
			{
				// no larget tree - trivial insert
				pParentNode->iChildMoreIdx = iInsNode;
				pInsNode->iParentIdx       = iParentNode;
				return;
			}

			// traverse to locate correct insertion point
			iParentNode = pParentNode->iChildMoreIdx;
		}

		pParentNode = SoMemManagerAddx(iParentNode);
	}
}

/*!
//  \brief	Remove specified node from free pool tree
// 
//  \param pMemPool the memory pool (this argument is omitted if
//                  \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #define'd)
//  \param iNode	index of node to be removed
//  \param pNode	pointer to node to be removed
// 
//  \internal
*/
#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
static void SoMemManagerRemoveNode(u16 iNode, struct SoMemManagerNode_t *pNode)
{
#else
static void SoMemManagerRemoveNode(u32* pMemPool, u16 iNode, struct SoMemManagerNode_t *pNode)
{
#endif
	struct SoMemManagerNode_t *pParent;
	u16         iParent;

	// remove subtree from free tree
	if(pNode->iParentIdx == SO_MEM_MANAGER_NULL_IDX)
	{
		// special case for root node
		pParent = NULL;
		iParent = SO_MEM_MANAGER_NULL_IDX;

		SoMemManagerHdr()->iFreeRootIdx = SO_MEM_MANAGER_NULL_IDX;
	}
	else
	{
		iParent = pNode->iParentIdx;
		pParent = SoMemManagerAddx(iParent);

		if(pParent->iChildLessIdx == iNode)
			pParent->iChildLessIdx = SO_MEM_MANAGER_NULL_IDX;
		else if(pParent->iChildMoreIdx == iNode)
			pParent->iChildMoreIdx = SO_MEM_MANAGER_NULL_IDX;
	}

	// add child subtrees back to free tree
	if(pNode->iChildLessIdx != SO_MEM_MANAGER_NULL_IDX)
	{
#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
		SoMemManagerInsertNode(iParent, pParent, pNode->iChildLessIdx);
#else
		SoMemManagerInsertNode(pMemPool, iParent, pParent, pNode->iChildLessIdx);
#endif
		pNode->iChildLessIdx = SO_MEM_MANAGER_NULL_IDX;
	}
	if(pNode->iChildMoreIdx != SO_MEM_MANAGER_NULL_IDX)
	{
#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
		SoMemManagerInsertNode(iParent, pParent, pNode->iChildMoreIdx);
#else
		SoMemManagerInsertNode(pMemPool, iParent, pParent, pNode->iChildMoreIdx);
#endif
		pNode->iChildMoreIdx = SO_MEM_MANAGER_NULL_IDX;
	}
}

// MEMORY MAP HELPER FUNCTIONS - - - - - - - - - - - - - - - - - - - -

//! \internal  The number of bits in a byte
#define BITSPERBYTE 8

/*!
//  \brief	tests to see if bit in bitmap is set
//
//  \param bmdata	pointer to start of bitmap
//  \param bitnum	bit number to check
//
//  \internal
//
//  Yes, I realize this isn't the most efficient way to do this...
*/
inline bool SoMemManagerBitCheck(u8 *bmdata, u16 bitnum)
{
	return (bool)(( *((bmdata) + (bitnum)/BITSPERBYTE) >>
	  ((BITSPERBYTE-1) - (bitnum)%BITSPERBYTE) )&1);
}
/*!
//  \brief	sets a bit in a bitmap
//
//  \param bmdata	pointer to start of bitmap
//  \param bitnum	bit number to set
//
//  \internal
//
//  Yes, I realize this isn't the most efficient way to do this...
*/
inline void SoMemManagerBitSet(u8 *bmdata, u16 bitnum)
{
	( *((bmdata) + (bitnum)/BITSPERBYTE)  |=  
	  (1<<((BITSPERBYTE-1) - (bitnum)%BITSPERBYTE)) );
}
/*!
//  \brief	clears a bit in a bitmap
//
//  \param bmdata	pointer to start of bitmap
//  \param bitnum	bit number to clear
//
//  \internal
//
//  Yes, I realize this isn't the most efficient way to do this...
*/
inline void SoMemManagerBitClear(u8 *bmdata, u16 bitnum)
{
	( *((bmdata) + (bitnum)/BITSPERBYTE)  &= 
	  ~(1<<((BITSPERBYTE-1) - (bitnum)%BITSPERBYTE)) );
}

#ifndef SO_MEM_MANAGER_SINGLE_POOL_BASE
/*!
//  \brief	tests to see if this is the first block in an free range
//
//  \param pMemPool the memory pool (this argument is omitted if
//                  \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #define'd)
//  \param iBlock	index of block to test
//
//  \internal
*/
static bool IsFirstBlock(u32 *pMemPool, u16 iBlock)
{
#else
static bool IsFirstBlock(u16 iBlock)
{
#endif
	return SoMemManagerBitCheck((u8*)SoMemManagerHdr() + sizeof(struct SoMemManagerHdr_t), (u16)((iBlock)<<1));
}
#ifndef SO_MEM_MANAGER_SINGLE_POOL_BASE
/*!
//  \brief	tests to see if the block is allocated
//
//  \param pMemPool the memory pool (this argument is omitted if
//                  \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #define'd)
//  \param iBlock	index of block to test
// 
//  \internal
*/
static bool IsUsedBlock(u32 *pMemPool, u16 iBlock)
{
#else
static bool IsUsedBlock(u16 iBlock)
{
#endif
	return SoMemManagerBitCheck((u8*)SoMemManagerHdr() + sizeof(struct SoMemManagerHdr_t), (u16)(((iBlock)<<1)+1));
}

#ifndef SO_MEM_MANAGER_SINGLE_POOL_BASE
/*!
//  \brief	sets an allocation range to used/free
//
//  \param pMemPool the memory pool (this argument is omitted if
//                  \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #define'd)
//  \param iBlock	block index of start of range
//  \param iSize	number of blocks in range
//  \param bOnOff	state to set (true=used, false=free)
// 
//  \internal
*/
void SetBlockMemMap(u32 *pMemPool, u16 iBlock, u16 iSize, bool bOnOff)
{
#else
void SetBlockMemMap(u16 iBlock, u16 iSize, bool bOnOff)
{
#endif
	u16 iBitNum = iBlock << 1;
	// point to start of memory map
	u8* pMemMap = (u8*)SoMemManagerHdr() + sizeof(struct SoMemManagerHdr_t);

#ifdef SO_MEM_MANAGER_VERIFY
	if(!iSize || ((iBlock + iSize) > SoMemManagerHdr()->iBlockCnt))
		return;
#endif

	// Set entry for 1st block (start = TRUE, used as appropriate)
	SoMemManagerBitSet(pMemMap, iBitNum);
	++iBitNum;
	if(bOnOff)
		SoMemManagerBitSet(pMemMap, iBitNum);
	else
		SoMemManagerBitClear(pMemMap, iBitNum);
	++iBitNum;

	// set entries for Nth blocks (start = FALSE, used as appropriate)
	while(--iSize)
	{
		SoMemManagerBitClear(pMemMap, iBitNum);
		++iBitNum;
		if(bOnOff)
			SoMemManagerBitSet(pMemMap, iBitNum);
		else
			SoMemManagerBitClear(pMemMap, iBitNum);
		++iBitNum;
	}
}


// ----------------------------------------------------------------------------
// Exported Function implementations
// ----------------------------------------------------------------------------

#ifndef SO_MEM_MANAGER_SINGLE_POOL_BASE
/*!
//  \brief	initialize memory pool for allocation
//
//  \param pMemPool	   The starting address of the free pool to be initialized.
//                     Must be aligned on a 4-byte boundary. (this argument is
//                     omitted if \a SO_MEM_MANAGER_SINGLE_POOL_BASE is
//                     #define'd)
//  \param iBufSize    The number of bytes reserved for the free pool,
//                     including space for memory manager overhead. (this
//                     argument is omitted if \a SO_MEM_MANAGER_SINGLE_POOL_SIZE
//                     is #define'd)
//  \param iBlockSize  The number of bytes per allocation block.  The larger
//                     this value, the more space will be wasted per allocation.
//                     Must be a power of 2 greater than 8. (this argument is
//                     omitted if \a SO_MEM_MANAGER_BLOCK_BITS is #define'd)
//
//  Call this fucntion before you use any other \a SoMemManager functions.
*/
#ifndef SO_MEM_MANAGER_BLOCK_BITS
void SoMemManagerInit(u32* pMemPool, u32 iBufSize, u16 iBlockSize)
{
#else
void SoMemManagerInit(u32 *pMemPool, u32 iBufSize)
{
#endif
#else
void SoMemManagerInit(void)
{
#endif
	struct SoMemManagerNode_t *pNode;

#ifdef SO_MEM_MANAGER_VERIFY
	// validate that pMemPool is aligned on a 4-byte boundary
	if((u32)SoMemManagerHdr() & 0x03)
		return;

	// validate that at least one set of blocks can fit in the pool
#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
	if(SO_MEM_MANAGER_SINGLE_POOL_SIZE < sizeof(struct SoMemManagerHdr_t) + 2 + sizeof(struct SoMemManagerNode_t)*8)
		return;
#else
	if(iBufSize < sizeof(struct SoMemManagerHdr_t) + 2 + sizeof(struct SoMemManagerNode_t)*8)
		return;
#endif
#endif

#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
	SoMemManagerHdr()->iBlockCnt = (u16)((((SO_MEM_MANAGER_SINGLE_POOL_SIZE - sizeof(struct SoMemManagerHdr_t)) << 3) / ((SO_MEM_MANAGER_BLOCK_SIZE<<3) + 2)) & ~0x7);
#else
#ifdef SO_MEM_MANAGER_BLOCK_BITS
	SoMemManagerHdr()->iBlockCnt = (u16)((((iBufSize              - sizeof(struct SoMemManagerHdr_t)) << 3) / ((SO_MEM_MANAGER_BLOCK_SIZE<<3) + 2)) & ~0x7);

#else
	// calculate size of memory map / free pool
	// NOTE: this (simple) formula only allows the number of blocks to be in
	// multiples of 8
	//
	// bs(BufSize), kc(BlockCount), ks(BlockSize)
	//   bs = ( kc * (ks + 2/8) ) + 10;
	//   bs - 10 = (kc * (ks + 2/8));
	//   kc = (bs - 10) / (ks + 2/8);
	//   kc = ((bs - 10) * 8)  /  (ks * 8 + 2)
	SoMemManagerHdr()->iBlockCnt    = (u16)((((iBufSize - sizeof(struct SoMemManagerHdr_t)) << 3) / ((iBlockSize<<3) + 2)) & ~0x7);

	// calculate block shift size
	SoMemManagerHdr()->iBlockBits   = 0;
	{
		u16 iNumBits;
		for(iNumBits = iBlockSize; iNumBits > 1; iNumBits >>= 1)
		{
			SoMemManagerHdr()->iBlockBits += 1;
		}
	}
#endif
#endif

	// construct remainder of pool header
	SoMemManagerHdr()->pFreePool    = (u8*)SoMemManagerHdr() + sizeof(struct SoMemManagerHdr_t) + (SoMemManagerHdr()->iBlockCnt >> 2);
	SoMemManagerHdr()->iFreeRootIdx = 0;

	// setup one and only free tree node
	pNode = SoMemManagerAddx(0);
	pNode->iParentIdx  = pNode->iChildLessIdx = pNode->iChildMoreIdx = SO_MEM_MANAGER_NULL_IDX;
	pNode->iBlockCount = SoMemManagerHdr()->iBlockCnt;

	// setup memory map
#ifdef SO_MEM_MANAGER_SINGLE_POOL_BASE
	SetBlockMemMap(0, SoMemManagerHdr()->iBlockCnt, 0);
#else
	SetBlockMemMap(pMemPool, 0, SoMemManagerHdr()->iBlockCnt, 0);
#endif
}

#ifndef SO_MEM_MANAGER_SINGLE_POOL_BASE
/*!
//  \brief	Allocate memory from free pool
//
//  \param pMemPool  The starting address of the free pool.  Must have been
//                   previously initialized by calling \a SoMemManagerInit
//                   (this argument is omitted if
//                   \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #define'd)
//  \param iSize     The number of bytes to be allocated
//
//  \return  pointer to allocated buffer, or NULL if none was available
//
//  Always make sure that you test for the failure case.  If you always
//  consider allocation failure to be fatal you can just uncomment
//  \a SO_MEM_MANAGER_ASSERT_FAILURE in the \a SoMemManager.h file and this
//  function will call SO_ASSERT on failure.  You may want to consider doing
//  this in your code, however, as you will then know what call caused the
//  problem.
//
// 	The memory allocated by this function is not zeroed or initialized to any
//  value. Do not make any assumptions on the contents of the given memory.
*/
void*  SoMemManagerAlloc(u32* pMemPool, u16 iSize)
{
#else
void*  SoMemManagerAlloc(u16 iSize)
{
#endif
	struct SoMemManagerNode_t *pBlock;
	u16         iBlock = SoMemManagerHdr()->iFreeRootIdx;
	struct SoMemManagerNode_t *pBest;
	u16         iBest;

	if(iBlock == SO_MEM_MANAGER_NULL_IDX)
		return NULL;

	// calculate the number of blocks required to hold iSize bytes
#ifdef SO_MEM_MANAGER_BLOCK_BITS
	iSize = (iSize + SO_MEM_MANAGER_BLOCK_SIZE-1) >> SO_MEM_MANAGER_BLOCK_BITS;
#else
	iSize = (iSize + (1<<SoMemManagerHdr()->iBlockBits)-1) >> SoMemManagerHdr()->iBlockBits;
#endif

	// search for smallest free block large enough to satisfy request
	pBlock = SoMemManagerAddx(iBlock);
	iBest = SO_MEM_MANAGER_NULL_IDX;
	for(;;)
	{
		if(pBlock->iBlockCount == iSize)
		{
			// found exact fit! -- use it
			SoMemManagerRemoveNode(SoMemManagerPoolArg() iBlock, pBlock);
			SetBlockMemMap(SoMemManagerPoolArg() iBlock, iSize, 1);
			return (void*)pBlock;
		}
		else if(pBlock->iBlockCount > iSize)
		{
			// due to the order of our tree traversal,  we can only get
			// here with a block that is smaller than our previous best-fit
			pBest = pBlock;
			iBest = iBlock;

			iBlock = pBlock->iChildLessIdx;
		}
		else // if(pBlock->iBlockCount < iSize)
		{
			iBlock = pBlock->iChildMoreIdx;
		}

		if(iBlock == SO_MEM_MANAGER_NULL_IDX)
		{
			// off bottom of tree! -- use best-fit (if any)
			if(iBest == SO_MEM_MANAGER_NULL_IDX)
				return NULL;

			SoMemManagerRemoveNode(SoMemManagerPoolArg() iBest, pBest);
			// shrink node by size of allocation
			pBest->iBlockCount -= iSize;
			SoMemManagerInsertNode(SoMemManagerPoolArg() SO_MEM_MANAGER_NULL_IDX, NULL, iBest);

			iBlock = iBest + pBest->iBlockCount;
			pBlock = SoMemManagerAddx(iBlock);

			SetBlockMemMap(SoMemManagerPoolArg() iBlock, iSize, 1);
			return (void*)pBlock;
		}

		// calc new block pointer and loop
		pBlock = SoMemManagerAddx(iBlock);
	}
}


#ifndef SO_MEM_MANAGER_SINGLE_POOL_BASE
/*!
//  \brief Release memory back to free pool
//
//  \param pMemPool The starting address of the free pool.  Must have been
//                  previously initialized by calling \a SoMemManagerInit
//                  (this argument is omitted if
//                  \a SO_MEM_MANAGER_SINGLE_POOL_BASE is #define'd)
//  \param pBuf     Pointer to the buffer to free.  Must have been previously
//                  created via a call to \a SoMemManagerAlloc
//
// 	This releases the memory back into the pool.  Only call this function with
//  pointers you received from \a SoMemManagerAlloc or results will be
//  unpredictable.
//
// 	After this function is called the contents of the released memory is
//  unpredictable and modifying it will likely cause a crash.  Never make any
//  assumptions about the memory after giving it to \a SoMemManagerFree.
*/
void SoMemManagerFree(void* pMemPool, void* pBuf)
{
#else
void SoMemManagerFree(void* pBuf)
{
#endif
	struct SoMemManagerNode_t *pBlock;
	u16         iBlock;
	u16         iSize;

	// calculate block's index
#ifdef SO_MEM_MANAGER_BLOCK_BITS
	iBlock = ((u8*)pBuf - SoMemManagerHdr()->pFreePool) >> SO_MEM_MANAGER_BLOCK_BITS;
#else
	iBlock = ((u8*)pBuf - SoMemManagerHdr()->pFreePool) >> SoMemManagerHdr()->iBlockBits;
#endif

#ifdef SO_MEM_MANAGER_VERIFY
	if(iBlock >= SoMemManagerHdr()->iBlockCnt)
		return;	// block out of range

	if(!IsFirstBlock(SoMemManagerPoolArg() iBlock))
		return;	// not first block in allocation unit
	if(!IsUsedBlock(SoMemManagerPoolArg() iBlock))
		return; // not allocated
#endif

	// scan to find size of block
	for(iSize = iBlock+1; (iSize < SoMemManagerHdr()->iBlockCnt) && !IsFirstBlock(SoMemManagerPoolArg() iSize); ++iSize)
		;
	iSize = iSize - iBlock;

	// check & consolidate with preceeding block
	if(iBlock && !IsUsedBlock(SoMemManagerPoolArg() (u16)(iBlock-1)))
	{
		do
			--iBlock;
		while(!IsFirstBlock(SoMemManagerPoolArg() iBlock));

		pBlock = SoMemManagerAddx(iBlock);
		iSize += pBlock->iBlockCount;
		
		SoMemManagerRemoveNode(SoMemManagerPoolArg() iBlock, pBlock);
	}

	// check & consolidate with following block
	if(iBlock + iSize < SoMemManagerHdr()->iBlockCnt && !IsUsedBlock(SoMemManagerPoolArg() (u16)(iBlock+iSize)))
	{
		struct SoMemManagerNode_t *pNode2 = SoMemManagerAddx(iBlock+iSize);
		
		SoMemManagerRemoveNode(SoMemManagerPoolArg() (u16)(iBlock+iSize), pNode2);

		iSize += pNode2->iBlockCount;
	}

	// set node header
	pBlock = SoMemManagerAddx(iBlock);
	pBlock->iParentIdx = pBlock->iChildLessIdx = pBlock->iChildMoreIdx = SO_MEM_MANAGER_NULL_IDX;
	pBlock->iBlockCount = iSize;

	SoMemManagerInsertNode(SoMemManagerPoolArg() SO_MEM_MANAGER_NULL_IDX, NULL, iBlock);
	SetBlockMemMap(SoMemManagerPoolArg() iBlock, iSize, 0);
}

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
