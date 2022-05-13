#include "client.h"
int main()
{
    const char* namesrv_ip="127.0.0.1";
    const char* group="facedawn";
    // const char* group="default";
    int namesrv_port=12345;
    //you can define your own runner class, just need to rewrite run() which decide what you want to do after you recive message
    Client *client=new Client(namesrv_ip,namesrv_port,group);
    Message message;
    message.clear();
    message.set_message_header(Message::message);

    message.append("1234123");
    
    for(int i=0;i<1000;i++)
    {
        message.clear();
        message.set_message_header(Message::message);

        message.append("group:");
        message.append(group);
        message.append(std::to_string(i).c_str());
        client->send_message(message.buff);
    }
    client->close_connection();   
} 