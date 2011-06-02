// ----------------------------------------------------------------------------
/*!
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMultiPlayer.c
	\author		Jaap Suter
	\date		March 7 2002
	\ingroup	SoMultiPlayer

	See the \a SoMultiPlayer module for more information.
	
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoMultiPlayer.h"
#include "SoDMA.h"
#include "SoIntManager.h"
#include "SoSystem.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define SO_MULTI_PLAYER_REG_RCNT			(*(volatile u16*)0x4000134)		//!< \internal Used in this file only.

#define SO_MULTI_PLAYER_REG_SIO_CNT			(*(volatile u16*)0x4000128) 	//!< \internal Used in this file only.
#define SO_MULTI_PLAYER_REG_SIO_SEND		(*(volatile u16*)0x400012A) 	//!< \internal Used in this file only.
#define SO_MULTI_PLAYER_REG_SIO_RECEIVE		( (volatile u16*)0x4000120)		//!< \internal Used in this file only.

#define SO_MULTI_PLAYER_REG_SIO_TIMER_CNT	(*(volatile u16*)0x400010E) 	//!< \internal Used in this file only.
#define SO_MULTI_PLAYER_REG_SIO_TIMER_DATA	(*(volatile u16*)0x400010C) 	//!< \internal Used in this file only.


// ----------------------------------------------------------------------------
// Constants;
// ----------------------------------------------------------------------------

//! \internal The following magic number is send to all AGB's when the 
//!			  s_Session starts (to decipher what m_ID each GBA has), or when a packet transfer
//!			  starts (to synchronize). Number can't be 0xFFFF or 0x0;
static const u16 SO_MULTI_PLAYER_MAGIC_NUMBER = 0xDEAD;

/*!
	\brief		Master communciation interrupt timer-wait setting;
	
	\internal 

	This number specifies the amount of clockcyles the master waits before it initiates another 16 bit transfer;
*/
static const u16 SO_MULTI_PLAYER_SIO_TIMER_COUNT = 0xFFFF - (	 (    SO_GBA_CLOCKCYCLES_PER_SECOND 
							/ (SO_MULTIPLAYER_FRAME_RATE + (SO_MULTIPLAYER_FRAME_RATE/10))
							 ) / SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE);


// ----------------------------------------------------------------------------
// Statics
// ----------------------------------------------------------------------------

/*!
	\brief Structure containing info about a multiplayer s_Session, for a particular GBA;
	\internal
*/
static struct
{
	s32	m_TransferCount;	//!< Number of transfers done for current packet, -1 if no transfer is going on;
														
	u8	m_NumPlayers;		//!< Number of AGB's connected to eachother. Can be 0, 2, 3 or 4. 

	s8	m_ID;				//!< Identifier of the GBA, 0 for the master, 1, 2 or 3 for slaves, negative 
							//!< when no multiplayer s_Session is active;
	
														
	//! Function that is called when communication fails;
	SoMultiPlayerFailureCallback m_FailureCallback;		
																	
	u16* m_CurrentOutGoingPacket;		//!< This is the packet that is send away during a transfer;
	u16* m_NextOutGoingPacket;			//!< This is the packet that is loaded with the data you want to send;

	//! The previous two pointers will point into this array, and be
	//! every time a packet transfer is completed.
	u16	 m_OutGoingPackets[ 2 ][ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ];
																		
																	
	u16* m_CurrentIncomingPacket[ 4 ];	//!< These are the four packets that will be filled with the incoming data.
	u16* m_PrevIncomingPacket[ 4 ];		//!< These are the four packets that were filled during a previous transfer.
	
	//! The previous two arrays of pointers will point in this buffer, 
	//! and swapped everytime a packet transfer is complete;
	u16  m_IncomingPackets[ 2 ][ 4 ][ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ];	

} s_Session;

//! \internal To prevent multiple initialization.
static bool s_Initialized = false;


