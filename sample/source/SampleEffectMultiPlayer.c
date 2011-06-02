// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSampleEffectMultiPlayer.c
	\author		Jaap Suter
	\date		Mar 3 2002
	\ingroup	SoSampleEffects

	See the \a SoSampleEffects module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Sample application includes;
#include "SampleEffects.h"

// ----------------------------------------------------------------------------
// Statics;
// ----------------------------------------------------------------------------
static bool s_CommunicationError = false;

// ----------------------------------------------------------------------------
// Forward declarations;
// ----------------------------------------------------------------------------
bool MultiPlayerSetupFailure( void );
bool MultiPlayerCommunicationFailure( void );

// ----------------------------------------------------------------------------
/*!
	Does a multiplayer effect. Can easily be skipped if no 
	other GBA's are attached.
*/
// ----------------------------------------------------------------------------
void SoSampleEffectMultiPlayer( void )
{
	// Dummy counter;
	u32 i;

	// Multiplayer packets;
	u16 incomingPackets[ 4 ][ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ];
	u16 outGoingPacket[ SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE ];

	// Set the outgoing packet to some data;
	for ( i = 0; i < SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE; i++ )
	{
		outGoingPacket[ i ] = i;
	}
		
	// Go to mode 4;
	SoMode4RendererEnable();

	// Clear the buffers;
	SoMode4RendererClearFrontAndBackBuffer();

	// Set the screen palette to greyscale;
	SoPaletteSetGreyScale( SO_SCREEN_PALETTE, true );

	// Initialize the multiplayer system;
	SoMultiPlayerInitialize();

	// Print a message;
	SoMode4RendererDrawString( 10, 240,  40, "Multiplayer Demonstration", SoFontGetDefaultFontImage() );
	SoMode4RendererDrawString( 10, 240,  80, "Waiting for connection to other GBA......", SoFontGetDefaultFontImage() );
	SoMode4RendererDrawString( 10, 240, 140, "Press start to skip multiplayer demo.",     SoFontGetDefaultFontImage() );
	SoMode4RendererFlip();

	// Install the session setup handler;
	SoMultiPlayerSetFailureCallback( MultiPlayerSetupFailure );

	// Start a multiplayer session for two players;
	if ( ! SoMultiPlayerStart( SO_MULTI_PLAYER_115200_BPS, 2 ) )
	{
		// We failed setting it up, so we exit;
		return;		
	}

	// Multiplayer session setup succesfully, so now attach 
	// the communication error handler;
	SoMultiPlayerSetFailureCallback( MultiPlayerCommunicationFailure );	

	// Tell the user we've established a connection;
	SoMode4RendererDrawString( 10, 240, 10, "Connection Established, Session Active.", SoFontGetDefaultFontImage() );

	// Are we the master;
	if ( SoMultiPlayerGetID() == 0 )
	{
		// I'm the master;
		SoMode4RendererDrawString( 10, 240, 50, "This is the master", SoFontGetDefaultFontImage() );

		// Ask for a keypress, if we are the master;	
		SoMode4RendererDrawString( 10, 240, 140, "Press A to start communicating.", SoFontGetDefaultFontImage() );

		// Show what we just printed;
		SoMode4RendererFlip();
		
		// Wait for a keypress;
		do SoKeysUpdate(); while ( !SoKeysPressed( SO_KEY_A ) );		
	} 		

	// Loop until we press the start key;
	do 
	{	
		// Wait for VBlank;
		SoDisplayWaitForVBlankStart();
		
		// Update the keys;
		SoKeysUpdate();

		// Clear the screen;
		SoMode4RendererClear();

		// Do something with the key presses;
		if ( SoKeysDown( SO_KEY_A ) )
		{
			for ( i = 0; i < SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE; i++ )
			{
				outGoingPacket[ i ]++;
			}
		}
		if ( SoKeysDown( SO_KEY_B ) )
		{
			for ( i = 0; i < SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE; i++ )
			{
				outGoingPacket[ i ]--;
			}
		}

		// Did an error occur;
		if ( s_CommunicationError )
		{
			// Print an error message;
			SoMode4RendererDrawString( 10, 240, 20, "Error communnicating....", SoFontGetDefaultFontImage() );
			SoMode4RendererDrawString( 10, 240, 30, "Press start to exit.", SoFontGetDefaultFontImage() );
		}
		else
		{
			// Update the multiplayer system;
			SoMultiPlayerUpdate( outGoingPacket, incomingPackets );

			// Print what we send;
			SoMode4RendererDrawString( 10, 240, 40, "Send: ", SoFontGetDefaultFontImage() );
			for ( i = 0; i < SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE; i++ )
			{
				SoMode4RendererDrawNumber( 10, 240, 50 + (10*i), outGoingPacket[ i ], SoFontGetDefaultFontImage() );
			}
			
			// Print what we received;
			SoMode4RendererDrawString( 10, 240, 100, "Received: ", SoFontGetDefaultFontImage() );
			if ( SoMultiPlayerGetID() == 0 )
			{
				// Print the packet of the slave;
				for ( i = 0; i < SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE; i++ )
				{
					SoMode4RendererDrawNumber( 10, 240, 110 + (10*i), incomingPackets[ 1 ][ i ], SoFontGetDefaultFontImage() );
				}
			}
			else
			{
				// Print the packet of the master;
				for ( i = 0; i < SO_MULTI_PLAYER_PACKET_TRANSFER_SIZE; i++ )
				{
					SoMode4RendererDrawNumber( 10, 240, 110 + (10*i), incomingPackets[ 0 ][ i ], SoFontGetDefaultFontImage() );
				}
			}
		}	
		
		// Wait for a vblank;
		SoDisplayWaitForVBlankStart();
		
		// Flip;
		SoMode4RendererFlip();
	
	} while ( ! SoKeysPressed( SO_KEY_START ) );

	// Stop the current multiplayer session;
	SoMultiPlayerStop();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Communication setup error handler;

	Function that is going to be called by the \a SoMultiPlayer module during a 
	multiplayer session setup error.
*/
// ----------------------------------------------------------------------------
bool MultiPlayerSetupFailure( void )
{
	// Update the key status;
	SoKeysUpdate();

	// If the user pressed start
	if ( SoKeysPressed( SO_KEY_START ) )
	{
		// We stop trying;
		return false;
	}
	else
	{
		// Continue trying;
		return true;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Communication transfer error handler;

	Function that is going to be called by the \a SoMultiPlayer module during a 
	communication transfer error.
*/
// ----------------------------------------------------------------------------
bool MultiPlayerCommunicationFailure( void )
{
	// Error occured;
	s_CommunicationError = true;

	// Stop trying;
	return false;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
