#include "handler.h"
#include "message_storehouse.h"

class Message_handler : public Handler
{
public:
    virtual void handle(Message &message, Message::send_type type, int fd);
};
void Message_handler::handle(Message &message, Message::send_type type, int fd)
{
    if (type != Message::message)
        return;
    if (strlen(message.buff) > 3)
    {
        // printf("recv: %s\n", message.buff);

        message.buff[-1]=Message::split_header;//由于split时将分隔符去掉了，目前buff头部没有分隔符，得加上。  并且这里-1位置是绝对有空间的，就是原来分隔符的空间
        Message* store_message=new Message(message.buff-1,1);
        // printf("address::%d\n",store_message);
        Message_storehouse::get_message_storehouse()->store(store_message);//得新建一个message，原来的message会被销毁
    }
}