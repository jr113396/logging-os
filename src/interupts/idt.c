#include "idt.h"
#include <io/io.h>
#include <stdio.h>
#include "drivers/serial/serial.h"

struct idt_value_s idt_entries[256];
struct idt_ptr_s idt_ptr;

extern void idt_flush(uint32_t);


void *irq_routines[16] = {
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

void *isr_routines[32] = {0};

char *exceptionMsgs[] = {
    "Division by Zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "Device not Avail",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown - Intel",
    "x87 Float Error",
    "Alignment Fault",
    "Machine Check",    
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
    
};

// using info from https://wiki.osdev.org/CPU_Registers_x86
void eflagsTranslate(uint32_t eflags) {
    sprintf("EFLAGS: %b\r\n", eflags);
    if (eflags & (1 << 0)) 
        sprintf("Carry Set\r\n");
    
    if (eflags & (1 << 2)) 
        sprintf("Parity Set\r\n");
    
    if (eflags & (1 << 4)) 
        sprintf("Aux Set\r\n");
    
    if (eflags & (1 << 6)) 
        sprintf("Zero Set\r\n");
    
    if (eflags & (1 << 7)) 
        sprintf("Sign Set\r\n");
    
    if (eflags & (1 << 8)) 
        sprintf("Trap Set\r\n");
    
    if (eflags & (1 << 9)) 
        sprintf("Interupt enable Set\r\n");
    
    if (eflags & (1 << 10)) 
        sprintf("Direction Set\r\n");
    
    if (eflags & (1 << 11)) 
        sprintf("Overflow Set\r\n");
    
    if (eflags & (0b11 << 12)) 
        sprintf("IO Priv Set\r\n");
    
    if (eflags & (1 << 14)) 
        sprintf("Nested task Set\r\n");
    
    if (eflags & (1 << 16)) 
        sprintf("Resume flag Set\r\n");
    
    if (eflags & (1 << 17)) 
        sprintf("V8086 Set\r\n");

    if (eflags & (1 << 18)) 
        sprintf("Alignment Check\r\n");    
    
    if (eflags & (1 << 19)) 
        sprintf("Virtual Interupt Flag\r\n");

    if (eflags & (1 << 20)) 
        sprintf("Virtual Interupt pending\r\n");

    if (eflags & (1 << 21)) 
        sprintf("Able to use CPUID Instruction\r\n");
}

void displayRegisters(struct InterruptRegisters *regs) {
     kprintf("cr2 %d, ds: %d, edi: %d, esi: %d, ebp: %d, esp: %d, ebx: %d, esp: %d, ecx: %d, eax: %d\n",
        regs->cr2,
        regs->ds,
        regs->edi,
        regs->esi,
        regs->ebp,
        regs->esp,
        regs->ebx,
        regs->edx,
        regs->ecx,
        regs->eax
    );

    kprintf("I#: %d, ECode: %d\n eip: %d, csm: %d, eflags: %d, use resp: %d, ss: %d \n",
        regs->interuptNo,
        regs->errorCode,
        regs->eip,
        regs->csm,
        regs->eflags,
        regs->useresp,
        regs->ss
    );
    eflagsTranslate(regs->eflags);
    uint32_t *eip_broken = (uint32_t *)(regs->eip);
    kprintf("EIP: %x, %ux\n", eip_broken, *eip_broken);
}
void divByZeroHandler(struct InterruptRegisters *regs) {
    kprintf("Division by zero handler being run yay\n");
    asm ( "hlt;\n\t" );

}



void install_default_isrs() {
    sprintf("Installing /0 handler\r\n");
    install_isr_handler(0, &divByZeroHandler);
}

void isr_handler(struct InterruptRegisters *regs) {
    if (regs-> interuptNo <  32) {
        kprintf("%d - %d\n", isr_routines[regs->interuptNo], regs->interuptNo);
        if (isr_routines[regs->interuptNo] == 0) { // if no handler then use the default -> print error message and halt
            kprintf("Interrupt %d fired and is being handled by the fault handler\n", regs->interuptNo);
            // we have an error message for that interupt no
            kprintf(exceptionMsgs[regs->interuptNo]);
            kprintf("\n");
            displayRegisters(regs);
            kprintf("System Halted\n");
            asm ( "hlt;\n\t" );
        }
        else {
            kprintf("Running Custom Handler for %d\n", regs->interuptNo);
            void (*handler)(struct InterruptRegisters *regs); // call the defined handler function for that error
            handler = isr_routines[regs->interuptNo];
            handler(regs);
            kprintf("Custom Handler for %d\n returned\r\n", regs->interuptNo);
        }
    }
}


void setIDTGate(uint8_t num, uint32_t offset, uint16_t segment, uint8_t flags) {
    idt_entries[num].offset_low         = offset & 0xFFFF;
    idt_entries[num].offset_high        = (offset >> 16) & 0xFFFF;

    idt_entries[num].segment_selector   = segment;
    idt_entries[num].resvd              = 0;
    idt_entries[num].flags              = flags | 0x60;
}


void irq_handler(struct InterruptRegisters *regs) {
    void (*handler)(struct InterruptRegisters *regs);
    handler = irq_routines[regs->interuptNo -32];
    if (handler) {
        handler(regs);
    }
    if (regs->interuptNo >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

void install_irq_handler (int irq, void (*handler)(struct InterruptRegisters *r)) {
    irq_routines[irq] = handler;
}

void uinstall_irq_handler(int irq) {
    irq_routines[irq] = 0;
}

void install_isr_handler (int isr, void (*handler)(struct InterruptRegisters *r)) {
    isr_routines[isr] = handler;
}

void uinstall_isr_handler(int isr) {
    isr_routines[isr] = 0;
}


void setIDT() {
    idt_ptr.limit = sizeof(struct idt_value_s) * 256 - 1;
    idt_ptr.base = (uint32_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_value_s) * 256); //reset all values to 0

    // PICs sit at 
    //          0x20 commands, 0x21 data
    //          0xA0 commands, 0xA1 data
    
    // set chips into initalisation mode
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // vector offsets
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x00);
    outb(0xA1, 0x00);

    // 0x8E - 1000 1110
    //Present ^^ ^ ^
    //        /  | |
    //DPL to 0   / |
    //          /  |
    // Always 0    Gate type - Always 'E'

    // 0x08 - 0000 1000
    // Selector Code Segment
    //

    // ISRs
    setIDTGate(0, (uint32_t)isr0,0x08, 0x8E);
    setIDTGate(1, (uint32_t)isr1,0x08, 0x8E);
    setIDTGate(2, (uint32_t)isr2,0x08, 0x8E);
    setIDTGate(3, (uint32_t)isr3,0x08, 0x8E);
    setIDTGate(4, (uint32_t)isr4, 0x08, 0x8E);
    setIDTGate(5, (uint32_t)isr5, 0x08, 0x8E);
    setIDTGate(6, (uint32_t)isr6, 0x08, 0x8E);
    setIDTGate(7, (uint32_t)isr7, 0x08, 0x8E);
    setIDTGate(8, (uint32_t)isr8, 0x08, 0x8E);
    setIDTGate(9, (uint32_t)isr9, 0x08, 0x8E);
    setIDTGate(10, (uint32_t)isr10, 0x08, 0x8E);
    setIDTGate(11, (uint32_t)isr11, 0x08, 0x8E);
    setIDTGate(12, (uint32_t)isr12, 0x08, 0x8E);
    setIDTGate(13, (uint32_t)isr13, 0x08, 0x8E);
    setIDTGate(14, (uint32_t)isr14, 0x08, 0x8E);
    setIDTGate(15, (uint32_t)isr15, 0x08, 0x8E);
    setIDTGate(16, (uint32_t)isr16, 0x08, 0x8E);
    setIDTGate(17, (uint32_t)isr17, 0x08, 0x8E);
    setIDTGate(18, (uint32_t)isr18, 0x08, 0x8E);
    setIDTGate(19, (uint32_t)isr19, 0x08, 0x8E);
    setIDTGate(20, (uint32_t)isr20, 0x08, 0x8E);
    setIDTGate(21, (uint32_t)isr21, 0x08, 0x8E);
    setIDTGate(22, (uint32_t)isr22, 0x08, 0x8E);
    setIDTGate(23, (uint32_t)isr23, 0x08, 0x8E);
    setIDTGate(24, (uint32_t)isr24, 0x08, 0x8E);
    setIDTGate(25, (uint32_t)isr25, 0x08, 0x8E);
    setIDTGate(26, (uint32_t)isr26, 0x08, 0x8E);
    setIDTGate(27, (uint32_t)isr27, 0x08, 0x8E);
    setIDTGate(28, (uint32_t)isr28, 0x08, 0x8E);
    setIDTGate(29, (uint32_t)isr29, 0x08, 0x8E);
    setIDTGate(30, (uint32_t)isr30, 0x08, 0x8E);
    setIDTGate(31, (uint32_t)isr31, 0x08, 0x8E);

    // IRQs
    setIDTGate(32, (uint32_t)irq0, 0x08, 0x8E);
    setIDTGate(33, (uint32_t)irq1, 0x08, 0x8E);
    setIDTGate(34, (uint32_t)irq2, 0x08, 0x8E);
    setIDTGate(35, (uint32_t)irq3, 0x08, 0x8E);
    setIDTGate(36, (uint32_t)irq4, 0x08, 0x8E);
    setIDTGate(37, (uint32_t)irq5, 0x08, 0x8E);
    setIDTGate(38, (uint32_t)irq6, 0x08, 0x8E);
    setIDTGate(39, (uint32_t)irq7, 0x08, 0x8E);
    setIDTGate(40, (uint32_t)irq8, 0x08, 0x8E);
    setIDTGate(41, (uint32_t)irq9, 0x08, 0x8E);
    setIDTGate(42, (uint32_t)irq10, 0x08, 0x8E);
    setIDTGate(43, (uint32_t)irq11, 0x08, 0x8E);
    setIDTGate(44, (uint32_t)irq12, 0x08, 0x8E);
    setIDTGate(45, (uint32_t)irq13, 0x08, 0x8E);
    setIDTGate(46, (uint32_t)irq14, 0x08, 0x8E);
    setIDTGate(47, (uint32_t)irq15, 0x08, 0x8E);


    // for system calls
    setIDTGate(128, (uint32_t)isr128, 0x08, 0x8E);
    setIDTGate(177, (uint32_t)isr177, 0x08, 0x8E);

    idt_flush((uint32_t) &idt_ptr);

    install_default_isrs();
}