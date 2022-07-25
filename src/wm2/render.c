#include <fudge.h>
#include <abi.h>
#include "util.h"
#include "widget.h"
#include "pool.h"
#include "render.h"

#define DAMAGE_STATE_NONE               0
#define DAMAGE_STATE_MADE               1
#define CMAP_INDEX_SHADOW               0
#define CMAP_INDEX_MAIN_LIGHT           1
#define CMAP_INDEX_MAIN_NORMAL          2
#define CMAP_INDEX_AREA_NORMAL          3
#define LINESEGMENT_TYPE_RELX0X0        1
#define LINESEGMENT_TYPE_RELX0X1        2
#define LINESEGMENT_TYPE_RELX1X1        3

struct font
{

    unsigned char *data;
    unsigned char *bitmapdata;
    unsigned int bitmapalign;
    unsigned int lineheight;
    unsigned int padding;

};

struct linesegment
{

    unsigned int type;
    int p0;
    int p1;
    unsigned int color;

};

static struct font fonts[32];

static unsigned short getfontindex(struct font *font, unsigned short c)
{

    switch (c)
    {

    case '\n':
        return pcf_getindex(font->data, ' ');

    }

    return pcf_getindex(font->data, c);

}

static void blitline(struct render_display *display, unsigned int color, int line, int x0, int x1)
{

    unsigned int *buffer = display->framebuffer;
    int x;

    for (x = x0; x < x1; x++)
        buffer[line * display->size.w + x] = color;

}

static void blitcmap32line(struct render_display *display, struct position *p, void *idata, unsigned int iwidth, unsigned int *cmap, int line)
{

    unsigned int *buffer = display->framebuffer;
    unsigned char *data = idata;
    unsigned int w = (p->x + iwidth >= display->size.w) ? display->size.w - p->x : iwidth;
    int x;

    for (x = 0; x < w; x++)
    {

        unsigned int soffset = (line * iwidth + x);
        unsigned int toffset = (p->y * display->size.w + p->x) + (line * display->size.w + x);

        if (data[soffset] != 0xFF)
            buffer[toffset] = cmap[data[soffset]];

    }

}

static void blitchar(struct render_display *display, unsigned char *data, unsigned int color, int rx, int line, int x0, int x1)
{

    unsigned int i;

    for (i = x0; i < x1; i++)
    {

        if (data[(i >> 3)] & (0x80 >> (i % 8)))
            blitline(display, color, line, rx + i, rx + i + 1);

    }

}

/*
static void blitcharcursor(struct render_display *display, unsigned char *data, unsigned int color, int rx, int line, int x0, int x1)
{

    unsigned int i;

    for (i = x0; i < x1; i++)
    {

        if (!(data[(i >> 3)] & (0x80 >> (i % 8))))
            blitline(display, color, line, rx + i, rx + i + 1);

    }

}
*/

static void blittext(struct render_display *display, struct font *font, unsigned int color, char *text, unsigned int length, int rx, int ry, int line, int x0, int x1)
{

    if (util_intersects(line, ry, ry + font->lineheight))
    {

        unsigned int lline = (line - ry) % font->lineheight;
        unsigned int i;

        for (i = 0; i < length; i++)
        {

            unsigned short index = getfontindex(font, text[i]);
            unsigned int offset = pcf_getbitmapoffset(font->data, index);
            struct pcf_metricsdata metricsdata;

            pcf_readmetricsdata(font->data, index, &metricsdata);

            if (util_intersects(lline, 0, metricsdata.ascent + metricsdata.descent))
            {

                if (util_intersects(rx, x0, x1) || util_intersects(rx + metricsdata.width, x0, x1))
                {

                    unsigned char *data = font->bitmapdata + offset + lline * font->bitmapalign;
                    int r0 = util_max(0, x0 - rx);
                    int r1 = util_min(x1 - rx, metricsdata.width);

                    blitchar(display, data, color, rx, line, r0, r1);

                }

            }

            rx += metricsdata.width;

        }

    }

}

