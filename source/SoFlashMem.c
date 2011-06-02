// ----------------------------------------------------------------------------
/*!
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoFlashMem.c
	\author		Gabriele Scibilia
	\date		Dec 9 2001	
	\ingroup	SoFlashMem

	Implementation of SoFlashMem.h
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoFlashMem.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------
#define		SO_SRAM_START_ADDRESS	(0xE000000)		//!< \internal Start of SRAM
#define		SO_SRAM_END_ADDRESS		(0xE00FFFF)		//!< \internal End of SRAM

// ----------------------------------------------------------------------------
/*!
	\brief Saves a byte (8 bit) into SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to write.
	\param	a_Value		Value you want to write in SRAM.
	
	\return	The amount of bytes written. Add this to \a a_Offset to get the 
			new offset.
*/
// ----------------------------------------------------------------------------
u16 SoFlashMemSaveByte( u16 a_Offset, u8 a_Value )
{
	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + sizeof( a_Value ) < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Saves value
	*(u8 *) ( sRam ) = a_Value;

	// Returns written bytes
	return sizeof( a_Value );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Saves a halfword (16 bit) into SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to write.
	\param	a_Value		Value you want to write in SRAM.
	
	\return	The amount of bytes written. Add this to \a a_Offset to get the 
			new offset.
*/
// ----------------------------------------------------------------------------
u16 SoFlashMemSaveHalfword( u16 a_Offset, u16 a_Value )
{
	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + sizeof( a_Value ) < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Saves value
	*(u8 *) ( sRam     ) = ( a_Value >> 8 );
	*(u8 *) ( sRam + 1 ) = ( a_Value & 0xFF );

	// Returns written bytes
	return sizeof( a_Value );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Saves a word (32 bit) into SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to write.
	\param	a_Value		Value you want to write in SRAM.
	
	\return	The amount of bytes written. Add this to \a a_Offset to get the 
			new offset.
*/
// ----------------------------------------------------------------------------
u16 SoFlashMemSaveWord( u16 a_Offset, u32 a_Value )
{
	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + sizeof( a_Value ) < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Saves value
	*(u8 *) ( sRam     ) = ( a_Value >> 24 ) & 0xFF;
	*(u8 *) ( sRam + 1 ) = ( a_Value >> 16 ) & 0xFF;
	*(u8 *) ( sRam + 2 ) = ( a_Value >>  8 ) & 0xFF;
	*(u8 *) ( sRam + 3 ) = ( a_Value & 0xFF );

	// Returns written bytes
	return sizeof( a_Value );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Saves a buffer into SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to write.
	\param	a_Length	Number of bytes in the buffer.
	\param	a_Buffer	Start of the buffer
	
	\return	The amount of bytes written. Add this to \a a_Offset to get the 
			new offset.
*/
// ----------------------------------------------------------------------------
u16 SoFlashMemSaveBuffer( u16 a_Offset, u16 a_Length, u8* a_Buffer )
{
	// Dummy counter;
	u32 i;

	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + a_Length < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Do it;
	for ( i = 0; i < a_Length; i++ )
	{
		*(u8 *) ( sRam + i ) = a_Buffer[ i ];
	}

	// Returns written bytes
	return a_Length;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Loads a byte (8 bit) from SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to read from.
	
	\return	The byte read.
*/
// ----------------------------------------------------------------------------
u8  SoFlashMemLoadByte( u16 a_Offset )
{
	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + sizeof( u8 ) < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Returns value
	return *(u8 *) ( sRam );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Loads a halfword (16 bit) from SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to read from.
	
	\return	The halfword read.
*/
// ----------------------------------------------------------------------------
u16 SoFlashMemLoadHalfword( u16 a_Offset )
{
	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + sizeof( u16 ) < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Returns value
	return
		(*(u8 *) ( sRam     ) << 8) +
		(*(u8 *) ( sRam + 1 ));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Loads a word (32 bit) from SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to read from.
	
	\return	The word read.
*/
// ----------------------------------------------------------------------------
u32 SoFlashMemLoadWord( u16 a_Offset )
{
	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + sizeof( u32 )  < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Returns value
	return
		(*(u8 *) ( sRam     ) << 24) +
		(*(u8 *) ( sRam + 1 ) << 16) +
		(*(u8 *) ( sRam + 2 ) <<  8) +
		(*(u8 *) ( sRam + 3 ));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Loads a buffer from SRAM
	
	\param	a_Offset	Offset against the start of SRAM. Location you want to read from.
	\param	a_Length	Length of the buffer you want to read.
	
	\retval	a_Buffer	Buffer you want to put the data into.

	\return	Amount of bytes read. Subtract this from \a a_Offset to get the new offset.
*/
// ----------------------------------------------------------------------------
u16 SoFlashMemLoadBuffer( u16 a_Offset, u16 a_Length, u8* a_Buffer )
{
	// Dummy counter;
	u32 i;

	u32 sRam = SO_SRAM_START_ADDRESS + a_Offset;

	SO_ASSERT( sRam >= SO_SRAM_START_ADDRESS, "Writing out of sram boundaries" );
	SO_ASSERT( sRam + a_Length < SO_SRAM_END_ADDRESS, "Writing out of sram boundaries" );

	// Do it;
	for ( i = 0; i < a_Length; i++ )
	{
		a_Buffer[ i ] = *(u8 *) ( sRam + i );
	}

	// Returns read bytes
	return a_Length;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
