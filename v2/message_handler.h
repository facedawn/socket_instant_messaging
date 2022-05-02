#include "handler.h"

class Message_handler : public Handler
{
public:
    virtual void handle(Message &message, Message::send_type type,int fd);
};
void Message_handler::handle(Message &message, Message::send_type type,int fd)
{
    if(type!=Message::message)return;
    if (strlen(message.buff) > 3)
        printf("recv: %s\n", message.buff);
}