#include <fudge.h>
#include <abi.h>

static void onstop(struct event_channel *channel)
{

    unsigned int id;

    if (!file_walk2(FILE_CP, "/bin/echo"))
        return;

    if (!file_walk2(FILE_C0, "/data/help.txt"))
        return;

    id = call_spawn();

    if (!id)
        return;

    event_forward(channel, EVENT_INIT);
    event_place(id, &channel->o);
    event_forward(channel, EVENT_FILE);
    event_addfile(&channel->o, FILE_P0);
    event_place(id, &channel->o);
    event_forward(channel, EVENT_STOP);
    event_place(id, &channel->o);

}

void main(void)
{

    struct event_channel channel;

    event_initsignals(&channel);
    event_setsignal(&channel, EVENT_STOP, onstop);
    event_listen(&channel);

}

