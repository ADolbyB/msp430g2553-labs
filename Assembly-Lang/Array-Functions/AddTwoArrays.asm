;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
; 
; This Program creates two arrays in memory with different values, then
; iterates the 1st array, adding those values to the values in the 2nd array
; and terminates when finished.
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
CLR_REG     clr     R5                      ; R5 reads array1 values as pointer
            clr     R6                      ; R6 points to 2nd array index
            clr     R7                      ; R7 stores value to be added to array2

SET_REG     mov.w	#200h, R5               ; set value to R5 for 1st array
            mov.w	#210h, R6               ; set R6 for 2nd array

INIT_A      mov.w	#1, &200h               ; index 1: add values to 1st array
            mov.w	#3, &202h               ; index 2
            mov.w	#5, &204h               ; index 3
            mov.w	#7, &206h               ; index 4
            mov.w	#9, &208h               ; index 5
            mov.w	#"*", &20Ah             ; end of array 1 marker

INIT_B      mov.w	#5, &210h               ; index 1: add values to 2nd array
            mov.w	#10, &212h              ; index 2
            mov.w	#15, &214h              ; index 3
            mov.w	#20, &216h              ; index 4
            mov.w	#25, &218h              ; index 5
            mov.w	#"*", &21Ah             ; end of array 2 marker

LOOP        mov.w	@R5+, R7                ; temp storage in R7
            cmp.w	#"*", R7                ; look for the end of the array
            jz      DONE                    ; jump if zero ("*" found: end of array)

            add.w	R7, 0(R6)               ; add array element to 2nd array
            incd.w	R6                      ; increment counter
            jmp     LOOP                    ; jump back to the LOOP

DONE        jmp     DONE                    ; jump in place forever when done
            ;ret                            ; use return as part of subroutine
                                            

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
            