// ----------------------------------------------------------------------------
// Forward declarations;
// ----------------------------------------------------------------------------
void SoInterruptMultiPlayerSetup( void );
void SoInterruptMultiPlayerTransfer( void );

// ----------------------------------------------------------------------------
// Implementation
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Initializes the multi player system;

	This should be the first SoMultiPlayer function you call. Multiple calls 
	don't matter.
*/
// ----------------------------------------------------------------------------
void SoMultiPlayerInitialize( void )
{
	// Maybe we are already initialized;
	if ( s_Initialized ) return;

	// Set the incoming packet pointers;
	s_Session.m_CurrentOutGoingPacket = s_Session.m_OutGoingPackets[ 0 ];
	s_Session.m_NextOutGoingPacket	  = s_Session.m_OutGoingPackets[ 0 ];

	// Set the outgoing packet pointers;
	s_Session.m_CurrentIncomingPacket[ 0 ] = s_Session.m_IncomingPackets[ 0 ][ 0 ];
	s_Session.m_CurrentIncomingPacket[ 1 ] = s_Session.m_IncomingPackets[ 0 ][ 1 ];
	s_Session.m_CurrentIncomingPacket[ 2 ] = s_Session.m_IncomingPackets[ 0 ][ 2 ];
	s_Session.m_CurrentIncomingPacket[ 3 ] = s_Session.m_IncomingPackets[ 0 ][ 3 ];
	
	s_Session.m_PrevIncomingPacket[ 0 ] = s_Session.m_IncomingPackets[ 1 ][ 0 ];
	s_Session.m_PrevIncomingPacket[ 1 ] = s_Session.m_IncomingPackets[ 1 ][ 1 ];
	s_Session.m_PrevIncomingPacket[ 2 ] = s_Session.m_IncomingPackets[ 1 ][ 2 ];
	s_Session.m_PrevIncomingPacket[ 3 ] = s_Session.m_IncomingPackets[ 1 ][ 3 ];

	// Reset;
	s_Session.m_NumPlayers					= 0;
	s_Session.m_FailureCallback				= NULL;
	s_Session.m_ID							= -1;
	s_Session.m_TransferCount				= -1;

	// Initialize the interrupt manager;
	SoIntManagerInitialize();
	
	// From now on, we are initialized.
	s_Initialized = true;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the callback routine that is called whenever something is wrong with 
			the communication either during setup, or during communication.

	\param	a_FailureCallback	The failure callback. Can be set to NULL if you don't
								want to use the callback.
	
	\return	Let the callback return \a true if you want to continue trying, or \a false if 
			you want to stop trying.
*/
// ----------------------------------------------------------------------------
void SoMultiPlayerSetFailureCallback( SoMultiPlayerFailureCallback a_FailureCallback )
{
	s_Session.m_FailureCallback = a_FailureCallback;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/*!
	\brief Starts a multiplayer session. 

	\param a_BaudRate	Speed of communication. Can be one of the SO_MULTI_PLAYER_*_BPS
						defines. Not that the the effective baudrate 
						decreases with the amount of players connected. The speed using a baudrate of 115200 with
						2 players is not the same as it is with 4 players.

	\param a_NumPlayers	Number of participants in this multiplayer s_Session. Can be 2, 3 or 4.
	
	\return			\a true if the session is active and can be used. \a false when the gameboys are not 
					properly connected.

	\warning		The set failure callback will almost certainly fail at least once because of timing issues. I could
					add a couple of NOP instructies inbetween enabling and testing, but it's better to put some sort of
					countdown thingy in the failure callback, so it fails after 5 seconds of trying or maybe when the user
					presses a cancel-button.

	\warning		Do not call this routine again, without calling \a stopMultiSession again first.
*/
// ----------------------------------------------------------------------------
bool SoMultiPlayerStart( u32 a_BaudRate, u32 a_NumPlayers )
{
	// Assert the input;
	SO_ASSERT( (a_NumPlayers > 1) && (a_NumPlayers < 4), 
			   "Valid number of players for a multiplayer s_Session are 2, 3, or 4" );

	// Assert that the s_Session isn't active yet;
	SO_ASSERT( s_Session.m_NumPlayers == 0, "You are starting a s_Session that was already active." );

	// Enable the interrupt master;
	SoIntManagerEnableInterruptMaster();

	// Set the general purpose communication register so 
	// it can do serial communication
	SO_MULTI_PLAYER_REG_RCNT = 0;

	// Enable 16 bit multi player SIO mode;
	// Setting bit 12 to zero and bit 13 to one does this (if 
	// the general purpose communication register is set to zero).
	SO_MULTI_PLAYER_REG_SIO_CNT = SO_BIT_13;

	// Set the baud rate and enable interrupt request flag (can't do it at the 
	// same time in the previous statement, because of hardware reasons);
	// The 14th bit enables interrupt request.
	SO_MULTI_PLAYER_REG_SIO_CNT |= a_BaudRate | SO_BIT_14;

	// Wait until every connected GBA has entered multiplayer mode by checking 
	// whether the SD is low or the start-busy flag is high;
	while (	(!(SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_3))			// SD Terminal bit
		   || (SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_7))			// Start-busy bit
	{
		// They are not connected yet, so call the callback if there is one.
		if ( s_Session.m_FailureCallback != NULL ) 
		{
			// There is one, so call it;
			if ( ! s_Session.m_FailureCallback() ) 
			{
				// Apparently, we want to stop trying
				return false;
			}
		}
	}

	// Set the number of players;
	s_Session.m_NumPlayers = a_NumPlayers;

	// We are going to send the magic number 
	// to each GBA to identify those connected
	SO_MULTI_PLAYER_REG_SIO_SEND = SO_MULTI_PLAYER_MAGIC_NUMBER;	

	// Is SI low, then we are the master;
	if ( (!(SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_2) ) )		// SI terminal bit
	{
		// Set the setup-multi handler on the timer interrupt;
		SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_TIMER_3, SoInterruptMultiPlayerSetup );

		// The master communication interrupt triggers on the timer;
		SoIntManagerEnableInterrupt( SO_INTERRUPT_TYPE_TIMER_3 );

		// Set the timer counter;
		SO_MULTI_PLAYER_REG_SIO_TIMER_DATA = SO_MULTI_PLAYER_SIO_TIMER_COUNT;	

		// Give the start send signal;
		SO_MULTI_PLAYER_REG_SIO_CNT |= SO_BIT_7;

		// Start the timer, with the speed of one clockcycle per count,
		// an interrupt request, and enable flag (so it starts immediately);
		SO_MULTI_PLAYER_REG_SIO_TIMER_CNT = SO_BIT_6 | SO_BIT_7;
	}
	else
	{
		// Set the setup-multi handler on the communication interrupt;
		SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_SERIAL, SoInterruptMultiPlayerSetup );		

		// We are the slave so we trigger on the communication interrupt;
		SoIntManagerEnableInterrupt( SO_INTERRUPT_TYPE_SERIAL );
	}
		
	// Keep waiting until we have been identified, which means
	// everybody is connected correctly;
	while ( s_Session.m_ID == -1 )
	{
		// There is some problem with the link cable, call the 
		// callback. If there is one given, ofcourse.
		if ( s_Session.m_FailureCallback ) 
		{
			// There is a callback, so call it;
			if ( ! s_Session.m_FailureCallback() ) 
			{
				// Apparently, we want to stop trying. 
				
				// Are we the master;
				if ( (!(SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_2) ) )		
				{
					// We have to remove the installed interrupt;
					SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_TIMER_3, NULL );
				}
				else
				{
					// We have to remove the installed interrupt;
					SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_SERIAL, NULL );
				}			

				// Reset the session;
				s_Session.m_NumPlayers = 0;
				s_Session.m_ID		   = -1;

				// And then we return;
				return false;
			}
		}
	}

	// Are we the master;
	if ( s_Session.m_ID == 0 ) 
	{
		// Set the transfer handler on a timer interrupt;
		SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_TIMER_3, SoInterruptMultiPlayerTransfer );
	}
	else
	{
		// Set the transfer handler on a communication interrupt;
		SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_SERIAL, SoInterruptMultiPlayerTransfer );				
	}
	
	// Session running fine;
	return true;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Stops the current multiplayer s_Session.
