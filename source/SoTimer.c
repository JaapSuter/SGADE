// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTimer.c
	\author		Jaap Suter, Eric Rounds, Mark T. Price
	\date		May 23, 2003
	\ingroup	SoTimer
	
	See the \a SoTimer module for more information.	
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoTimer.h"
#include "SoIntManager.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Function forwards
// ----------------------------------------------------------------------------
void SoTimerClockHandler( void );

// ----------------------------------------------------------------------------
// Statics
// ----------------------------------------------------------------------------

// Clock counters;	
static u32  s_NumTotalSeconds;	//!< \internal Total number of seconds elapsed since clock start.
static u32  s_NumSeconds;	//!< \internal seconds portion of time elapsed since clock start.
static u32  s_NumMinutes;	//!< \internal minutes portion of time elapsed since clock start.
static u32  s_NumHours;		//!< \internal hours portion of time elapsed since clock start.
static u32  s_NumDays;		//!< \internal days portion of time elapsed since clock start.

//! \internal Timer ID the clock is currently using. Valid values
//!			  are [0..3] for IRQ's and -1 when the clock is not in use.
//!			  This also means that \a s_ClockTimerID + \a SO_INTERRUPT_TYPE_TIMER_0
//!			  equals the interrupt for the clock (when calling \a SoInterruptManager functions).
static s8	s_ClockTimerID = -1;

/*!
	\brief Timer register overlay struct;

	\internal
	
	This struct is put over the timer registers on the GBA, so we 
	can easily acces different timers.
*/
static struct tag_Timer
{
	volatile u16 m_Data;		//!< \internal Timer interrupt data register;
	volatile u16 m_Ctrl;		//!< \internal Timer interrupt control register;

} *s_Timer = (struct tag_Timer*) (0x4000100);

// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Enables and starts the clock.

	\param	a_TimerID			The timer you want the clock to use. Range in [0..3].

	\warning	Note that something else (direct sound for example) might already 
				be using this timer, so make sure you pick a free one.

	The given timer will be in use until you call \a SoTimerClockDisable. So do not use the 
	timer for anything else in between.
