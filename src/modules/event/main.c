#include <fudge.h>
#include <kernel.h>
#include <modules/system/system.h>
#include "event.h"

static struct system_node root;

void event_unicast(struct list *links, struct event_header *header, unsigned int count)
{

    struct list_item *current;

    for (current = links->head; current; current = current->next)
    {

        struct service_state *state = current->data;

        if (header->destination != (unsigned int)state->task)
            continue;

        kernel_writetask(state->task, header, count);

    }

}

void event_multicast(struct list *links, struct event_header *header, unsigned int count)
{

    struct list_item *current;

    for (current = links->head; current; current = current->next)
    {

        struct service_state *state = current->data;

        header->destination = (unsigned int)state->task;

        kernel_writetask(state->task, header, count);

    }

}

unsigned int event_send(struct list *links, struct service_state *state, void *buffer, unsigned int count)
{

    struct event_header *header = buffer;

    header->source = (unsigned int)state->task;

    if (header->destination)
        event_unicast(links, header, count);
    else
        event_multicast(links, header, count);

    return count;

}

void module_init(void)
{

    system_initnode(&root, SYSTEM_NODETYPE_NORMAL, "event");

    root.read = system_readtask;

}

void module_register(void)
{

    system_registernode(&root);

}

void module_unregister(void)
{

    system_unregisternode(&root);

}

