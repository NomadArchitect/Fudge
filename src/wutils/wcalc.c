#include <fudge.h>
#include <abi.h>

#define STATE_SUM                       0
#define STATE_ADD                       1
#define STATE_SUB                       2
#define STATE_MUL                       3
#define STATE_DIV                       4

static unsigned int prevstate;
static int number;
static int accumulator;

static void refresh(int value)
{

    channel_send_fmt1(CHANNEL_DEFAULT, EVENT_WMRENDERDATA, "= result content \"%i\"\n", &value);

}

static void updatestate(int state)
{

    switch (prevstate)
    {

    case STATE_SUM:
        accumulator = number;
        number = 0;

        break;

    case STATE_ADD:
        accumulator += number;
        number = 0;

        break;

    case STATE_SUB:
        accumulator -= number;
        number = 0;

        break;

    case STATE_MUL:
        accumulator *= number;
        number = 0;

        break;

    case STATE_DIV:
        accumulator /= number;
        number = 0;

        break;

    }

    prevstate = state;

    switch (state)
    {

    case STATE_SUM:
        refresh(accumulator);

        break;

    }

}

static void updatevalue(int value)
{

    number *= 10;
    number += value;

    refresh(number);

}

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    if (!call_walk_absolute(FILE_L0, option_getstring("wm-service")))
        PANIC();

    call_notify(FILE_L0, EVENT_WMMAP, 0, 0);

}

static void onterm(unsigned int source, void *mdata, unsigned int msize)
{

    channel_send(CHANNEL_DEFAULT, EVENT_WMUNMAP);

}

static void onwmevent(unsigned int source, void *mdata, unsigned int msize)
{

    char *data = (char *)mdata + sizeof (struct event_wmevent);

    if (cstring_match_word(data, 0, "val"))
        updatevalue(cstring_read_value(cstring_get_word(data, 1), 1, 10));
    else if (cstring_match_word(data, 0, "sum"))
        updatestate(STATE_SUM);
    else if (cstring_match_word(data, 0, "add"))
        updatestate(STATE_ADD);
    else if (cstring_match_word(data, 0, "sub"))
        updatestate(STATE_SUB);
    else if (cstring_match_word(data, 0, "mul"))
        updatestate(STATE_MUL);
    else if (cstring_match_word(data, 0, "div"))
        updatestate(STATE_DIV);

}

static void onwminit(unsigned int source, void *mdata, unsigned int msize)
{

    char *data0 =
        "+ window id \"window\" title \"Calculator\"\n"
        "  + layout id \"base\" in \"window\" flow \"vertical\" padding \"1\"\n"
        "    + layout id \"display\" in \"base\" flow \"horizontal\"\n"
        "      + textbox id \"output\" in \"display\" mode \"readonly\" span \"1\"\n"
        "        + text id \"result\" in \"output\" content \"0\" halign \"right\"\n";
    char *data1 =
        "    + layout id \"buttons\" in \"base\" flow \"vertical\" span \"1\"\n"
        "      + layout id \"row1\" in \"buttons\" flow \"horizontal-stretch\" span \"1\"\n"
        "        + button in \"row1\" label \"7\" span \"1\" onclick \"val 7\"\n"
        "        + button in \"row1\" label \"8\" span \"1\" onclick \"val 8\"\n"
        "        + button in \"row1\" label \"9\" span \"1\" onclick \"val 9\"\n"
        "        + button in \"row1\" label \"/\" span \"1\" onclick \"div\"\n"
        "      + layout id \"row2\" in \"buttons\" flow \"horizontal-stretch\" span \"1\"\n"
        "        + button in \"row2\" label \"4\" span \"1\" onclick \"val 4\"\n"
        "        + button in \"row2\" label \"5\" span \"1\" onclick \"val 5\"\n"
        "        + button in \"row2\" label \"6\" span \"1\" onclick \"val 6\"\n"
        "        + button in \"row2\" label \"x\" span \"1\" onclick \"mul\"\n"
        "      + layout id \"row3\" in \"buttons\" flow \"horizontal-stretch\" span \"1\"\n"
        "        + button in \"row3\" label \"3\" span \"1\" onclick \"val 3\"\n"
        "        + button in \"row3\" label \"2\" span \"1\" onclick \"val 2\"\n"
        "        + button in \"row3\" label \"1\" span \"1\" onclick \"val 1\"\n"
        "        + button in \"row3\" label \"-\" span \"1\" onclick \"sub\"\n"
        "      + layout id \"row4\" in \"buttons\" flow \"horizontal-stretch\" span \"1\"\n"
        "        + button in \"row4\" label \"0\" span \"1\" onclick \"val 0\"\n"
        "        + button in \"row4\" label \".\" span \"1\" onclick \"dot\"\n"
        "        + button in \"row4\" label \"+\" span \"1\" onclick \"add\"\n"
        "        + button in \"row4\" label \"=\" span \"1\" onclick \"sum\"\n";

    channel_send_fmt0(CHANNEL_DEFAULT, EVENT_WMRENDERDATA, data0);
    channel_send_fmt0(CHANNEL_DEFAULT, EVENT_WMRENDERDATA, data1);

}

