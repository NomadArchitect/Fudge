#include <fudge.h>
#include <abi.h>

static void ondata(unsigned int source, void *mdata, unsigned int msize)
{

    channel_sendbuffer(CHANNEL_DEFAULT, EVENT_DATA, msize, mdata);

}

static void onpath(unsigned int source, void *mdata, unsigned int msize)
{

    char buffer[MESSAGE_SIZE];
    unsigned int count;

    if (!file_walk2(FILE_L0, mdata))
        channel_error("File not found");

    while ((count = file_read(FILE_L0, buffer, MESSAGE_SIZE)))
        channel_sendbuffer(CHANNEL_DEFAULT, EVENT_DATA, count, buffer);

}

void init(void)
{

    channel_bind(EVENT_DATA, ondata);
    channel_bind(EVENT_PATH, onpath);

}

