#pragma once
#include "common.h"
#include "handler.h"
#include "message.h"
class Buff_handler : public Handler
{
private:
    std::vector<Handler *> handler_list;
    //返回下一条信息起点
    char *split_message(char *buff)
    {
        int i = 0;
        while (buff[i] != 0 && buff[i] != Message::split_header)
        {
            i++;
        }
        // printf("%s......\n",buff+i+1);
        if (buff[i] == 0)
            return NULL;
        else
        {
            buff[i] = 0;
            return (buff + i + 1);
        }
    }

public:
    Buff_handler() {}
    ~Buff_handler() {}
    virtual void handle(Message &message, Message::send_type type, int fd);

    void append_handler(Handler *handler)
    {
        handler_list.push_back(handler);
    }
};

void Buff_handler::handle(Message &message, Message::send_type type, int fd)
{
    char *buff = message.buff;
    while (true)
    {
        buff = split_message(buff);
        if(buff==NULL)break;
        Message temp(buff);
        type = temp.get_message_header();

        for (auto i : handler_list)
        {
            i->handle(temp, type, fd);
        }
    }
}