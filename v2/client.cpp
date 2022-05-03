#include "client_socket.h"
#include "buff_handler.h"
#include "message_handler_client.h"
#include "heartbeat_handler.h"
#include "sender.h"

static const char *ip = "127.0.0.1";
static const int port = 1234;
static int client_socketfd;

void stopServerRunning(int p)
{
    printf("Close\n");
    close(client_socketfd);
    exit(0);
}
char b[2048];
int main()
{
    Client_socket client_socket;
    client_socketfd = client_socket.init_socket_create(port, ip);

    Buff_handler buff_handler;
    Handler* message_handler_client=new Message_handler_client();
    Handler* heartbeat_handler=new Heartbeat_handler();
    buff_handler.append_handler(message_handler_client);
    buff_handler.append_handler(heartbeat_handler);

    Sender sender(client_socket.fd);
    sender.start_heartbeat();
    sender.start_send();
    while (true)
    {
        signal(SIGINT, stopServerRunning); // 这句用于在输入Ctrl+C的时候关闭服务器
        client_socket.recive(client_socket.fd);
        //printf("%s\n",client_socket.message->buff);
        buff_handler.handle(*(client_socket.message),Message::unknow,client_socketfd);
        // break;
    }
}