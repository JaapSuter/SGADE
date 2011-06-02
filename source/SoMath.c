// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoMath.c
	\author		Jaap Suter, Mark T. Price
	\date		Sept 22 2001
	\ingroup	SoMath

	See the documentation on SoMath.h for more information.  
*/

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoMath.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//! Internal use only. Used by the Mersenne Twister. Length of random state vector
#define SO_RAND_STATE_VECTOR_LENGTH     (624)                 

//! Internal use only. Used by the Mersenne Twister. A period parameter
#define SO_RAND_PERIOD					(397)                 

//! Internal use only. Used by the Mersenne Twister. A magic constant
#define SO_RAND_MAGIC					(0x9908B0DFU)         

// ----------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------

// Used by the mersenne twister;
#define SO_RAND_HI_BIT(u)       ((u) & 0x80000000U)   //!< \internal Mask all but highest bit of u
#define SO_RAND_LO_BIT(u)       ((u) & 0x00000001U)   //!< \internal Mask all but lowest  bit of u
#define SO_RAND_LO_BITS(u)      ((u) & 0x7FFFFFFFU)   //!< \internal Mask the highest bit of u
#define SO_RAND_MIX_BITS(u, v)  (SO_RAND_HI_BIT(u)|SO_RAND_LO_BITS(v))  //!< \internal Move hi bit of u to hi bit of v	

// ----------------------------------------------------------------------------
// Static variables
// ----------------------------------------------------------------------------

//! Internal use only. Used by the Mersenne Twister.
//! The next random value is computed from here;
static u32  *s_RandNext;          

//! Internal use only. Used by the Mersenne Twister.
//! Can do a *s_RandNext++ this many times before reloading;
static s32  s_RandLeft = -1;      

//! Internal use only. Used by the Mersenne Twister.
//! Random state vector + 1 extra to not violate ANSI C;
static u32  s_RandState[ SO_RAND_STATE_VECTOR_LENGTH + 1 ];  

// ----------------------------------------------------------------------------
// Forward declarations
// ----------------------------------------------------------------------------
u32 SoMathRandReloadMersenneTwister( void );

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Fixed-point multiplication function

	\param a_A		Fixed point number A
	\param a_B		Fixed point number B

	\return The fixed point multiple of \a a_A * \a a_B

  	Safe fixed point multiplication routine.  This routine will return the most
	accurate representation of the multiplied number as long as it fits within the
	limits of the fixed point number range.
*/
// ----------------------------------------------------------------------------
sofixedpoint SoMathFixedMultiply(sofixedpoint a_A, sofixedpoint a_B)
{
	sofixedpoint lalb, lahb, halb, hahb;

	// Strategy: calculate the result in parts & combine.
	
	// First we multiply the high & low portions of A and B against each other
	// (in each of the four possible permutations)
	lalb = (a_A & 0xffff)   * (a_B & 0xffff);		// low a, low b
	lahb = (a_A & 0xffff)   * (a_B >> 16);		// low a, high b
	halb = (a_A >> 16) * (a_B & 0xffff);		// high a, low b
	hahb = (a_A >> 16) * (a_B >> 16);		// high a, high b;

	// Then shift and add the results.
	// Here's a picture of how this goes together in 16.16 format.
	// (for the purposes of this illustration, each instance of variable name
	// represents 8 bits of data)
	//
	//  hahb  hahb  hahb  hahb
	//              lahb  lahb  lahb  lahb
	//              halb  halb  halb  halb
	//                          lalb  lalb  lalb  lalb
	//              ----  ----  ----  ----  <= result bits
	//
	// Note that modifying SO_FIXED_Q will automatically adjust which
	// bits are used in the result -- as long as it is less than or equal
	// to 16.
	//
	return (hahb << (32-SO_FIXED_Q)) + (lahb >> (16-SO_FIXED_Q)) +
		   (halb >> (16-SO_FIXED_Q)) + (lalb >> SO_FIXED_Q);
}

// ----------------------------------------------------------------------------
/*!
	\brief Fixed-point multiplication by fraction function

	\param a_A		Fixed point number A
	\param a_B		Fractional fixed point number B (less than one)

	\return The fixed point multiple of \a a_A * \a a_B

  	Safe fixed point multiplication routine.  This routine multiplies an arbitrary fixed
	point number by a fractional fixed point number.  This routine will only work if the
	SO_FIXED_Q value is less than or equal to 16.  This routine will return the most
	accurate representation of the multiplied number.
*/
// ----------------------------------------------------------------------------
sofixedpoint SoMathFixedMultiplyByFraction(sofixedpoint a_A, sofixedpoint a_B)
{
	sofixedpoint lalb, halb;

	// Strategy: calculate the result in parts & combine.
	
	// First we multiply the high & low portions of A and B against each other
	// (there are only two permutations since B is all low)
	lalb = (a_A & 0xffff) * a_B;		// low a, low b
	halb = (a_A >> 16)    * a_B;		// high a, low b

	// Then shift and add the results.
	// Here's a picture of how this goes together in 16.16 format.
	// (for the purposes of this illustration, each instance of variable name
	// represents 8 bits of data)
	//
	//              halb  halb  halb  halb
	//                          lalb  lalb  lalb  lalb
	//              ----  ----  ----  ----  <= result bits
	//
	// Note that modifying SO_FIXED_Q will automatically adjust which
	// bits are used in the result -- as long as it is less than or equal
	// to 16.
	//
	return (halb >> (16-SO_FIXED_Q)) + (lalb >> SO_FIXED_Q);
}


