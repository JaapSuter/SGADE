@ --------------------------------------------------------------------------------------
@
@	Copyright (C) 2002 by the SGADE authors
@	For conditions of distribution and use, see copyright notice in SoLicense.txt
@ 
@	\file		SoMathDivide.s
@	\author		Jaap Suter
@	\date		Jan 10 2002
@	\ingroup	SoMath
@
@	See the \a SoMath module for more information.  
@
@	\implements		SoMathDivideAndModulus
@					SoMathDivide
@					SoMathModulus
@
@ --------------------------------------------------------------------------------------


@ --------------------------------------------------------------------------------------
@ Initialize;
@ --------------------------------------------------------------------------------------

        .ARM
        .ALIGN
        .GLOBL  SoMathDivide
		.GLOBL  SoMathDivideAndModulus
		.GLOBL  SoMathModulus
		
@ --------------------------------------------------------------------------------------
@
@	\brief Fast divide and modulus function 
@
@   \param	a_Numerator,	Whole numerator.
@	\param  a_Denominator,	Whole denominator.
@	\return					\a a_Numerator divided by \a a_Denominator (whole format).		
@	\retval a_Remainder		\a a_Numerator modulus \a a_Denominator.
@	
@	Divide function that uses an SWI (bios) call instead of the slow software 
@	emulated divide. However, note that it's still faster to use the \a g_OneOver table
@	also availabe in the \a SoMath module. 
@	
@	Because sometimes you also need them modulus at the same time, and it's a 
@	by-product of the divide, you can use this routine if you need both.
@
@	Note, that in order to use this on an emulator you need a bios that supports 
@	SWI 6 calls;
@
@	\prototype 
@	
@	s32 SoMathDivideAndModulus( s32 a_Numerator, s32 a_Denominator, s32 *a_Remainder );
@
@ --------------------------------------------------------------------------------------
SoMathDivideAndModulus:
        
		swi   0x60000	@ Call the SWI instruction;
		str   r1, [r2]	@ Load the remainder in the given address;
		bx	  lr		@ Return;		

@ --------------------------------------------------------------------------------------

@ --------------------------------------------------------------------------------------
@
@	\brief Fast modulus function 
@
@   \param	a_Numerator,	Whole numerator.
@	\param  a_Denominator,	Whole denominator.
@	\return					\a a_Numerator modulus \a a_Denominator.
@	
@	Modulus function that uses an SWI (bios) call instead of the slow software 
@	emulated modulus.
@
@	Note, that in order to use this on an emulator you need a bios that supports 
@	SWI 0x60000 calls;
@
@	\prototype
@
@	s32 SoMathModulus( s32 a_Numerator, s32 a_Denominator );
@ --------------------------------------------------------------------------------------
SoMathModulus:

		swi   0x60000	@ Call the SWI instruction;
		mov	  r0, r1	@ Set the return value;
		bx	  lr		@ Return;



@ --------------------------------------------------------------------------------------
@
@	\brief Fast divide function 
@
@   \param	a_Numerator,	Whole numerator.
@	\param  a_Denominator,	Whole denominator.
@	\return					\a a_Numerator divided by \a a_Denominator (whole format).		
@	
@	Divide function that uses an SWI (bios) call instead of the slow software 
@	emulated divide. However, note that it's still faster to use the \a g_OneOver table
@	also availabe in the \a SoMath module.
@
@	Note, that in order to use this on an emulator you need a bios that supports 
@	SWI 6 calls;
@
@	\prototype
@
@	s32 SoMathDivide( s32 a_Numerator, s32 a_Denominator );
@
@ --------------------------------------------------------------------------------------

SoMathDivide:

		swi   0x60000	@ Call the SWI instruction;
		bx	  lr		@ Return;



@ --------------------------------------------------------------------------------------
@ EOF;
@ --------------------------------------------------------------------------------------



