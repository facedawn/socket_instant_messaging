#include "server_socket.h"
#include "selector.h"
#include "buff_handler.h"
#include "message.h"
#include "message_handler.h"
#include "heartbeat_handler.h"
#include "server_message_back_handler.h"
#include "sender.h"
#include "set_address_handler.h"
#include "set_group_handler.h"
#include "get_address_handler.h"
#include "address_storehouse.h"
#include "group_storehouse.h"

static int server_socketfd;
void stopServerRunning(int p)
{
    printf("Close\n");
    close(server_socketfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    std::pair<const char*,int>address= getaddress(argc, argv);
    const char *ip=address.first;
    int port=address.second;
    printf("namesrv start...\nlistinging on %s:%d\n", ip,port);

    Server_socket server_socket;
    server_socketfd = server_socket.init_socket_create(port);

    Selector *selector = new Selector();
    selector->new_connect(server_socket.fd);

    Handler *buff_handler = new Buff_handler();
    Handler *message_handler = new Message_handler();
    Handler *heartbeat_handler = new Heartbeat_handler();
    Handler *server_message_back_handler = new Server_message_back_handler();
    Handler *set_address_handler=new Set_address_handler();
    Handler *set_group_handler=new Set_group_handler();
    Handler *get_address_handler=new Get_address_handler();

    ((Buff_handler *)buff_handler)->append_handler(message_handler);
    ((Buff_handler *)buff_handler)->append_handler(heartbeat_handler);
    ((Buff_handler *)buff_handler)->append_handler(server_message_back_handler);
    ((Buff_handler *)buff_handler)->append_handler(set_address_handler);
    ((Buff_handler *)buff_handler)->append_handler(set_group_handler);
    ((Buff_handler *)buff_handler)->append_handler(get_address_handler);

    Sender *sender = new Sender(server_socketfd);

    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)heartbeat_handler);
    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)selector);
    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)sender);
    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)Address_storehouse::get_address_storehouse());
    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)Group_storehouse::get_group_storehouse());
    while (true)
    {
        signal(SIGINT, stopServerRunning); // 这句用于在输入Ctrl+C的时候关闭服务器
        selector->select_fds();
        for (int i = 0; i < 1024; i++)
        {
            if (!selector->ready_on(i))
            {
                continue;
            }
            if (server_socket.fd == i)
            {
                int new_client = server_socket.accept_new_connect(i);

                if (new_client == -1)
                {
                    continue;
                }
                printf("%d connect\n", new_client);
                ((Heartbeat_handler *)heartbeat_handler)->all_new_connect(new_client);
            }
            else
            {
                server_socket.recive(i);
                buff_handler->handle(*server_socket.message, Message::unknow, i);
            }
        }
        sender->send_message_storehouse();
        // break;
    }
}