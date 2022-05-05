#include "client_socket.h"
#include "buff_handler.h"
#include "message_handler_client.h"
#include "heartbeat_handler.h"
#include "client_message_back_handler.h"
#include "sender.h"

static int client_socketfd;

void stopServerRunning(int p)
{
    printf("Close\n");
    close(client_socketfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    std::pair<const char *, int> address = getaddress(argc, argv);
    const char *ip = address.first;
    int port = address.second;
    printf("listinging on %s:%d\n", ip, port);

    Client_socket client_socket;
    client_socketfd = client_socket.init_socket_create(port, ip);
    if (client_socketfd == -1)
        return 0;

    //TODO:get address from namesrv


    Buff_handler buff_handler;
    Handler *message_handler_client = new Message_handler_client();
    Handler *heartbeat_handler = new Heartbeat_handler();
    Handler *client_message_back_handler = new Client_message_back_handler();
    buff_handler.append_handler(message_handler_client);
    buff_handler.append_handler(heartbeat_handler);
    buff_handler.append_handler(client_message_back_handler);

    Sender sender(client_socket.fd);
    sender.start_heartbeat();
    sender.start_send();
    while (true)
    {
        signal(SIGINT, stopServerRunning); // 这句用于在输入Ctrl+C的时候关闭服务器
        client_socket.recive(client_socket.fd);
        // printf("%s\n",client_socket.message->buff);
        buff_handler.handle(*(client_socket.message), Message::unknow, client_socketfd);
        // break;
    }
}