#include <memory.h>
#include <kernel.h>
#include <arch/x86/arch.h>
#include <arch/x86/cpu.h>
#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/isr.h>
#include <arch/x86/mmu.h>
#include <arch/x86/syscall.h>
#include <arch/x86/tss.h>

void arch_setup()
{

    unsigned short cs;
    unsigned short ss;

    gdt_setup();
    idt_setup();

    cs = gdt_get_selector(GDT_INDEX_KCODE);
    ss = gdt_get_selector(GDT_INDEX_KDATA);

    tss_setup(ss, 0x00400000);
    isr_setup(cs);

    mmu_setup_arch(cs);
    syscall_setup_arch(cs);

}