static void blitlinesegments(struct render_display *display, int x0, int x1, unsigned int *cmap, struct linesegment *ls, unsigned int n, int line)
{

    unsigned int i;

    for (i = 0; i < n; i++)
    {

        struct linesegment *p = &ls[i];
        int p0;
        int p1;

        switch (p->type)
        {

        case LINESEGMENT_TYPE_RELX0X0:
            p0 = x0 + p->p0;
            p1 = x0 + p->p1;

            break;

        case LINESEGMENT_TYPE_RELX0X1:
            p0 = x0 + p->p0;
            p1 = x1 + p->p1;

            break;

        case LINESEGMENT_TYPE_RELX1X1:
            p0 = x1 + p->p0;
            p1 = x1 + p->p1;

            break;

        default:
            p0 = x0;
            p1 = x1;

            break;

        }

        p0 = util_max(p0, display->damage.position0.x);
        p1 = util_min(p1, display->damage.position1.x);

        blitline(display, cmap[p->color], line, p0, p1);

    }

}

static void paintbutton(struct render_display *display, struct widget *widget, int line, int x0, int x1)
{

    static unsigned int buttoncmapnormal[] = {
        0xFF101010,
        0xFF687888,
        0xFF485868,
        0xFF182838
    };
    static unsigned int buttoncmapfocus[] = {
        0xFF101010,
        0xFF48C888,
        0xFF28A868,
        0xFF182838
    };
    static struct linesegment buttonborder0[1] = {
        {LINESEGMENT_TYPE_RELX0X1, 1, -1, CMAP_INDEX_SHADOW}
    };
    static struct linesegment buttonborder1[1] = {
        {LINESEGMENT_TYPE_RELX0X1, 0, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment buttonborder2[3] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 3, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X1, 3, -3, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -3, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment buttonborder3[5] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 2, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X0, 2, 2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX0X1, 4, -4, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX1X1, -4, -2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -2, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment buttonborderlabel[5] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 2, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X0, 2, 3, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX0X1, 3, -3, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX1X1, -3, -2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -2, 0, CMAP_INDEX_SHADOW}
    };

    struct widget_button *button = widget->data;
    unsigned int *cmap = (button->focus) ? buttoncmapfocus : buttoncmapnormal;
    int ly = line - widget->position.y;
    struct linesegment *segments;
    unsigned int nsegments;
    unsigned int tl = cstring_length(button->label);
    unsigned int tw = render_getrowwidth(button->label, tl);

    if (ly == 0 || ly == widget->size.h - 1)
    {

        segments = buttonborder0;
        nsegments = 1;

    }

    else if (ly == 1 || ly == widget->size.h - 2)
    {

        segments = buttonborder1;
        nsegments = 1;

    }

    else if (ly == 2 || ly == widget->size.h - 3)
    {

        segments = buttonborder2;
        nsegments = 3;

    }

    else if (ly == 3 || ly == widget->size.h - 4)
    {

        segments = buttonborder3;
        nsegments = 5;

    }

    else if (ly > 3 && ly < widget->size.h - 4)
    {

        segments = buttonborderlabel;
        nsegments = 5;

    }

    else
    {

        segments = 0;
        nsegments = 0;

    }

    blitlinesegments(display, widget->position.x, widget->position.x + widget->size.w, cmap, segments, nsegments, line);
    blittext(display, &fonts[0], 0xFFFFFFFF, button->label, tl, widget->position.x + (widget->size.w / 2) - (tw / 2), widget->position.y + (widget->size.h / 2) - (16 / 2), line, x0, x1);

}

static void paintfill(struct render_display *display, struct widget *widget, int line, int x0, int x1)
{

    struct widget_fill *fill = widget->data;

    blitline(display, fill->color, line, x0, x1);

}

static void paintimage(struct render_display *display, struct widget *widget, int line, int x0, int x1)
{

    struct widget_image *image = widget->data;

    blitcmap32line(display, &widget->position, image->data, widget->size.w, image->cmap, line - widget->position.y);

}

static void painttextbox(struct render_display *display, struct widget *widget, int line, int x0, int x1)
{

    struct widget_textbox *textbox = widget->data;
    unsigned int rownum = (line - widget->position.y) / fonts[0].lineheight;
    unsigned int rowtotal = util_findrowtotal(textbox->content, textbox->length);

    if (rownum < rowtotal)
    {

        unsigned int s = util_findrowstart(textbox->content, textbox->length, rownum);
        unsigned int length = util_findrowcount(textbox->content, textbox->length, s);

        blittext(display, &fonts[0], 0xFFFFFFFF, textbox->content + s, length, widget->position.x, widget->position.y + rownum * fonts[0].lineheight, line, x0, x1);

    }

}

