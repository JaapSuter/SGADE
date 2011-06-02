// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoDMA.c
	\author		Gabriele Scibilia, Jaap Suter
	\date		Jan 31 2002
	\ingroup	SoDMA

	See the \a SoDMA module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoDMA.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

//! \internal Turns the DMA transfer on
#define SO_DMA_ENABLE		(SO_BIT_15)		

//! \internal DMA n Source Address Register
#define SO_REG_DMA_N_SAD(n)		(*((volatile u32*)(0x040000B0 + ((n) * 0x0C))))	

//! \internal DMA n Destination Address Register
#define SO_REG_DMA_N_DAD(n)		(*((volatile u32*)(0x040000B4 + ((n) * 0x0C))))	

//! \internal DMA n Control Register (word count)
#define SO_REG_DMA_N_CNT_L(n)	(*((volatile u16*)(0x040000B8 + ((n) * 0x0C))))	

//! \internal DMA n Control Register (control)
#define SO_REG_DMA_N_CNT_H(n)	(*((volatile u16*)(0x040000BA + ((n) * 0x0C))))	

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Stops DMA transfer
	
	\param	a_Channel		DMA channel
*/
// ----------------------------------------------------------------------------
void SoDMAStop( u32 a_Channel )
{
	// Assert that the channel is ok;
	SO_ASSERT( a_Channel < 4, "Invalid DMA channel." );

	// Stop;
	SO_REG_DMA_N_CNT_H( a_Channel ) &= ~SO_DMA_ENABLE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Transfers a memory block using DMA
	
	\param	a_Channel		DMA channel
	\param	a_Source		Source address
	\param	a_Destination	Destination address
	\param	a_WordCount		Set the number of data items transferred
	\param	a_Control		Specifies the transfer method
*/
// ----------------------------------------------------------------------------
void SoDMATransfer( u32 a_Channel, void* a_Source, void* a_Destination, u32 a_WordCount, u32 a_Control )
{
	switch ( a_Channel )
	{
		case 0 : return SoDMA0Transfer( a_Source, a_Destination, a_WordCount, a_Control );
		case 1 : return SoDMA1Transfer( a_Source, a_Destination, a_WordCount, a_Control );
		case 2 : return SoDMA2Transfer( a_Source, a_Destination, a_WordCount, a_Control );
		case 3 : return SoDMA3Transfer( a_Source, a_Destination, a_WordCount, a_Control );

		default :

			// This shouldn't be reachable.
			SO_ASSERT( false, "Illegal default switch case reached." );

			// For release builds;
			return SoDMA3Transfer( a_Source, a_Destination, a_WordCount, a_Control );
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Transfers a memory block using DMA channel 0
	
	\param	a_Source		Source address
	\param	a_Destination	Destination address
	\param	a_WordCount		Set the number of data items transferred
	\param	a_Control		Specifies the transfer method
	
	DMA0 has the highest priority, and is therefore never interrupted.
	It's best to use it for short bursts of transfer that need to be reliable, for example
	during HBlank intervals.

	\note		The area 0x00000000-0x7FFFFFF (All RAM space) can
				be specified as source and destination.

				The amount of data can be specified in the range 0x0000 to 0x3FFF
				(when 0x0000h is set, 0x4000 is used).
				In 16-bit transfers, up to 0x4000 * 2 bytes = 0x8000 bytes can be
				transferred, and in 32-bit transfers, up to 0x4000 * 4 = 0x10000
				bytes can be transferred.
*/
// ----------------------------------------------------------------------------
void SoDMA0Transfer( const void *a_Source, void *a_Destination, u32 a_WordCount, u32 a_Control )
{
	SO_REG_DMA_N_SAD( 0 ) = (u32) a_Source;
	SO_REG_DMA_N_DAD( 0 ) = (u32) a_Destination;
	SO_REG_DMA_N_CNT_L( 0 ) = a_WordCount;
	SO_REG_DMA_N_CNT_H( 0 ) = a_Control | SO_DMA_ENABLE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Transfers a memory block using DMA channel 1
	
	\param	a_Source		Source address
	\param	a_Destination	Destination address
	\param	a_WordCount		Set the number of data items transferred
	\param	a_Control		Specifies the transfer method
	
	\note		The area 0x00000000 to 0xFFFFFFF (ROM and RAM) can be used as source.

				The area 0x00000000 to 0x7FFFFFF (RAM only) can
				be used as destination.

				The amount of data can be specified in the range 0x0000 to 0x3FFF
				(when 0x0000h is set, 0x4000 is used).
				In 16-bit transfers, up to 0x4000 * 2 bytes = 0x8000 bytes can be
				transferred, and in 32-bit transfers, up to 0x4000 * 4 = 0x10000
				bytes can be transferred.

				The word-count register setting is not used in direct-sound transfers.
				With each request received from sound FIFO, 32 bits * 4 words of
				sound data are transferred.

				When in direct sound transfers, the destination address is
				fixed and unrelated to the control settings and the data are transferred
				in 32-bit units, make sure to select SO_DMA_REPEAT too.

				When the Gamepakbus has been set to the source address, make sure
				you select increment.
*/
// ----------------------------------------------------------------------------
void SoDMA1Transfer( const void *a_Source, void* a_Destination, u32 a_WordCount, u32 a_Control )
{
	SO_REG_DMA_N_SAD( 1 ) = (u32) a_Source;
	SO_REG_DMA_N_DAD( 1 ) = (u32) a_Destination;
	SO_REG_DMA_N_CNT_L( 1 ) = a_WordCount;
	SO_REG_DMA_N_CNT_H( 1 ) = a_Control | SO_DMA_ENABLE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Transfers a memory block using DMA channel 2
	
	\param	a_Source		Source address
	\param	a_Destination	Destination address
	\param	a_WordCount		Set the number of data items transferred
	\param	a_Control		Specifies the transfer method
	
	DMA2 is used for direct sound functions, which require relatively high priority,
	or for general-purpose transfers.

	\note		The area 0x00000000 to 0xFFFFFFF (ROM and RAM) can be used as source.

				The area 0x00000000 to 0x7FFFFFF (RAM only) can
				be used as destination.

				The amount of data can be specified in the range 0x0000 to 0x3FFF
				(when 0x0000h is set, 0x4000 is used).
				In 16-bit transfers, up to 0x4000 * 2 bytes = 0x8000 bytes can be
				transferred, and in 32-bit transfers, up to 0x4000 * 4 = 0x10000
				bytes can be transferred.

				The word-count register setting is not used in direct-sound transfers.
				With each request received from sound FIFO, 32 bits * 4 words of
				sound data are transferred.

				When in direct sound transfers, the destination address is
				fixed and unrelated to the control settings and the data are transferred
				in 32-bit units, make sure to select SO_DMA_REPEAT too.

				When the Gamepakbus has been set to the source address, make sure
				you select increment.
*/
// ----------------------------------------------------------------------------
void SoDMA2Transfer( const void *a_Source, void* a_Destination, u32 a_WordCount, u32 a_Control )
{
	SO_REG_DMA_N_SAD( 2 ) = (u32) a_Source;
	SO_REG_DMA_N_DAD( 2 ) = (u32) a_Destination;
	SO_REG_DMA_N_CNT_L( 2 ) = a_WordCount;
	SO_REG_DMA_N_CNT_H( 2 ) = a_Control | SO_DMA_ENABLE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Transfers a memory block using DMA channel 3
	
	\param	a_Source		Source address
	\param	a_Destination	Destination address
	\param	a_WordCount		Set the number of data items transferred
	\param	a_Control		Specifies the transfer method
	
	DMA3 is used for the most general types of transfers.

	\warning	The area 0x00000000 - 0x0FFFFFFF (all addresses) 
				can be specified as source and destination addresses.

				The number of data items can be specified in the range 0x0000
				to 0xFFFF (when 0x0000h is used, 0x10000 is done.  Thus, in
				16-bit data transfer mode, up to 0x10000 * 2 bytes = 0x20000
				bytes can be transferred, and in 32-bit data transfer mode, up
				to 0x10000 * 4 bytes = 0x40000 bytes can be transferred.

				When transfering from ROM do not use the SO_DMA_REPEAT function.

				If the source is from ROM, make sure you select SO_DMA_SOURCE_INC (increment).
*/
// ----------------------------------------------------------------------------
void SoDMA3Transfer( const void *a_Source, void* a_Destination, u32 a_WordCount, u32 a_Control )
{
	SO_REG_DMA_N_SAD( 3 ) = (u32) a_Source;
	SO_REG_DMA_N_DAD( 3 ) = (u32) a_Destination;
	SO_REG_DMA_N_CNT_L( 3 ) = a_WordCount;
	SO_REG_DMA_N_CNT_H( 3 ) = a_Control | SO_DMA_ENABLE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
