#include "client.h"
int main()
{
    const char* namesrv_ip="127.0.0.1";
    int namesrv_port=12345;
    //you can define your own runner class, just need to rewrite run() which decide what you want to do after you recive message
    Client *client=new Client(namesrv_ip,namesrv_port);
    Message message;
    message.clear();
    message.set_message_header(Message::message);

    message.append("1234123");
    
    for(int i=0;i<10;i++)
    {
        client->send_message(message.buff);
    }
    client->close_connection();   
}