static void paintwindow(struct render_display *display, struct widget *widget, int line, int x0, int x1)
{
    static unsigned int windowcmapnormal[] = {
        0xFF101010,
        0xFF687888,
        0xFF485868,
        0xFF182838
    };
    static unsigned int windowcmapfocus[] = {
        0xFF101010,
        0xFF48C888,
        0xFF28A868,
        0xFF182838
    };
    static struct linesegment windowborder0[1] = {
        {LINESEGMENT_TYPE_RELX0X1, 1, -1, CMAP_INDEX_SHADOW}
    };
    static struct linesegment windowborder1[1] = {
        {LINESEGMENT_TYPE_RELX0X1, 0, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment windowborder2[3] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 3, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X1, 3, -3, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -3, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment windowborder3[5] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 2, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X0, 2, 2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX0X1, 4, -4, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX1X1, -4, -2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -2, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment windowbordertitle[5] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 2, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X0, 2, 3, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX0X1, 3, -3, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX1X1, -3, -2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -2, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment windowborderspacing[7] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 2, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X0, 2, 3, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX0X0, 3, 4, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX0X1, 4, -4, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX1X1, -4, -3, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX1X1, -3, -2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -2, 0, CMAP_INDEX_SHADOW}
    };
    static struct linesegment windowborderarea[9] = {
        {LINESEGMENT_TYPE_RELX0X0, 0, 2, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X0, 2, 3, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX0X0, 3, 4, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX0X0, 4, 5, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX0X1, 5, -5, CMAP_INDEX_AREA_NORMAL},
        {LINESEGMENT_TYPE_RELX1X1, -5, -4, CMAP_INDEX_SHADOW},
        {LINESEGMENT_TYPE_RELX1X1, -4, -3, CMAP_INDEX_MAIN_NORMAL},
        {LINESEGMENT_TYPE_RELX1X1, -3, -2, CMAP_INDEX_MAIN_LIGHT},
        {LINESEGMENT_TYPE_RELX1X1, -2, 0, CMAP_INDEX_SHADOW}
    };

    struct widget_window *window = widget->data;
    unsigned int *cmap = (window->focus) ? windowcmapfocus : windowcmapnormal;
    int ly = line - widget->position.y;
    struct linesegment *segments;
    unsigned int nsegments;
    unsigned int tl = cstring_length(window->title);
    unsigned int tw = render_getrowwidth(window->title, tl);

    if (ly == 0 || ly == widget->size.h - 1)
    {

        segments = windowborder0;
        nsegments = 1;

    }

    else if (ly == 1 || ly == widget->size.h - 2)
    {

        segments = windowborder1;
        nsegments = 1;

    }

    else if (ly == 2 || ly == widget->size.h - 3)
    {

        segments = windowborder2;
        nsegments = 3;

    }

    else if (ly == 3 || ly == widget->size.h - 4)
    {

        segments = windowborder3;
        nsegments = 5;

    }

    else if (ly >= 4 && ly < 40)
    {

        segments = windowbordertitle;
        nsegments = 5;

    }

    else if (ly == 40)
    {

        segments = windowborderspacing;
        nsegments = 7;

    }

    else if (ly > 40 && ly < widget->size.h - 4)
    {

        segments = windowborderarea;
        nsegments = 9;

    }

    else
    {

        segments = 0;
        nsegments = 0;

    }

    blitlinesegments(display, widget->position.x, widget->position.x + widget->size.w, cmap, segments, nsegments, line);
    blittext(display, &fonts[0], 0xFFFFFFFF, window->title, tl, widget->position.x + (widget->size.w / 2) - (tw / 2), widget->position.y + 12, line, x0, x1);

}

static void paintwidget(struct render_display *display, struct widget *widget, int line)
{

    if (util_intersects(line, widget->position.y, widget->position.y + widget->size.h))
    {

        int x0 = util_max(widget->position.x, display->damage.position0.x);
        int x1 = util_min(widget->position.x + widget->size.w, display->damage.position1.x);

        switch (widget->type)
        {

        case WIDGET_TYPE_BUTTON:
            paintbutton(display, widget, line, x0, x1);

            break;

        case WIDGET_TYPE_FILL:
            paintfill(display, widget, line, x0, x1);

            break;

        case WIDGET_TYPE_IMAGE:
            paintimage(display, widget, line, x0, x1);

            break;

        case WIDGET_TYPE_TEXTBOX:
            painttextbox(display, widget, line, x0, x1);

            break;

        case WIDGET_TYPE_WINDOW:
            paintwindow(display, widget, line, x0, x1);

            break;

        }

    }

}

