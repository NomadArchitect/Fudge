#include <fudge.h>
#include "resource.h"
#include "service.h"

unsigned int service_checkdescriptor(struct service_descriptor *descriptor)
{

    return descriptor->protocol && descriptor->id;

}

struct service_protocol *service_findprotocol(unsigned int count, char *name)
{

    struct resource *current = 0;

    while ((current = resource_foreachtype(current, RESOURCE_SERVICEPROTOCOL)))
    {

        struct service_protocol *protocol = current->data;

        if (buffer_match(protocol->name, name, count))
            return protocol;

    }

    return 0;

}

void service_initprotocol(struct service_protocol *protocol, char *name, unsigned int (*root)(void), unsigned int (*parent)(unsigned int id), unsigned int (*child)(unsigned int id, char *path, unsigned int length), unsigned int (*create)(unsigned int id, char *name, unsigned int length), unsigned int (*destroy)(unsigned int id), unsigned int (*step)(unsigned int id, unsigned int current), unsigned int (*read)(unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset), unsigned int (*write)(unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset), unsigned int (*seek)(unsigned int id, unsigned int offset), unsigned int (*map)(unsigned int id), unsigned int (*link)(unsigned int id, unsigned int source), unsigned int (*unlink)(unsigned int id, unsigned int source), unsigned int (*notify)(unsigned int id, unsigned int source, unsigned int event, unsigned int count, void *data))
{

    resource_init(&protocol->resource, RESOURCE_SERVICEPROTOCOL, protocol);

    protocol->name = name;
    protocol->root = root;
    protocol->parent = parent;
    protocol->child = child;
    protocol->create = create;
    protocol->destroy = destroy;
    protocol->step = step;
    protocol->read = read;
    protocol->write = write;
    protocol->seek = seek;
    protocol->map = map;
    protocol->link = link;
    protocol->unlink = unlink;
    protocol->notify = notify;

}

