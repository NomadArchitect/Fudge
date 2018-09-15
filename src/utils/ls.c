#include <abi.h>
#include <fudge.h>
#include <event/base.h>

static unsigned int quit;

static void listall(struct event_header *header, unsigned int descriptor)
{

    struct record record;

    file_open(descriptor);

    while (file_readall(descriptor, &record, sizeof (struct record)))
    {

        event_senddata(FILE_L0, header->destination, header->source, record.length, record.name);
        event_senddata(FILE_L0, header->destination, header->source, 1, "\n");
        file_step(descriptor);

    }

    file_close(descriptor);

}

static void oninit(struct event_header *header, void *data)
{

    listall(header, FILE_PW);

}

static void onkill(struct event_header *header, void *data)
{

    quit = 1;

}

static void onfile(struct event_header *header, void *data)
{

    struct event_file *file = data;

    listall(header, file->num);

}

void main(void)
{

    if (!file_walk(FILE_L0, "/system/event"))
        return;

    file_open(FILE_L0);

    while (!quit)
    {

        struct event event;

        event_read(FILE_L0, &event);

        switch (event.header.type)
        {

        case EVENT_INIT:
            oninit(&event.header, event.data);

            break;

        case EVENT_EXIT:
        case EVENT_KILL:
            onkill(&event.header, event.data);

            break;

        case EVENT_FILE:
            onfile(&event.header, event.data);

            break;

        }

    }

    file_close(FILE_L0);

}