unsigned int render_getrowwidth(char *text, unsigned int length)
{

    struct font *font = &fonts[0];
    unsigned int w = 0;
    unsigned int i;

    for (i = 0; i < length; i++)
    {

        struct pcf_metricsdata metricsdata;
        unsigned short index;

        if (text[i] == '\n')
            break;

        index = getfontindex(font, text[i]);

        pcf_readmetricsdata(font->data, index, &metricsdata);

        w += metricsdata.width;

    }

    return w;

}

unsigned int render_getrowheight(char *text, unsigned int length)
{

    return fonts[0].lineheight;

}

unsigned int render_gettextwidth(char *text, unsigned int length)
{

    unsigned int wlast = 0;
    unsigned int cw = 0;
    unsigned int s = 0;
    unsigned int i;

    for (i = 0; i < length; i++)
    {

        if (text[i] == '\n')
        {

            unsigned int w = render_getrowwidth(text + s, length - s);

            cw = util_max(cw, w);
            s = i + 1;

        }

    }

    if (length - s > 0)
        wlast = render_getrowwidth(text + s, length - s);

    return util_max(cw, wlast);

}

unsigned int render_gettextheight(char *text, unsigned int length)
{

    unsigned int hlast = 0;
    unsigned int ch = 0;
    unsigned int s = 0;
    unsigned int i;

    for (i = 0; i < length; i++)
    {

        if (text[i] == '\n')
        {

            unsigned int h = render_getrowheight(text + s, length - s);

            ch += h;
            s = i + 1;

        }

    }

    if (length - s > 0)
        hlast = render_getrowheight(text + s, length - s);

    return ch + hlast;

}

void render_damage(struct render_display *display, int x0, int y0, int x1, int y1)
{

    x0 = util_clamp(x0, 0, display->size.w);
    y0 = util_clamp(y0, 0, display->size.h);
    x1 = util_clamp(x1, 0, display->size.w);
    y1 = util_clamp(y1, 0, display->size.h);

    switch (display->damage.state)
    {

    case DAMAGE_STATE_NONE:
        display->damage.position0.x = x0;
        display->damage.position0.y = y0;
        display->damage.position1.x = x1;
        display->damage.position1.y = y1;

        break;

    case DAMAGE_STATE_MADE:
        display->damage.position0.x = util_min(x0, display->damage.position0.x);
        display->damage.position0.y = util_min(y0, display->damage.position0.y);
        display->damage.position1.x = util_max(x1, display->damage.position1.x);
        display->damage.position1.y = util_max(y1, display->damage.position1.y);

        break;

    }

    display->damage.state = DAMAGE_STATE_MADE;

}

void render_damagebywidget(struct render_display *display, struct widget *widget)
{

    render_damage(display, widget->position.x, widget->position.y, widget->position.x + widget->size.w, widget->position.y + widget->size.h);

}

void render_paint(struct render_display *display, struct widget *rootwidget, struct widget_image *image)
{

    if (!display->framebuffer)
        return;

    if (display->damage.state == DAMAGE_STATE_MADE)
    {

        int line;

        for (line = display->damage.position0.y; line < display->damage.position1.y; line++)
        {

            struct list_item *current = 0;

            while ((current = pool_next(current)))
            {

                struct widget *child = current->data;

                paintwidget(display, child, line);

            }

        }

        display->damage.state = DAMAGE_STATE_NONE;

    }

}

void render_setfont(unsigned int index, void *data, unsigned int lineheight, unsigned int padding)
{

    fonts[index].data = data;
    fonts[index].bitmapdata = pcf_getbitmapdata(fonts[index].data);
    fonts[index].bitmapalign = pcf_getbitmapalign(fonts[index].data);
    fonts[index].lineheight = lineheight;
    fonts[index].padding = padding;

}

void render_setup(struct render_display *display, void *framebuffer, unsigned int w, unsigned int h, unsigned int bpp)
{

    display->framebuffer = framebuffer;
    display->size.w = w;
    display->size.h = h;
    display->bpp = bpp;

    render_damage(display, 0, 0, display->size.w, display->size.h);

}

