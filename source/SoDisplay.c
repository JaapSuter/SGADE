// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoDisplay.c
	\author		Jaap Suter, Mark T. Price
	\date		April 3, 2003
	\ingroup	SoDisplay

	See the \a SoDisplay module for more information.
*/
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoDisplay.h"
#include "SoEffects.h"	// for SoEffectsSetMode call
#include "SoBkg.h"		// for SoBkgSetPriority call

// ----------------------------------------------------------------------------
// Registers
// ----------------------------------------------------------------------------

//! GBA display mosaic register backup, so we can both read and write;
//! \warning This register needs to be copied into SO_REG_DISP_MOSAIC_W to have any effect.
u16 SO_REG_DISP_MOSAIC_RW = 0;


#define DISPCNT_MODE_MASK 0x7
#define DISPCNT_MODE(n)     ((n)&0x7)
#define DISPCNT_BACKBUFFER      0x0010
#define DISPCNT_H_BLANK_OAM     0x0020
// OAM mapping mode
#define DISPCNT_OBJ_MAP_1D      0x0040
#define DISPCNT_OBJ_MAP_2D      0x0000
// forced blank display
#define DISPCNT_FORCE_BLANK     0x0080
// Background/Sprite enable bits
#define DISPCNT_BG0_ENABLE      0x0100
#define DISPCNT_BG1_ENABLE      0x0200
#define DISPCNT_BG2_ENABLE      0x0400
#define DISPCNT_BG3_ENABLE      0x0800
#define DISPCNT_OBJ_ENABLE      0x1000
// Windowing enable bits
#define DISPCNT_WIN0_ENABLE     0x2000
#define DISPCNT_WIN1_ENABLE     0x4000
#define DISPCNT_WINOBJ_ENABLE   0x8000

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Initializes the display.

	Resets the display registers.
*/
// ----------------------------------------------------------------------------
void SoDisplayInitialize( void )
{
	// Reset the neccesary registers;
	SO_REG_DISP_CNT  = 0;
	SO_REG_DISP_STAT = 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns the current scanline count.

	\return	Current scanline count.
*/
// ----------------------------------------------------------------------------
u32 SoDisplayGetVCounter( void )
{
    return ( SO_REG_SCANLINE_COUNT & SO_8_BITS );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Tests whether we are in VBlank or not.

	\return \a true if we are in VBlank and \a false otherwise.
*/
// ----------------------------------------------------------------------------
bool SoDisplayIsInVBlank( void )
{
    return ( SO_REG_DISP_STAT & SO_BIT_0 );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Tests whether we are in HBlank or not.

	\return \a true if we are in HBlank and \a false otherwise.
*/
// ----------------------------------------------------------------------------
bool SoDisplayIsInHBlank( void )
{
    return ( SO_REG_DISP_STAT & SO_BIT_1 );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Waits until we are at the start of VBlank

	Routine just halts the CPU (except for interrupts) until the display
	reaches the start of VBlank.

	\warning If this function is already in VBlank it will return immediately. 
*/
// ----------------------------------------------------------------------------
void SoDisplayWaitForVBlankStart( void )
{
	while ( !SoDisplayIsInVBlank() );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Waits until we VBlank has ended.

	Routine just halts the CPU (except for interrupts) until the display
	finishes VBlank.

	\warning If this function is already outside of VBlank it will return immediately. 
*/
// ----------------------------------------------------------------------------
void SoDisplayWaitForVBlankEnd( void )
{
    while ( SoDisplayIsInVBlank() );
}
// ----------------------------------------------------------------------------

//!	\brief	Sets the current video display mode (0..6)
//!
//! \param	a_Mode	the mode to be set
//!
//! This routine changes the current display mode.  Currently only supports modes
//! 0 (text BG mode) and 4 (bitmap mode)
//!
void SoDisplaySetMode(u32 a_Mode)
{
	u16 dispcnt;

	// validate argument
	SO_ASSERT(a_Mode == 0 || a_Mode == 4, "Bad or unsupported graphics mode");

	SoEffectsSetMode(SO_EFFECTS_MODE_NONE, SO_EFFECTS_TARGET2_ALL, SO_EFFECTS_TARGET2_ALL);

	switch(a_Mode)
	{
	case 0:
		dispcnt = SO_REG_DISP_CNT;
		dispcnt &= ~(DISPCNT_MODE_MASK | DISPCNT_BG0_ENABLE | DISPCNT_BG1_ENABLE | DISPCNT_BG2_ENABLE | DISPCNT_BG3_ENABLE);
		dispcnt |= DISPCNT_MODE(a_Mode);
		SO_REG_DISP_CNT = dispcnt;
		break;
	case 4:
		dispcnt = SO_REG_DISP_CNT;
		// Clear any previous display mode, reset the backbuffer selection bit, and 
		// enable the LCD screen.
		dispcnt &= ~( DISPCNT_MODE_MASK | DISPCNT_BACKBUFFER | DISPCNT_FORCE_BLANK | DISPCNT_BG0_ENABLE | DISPCNT_BG1_ENABLE | DISPCNT_BG3_ENABLE);
		// Enable background 2 and mode 4;
		dispcnt |= DISPCNT_MODE(4) | DISPCNT_BG2_ENABLE;
		SO_REG_DISP_CNT = dispcnt;
		SoBkgSetPriority(2, 3);
	}
}

//! \brief	Gets the current video display mode
//!
//! This routine retrieves the current video display mode.
//!
u32 SoDisplayGetMode(void)
{
	return SO_REG_DISP_CNT & DISPCNT_MODE_MASK;
}

//! \brief	Enables or disables the display
//!
//! \param	a_Enable
//!
//! This function updates the display's forced blank status.  If it is enabled, the
//! display will be show as all white.
//!
void SoDisplayEnable(u32 a_Enable)
{
	if(a_Enable)
		SO_REG_DISP_CNT = SO_REG_DISP_CNT & ~DISPCNT_FORCE_BLANK;
	else
		SO_REG_DISP_CNT = SO_REG_DISP_CNT | DISPCNT_FORCE_BLANK;
}

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
