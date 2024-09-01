#include <fudge.h>
#include <abi.h>
#include <image.h>

static void onpath(unsigned int source, void *mdata, unsigned int msize)
{

    unsigned int service = fsp_auth(mdata);
    unsigned int id = fsp_walk(service, 0, mdata);

    if (id)
    {

        struct pcx_header header;
        struct record record;
        unsigned char magic;

        fsp_stat(service, id, &record);
        fsp_read_all(service, id, &header, sizeof (struct pcx_header), 0);
        fsp_read_all(service, id, &magic, 1, record.size - 768 - 1);

        if (magic == PCX_COLORMAP_MAGIC)
        {

            unsigned char colormap[768];

            fsp_read_all(service, id, colormap, 768, record.size - 768);
            channel_send_buffer(source, EVENT_DATA, 768, colormap);

        }

    }

    else
    {

        channel_send_fmt1(source, EVENT_ERROR, "Path not found: %s\n", mdata);

    }

}

void init(void)
{

    channel_bind(EVENT_PATH, onpath);

}

