#include "common.h"
#include "channel.h"
class client_status{
public:
    channel *now_channel;
    int fd;

    client_status(int now_fd)
    {
        fd=now_fd;
        now_channel=NULL;
    }

    client_status(int now_fd,channel* channel)
    {
        fd=now_fd;
        now_channel=channel;
        channel->someone_enter(now_fd);
    }

    void client_enter_channel(string son_channel)
    {
        now_channel->someone_leave(fd);
        now_channel=now_channel->enter_channel(son_channel);
        now_channel->someone_enter(fd);
    }

    void client_enter_channel_from_root(std::string son_channel)
    {
        now_channel->someone_leave(fd);
        now_channel=now_channel->enter_channel_from_root(son_channel);
        now_channel->someone_enter(fd);
    }

    void send_message_only_local_channel(char* buff)
    {
        now_channel->send_only_local_cannel(buff);
    }

    void send_message_to_all_son_channel(char* buff)
    {
        now_channel->send_to_all_son_channel(buff);
    }
};