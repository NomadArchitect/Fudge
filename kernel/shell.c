#include <lib/call.h>
#include <lib/file.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <kernel/shell.h>

static char shellBuffer[SHELL_BUFFER_SIZE];
static unsigned int shellBufferHead;

static void shell_stack_push(char c)
{

    if (shellBufferHead < SHELL_BUFFER_SIZE)
    {

        shellBuffer[shellBufferHead] = c;
        shellBufferHead++;

    }

}

static char shell_stack_pop()
{

    return (shellBufferHead > 0) ? shellBuffer[--shellBufferHead] : 0;

}

static void shell_stack_clear()
{

    shellBufferHead = 0;

}

static void shell_clear()
{

    file_write_string2(FILE_STDOUT, "fudge:/$ ");
    shell_stack_clear();

}

static void shell_call(struct file_node *node, int argc, char *argv[])
{

    void *buffer = (void *)0x00300000;
    file_read(node, 0, node->length, buffer);

    unsigned int address = call_map((unsigned int)buffer);

    void (*func)(int argc, char **argv) = (void (*)(int argc, char **argv))address;

    func(argc, argv);

}

static void shell_interpret(char *command)
{

    char *argv[32];
    unsigned int argc = string_split(argv, command, ' ');

    if (argc)
    {

        struct file_node *initrd = call_open("/bin");
        struct file_node *node = file_find(initrd, argv[0]);

        if (node)
        {

            shell_call(node, argc, argv);

        }

        else
        {

            file_write_string2(FILE_STDOUT, argv[0]);
            file_write_string2(FILE_STDOUT, ": Command not found\n");

        }

    }

    shell_clear();

}

static void shell_handle_input(char c)
{

    switch (c)
    {

        case '\t':

            break;

        case '\b':

            if (shell_stack_pop())
            {

                file_write_byte2(FILE_STDOUT, '\b');
                file_write_byte2(FILE_STDOUT, ' ');
                file_write_byte2(FILE_STDOUT, '\b');

             }

            break;

        case '\n':

            shell_stack_push('\0');
            file_write_byte2(FILE_STDOUT, c);
            shell_interpret(shellBuffer);

            break;

        default:

            shell_stack_push(c);
            file_write_byte2(FILE_STDOUT, c);

            break;

    }

}

static void shell_poll()
{

    char c;

    for (;;)
    {

        while (!call_read(FILE_STDIN, &c, 1));

        shell_handle_input(c);

    }

}

void shell_init()
{

    shellBufferHead = 0;

    int sin = call_open2("/dev/kbd");
    int sout = call_open2("/dev/tty");

    file_write_string2(FILE_STDOUT, "Fudge\n\n");
    file_write_string2(FILE_STDOUT, "Copyright (c) 2009 Jens Nyberg\n");
    file_write_string2(FILE_STDOUT, "Type 'cat help.txt' to read the help section.\n\n");

    shell_clear();
    shell_poll();

    call_close(sin);
    call_close(sout);

}

