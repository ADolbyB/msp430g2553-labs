;----------------------------------------------------------------------------------
; Joel Brigida
; CDA-3331C: Microprocessors
; This sample code performs a bunch of basic operations that are 
; useful for many operations
;----------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section
            .retainrefs                     ; Additionally retain any sections
                                            ; that have references to current
                                            ; section
;-------------------------------------------------------------------------------

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here


Setup       mov.w   #0AAAAh, R4             ; Setup for examples below
            mov.w   #1111h,  R5             ; Instruction SRC, DEST (Double Operand)
            mov.w   #3333h,  R6
            mov.w   #1,  R7                ; Instruction DST (Single Operand) or Instruction SRC
            mov.w   #2,  R8



Movement    mov.w   R4,  R5                 ; Moves the value from the source to the destination
            mov.b   R4,  R6                 ; (DST = SRC)

            push.w  R5                      ; The SP starts at 400h so set Memory browser to x3F0
            push.b  R5                      ; This will push the SRC (single operand) into the memory address pointed to by SP-2
                                            ; SP will be decreased by 2 for either .b or .w
                                            ; (-2(SP) = SRC)
            pop.w   R7                      ; Will push the current value located at 0(PC) into the DEST
            pop.b   R7                      ; (DST = 0(SP)) SP is post incremented
                                            ; Notice the values in address stays the same and only the SP is changed

Addition    add.w   R6,  R4                 ;
            add.b   R6,  R4                 ; (DST += SRC)
            addc.w  #0FFFFh, R8             ; (DST += SRC + Carry)
            adc.w   R8                      ; (DST += Carry)

Subtract    sub.w   R6,  R5                 ;
            sub.b   R6,  R5                 ; (DST -= SRC)
            subc.w  R6,  R5                 ; (DST -= SRC + Carry)
            sbc.w   R4                      ; (DST += FFFFh) Subtract source and borrow

Compare     cmp.w   R5,  R6                 ; (DST-SRC)
            cmp.w   R6,  R5
            tst.w   R5                      ; (cmp #0,DST)

Clear       clr.w   R5                      ; (DST = 0) if .b only clears the first byte

Inc_Dec     inc.w   R5                      ; (DST += 1)
            incd.w  R5                      ; (DST += 2)
            dec.w   R5                      ; (DST -= 1)
            decd.w  R5                      ; (DST -= 2)

Logic       mov.w   #0FAFAh, R5             ; Setting up example
            and.w   R6,  R5                 ; Does the And operation for each bit and stores it in R6

            mov.w   #0FAFAh, R5
            xor.w   R6,  R5                 ; Does the Xor operation for each bit and stores it in DST

            mov.w   #0FAFAh, R5
            bit.w   R6,  R5                 ; Does the Or operation for each bit and stores it in DST

            mov.w   #0FAFAh, R5
            inv.w   R5                      ; Inverts each bit and stores it in DST

            mov.w   #0FAFAh, R5
            bis.w   R6,  R5                 ;(R6 |= R5)

            mov.w   #0FAFAh, R5
            bic.w   R6,  R5                 ;(R6 &= /R5)

            mov.w   #4,  R8                 ; setting our loop counter
Jumps       dec.w   R8                      ; decrementing the counter each loop
            tst.w   R8                      ; testing if R8 is 0
            jnz     Jumps                   ; jump to Jumps if R8 is not 0, otherwise keep going
            call    #Test                   ; function call to Test, pushes current location onto the stack

Loop        jmp     Loop

Test        mov.w   #0FFFFh, R6             ;
            ret                             ;returns you back to the line after the function call by pushing the current SP location's value into the PC

;-------------------------------------------------------------------------------
;           Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect 	.stack
;-------------------------------------------------------------------------------
;           Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                    ; MSP430 RESET Vector
            .short  RESET