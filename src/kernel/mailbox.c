#include <fudge.h>
#include "resource.h"
#include "mailbox.h"

unsigned int mailbox_pick(struct mailbox *mailbox, struct message *message, unsigned int count, void *data)
{

    unsigned int length = 0;

    spinlock_acquire(&mailbox->spinlock);

    if (ring_count(&mailbox->ring))
    {

        length += ring_readall(&mailbox->ring, message, sizeof (struct message));

        if (message_datasize(message) <= count)
        {

            length += ring_readall(&mailbox->ring, data, message_datasize(message));

        }

        else
        {

            message_dropdata(message);
            ring_skip(&mailbox->ring, message_datasize(message));

        }

    }

    spinlock_release(&mailbox->spinlock);

    return length;

}

unsigned int mailbox_place(struct mailbox *mailbox, struct message *message, void *data)
{

    unsigned int length = 0;

    spinlock_acquire(&mailbox->spinlock);

    if (ring_avail(&mailbox->ring) > message->length)
    {

        length += ring_writeall(&mailbox->ring, message, sizeof (struct message));
        length += ring_writeall(&mailbox->ring, data, message_datasize(message));

    }

    spinlock_release(&mailbox->spinlock);

    return length;

}

void mailbox_reset(struct mailbox *mailbox)
{

    spinlock_acquire(&mailbox->spinlock);
    ring_reset(&mailbox->ring);
    spinlock_release(&mailbox->spinlock);

}

void mailbox_register(struct mailbox *mailbox)
{

    resource_register(&mailbox->resource);

}

void mailbox_unregister(struct mailbox *mailbox)
{

    resource_unregister(&mailbox->resource);

}

void mailbox_init(struct mailbox *mailbox, void *buffer, unsigned int count)
{

    resource_init(&mailbox->resource, RESOURCE_MAILBOX, mailbox);
    ring_init(&mailbox->ring, count, buffer);
    spinlock_init(&mailbox->spinlock);

}

