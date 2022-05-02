#include "socket.h"

class Client_socket : public Socket
{
private:
    int socket_create()
    {
        int socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketfd == -1)
        {
            printf("socket 创建失败\n");
            return -1;
        }
        else
        {
            printf("socket 创建成功\n");
            return socketfd;
        }
    }

    int socket_connect(int socketfd, const char *ip, int port)
    {
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &server_addr.sin_addr);
        server_addr.sin_port = htons(port);

        printf("connecting...\n");
        int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (-1 == connect_status)
        {
            printf("Connect error(%d): %s\n", errno, strerror(errno));
            close(socketfd);
        }
        else
        {
            printf("connect success\n");
        }
        return connect_status;
    }

public:
    int init_socket_create(int port, const char *ip);
    int fd;
};
int Client_socket::init_socket_create(int port, const char *ip)
{
    int client_socketfd = socket_create();
    if (-1 == client_socketfd)
        return -1;

    if (-1 == socket_connect(client_socketfd, ip, port))
    {
        close(client_socketfd);
        return -1;
    }
    this->fd = client_socketfd;
    return client_socketfd;
}