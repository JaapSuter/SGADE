@ --------------------------------------------------------------------------------------
@
@	Copyright (C) 2002 by the SGADE authors
@	For conditions of distribution and use, see copyright notice in SoLicense.txt
@
@	\file		SoMode4PolygonRasterizerSolidTriangle.S
@	\author		Jaap Suter
@	\date		September 5 2001
@	\ingroup	SoMode4PolygonRasterizer
@
@	See the \a SoMode4PolygonRasterizer module for more information.
@
@	\implements		SoMode4PolygonRasterizerDrawSolidTriangle
@
@ --------------------------------------------------------------------------------------

@ --------------------------------------------------------------------------------------
@ Initialize;
@ --------------------------------------------------------------------------------------

        .ARM
        .ALIGN
        .GLOBL  SoMode4PolygonRasterizerDrawSolidTriangle
		
@ --------------------------------------------------------------------------------------
@ Externals;
@ --------------------------------------------------------------------------------------
		
		.EXTERN	g_OneOver								@ One over N table, to avoid divisions;
		.EXTERN	g_SoMode4PolygonRasterizerBuffer		@ Pointer to the current backbuffer;

@ --------------------------------------------------------------------------------------
@ 
@ Documentation at declaration in header file.
@
@ --------------------------------------------------------------------------------------

