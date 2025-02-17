.org $0600   ; Start at address $0600 (typical for Commodore 64 BASIC)

LDX #$00     ; Set X register to 0 (index for our string)
        
loop:   LDA message, X   ; Load character from message[X]
        BEQ done         ; If character is 0 (null terminator), exit loop
        STA $0400, X     ; Store character in video memory ($0400 for C64 screen RAM)
        INX              ; Increment X
        JMP loop         ; Repeat until null terminator is found

done:   JMP done         ; Infinite loop to prevent execution past message

message:
        .asciiz "HELLO, WORLD!"  ; Zero-terminated string