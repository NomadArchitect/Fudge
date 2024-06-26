union cpu_register
{

    unsigned int value;
    void *reference;

};

struct cpu_general
{

    union cpu_register r0;
    union cpu_register r1;
    union cpu_register r2;
    union cpu_register r3;
    union cpu_register r4;
    union cpu_register r5;
    union cpu_register r6;
    union cpu_register r7;
    union cpu_register r8;
    union cpu_register r9;
    union cpu_register r10;
    union cpu_register r11;
    union cpu_register r12;

};

struct cpu_interrupt
{

    union cpu_register sp;
    union cpu_register pc;

};

unsigned int cpu_get_cpsr(void);
void cpu_set_cpsr(unsigned int value);
void cpu_disable_interrupts(void);
void cpu_enable_interrupts(void);
void cpu_leave(unsigned int sp, unsigned int pc);
void cpu_halt(void);
void ivt_enable(void);
