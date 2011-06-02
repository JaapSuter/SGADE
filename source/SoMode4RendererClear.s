@ --------------------------------------------------------------------------------------
@ 
@	Copyright (C) 2002 by the SGADE authors
@	For conditions of distribution and use, see copyright notice in SoLicense.txt
@
@	\file		SoMode4RendererClear.s
@	\author		Jaap Suter
@	\date		Jan 16 2002
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
        .GLOBL  SoMode4RendererClear

		.EXTERN s_SoMode4RendererBackBuffer
		
@ --------------------------------------------------------------------------------------
@
@	\brief Clears the backbuffer in mode 4 rendering.
@
@	Uses stmia instructions to quickly clear the entire screen to the zero color.
@
@	\prototype 
@	
@	void SoMode4RendererClear( void );
@
@ --------------------------------------------------------------------------------------
SoMode4RendererClear:

	@ Initialize;

		stmfd	sp!,{r0-r9}		@ Save the registers on the stack;

		@ Load the start addres of the video memory;		
		ldr		r0, =s_SoMode4RendererBackBuffer
		ldr		r0, [r0]
		
		mov		r1, #1200		@ Load the size of the screen in 32 byte blocks;	
																				
		mov		r2, #0			@ Load the registers with 0's;						
		mov		r3, #0			@ \todo there must be a smarter way of doing this;	
		mov		r4, #0																
		mov		r5, #0																
		mov		r6, #0																
		mov		r7, #0																
		mov		r8, #0																
		mov		r9, #0																

	@ Loop that does the clearing;
																					
		clear:
																	
		stmia	r0!, {r2-r9}	@ Perform a 32 byte clear; This advances			
								@ the r0 pointer as well;							
		subs	r1, r1, #1		@ One block done;									
		bne		clear			@ Jump back if we're not done yet;					

	@ Return;

		ldmfd	sp!,{r0-r9}		@ Restore the registers from the stack;
		bx		lr				@ Return;
     

@ --------------------------------------------------------------------------------------

@ --------------------------------------------------------------------------------------
@ EOF;
@ --------------------------------------------------------------------------------------