*/
// ----------------------------------------------------------------------------
void SoMultiPlayerStop()
{
	// Are we the master;
	if ( s_Session.m_ID == 0 )
	{
		// Remove the timer interrupt handler;
		SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_TIMER_3, NULL );
	}
	else
	{
		// Remove the communication interrupt handler;
		SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_SERIAL, NULL );
	}

	// The session is no longer active;
	s_Session.m_NumPlayers = 0;

	// From now on the ID is unknown;
	s_Session.m_ID = -1;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Just call this every frame to handle all communication.

	\param	a_OutGoingPacket	Packet filled with the info you want to send;
	\param	a_IncomingPackets	Array of four packets that will be filled with the received information as far
								as the number of AGB's connected ofcourse.
	
	This is the meat of the module. Here all stuff is done, so to speak...
	Ofcourse you can only call this when a multiplayer session is running. 
	Use \a SoMultiPlayerStart to start a session.

*/
// ----------------------------------------------------------------------------
void SoMultiPlayerUpdate( u16 a_OutGoingPacket[ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ], 
						  u16 a_IncomingPackets[ 4 ][ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ] )
{
	// Assert the system state;
	SO_ASSERT( s_Session.m_NumPlayers > 1, "You can't update the multiplayer session, when no session is running." );

	// Dummy counter for the number of players;
	u32	p = s_Session.m_NumPlayers; 

	// Packet pointer for swapping;
	u16* swapPacket;
	
	// Copy the outgoing packet;
	/*
	SoDMATransfer( 3, a_OutGoingPacket, s_Session.m_NextOutGoingPacket, 
				   SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE, 
				   SO_DMA_DEST_INC | SO_DMA_SOURCE_INC | SO_DMA_16 | SO_DMA_START_NOW );
				   */
	s_Session.m_NextOutGoingPacket[ 0 ] = a_OutGoingPacket[ 0 ];
	s_Session.m_NextOutGoingPacket[ 1 ] = a_OutGoingPacket[ 1 ];
	s_Session.m_NextOutGoingPacket[ 2 ] = a_OutGoingPacket[ 2 ];
	s_Session.m_NextOutGoingPacket[ 3 ] = a_OutGoingPacket[ 3 ];
	
	// Copy the received packets;
	while ( p-- ) 
	{
		/*
		SoDMATransfer( 3, s_Session.m_PrevIncomingPacket[ p ], a_IncomingPackets[ p ], 
					   SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE, 
					   SO_DMA_DEST_INC | SO_DMA_SOURCE_INC | SO_DMA_16 | SO_DMA_START_NOW );
					   */
		a_IncomingPackets[ p ][ 0 ] = s_Session.m_PrevIncomingPacket[ p ][ 0 ];
		a_IncomingPackets[ p ][ 1 ] = s_Session.m_PrevIncomingPacket[ p ][ 1 ];
		a_IncomingPackets[ p ][ 2 ] = s_Session.m_PrevIncomingPacket[ p ][ 2 ];
		a_IncomingPackets[ p ][ 3 ] = s_Session.m_PrevIncomingPacket[ p ][ 3 ];
	}

	// If all previous transfers have been finished;
	if ( s_Session.m_TransferCount == -1 ) 
	{
		// Swap the send buffer;
		swapPacket						  = s_Session.m_CurrentOutGoingPacket;
		s_Session.m_CurrentOutGoingPacket = s_Session.m_NextOutGoingPacket;
		s_Session.m_NextOutGoingPacket	  = swapPacket;
	
		// If we are the master;
		if ( s_Session.m_ID == 0 )
		{
			// Give the start send signal;
			SO_MULTI_PLAYER_REG_SIO_CNT |= SO_BIT_7;

			// Start the timer, with the speed of one clockcycle per count,
			// an interrupt request, and enable flag (so it starts immediately);
			SO_MULTI_PLAYER_REG_SIO_TIMER_CNT = SO_BIT_6 | SO_BIT_7;
		}
	}
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/*!
	\brief	Returns the ID of this GBA in a multiplayer session.

	\return	The ID of this GBA in the session. 0 if it's the master, 1, 2, or 3 if it's a slave.
*/
// ----------------------------------------------------------------------------
u32	SoMultiPlayerGetID(	void )
{
	// Assert the system state;
	SO_ASSERT( s_Session.m_NumPlayers > 1, "You can't get the GBA ID when no multi session is running." );

	// Done;	
	return s_Session.m_ID;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Returns the number of players in this s_Session.
	\return	The number of players in this s_Session. Can be 2, 3 or 4.
*/
// ----------------------------------------------------------------------------
u32	SoMultiPlayerGetNumPlayers( void )
{
	// Assert the system state;
	SO_ASSERT( s_Session.m_NumPlayers > 1, "You can't get the number of players when no multi session is running." );

	// Done;	
	return s_Session.m_NumPlayers;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Communication interrupt handler used during setup;

	\internal
*/
// ----------------------------------------------------------------------------
void SoInterruptMultiPlayerSetup( void )
{
	// Variables;
	u32 numPlayersConnected = 0;

	// Are we the master, then stop the timer. Bit 2 is the SI terminal bit.
	if ( (!(SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_2) ) ) SO_MULTI_PLAYER_REG_SIO_TIMER_CNT = 0;

	// Check that no error occured. Bit 6 is the error bit.
	if ( ! (SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_6) )
	{
		// See how many GBA's are connected, by checking the transfer values for the magic numbers;
		if ( SO_MULTI_PLAYER_REG_SIO_RECEIVE[ 0 ] == SO_MULTI_PLAYER_MAGIC_NUMBER ) numPlayersConnected++;
		if ( SO_MULTI_PLAYER_REG_SIO_RECEIVE[ 1 ] == SO_MULTI_PLAYER_MAGIC_NUMBER ) numPlayersConnected++;
		if ( SO_MULTI_PLAYER_REG_SIO_RECEIVE[ 2 ] == SO_MULTI_PLAYER_MAGIC_NUMBER ) numPlayersConnected++;
		if ( SO_MULTI_PLAYER_REG_SIO_RECEIVE[ 3 ] == SO_MULTI_PLAYER_MAGIC_NUMBER ) numPlayersConnected++;
	}

	// Fill the send register with the magic number again;
	SO_MULTI_PLAYER_REG_SIO_SEND = SO_MULTI_PLAYER_MAGIC_NUMBER;	

	// Are the correct number of players connected;
	if ( numPlayersConnected == s_Session.m_NumPlayers )
	{
		// Find out what ID this GBA has (bit 4 and 5 are the GBA's ID).
		s_Session.m_ID = (SO_MULTI_PLAYER_REG_SIO_CNT >> 4) & 3;
	}
	else
	{
		// If we are the master, start a send again;
		if ( (!(SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_2) ) )
		{
			// Give the start send signal;
			SO_MULTI_PLAYER_REG_SIO_CNT |= SO_BIT_7;

			// Start the timer, with the speed of one clockcycle per count,
			// an interrupt request, and enable flag (so it starts immediately);
			SO_MULTI_PLAYER_REG_SIO_TIMER_CNT = SO_BIT_6 | SO_BIT_7;
		}
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Communication transfer interrupt routine

	\internal

	This routine is called everytime a single register transfer is finished.
*/ 
// ----------------------------------------------------------------------------
void SoInterruptMultiPlayerTransfer( void )
{
	// Dummy counter for the number of players;
	u32	p = s_Session.m_NumPlayers; 

	// Packet pointer for swapping;
	u16* swapPacket;

	// Are we the master, then stop the timer;
	if ( s_Session.m_ID == 0 ) SO_MULTI_PLAYER_REG_SIO_TIMER_CNT = 0;

	// Check that no error occured. Bit 6 is the error bit.
	if ( SO_MULTI_PLAYER_REG_SIO_CNT & SO_BIT_6 )
	{
		// Is there a callback installed.
		if ( s_Session.m_FailureCallback ) 
		{
			// Call the failure callback;
			if ( ! s_Session.m_FailureCallback() ) 
			{
				// We don't want to continue trying...
				return;
			}
		}
	}
	
	// Is this not the first send (cause then we haven't received anything yet);
	if ( s_Session.m_TransferCount > -1 )
	{
		// Put the receive registers in the incoming data buffers;
		while ( p-- )
		{
			s_Session.m_CurrentIncomingPacket[ p ][ s_Session.m_TransferCount ] 
			= SO_MULTI_PLAYER_REG_SIO_RECEIVE[ p ];
		}
	}
	else
	{
		// Iterate over every player;
		while ( p-- ) 
		{
			// Did we receive the magic synchronization number from everyone;
			if ( SO_MULTI_PLAYER_REG_SIO_RECEIVE[ p ] != SO_MULTI_PLAYER_MAGIC_NUMBER )
			{
				// Is there a callback installed.
				if ( s_Session.m_FailureCallback ) 
				{
					// Call the failure callback;
					if ( ! s_Session.m_FailureCallback() ) 
					{
						// We don't want to continue trying;
						return;
					}
				}
			}
		}
		
	}

	// Increase the transfer count;
	s_Session.m_TransferCount++;
	
	// Are there any transfers left;
	if ( s_Session.m_TransferCount < SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE )
	{	
		// Setup the send data;
		SO_MULTI_PLAYER_REG_SIO_SEND = s_Session.m_CurrentOutGoingPacket[ s_Session.m_TransferCount ];	

		// Are we the master;
		if ( s_Session.m_ID == 0 )
		{
			// Give the start send signal;
			SO_MULTI_PLAYER_REG_SIO_CNT |= SO_BIT_7;

			// Start the timer, with the speed of one clockcycle per count,
			// an interrupt request, and enable flag (so it starts immediately);
			SO_MULTI_PLAYER_REG_SIO_TIMER_CNT = SO_BIT_6 | SO_BIT_7;
		}
	}
	else
	{
		// Packet transfer complete;
		s_Session.m_TransferCount = -1;

		// Swap the receive buffers;
		swapPacket							   = s_Session.m_CurrentIncomingPacket[ 0 ];
		s_Session.m_CurrentIncomingPacket[ 0 ] = s_Session.m_PrevIncomingPacket[ 0 ];
		s_Session.m_PrevIncomingPacket[ 0 ]	   = swapPacket;

		swapPacket							   = s_Session.m_CurrentIncomingPacket[ 1 ];
		s_Session.m_CurrentIncomingPacket[ 1 ] = s_Session.m_PrevIncomingPacket[ 1 ];
		s_Session.m_PrevIncomingPacket[ 1 ]	   = swapPacket;
		
		swapPacket							   = s_Session.m_CurrentIncomingPacket[ 2 ];
		s_Session.m_CurrentIncomingPacket[ 2 ] = s_Session.m_PrevIncomingPacket[ 2 ];
		s_Session.m_PrevIncomingPacket[ 2 ]	   = swapPacket;

		swapPacket							   = s_Session.m_CurrentIncomingPacket[ 3 ];
		s_Session.m_CurrentIncomingPacket[ 3 ] = s_Session.m_PrevIncomingPacket[ 3 ];
		s_Session.m_PrevIncomingPacket[ 3 ]	   = swapPacket;

		// Put the synchronization number in the send register, for a
		// possible next transfer;
		SO_MULTI_PLAYER_REG_SIO_SEND = SO_MULTI_PLAYER_MAGIC_NUMBER;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
