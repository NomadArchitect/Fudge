#include <fudge.h>
#include <abi.h>

static void onpath(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    if (file_walk2(FILE_G0, mdata))
        call_unload(FILE_G0);

}

void init(struct channel *channel)
{

    channel_setcallback(channel, EVENT_PATH, onpath);

}

