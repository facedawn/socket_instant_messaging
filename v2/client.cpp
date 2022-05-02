#include "client_socket.h"
#include "buff_handler.h"
#include "message_handler.h"
#include "sender.h"

const char *ip="127.0.0.1";
static const int port=1234;

void stopServerRunning(int p)
{
    printf("Close\n");
    
    exit(0);
}
char b[2048];
int main()
{
    Client_socket client_socket;
    client_socket.init_socket_create(port,ip);

    Buff_handler buff_handler;
    Message_handler message_handler;
    buff_handler.append_handler(message_handler);

    Sender sender(client_socket.fd);
    sender.start();
    sender.stop();
    while(true)
    {
        client_socket.recive(client_socket.fd);
        printf("%s...\n",client_socket.message->buff);
        buff_handler.handle(*(client_socket.message));
        // break;
    }
}