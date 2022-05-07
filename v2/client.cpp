#include "client_socket.h"
#include "buff_handler.h"
#include "message_handler_client.h"
#include "heartbeat_handler.h"
#include "client_message_back_handler.h"
#include "sender.h"
#include "address_handler.h"

static int client_socketfd;

void stopServerRunning(int p)
{
    printf("Close\n");
    close(client_socketfd);
    exit(0);
}

void get_server_address(Client_socket &client_socket, Buff_handler &buff_handler, const char *namesrv_ip, int namesrv_port)
{
    client_socketfd = client_socket.init_socket_create(namesrv_port, namesrv_ip);
    Handler *address_handler = new Address_handler();
    buff_handler.append_handler(address_handler);
    Message get_message;
    get_message.clear();
    get_message.set_message_header(Message::get_address);
    get_message.append(Configure::group);

    for (int i = 0; i < 5; i++)
    {
        printf("%d-th try.\n", i);
        while (((Address_handler *)address_handler)->server_port == -1)
        {
            sleep(1);
            Sender::mysend(client_socketfd, get_message.buff);
            client_socket.recive(client_socket.fd);
            
            printf("%s\n",client_socket.message->buff);
            buff_handler.handle(*(client_socket.message), Message::unknow, client_socketfd);
        }
        if (((Address_handler *)address_handler)->server_port != 0)
        {
            break;
        }
        ((Address_handler *)address_handler)->server_port = -1;
    }
    Configure::server_ip = ((Address_handler *)address_handler)->server_ip;
    Configure::server_port = ((Address_handler *)address_handler)->server_port;
    printf("%s:%d\n", Configure::server_ip, Configure::server_port);
    close(client_socketfd);
}

int main(int argc, char *argv[])
{
    std::pair<const char *, int> address = getaddress(argc, argv);
    const char *ip = address.first;
    int port = address.second;
    printf("connecting namesrv to %s:%d\n", ip, port);

    Buff_handler buff_handler;
    Client_socket client_socket;

    get_server_address(client_socket, buff_handler, ip, port);
    if (Configure::server_port == 0||Configure::server_port==-1)
    {
        printf("dont find server.\n");
        return 0;
    }

    printf("connecting server to %s:%d\n", Configure::server_ip, Configure::server_port);
    client_socketfd = client_socket.init_socket_create(Configure::server_port, Configure::server_ip);
    if (client_socketfd == -1)
        return 0;

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