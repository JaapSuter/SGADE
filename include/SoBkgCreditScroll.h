// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2003 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgCreditScroll.h
	\author		Mark Price
	\date		Oct 18, 2003
	\ingroup	SoBkgCreditScroll

	See the \a SoBkgCreditScroll module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_BKG_CREDIT_SCROLL_H
#define SO_BKG_CREDIT_SCROLL_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoBkgCreditScroll SoBkgCreditScroll
	\brief	  Functions to support 'scrolling credits' type display

	This special-purpose module provides a simple interface to generate
	a typical scrolling credits type of display normally seen at the end of
	a movie.  It does this using a \a SoBkg (tile mode) display.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoSystem.h"
#include "SoBkg.h"
#include "SoDisplay.h"
#include "SoTileMap.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

#define SO_CREDIT_CODE_TEXT         0x0000  //!< data is the text to display for this line

#define SO_CREDIT_CODE_SPACE        0x0001  //!< data is the number of blank lines to insert
#define SO_CREDIT_CODE_SPEED        0x0002  //!< data is the number of update calls per move
#define SO_CREDIT_CODE_PAUSE        0x0003  //!< data is the number of update calls to wait
#define SO_CREDIT_CODE_FONT         0x0004  //!< data is a new SoBkgFont pointer

#define SO_CREDIT_CODE_ALIGNLEFT    0x4000  //!< align text left
#define SO_CREDIT_CODE_ALIGNRIGHT   0x8000  //!< align text right
#define SO_CREDIT_CODE_ALIGNCENTER  0xc000  //!< align text centered


//#define SO_BKG_CREDIT_MODE_RAW      0x0000  //!< flag - raw mode display
//#define SO_BKG_CREDIT_MODE_DYNAMIC  0x0001  //!< flag - dynamic mode display


// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------

//! \brief	Scrolling Credits line structure
//!
typedef struct {
	u32         m_Code;	//!< One of the SO_CREDIT_CODE_... define values
	const char *m_Data;	//!< Data for command code (usually text to display)
} SoBkgCreditLine;

//! \brief	Scrolling Credits definition structure
//!
//! A single \a SoBkgCreditScroll instance represents all of the data displayed
//! in a scrolling credits display. All the members are private.  Use the methods
//! instead.
//!
typedef struct
{
	u16        m_Mode;			//!< \internal  operation mode
	u16        m_Bkg;			//!< \internal	Background used for display
	u16	       m_ScrollPos;		//!< \internal  current vertical scroll position (in pixels)
	u16        m_UpdSpeed;		//!< \internal	number of ticks per update
	u16        m_FrameCnt;		//!< \internal	number of ticks elapsed since last update
	u16        m_LineCnt;		//!< \internal	total number of entries in m_Lines[]
	u16        m_LineNum;		//!< \internal	current index into m_Lines[]
	u16        m_LineArg;		//!< \internal	effect argument value (used by PAUSE & SPACE)
	const SoBkgCreditLine *m_Lines;	//!< \internal data to be scrolled
} SoBkgCreditScroll;


// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------

//#define SoBkgCreditScrollIsDynamic(a_This)      ((a_This)->m_Mode & SO_BKG_CREDIT_MODE_DYNAMIC)

//! \brief  Retrieves the background index used by the credit scroll
#define SoBkgCreditScrollGetBackground(a_This)  ((const u16)((a_This)->m_Bkg))

void SoBkgCreditScrollInit(SoBkgCreditScroll *a_This, u16 a_Bkg, u16 a_LineCnt, const SoBkgCreditLine *a_Lines);
bool SoBkgCreditScrollUpdate(SoBkgCreditScroll *a_This);
bool SoBkgCreditScrollDone(SoBkgCreditScroll *a_This);

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SO_BKG_CREDIT_SCROLL_H
