;-------------------------------------------------------------------------------
; Joel Brigida
; CDA-3331C: Microprocessors
; This is a simple program for multiplying by shifting and adding bits.
;
;-------------------------------------------------------------------------------
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
;-------------------------------------------------------------------------------
; Main Loop:

            mov.w   #5, R5                  ; set up a value in teh multiplier
            mov.w   #6, R6                  ; set up a value in the multiplicand

            call    #MUL                    ; call the multiplication subroutine
loop        jmp     loop                    ; loop in place

MUL:        push.w  R5                      ; Push current values to the stack
            push.w  R7                      ;
            push.w  R8                      ;
            mov.w   #8, R8                  ; 8 bit multiplication, so we loop 8 times
            clr.w   R7                      ; additive accumulator should start with zero
            and.w   #0x00FF, R5             ; clear upper 8 bits of mltiplier
            and.w   #0x00FF, R6             ; clear upper 8 bits of multipliant

nextbit	    rrc.w   R5                      ; shift multiplier bits one at a time to the carry
            jnc     twice                   ; if no carry skip the add

addmore     add.w   R6, R7                  ; add a copy of the multiplicand to the accumulator
twice       add.w   R6, R6                  ; multiplicand times 2, (shifted 1 bit left)
            dec.w   R8			            ; decrement loop counter
            jnz     nextbit		            ; jump to check next bit of the multiplier
            mov.w   R7, R6                  ; save the result in R6

            pop.w   R8                      ; Restore previous values to the stack
            pop.w   R7                      ;
            pop.w   R5                      ;

MULTend     ret                             ; Return from subroutine

;-------------------------------------------------------------------------------
;           Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect 	.stack
;-------------------------------------------------------------------------------
;           Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET