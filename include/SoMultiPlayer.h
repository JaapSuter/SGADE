// ----------------------------------------------------------------------------
/*!
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMultiPlayer.h
	\author		Jaap Suter
	\date		March 7 2002
	\ingroup	SoMultiPlayer

	See the \a SoMultiPlayer module for more information.
	
*/
// ----------------------------------------------------------------------------

#ifndef SO_MULTIPLAYER_H
#define SO_MULTIPLAYER_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoMultiPlayer SoMultiPlayer

	\brief	  Module for handling multiplayer for 2, 3 or 4 players.
	
	\todo     This module uses a hardcoded timer 3, we should make 
			  this user definable.

	This module is for the GBA's multiplayer system where you can transfer
	packets from and to up to three other GBAs. It supports extensive error
	detection, and handles all possible failure cases. You can send a fair
	amount of bytes each frame, and it's very stable. If you lose a packet,
	it's because a cable got disconnected, or because you connected a third
	GBA in a 2 player session (for example).

	In short, it rocks, it's Nintendo guideline-compatible, and I suggest you
	use it. Cheers.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

/*!
	\brief Number of 16 bit transfers in a single packet. 
	
	The packet size in bytes is twice this number. Don't make this package too 
	big please, otherwise you won't be able to complete a single packet
	transfer in a single frame. At a framerate of 60hz, I've had success with
	transfer sizes up to 16 (32 bytes).

	Of course you should always be sending as few bits as possible anyway.
*/
#define SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE	4


/*!
	\brief	Frame rate at which you want each packet transfer to be complete.
	
	Of course this is related to the packet size you are using. If you have a
	large packet size you might not be able to complete it at 60 hz. Just
	fiddle with this value, and the \a SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE
	value until you have a satisfactory result.

	I've sent up to 32 bytes (16 transfers) stable at 60 hz. But don't qoute me
	on that :) hehe.
*/
#define SO_MULTIPLAYER_FRAME_RATE				60

// Different baud rates
#define SO_MULTI_PLAYER_9600_BPS				0	//!< Baud rate: 9600 bps
#define SO_MULTI_PLAYER_38400_BPS				1	//!< Baud rate: 38400 bps
#define SO_MULTI_PLAYER_57600_BPS				2	//!< Baud rate: 57600 bps
#define SO_MULTI_PLAYER_115200_BPS				3	//!< Baud rate: 115200 bps

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

/*!
	\brief	Callback routine that is called whenever something is wrong with
	        the communication 
	
	\return	Let it return \a TRUE if you want to continue trying, or \a FALSE
	        if you want to stop trying.
*/
typedef bool (*SoMultiPlayerFailureCallback)( void );

// ----------------------------------------------------------------------------
// Functions;
// ----------------------------------------------------------------------------

void SoMultiPlayerInitialize( void );

bool SoMultiPlayerStart( u32 a_BaudRate, u32 a_NumPlayers );
void SoMultiPlayerStop(  void );

void SoMultiPlayerUpdate( u16 a_OutGoingPacket[ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ], 
						  u16 a_IncomingPackets[ 4 ][ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ] );

void SoMultiPlayerSetFailureCallback( SoMultiPlayerFailureCallback a_FailureCallback );

u32	 SoMultiPlayerGetNumPlayers( void );
u32	 SoMultiPlayerGetID(			void );


// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
