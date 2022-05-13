#include "server_socket.h"
#include "selector.h"
#include "buff_handler.h"
#include "message.h"
#include "message_handler.h"
#include "heartbeat_handler.h"
#include "server_message_back_handler.h"
#include "sender.h"
#include "client_socket.h"

static int server_socketfd;
void stopServerRunning(int p)
{
    printf("Close\n");
    close(server_socketfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    std::pair<const char *, int> address = getaddress(argc, argv);
    const char *ip = address.first;
    int port = address.second;
    printf("listinging on %s:%d   register:%s\n", ip, port, Configure::group);

    Server_socket server_socket;
    server_socketfd = server_socket.init_socket_create(port);

    // TODO:send_back

    Client_socket client_socket;
    int client_socketfd = client_socket.init_socket_create(Configure::namesrv_port, Configure::namesrv_ip);
    Message *set_message = new Message();
    set_message->set_message_header(Message::set_address);
    set_message->append(ip);
    set_message->append(',');
    set_message->append(std::to_string(port).c_str());
    set_message->append((char)0);

    set_message->set_message_header(Message::set_group);
    set_message->append(Configure::group);
    set_message->append((char)0);

    set_message->set_message_header(Message::heartbeat);

    Sender namesrv_sender(client_socketfd);
    namesrv_sender.start_heartbeat(set_message->buff);
    printf("set_group:%s\n", set_message->buff);
    // set_group

    Selector *selector = new Selector();
    selector->new_connect(server_socket.fd);

    Handler *buff_handler = new Buff_handler();
    Handler *message_handler = new Message_handler();
    Handler *heartbeat_handler = new Heartbeat_handler();
    Handler *server_message_back_handler = new Server_message_back_handler();

    ((Buff_handler *)buff_handler)->append_handler(message_handler);
    ((Buff_handler *)buff_handler)->append_handler(heartbeat_handler);
    ((Buff_handler *)buff_handler)->append_handler(server_message_back_handler);

    Sender *sender = new Sender(server_socketfd);

    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)heartbeat_handler);
    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)selector);
    ((Heartbeat_handler *)heartbeat_handler)->append_keeper((Keeper *)sender);
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
            // printf("%d...(0)\n",i);
            if (server_socket.fd == i)
            {
                int new_client = server_socket.accept_new_connect(i);

                if (new_client == -1)
                {
                    continue;
                }
                printf("%d connect\n", new_client);
                Heartbeat_storehouse::get_heartbeat_storehouse()->heartbeat_cnt[new_client] = Heartbeat_handler::MAX_HEARTBEAT_CNT;
                ((Heartbeat_handler *)heartbeat_handler)->new_connect(new_client);
                ((Heartbeat_handler *)heartbeat_handler)->all_new_connect(new_client);
            }
            else
            {
                server_socket.recive(i);
                // if (strlen((*server_socket.message).buff) > 3)
                // {
                //     printf("%s.....\n", (*server_socket.message).buff);
                // }
                buff_handler->handle(*server_socket.message, Message::unknow, i);
            }
        }
        sender->send_message_storehouse();
        // break;
    }
}