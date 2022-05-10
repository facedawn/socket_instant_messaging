#include "client.h"
int main()
{
    const char* namesrv_ip="127.0.0.1";
    int namesrv_port=12345;
    Runner *runner=new Runner();
    //you can define your own runner class, just need to rewrite run() which decide what you want to do after you recive message
    Client *client=new Client(namesrv_ip,namesrv_port);
    client->start(runner);
}