#include <arch/x86/modules/io/io.h>
#include <arch/x86/kernel/idt.h>
#include <arch/x86/kernel/irq.h>
#include <arch/x86/kernel/isr.h>

void *irqRoutines[IRQ_ROUTINES_SIZE];

void irq_register_handler(unsigned char index, void (*handler)(struct isr_registers *registers))
{

    irqRoutines[index] = handler;

}

void irq_unregister_handler(unsigned char index)
{

    irqRoutines[index] = 0;

}

static void irq_remap()
{

    io_outb(0x20, 0x11);
    io_outb(0xA0, 0x11);
    io_outb(0x21, 0x20);
    io_outb(0xA1, 0x28);
    io_outb(0x21, 0x04);
    io_outb(0xA1, 0x02);
    io_outb(0x21, 0x01);
    io_outb(0xA1, 0x01);
    io_outb(0x21, 0x00);
    io_outb(0xA1, 0x00);

}

void irq_handler(struct isr_registers *registers)
{

    void (*handler)(struct isr_registers *registers) = irqRoutines[registers->int_no - 32];

    if (handler)
        handler(registers);

    if (registers->int_no >= 40)
        io_outb(0xA0, 0x20);

    io_outb(0x20, 0x20);

}

void irq_init()
{

    irq_remap();

    idt_set_gate(0x20, (unsigned int)irq00, 0x08, 0x8E);
    idt_set_gate(0x21, (unsigned int)irq01, 0x08, 0x8E);
    idt_set_gate(0x22, (unsigned int)irq02, 0x08, 0x8E);
    idt_set_gate(0x23, (unsigned int)irq03, 0x08, 0x8E);
    idt_set_gate(0x24, (unsigned int)irq04, 0x08, 0x8E);
    idt_set_gate(0x25, (unsigned int)irq05, 0x08, 0x8E);
    idt_set_gate(0x26, (unsigned int)irq06, 0x08, 0x8E);
    idt_set_gate(0x27, (unsigned int)irq07, 0x08, 0x8E);
    idt_set_gate(0x28, (unsigned int)irq08, 0x08, 0x8E);
    idt_set_gate(0x29, (unsigned int)irq09, 0x08, 0x8E);
    idt_set_gate(0x2A, (unsigned int)irq0A, 0x08, 0x8E);
    idt_set_gate(0x2B, (unsigned int)irq0B, 0x08, 0x8E);
    idt_set_gate(0x2C, (unsigned int)irq0C, 0x08, 0x8E);
    idt_set_gate(0x2D, (unsigned int)irq0D, 0x08, 0x8E);
    idt_set_gate(0x2E, (unsigned int)irq0E, 0x08, 0x8E);
    idt_set_gate(0x2F, (unsigned int)irq0F, 0x08, 0x8E);

}

