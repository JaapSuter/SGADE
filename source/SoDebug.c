// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoDebug.c
	\author		Matt Hattem, Jaap Suter, Mark T. Price, Erik Rounds
	\date		June 28, 2003
	\ingroup	SoDebug

	See the \a SoDebug module for more information. 
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------
#include "SoDebug.h"
#include "SoMode4Renderer.h"
#include "SoPalette.h"
#include "SoSpriteManager.h"
#include "SoWindow.h"
#include "SoBkg.h"
#include "SoEffects.h"

#define SO_TILE_MODE_ASSERT

// ----------------------------------------------------------------------------
// Include MBV2 debug stuff;
// ----------------------------------------------------------------------------
#ifdef SO_DEBUG_RUNNING_ON_MBV2	

	//! These typedefs and registere need to be defined 
	// in order to use the MBV2 stuff;
	typedef     volatile unsigned char           vu8;
	typedef     volatile unsigned short int      vu16;
	typedef     volatile unsigned int            vu32;
	typedef     volatile unsigned long long int  vu64;

	#define REG_BASE        0x4000000
	#define REG_SIOCNT      (REG_BASE + 0x128)  // Serial Communication Control
	#define REG_SIODATA8    (REG_BASE + 0x12a)  // 8bit Serial Communication Data
	#define REG_RCNT        (REG_BASE + 0x134)  // General Input/Output Control

	// Contains the print functions for MBV2
	#include "mbv2lib.c" 

#endif


// ----------------------------------------------------------------------------
// Implementation;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*! 
	\brief Called by the SO_ASSERT macro in debug builds.

	\internal

	\param a_Assertion	The boolean expression that you want to assert.
	\param a_Message	Message describing what happened when the assertion fails.
	\param a_Expression Stringized version of the a_Assertion parameter. So we can print the assertion.
	\param a_File		Name of the file in which the assertion is located.
	\param a_Line		Line number at which the assertion appears.

	\a SoAssertImplementation is called by the SO_ASSERT macro when the application is 
	running a debug build. If the given assertion fails, it halts the program and
	displays the assertion, the message, the line number, and the name of the file in which
	the assertion is located.

*/
// ----------------------------------------------------------------------------
void SoDebugAssert( bool a_Assertion, 
					char* a_Message, char* a_Expression, 
					char* a_File, u32 a_Line )
{
	// Variables;
	char* fileName;

	// When the assertion succeeded we can return immediately;
	if ( a_Assertion ) return;

	// Turn both palettes to a funky color;
	SoPaletteSetGreyScale( SO_SCREEN_PALETTE, true );

#ifdef SO_TILE_MODE_ASSERT
	SoPaletteSetColor(SO_SCREEN_PALETTE, 1, SoColor(15,0,0));
	SoPaletteSetColor(SO_SCREEN_PALETTE, 2, SoColor(0,0,15));
#endif

	// Disable any stuff that might mess with our message;
	SoBkgSetMosaicEnable( 2, false );
	SoSpriteManagerDisableSprites();
	SoWindowSetWindow0Enable( false );
	SoWindowSetWindow1Enable( false );

#ifndef SO_TILE_MODE_ASSERT

	// Enable mode 4;
	SoMode4RendererEnable();

	// Clear;
	SoMode4RendererClear();
#else
	SoDisplaySetMode(0);

	// Setup background 0 for solid color overlay
	SoBkgSetup(0, SO_BKG_PRIORITY_0|SO_BKG_CHARBASE(2)|SO_BKG_MOSAIC_DISABLE|SO_BKG_CLRMODE_256|SO_BKG_SCRBASE(30)|SO_BKG_TEXTSIZE_256x256);
	SoBkgEnable(0, true);

	// setup background 3 for text 
	SoBkgSetup(3, SO_BKG_PRIORITY_3|SO_BKG_CHARBASE(3)|SO_BKG_MOSAIC_DISABLE|SO_BKG_CLRMODE_256|SO_BKG_SCRBASE(31)|SO_BKG_TEXTSIZE_256x256);
//	SoBkgFontLoad(SoBkgFontGetDefaultFont(), 3);
	SoTileSetBkgLoad(SoBkgGetDefaultFontTileSet(), 3, 0);
	SoBkgSetFont(3, NULL);
	SoBkgEnable(3, true);

	// set solid color overlay tiles
	SoBkgLoadSolidColorTile(0, 1, 1);
	SoBkgLoadSolidColorTile(0, 2, 2);

	// set alpha blending
	SoEffectsSetMode(SO_EFFECTS_MODE_BLEND, SO_EFFECTS_TARGET1_BG0, SO_EFFECTS_TARGET2_BG3);
	SoEffectsSetBlend(8, 8);

#endif

	// Strip the path of the filename;
	fileName = a_File;
	while ( *fileName++     != '\0' )
		;
	while ( (*(--fileName-1) != '/') || (fileName == a_File) )
		;

#ifndef SO_TILE_MODE_ASSERT

	// Plot the text;
	SoMode4RendererDrawString( 10, 230,  10, "Assertion failed:", SoFontGetDefaultFontImage() );
	SoMode4RendererDrawString( 20, 230,  20, a_Expression,		  SoFontGetDefaultFontImage() );

	SoMode4RendererDrawString( 10, 230,  60, "File: ",			 SoFontGetDefaultFontImage() );
	SoMode4RendererDrawString( 20, 230,  70, fileName,		     SoFontGetDefaultFontImage() );
	SoMode4RendererDrawString( 10, 230,  80, "Line: ",			 SoFontGetDefaultFontImage() );
	SoMode4RendererDrawNumber( 20, 230,  90, a_Line,			 SoFontGetDefaultFontImage() );		
	
	SoMode4RendererDrawString( 10, 230, 100, "Message:",		 SoFontGetDefaultFontImage() );
	SoMode4RendererDrawString( 20, 230, 110, a_Message,			 SoFontGetDefaultFontImage() );

	// Flip;
	SoMode4RendererFlip();
#else
	SoBkgSetTextWindow(3, 2, 0, 28, 19);
	SoBkgSetTextWrapping(3, true);

	SoBkgPrintAt(3, 0, 0, "Assertion failed:");

	SoBkgPrintAt(3, 2, 1, a_Expression);

	const SoVector2 *pCursor = SoBkgGetTextCursor(3);
	SoBkgFillBlock(0, 2, 1,  27, pCursor->m_Y, 1);	// fill red block

	int row = pCursor->m_Y + 2;
	SoBkgSetTextWrapping(3, false);
	SoBkgPrintAt(3, 0, row, "File:");
	SoBkgPrintAt(3, 6, row, fileName);

	++row;
	SoBkgPrintAt(3, 0, row, "Line:");
	char line[16];
	dstrcpynum(&line[0], a_Line, 10);
	SoBkgPrintAt(3, 6, row, line);

	++row;
	++row;
	SoBkgPrintAt(3, 0, row, "Message:");

	++row;
	SoBkgSetTextWrapping(3, true);
	SoBkgPrintAt(3, 2, row, a_Message);

	SoBkgFillBlock(0, 2, row, 27, pCursor->m_Y+1-row, 2);	// fill blue block

#endif
	// Go to an infinite loop;
	for ( ;; );
}
// ----------------------------------------------------------------------------

