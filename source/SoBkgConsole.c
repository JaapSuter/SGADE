// ----------------------------------------------------------------------------
/*! 
Copyright (C) 2002 by the SGADE authors
For conditions of distribution and use, see copyright notice in SoLicense.txt

\file		SoBkgConsole.c
\author		Mark T. Price
\date		Oct 24, 2003
\ingroup	SoBkgConsole

See the \a SoBkgConsole module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoDebug.h"
#include "SoBkg.h"
#include "SoBkgConsole.h"

//! \a internal   Console structure, defines current state of console
typedef struct {
	u16 m_Bkg;			//!< associated background
	u16 m_ScrollPos;	//!< scroll position
} SoBkgConsole;

SoBkgConsole s_Console;   //!< \a internal   Console singleton

// ----------------------------------------------------------------------------
// Function implementations.
// ----------------------------------------------------------------------------

//! \brief	Initializes the SoBkgConsole for use
//!
//! \param a_Bkg	background to use for display
//!
void SoBkgConsoleInit(u16 a_Bkg)
{
	s_Console.m_Bkg  = a_Bkg;
	s_Console.m_ScrollPos = 0;

	SoBkgFill(a_Bkg, 0);
	SoBkgSetOffset(a_Bkg, 0, 0);
}

//! \brief	Writes a string to the console
//!
//! \param a_String	the string to write
//!
void SoBkgConsolePrint(const char *a_String)
{
	u16 oldBottom, newBottom;

	//***** calculate screen change
	oldBottom = (s_Console.m_ScrollPos + SO_SCREEN_HEIGHT - 1) >> 3;
	s_Console.m_ScrollPos += 8;
	newBottom = (s_Console.m_ScrollPos + SO_SCREEN_HEIGHT - 1) >> 3;

	//*****  load up about-to-be-exposed tiles

	int row = (s_Console.m_ScrollPos + SO_SCREEN_HEIGHT - 1) >> 3;
	// clear old row
	SoBkgFillBlock(s_Console.m_Bkg, 0, row&0x1f, 0x1f, 1, 0);

	// insert new row
	SoBkgPrintAt(s_Console.m_Bkg, 0, row&0x1f, a_String);

	//***** update the display offset to show the newly loaded tiles
	SoBkgSetOffset(s_Console.m_Bkg, 0, s_Console.m_ScrollPos & 0xff);
}

//! \brief	Writes a formatted string to the console
//!
//! \param a_Format	the format string
//!
//!  variable-argument printf-style function to output to the Bkg console
//!
void SoBkgConsolePrintf(const char* a_Format, ...)
{
	va_list args;

	va_start(args, a_Format);
	dvsprintf(s_DebugBuffer, a_Format, args);
	SoBkgConsolePrint(s_DebugBuffer);
	va_end(args);
}

#if 0

//!	\brief Draws a string to a background
//!
//! \param a_Index	 Background index: 0, 1, 2, or 3
//!	\param a_String	 The string you want to write.
//!
//! This function causes a text string to be copied to the specified background.
//! Before calling this function, you must first load a font and associate it
//! with the background you are using.
//!
void SoBkgConsolePrint(u16 a_Index, const char *a_String)
{
	const SoVector2 *pCur = SoBkgGetTextCursor(a_Index);
	u16 charsInWord = 0;

	SoBkgSetTextWrapping(a_Index, true);
	SoBkgSetTextWindow(a_Index, 0, 0, 29, 31);

	while(*a_String)
	{
		// count the characters in the word.
		while(
			// The characters that are not part of a word.
			// In ASCII, all escape characters are below 
			// the space character
			*(a_String + charsInWord) > ' ' &&
			*(a_String + charsInWord) <= '~'
			)
		{
			// '-' is included in the word, but is enough for a word-break
			// (note side-effect charsInWord increment takes place after
			// string is dereferenced)
			if(*(a_String + charsInWord++) == '-')
				break;
		}

		// too long to fit on a single line?
		if(charsInWord > 30)
		{
			charsInWord = 30;
			// set size to remaining space
			if(pCur->m_X < 30)
				charsInWord -= pCur->m_X;
		}

		SoBkgPrint(a_Index, a_String);

		a_String += charsInWord;
	}
}

//! \brief	Updates the scrolling display
//!
//! \param a_This	this pointer
//!
//! This function processes the commands in the scrolling credit definition
//! structure.  It should be called once per frame.  Returns true as long as
//! there is more data to be processed.
//!
bool SoBkgConsolePrint(SoBkgCreditScroll *a_This)
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
#endif

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
