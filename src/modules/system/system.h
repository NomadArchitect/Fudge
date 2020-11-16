#define SYSTEM_NODETYPE_NORMAL          0
#define SYSTEM_NODETYPE_GROUP           1
#define SYSTEM_NODETYPE_MULTIGROUP      2

struct system_operations
{

    unsigned int (*read)(void *buffer, unsigned int count, unsigned int offset);
    unsigned int (*write)(struct service_link *link, void *buffer, unsigned int count, unsigned int offset);
    unsigned int (*seek)(unsigned int offset);

};

struct system_node
{

    char *name;
    unsigned int type;
    unsigned int index;
    struct system_node *parent;
    struct system_operations operations;
    struct list_item item;
    struct list children;
    struct list links;

};

void system_addchild(struct system_node *group, struct system_node *node);
void system_removechild(struct system_node *group, struct system_node *node);
void system_movechild(struct system_node *to, struct system_node *from, struct system_node *node);
void system_registernode(struct system_node *node);
void system_unregisternode(struct system_node *node);
void system_initnode(struct system_node *node, unsigned int type, char *name);
