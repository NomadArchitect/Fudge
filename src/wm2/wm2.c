#include <fudge.h>
#include <abi.h>
#include "util.h"
#include "widget.h"
#include "pool.h"
#include "place.h"
#include "render.h"
#include "parser.h"

#define WINDOW_MIN_WIDTH                128
#define WINDOW_MIN_HEIGHT               128

struct configuration
{

    unsigned int displaywidth;
    unsigned int displayheight;
    unsigned int displaybpp;

};

struct state
{

    struct widget_position mouseposition;
    struct widget_position mousemovement;
    unsigned int mousebuttonleft;
    unsigned int mousebuttonright;
    struct widget *rootwidget;
    struct widget *mousewidget;
    struct widget *clickedwidget;
    struct widget *hoverwidget;
    struct widget *focusedwindow;
    struct widget *focusedwidget;

};

static unsigned int keymod = KEYMOD_NONE;
static struct render_display display;
static struct configuration configuration;
static struct state state;
static unsigned char fontnormal[0x8000];
static unsigned char fontbold[0x8000];
static unsigned char mousedata24[] = {
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
static unsigned char mousedata16[] = {
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0xFF,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
    0x00, 0x02, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00,
    0x00, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
static unsigned int mousecmap[] = {
    0xFF000000,
    0xFFB05070,
    0xFFF898B8
};

static void setupvideo(void)
{

    struct ctrl_videosettings settings;
    unsigned char black[768];

    ctrl_setvideosettings(&settings, configuration.displaywidth, configuration.displayheight, configuration.displaybpp);
    buffer_clear(black, 768);

    if (!file_walk(FILE_L0, FILE_G3, "ctrl"))
        return;

    if (!file_walk(FILE_L1, FILE_G3, "colormap"))
        return;

    file_seekwriteall(FILE_L1, black, 768, 0);
    file_seekwriteall(FILE_L0, &settings, sizeof (struct ctrl_videosettings), 0);

}

static void loadfont(unsigned int factor)
{

    switch (factor)
    {

    case 0:
        file_walk2(FILE_L0, "/data/ter-112n.pcf");
        file_walk2(FILE_L1, "/data/ter-112b.pcf");

        break;

    case 1:
        file_walk2(FILE_L0, "/data/ter-114n.pcf");
        file_walk2(FILE_L1, "/data/ter-114b.pcf");

        break;

    case 2:
        file_walk2(FILE_L0, "/data/ter-116n.pcf");
        file_walk2(FILE_L1, "/data/ter-116b.pcf");

        break;

    default:
        file_walk2(FILE_L0, "/data/ter-118n.pcf");
        file_walk2(FILE_L1, "/data/ter-118b.pcf");

        break;

    }

    file_seekread(FILE_L0, fontnormal, 0x8000, 0);
    file_seekread(FILE_L1, fontbold, 0x8000, 0);

}

static struct widget *getfocusedwindow(void)
{

    struct list_item *current = 0;

    while ((current = pool_next(current)))
    {
 
        struct widget *widget = current->data;

        if (widget->type == WIDGET_TYPE_WINDOW && widget->state == WIDGET_STATE_FOCUS)
            return widget;

    }

    return 0;

}

static struct widget *getwidgetat(struct widget *parent, int x, int y, unsigned int type)
{

    struct list_item *current = 0;
    struct widget *last = 0;

    while ((current = pool_nextin(current, parent)))
    {
 
        struct widget *child = current->data;
        struct widget *match = getwidgetat(child, x, y, type);

        if (match)
            last = match;

    }

    if (last)
        return last;

    if (!type || type == parent->type)
    {

        if (x > parent->position.x && x <= parent->position.x + parent->size.w && y > parent->position.y && y <= parent->position.y + parent->size.h)
            return parent;

    }

    return 0;

}

static void damage(struct widget *widget)
{

    struct list_item *current = 0;

    render_damagebywidget(&display, widget);

    while ((current = pool_nextin(current, widget)))
        damage(current->data);

}

static void bump(struct widget *widget)
{

    pool_bump(widget);
    pool_bump(state.mousewidget);
    damage(widget);
    damage(state.mousewidget);

}

/*
static void onfocus(struct widget *widget)
{

    if (widget_setstate(widget, WIDGET_STATE_FOCUS))
    {

        if (state.focusedwidget)
        {

            widget_setstate(state.focusedwidget, WIDGET_STATE_FOCUSOFF);
            widget_setstate(state.focusedwidget, WIDGET_STATE_NORMAL);
            damage(state.focusedwidget);

        }

        state.focusedwidget = widget;

        damage(state.focusedwidget);

    }

}
*/

static void onfocuswindow(struct widget *widget)
{

    if (widget_setstate(widget, WIDGET_STATE_FOCUS))
    {

        if (state.focusedwindow)
        {

            widget_setstate(state.focusedwindow, WIDGET_STATE_FOCUSOFF);
            widget_setstate(state.focusedwindow, WIDGET_STATE_NORMAL);
            damage(state.focusedwindow);

        }

        state.focusedwindow = widget;

        bump(state.focusedwindow);

    }

}

static void onhover(struct widget *widget)
{

    if (widget_setstate(widget, WIDGET_STATE_HOVER))
    {

        if (state.hoverwidget)
        {

            widget_setstate(state.hoverwidget, WIDGET_STATE_HOVEROFF);
            widget_setstate(state.hoverwidget, WIDGET_STATE_NORMAL);
            damage(state.hoverwidget);

        }

        state.hoverwidget = widget;

        damage(state.hoverwidget);

    }

}

static void onkeypress(unsigned int source, void *mdata, unsigned int msize)
{

    struct event_keypress *keypress = mdata;
    struct keymap *keymap = keymap_load(KEYMAP_US);
    struct keycode *keycode = keymap_getkeycode(keymap, keypress->scancode, keymod);

    keymod = keymap_modkey(keypress->scancode, keymod);

    if (!(keymod & KEYMOD_ALT))
    {

        if (state.focusedwidget)
        {

            struct event_wmkeypress2 wmkeypress;

            wmkeypress.scancode = keypress->scancode;
            wmkeypress.unicode = keycode->value[0];
            wmkeypress.length = keycode->length;
            wmkeypress.keymod = keymod;

            buffer_write(wmkeypress.pressed, 16, pool_getstring(state.focusedwidget->id), pool_getcstringlengthz(state.focusedwidget->id), 0);

            wmkeypress.pressed[15] = '\0';

            channel_sendbufferto(state.focusedwidget->source, EVENT_WMKEYPRESS, sizeof (struct event_wmkeypress2), &wmkeypress);

        }

    }

}

static void onkeyrelease(unsigned int source, void *mdata, unsigned int msize)
{

/*
    struct event_keyrelease *keyrelease = mdata;
    struct keymap *keymap = keymap_load(KEYMAP_US);
    struct keycode *keycode = keymap_getkeycode(keymap, keyrelease->scancode, keymod);

    keymod = keymap_modkey(keyrelease->scancode, keymod);

    if (!(keymod & KEYMOD_ALT))
    {

        if (state.focusedwidget)
        {

        }

    }
*/

}

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    file_link(FILE_G0);
    file_link(FILE_G1);
    file_link(FILE_G2);
    file_link(FILE_G4);
    setupvideo();

    while (channel_process())
    {

        place_widget(state.rootwidget, 0, 0, 0, 0, display.size.w, display.size.h);
        render_paint(&display, state.rootwidget);

    }

    file_unlink(FILE_G4);
    file_unlink(FILE_G2);
    file_unlink(FILE_G1);
    file_unlink(FILE_G0);

}

static void onmousemove(unsigned int source, void *mdata, unsigned int msize)
{

    struct event_mousemove *mousemove = mdata;
    int x = util_clamp(state.mouseposition.x + mousemove->relx, 0, display.size.w);
    int y = util_clamp(state.mouseposition.y + mousemove->rely, 0, display.size.h);
    struct widget *hoverwidget = getwidgetat(state.rootwidget, state.mouseposition.x, state.mouseposition.y, 0);

    if (hoverwidget)
        onhover(hoverwidget);

    state.mousemovement.x = x - state.mouseposition.x;
    state.mousemovement.y = y - state.mouseposition.y;
    state.mouseposition.x = x;
    state.mouseposition.y = y;

    damage(state.mousewidget);

    state.mousewidget->position.x = state.mouseposition.x;
    state.mousewidget->position.y = state.mouseposition.y;

    damage(state.mousewidget);

    if (state.mousebuttonleft || state.mousebuttonright)
    {

        struct widget *widget = getfocusedwindow();

        if (widget)
        {

            damage(widget);

            if (state.mousebuttonleft)
            {

                widget->position.x += state.mousemovement.x;
                widget->position.y += state.mousemovement.y;

            }

            if (state.mousebuttonright)
            {

                widget->size.w = util_max((int)(widget->size.w) + state.mousemovement.x, WINDOW_MIN_WIDTH);
                widget->size.h = util_max((int)(widget->size.h) + state.mousemovement.y, WINDOW_MIN_HEIGHT);

            }

            damage(widget);

        }

    }

}

static void onmousepress(unsigned int source, void *mdata, unsigned int msize)
{

    struct event_mousepress *mousepress = mdata;
    struct widget *clickedwindow = getwidgetat(state.rootwidget, state.mouseposition.x, state.mouseposition.y, WIDGET_TYPE_WINDOW);

    switch (mousepress->button)
    {

    case 1:
        state.mousebuttonleft = 1;
        state.clickedwidget = getwidgetat(state.rootwidget, state.mouseposition.x, state.mouseposition.y, 0);

        if (clickedwindow)
        {

            onfocuswindow(clickedwindow);
            damage(state.mousewidget);

        }

        if (state.clickedwidget)
        {

            struct event_wmclick wmclick;

            buffer_write(wmclick.clicked, 16, pool_getstring(state.clickedwidget->id), pool_getcstringlengthz(state.clickedwidget->id), 0);

            wmclick.clicked[15] = '\0';

            channel_sendbufferto(state.clickedwidget->source, EVENT_WMCLICK, sizeof (struct event_wmclick), &wmclick);

        }

        break;

    case 2:
        state.mousebuttonright = 1;

        break;

    }

}

static void onmouserelease(unsigned int source, void *mdata, unsigned int msize)
{

    struct event_mouserelease *mouserelease = mdata;

    switch (mouserelease->button)
    {

    case 1:
        state.mousebuttonleft = 0;
        state.clickedwidget = 0;

        break;

    case 2:
        state.mousebuttonright = 0;

        break;

    }

}

static void onoption(unsigned int source, void *mdata, unsigned int msize)
{

    char *key = mdata;
    char *value = key + cstring_lengthz(key);

    if (cstring_match(key, "width"))
        configuration.displaywidth = cstring_rvalue(value, cstring_length(value), 10);

    if (cstring_match(key, "height"))
        configuration.displayheight = cstring_rvalue(value, cstring_length(value), 10);

    if (cstring_match(key, "bpp"))
        configuration.displaybpp = cstring_rvalue(value, cstring_length(value), 10);

    if (cstring_match(key, "keyboard"))
        file_walk2(FILE_G1, value);

    if (cstring_match(key, "mouse"))
        file_walk2(FILE_G2, value);

    if (cstring_match(key, "video"))
    {

        if (file_walk2(FILE_G3, value))
        {

            file_walk(FILE_G4, FILE_G3, "event");
            file_walk(FILE_G5, FILE_G3, "data");

        }

    }

}

static void onvideomode(unsigned int source, void *mdata, unsigned int msize)
{

    struct event_videomode *videomode = mdata;
    unsigned int factor = videomode->h / 320;
    unsigned int lineheight = 12 + factor * 4;
    unsigned int padding = 4 + factor * 2;
    struct widget_image *mouseimage = state.mousewidget->data;

    render_setup(&display, videomode->framebuffer, videomode->w, videomode->h, videomode->bpp);

    state.mouseposition.x = videomode->w / 4;
    state.mouseposition.y = videomode->h / 4;

    loadfont(factor);
    render_setfont(RENDER_FONTNORMAL, fontnormal, lineheight, padding);
    render_setfont(RENDER_FONTBOLD, fontbold, lineheight, padding);

    switch (factor)
    {

    case 0:
    case 1:
        state.mousewidget->position.x = state.mouseposition.x;
        state.mousewidget->position.y = state.mouseposition.y;
        state.mousewidget->size.w = 12;
        state.mousewidget->size.h = 16;
        mouseimage->data = mousedata16;
        mouseimage->cmap = mousecmap;

        break;

    case 2:
    default:
        state.mousewidget->position.x = state.mouseposition.x;
        state.mousewidget->position.y = state.mouseposition.y;
        state.mousewidget->size.w = 18;
        state.mousewidget->size.h = 24;
        mouseimage->data = mousedata24;
        mouseimage->cmap = mousecmap;

        break;

    }

}

static void onwmmap(unsigned int source, void *mdata, unsigned int msize)
{

    channel_sendto(source, EVENT_WMINIT);

}

static unsigned int numwindows;

static void onwmrenderdata(unsigned int source, void *mdata, unsigned int msize)
{

    struct list_item *current = 0;

    parser_parse(source, "root", msize, mdata);

    while ((current = pool_nextsource(current, source)))
    {

        struct widget *widget = current->data;

        if (widget->type == WIDGET_TYPE_WINDOW)
        {

            if (widget->size.w == 0 && widget->size.h == 0)
            {

                widget->position.x = 64 + 128 * numwindows;
                widget->position.y = 64 + 64 * numwindows;
                widget->size.w = 640;
                widget->size.h = 640;

                bump(widget);

                numwindows++;

            }

            damage(widget);

        }

    }

}

static void onwmunmap(unsigned int source, void *mdata, unsigned int msize)
{

}

static void setupwidgets(void)
{

    char *data =
        "+ container id \"root\" layout \"float\"\n"
        "+ fill in \"root\" color \"FF142434\"\n"
        "+ image id \"mouse\" in \"root\"\n";

    parser_parse(0, "", cstring_length(data), data);

    state.rootwidget = pool_getwidgetbyid(0, "root");
    state.hoverwidget = pool_getwidgetbyid(0, "root");
    state.mousewidget = pool_getwidgetbyid(0, "mouse");

}

void init(void)
{

    pool_setup();
    setupwidgets();

    configuration.displaywidth = 1920;
    configuration.displayheight = 1080;
    configuration.displaybpp = 4;

    if (!file_walk2(FILE_G0, "system:service/wm"))
        return;

    channel_bind(EVENT_KEYPRESS, onkeypress);
    channel_bind(EVENT_KEYRELEASE, onkeyrelease);
    channel_bind(EVENT_MAIN, onmain);
    channel_bind(EVENT_MOUSEMOVE, onmousemove);
    channel_bind(EVENT_MOUSEPRESS, onmousepress);
    channel_bind(EVENT_MOUSERELEASE, onmouserelease);
    channel_bind(EVENT_OPTION, onoption);
    channel_bind(EVENT_VIDEOMODE, onvideomode);
    channel_bind(EVENT_WMMAP, onwmmap);
    channel_bind(EVENT_WMRENDERDATA, onwmrenderdata);
    channel_bind(EVENT_WMUNMAP, onwmunmap);

}

