#pragma once 
#include "common.h"
#include "handler.h"
#include "message_storehouse.h"

class Server_message_back_handler : public Handler{
    virtual void handle(Message &message, Message::send_type type,int fd);
};

void Server_message_back_handler::handle(Message &message, Message::send_type type,int fd)
{
    if(type!=Message::client_message_back){
        return;
    }
    std::vector<int>index=message.spilt_string2int(',');

    int nowindex=index[0];
    (*Message_storehouse::get_message_storehouse()->has_recived)[nowindex]=-1;
}