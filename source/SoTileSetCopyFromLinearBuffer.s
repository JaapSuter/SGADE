@ --------------------------------------------------------------------------------------
@ Title:    SpriteCopy
@ File:     sprite_copy.text.iwram.s
@ Author:   Willem Kokke (Gabriele Scibilia)
@ Created:  March 20 2002 (June 29 2003)
@
@ Info:     This file contains the assembly implementation 
@           of a function to copy a linear buffer to sprite format, which is
@           organised in a linear array of 8*8 blocks
@
@           This function is located in iwram
@
@           For optimal performance, make sure the source buffer is in iwram
@           If your destination buffer is in vram, make sure to only use this in vblank,
@           since the accestimes to vram are undefined while refreshing the display
@
@           The function Willem wrote takes ~ 2% cpu time for a 64*64 sprite, Gabriele
@           unrolled the loop and now it is even faster. The latest implementation
@           takes about 14% for a 240*160 fullscreen (while the oldest was ~ 18%)


@ --------------------------------------------------------------------------------------
@ Initialize;
@ --------------------------------------------------------------------------------------

        .ARM
        .ALIGN
        .GLOBL  SoTileSetCopyFromLinearBuffer

@ --------------------------------------------------------------------------------------
@ Externals;
@ --------------------------------------------------------------------------------------


@ --------------------------------------------------------------------------------------
@ SpriteCopy
@
@ Prototype: 
@
@ __attribute__ (( long_call )) void SpriteCopy( u32* source, u32* dest, u32 width, u32 height );
@
@ Parameters:
@
@ source:   r0 = the start of the linear buffer in iwram
@ dest:     r1 = the start of the sprite in vram
@ width:    r2 = the width of the iwram buffer in pixels
@ height:   r3 = the height of the iwram buffer in pixels
@ --------------------------------------------------------------------------------------

SoTileSetCopyFromLinearBuffer:

        @ Store the registers we crush on the stack;

            stmfd   sp!,{r0-r12,r14}

        @ calculate the number of 8*8 blocks across the width and height
        @ r4 indicates how many pixels to copy per line
        @ r3 indicates how many lines of pixels to copy

            mov     r4, r2


        @ Copy a 8*8 block from the linear buffer to vram
        @ r14 is the temporary source pointer


        Copy8x8Block:

            mov     r14,r0              @ set the start position for the new block
            add     r0, r14, #8         @ save the start position for the next block

            ldmia   r14,{r5, r6}        @ load 8 bytes from iwram
            add     r14,r14, r2         @ increase the source pointer with "width" bytes
            ldmia   r14,{r7, r8}        @ load 8 bytes from iwram
            add     r14,r14, r2         @ increase the source pointer with "width" bytes
            ldmia   r14,{r9, r10}       @ load 8 bytes from iwram
            add     r14,r14, r2         @ increase the source pointer with "width" bytes
            ldmia   r14,{r11,r12}       @ load 8 bytes from iwram
            stmia   r1!,{r5 -r12}       @ store 32 bytes in vram, and writeback the pointer
            add     r14,r14, r2         @ increase the source pointer with "width" bytes

            ldmia   r14,{r5, r6}        @ repeat this 2 times for a full 8*8 block
            add     r14,r14, r2
            ldmia   r14,{r7, r8}
            add     r14,r14, r2
            ldmia   r14,{r9, r10}
            add     r14,r14, r2
            ldmia   r14,{r11,r12}
            stmia   r1!,{r5 -r12}

            subs    r4, r4, #8          @ Substract 8 from the total number of pixels to copy
            bne     Copy8x8Block        @ Zero left?? then go to next row, else copy the next block

            subs    r3, r3, #8          @ Decrease the number of rows with 8
            beq     CopyEnd             @ Zero left?? then branch to end

            mov     r4, r2              @ A new line, so a new blocks per row counter
            add     r0, r14, #8         @ Move the source pointer to the next 8 rows

            b       Copy8x8Block        @ Start the next row

        @ we're finished, restore the registers and lets get outta here

        CopyEnd:

            ldmfd   sp!,{r0-r12,r14}
            bx      lr

@ --------------------------------------------------------------------------------------
@ EOF;
@ --------------------------------------------------------------------------------------
