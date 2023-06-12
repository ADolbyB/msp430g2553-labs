;-------------------------------------------------------------------------------
; Joel Brigida
; CDA-3331C: Microprocessors
; This is a simple subroutine for performing multipication
; using repretitive addition.
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
; Main loop here

            mov.w   #12, R5                 ; 
            mov.w   #7,  R6                 ;

            call    #MUL                    ; Function Call to MULT

loop        jmp     loop                    ; Failsafe: Jump in place forever

MUL:        clr     R10                     ; Start repetitive addition

addmore		add.w	R5, R10
            dec.b   R6
            cmp.b   #0, R6
            jne     addmore
            mov.w   R10, R6;

MULTend     ret                             ; Return from subroutine

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