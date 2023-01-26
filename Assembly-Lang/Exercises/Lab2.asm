;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
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
;
LINEA       clr.w   R5                      ; clear the entire R5 register
            clr.w   R6                      ; clear the entire R6 register
            clr     R8                      ; clear the entire R8 register
            clr.w   &0200h                  ; clear the contents of 0x0200h
            clr.w   &0202h                  ; clear the contents of 0x0202h
            clr.w   &0204h                  ; clear the contents of 0x0204h
            clr.w   &0206h                  ; clear the contents of 0x0206h
            clr.w   &0208h                  ; clear the contents of 0x0208h
            clr.w   &020Ah                  ; clear the contents of 0x020Ah

LINEB       mov.w   #1100h, &0200h          ; copy number 0x1100 to &0x0200
            mov.w   #2200h, &0202h          ; copy number 0x2200 to &0x0202
            mov.w   #3300h, &0204h          ; copy number 0x3300 to &0x0204
            mov.w   #4400h, &0206h          ; copy number 0x4400 to &0x0206
            mov.w   #5500h, &0208h          ; copy number 0x5500 to &0x0208
            mov.w   #6600h, &020Ah          ; copy number 0x6600 to &0x020A

            mov.w   #0208h, R5              ; copy number to R5
            mov.w   #0xA006, R6             ; copy number to R6

NOTQ2H      mov.w   #0004h, SR              ; set Negative N-bit to 1
            mov.w   #0002h, SR              ; set Zero Z-bit to 1
            mov.w   #0100h, SR              ; set Overflow V-bit to 1
            mov.w   #0001h, SR              ; set Carry C-bit to 1
            mov.w   #0000h, SR              ; zero out the status register
            setz                            ; set zero bit to 1
            setc                            ; set carry to 1
            clrc                            ; clear the carry bit
            clrz                            ; clear the zero bit

LINEC       addc.w  @R5+, R6                ; add with carry a word from R5 to R6
            mov.w   &0204h, R6              ; copy a word from &0204h to R6
            mov.w   R6, 0(R5)               ; move a word from R6 to R5
            bic.w   #3100h, R6              ; bit clear the 0x3100 with R6
            cmp.b   4(R6), -2(R5)           ; compare values in R6 with R5

LINED       mov.w   #5800h, &0200h          ; copy number 0x5800 to &0x0200
            mov.w   #5700h, &0202h          ; copy number 0x5700 to &0x0202
            mov.w   #5600h, &0204h          ; copy number 0x5600 to &0x0204
            mov.w   #5500h, &0206h          ; copy number 0x5500 to &0x0206
            mov.w   #5400h, &0208h          ; copy number 0x5400 to &0x0208
            mov.w   #5300h, &020Ah          ; copy number 0x5300 to &0x020A

            setz                            ; set zero bit to 1
            setc                            ; set carry bit to 1

            mov.w   #0200h, R5              ; copy number to R5
            mov.w   #020Ah, SP              ; copy number to Stack Pointer

LINEE		rrc.w	-4(SP)                  ; Rotate Right w/ Carry word to stack pointer
            push    4(R5)                   ; Push a 4 to the top of stack
            xor.w   -2(SP),R5               ; Bitwise XOR word -2 with value at top of stack
            and.w   SP,R5                   ; Bitwise AND word value at top of stack w/ R5
            pop     R5                      ; Pop the top of the stack to R5

Mainloop    jmp     Mainloop                ; Infinite Loop
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
            
