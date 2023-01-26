;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
; 
; Joel Brigida 
; CDA 3331C - Microprocessors
; 
; This is a simple program for a lab experiment which takes 3 separate integers
; from memory locations, copies them to 3 separate registers (R7, R8, R9), 
; then adds each value for a total in R10.
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
LAB0        mov.w   #01,&0200h              ; set a number on location $0200
            mov.w   #02,&0202h              ; set a number on location $0202
            mov.w   #03,&0204h              ; set a number on location $0204

LINEA       clr     R7                      ; clear the entire R7 register
            clr     R8                      ; clear the entire R8 register
            clr     R9                      ; clear the entire R9 register
            clr     R10                     ; clear the entire R10 register

LINEB       mov.w   &0200h, R7              ; copy a word from &0200h to R7
            mov.w   &0202h, R8              ; copy a word from &0202h to R8
            mov.w   &0204h, R9              ; copy a word from &0204h to R9


LINEC       mov.b   R7, R10                 ; Start accumulatior R10 with R7 value
            add.b   R8, R10                 ; add to it the content R8
            add.b   R9, R10                 ; add to it the content R9
            mov.w   R10,&0206h              ; now store the sum in L

Mainloop    jmp     Mainloop                ; Loop in Place when done.
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
            
