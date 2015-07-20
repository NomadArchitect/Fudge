#include <fudge.h>
#include <kernel.h>
#include "cpu.h"
#include "arch.h"
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "isr.h"
#include "mmu.h"

#define CONTAINERS                      8
#define TASKS                           64
#define GDTDESCRIPTORS                  6
#define IDTDESCRIPTORS                  256
#define TSSDESCRIPTORS                  1
#define MMUALIGN                        0x1000
#define KERNELSTACK                     0x00400000
#define TASKSTACK                       0x80000000
#define CODEBASE                        0x01000000
#define CODESIZE                        0x00080000
#define STACKBASE                       0x02000000
#define STACKSIZE                       0x00010000
#define CONTAINERDIRECTORYCOUNT         1
#define CONTAINERDIRECTORYBASE          0x00400000
#define CONTAINERTABLECOUNT             8
#define CONTAINERTABLEBASE              (CONTAINERDIRECTORYBASE + CONTAINERS * (MMUALIGN * CONTAINERDIRECTORYCOUNT))
#define TASKDIRECTORYCOUNT              1
#define TASKDIRECTORYBASE               (CONTAINERTABLEBASE + CONTAINERS * (MMUALIGN * CONTAINERTABLECOUNT))
#define TASKTABLECOUNT                  2
#define TASKTABLEBASE                   (TASKDIRECTORYBASE + TASKS * (MMUALIGN * TASKDIRECTORYCOUNT))

static struct
{

    struct gdt_pointer pointer;
    struct gdt_descriptor descriptors[GDTDESCRIPTORS];

} gdt;

static struct
{

    struct idt_pointer pointer;
    struct idt_descriptor descriptors[IDTDESCRIPTORS];

} idt;

static struct
{

    struct tss_pointer pointer;
    struct tss_descriptor descriptors[TSSDESCRIPTORS];

} tss;

static struct
{

    unsigned short kcode;
    unsigned short kdata;
    unsigned short ucode;
    unsigned short udata;
    unsigned short tlink;

} selector;

static struct arch_container
{

    struct container base;
    struct mmu_directory *directory;
    struct mmu_table *table;

} containers[CONTAINERS];

static struct arch_task
{

    struct task base;
    struct cpu_general general;
    struct mmu_directory *directory;
    struct mmu_table *table;
    unsigned int mapping[TASKTABLECOUNT];

} tasks[TASKS];

static struct
{

    struct container *container;
    struct task *task;

} current;

static void containermapcode(struct container *container)
{

    struct arch_container *acontainer = (struct arch_container *)container;

    mmu_map(acontainer->directory, &acontainer->table[0], 0x00000000, 0x00000000, 0x00400000, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);
    mmu_map(acontainer->directory, &acontainer->table[1], 0x00400000, 0x00400000, 0x00400000, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);
    mmu_map(acontainer->directory, &acontainer->table[2], 0x00800000, 0x00800000, 0x00400000, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);

}

static void taskmapcontainer(struct task *task, struct container *container)
{

    struct arch_task *atask = (struct arch_task *)task;
    struct arch_container *acontainer = (struct arch_container *)container;

    memory_copy(atask->directory, acontainer->directory, sizeof (struct mmu_directory));

}

static void taskmapcode(struct task *task, unsigned int address)
{

    struct arch_task *atask = (struct arch_task *)task;

    mmu_map(atask->directory, &atask->table[0], atask->mapping[0], address, CODESIZE, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE | MMU_TFLAG_USERMODE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE | MMU_PFLAG_USERMODE);
    mmu_map(atask->directory, &atask->table[1], atask->mapping[1], TASKSTACK - STACKSIZE, STACKSIZE, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE | MMU_TFLAG_USERMODE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE | MMU_PFLAG_USERMODE);

}

static void taskactivate(struct task *task)
{

    struct arch_task *atask = (struct arch_task *)task;

    mmu_setdirectory(atask->directory);

}

static void tasksave(struct task *task, struct cpu_general *general)
{

    struct arch_task *atask = (struct arch_task *)task;

    memory_copy(&atask->general, general, sizeof (struct cpu_general));

}

static void taskload(struct task *task, struct cpu_general *general)
{

    struct arch_task *atask = (struct arch_task *)task;

    memory_copy(general, &atask->general, sizeof (struct cpu_general));

}

static unsigned int spawn(struct container *container, struct task *task, void *stack)
{

    struct task *next = task_findinactive();

    if (!next)
        return 0;

    kernel_copytask(task, next);
    kernel_setuptask(next, TASKSTACK);
    task_setstatus(next, TASK_STATUS_ACTIVE);
    taskmapcontainer(next, container);

    return 1;

}

static unsigned int despawn(struct container *container, struct task *task, void *stack)
{

    task_setstatus(task, TASK_STATUS_INACTIVE);

    return 1;

}

void arch_setinterrupt(unsigned char index, void (*callback)())
{

    idt_setdescriptor(&idt.pointer, index, callback, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT);

}

void arch_setmap(unsigned char index, unsigned int paddress, unsigned int vaddress, unsigned int size)
{

    struct arch_container *acontainer = (struct arch_container *)current.container;

    mmu_map(acontainer->directory, &acontainer->table[index], paddress, vaddress, size, MMU_TFLAG_PRESENT | MMU_TFLAG_WRITEABLE, MMU_PFLAG_PRESENT | MMU_PFLAG_WRITEABLE);
    mmu_setdirectory(acontainer->directory);

}

