.text
.global loadPageDirectory
loadPageDirectory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax # load the addr of the page table from the stack
    mov %eax, %cr3 # move it into the cr3 register for the mmu
    mov %ebp, %esp
    pop %ebp
    ret

.global enablePaging
enablePaging:
    push %ebp
    mov %esp, %ebp
    mov %cr0, %eax
    or $0x80000000, %eax # - set the 32th bit which enables paging
    mov %eax, %cr0
    mov %ebp, %esp
    pop %ebp
    ret
