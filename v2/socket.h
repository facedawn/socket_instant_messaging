#pragma once
#include "common.h"
#include "message.h"
static const int BUFFSIZE = 2048;
class Socket
{

public:
    // char buff[BUFFSIZE];
    Message *message = new Message();
    virtual int init_socket_create(int port, const char *ip) { return -1; }
    virtual void recive(int fd);
    virtual void send_message(int fd, char *buff);
};

void Socket::recive(int fd)
{
    memset(message->buff, 0, BUFFSIZE);
    recv(fd, message->buff, BUFFSIZE - 1, 0);
    
    if (strlen(message->buff) > 3)
        printf("%s...\n", message->buff);
}

void Socket::send_message(int fd, char *buff)
{
    send(fd, buff, strlen(buff), MSG_NOSIGNAL);
}