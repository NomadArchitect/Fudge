#define RUNTIME_TASK_DESCRIPTOR_SLOTS   16
#define RUNTIME_TASK_MOUNT_SLOTS        32
#define RUNTIME_TASK_ADDRESS_SIZE       0x00010000
#define RUNTIME_TASK_PADDRESS_BASE      0x00400000
#define RUNTIME_TASK_VADDRESS_BASE      0x08048000
#define RUNTIME_STACK_PADDRESS_BASE     0x00810000
#define RUNTIME_STACK_VADDRESS_BASE     0x80000000
#define RUNTIME_STACK_ADDRESS_SIZE      0x00010000

struct runtime_descriptor
{

    struct vfs_interface *interface;
    unsigned int id;

};

struct runtime_mount
{

    struct runtime_descriptor parent;
    struct runtime_descriptor child;

};

struct runtime_registers
{

    unsigned int ip;
    unsigned int sp;
    unsigned int sb;
    unsigned int status;

};

struct runtime_status
{

    unsigned int used;
    unsigned int idle;

};

struct runtime_task
{

    struct runtime_status status;
    struct runtime_registers registers;
    struct runtime_descriptor descriptors[RUNTIME_TASK_DESCRIPTOR_SLOTS];
    struct runtime_mount mounts[RUNTIME_TASK_MOUNT_SLOTS];
    struct runtime_task *(*notify_interrupt)(struct runtime_task *self, unsigned int index);

};

struct runtime_descriptor *runtime_get_task_descriptor(struct runtime_task *task, unsigned int index);
struct runtime_mount *runtime_get_task_mount(struct runtime_task *task, unsigned int index);
unsigned int runtime_update_task_descriptor(struct runtime_task *task, struct runtime_descriptor *descriptor, unsigned int count, char *path);
void runtime_init_descriptor(struct runtime_descriptor *descriptor, struct vfs_interface *interface, unsigned int id);
void runtime_init_mount(struct runtime_mount *mount, struct vfs_interface *parent, unsigned int parentid, struct vfs_interface *child, unsigned int childid);
void runtime_init_registers(struct runtime_registers *registers, unsigned int ip, unsigned int sp, unsigned int sb, unsigned int status);
void runtime_init_task(struct runtime_task *task);
