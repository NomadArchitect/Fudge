#include <abi.h>
#include <fudge.h>

static unsigned int quit;

static void dump(struct event_header *header, unsigned int count, void *buffer, unsigned int stream)
{

    char *data = buffer;
    unsigned int i;

    for (i = 0; i < count; i++)
    {

        char message[FUDGE_BSIZE];
        unsigned char num[FUDGE_NSIZE];
        unsigned int cnum = ascii_wzerovalue(num, FUDGE_NSIZE, data[i], 16, 2, 0);

        event_addresponse(message, header, EVENT_DATA);
        event_adddata(message, stream, cnum, num);
        event_send(message);
        event_addresponse(message, header, EVENT_DATA);
        event_adddata(message, stream, 2, "  ");
        event_send(message);

    }

}

static void ondata(struct event_header *header)
{

    struct event_data *data = event_getdata(header);

    dump(header, data->count, data + 1, data->stream);

}

static void onfile(struct event_header *header)
{

    struct event_file *file = event_getdata(header);
    char buffer[FUDGE_BSIZE];
    unsigned int count;

    if (!file->descriptor)
        return;

    file_open(file->descriptor);

    while ((count = file_read(file->descriptor, buffer, FUDGE_BSIZE)))
        dump(header, count, buffer, 0);

    file_close(file->descriptor);

}

static void onkill(struct event_header *header)
{

    char message[FUDGE_BSIZE];

    event_addresponse(message, header, EVENT_CHILD);
    event_send(message);

    quit = 1;

}

void main(void)
{

    event_open();

    while (!quit)
    {

        char data[FUDGE_BSIZE];
        struct event_header *header = event_read(data);

        switch (header->type)
        {

        case EVENT_EXIT:
        case EVENT_KILL:
            onkill(header);

            break;

        case EVENT_DATA:
            ondata(header);

            break;

        case EVENT_FILE:
            onfile(header);

            break;

        }

    }

    event_close();

}

