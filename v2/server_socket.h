#include "socket.h"

class Server_socket : public Socket
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

    int socket_bind(int socketfd, int port)
    {
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);
        int bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bind_status == -1)
        {
            printf("bind error(%d): %s\n", errno, strerror(errno));
        }
        return bind_status;
    }

    int socket_listen(int socketfd)
    {
        int listen_status = listen(socketfd, 2048);
        return listen_status;
    }

    int socket_accept(int socketfd)
    {
        struct sockaddr_in client_addr;
        bzero(&client_addr, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);
        int accept_status = accept(socketfd, NULL, NULL);
        // if(accept_status==-1)
        // {
        // printf("accept error(%d):%s",errno,strerror(errno));
        // }
        return accept_status;
    }

public:
    int fd;

    //建立非阻塞的socket,并且监听相应端口
    Server_socket() {}
    int accept_new_connect(int fd);
    int init_socket_create(int port, const char *ip);
};

int Server_socket::init_socket_create(int port, const char *ip="127.0.0.1")
{
    int socketfd = socket_create();
    if (socketfd == -1)
        return -1;
    int flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);

    if (socket_bind(socketfd, port) == -1)
        return -1;

    if (socket_listen(socketfd) == -1)
        return -1;

    this->fd = socketfd;
    return socketfd;
}

int Server_socket::accept_new_connect(int fd)
{
    int client_socketfd = socket_accept(fd);
    if (client_socketfd == -1)
    {
        return -1;
    }
    return client_socketfd;
}