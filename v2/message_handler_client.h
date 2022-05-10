#include "handler.h"
#include "runner.h"

class Message_handler_client : public Handler
{
    private: Runner *runner;
public:
    Message_handler_client(){runner=new Runner();}
    Message_handler_client(Runner* runner){this->runner=runner;}
    virtual void handle(Message &message, Message::send_type type, int fd);
};
void Message_handler_client::handle(Message &message, Message::send_type type, int fd)
{
    if (type != Message::message)
        return;
    if (strlen(message.buff) > 3)
    {
        // printf("recv: %s\n", message.buff+Message::PREFIX-1);
        runner->run(message);
        //这里添加拿到消息后的进一步行动
    }
}