SoMode4PolygonRasterizerDrawSolidTriangle:
        
		@ Store the registers we crush on the stack;
		
			stmfd	sp!,{r4-r11, r14}

		@ ------------------------------------------------------------------------@
		@					Register usage at this moment						  @	
		@ ------------------------------------------------------------------------@
		@ r0 | r1 | r2 | r3 | r4 | r5 | r6 | r7 | r8 | r9 | r10 | r11 | r12 | r14 @
		@  x |  x |  x |    |    |    |    |    |    |    |     |     |  x  |  x  @
		@ ------------------------------------------------------------------------@

		@ Shift and bitwise-or the paletteindex cause we are going to 
		@ plot two pixels at a time. Putting it in r12;

			orr		r12, r1, r1, lsl#8		@ a_PaletteIndex | (a_PaletteIndex << 8)

		@ We'll be using the top halfword of r12 as two booleans. The 24th bit indicating whether the middle vertex is
		@ on the left or on the right (0 means it's on the right), the 25th bit indicating what part 
		@ of the triangle we're drawing (1 means the bottom half).

		
		@ At this moment only r0 contains the starting address of the
		@ triangle array of SoVector2 objects.
		@ We are going to sort the three vertices in increasing Y order;
		@ At the end of this block, register 0, 1, and 2 will have pointers to 
		@ the vertices in sorted order, and the first bit of r11 will be set according
		@ to where the middle vertex is;

			add		r1, r0, #8				@ Put the addres of a_Triangle[ 1 ] into r1, one SoVector2
			add		r2, r0, #16				@ is 8 bytes; Do the same with a_Triangle[ 2 ] into r2;
											
			ldr		r3, [r0, #4]			@ Load Y values and
			ldr		r4, [r1, #4]			@ compare them;
			cmps	r4, r3					

			movmi   r3, r1					@ If the result is negative we have to swap 'm.
			movmi   r1, r0					@ We use register 3 as a temporary swap register;
			movmi   r0, r3					@ Also swap the 
			eormi	r12, r12, #0x1000000	@ middle vertex bool;

			ldr		r3, [r1, #4]			
			ldr		r4, [r2, #4]			
			cmps	r4, r3					

			movmi   r3, r1					@ Swap again;
			movmi   r1, r2				
			movmi   r2, r3					
			eormi	r12, r12, #0x1000000

			ldr		r3, [r0, #4]
			ldr		r4, [r1, #4]
			cmps	r4, r3

			movmi   r3,  r1					@ Swap again;
			movmi   r1,  r0				
			movmi   r0,  r3	
			eormi	r12, r12, #0x1000000


		@ ------------------------------------------------------------------------@
		@					Register usage at this moment						  @	
		@ ------------------------------------------------------------------------@
		@ r0 | r1 | r2 | r3 | r4 | r5 | r6 | r7 | r8 | r9 | r10 | r11 | r12 | r14 @
		@  x |  x |  x |    |    |    |    |    |    |    |     |     |  x  |  x  @
		@ ------------------------------------------------------------------------@
		

		@ Load all X's and Y's.

			ldr		r3, [r0, #4]			@ bottomOfTriangle.m_Y;
			ldr		r4, [r1, #4]			@ middleOfTriangle.m_Y;
			ldr		r5, [r2, #4]			@    topOfTriangle.m_Y;

			ldr		r0, [r0, #0]			@ bottomOfTriangle.m_X;
			ldr		r1, [r1, #0]			@ middleOfTriangle.m_X;
			ldr		r2, [r2, #0]			@    topOfTriangle.m_X;

		@ Calculate the integer top delta Y, into r10;

			ldr		r10, =0xFFFF			@ Temporarily load the ceil value;

			add		r6, r4, r10				@ Do a ...
			mov		r6, r6, lsr#16			@ ceiling
			add		r7, r3, r10				@ And another one...
			subs	r10, r6, r7, lsr#16		@ combined with the subtract, also sets the zero flag;

		@ Check for degenerate triangle (uses r8 to hold ceil value)

			cmp		r10, #0
			bne		calcTangents

			ldr		r8, =0xFFFF				@ Temporarily load the ceil value;
			add     r6, r5, r8
			mov		r6, r6, lsr#16
			add		r7, r3, r8
			subs	r8, r6, r6, lsr#16

			cmp		r8, #0
			bne		calcTangents
					
			ldmnefd	sp!,{r4-r11, r14}		@ Restore the registers we threw on the stack before;
			bxne	lr						@ Return;


		@ ------------------------------------------------------------------------@
		@					Register usage at this moment						  @	
		@ ------------------------------------------------------------------------@
		@ r0 | r1 | r2 | r3 | r4 | r5 | r6 | r7 | r8 | r9 | r10 | r11 | r12 | r14 @
		@  x |  x |  x |  x |  x |  x |    |    |    |    |  x  |     |  x  |  x  @
		@ ------------------------------------------------------------------------@
			
		@ Calculate tangents
	calcTangents:

			sub		r6, r5, r3				@ Calculate the fixed point delta whole Y;
			mov		r6, r6, lsr#12			@ Now this can be used as an index in the one-over table;
											
			ldr		r9, =g_OneOver			@ Load the address of the one over table;
			ldr		r6, [r9, r6, lsl#2]		@ Load one-over whole Y;
											
			sub		r7, r2, r0				@ Calculate the fixed point delta whole X;
											
			smull	r14, r6, r7, r6			@ Calculate the longest edge tangent X, (r14 is bogus register)
			mov		r6, r6, lsl#16			@ Finish off the ...
			add		r6, r6, r14, lsr#16		@ fixed point multiply;			
											
			beq		tangentsCalculated		@ If the zero flag is set, there is no top half, so don't calculate the short-side tangent;
											
			sub		r7, r4, r3				@ Calculate the fixed point delta top Y;
			mov		r7, r7, lsr#12			@ Now this can be used as an index in the one-over table;
											
			ldr		r7, [r9, r7, lsl#2]		@ Load one-over whole Y;
											
			sub		r8, r1, r0				@ Calculate the fixed point delta top X;
											
			smull	r14, r7, r8, r7			@ Calculate the longest edge tangent X, (r14 is bogus register)				
			mov		r7, r7, lsl#16			@ Finish off the ...
			add		r7, r7, r14, lsr#16		@ fixed point multiply;			
		
		tangentsCalculated:

			ands	r11, r12, #0x1000000	@ Is the middle vertex on the right (r11 is a bogus register);
			movne	r8, r6					@ Swap the tangents, if it's on the left;
			movne	r6, r7
			movne	r7, r8;						
		
		@ ------------------------------------------------------------------------@
		@					Register usage at this moment						  @				
		@ ------------------------------------------------------------------------@
		@ r0 | r1 | r2 | r3 | r4 | r5 | r6 | r7 | r8 | r9 | r10 | r11 | r12 | r14 @
		@  x |  x |  x |  x |  x |  x |  x | x  |    |    |   x |     |  x  |     @
		@ ------------------------------------------------------------------------@

			ldr		r14, =0xFFFF								@ Temporarily load the ceil value;
			add		r8, r3, r14									@ Ceil the top Y
			bic		r8, r3, r14									@ value, keeping it fixed;

			mov		r14, r8, lsr#8								@ Multiply it with 240 (240 = 256 - 16), converting to 
			sub		r14, r14, r8, lsr#12						@ integer in the process;

			ldr		r9, =g_SoMode4PolygonRasterizerBuffer		@ Load the address of the address of the buffer;
			ldr		r9, [r9]									@ Load the adress of the buffer;
			add		r14, r9, r14	 							@ Add the screen address;
		
			sub		r3, r8, r3									@ r3 now contains the Y shift value;
			
			mov		r3, r0										@ r0 and r3 now contain the starting X value for left and right edge;
			
			@ Subpixel correct them;

			@ TODO;


			@ Maybe there is no top triangle half;
			cmps r10, #0
			beq  triangleHalfDone			@ Then we simply skip it;
	
		@ We are now ready to begin drawing a triangle half;

		drawTriangleHalf:

		@ ------------------------------------------------------------------------@
		@					Register setup at this moment						  @	
		@ ------------------------------------------------------------------------@
		@ r0 | r1 | r2 | r3 | r4 | r5 | r6 | r7 | r8 | r9 | r10 | r11 | r12 | r14 @
		@  x |  x |  x |  x |  x |  x |  x |  x |    |    |   x |     |  x  |  x  @
		@ ------------------------------------------------------------------------@
		@ 																		  @
		@ r14 = Start of screen-scanline, at Y = top of triangle;				  @
		@																		  @	  		
		@ r12 = Doubled palette index in lower halfword, two bools in higher half @
		@																		  @
		@																		  @
		@ r6 = Fixed tangent X of the left side of the triangle;				  @
		@ r7 = Fixed tangent X of the right side of the triangle;				  @
		@																		  @
		@ r0 = Fixed value of the left scanline start;							  @
		@ r3 = Fixed value of the right scanline end;							  @
		@																		  @
		@ r10 = Whole vertical size of the top half of the triangle;			  @
		@																		  @
		@ r1 = triangleMiddle.m_X;												  @
		@ r2 = triangleBottom.m_X;												  @	  	
		@ r4 = triangleMiddle.m_Y;												  @
		@ r5 = triangleBottom.m_Y;												  @	
		@																		  @	
		@ ------------------------------------------------------------------------@

		triangleHalfLoop:


			ldr		r11, =0xFFFF			@ Temporarily load the ceil value;

			add		r8, r0, r11				@ Ceil the left fixed X...
			mov		r8, r8, lsr#16			@ to an integer X;
			
			add		r9, r3, r11				@ Ceil the right X, to calculate the scanline length;
					
			rsbs	r9, r8, r9, lsr#16		@ Whole scanline length = whole rightX - whole 
											@ leftX and sets the zero flag as well;

			beq		endOfScanline			@ If the scanline has length zero, we skip this line;
			
			add		r8, r8, r14				@ Calculate the starting address of the scanline;
												

			@ If the starting address ends with a set bit, it's on a 
			@ uneven byte boundary, and we need to take care of the first pixel;
			@ This is because we can only write two pixels at a time (u16's);
				
					ands	r11, r8, #1				@ And with 0x00000001, this sets 
													@ the zero flag;

					beq		endOfTrickyFirstPixel	@ If set, we don't have to do the tricky
													@ first pixel;
					
					bic		r8, r8, #1				@ Clear the last bit, setting it to a 16 bit boundary;
					ldrh	r11, [r8]				@ Read the value at the current u16;
					bic		r11, r11, #0xFF00		@ Mask out the second pixel;
					orr		r11, r11, r12, lsl#8	@ Set the new pixel;
					strh	r11, [r8], #2			@ Write back, and increase pointer to next pixel;
					subs	r9, r9, #1				@ Decrease the scanline length by one;

					beq		endOfScanline			@ If there are no pixels left, this scanline is
													@ done;

				endOfTrickyFirstPixel:
			
			@ Divide the scanline length by two, cause we will plot
			@ two pixels at a time; This also sets the carry if the last
			@ pixel is on an even boundary, which means the last pixel is 
			@ a special case;

					movs	r9, r9, lsr#1		
																
			@ Plot the scanline, we do this by jumping into a list of
			@ of 120 two-pixel draw instructions
					
					rsb		r9, r9, #119		@ Calculate the index in the draw list, because
												@ the PC is incremented every instruction anyway
												@ We substract from 119 instead of 120;
					
					add		pc, pc, r9, lsl#2	@ Jump into the list of draw instructions;
				
					@ What follows is a list of 120 two-pixel plots;
					
					.REPT 120
	
						strh	r12, [r8], #2
						
					.ENDR
																		
			@ If the carry is not set, the last pixel was not a special case
			@ so we can skip drawing the last tricky pixel;

					bcc		endOfScanline		 @ If carry clear, skip to the end;

					ldrh	r11, [r8]			 @ Read the value at the current u16;
					bic		r11, r11,#0xFF		 @ Mask out the first pixel;
					orr		r11, r11, r12, lsr#8 @ Set the new pixel;
					strh	r11, [r8]			 @ Write back;
				
					endOfScanline:

			@ Add the tangents;
				
					add r0, r0, r6
					add r3, r3, r7

		@ Loop around;

			add		r14,  r14, #240			@ Advance the scanline pointer;
			
			subs	r10,  r10, #1			@ One scanline done;			
			bne		triangleHalfLoop		@ Jump back if we're not done yet;

		triangleHalfDone:

		@ Are we done with the triangle (flag in r12 set), or do we still need 
		@ to do the bottom half of the triangle;

			ands	r11, r12, #0x2000000	@ r11 is bogus register;
			ldmnefd	sp!,{r4-r11, r14}		@ Restore the registers we threw on the stack before;
			bxne	lr						@ Return;

		@ We are going to do the bottom triangle now;

			orr		r12, r12, #0x2000000		
		
		@ Calculate the integer bot delta Y, into r10;

			ldr		r10, =0xFFFF			@ Temporarily load the ceil value;

			add		r8, r5, r10				@ Do a ...
			mov		r8, r8, lsr#16			@ ceiling
			add		r9, r4, r10				@ And another one...
			subs	r10, r8, r9, lsr#16		@ combined with the subtract;

		@ Maybe we're done already;

			ldmeqfd	sp!,{r4-r11, r14}		@ Restore the registers we threw on the stack before;
			bxeq	lr						@ Return;

		@ Setup the tangent and start X for the bottom half;

			ands	r11, r12, #0x1000000		@ Is the middle vertex on the right (r12 is a bogus register);
			beq		reSetupRightHalf

		reSetupLeftHalf:
		
			sub		r6, r5, r4			@ Calculate the fixed point delta bottom Y;
			mov		r6, r6, lsr#12		@ Now this can be used as an index in the one-over table;
		
			ldr		r9, =g_OneOver		@ Load the address of the one over table;
			ldr		r6, [r9, r6, lsl#2]	@ Load one-over whole Y;

			sub		r8, r2, r1			@ Calculate the fixed point delta bottom X;
			
			smull	r11, r6, r8, r6		@ Calculate the bottom edge tangent X, (r11 is bogus register)
			mov		r6, r6, lsl#16		@ Finish off the ...
			add		r6, r6, r11, lsr#16	@ fixed point multiply;			

			mov		r0, r1				@ Reinitialize the left X;
			
			b		drawTriangleHalf	@ We're done with the bottom half setup, so start drawing again;	

		reSetupRightHalf:

			sub		r7, r5, r4			@ Calculate the fixed point delta bottom Y;
			mov		r7, r7, lsr#12		@ Now this can be used as an index in the one-over table;
		
			ldr		r9, =g_OneOver		@ Load the address of the one over table;
			ldr		r7, [r9, r7, lsl#2]	@ Load one-over whole Y;

			sub		r8, r2, r1			@ Calculate the fixed point delta bottom X;
			
			smull	r11, r7, r8, r7		@ Calculate the bottom edge tangent X, (r11 is bogus register)
			mov		r7, r7, lsl#16		@ Finish off the ...
			add		r7, r7, r11, lsr#16	@ fixed point multiply;			

			mov		r3, r1				@ Reinitialize the right X;
			
			b		drawTriangleHalf	@ We're done with the bottom half setup, so start drawing again;


			
			

		

@ --------------------------------------------------------------------------------------

@ --------------------------------------------------------------------------------------
@ EOF
@ --------------------------------------------------------------------------------------
