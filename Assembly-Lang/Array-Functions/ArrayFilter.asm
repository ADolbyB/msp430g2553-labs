;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida
; CDA 3331C - Microprocessors
; March 23, 2021
; 
; This subroutine will scan the whole list (all n elements) and filter any
; element which is higher(as an unsigned value) than the Threshold value. Filtering
; means replacing the element with the 0x00 value, and updating the number m.
; The first word in the list is number n: the actual elements in the list.
; The second word in the list is the Filter Threshold value.
; The third word in the list is number m: the # of elements filtered.
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
; Memory allocation of arrays must be done prior to the RESET & StopWDT

ARRAY1      .set    0x0200                  ; Array1 Unsorted
ARRAY1S     .set    0x021C                  ; Array1 Sorted

LIST_IN     .set    0x021C                  ; Sorted Array with Duplicates
LIST_OUT    .set    0x0238                  ; Sorted Array No Duplicates

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
;
; BEGIN Program:    Step 1: Setup 1 Unsorted Array and 1 copied array

REG         clr     R4                  ; Clear R4: points to UNSORTED array 1st index
            clr     R5                  ; Clear R5: iterates UNSORTED array
            clr     R6                  ; Clear R6: Holds address for ARRAY1S[0]
            clr     R7                  ;
            clr     R9                  ; INNER loop counter
            clr     R10                 ; Clear R10: used as index counter
            clr     R11                 ; Backs up starting array index
            clr     R12                 ; swap backup for larger value

SORT1       mov.w   #ARRAY1, R4         ; Initialize R4 to point to ARY1 in the memory
            mov.w   #ARRAY1S, R6        ; Initialize R6 to point to ARY1S in the memory where the sorted ARY1 will be stored
            call    #SETARRAY           ; Create elements are store them in ARY1
            call    #COPY               ; Copy the elements from the ARY1 space to ARY1S space

            mov.w   #0x0000, SR         ; clear the status register before proceeding

            call    #FILTER             ; CALL the FILTER Routine

            ;mov.w  #0x021C, R11        ; set starting sort address to R11
            ;call   #SORT               ; Call Sort to sort filtered array

THEEND      jmp     THEEND              ; jump in place forever

; Define Array indices:
SETARRAY    mov.w   #10, 0(R4)          ; index[0] Define 10 elements in the array
            mov.w   #4910, 2(R4)        ; index[1] Filter Threshold Value (in decimal)
            mov.w   #0, 4(R4)           ; index[2] # of Filtered indices
            mov.w   #4910, 6(R4)        ; index[3] -> 1st array index
            mov.w   #4909, 8(R4)        ; index[4]
            mov.w   #4911, 10(R4)       ; index[5]
            mov.w   #39456, 12(R4)      ; index[6]
            mov.w   #3225, 14(R4)       ; index[7]
            mov.w   #31, 16(R4)         ; index[8]
            mov.w   #60251, 18(R4)      ; index[9]
            mov.w   #1234, 20(R4)       ; index[10]
            mov.w   #4909, 22(R4)       ; index[11]
            mov.w   #50001, 24(R4)      ; index[12] -> Last Array index

            ret                         ; RETURN from subroutine

; ARRAY COPY Subroutine for 16-Bit UNSIGNED integers
COPY        mov.w   0(R4), R10          ; save n (# of elements) in R10 (counter)
            inc.w   R10                 ; increment by 1 to account for the byte n to be copied as well
            incd.w  R10                 ; increment by 2 to accouunt for threshold & # filtered elements
            mov.w   R4, R5              ; copy R4 to R5 so we keep R4 unchanged for later use
            mov.w   R6, R7              ; copy R6 to R7 so we keep R6 unchanged for later use

LP          mov.w   @R5+, 0(R7)         ; copy elements using R5/R7 as pointers
            incd    R7                  ; increment R7 to next array index
            dec     R10                 ; decrement index counter
            jnz     LP                  ; jump if not zero to: LP (Keep looping)

            ret                         ; RETURN from subroutine


;***************************=>> Start Filter Routine <<=***************************/
;**********************************************************************************/

            ; set the pointers
FILTER      mov.w   #LIST_IN, R5        ; set pointer to LIST_IN array
            mov.w   R5, R6              ; save R6 as pointer to the array address pointer
            mov.w   @R5+, R10           ; save # array elements in R10
            mov.w   @R5+, R11           ; save Threshold value to R11
            incd    R5                  ; point R5 to the 1st array element
            clr     R12                 ; counter for # of filtered elements

            ; get the 'n' number and exit if the array is empty
            ;mov.w  @R4+, R11           ; save n number in R11 as a loop counter
            ;jz     DONE                ; exit if LIST_IN is empty
            ;incd.w R5                  ; manually increment the pointer to LIST_OUT array
            ;dec    R11                 ; avoids off by one error at the end

LOOP        mov.w   #0x0000, SR         ; CLEAR ERRONEOUS SR bits

            cmp.w   @R5+, R11           ; Compare array index with threshold value
            jeq     SKIP_FILT           ; DO NOT FILTER if the SAME
            jhs     SKIP_FILT           ; DO NOT FILTER if lower (Why is it jhs, though??)

FILT_X      mov.w   #0x0000, -2(R5)     ; FILTER if value is higher or same as threshold
            inc     R12                 ; increment counter: # of filtered elements

SKIP_FILT   dec     R10                 ; decrement # array indices left till end of array
            jz      DONE                ; jump to DONE if at end of array (R10 = 0)
            jmp     LOOP                ; jump back to the loop if not at end of array

DONE        mov.w   R12, 4(R6)          ; write # of filtered elements (m) to 3rd array index
            ret                         ; exit subroutine

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
            
