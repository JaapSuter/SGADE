// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoIntManager.c
	\date		Sep 19 2001
	\ingroup	SoIntManager

	See the \a SoIntManager module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoIntManager.h"

#include "SoMode4Renderer.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

#define SO_REG_INT_ENABLE			 (*((u16*)0x4000200))	//!< \internal Interrupt Enable Register
#define SO_REG_INT_REQUEST			 (*((u16*)0x4000202))	//!< \internal Interrupt Request Register
#define SO_REG_INT_ENABLE_MASTER	 (*((u16*)0x4000208))	//!< \internal Interrupt Master Enable Register

#define SO_INTERRUPT_HANDLER_ADDRESS (*((u32*)0x03007FFC))	//!< \internal Location of address of interrupt handler.

// ----------------------------------------------------------------------------
// Statics
// ----------------------------------------------------------------------------

/*!
	\brief Interrupt handler array

	\internal

	This array contains all interrupt handlers. If an entry is \a NULL there
	is no handler installed for the interrupt type.
*/
SoInterruptHandler g_InterruptHandlers[ SO_NUM_INTERRUPT_TYPES ];

//! \internal To prevent multiple initialization.
static bool s_Initialized = false;

// ----------------------------------------------------------------------------
// Function implementations
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets an interrupt;

	\param	a_InterruptType		Type of the interrupt you want to install the handler for.
								This is one of the \a SO_INTERRUPT_TYPE_ * constants.
	\param	a_IntHandler		The interrupt handler that is to be installed. Can be \a NULL
								if you want to disable an interrupt handler.
*/
// ----------------------------------------------------------------------------
void SoIntManagerSetInterruptHandler( u32 a_InterruptType, SoInterruptHandler a_IntHandler )
{
	// Set it;
	g_InterruptHandlers[ a_InterruptType ] = a_IntHandler;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*! 
	\brief Initializes the interrupt manager.

	First function you should call before you use any other function of the 
	interrupt manager.

	This function will return immediately if it was called before (cause then the 
	manager is already initialized).
*/
// ----------------------------------------------------------------------------
void SoIntManagerInitialize( void )
{
	// Dummy counter;
	u32 i;

	// Return if we are already initialized;
	if ( s_Initialized ) return;
	
	// Reset the interrupt handlers;
	for ( i = 0; i < SO_NUM_INTERRUPT_TYPES; i++ )
	{
		g_InterruptHandlers[ i ] = NULL;
	}
	
	// Disable all interrupts;
	SO_REG_INT_ENABLE_MASTER		= 0;
	SO_REG_INT_ENABLE				= 0;
	SO_REG_INT_REQUEST				= 0;
	
	// Set the interrupt handler;
	SO_INTERRUPT_HANDLER_ADDRESS	= (u32) SoIntManagerInterruptHandler;

	// Enable all the DISP_STAT interrupts, cause these will be masked
	// by the other interrupt enable register anyway;
	SO_REG_DISP_STAT = SO_BIT_3 | SO_BIT_4 | SO_BIT_5;

	// From now on we are initialized;
	s_Initialized = true;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Enables the master-enable setting for the interrupts.
	
	Without the master enabled, no interrupt will function even when you 
	individually enable them.
*/
// ----------------------------------------------------------------------------
void SoIntManagerEnableInterruptMaster(	 void ) 
{ 
	SO_REG_INT_ENABLE_MASTER = SO_BIT_0; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Disables the master-enable setting for the interrupts.

	Disables all interrupts even when they are individually enabled.
*/
// ----------------------------------------------------------------------------
void SoIntManagerDisableInterruptMaster(   void ) 
{ 
	SO_REG_INT_ENABLE_MASTER = 0; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Enables a specific interrupt.

	\param	a_InterruptType		Type of the interrupt you want to enable.
								This is one of the \a SO_INTERRUPT_TYPE_ * constants.

	Note that if there are no handlers installed for this type, or the master enable 
	is disabled, then nothing will happen.
*/
// ----------------------------------------------------------------------------
void SoIntManagerEnableInterrupt( u32 a_InterruptType )
{
	SO_REG_INT_ENABLE |= (1 << a_InterruptType);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Disables a specific interrupt.

	\param	a_InterruptType		Type of the interrupt you want to disable.
								This is one of the \a SO_INTERRUPT_TYPE_ * constants.
*/
// ----------------------------------------------------------------------------
void SoIntManagerDisableInterrupt( u32 a_InterruptType )
{
	SO_REG_INT_ENABLE &= ~(1 << a_InterruptType);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