static void onwmkeypress(unsigned int source, void *mdata, unsigned int msize)
{

    struct event_wmkeypress *wmkeypress = mdata;

    switch (wmkeypress->id)
    {

    case KEYS_KEY_0:
    case KEYS_KEY_KEYPAD_0:
        updatevalue(0);

        break;

    case KEYS_KEY_1:
    case KEYS_KEY_KEYPAD_1:
        updatevalue(1);

        break;

    case KEYS_KEY_2:
    case KEYS_KEY_KEYPAD_2:
        updatevalue(2);

        break;

    case KEYS_KEY_3:
    case KEYS_KEY_KEYPAD_3:
        updatevalue(3);

        break;

    case KEYS_KEY_4:
    case KEYS_KEY_KEYPAD_4:
        updatevalue(4);

        break;

    case KEYS_KEY_5:
    case KEYS_KEY_KEYPAD_5:
        updatevalue(5);

        break;

    case KEYS_KEY_6:
    case KEYS_KEY_KEYPAD_6:
        updatevalue(6);

        break;

    case KEYS_KEY_7:
    case KEYS_KEY_KEYPAD_7:
        updatevalue(7);

        break;

    case KEYS_KEY_8:
        if (wmkeypress->keymod & KEYS_MOD_SHIFT)
            updatestate(STATE_MUL);
        else
            updatevalue(8);

        break;

    case KEYS_KEY_KEYPAD_8:
        updatevalue(8);

        break;

    case KEYS_KEY_9:
    case KEYS_KEY_KEYPAD_9:
        updatevalue(9);

        break;

    case KEYS_KEY_MINUS:
    case KEYS_KEY_KEYPAD_MINUS:
        updatestate(STATE_SUB);

        break;

    case KEYS_KEY_EQUAL:
        if (wmkeypress->keymod & KEYS_MOD_SHIFT)
            updatestate(STATE_ADD);
        else
            updatestate(STATE_SUM);

        break;

    case KEYS_KEY_KEYPAD_PLUS:
        updatestate(STATE_ADD);

        break;

    case KEYS_KEY_ENTER:
    case KEYS_KEY_KEYPAD_ENTER:
        updatestate(STATE_SUM);

        break;

    case KEYS_KEY_SLASH:
    case KEYS_KEY_KEYPAD_SLASH:
        updatestate(STATE_DIV);

        break;

    }

}

void init(void)
{

    option_add("wm-service", "system:service/wm");
    channel_autoclose(EVENT_MAIN, 0);
    channel_bind(EVENT_MAIN, onmain);
    channel_bind(EVENT_TERM, onterm);
    channel_bind(EVENT_WMEVENT, onwmevent);
    channel_bind(EVENT_WMINIT, onwminit);
    channel_bind(EVENT_WMKEYPRESS, onwmkeypress);

}

