// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSampleEffectCredits.c
	\author		Mark T. Price
	\date		Oct 18, 2003
	\ingroup	SoSampleEffects

	See the \a SoSampleEffects module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Sample application includes;
#include "SampleEffects.h"
#include "SoBkgFont.h"
#include "SoBkgCreditScroll.h"


// ----------------------------------------------------------------------------
// Upper / Lower case swapping font map.  Use of this font causes prints all
// upper case characters as lower case and all lower case characters as upper
// case.

u16 upperToLowerMap[] = {
//       !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
//   0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?   @
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,

//   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
//   p   q   r   s   t   u   v   w   x   y   z 
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,

//	 [   \   ]   ^   _   `
    59, 60, 61, 62, 63, 64,

//   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
//   P   Q   R   S   T   U   V   W   X   Y   Z 
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,

//	 {   |   }   ~
    91, 92, 93, 94
};
SoBkgFont upperToLowerFont = {
	' ', ('~' - ' ') + 1, 0, &upperToLowerMap[0]
};

// ----------------------------------------------------------------------------
// Credits data

SoBkgCreditLine credits[] = 
{
	// run at one line every other frame
	{ SO_CREDIT_CODE_SPEED, (const char *)2 },

	// start with everything off screen
	{ SO_CREDIT_CODE_PAUSE, (const char *)0 },

	// the actual credit text starts here
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "The SGADE is brought to you" },

	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "by the efforts of:" },
	{ SO_CREDIT_CODE_SPACE, (const char *)1},
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Jaap Suter" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Mark Price" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Gabriele Scibilia" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Willem Kokke" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Matt Hattem" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Raleigh Cross" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Eric Rounds" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "Arthur Kim Daniel" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER,	"Benjamin Hale" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER,	"Mark Schmidt" },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER,	"Jeff Frohwein" },
	{ SO_CREDIT_CODE_SPACE, (const char *)4},
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER,	"Thank You for downloading and " },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER,	"trying the SGADE" },
	{ SO_CREDIT_CODE_SPACE, (const char *)11 },

	// switch to use upper/lower case conversion font (defined above)
	// This will cause the 'www.sgade.com' text to appear in upper case
	{ SO_CREDIT_CODE_FONT,  (const char *)&upperToLowerFont },
	{ SO_CREDIT_CODE_TEXT | SO_CREDIT_CODE_ALIGNCENTER, "www.sgade.com" },

	// scroll last line 1/2 way up screen
	{ SO_CREDIT_CODE_SPACE, (const char *)10 },

	// wait for a couple of seconds with "www.sgade.com" in center of screen
	{ SO_CREDIT_CODE_PAUSE, (const char *)120 },
};

// ----------------------------------------------------------------------------
/*!
*/
// ----------------------------------------------------------------------------
void SoSampleEffectCredits( void )
{

	// Go to mode 0;
	SoDisplaySetMode(0);
	SoPaletteSetGreyScale(SO_SCREEN_PALETTE, true );
	SoBkgSetup(3, SO_BKG_PRIORITY_0|SO_BKG_CHARBASE(3)|SO_BKG_MOSAIC_DISABLE|SO_BKG_CLRMODE_256|SO_BKG_SCRBASE(31)|SO_BKG_TEXTSIZE_256x256);
	SoTileSetBkgLoad(SoBkgGetDefaultFontTileSet(), 3, 0);
	SoBkgSetFont(3, NULL);	// use default (no) text mapping
	SoBkgEnable(3, true);

	SoBkgCreditScroll cred;
	SoBkgCreditScrollInit(&cred, 3, sizeof(credits)/sizeof(credits[0]), &credits[0]);

	do {
		SoDisplayWaitForVBlankEnd();
		SoDisplayWaitForVBlankStart();
 		SoKeysUpdate();
	} while(!SoKeysPressed( SO_KEY_START ) && SoBkgCreditScrollUpdate(&cred));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF;
// ----------------------------------------------------------------------------
