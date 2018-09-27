#include <abi.h>
#include <fudge.h>

static unsigned int quit;

static void onkill(struct event_header *header, void *message)
{

    event_addresponse(message, header, EVENT_EXIT);
    event_send(message);

    quit = 1;

}

static void ondata(struct event_header *header, void *message)
{

    struct event_data *data = event_getdata(header);

    event_addresponse(message, header, EVENT_DATA);
    event_adddata(message, data->session);
    event_appenddata(message, data->count, data + 1);
    event_send(message);

}

static void onfile(struct event_header *header, void *message)
{

    struct event_file *file = event_getdata(header);
    char buffer[FUDGE_BSIZE];
    unsigned int count;

    if (!file->descriptor)
        return;

    file_open(file->descriptor);

    while ((count = file_read(file->descriptor, buffer, FUDGE_BSIZE - sizeof (struct event_header) - sizeof (struct event_data))))
    {

        event_addresponse(message, header, EVENT_DATA);
        event_adddata(message, file->session);
        event_appenddata(message, count, buffer);
        event_send(message);

    }

    file_close(file->descriptor);

}

void main(void)
{

    event_open();

    while (!quit)
    {

        char data[FUDGE_BSIZE];
        char message[FUDGE_BSIZE];
        struct event_header *header = event_read(data);

        switch (header->type)
        {

        case EVENT_EXIT:
        case EVENT_KILL:
            onkill(header, message);

            break;

        case EVENT_DATA:
            ondata(header, message);

            break;

        case EVENT_FILE:
            onfile(header, message);

            break;

        }

    }

    event_close();

}

