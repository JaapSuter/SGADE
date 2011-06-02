// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMath.h
	\author		Jaap Suter, Mark T. Price
	\date		April 3, 2003	
	\ingroup	SoMath
*/
// ----------------------------------------------------------------------------

#ifndef SO_MATH_H
#define SO_MATH_H

#ifdef __cplusplus
	extern "C" {
#endif


// ----------------------------------------------------------------------------
/*! 
	\defgroup SoMath SoMath
	\brief	  All math related stuff.

	Singleton

	This file contains all math related stuff. It has a couple of constants,
	fixed point macros, precalculated sine and cosine tables, a pseudo random
	number generator and some math functions.
    
*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoSystem.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

/*!
	\brief Number of bits reserved for the fraction in fixed point numbers.

	We use a N.Q fixed point format where Q is the 
	number of bits for the fraction. 
	
	Notice that some of the macros below rely heavily (hardcoded) on 
	Q. This means that if you change this number, you will
	have to change these macros as well as some hardcoded stuff 
	in some assembly code.

	If you change this macro, then an assertion is triggered during compilation
	of the \a SoSystem.c file to remind you about this.
*/
#define SO_FIXED_Q					16

//! PI a.k.a. 3.14 etc. in a fixed point format.
#define SO_FIXED_PI					205887

// ----------------------------------------------------------------------------
// Functions implemented in C;
// ----------------------------------------------------------------------------

sofixedpoint SoMathFixedMultiply(sofixedpoint a_A, sofixedpoint a_B);
sofixedpoint SoMathFixedMultiplyByFraction(sofixedpoint a_A, sofixedpoint a_B);
sofixedpoint SoMathFixedSqrt(sofixedpoint a_FixedValue);
void SoMathRandSeed(u32 a_Seed);
u32  SoMathRand(void);


// ----------------------------------------------------------------------------
// Functions implemented in asm;
// Sadly, these need to be documented in here, cause Doxygen can't handle .s
// files very well.
// ----------------------------------------------------------------------------
/*!
	\brief Fast divide and modulus function 

    \param	a_Numerator		Whole numerator.
	\param  a_Denominator	Whole denominator.
	\return					\a a_Numerator divided by \a a_Denominator (whole format).		
	\retval a_Remainder		\a a_Numerator modulus \a a_Denominator.
	
	Divide function that uses an SWI (bios) call instead of the slow software 
	emulated divide. However, note that it's still faster to use the
	\a g_OneOver table also availabe in the \a SoMath module. 
	
	Because sometimes you also need them modulus at the same time, and it's a 
	by-product of the divide, you can use this routine if you need both.

	Note, that in order to use this on an emulator you need a bios that supports 
	SWI 6 calls;
*/
// ----------------------------------------------------------------------------
s32 SoMathDivideAndModulus( s32 a_Numerator, s32 a_Denominator, s32 *a_Remainder );

// ----------------------------------------------------------------------------
/*!
	\brief Fast modulus function 

    \param	a_Numerator		Whole numerator.
	\param  a_Denominator	Whole denominator.
	\return					\a a_Numerator modulus \a a_Denominator.
	
	Modulus function that uses an SWI (bios) call instead of the slow software 
	emulated modulus.

	Note, that in order to use this on an emulator you need a bios that supports 
	SWI 6 calls;
*/
// ----------------------------------------------------------------------------
s32 SoMathModulus( s32 a_Numerator, s32 a_Denominator );

// ----------------------------------------------------------------------------
/*!
	\brief Fast divide function 

    \param	a_Numerator		Whole numerator.
	\param  a_Denominator	Whole denominator.
	\return					\a a_Numerator divided by \a a_Denominator (whole format).		
	
	Divide function that uses an SWI (bios) call instead of the slow software 
	emulated divide. However, note that it's still faster to use the
	\a g_OneOver table also availabe in the \a SoMath module.

	Note, that in order to use this on an emulator you need a bios that supports 
	SWI 6 calls;

*/
// ----------------------------------------------------------------------------
s32 SoMathDivide( s32 a_Numerator, s32 a_Denominator );

// ----------------------------------------------------------------------------
// Macros;
// ----------------------------------------------------------------------------

//! Returns the minimum of a pair of numbers
#define SO_MIN(x,y) ((x) < (y) ? (x) : (y))

//! Returns the maximum of a pair of numbers
#define SO_MAX(x,y) ((x) > (y) ? (x) : (y))

//! Returns the absolute value of a number
#define SO_ABS(x) ((x) < 0 ? -(x) : (x))

//! Evaluates to true when a number is even, false otherwise
#define SO_NUMBER_IS_EVEN( n )		(!((n) & 1))

//! Evaluates to false when a number is even, true otherwise
#define SO_NUMBER_IS_UNEVEN( n )	(  (n) & 1)

//! Converts a whole (integer) number to a fixedpoint number.
#define SO_FIXED_FROM_WHOLE( n )  ( (n) << SO_FIXED_Q)

//! Converts a fixed point number to a whole number. Cuts of the fraction.
#define SO_FIXED_TO_WHOLE( n )	  ( (n) >> SO_FIXED_Q)

//! Converts a whole number to a fixedpoint number.
//! \warning Slow, so please avoid floats
#define SO_FIXED_FROM_FLOAT( n )  ( (n) * (float)(1<<SO_FIXED_Q))

//! Converts a fixed point number to a whole number. Cuts of the fraction.
//! \warning Very slow cause it uses a divide.
#define SO_FIXED_TO_FLOAT( n )	  ( (n) / (float)(1<<SO_FIXED_Q))

//! Leaves only the fraction of a fixed point number. 
#define SO_FIXED_TO_FRACTION( n ) ( (n) & ((1 << SO_FIXED_Q)-1))

//! Makes a fixed point number whole.
#define SO_FIXED_MAKE_WHOLE( n )  ( (n) >>= SO_FIXED_Q)

//! Returns the whole ceiling of the fixed point number.
#define SO_FIXED_CEIL_WHOLE( n ) ( ((n) + ((1<<SO_FIXED_Q)-1)) >> SO_FIXED_Q )

//! Converts our fixed point format to a 24.8 format.
#define SO_FIXED_TO_N_8_FORMAT( n ) ( (n) >> (SO_FIXED_Q - 8) )

//! NOTE: all of the SO_FIXED_MULTIPLY_* macros are very sensitive to input
//!       data.  They can easily overflow, so use with extreme caution.
//!
//! Multiply macro to multiply two relatively small fixed point numbers.
#define SO_FIXED_MULTIPLY_SMALL_SMALL( n, m )	  ( (((m)>>2)   * ((n)>>2)) >> 12 )

//! Multiply macro to multiply a relatively small fixed point number by a relatively big one.
#define	SO_FIXED_MULTIPLY_BIG_SMALL( big, small ) ( (((big)>>6) * ((small)>>2)) >> 8 )

//! Multiply macro to multiply two fixed point numbers
#define	SO_FIXED_MULTIPLY( n, m )				  ( (((n)>>4)   * ((m)>>4))     >> 8 )

//! Divide macro to divide a small fixed number by a big fixed point number
#define SO_FIXED_DIVIDE_SMALL_BIG( n, m )	( (((n)<<8) / (m)) << 8 )

//! Divide macro to divide a fixed point number by another fixed point number.
#define SO_FIXED_DIVIDE( n, m )				( (((n)<<4) / ((m)>>4)) << 8 )

//! One-over macro that uses a repricocal-table. This can only be used when
//! the denominator is a positive number smaller than \a SO_ONE_OVER_N_MAX_N.
//! Be carefull with this one, and only use it if you understand its limitations.
#define SO_FIXED_ONE_OVER_FAST_INACCURATE( n )	(g_OneOver[ (n) >> (SO_FIXED_Q - SO_ONE_OVER_N_INDEX_Q) ] )
//#define SO_FIXED_ONE_OVER_FAST_INACCURATE( n )	( SoMathDivide( (SO_FIXED_FROM_WHOLE( 1 ) << (30 - SO_FIXED_Q)), ((n)>>(SO_FIXED_Q - (30 - SO_FIXED_Q)))))

//! Macro to calculate one-over-N the slow and accurate way.
//! Well, slow. Uses a bios SWI call for faster divides.
#define SO_FIXED_ONE_OVER_SLOW_ACCURATE( n )	( SoMathDivide( (SO_FIXED_FROM_WHOLE( 1 ) << (30 - SO_FIXED_Q)), ((n)>>(SO_FIXED_Q - (30 - SO_FIXED_Q)))))

/*! 
	\brief Returns the fixed point sine of an angle. A full circle is 256 degrees.

	\param n Any 32 bits value.

	Because we are using a 256-degree system we can do an easy and fast
	modulus by bitwise-anding with 0xFF. Therefore we can use 
	any angle. Negative, positive and beyond the 256 range.

	Some examples:

	\code
		SO_SINE( 256 ) == SO_SINE( 0 ) == SO_SINE( -256 ) == SO_SINE( 1024 )

		SO_COSINE( 64 ) == SO_COSINE( -192 )
	\endcode
*/
#define SO_SINE( n )				( g_Sine[ (n) & 0xFF ] )

//! Returns the fixed point cosine of an angle. A full circle 
//! is 256 degrees. See \a SO_SINE for more information.
#define SO_COSINE( n )				( g_Cosine[ (n) & 0xFF ] )


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