unsigned short arch_schedule(struct cpu_general *general, struct cpu_interrupt *interrupt)
{

    if (current.task)
    {

        current.task->state.registers.ip = interrupt->eip;
        current.task->state.registers.sp = interrupt->esp;

        tasksave(current.task, general);

    }

    current.task = task_findactive();

    if (current.task)
    {

        if (current.task->state.status == TASK_STATUS_READY)
        {

            current.task->state.status = TASK_STATUS_ACTIVE;
            current.task->state.registers.ip -= 7;

        }

        interrupt->cs = selector.ucode;
        interrupt->ss = selector.udata;
        interrupt->eip = current.task->state.registers.ip;
        interrupt->esp = current.task->state.registers.sp;

        taskload(current.task, general);
        taskactivate(current.task);

    }

    else
    {

        interrupt->cs = selector.kcode;
        interrupt->ss = selector.kdata;
        interrupt->eip = (unsigned int)cpu_halt;
        interrupt->esp = KERNELSTACK;

    }

    return interrupt->ss;

}

unsigned short arch_generalfault(struct cpu_general general, unsigned int selector, struct cpu_interrupt interrupt)
{

    return arch_schedule(&general, &interrupt);

}

unsigned short arch_pagefault(struct cpu_general general, unsigned int type, struct cpu_interrupt interrupt)
{

    /*
    unsigned int address = cpu_getcr2();
    */

    if (current.task)
    {

        taskmapcode(current.task, 0x08048000);
        kernel_copyprogram(current.task);

    }

    return arch_schedule(&general, &interrupt);

}

unsigned short arch_syscall(struct cpu_general general, struct cpu_interrupt interrupt)
{

    general.eax = abi_call(general.eax, current.container, current.task, (void *)interrupt.esp);

    return arch_schedule(&general, &interrupt);

}

static struct container *setupcontainers()
{

    unsigned int index;

    for (index = 0; index < CONTAINERS; index++)
    {

        struct arch_container *container = &containers[index];

        container_init(&container->base);

        container->directory = (struct mmu_directory *)CONTAINERDIRECTORYBASE + index * CONTAINERDIRECTORYCOUNT;
        container->table = (struct mmu_table *)CONTAINERTABLEBASE + index * CONTAINERTABLECOUNT;

    }

    return &containers[0].base;

}

static struct task *setuptasks()
{

    unsigned int index;

    for (index = 0; index < TASKS; index++)
    {

        struct arch_task *task = &tasks[index];

        task_init(&task->base);

        task->directory = (struct mmu_directory *)TASKDIRECTORYBASE + index * TASKDIRECTORYCOUNT;
        task->table = (struct mmu_table *)TASKTABLEBASE + index * TASKTABLECOUNT;
        task->mapping[0] = CODEBASE + index * CODESIZE;
        task->mapping[1] = STACKBASE + index * STACKSIZE;

        task_register(&task->base);

    }

    return &tasks[0].base;

}

void arch_setup(struct vfs_backend *backend)
{

    struct cpu_interrupt interrupt;

    gdt_initpointer(&gdt.pointer, GDTDESCRIPTORS, gdt.descriptors);
    idt_initpointer(&idt.pointer, IDTDESCRIPTORS, idt.descriptors);
    tss_initpointer(&tss.pointer, TSSDESCRIPTORS, tss.descriptors);

    selector.kcode = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_KCODE, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW | GDT_ACCESS_EXECUTE, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.kdata = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_KSTACK, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.ucode = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_UCODE, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW | GDT_ACCESS_EXECUTE, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.udata = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_USTACK, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_ALWAYS1 | GDT_ACCESS_RW, GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT);
    selector.tlink = gdt_setdescriptor(&gdt.pointer, GDT_INDEX_TLINK, (unsigned long)tss.pointer.descriptors, (unsigned long)tss.pointer.descriptors + tss.pointer.limit, GDT_ACCESS_PRESENT | GDT_ACCESS_EXECUTE | GDT_ACCESS_ACCESSED, GDT_FLAG_32BIT);

    idt_setdescriptor(&idt.pointer, IDT_INDEX_GF, isr_generalfault, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT);
    idt_setdescriptor(&idt.pointer, IDT_INDEX_PF, isr_pagefault, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT);
    idt_setdescriptor(&idt.pointer, IDT_INDEX_SYSCALL, isr_syscall, selector.kcode, IDT_FLAG_PRESENT | IDT_FLAG_TYPE32INT | IDT_FLAG_RING3);
    tss_setdescriptor(&tss.pointer, TSS_INDEX_DEFAULT, selector.kdata, KERNELSTACK);
    cpu_setgdt(&gdt.pointer, selector.kcode, selector.kdata);
    cpu_setidt(&idt.pointer);
    cpu_settss(selector.tlink);
    kernel_setup();

    current.container = setupcontainers();
    current.task = setuptasks();

    kernel_setupramdisk(current.container, current.task, backend);
    kernel_copytask(current.task, current.task);
    kernel_setuptask(current.task, TASKSTACK);
    task_setstatus(current.task, TASK_STATUS_ACTIVE);
    containermapcode(current.container);
    taskmapcontainer(current.task, current.container);
    taskactivate(current.task);
    mmu_setup();
    abi_setup(spawn, despawn);

    interrupt.cs = selector.ucode;
    interrupt.ss = selector.udata;
    interrupt.eip = current.task->state.registers.ip;
    interrupt.esp = current.task->state.registers.sp;
    interrupt.eflags = cpu_geteflags() | 0x200;

    cpu_leave(interrupt);

}

