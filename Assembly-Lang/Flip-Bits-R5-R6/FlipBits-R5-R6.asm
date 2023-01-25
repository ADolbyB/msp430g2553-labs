;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
; 
; This program will take a value from R5 and perform a "bit flipping" operation.
; This will result in all 1s becoming 0s, and all 0s becoming 1s. The final
; result will be stored in R5
; 
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

            clr.w   R6                      ; clear R6 to zero
            mov.w   #0xEDCB, R5             ; copy EDCB to R5 for flipping

FLIP        mov.b   #16, R10                ; move 16(dec) to R10 (as a counter)

LOOP        rlc.w   R5                      ; rotate left w/ carry R5
            rrc.w   R6                      ; rotate right from carry to R6

            dec.b   R10                     ; Loop condition
            jnz     LOOP                    ; jump if not zero to 'LOOP'

            mov.w   R6, R5                  ; move the flipped word back to R5
DONE        jmp     DONE                    ; jump in place forever
            ;ret if subroutine              ; return from subroutine

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