//!	\brief	debug helper: convert integer to string
//!
//!	\param num	the number to be converted
//!	\param buf	where the result of the conversion will be stored
//!	\param len	number of bytes in destination buffer
//!	\param base	number base for conversion, must be between 2 and 16
//!
char *ditoa(s32 num, char *buf, int len, int base)
{
	bool bNeg = false;
	char *digits = "0123456789ABCDEF";
	char *fchar;

	buf[--len] = '\0';

	// first, fill in the number
	if(num < 0)
	{
		bNeg = true;
		num = -num;
	}
	else if(!num)
		buf[--len] = digits[0];
	while(num && len > 0)
	{
		buf[--len] = digits[num%base];
		num /= base;
	}
	// put in the sign (if needed)
	if(bNeg && len)
		buf[--len] = '-';

	fchar = &buf[len];

	// and pad with blanks
	while(len > 0)
		buf[--len] = ' ';

	return fchar;
}
//! \brief	debug helper: copy string
//!
//!	\param strdst	target string
//! \param strsrc	source string
//!
//! \return pointer to end of target string
//!
//! Multiple calls to \a dstrcpy and \dstrcpynum can be chained passing the
//! result of one call into the next.  The appending effect this produces
//! can be used to create a "poor-man's printf".
//!
char *dstrcpy(u8 *strdst, const u8 *strsrc)
{
	while(*strsrc)
		*strdst++ = *strsrc++;
	*strdst = '\0';
	return strdst;
}
//! \brief	debug helper: string length
//!
//! \param str	string to check
//!
//! \return length of str (number of characters before first nul byte)
//!
u16 dstrlen(const u8 *str)
{
	u16 len = 0;
	while(*str)
	{
		++len;
		++str;
	}
	return len;
}
//! \brief	debug helper: copy number to string
//!
//! \param strdst	target string
//! \param value	number to be copied
//! \param base 	number base for conversion, should be 10 or 16
//!
//! \return	pointer to end of target string
//!
char *dstrcpynum(u8 *strdst, s32 value, int base)
{
	char numbuf[11];
	char *sp;

	if(base == 16)
		strdst = dstrcpy(strdst, "0x");

	ditoa(value, &numbuf[0], 11, base);
	for(sp = &numbuf[0]; *sp == ' '; ++sp)
		;
	return dstrcpy(strdst, sp);
}

