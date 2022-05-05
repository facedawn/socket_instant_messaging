#include "common.h"
#include "handler.h"
#include "keeper.h"
#include "address_storehouse.h"
class Set_address_handler : public Handler
{
private:
public:
    Set_address_handler() {}
    ~Set_address_handler() {}
    virtual void handle(Message &message, Message::send_type type, int fd);
};
void Set_address_handler::handle(Message &message, Message::send_type type, int fd)
{
    if (type != Message::set_address)
    {
        return;
    }
    message.buff[-1] = Message::split_header;
    Message *new_message = new Message(message.buff - 1);
    message.replace_header(Message::address, 0);

    Address_storehouse::get_address_storehouse()->store(fd, new_message);
}