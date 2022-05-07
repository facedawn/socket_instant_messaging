#pragma once
#include "common.h"
#include "handler.h"
#include "message.h"

class Address_handler:public Handler{
    public:
    const char* server_ip;
    int server_port;
    Address_handler(){server_port=-1;}
    virtual void handle(Message &message, Message::send_type type,int fd);
};

void Address_handler::handle(Message &message,Message::send_type type,int fd)
{
    if(type!=Message::address){
        return;
    }

    Message* copy_message=new Message(message.buff,1);
    std::vector<char*>ans=copy_message->split(',');
    this->server_ip=ans[0];
    this->server_port=char2int(ans[1]);
}