// ----------------------------------------------------------------------------
/*!
	\brief This prints a debug message depending on the running environment;

	\param a_Message	Message you want to print;

	The actual \a SO_DEBUG_RUNNING_ON_* define will decide where the 
	message is routed to.

	\warning In order to be able to print stuff on MBV2 you have to supply the include
			 path to where "mbv2lib.c" is located on your harddrive. You can change this
			 in the SGADE makefile;
*/
// ----------------------------------------------------------------------------
void SoDebugPrint(const char* a_Message )
{
	// Print on MBV2 if enabled;
	#ifdef SO_DEBUG_RUNNING_ON_MBV2
		dprintf( a_Message );
	#endif 

	// Print on Mappy if enabled;
	#ifdef SO_DEBUG_RUNNING_ON_MAPPY

		asm volatile("								\n\
						mov r2, %0					\n\
						ldr r0, =0xc0ded00d			\n\
						mov r1, #0					\n\
						and r0, r0, r0				\n\
		  " :
		  /* No output */ :
		  "r" (a_Message) :
		  "r0", "r1", "r2");

	#endif

	#ifdef SO_DEBUG_RUNNING_ON_BATGBA
		#define BATGBA_PRINT_BUFFER ((volatile u8*)(0x10000000))

		// Print on BatGBA if enabled
		int i;
		for(i=0; *a_Message && i < 1023; ++i)
			BATGBA_PRINT_BUFFER[i] = *a_Message++;
		BATGBA_PRINT_BUFFER[i] = '\0';
		asm volatile ("swi 0x50");

	#endif
}

//! \brief	debug helper: print 'watch' for a number
//!
//! \param a_Name	the name of the value to be printed
//! \param a_Value	the value to be printed
//! \param base		number base for value, should be 10 or 16
//!
//! This routine generates a printout containing the name of the
//! value, an equal sign and the value itself.  For example, calling
//! this routine with the arguments ("FrameCount", 798, 10) would result
//! in a printed line containing "FrameCount = 798".
//!
void SoDebugPrintIntValue(const char *a_Name, s32 a_Value, int base)
{
	u8 debugstr[80], *buf=&debugstr[0];

	buf = dstrcpy(buf, a_Name);
	buf = dstrcpy(buf, " = ");
	buf = dstrcpynum(buf, a_Value, base);

	SoDebugPrint(&debugstr[0]);
}

//! \brief  debug helper: print 'watch' for a character
//!
//! \param a_Name    the name of the value to be printed
//! \param a_cValue  the value to be printed
//!
//! This routine generates a printout containing the name of the
//! value, an equal sign and the value itself.  For example, calling
//! this routine with the arguments ("Initial", 'M') would result in
//! a printed line containing "Initial = M".
//!
void SoDebugPrintCharValue(const char *a_Name, char a_cValue)
{
	char debugstr[80], letter[2], *buf=&debugstr[0];
	letter[0] = a_cValue;
	letter[1] = '\0';

	buf = dstrcpy(buf, a_Name);
	buf = dstrcpy(buf, " = ");
	buf = dstrcpy(buf, letter);

	SoDebugPrint(&debugstr[0]);
}

//! \brief  debug helper: print 'watch' for a string
//!
//! \param a_Name     the name of the value to be printed
//! \param a_szValue  the value to be printed
//!
//! This routine generates a printout containing the name of the
//! value, an equal sign and the value itself.  For example, calling
//! this routine with the arguments ("Player", "Henry") would result in
//! a printed line containing "Player = "Henry"".
//!
void SoDebugPrintStringValue(const char *a_Name, const char* a_szValue)
{
	char debugstr[160], *buf=&debugstr[0];

	buf = dstrcpy(buf, a_Name);
	buf = dstrcpy(buf, " = \"");
	buf = dstrcpy(buf, a_szValue);
	buf = dstrcpy(buf, "\"");

	SoDebugPrint(&debugstr[0]);
}

//! \brief  debug helper: print a 'watch' for a binary block
//!
//! \param a_szDescription  description of the binary block
//! \param a_pBuffer        pointer to the binary block
//! \param a_iCount         number of words in the block
//!
//! This routine generates a multi-line printout containing the description
//! followed by the byte count in parenthesis and one or more lines of hex
//! numbers representing the contents of the block.  For example, when called
//! with the arguments "("Player Info", { 0, 5, 8, 10, 7 }, 5) would result
//! in the following output:
//!   Player Info (5 entries)
//!   0000, 0005, 0008, 000A, 0007
//!
void SoDebugDumpWordBuffer(const char *a_szDescription, u16 *a_pBuffer, u16 a_iCount)
{
	char buf[80], *dp;
	int ii, jj;

	dp = &buf[0];
	dp = dstrcpy(dp, a_szDescription);
	dp = dstrcpy(dp, " (");
	dp = dstrcpynum(dp, a_iCount, 10);
	dp = dstrcpy(dp, " entries)");
	SoDebugPrint(&buf[0]);

	for(ii = 0, jj = 0; ii < a_iCount; ++ii)
	{
		if(!jj)
		{
			dp = &buf[0];
			dp = dstrcpy(dp, "  ");
		}

		dp = dstrcpynum(dp, a_pBuffer[ii], 16);
		dp = dstrcpy(dp, ", ");

		if(++jj >= 8)
		{
			SoDebugPrint(&buf[0]);
			jj = 0;
		}
	}
	if(jj)
		SoDebugPrint(&buf[0]);
}

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