// ----------------------------------------------------------------------------
/*!
	\brief Squareroot function.

	\param a_FixedValue Some fixedpoint number in the range [0...64K] (+ the fraction ofcourse)
	                    Note that the number is always treated as unsigned.;

	\return The fixed point squareroot of \a a_FixedValue.

	\warning This routine is not very precise, but for my purposes it gets the job done.
			 It might suit your needs too, but just be carefull when using it.

	\todo Replace with another version of that provides as many bits of accuracy as possible
	      (i.e. no ending shift).

  	Squareroot method taken from the internet. No credits were given. If you think this is
	your routine and I stole it, let me know and I'll give you credit.

	This was originally a squareroot function for integer math, but I adapted it to work 
	for fixed point math. I don't know whether this is the fastest way, but it works. I don't
	want to spend more thinking on it anyway, cause you shouldn't be doing squareroots at 
	runtime anyway.
*/
// ----------------------------------------------------------------------------
sofixedpoint SoMathFixedSqrt( sofixedpoint a_FixedValue )
{
	// Some variables from which I don't know what they mean,
	// because the code from the internet was badly documentend.
	// Maybe I could derive it from the below algorithm, but it works
	// so I'd rather spend my time on something usefull;

    u32 i = 0;
	u32 a = 0;
	u32 e = 0;
	u32 r = 0;

	// Calculate the sqrt of the fixed value;
    for ( ; i < (32 >> 1); i++ )
	{
        r			 <<= 2;
        r			  += ((u32)a_FixedValue >> 30);
        a_FixedValue <<= 2;

        a <<= 1;
        e  =  (a<<1) | 1;

        if (r >= e)
        {
			r -= e;
			a++;
        }
	}

	return (a << (SO_FIXED_Q/2));	// the square root takes the middle 16 bits
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Returns a pseudo random number

	\return A 32 bits random number

	Pseudo random number generator based on the Mersenne Twister. It's implementation
	is taken from the internet and changed to adapt my code style. Credits go to 
	Shawn J.Cokus.
*/
// ----------------------------------------------------------------------------
inline u32 SoMathRand( void )
{
	u32 y;

    if( --s_RandLeft < 0 ) return( SoMathRandReloadMersenneTwister() );

    y  = *s_RandNext++;
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9D2C5680U;
    y ^= (y << 15) & 0xEFC60000U;
    
	return(y ^ (y >> 18));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Seeds the random number generator

	\param a_Seed	Any 32 bits number can be used.

	Pseudo random number generator based on the Mersenne Twister. To obtain a good seed, I 
	suggest you take some user-input based number (which is very pseudo-random), for example
	the amount of milliseconds elapsed since the Gameboy Advance was turned on, until the user
	pressed start.
*/
// ----------------------------------------------------------------------------
void SoMathRandSeed( u32 a_Seed )
{
	
	register u32 x = (a_Seed | 1U) & 0xFFFFFFFFU, *s = s_RandState;
    register s32 j;

    for( s_RandLeft = 0, *s++ = x, j = SO_RAND_STATE_VECTOR_LENGTH; 
		 --j; 
		 *s++ = (x*=69069U) & 0xFFFFFFFFU );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief For internal use only. 

	Reloads the Mersenne Twister state vector. Implementation taken from
    the internet. Credits go to Shawn J. Cokus;
*/
// ----------------------------------------------------------------------------
u32 SoMathRandReloadMersenneTwister( void )
{
	// Variables;
    register u32 *p0=s_RandState, *p2=s_RandState+2, *pM=s_RandState+SO_RAND_PERIOD, s0, s1;
    register s32 j;

	// Code;
    if( s_RandLeft < -1 ) SoMathRandSeed( 4357U );

    s_RandLeft = SO_RAND_STATE_VECTOR_LENGTH - 1;
	s_RandNext = s_RandState + 1;

    for( s0 = s_RandState[0], s1 = s_RandState[1], j = SO_RAND_STATE_VECTOR_LENGTH - SO_RAND_PERIOD + 1; --j; s0 = s1, s1 = *p2++ )
	{
        *p0++ = *pM++ ^ (SO_RAND_MIX_BITS(s0, s1) >> 1) ^ (SO_RAND_LO_BIT(s1) ? SO_RAND_MAGIC : 0U);
	}

    for( pM = s_RandState, j = SO_RAND_PERIOD; --j; s0 = s1, s1 = *p2++ )
	{
        *p0++ = *pM++ ^ (SO_RAND_MIX_BITS(s0, s1) >> 1) ^ (SO_RAND_LO_BIT(s1) ? SO_RAND_MAGIC : 0U);
	}

    s1=s_RandState[0], *p0 = *pM ^ (SO_RAND_MIX_BITS(s0, s1) >> 1) ^ (SO_RAND_LO_BIT(s1) ? SO_RAND_MAGIC : 0U);
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9D2C5680U;
    s1 ^= (s1 << 15) & 0xEFC60000U;
    
	return(s1 ^ (s1 >> 18));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
