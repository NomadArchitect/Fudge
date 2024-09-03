#define KERNEL_TASKS                    64
#define KERNEL_LINKS                    64
#define KERNEL_MAILBOXES                64
#define KERNEL_DESCRIPTORS              32
#define KERNEL_CHANNELS                 0x4000

struct core *kernel_getcore(void);
void kernel_setcallback(struct core *(*get)(void), void (*assign)(struct list_item *item));
void kernel_link(struct list *list, unsigned int target, unsigned int source);
void kernel_link2(unsigned int ichannel, unsigned int target, unsigned int source);
void kernel_unlink(struct list *list, unsigned int target);
void kernel_unlink2(unsigned int ichannel, unsigned int target);
unsigned int kernel_schedule(struct core *core);
unsigned int kernel_codebase(unsigned int itask, unsigned int address);
unsigned int kernel_loadprogram(unsigned int itask);
void kernel_signal(unsigned int itask, unsigned int signal);
struct task_thread *kernel_getthread(unsigned int itask);
unsigned int kernel_pick(unsigned int source, struct message *message, unsigned int count, void *data);
unsigned int kernel_place(unsigned int source, unsigned int ichannel, unsigned int event, unsigned int count, void *data);
void kernel_announce(unsigned short index, unsigned int target, unsigned int (*place)(unsigned int target, unsigned int source, unsigned int event, unsigned int count, void *data));
void kernel_notify(struct list *states, unsigned int event, unsigned int count, void *data);
void kernel_notify2(unsigned int ichannel, unsigned int event, unsigned int count, void *data);
unsigned int kernel_createtask(void);
unsigned int kernel_loadtask(unsigned int itask, unsigned int ip, unsigned int sp, unsigned int address);
void kernel_setup(unsigned int saddress, unsigned int ssize, unsigned int mbaddress, unsigned int mbsize);
