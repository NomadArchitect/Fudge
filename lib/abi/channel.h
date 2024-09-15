void channel_dispatch(struct message *message, void *data);
unsigned int channel_send(unsigned int channel, unsigned int event);
unsigned int channel_send_buffer(unsigned int channel, unsigned int event, unsigned int count, void *data);
unsigned int channel_send_fmt0(unsigned int channel, unsigned int event, char *fmt);
unsigned int channel_send_fmt1(unsigned int channel, unsigned int event, char *fmt, void *arg1);
unsigned int channel_send_fmt2(unsigned int channel, unsigned int event, char *fmt, void *arg1, void *arg2);
unsigned int channel_send_fmt3(unsigned int channel, unsigned int event, char *fmt, void *arg1, void *arg2, void *arg3);
unsigned int channel_send_fmt4(unsigned int channel, unsigned int event, char *fmt, void *arg1, void *arg2, void *arg3, void *arg4);
unsigned int channel_send_fmt6(unsigned int channel, unsigned int event, char *fmt, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6);
unsigned int channel_send_fmt8(unsigned int channel, unsigned int event, char *fmt, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6, void *arg7, void *arg8);
unsigned int channel_pick(struct message *message, unsigned int count, void *data);
unsigned int channel_process(void);
unsigned int channel_poll(unsigned int event, struct message *message, unsigned int count, void *data);
unsigned int channel_read(unsigned int event, unsigned int count, void *data);
unsigned int channel_wait(unsigned int event);
unsigned int channel_wait_buffer(unsigned int event, unsigned int count, void *data);
void channel_bind(unsigned int event, void (*callback)(unsigned int source, void *mdata, unsigned int msize));
void channel_route(unsigned int event, unsigned int target);
void channel_open(void);
void channel_close(void);
