// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoDMA.h
	\author		Gabriele Scibilia, Jaap Suter, Mark T. Price
	\date		Jan 21 2003
	\ingroup	SoDMA

	See the \a SoDMA module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_DMA_H
#define SO_DMA_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
//! 
//! \defgroup SoDMA SoDMA
//! \brief	  All DMA handling
//!
//!  Singleton
//!
//!  This module takes care of everything that has to do with DMA transfers.
//!
//!  There are 4 seperate DMA channels on the GBA each with their own priority.
//!  The highest priority channel is DMA0, then DMA1, then DMA2, and finally
//!  DMA3.
//!	
//!  Suppose a DMA transfer is currently being executed, and another DMA
//!  transfer is started with a higher priority. The current transfer will be
//!  halted, and it will resume upon completion of the higher priority DMA.
//!	
//!  There are several different uses for DMA, for which different channels are
//!  appropriate:
//!
//!  - DMA 0 has the highest priority of the DMA channels. It can only be used
//!    from internal memory (RAM) to any other internal memory (RAM). It can
//!    not be used to transfer from ROM.
//!
//!  - DMA channels 1 and 2 are for transfering between the ROM and RAM and the
//!    direct sound FIFO.  Direct-sound FIFO transfers can be accomplished only
//!    by using DMA 1 and 2. Therefore whenever you use direct sound you will
//!    use DMA 1 or 2.
//!
//!  - DMA 3 has the lowest priority and can be used to transfer data to and
//!    from any area of memory (RAM and ROM).
//!
//!  Perform the following steps when you want to do a DMA transfer:
//!
//!  - Set the transfer source address.
//!  - Set the transfer destination address.
//!  - Set the amount of data you want to transfer (in 16 or 32 bit bursts
//!    depending on the transfer method.)
//!  - Specify the transfer method to be used.
//!
//!  \warning	When \a SO_DMA_REPEAT is set and V-blanking or H-blanking are
//!		  		selected as the moment of DMA startup, DMA is restarted when
//!		  		the next startup condition occurs (a VBlank or HBlank). In this
//!				mode, restarting will continue as long as the DMA enable flag
//!				is set.  To clear this flag and prevent the DMA from restarting,
//!				use the \a SoDMAStop method
//!
//!  \warning	When transferring data to OAM or OBJ VRAM by DMA during a
//!				H-blank, the HBlank must first be freed from OBJ display
//!				hardware processing periods using the \a SO_REG_DISP_CNT
//!				register.

//! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

#define SO_DMA_DEST_INC		0				//!< The destination counter is incremented after each transfer (default)
#define SO_DMA_DEST_DEC		(SO_BIT_5)		//!< The destination counter is decremented after each transfer
#define SO_DMA_DEST_FIX		(SO_BIT_6)		//!< Fixed

#define SO_DMA_SOURCE_INC	0				//!< The source counter is incremented after each transfer (default)
#define SO_DMA_SOURCE_DEC	(SO_BIT_7)		//!< The source counter is decremented after each transfer
#define SO_DMA_SOURCE_FIX	(SO_BIT_8)		//!< Fixed

#define SO_DMA_REPEAT		(SO_BIT_9)		//!< This will cause the DMA to repeat every HBlank or VBlank if you have that specified in your timing bits

#define SO_DMA_16			0				//!< Copies in 16-bit words (default)
#define SO_DMA_32			(SO_BIT_10)		//!< Copies in 32-bit words

#define SO_DMA_START_NOW	0				//!< Start immediately (default)
#define SO_DMA_START_VBL	(SO_BIT_12)		//!< Start at the VBlank
#define SO_DMA_START_HBL	(SO_BIT_13)		//!< Start at the HBlank

//! Start when direct sound FIFO is empty.
//! Only available for DMA channel 1 and 2.
#define SO_DMA_START_FIFO	(SO_BIT_12 | SO_BIT_13)

//! Causes the end of the DMA transfer to generate an interrupt
#define SO_DMA_INTERRUPT	(SO_BIT_14)		


// ----------------------------------------------------------------------------
// Macros;
// ----------------------------------------------------------------------------

//! \brief copy one block of memory to another
#define SO_DMA_MEMCPY(src, dest, wordcount)     \
{                                                   \
	SoDMATransfer(3, (void*)src, (void*)dest, wordcount, \
	  SO_DMA_SOURCE_INC|SO_DMA_DEST_INC|SO_DMA_32); \
}

//! \brief copy one block of memory to another (back to front order)
#define SO_DMA_MEMCPY_REV(src, dest, wordcount)     \
{                                                   \
	SoDMATransfer(3, (u32*)(src)+((wordcount)-1),   \
	  (u32*)(dest)+((wordcount)-1), wordcount,      \
	  SO_DMA_SOURCE_DEC|SO_DMA_DEST_DEC|SO_DMA_32); \
}

//! \brief set the contents of memory to a fixed value
#define SO_DMA_MEMSET(dest, wordcount, value)    \
{                                                   \
	u32 data = value;                               \
	SoDMATransfer(3, &data, (void*)dest, wordcount,        \
	SO_DMA_SOURCE_FIX|SO_DMA_DEST_INC|SO_DMA_32); \
}


// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------
void SoDMAStop( u32 a_Channel );

void SoDMATransfer ( u32 a_Channel, void* a_Source, void* a_Destination, u32 a_WordCount, u32 a_Control );

void SoDMA0Transfer( const void *a_Source, void *a_Destination, u32 a_WordCount, u32 a_Control );
void SoDMA1Transfer( const void *a_Source, void *a_Destination, u32 a_WordCount, u32 a_Control );
void SoDMA2Transfer( const void *a_Source, void *a_Destination, u32 a_WordCount, u32 a_Control );
void SoDMA3Transfer( const void *a_Source, void *a_Destination, u32 a_WordCount, u32 a_Control );

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
