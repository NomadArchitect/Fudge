#include <fudge.h>
#include <kernel.h>
#include <modules/system/system.h>
#include "keyboard.h"

static struct system_node root;
static struct system_node event;

void keyboard_notify(struct keyboard_interface *interface, void *buffer, unsigned int count)
{

    union event_message message;

    event_create(&message, EVENT_DATA, EVENT_BROADCAST, EVENT_BROADCAST, 0);
    event_append(&message, count, buffer);
    kernel_multicast(&interface->data.states, &message);

}

void keyboard_notifypress(struct keyboard_interface *interface, unsigned char scancode)
{

    union event_message message;

    event_create(&message, EVENT_KEYPRESS, EVENT_BROADCAST, EVENT_BROADCAST, 0);
    event_addkeypress(&message, scancode);
    kernel_multicast(&event.states, &message);
    kernel_multicast(&interface->event.states, &message);

}

void keyboard_notifyrelease(struct keyboard_interface *interface, unsigned char scancode)
{

    union event_message message;

    event_create(&message, EVENT_KEYRELEASE, EVENT_BROADCAST, EVENT_BROADCAST, 0);
    event_addkeyrelease(&message, scancode);
    kernel_multicast(&event.states, &message);
    kernel_multicast(&interface->event.states, &message);

}

void keyboard_registerinterface(struct keyboard_interface *interface)
{

    resource_register(&interface->resource);
    system_addchild(&interface->root, &interface->data);
    system_addchild(&interface->root, &interface->event);
    system_addchild(&root, &interface->root);

}

void keyboard_unregisterinterface(struct keyboard_interface *interface)
{

    resource_unregister(&interface->resource);
    system_removechild(&interface->root, &interface->data);
    system_removechild(&interface->root, &interface->event);
    system_removechild(&root, &interface->root);

}

void keyboard_initinterface(struct keyboard_interface *interface, unsigned int id)
{

    resource_init(&interface->resource, RESOURCE_KEYBOARDINTERFACE, interface);
    system_initnode(&interface->root, SYSTEM_NODETYPE_MULTIGROUP, "if");
    system_initnode(&interface->data, SYSTEM_NODETYPE_NORMAL, "data");
    system_initnode(&interface->event, SYSTEM_NODETYPE_NORMAL, "event");

    interface->id = id;

}

void module_init(void)
{

    system_initnode(&root, SYSTEM_NODETYPE_GROUP, "keyboard");
    system_initnode(&event, SYSTEM_NODETYPE_NORMAL, "event");

}

void module_register(void)
{

    system_registernode(&root);
    system_addchild(&root, &event);

}

void module_unregister(void)
{

    system_unregisternode(&root);
    system_removechild(&root, &event);

}

