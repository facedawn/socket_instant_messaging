#include "handler.h"

class Message_handler : public Handler
{
public:
    virtual void handle(Message &message);
};
void Message_handler::handle(Message &message)
{
    printf("recv: %s\n", message.buff);
}