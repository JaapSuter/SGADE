// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoWindow.c
	\author		Gabriele Scibilia
	\date		February 3 2002
	\ingroup	SoWindow

	See the \a SoWindow module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoWindow.h"
#include "SoDisplay.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! \internal Pointer to the first mode 4 buffer
#define SO_REG_WIN_IN			(*(volatile u16*)0x04000048)

//! \internal Pointer to the first mode 4 buffer
#define SO_REG_WIN_OUT			(*(volatile u16*)0x0400004A)

//! \internal Left-upper (high-order bits) and right-lower x-coord (low-order) of window 0
#define SO_REG_WIN0_H			(*(volatile u16*)0x04000040)

//! \internal Left-upper (high-order bits) and right-lower x-coord (low-order) of window 1
#define SO_REG_WIN1_H			(*(volatile u16*)0x04000042)

//! \internal Left-upper (high-order bits) and right-lower y-coord (low-order) of window 0
#define SO_REG_WIN0_V			(*(volatile u16*)0x04000044)

//! \internal Left-upper (high-order bits) and right-lower y-coord (low-order) of window 1
#define SO_REG_WIN1_V			(*(volatile u16*)0x04000046)


// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Changes the position of window 0 setting its upper-left and lower-right
			coordinates.

	\param	a_UpperLeftX	Upper-left X coordinate.
	\param	a_UpperLeftY	Upper-left Y coordinate.
	\param	a_LowerRightX	Lower-right X coordinate.
	\param	a_LowerRightY	Lower-right Y coordinate.
*/
// ----------------------------------------------------------------------------
void SoWindow0SetPosition( u16 a_UpperLeftX, u16 a_UpperLeftY,
						   u16 a_LowerRightX, u16 a_LowerRightY )
{
	SO_REG_WIN0_H = ( a_UpperLeftX << 8 ) | ( a_LowerRightX & 0xFF );
	SO_REG_WIN0_V = ( a_UpperLeftY << 8 ) | ( a_LowerRightY & 0xFF );
}

// ----------------------------------------------------------------------------
/*!
	\brief	Changes the position of window 1 setting its upper-left and lower-right
			coordinates.

	\param	a_UpperLeftX	Upper-left X coordinate.
	\param	a_UpperLeftY	Upper-left Y coordinate.
	\param	a_LowerRightX	Lower-right X coordinate.
	\param	a_LowerRightY	Lower-right Y coordinate.
*/
// ----------------------------------------------------------------------------
void SoWindow1SetPosition( u16 a_UpperLeftX, u16 a_UpperLeftY,
						   u16 a_LowerRightX, u16 a_LowerRightY )
{
	SO_REG_WIN1_H = ( a_UpperLeftX << 8 ) | ( a_LowerRightX & 0xFF );
	SO_REG_WIN1_V = ( a_UpperLeftY << 8 ) | ( a_LowerRightY & 0xFF );
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG0 on through the window 0.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG0InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_0;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_0;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG1 on through the window 0.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG1InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_1;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_1;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG2 on through the window 0.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG2InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_2;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_2;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG3 on through the window 0.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG3InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_3;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_3;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the OBJ on through the window 0.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0OBJInsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_4;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_4;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Enables effects through the window 0.

	\param	a_Enable	True enables the effects through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0InsideEffectsEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_5;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_5;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG0 on through the window 1.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG0InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_8;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_8;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG1 on through the window 1.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG1InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_9;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_9;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG2 on through the window 1.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG2InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_10;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_10;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG3 on through the window 1.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG3InsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_11;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_11;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the OBJ on through the window 1.

	\param	a_Enable	True if you want the BG displayed through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1OBJInsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_12;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_12;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Enables effects through the window 1.

	\param	a_Enable	True enables the effects through the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1InsideEffectsEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_IN |= SO_BIT_13;
	}
	else
	{
		SO_REG_WIN_IN &= ~SO_BIT_13;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG0 on around the window 0.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG0OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_0;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_0;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG1 on around the window 0.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG1OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_1;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_1;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG2 on around the window 0.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG2OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_2;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_2;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG3 on around the window 0.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0BG3OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_3;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_3;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the OBJ on around the window 0.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0OBJOutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_4;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_4;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Enables effects around the window 0.

	\param	a_Enable	True enables the effects around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow0OutsideEffectsEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_5;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_5;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG0 on around the window 1.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG0OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_8;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_8;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG1 on around the window 1.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG1OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_9;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_9;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG2 on around the window 1.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG2OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_10;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_10;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the BG3 on around the window 1.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1BG3OutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_11;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_11;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Makes the OBJ on around the window 1.

	\param	a_Enable	True if you want the BG displayed around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1OBJOutsideEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_12;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_12;
	}
}

// ----------------------------------------------------------------------------
/*!
	\brief	Enables effects around the window 1.

	\param	a_Enable	True enables the effects around the window.
*/
// ----------------------------------------------------------------------------
void SoWindow1OutsideEffectsEnable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_WIN_OUT |= SO_BIT_13;
	}
	else
	{
		SO_REG_WIN_OUT &= ~SO_BIT_13;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief This function simply turns the window 0 on and off.

	\param	a_Enable	True turns on the window.
*/
// ----------------------------------------------------------------------------
void SoWindowSetWindow0Enable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_DISP_CNT |= SO_BIT_13;
	}
	else
	{
		SO_REG_DISP_CNT &= ~SO_BIT_13;
	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief This function simply turns the window 1 on and off.

	\param	a_Enable	True turns on the window.
*/
// ----------------------------------------------------------------------------
void SoWindowSetWindow1Enable( bool a_Enable )
{
	if ( a_Enable )
	{
		SO_REG_DISP_CNT |= SO_BIT_14;
	}
	else
	{
		SO_REG_DISP_CNT &= ~SO_BIT_14;
	}
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
