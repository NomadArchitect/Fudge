#include <fudge.h>
#include <kernel.h>
#include "cpu.h"
#include "reg.h"
#include "pic.h"
#include "uart.h"
#include "kmi.h"
#include "timer.h"
#include "lcd.h"
#include "arch.h"

#define ISR_RESET   0x00
#define ISR_UNDEF   0x01
#define ISR_SWINT   0x02
#define ISR_ABRTP   0x03
#define ISR_ABRTD   0x04
#define ISR_RESV1   0x05
#define ISR_IRQ     0x06
#define ISR_FIQ     0x07

extern unsigned int isr_swi;
extern unsigned int isr_irq;
extern unsigned int isr_fiq;
extern unsigned int test_call(unsigned int);

static struct cpu_general registers[KERNEL_TASKS];

static void shownum(unsigned int n)
{

    char num[32];

    cstring_write_fmt1(num, 32, "%u\n\\0", 0, &n);
    uart_puts(num);

}

static void isr_install(unsigned int index, void *addr)
{

    unsigned int value = 0xEA000000 | (((unsigned int)addr - (8 + (4 * index))) >> 2);

    buffer_copy((void *)(index * 4), &value, 4);

}

static unsigned int spawn(unsigned int itask, void *stack)
{

    struct {void *caller; unsigned int ichannel; unsigned int id;} *args = stack;

    if (args->ichannel && args->id)
    {

        unsigned int ntask = kernel_createtask();

        if (ntask)
        {

            return kernel_loadtask(ntask, 0, 0x6000, args->ichannel, args->id);

        }

    }

    DEBUG_FMT0(DEBUG_ERROR, "spawn failed");

    return 0;

}

static void testtask(void)
{

    uart_puts("TEST TASK 1\n");

    test_call(123);
    test_call(2);
    test_call(1);
    test_call(3);

    uart_puts("TEST TASK 2\n");

    for(;;);

}

static void schedule(struct cpu_general *general, struct cpu_interrupt *interrupt)
{

    struct core *core = kernel_getcore();

    if (core->itask)
    {

        struct task_thread *thread = kernel_getthread(core->itask);

        buffer_copy(&registers[core->itask], general, sizeof (struct cpu_general));

        thread->ip = interrupt->pc.value;
        thread->sp = interrupt->sp.value;

        uart_puts("SAVE TASK\n");

    }

    core->itask = kernel_schedule(core);

    if (core->itask)
    {

        struct task_thread *thread = kernel_getthread(core->itask);

        buffer_copy(general, &registers[core->itask], sizeof (struct cpu_general));

        interrupt->pc.value = thread->ip;
        interrupt->sp.value = thread->sp;

        uart_puts("LOAD TASK\n");

    }

    else
    {

        interrupt->pc.value = (unsigned int)cpu_halt;
        interrupt->sp.value = core->sp;

        uart_puts("HALT\n");

    }

}

void arch_syscall(void *stack)
{

    struct {struct cpu_interrupt interrupt; struct cpu_general general; unsigned int lr;} *args = stack;
    struct core *core = kernel_getcore();

    shownum(args->general.r0.value);

    args->general.r0.value = abi_call(args->general.r7.value, core->itask, args->interrupt.sp.reference);

    schedule(&args->general, &args->interrupt);

}

void arch_leave(void)
{

    struct cpu_general general;
    struct cpu_interrupt interrupt;

    buffer_clear(&general, sizeof (struct cpu_general));
    buffer_clear(&interrupt, sizeof (struct cpu_interrupt));
    schedule(&general, &interrupt);
    cpu_leave(interrupt.sp.value, interrupt.pc.value);

}

void arch_setup1(void)
{

    resource_setup();
    pic_setup();
    uart_setup();
    timer_setup();
    kmi_setup();
    lcd_setup();
    kernel_setup(ARCH_KERNELSTACKPHYSICAL, ARCH_KERNELSTACKSIZE, ARCH_MAILBOXPHYSICAL, ARCH_MAILBOXSIZE);
    abi_setup();
    abi_setcallback(0x0C, spawn);

}

void arch_setup2(void)
{

    unsigned int ntask = kernel_createtask();

    if (ntask)
    {

        kernel_loadtask(ntask, (unsigned int)&testtask, 0x6000, 0, 0);
        kernel_place(0, ntask, EVENT_MAIN, 0, 0);

    }

    else
    {

        DEBUG_FMT0(DEBUG_ERROR, "spawn failed");

    }

    arch_leave();

}

void arch_setup(void)
{

    isr_install(ISR_RESET, 0);
    isr_install(ISR_UNDEF, 0);
    isr_install(ISR_SWINT, &isr_swi);
    isr_install(ISR_ABRTP, 0);
    isr_install(ISR_ABRTD, 0);
    isr_install(ISR_RESV1, 0);
    isr_install(ISR_IRQ, &isr_irq);
    isr_install(ISR_FIQ, &isr_fiq);
    arch_setup1();
    arch_setup2();

}

