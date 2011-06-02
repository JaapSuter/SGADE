// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoTimer.h
	\author		Jaap Suter, Erik Rounds, Mark T. Price
	\date		May 23 2003
	\ingroup	SoTimer
	
	See the \a SoTimer module for more information.	
*/
// ----------------------------------------------------------------------------

#ifndef SO_TIMER_H
#define SO_TIMER_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoTimer SoTimer 
	\brief	  All timer related stuff.

	Singleton

	At the moment this module contains a clock and a wait-routine. It also
	contains some functions to setup timers for using them together with the
	\a SoSound module.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

void SoTimerClockEnable(	 u32 a_TimerID );
u32  SoTimerClockGetTimerID( void );
void SoTimerClockDisable(	 void );
void SoTimerClockReset(		 void );
void SoTimerClockPause(		 void );
void SoTimerClockResume(	 void );
u32  SoTimerClockGetTotalSeconds( void );
u32  SoTimerClockGetTotalMilliseconds( void );
u32  SoTimerClockGetSeconds( void );
u32  SoTimerClockGetMinutes( void );
u32  SoTimerClockGetHours(   void );
u32  SoTimerClockGetDays(	 void );
void SoTimerClockSet(		 u32 a_NumDays, 
							 u32 a_NumHours,
							 u32 a_NumMinutes, 
							 u32 a_NumSeconds );


void SoTimerWait( u32 a_Timer, u32 a_NumMilliSeconds );

void SoTimerEnableForDirectSound( u32 a_TimerID );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
