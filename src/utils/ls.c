#include <abi.h>
#include <fudge.h>

static unsigned int quit;

static void list(struct event_header *header, void *message, unsigned int descriptor, unsigned int session)
{

    struct record record;

    file_open(descriptor);

    while (file_readall(descriptor, &record, sizeof (struct record)))
    {

        event_addresponse(message, header, EVENT_DATA);
        event_adddata(message, session);
        event_appenddata(message, record.length, record.name);
        event_appenddata(message, 1, "\n");
        event_send(message);

        if (!file_step(descriptor))
            break;

    }

    file_close(descriptor);

}

static void onkill(struct event_header *header, void *message)
{

    event_addresponse(message, header, EVENT_EXIT);
    event_send(message);

    quit = 1;

}

static void onfile(struct event_header *header, void *message)
{

    struct event_file *file = event_getdata(header);

    if (file->descriptor)
        list(header, message, file->descriptor, file->session);
    else
        list(header, message, FILE_PW, file->session);

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

        case EVENT_FILE:
            onfile(header, message);

            break;

        }

    }

    event_close();

}

