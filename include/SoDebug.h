// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoDebug.h
	\author		Matt Hattem, Jaap Suter, Mark T. Price, Erik Rounds, Gabriele Scibilia
	\date		July 1, 2003
	\ingroup	SoDebug

	See the \a SoDebug module for more information. 
*/
// ----------------------------------------------------------------------------

#ifndef SO_DEBUG_H
#define SO_DEBUG_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoDebug SoDebug
	\brief	  All debug related functionality

	Singleton

	This module contains all stuff related to debugging, like debug-output, 
	assertions, etc.

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------
#include "SoSystem.h"
#include <stdarg.h>

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

/*! 
	\brief Undefine this (comment it) if you are going to do a release build;
    
	When this is not defined then assertions are optimized away, as well as
	some other stuff that is done. It's all magic :) hehe...

	However, you can always be sure that if something works in debug mode, it
	WILL work in release mode too.
*/
#define SO_DEBUG

/*!
	\brief		Define that controls what system the debugprint should use.
	
*/	
// Only one or zero of these macros may be defined at any time;
// #define SO_DEBUG_RUNNING_ON_MAPPY
// #define SO_DEBUG_RUNNING_ON_MBV2
// #define SO_DEBUG_RUNNING_ON_BATGBA


// ----------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------

/*!
	\brief Assertion macro.

	\param a_Assertion  Expression that results in a boolean value (true,
	                    false, 0 or non zero).
	\param a_Message	Message that is displayed when the assertion fails.    
	
	This macro can be used for assertions. When you are running a debug build
	and the given assertion fails, it halts the program and displays the
	assertion, the message, the line number, and the name of the file in which
	the assertion is located.

	In release builds (when SO_DEBUG is not defined) these assertions are
	removed by the preprocessor.
*/
#ifdef SO_DEBUG
	#define	SO_ASSERT( a_Assertion, a_Message )		\
			SoDebugAssert( (a_Assertion),			\
							(a_Message),			\
							#a_Assertion,			\
							__FILE__,				\
							__LINE__ )				
#else
	#define SO_ASSERT( a_Assertion, a_Message )
#endif

/*!
	\brief Prints debug messages;

	\param a_String Message you want to display;

	This macro routs the given string to a console depending 
	on the \a SO_DEBUG_RUNNING_ON_* defines.

	In release builds (when SO_DEBUG is not defined) these debug prints are
	removed by the preprocessor.
*/
#ifdef SO_DEBUG
	#define	SO_DEBUG_PRINT( a_String )	SoDebugPrint( a_String )

	//! \brief prints integer value
	#define SO_DEBUG_PRINT_INT_VALUE(a_Var) SoDebugPrintIntValue(#a_Var, a_Var, 10);
	//! \brief prints character value
	#define SO_DEBUG_PRINT_CHAR_VALUE(a_Var) SoDebugPrintCharValue(#a_Var, a_Var);
	//! \brief prints string value
	#define SO_DEBUG_PRINT_STRING_VALUE(a_Var) SoDebugPrintStringValue(#a_Var, a_Var);
#else
	#define	SO_DEBUG_PRINT( a_String )	
	#define SO_DEBUG_PRINT_INT_VALUE(a_Var)
	#define SO_DEBUG_PRINT_CHAR_VALUE(a_Var)
	#define SO_DEBUG_PRINT_STRING_VALUE(a_Var)
#endif

//! \brief	generates a breakpoint (BATGBA only)
//!
//! \param	a_BreakNum	ID number of the breakpoint
//!
#ifdef SO_DEBUG_RUNNING_ON_BATGBA
#define SO_DEBUG_BREAK(a_BreakNum)			asm ("swi " #a_BreakNum " << 16 + 0x90");
#endif


/*!
	\brief	Compile-time assertion.
	
	\param a_Assertion  Expression that results in a boolean value (true,
	                    false, 0 or non zero).
	\param a_Message    Message that is displayed when the assertion fails.
	                    This must be an identifier not a string, due to the
	                    nature of this assertion (compile-time). So if you 
	                    want to say "Value 64 not allowed", then just use
	                    Value_64_not_allowed instead. And if you don't
	                    understand this, then you shouldn't be using compile
	                    time assertions.

	This is to check certain preconditions during the compile phase, like
	values of constants and their relations. If the assertion fails, you will
	get an error from the compiler, telling you where the assertion occured.
	This error might look a bit weird, something like size of array .... is
	negative. But the array identifier will given an indication of what
	happened. You can then go to the line of the error for a better
	explanation.
*/
#define SO_COMPILE_TIME_ASSERT( a_Assertion, a_Message ) typedef char COMPILE_TIME_ASSERTION_FAILED_##a_Message[(a_Assertion) != 0 ? 1 : -1] 

/*!
	\brief	Deprecated function declaration macro

	\param a_FunctionName	Name of the function that has been deprecated
	\param a_Message		Message to explain what to do instead (if anything)

	This macro gives a more friendly notification to a user of the SGADE
	library if they are attempting to use a function that is no longer
	supported.  It should make it much easier to track down the error than
	getting a link error, or the old method of providing a stub for the removed
	function that just asserts.
*/
#define SO_DEPRECATED_FUNCTION(a_FunctionName, a_Message) SO_COMPILE_TIME_ASSERT(0, a_FunctionName##_has_been_deprecated_##a_Message);


// ----------------------------------------------------------------------------
// Private functions;
// ----------------------------------------------------------------------------

//! \internal  Temporary buffer for debug print output
char s_DebugBuffer[ 256 ];

// helper functions
char *dstrcpy(u8 *strdst, const u8 *strsrc);
char *dstrcpynum(u8 *strdst, s32 value, int base);
u16   dstrlen(const u8 *str);
u16   dstrnlen(const u8 *str, u16 count);
char *ditoa(s32 num, char *buf, int len, int base);
int dsprintf(char *buf, const char *fmt, ...);
int dvsprintf(char *buf, const char *fmt, va_list args);

void SoDebugPrintIntValue(const char *a_Name, s32 a_Value, int base);

void SoDebugAssert(	bool a_Assertion, 
					char* a_Message, 
					char* a_Expression, 
					char* a_File, 
					u32 a_Line );

void SoDebugPrintf(const char* a_Format, ...);
void SoDebugPrint(const char* a_Message);

void SoDebugPrintIntValue(const char *a_Name, s32 a_Value, int base);
void SoDebugPrintCharValue(const char *a_Name, char a_cValue);
void SoDebugPrintStringValue(const char *a_Name, const char* a_szValue);

void SoDebugDumpWordBuffer(const char *a_szDescription, u16 *a_pBuffer, u16 a_iCount);

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------


//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
