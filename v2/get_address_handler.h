#include "common.h"
#include "handler.h"
#include "group_storehouse.h"
#include "address_storehouse.h"
#include "sender.h"

class Get_address_handler : public Handler
{
    virtual void handle(Message &message, Message::send_type type, int fd);
};

void Get_address_handler::handle(Message &message, Message::send_type type, int fd) //传入订阅的group_name
{
    if (type != Message::get_address)
    {
        return;
    }
    printf("get_group:%s\n", message.buff);
    message.buff[-1] = Message::split_header;
    std::string group_name(message.buff + 3);
    int server_fd = Group_storehouse::get_group_storehouse()->rand_select_fd_in_group(group_name);

    if (server_fd == -1)
    {
        Message *temp_message = new Message();
        temp_message->set_message_header(Message::address);
        temp_message->append("0,0");
        Sender::mysend(fd, temp_message->buff);
        return;
    }
    Message *address = Address_storehouse::get_address_storehouse()->get_address_by_fd(server_fd);
    printf("send address:%s\n", address->buff);
    Sender::mysend(fd, address->buff);
}