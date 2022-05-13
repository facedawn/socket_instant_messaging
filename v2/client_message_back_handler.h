#pragma once 
#include "common.h"
#include "handler.h"
#include "message_storehouse.h"
#include "sender.h"

class Client_message_back_handler : public Handler{
    private:
    Message temp_buff;
    public:
    virtual void handle(Message &message, Message::send_type type,int fd);
};


void Client_message_back_handler::handle(Message &message, Message::send_type type,int fd)
{
    if(type!=Message::server_message_back){
        return;
    }
    
    temp_buff.clear();
    temp_buff.set_message_header(Message::client_message_back);
    temp_buff.append(message.buff+Message::PREFIX-1);

    // send(fd,temp_buff.buff,strlen(temp_buff.buff),MSG_NOSIGNAL);
    // sleep(1);
    Sender::mysend(fd,temp_buff.buff);
}