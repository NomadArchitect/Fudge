#include <fudge.h>
#include <abi.h>

static void ondata(unsigned int source, void *mdata, unsigned int msize)
{

    channel_send_buffer(CHANNEL_DEFAULT, EVENT_DATA, msize, mdata);

}

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    unsigned int service = fsp_auth(option_getstring("echo"));
    unsigned int id = fsp_walk(service, 0, option_getstring("echo"));

    if (id)
    {

        unsigned int channel = call_spawn(service, id);

        if (channel)
        {

            channel_listen(channel, EVENT_DATA);
            channel_listen(channel, EVENT_TERMRESPONSE);
            channel_send_fmt0(channel, EVENT_PATH, "/data/help.txt\\0");
            channel_send(channel, EVENT_MAIN);
            channel_wait_from(channel, EVENT_TERMRESPONSE);

        }

        else
        {

            channel_send_fmt0(CHANNEL_DEFAULT, EVENT_ERROR, "Could not spawn process\n");

        }

    }

    else
    {

        channel_send_fmt1(CHANNEL_DEFAULT, EVENT_ERROR, "Program not found: %s\n", option_getstring("echo"));

    }

}

void init(void)
{

    option_add("echo", "initrd:bin/echo");
    channel_bind(EVENT_DATA, ondata);
    channel_bind(EVENT_MAIN, onmain);

}

