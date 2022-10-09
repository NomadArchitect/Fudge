#include <fudge.h>
#include <net.h>
#include <abi.h>
#include <socket.h>

static struct socket local;
static struct socket remote;
static struct socket router;
static char inputdata[BUFFER_SIZE];
static struct ring input;

static void handlehttppacket(void)
{

    char buffer[MESSAGE_SIZE];
    unsigned int count;

    while ((count = ring_read(&input, buffer, MESSAGE_SIZE)))
        channel_sendbuffer(EVENT_DATA, count, buffer);

}

static unsigned int buildrequest(unsigned int count, void *buffer, struct url *url)
{

    unsigned int offset = 0;

    offset += cstring_write(buffer, count, "GET /", offset);

    if (url->path)
        offset += cstring_write(buffer, count, url->path, offset);

    offset += cstring_write(buffer, count, " HTTP/1.1\r\n", offset);
    offset += cstring_write(buffer, count, "Host: ", offset);
    offset += cstring_write(buffer, count, url->host, offset);
    offset += cstring_write(buffer, count, "\r\n\r\n", offset);

    return offset;

}

static void dnsresolve(struct socket *socket, char *domain)
{

    unsigned int id = file_spawn("/bin/dns");
    struct message message;

    if (!id)
        channel_error("Could not spawn process");

    message_init(&message, EVENT_OPTION);
    message_putstringz(&message, "domain");
    message_putstringz(&message, domain);
    channel_redirectback(id, EVENT_QUERY);
    channel_redirectback(id, EVENT_CLOSE);
    channel_sendmessageto(id, &message);
    channel_sendto(id, EVENT_MAIN);

    while (channel_pollfrom(id, &message) != EVENT_CLOSE)
    {

        if (message.header.event == EVENT_QUERY)
        {

            char *key = message.data.buffer;
            char *value = key + cstring_lengthz(key);

            if (cstring_match(key, "data"))
                socket_bind_ipv4s(socket, value);

        }

    }

}

static void seed(struct mtwist_state *state)
{

    struct ctrl_clocksettings settings;

    if (!file_walk2(FILE_L0, option_getstring("clock")))
        channel_error("Could not find clock device");

    if (!file_walk(FILE_L1, FILE_L0, "ctrl"))
        channel_error("Could not find clock device ctrl");

    file_readall(FILE_L1, &settings, sizeof (struct ctrl_clocksettings));
    mtwist_seed1(state, time_unixtime(settings.year, settings.month, settings.day, settings.hours, settings.minutes, settings.seconds));

}

static void parseurl(struct url *url, char *urldata, unsigned int urlsize)
{

    char *opturl = option_getstring("url");
    unsigned int count = cstring_length(opturl);

    if (count)
    {

        if (cstring_length(opturl) >= 4 && buffer_match(opturl, "http", 4))
            url_parse(url, urldata, urlsize, opturl, URL_SCHEME);
        else
            url_parse(url, urldata, urlsize, opturl, URL_HOST);

    }

}

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    char urldata[BUFFER_SIZE];
    struct url url;
    unsigned char buffer[BUFFER_SIZE];
    unsigned int count;
    struct mtwist_state state;

    seed(&state);
    parseurl(&url, urldata, BUFFER_SIZE);

    if (!file_walk2(FILE_L0, option_getstring("ethernet")))
        channel_error("Could not find ethernet device");

    if (!file_walk(FILE_L1, FILE_L0, "data"))
        channel_error("Could not find ethernet device data");

    if (!file_walk(FILE_L2, FILE_L0, "addr"))
        channel_error("Could not find ethernet device addr");

    socket_bind_ipv4s(&local, option_getstring("local-address"));
    socket_bind_tcpv(&local, mtwist_rand(&state), mtwist_rand(&state));
    socket_bind_ipv4s(&router, option_getstring("router-address"));
    socket_resolvelocal(FILE_L2, &local);

    if (url.host)
        dnsresolve(&remote, url.host);
    else
        socket_bind_ipv4s(&remote, option_getstring("remote-address"));

    if (url.port)
        socket_bind_tcps(&remote, url.port, 0);
    else
        socket_bind_tcpv(&remote, option_getdecimal("remote-port"), 0);

    count = buildrequest(BUFFER_SIZE, buffer, &url);

    file_link(FILE_L1);
    socket_resolveremote(FILE_L1, &local, &router);
    socket_connect_tcp(FILE_L1, &local, &remote, &router);
    socket_send_tcp(FILE_L1, &local, &remote, &router, count, buffer);

    while ((count = socket_receive_tcp(FILE_L1, &local, &remote, &router, buffer, BUFFER_SIZE)))
    {

        if (ring_write(&input, buffer, count))
            handlehttppacket();

    }

    socket_disconnect_tcp(FILE_L1, &local, &remote, &router);
    file_unlink(FILE_L1);
    channel_close();

}

void init(void)
{

    ring_init(&input, BUFFER_SIZE, inputdata);
    socket_init(&local);
    socket_init(&remote);
    socket_init(&router);
    option_add("clock", "system:clock/if:0");
    option_add("ethernet", "system:ethernet/if:0");
    option_add("local-address", "10.0.5.1");
    option_add("remote-address", "");
    option_add("remote-port", "80");
    option_add("router-address", "10.0.5.80");
    option_add("url", "");
    channel_bind(EVENT_MAIN, onmain);

}

