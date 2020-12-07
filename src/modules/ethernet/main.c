#include <fudge.h>
#include <net.h>
#include <kernel.h>
#include <modules/system/system.h>
#include "ethernet.h"

static struct system_node root;

static struct ethernet_interface *findinterface(void *address)
{

    struct resource *current = 0;

    while ((current = resource_foreachtype(current, RESOURCE_ETHERNETINTERFACE)))
    {

        struct ethernet_interface *interface = current->data;

        if (interface->matchaddress(address, ETHERNET_ADDRSIZE))
            return interface;

    }

    return 0;

}

void ethernet_send(void *buffer, unsigned int count)
{

    struct ethernet_header *header = buffer;
    struct ethernet_interface *interface = findinterface(header->sha);

    if (!interface)
        return;

    interface->send(buffer, count);

}

void ethernet_notify(struct ethernet_interface *interface, void *buffer, unsigned int count)
{

    kernel_notify(&interface->data.links, EVENT_DATA, buffer, count);

}

void ethernet_registerinterface(struct ethernet_interface *interface)
{

    resource_register(&interface->resource);
    system_addchild(&interface->root, &interface->ctrl);
    system_addchild(&interface->root, &interface->data);
    system_addchild(&interface->root, &interface->addr);
    system_addchild(&root, &interface->root);

}

void ethernet_unregisterinterface(struct ethernet_interface *interface)
{

    resource_unregister(&interface->resource);
    system_removechild(&interface->root, &interface->ctrl);
    system_removechild(&interface->root, &interface->data);
    system_removechild(&interface->root, &interface->addr);
    system_removechild(&root, &interface->root);

}

void ethernet_initinterface(struct ethernet_interface *interface, unsigned int id, unsigned int (*matchaddress)(void *buffer, unsigned int count), unsigned int (*send)(void *buffer, unsigned int count))
{

    resource_init(&interface->resource, RESOURCE_ETHERNETINTERFACE, interface);
    system_initnode(&interface->root, SYSTEM_NODETYPE_MULTIGROUP, "if");
    system_initnode(&interface->ctrl, SYSTEM_NODETYPE_NORMAL, "ctrl");
    system_initnode(&interface->data, SYSTEM_NODETYPE_NORMAL, "data");
    system_initnode(&interface->addr, SYSTEM_NODETYPE_NORMAL, "addr");

    interface->id = id;
    interface->matchaddress = matchaddress;
    interface->send = send;

}

void module_init(void)
{

    system_initnode(&root, SYSTEM_NODETYPE_GROUP, "ethernet");

}

void module_register(void)
{

    system_registernode(&root);

}

void module_unregister(void)
{

    system_unregisternode(&root);

}