*/
// ----------------------------------------------------------------------------
void SoTimerClockEnable( u32 a_TimerID )
{
	// Assert the input;
	SO_ASSERT( a_TimerID < 4, "Invalid timer IRQ range, valid range is [0..3]" );
	
	// Reset the clock;
	SoTimerClockReset();

	// Set the clock irq;
	s_ClockTimerID = a_TimerID;

	// Initialize the clock timer;
	s_Timer[ s_ClockTimerID ].m_Ctrl = 2 | SO_BIT_6 | SO_BIT_7;	// This will generate an 
	s_Timer[ s_ClockTimerID ].m_Data = 0;						// interrupt every second
	SoIntManagerInitialize();
	SoIntManagerSetInterruptHandler( SO_INTERRUPT_TYPE_TIMER_0 + s_ClockTimerID, SoTimerClockHandler );
	SoIntManagerEnableInterrupt( SO_INTERRUPT_TYPE_TIMER_0 + s_ClockTimerID );
	SoIntManagerEnableInterruptMaster();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Resets the clock.
*/
// ----------------------------------------------------------------------------
void SoTimerClockReset( void )
{
	s_NumTotalSeconds = 0;
	s_NumSeconds = 0;
	s_NumMinutes = 0;
	s_NumHours   = 0;
	s_NumDays    = 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Disables the clock;
*/
// ----------------------------------------------------------------------------
void SoTimerClockDisable( void )
{
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	// Remove the clock interrupt;
	SoIntManagerSetInterruptHandler( s_ClockTimerID + SO_INTERRUPT_TYPE_TIMER_0, NULL );

	// Reset the clock;
	SoTimerClockReset();

	// Disable the clock;
	s_ClockTimerID = -1;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Pauses the clock.
*/
// ----------------------------------------------------------------------------
void SoTimerClockPause(	void )
{
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	// Disable the clock timer interrupt;
	SoIntManagerDisableInterrupt( s_ClockTimerID + SO_INTERRUPT_TYPE_TIMER_0 );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Resumes the clock.
*/
// ----------------------------------------------------------------------------
void SoTimerClockResume(   void )
{
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	// Enable the clock timer interrupt;
	SoIntManagerEnableInterrupt( SO_INTERRUPT_TYPE_TIMER_0 );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the total number of seconds of the clock.

	\return	Total number of seconds of the clock.
*/
// ----------------------------------------------------------------------------
u32  SoTimerClockGetTotalSeconds( void )
{
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	return s_NumTotalSeconds; 
}

// ----------------------------------------------------------------------------
/*!
	\brief Returns the total number of seconds of the clock.

	\return	Total number of 'milliseconds' of the clock (actually number of 1024th seconds).
*/
// ----------------------------------------------------------------------------
u32  SoTimerClockGetTotalMilliseconds( void )
{
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	return (s_NumTotalSeconds << 10) | (s_Timer[ s_ClockTimerID ].m_Data >> 6);
}

// ----------------------------------------------------------------------------
/*!
	\brief Returns the number of seconds of the clock.

	\return	Number of seconds of the clock.
*/
// ----------------------------------------------------------------------------
u32  SoTimerClockGetSeconds( void ) 
{ 
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	return s_NumSeconds; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the number of minutes of the clock.

	\return	Number of minutes of the clock.
*/
// ----------------------------------------------------------------------------
u32  SoTimerClockGetMinutes( void ) 
{ 
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	return s_NumMinutes; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the number of hours of the clock.

	\return	Number of hours of the clock.
*/
// ----------------------------------------------------------------------------
u32  SoTimerClockGetHours(   void ) 
{ 
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	return s_NumHours; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the number of days of the clock.

	\return	Number of days of the clock.
*/
// ----------------------------------------------------------------------------
u32  SoTimerClockGetDays(	 void ) 
{ 
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	return s_NumDays; 
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Sets the clock to the given value.

	\param a_NumDays	Number of days you want to set the clock to.
	\param a_NumHours	Number of hours you want to set the clock to.
	\param a_NumMinutes	Number of minutes you want to set the clock to.
	\param a_NumSeconds	Number of seconds you want to set the clock to.
*/
// ----------------------------------------------------------------------------
void SoTimerClockSet( u32 a_NumDays, u32 a_NumHours,
					  u32 a_NumMinutes, u32 a_NumSeconds )
{
	// Assert the clock is enabled;
	SO_ASSERT( s_ClockTimerID != -1, "Clock is not enabled." );

	s_NumDays = a_NumDays;
	s_NumHours = a_NumHours;
	s_NumMinutes = a_NumMinutes;
	s_NumSeconds = a_NumSeconds;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Clock interrupt handler.

	\internal

	This function is called every clock second, to update the clock values.
*/
// ----------------------------------------------------------------------------
void SoTimerClockHandler( void )
{
	// Increase the time;
	s_NumSeconds++;
	s_NumTotalSeconds++;

	// Check for overflow;
	if ( s_NumSeconds > 59 ) 
	{
		s_NumSeconds -= 60;
		s_NumMinutes++;
		if ( s_NumMinutes > 59 )
		{
			s_NumMinutes -= 60;
			s_NumHours++;
			if ( s_NumHours > 23 )
			{
				s_NumHours -= 24;
				s_NumDays++;
			}
		}
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the timer that is used by the clock, if one is used at all.
	
	\return 0, 1, 2, or 3 if the clock is enabled, -1 if no timer is used by the clock.
*/
// ----------------------------------------------------------------------------
u32 SoTimerClockGetTimerID( void )
{
	return s_ClockTimerID;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Halts the CPU for the given time. Interrupts are still executed.

	\param a_TimerID				Timer you want the wait to use. Range in [0..3].
	\param a_NumMilliSeconds	Number of milliseconds you want to wait.

	\warning Note that something else (the clock, or a direct sound channel for example) 
			 might already be using the timer, so make sure you pick a free one. This is up 
			 to you to take care of.

	Upon returning the timer is free to use again. The timer is only used inside the function.
*/
// ----------------------------------------------------------------------------
void SoTimerWait( u32 a_TimerID, u32 a_NumMilliSeconds )
{
	// Pointer to the timer data;
	volatile u16* data = &(s_Timer[ a_TimerID ].m_Data);

	// Count down;
	while ( a_NumMilliSeconds-- )
	{
		// Reset the timer;
		*data = 0;

		// Start the timer, counting every 3.814 microseconds;
		s_Timer[ a_TimerID ].m_Ctrl = SO_BIT_7 | 1;

		// Wait 1 millisecond;
		while ( (*data) < 262 );	// 1 millisecond / 3.814 microseconds;

		// Stop the timer again;
		s_Timer[ a_TimerID ].m_Ctrl = 0;

	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief This enables the given timer for direct sound purposes.

    \param a_TimerID Timer you want to use. Note that only timer and 0 and 1 are available
				   for direct sound purposes. Do not supply 2 or 3.

	\warning Make sure the timer you supply is free, and not in use by something else (the
			 clock for example). This is up to you to take care of.
*/
// ----------------------------------------------------------------------------
void SoTimerEnableForDirectSound( u32 a_TimerID )
{
	// Assert the input;
	SO_ASSERT( (a_TimerID == 0) || (a_TimerID == 1), "Invalid Direct Sound Timer given." );

	// DEBUG TEST;
	// TODO: fix;
	s_Timer[ a_TimerID ].m_Data = 0xFAE8;
	s_Timer[ a_TimerID ].m_Ctrl = 0x0080;	
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
