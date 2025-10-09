.global gdt_flush
.intel_syntax noprefix

gdt_flush:
    mov eax, [esp+4]
    LGDT [eax]

    mov eax, 0x10 # offset to gdt
    mov ds, ax # point data segment to gdt table
    mov es, ax # and same for other segment registers
    mov fs, ax
    mov gs, ax
    mov ss, ax

    JMP 0x08:.flush # set code segment with jump to segment

.flush:
    RET

.global tss_flush
tss_flush:
    mov ax, 0x28
    ltr ax
    RET
