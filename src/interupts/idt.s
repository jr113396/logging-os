.intel_syntax noprefix

.global idt_flush

idt_flush:
    MOV eax, [esp + 4]
    LIDT [eax]
    STI
    RET

.macro ISR_NOERRORCODE arg
.global isr\arg

isr\arg:
    CLI
    PUSH  0
    PUSH  \arg

    JMP isr_common

.endm

.macro ISR_ERRORCODE arg
.global isr\arg
isr\arg:
    CLI
    PUSH  \arg
    JMP isr_common

.endm

.macro IRQ arga argb
.global irq\arga
irq\arga:
    CLI
    PUSH 0
    PUSH  \argb
    JMP irq_common

.endm


ISR_NOERRORCODE 0
ISR_NOERRORCODE 1
ISR_NOERRORCODE 2
ISR_NOERRORCODE 3
ISR_NOERRORCODE 4
ISR_NOERRORCODE 5
ISR_NOERRORCODE 6
ISR_NOERRORCODE 7
ISR_ERRORCODE   8
ISR_NOERRORCODE 9
ISR_ERRORCODE   10
ISR_ERRORCODE   11
ISR_ERRORCODE   12
ISR_ERRORCODE   13
ISR_ERRORCODE   14
ISR_NOERRORCODE 15
ISR_NOERRORCODE 16
ISR_NOERRORCODE 17
ISR_NOERRORCODE 18
ISR_NOERRORCODE 19
ISR_NOERRORCODE 20
ISR_NOERRORCODE 21
ISR_NOERRORCODE 22
ISR_NOERRORCODE 23
ISR_NOERRORCODE 24
ISR_NOERRORCODE 25
ISR_NOERRORCODE 26
ISR_NOERRORCODE 27
ISR_NOERRORCODE 28
ISR_NOERRORCODE 29
ISR_NOERRORCODE 30
ISR_NOERRORCODE 31
ISR_NOERRORCODE 128
ISR_NOERRORCODE 177

IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47


.extern isr_handler
isr_common:
    pusha
    mov eax, ds
    PUSH eax
    MOV eax, cr2
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp

    CALL isr_handler

    ADD esp, 8
    POP ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    POPA
    ADD esp, 8
    STI
    IRET

.extern irq_handler
irq_common:
    pusha
    mov eax, ds
    PUSH eax
    MOV eax, cr2
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp

    CALL irq_handler

    ADD esp, 8
    POP ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    POPA
    ADD esp, 8
    STI
    IRET
