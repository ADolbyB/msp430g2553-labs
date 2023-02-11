;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida
; CDA 3331C - Microprocessors
; March 17, 2021
;
; This Function creates an unsorted array of 16-bit integers. It then copies that array to a
; 2nd array. This 2nd array is then sorted in ascending order. The 2nd array is then iterated and
; all the non-duplicate entries are copied into a 3rd sorted array.
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
ARRAY1S     .set    0x0218                  ; Array1 Sorted

LIST_IN     .set    0x0218                  ; Sorted Array with Duplicates
LIST_OUT    .set    0x0230                  ; Sorted Array No Duplicates

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
; BEGIN Program:	Step 1: Setup 1 Unsorted and 1 Sorted Array

REG         clr     R4                      ; Clear R4: points to UNSORTED array 1st index
            clr     R5                      ; Clear R5: iterates UNSORTED array
            clr     R6                      ; Clear R6: Holds address for ARRAY1S[0]
            clr     R7                      ;
            clr     R9                      ; INNER loop counter
            clr     R10                     ; Clear R10: used as index counter
            clr     R11                     ; Backs up starting array index
            clr     R12                     ; swap backup for larger value

SORT1       mov.w   #ARRAY1, R4             ; Initialize R4 to point to ARY1 in the memory
            mov.w   #ARRAY1S, R6            ; Initialize R6 to point to ARY1S where sorted ARY1 will be stored
            call    #SETARRAY               ; Create elements are store them in ARY1
            call    #COPY                   ; Copy the elements from the ARY1 space to ARY1S space

            mov.w   #10, R10                ; set array size to R10 as a counter
            mov.w   #1, R9                  ; set inner loop counter to start at 1
            mov.w   #ARRAY1S, R7            ; reset sort iterator back to 1st sorted array index
            mov.w   #0x0218, R11            ; set starting sort address to R11
            call    #SORT                   ; Calling Subroutine Sort with parameter passing in R4 and R6

            mov.w   #0x0000, SR             ; clear the status register before proceeding

            call    #REPFR                  ; call the quiz 4 routine

THEEND      jmp     THEEND                  ; jump in place forever

; Define Array indices:
SETARRAY    mov.w   #10, 0(R4)              ; index[0] Define 10 elements in the array
            mov.w   #491, 2(R4)             ; index[1] store elements 1 - 10
            mov.w   #491, 4(R4)             ; index[2]
            mov.w   #12344, 6(R4)           ; index[3]
            mov.w   #64450, 8(R4)           ; index[4]
            mov.w   #3421, 10(R4)           ; index[5]
            mov.w   #35599, 12(R4)          ; index[6]
            mov.w   #35599, 14(R4)          ; index[7]
            mov.w   #31, 16(R4)             ; index[8]
            mov.w   #65500, 18(R4)          ; index[9]
            mov.w   #35599, 20(R4)          ; index[10]

            ret                             ; RETURN from subroutine

; ARRAY COPY Subroutine for 16-Bit UNSIGNED integers
COPY        mov.w   0(R4), R10              ; save n (# of elements) in R10 (counter)
            inc.w   R10                     ; increment by 1 to account for the byte n to be copied as well
            mov.w   R4, R5                  ; copy R4 to R5 so we keep R4 unchanged for later use
            mov.w   R6, R7                  ; copy R6 to R7 so we keep R6 unchanged for later use

LP          mov.w   @R5+, 0(R7)             ; copy elements using R5/R7 as pointers
            incd    R7                      ; increment R7 to next array index
            dec     R10                     ; decrement index counter
            jnz     LP                      ; jump if not zero to: LP (Keep looping)

            ret                             ; RETURN from subroutine

; SORT ROUTINE for 16-Bit UNSIGNED integers
SORT        cmp.w   @R7, 2(R7)              ; compare index n with index n+1 and
IF_GRTR     jhs     SKIP                    ; if: already in order: SKIP

IF_LESS     mov.w   0(R7), R12              ; Store larger value to R12
            mov.w   2(R7), 0(R7)            ; Store smaller value lower array index
            mov.w   R12, 2(R7)              ; Store larger value to higher array index
            incd    R7                      ; increment R7 to next index in SORTED array
            cmp.b   R9, R10                 ; check if at end of array
            jeq     OUTER_LP                ; decrement outer counter if at end of array
            inc.b   R9                      ; increment inner loop counter
            jmp     SORT                    ; jump back to compare

SKIP        incd    R7                      ; increment index of unsorted array
            cmp.b   R9, R10                 ; check if at end of array
            jeq     OUTER_LP                ; decrement outer counter if at end of array
            inc.b   R9                      ; increment inner loop counter
            jmp     SORT                    ; skip the sorting if the values are in order

OUTER_LP    dec.b   R10                     ; decrement R10 counter
            jz      fDONE                   ; end if counter is ZERO
            mov.b   #1, R9                  ; set R9 inner loop counter to 0 again
            mov.w   R11, R7                 ; point R7 to beginning of array again
            jmp     SORT

fDONE       ret                             ; DONE sorting

;********************=>> Copy w/ No Duplicates Function <<=********************/
;******************************************************************************/

            ;set the pointers
REPFR       mov.w   #LIST_IN, R4            ; set pointer to LIST_IN array
            mov.w   #LIST_OUT, R5           ; set pointer to LIST_OUT array
            mov.w   R5, R6                  ; save R6 as pointer where m needs to be stored
            clr     R10                     ; counter for updated m number

            ;get the 'n' number and exit if the array is empty
            mov.w   @R4+, R11               ; save n number in R11 as a loop counter
            jz      DONE                    ; exit if LIST_IN is empty
            incd.w  R5                      ; manually increment the pointer to LIST_OUT array

            ;copy 1st element to LIST_OUT and exit if that is the only element
            mov.w   @R4+, 0(R5)             ; copy 1st/NEXT element from LIST_IN to LIST_OUT
            incd    R5                      ; manually increment *pointer R5
            inc     R10                     ; increment the m counter to count for 1st element move
            dec     R11                     ; decrement the n counter to count for 1st element move
            jz      DONE                    ; DONE sorting if R11 counter = 0

CLEAR       mov.w   #0x0000, SR             ; CLEAR ERRONEOUS SR bits

LOOP        cmp.w   @R4+, -2(R5)            ; compare LIST_IN[x] with LIST_OUT[m]
            jeq     MATCH                   ; DO NOT COPY if they match

NO_MATCH    mov.w   -2(R4), 0(R5)           ; Copy next element to LIST_OUT
            incd    R5                      ; iterate to LIST_OUT[m + 1]
            inc     R10                     ; increment 'm' in LIST_OUT[0]

MATCH       dec     R11                     ; decrement 'n' # LOOPS LEFT BEFORE EXIT
            jz      DONE                    ; EXIT IF NO LOOPS LEFT
            jmp     LOOP                    ; jump back to the loop

DONE        mov.w   R10, 0(R6)              ; save the 'm' to LIST_OUT[0]
            ret                             ; exit subroutine


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
            
