// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoBkgCreditScroll.c
	\author		Mark T. Price
	\date		Oct 18, 2003
	\ingroup	SoBkgCreditScroll

	See the \a SoBkgCreditScroll module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoDebug.h"
#include "SoBkgCreditScroll.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------
#define SO_CREDIT_CODE_COMMANDMASK   0x0fff   //!< \internal
#define SO_CREDIT_CODE_ALIGNMASK     0xf000   //!< \internal

// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

//! \internal process a credits definition line 
//!
//! returns true if a new line was written
//!
static bool handleLine(SoBkgCreditScroll *a_This)
{
	int row = (a_This->m_ScrollPos + SO_SCREEN_HEIGHT - 1) >> 3;

	switch(a_This->m_Lines[a_This->m_LineNum].m_Code & SO_CREDIT_CODE_COMMANDMASK)
	{
	case SO_CREDIT_CODE_TEXT:
		{
			int ipos = dstrlen(a_This->m_Lines[a_This->m_LineNum].m_Data);
			SoBkgFillBlock(a_This->m_Bkg, 0, row&0x1f, 0x1f, 1, 0);
			switch(a_This->m_Lines[a_This->m_LineNum].m_Code & SO_CREDIT_CODE_ALIGNMASK)
			{
			case SO_CREDIT_CODE_ALIGNLEFT:   ipos = 0;                               break;
			case SO_CREDIT_CODE_ALIGNRIGHT:  ipos = (SO_SCREEN_WIDTH/8 - ipos);      break;
			case SO_CREDIT_CODE_ALIGNCENTER: ipos = (SO_SCREEN_WIDTH/8 - ipos) / 2;  break;
			}
			// draw new line
			if(ipos < 0)
				ipos = 0;
			SoBkgPrintAt(a_This->m_Bkg, ipos, row&0x1f, a_This->m_Lines[a_This->m_LineNum].m_Data);
			++a_This->m_LineNum;
			return true;
		}

	case SO_CREDIT_CODE_SPACE:
		if(!a_This->m_LineArg)
			a_This->m_LineArg = (u16)(u32)a_This->m_Lines[a_This->m_LineNum].m_Data;
			SoBkgFillBlock(a_This->m_Bkg, 0, row&0x1f, 0x1f, 1, 0);
		if(!(--a_This->m_LineArg))
		{
			++a_This->m_LineNum;
		}
		return true;


	case SO_CREDIT_CODE_FONT:
		SoBkgSetFont(a_This->m_Bkg, ((const SoBkgFont*)(a_This->m_Lines[a_This->m_LineNum].m_Data)));
		++a_This->m_LineNum;
		break;

	case SO_CREDIT_CODE_SPEED:
		a_This->m_UpdSpeed = (u16)(u32)a_This->m_Lines[a_This->m_LineNum].m_Data;
		++a_This->m_LineNum;
		break;

//	case SO_CREDIT_CODE_PAUSE:
	default:
		break;
	}

	return false;
}

//! \brief	Initializes the SoBkgCreditScroll for use
//!
//! \param a_This	this pointer
//! \param a_Bkg	background to use for display
//! \param a_LineCnt number of entries in a_Lines[]
//! \param a_Lines	command & text definitions for scrolling display
//!
void SoBkgCreditScrollInit(SoBkgCreditScroll *a_This, u16 a_Bkg, u16 a_LineCnt, const SoBkgCreditLine *a_Lines)
{
	u16 row;

	a_This->m_Bkg     = a_Bkg;
	a_This->m_LineCnt = a_LineCnt;
	a_This->m_Lines   = a_Lines;

	a_This->m_LineNum   = 0;
	a_This->m_ScrollPos = 0;
	a_This->m_LineArg   = 0;
	a_This->m_UpdSpeed  = 1;
	a_This->m_FrameCnt  = 0;

	SoBkgFill(a_This->m_Bkg, 0);

	// pre-load the screen
	row = 0;
	while(a_This->m_LineNum < a_LineCnt && row < (SO_SCREEN_HEIGHT-1)>>3)
	{
		// pause automatically stops copy
		if(a_This->m_Lines[a_This->m_LineNum].m_Code == SO_CREDIT_CODE_PAUSE)
			break;

		if(handleLine(a_This))
		{
			++row;
			a_This->m_ScrollPos += 8;
		}
	}
	SoBkgSetOffset(a_This->m_Bkg, 0, a_This->m_ScrollPos & 0xff);
}

//! \brief	Updates the scrolling display
//!
//! \param a_This	this pointer
//!
//! This function processes the commands in the scrolling credit definition
//! structure.  It should be called once per frame.  Returns true as long as
//! there is more data to be processed.
//!
bool SoBkgCreditScrollUpdate(SoBkgCreditScroll *a_This)
{
	u16 oldBottom, newBottom;

	if(SoBkgCreditScrollDone(a_This))
		return false;

	// handle speed
	if(++a_This->m_FrameCnt < a_This->m_UpdSpeed)
		return true;
	a_This->m_FrameCnt = 0;

	// handle pauses
	if(a_This->m_Lines[a_This->m_LineNum].m_Code == SO_CREDIT_CODE_PAUSE)
	{
		if(!a_This->m_LineArg)
		{
			if(!(a_This->m_LineArg = (u16)(u32)a_This->m_Lines[a_This->m_LineNum].m_Data))
				++a_This->m_LineNum;	// handle 0-count pause
		}
		else if(!(--a_This->m_LineArg))
			++a_This->m_LineNum;
		return true;
	}

	// calculate screen change
	oldBottom = (a_This->m_ScrollPos + SO_SCREEN_HEIGHT - 1) >> 3;
	++a_This->m_ScrollPos;
	newBottom = (a_This->m_ScrollPos + SO_SCREEN_HEIGHT - 1) >> 3;

	// load up about-to-be-exposed tiles
	if(newBottom > oldBottom)
	{
		handleLine(a_This);
	}

	// update the display offset to show the newly loaded tiles
	SoBkgSetOffset(a_This->m_Bkg, 0, a_This->m_ScrollPos & 0xff);

	return true;
}

//! \brief	Checks to see if the scrolling display is done
//!
//! \param a_This	this pointer
//!
//! This function returns true if the scrolling display defined by the
//! passed pointer has finished.
//!
bool SoBkgCreditScrollDone(SoBkgCreditScroll *a_This)
{
	if(a_This->m_LineNum >= a_This->m_LineCnt)
		return true;
	return false;
}


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
