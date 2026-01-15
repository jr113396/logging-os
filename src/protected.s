# The following code is public domain licensed
.intel_syntax noprefix
.global enable_a20
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

.global enable_protected
    cli            # disable interrupts
    mov eax, cr0 
    or al, 1       # set protection bit in CR0
    mov cr0, eax
    ret
    # Perform far jump to selector 08h (offset into GDT, pointing at a 32bit PM code segment descriptor) 
    # to load CS with proper PM32 descriptor)
//     jmp PModeMain

// PModeMain:
//     call kprotectedMain

