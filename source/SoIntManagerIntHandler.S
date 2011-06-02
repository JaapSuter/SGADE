@ --------------------------------------------------------------------------------------
@ 
@	Copyright (C) 2002 by the SGADE authors
@	For conditions of distribution and use, see copyright notice in SoLicense.txt
@
@	\file		SoIntManagerIntHandler.S
@	\author		Jeff Frohwein (as extracted by Mark T. Price)
@	\date		May 24 2002
@	\ingroup	SoIntManager
@
@	See the \a SoIntManager module for more information.  
@
@	\implements	SoIntManagerInterruptHandler
@
@   This code is based heavily on the crt0.s written by Jeff Frohwein (www.devrs.com/gba)
@
@ --------------------------------------------------------------------------------------

@ System register definitions (not yet used)
@	.equ	greg_IE,  0x4000200		@ Interrupt Enable Register (R/W)
@	.equ	greg_IF,  0x4000202		@ Interrupt Request Register (R)
@	.equ	greg_IME, 0x4000208		@ Interrupt Master Enable Register (R/W)

@ --------------------------------------------------------------------------------------
@ Initialize;
@ --------------------------------------------------------------------------------------

	.ARM
	.ALIGN

@ Imports
	.EXTERN     g_InterruptHandlers
	
@ Exports
	.GLOBL  SoIntManagerInterruptHandler


SoIntManagerInterruptHandler:
	mov     r2, #0x4000000           @ REG_BASE
	ldr     r3, [r2,#0x200]!         @ r2 = IE : r3 = IF|IE
	ldrh    r1, [r2, #0x8]           @ r1 = IME
	mrs     r0, spsr
	stmfd   sp!, {r0-r2,lr}          @ {spsr, IME, REG_IE, lr}  // IF|IE

	mov     r0, #1                   @ IME = 1 (To permit multiple interrupts if
									 @ an interrupt occurs)
	strh    r0, [r2, #0x8]
	and     r1, r3, r3, lsr #16      @ r1 = IE & IF
	ldr     r12, =g_InterruptHandlers

	@ The following instructions test each bit of the value
	@ read from the IF register in turn, jumping to 'jump_intr'
	@ when the first non-zero bit is found.  At the same time,
	@ the address of the entry into the interrupt table is
	@ accumulated (each entry is 4 bytes long).
	ands    r0, r1, #1               @ V-blank interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #2               @ H-blank interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #4               @ V-counter interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #8               @ Timer 0 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x10            @ Timer 1 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x20            @ Timer 2 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x40            @ Timer 3 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x80            @ Serial Communication Interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x100           @ DMA 0 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x200           @ DMA 1 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x400           @ DMA 2 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x800           @ DMA 3 interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x1000          @ Key interrupt
	bne     jump_intr

	add     r12,r12, #4
	ands    r0, r1, #0x2000          @ Cart interrupt

@ Not provided since it requires that this routine be loaded into IWRAM
@
@   .ifdef __HandleCartInterrupt
@        strneb  r0, [r2, #0x84 - 0x200]  @ Stop sound if cart removed (REG_SOUNDCNT_X)
@loop:   bne     loop                     @ Infinite loop if cart removed
@   .endif

jump_intr:
	@ Store the interrupt bit back into the IF register
	@ to acknowledge receipt of the interrupt.
	strh    r0, [r2, #2]             @ Clear IF

	@ Switch to system mode (to get a larger stack)
	mrs     r3, cpsr
	bic     r3, r3, #0xdf            @ Enable IRQ & FIQ...
	orr     r3, r3, #0x1f            @   Set CPU mode to System.
	msr     cpsr, r3

	ldr     r0, [r12]                @ Load handler address

	stmfd   sp!, {lr}                @ save registers...
	adr     lr, IntrRet              @   set return address...
	bx      r0                       @   and call handler
IntrRet:
	ldmfd   sp!, {lr}                @ restore registers

	@ Switch back to IRQ mode (so we can correctly return)
	mrs     r3, cpsr
	bic     r3, r3, #0xdf            @ Enable FIQ...
	orr     r3, r3, #0x92            @   Disable IRQ. Set CPU mode to IRQ.
	msr     cpsr, r3

	ldmfd   sp!, {r0-r2,lr}          @ {spsr, IME, REG_IE, lr}  //IF|IE
@	strh    r3,  [r2]                @ set IE
	strh    r1,  [r2, #0x8]          @ restore REG_IME
	msr     spsr, r0                 @ restore spsr
	bx      lr

	.POOL

@ --------------------------------------------------------------------------------------
@ EOF;
@ --------------------------------------------------------------------------------------

