#define EVENT_ADDR_BROADCAST            0
#define EVENT_INIT                      0x01
#define EVENT_KILL                      0x02
#define EVENT_DATAPIPE                  0x03
#define EVENT_DATAFILE                  0x04
#define EVENT_DATASTOP                  0x05
#define EVENT_KEYPRESS                  0x10
#define EVENT_KEYRELEASE                0x11
#define EVENT_MOUSEMOVE                 0x12
#define EVENT_MOUSEPRESS                0x13
#define EVENT_MOUSERELEASE              0x14
#define EVENT_CONSOLEDATA               0x15
#define EVENT_TIMERTICK                 0x16
#define EVENT_VIDEOMODE                 0x17
#define EVENT_WMMAP                     0x80
#define EVENT_WMUNMAP                   0x81
#define EVENT_WMCONFIGURE               0x82
#define EVENT_WMSHOW                    0x83
#define EVENT_WMHIDE                    0x84
#define EVENT_WMKEYPRESS                0x90
#define EVENT_WMKEYRELEASE              0x91
#define EVENT_WMMOUSEMOVE               0x92
#define EVENT_WMMOUSEPRESS              0x93
#define EVENT_WMMOUSERELEASE            0x94

struct event_header
{

    unsigned int type;
    unsigned int source;
    unsigned int target;
    unsigned int length;
    unsigned int nroutes;
    unsigned int reserved0;
    unsigned int reserved1;
    unsigned int reserved2;
    unsigned int routes[16];

};

struct event_datapipe
{

    unsigned int session;
    unsigned int count;

};

struct event_datafile
{

    unsigned int session;
    unsigned char descriptor;

};

struct event_datastop
{

    unsigned int session;

};

struct event_keypress
{

    unsigned char scancode;

};

struct event_keyrelease
{

    unsigned char scancode;

};

struct event_mousepress
{

    unsigned int button;

};

struct event_mouserelease
{

    unsigned int button;

};

struct event_mousemove
{

    char relx;
    char rely;

};

struct event_consoledata
{

    unsigned char data;

};

struct event_timertick
{

    unsigned int counter;

};

struct event_videomode
{

    unsigned int w;
    unsigned int h;
    unsigned int bpp;

};

struct event_wmconfigure
{

    unsigned int rendertarget;
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
    unsigned int padding;
    unsigned int lineheight;

};

struct event_wmkeypress
{

    unsigned char scancode;

};

struct event_wmkeyrelease
{

    unsigned char scancode;

};

struct event_wmmousepress
{

    unsigned int button;

};

struct event_wmmouserelease
{

    unsigned int button;

};

struct event_wmmousemove
{

    char relx;
    char rely;

};

void *event_getdata(struct event_header *header);
unsigned int event_avail(struct event_header *header);
unsigned int event_route(struct event_header *header, unsigned int target);
unsigned int event_appenddata(struct event_header *header, unsigned int count, void *buffer);
struct event_header *event_create(struct event_header *oheader, unsigned int type, unsigned int target);
struct event_header *event_createconsoledata(struct event_header *oheader, unsigned int target, char data);
struct event_header *event_createdatafile(struct event_header *oheader, unsigned int session, unsigned int target, unsigned int descriptor);
struct event_header *event_createdatapipe(struct event_header *oheader, unsigned int session, unsigned int target);
struct event_header *event_createdatastop(struct event_header *oheader, unsigned int session, unsigned int target);
struct event_header *event_createkeypress(struct event_header *oheader, unsigned int target, unsigned char scancode);
struct event_header *event_createkeyrelease(struct event_header *oheader, unsigned int target, unsigned char scancode);
struct event_header *event_createmousepress(struct event_header *oheader, unsigned int target, unsigned int button);
struct event_header *event_createmouserelease(struct event_header *oheader, unsigned int target, unsigned int button);
struct event_header *event_createmousemove(struct event_header *oheader, unsigned int target, char relx, char rely);
struct event_header *event_createtimertick(struct event_header *oheader, unsigned int target, unsigned int counter);
struct event_header *event_createvideomode(struct event_header *oheader, unsigned int target, unsigned int w, unsigned int h, unsigned int bpp);
struct event_header *event_forward(struct event_header *oheader, struct event_header *iheader, unsigned int type, unsigned int target);
struct event_header *event_forwarddatafile(struct event_header *oheader, struct event_header *iheader, unsigned int session, unsigned int target, unsigned int descriptor);
struct event_header *event_forwarddatastop(struct event_header *oheader, struct event_header *iheader, unsigned int session, unsigned int target);
struct event_header *event_request(struct event_header *oheader, struct event_header *iheader, unsigned int type, unsigned int target);
struct event_header *event_requestdatafile(struct event_header *oheader, struct event_header *iheader, unsigned int session, unsigned int target, unsigned int descriptor);
struct event_header *event_requestdatapipe(struct event_header *oheader, struct event_header *iheader, unsigned int session, unsigned int target);
struct event_header *event_requestdatastop(struct event_header *oheader, struct event_header *iheader, unsigned int session, unsigned int target);
struct event_header *event_requestwmkeypress(struct event_header *oheader, struct event_header *iheader, unsigned int target, unsigned char scancode);
struct event_header *event_requestwmkeyrelease(struct event_header *oheader, struct event_header *iheader, unsigned int target, unsigned char scancode);
struct event_header *event_requestwmmousepress(struct event_header *oheader, struct event_header *iheader, unsigned int target, unsigned int button);
struct event_header *event_requestwmmouserelease(struct event_header *oheader, struct event_header *iheader, unsigned int target, unsigned int button);
struct event_header *event_requestwmmousemove(struct event_header *oheader, struct event_header *iheader, unsigned int target, char relx, char rely);
struct event_header *event_requestwmconfigure(struct event_header *oheader, struct event_header *iheader, unsigned int target, unsigned int rendertarget, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int padding, unsigned int lineheight);
struct event_header *event_reply(struct event_header *oheader, struct event_header *iheader, unsigned int type);
struct event_header *event_replydatapipe(struct event_header *oheader, struct event_header *iheader, unsigned int session);
struct event_header *event_replydatastop(struct event_header *oheader, struct event_header *iheader, unsigned int session);
