#include "server_socket.h"
#include "selector.h"
#include "buff_handler.h"
#include "message.h"
#include "message_handler.h"
static int server_socketfd;
void stopServerRunning(int p)
{
    printf("Close\n");
    close(server_socketfd);
    exit(0);
}
int main()
{

    Server_socket server_socket;
    server_socketfd = server_socket.init_socket_create(1234);

    Selector selector;
    selector.new_connect(server_socket.fd);

    Buff_handler buff_handler;
    Message_handler message_handler;

    buff_handler.append_handler(message_handler);

    while (true)
    {
        signal(SIGINT, stopServerRunning); // 这句用于在输入Ctrl+C的时候关闭服务器
        selector.select_fds();
        for (int i = 0; i < 1024; i++)
        {
            if (!selector.ready_on(i))
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
                selector.new_connect(new_client);
            }
            else
            {
                server_socket.recive(i);

                buff_handler.handle(*server_socket.message);
            }
        }
        // break;
    }
}