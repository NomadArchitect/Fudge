#include <fudge.h>
#include <abi.h>

static unsigned int bytes;
static unsigned int words;
static unsigned int lines;
static unsigned int whitespace = 1;

static void sum(unsigned int count, void *buffer)
{

    char *data = buffer;
    unsigned int i;

    for (i = 0; i < count; i++)
    {

        switch (data[i])
        {

        case '\n':
            whitespace = 1;
            lines++;

            break;

        case ' ':
            whitespace = 1;

            break;

        default:
            if (whitespace)
                words++;

            whitespace = 0;

            break;

        }

        bytes++;

    }

}

static void ondata(unsigned int source, void *mdata, unsigned int msize)
{

    sum(msize, mdata);

}

static void onexit(unsigned int source, void *mdata, unsigned int msize)
{

    channel_send_fmt3(0, source, EVENT_DATA, "%u\n%u\n%u\n", &lines, &words, &bytes);

}

static void onpath(unsigned int source, void *mdata, unsigned int msize)
{

    unsigned int target = fs_auth(mdata);

    if (target)
    {

        unsigned int id = fs_walk(1, target, 0, mdata);

        if (id)
        {

            char buffer[4096];
            unsigned int count;
            unsigned int offset;

            for (offset = 0; (count = fs_read(1, target, id, buffer, 4096, offset)); offset += count)
                sum(count, buffer);

        }

        else
        {

            channel_send_fmt1(0, source, EVENT_ERROR, "Path not found: %s\n", mdata);

        }

    }

}

void init(void)
{

    channel_bind(EVENT_DATA, ondata);
    channel_bind(EVENT_EXIT, onexit);
    channel_bind(EVENT_PATH, onpath);

}

