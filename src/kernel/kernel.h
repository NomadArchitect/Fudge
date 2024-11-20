#define KERNEL_TASKS                    64
#define KERNEL_NODES                    1024
#define KERNEL_MAILBOXES                512

void *kernel_getnodeinterface(unsigned int inode);
struct task_thread *kernel_gettaskthread(unsigned int itask);
struct core *kernel_getcore(void);
unsigned int kernel_link(struct list *nodes, struct mailbox *mailbox, struct resource *resource, unsigned int (*place)(unsigned int source, unsigned int target, unsigned int event, unsigned int count, void *data));
unsigned int kernel_unlink(struct list *nodes, unsigned int inode);
unsigned int kernel_schedule(struct core *core);
unsigned int kernel_codebase(unsigned int itask, unsigned int address);
unsigned int kernel_loadprogram(unsigned int itask);
void kernel_signal(unsigned int itask, unsigned int signal);
unsigned int kernel_pick(unsigned int itask, unsigned int index, struct message *message, unsigned int count, void *data);
unsigned int kernel_place(unsigned int source, unsigned int target, unsigned int event, unsigned int count, void *data);
unsigned int kernel_placetask(unsigned int itask, unsigned int index, unsigned int target, unsigned int event, unsigned int count, void *data);
void kernel_notify(unsigned int source, unsigned int event, unsigned int count, void *data);
unsigned int kernel_createtask(void);
unsigned int kernel_loadtask(unsigned int itask, unsigned int ntask, unsigned int ip, unsigned int sp, unsigned int address);
void kernel_setcallback(struct core *(*get)(void), void (*assign)(struct list_item *item));
void kernel_setup(unsigned int saddress, unsigned int ssize, unsigned int mbaddress, unsigned int mbsize);
