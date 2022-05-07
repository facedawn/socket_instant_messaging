#include "common.h"
#include "handler.h"
#include "keeper.h"
#include "group_storehouse.h"

class Set_group_handler : public Handler
{
private:
public:
    virtual void handle(Message &message, Message::send_type type, int fd);
};

void Set_group_handler::handle(Message &message, Message::send_type type, int fd)
{
    if (type != Message::set_group)
    {
        return;
    }

    // printf("set_group:%s\n",message.buff);
    Group_storehouse::get_group_storehouse()->store(fd,new Message(message.buff,1));
}