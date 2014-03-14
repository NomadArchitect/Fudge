#include <fudge.h>

void main()
{

    unsigned char buffer[32];
    unsigned int count = call_read(CALL_I0, 0, 32, buffer);

    if (!count)
        return;

    if (call_walk(CALL_L0, CALL_DR, 18, "system/timer/clone"))
    {

        call_open(CALL_L0);

        if (call_walk(CALL_L1, CALL_L0, 8, "../sleep"))
        {

            call_open(CALL_L1);
            call_write(CALL_L1, 0, count, buffer);
            call_read(CALL_L1, 0, 32, buffer);
            call_close(CALL_L1);

        }

        call_close(CALL_L0);

    }

}

