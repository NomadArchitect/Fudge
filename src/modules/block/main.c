#include <fudge.h>
#include <kernel.h>
#include "block.h"

static unsigned int onblockrequest(struct block_interface *interface, unsigned int source, unsigned int count, void *data)
{

    struct event_blockrequest *blockrequest = data;

    return interface->onblockrequest(blockrequest->count, blockrequest->sector);

}

static unsigned int place(void *interface, unsigned int ichannel, unsigned int source, unsigned int event, unsigned int count, void *data)
{

    switch (event)
    {

    case EVENT_BLOCKREQUEST:
        return onblockrequest(interface, source, count, data);

    case EVENT_LINK:
        return kernel_link(ichannel, source);

    case EVENT_UNLINK:
        return kernel_unlink(ichannel, source);

    }

    return MESSAGE_UNIMPLEMENTED;

}

void block_notifyblockresponse(struct block_interface *interface, void *buffer, unsigned int count)
{

    kernel_notify(interface->ichannel, EVENT_BLOCKRESPONSE, count, buffer);

}

void block_registerinterface(struct block_interface *interface)
{

    resource_register(&interface->resource);
    kernel_announce(interface->ichannel, interface, 0, place);

}

void block_unregisterinterface(struct block_interface *interface)
{

    resource_unregister(&interface->resource);
    kernel_unannounce(interface->ichannel);

}

void block_initinterface(struct block_interface *interface, unsigned int id, unsigned int ichannel, unsigned int (*onblockrequest)(unsigned int count, unsigned int sector))
{

    resource_init(&interface->resource, RESOURCE_BLOCKINTERFACE, interface);

    interface->id = id;
    interface->ichannel = ichannel;
    interface->onblockrequest = onblockrequest;

}

