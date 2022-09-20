#include <fudge.h>
#include <net.h>
#include <abi.h>
#include <socket.h>

static struct socket local;
static struct socket remote;
static struct socket router;
static char url[512];

static unsigned int buildrequest(unsigned int count, void *buffer, struct url *kurl)
{

    unsigned int offset = 0;

    offset += cstring_write(buffer, count, "GET /", offset);

    if (kurl->path)
        offset += cstring_write(buffer, count, kurl->path, offset);

    offset += cstring_write(buffer, count, " HTTP/1.1\r\n", offset);
    offset += cstring_write(buffer, count, "Host: ", offset);
    offset += cstring_write(buffer, count, kurl->host, offset);
    offset += cstring_write(buffer, count, "\r\n\r\n", offset);

    return offset;

}

static void resolve(char *domain)
{

    unsigned int id = file_spawn("/bin/dns");

    if (id)
    {

        struct message message;

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
                    socket_bind_ipv4s(&remote, value);

            }

        }

    }

    else
    {

        channel_error("Could not spawn process");

    }

}

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    file_notify(FILE_G1, EVENT_WMMAP, 0, 0);

}

static void onoption(unsigned int source, void *mdata, unsigned int msize)
{

    char *key = mdata;
    char *value = key + cstring_lengthz(key);

    if (cstring_match(key, "ethernet"))
        file_walk2(FILE_G0, value);

    if (cstring_match(key, "url"))
        cstring_copy(url, value);

    if (cstring_match(key, "local-address"))
        socket_bind_ipv4s(&local, value);

    if (cstring_match(key, "local-port"))
        socket_bind_tcps(&local, value, 42);

    if (cstring_match(key, "remote-address") || cstring_match(key, "address"))
        socket_bind_ipv4s(&remote, value);

    if (cstring_match(key, "remote-port") || cstring_match(key, "port"))
        socket_bind_tcps(&remote, value, 0);

    if (cstring_match(key, "router-address"))
        socket_bind_ipv4s(&router, value);

}

static void onterm(unsigned int source, void *mdata, unsigned int msize)
{

    file_notify(FILE_G1, EVENT_WMUNMAP, 0, 0);
    channel_close();

}

static void onwminit(unsigned int source, void *mdata, unsigned int msize)
{

    char urldata[BUFFER_SIZE];
    struct url kurl;

    if (file_walk(FILE_L0, FILE_G0, "addr"))
        socket_resolvelocal(FILE_L0, &local);
    else
        channel_error("Could not find address");

    if (cstring_length(url) >= 4 && buffer_match(url, "http", 4))
        url_parse(&kurl, urldata, BUFFER_SIZE, url, URL_SCHEME);
    else
        url_parse(&kurl, urldata, BUFFER_SIZE, url, URL_HOST);

    if (kurl.host)
        resolve(kurl.host);

    if (kurl.port)
        socket_bind_tcps(&remote, kurl.port, 0);

    if (file_walk(FILE_L0, FILE_G0, "data"))
    {

        unsigned char buffer[BUFFER_SIZE];
        unsigned int count = buildrequest(BUFFER_SIZE, buffer, &kurl);

        file_link(FILE_L0);
        socket_resolveremote(FILE_L0, &local, &router);
        socket_connect_tcp(FILE_L0, &local, &remote, &router);
        socket_send_tcp(FILE_L0, &local, &remote, &router, count, buffer);

        while ((count = socket_receive_tcp(FILE_L0, &local, &remote, &router, buffer, BUFFER_SIZE)))
        {

            /* Todo: Strip away headers by reading line by line */

            file_notify(FILE_G1, EVENT_WMRENDERDATA, count, buffer);

        }

        socket_disconnect_tcp(FILE_L0, &local, &remote, &router);
        file_unlink(FILE_L0);

    }

    else
    {

        channel_error("Could not find data");

    }

}

void init(void)
{

    if (!file_walk2(FILE_G1, "system:service/wm"))
        return;

    file_walk2(FILE_G0, "system:ethernet/if:0");
    socket_init(&local);
    socket_bind_ipv4s(&local, "10.0.5.1");
    socket_bind_tcps(&local, "50001", 42);
    socket_init(&remote);
    socket_bind_tcps(&remote, "80", 0);
    socket_init(&router);
    socket_bind_ipv4s(&router, "10.0.5.80");
    channel_bind(EVENT_MAIN, onmain);
    channel_bind(EVENT_OPTION, onoption);
    channel_bind(EVENT_TERM, onterm);
    channel_bind(EVENT_WMINIT, onwminit);

}
