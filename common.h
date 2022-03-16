#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <thread>

#define PREFIX 3

enum send_type{
    message=0,
    heartbeat=100
};

void append(char *buff,const char *str)
{
    int initlen=strlen(buff);
    int sumlen=initlen+strlen(str);
    for(int i=initlen;i<sumlen;i++)
    {
        buff[i]=str[i-initlen];
    }
}

void set_message_header(char *buff,send_type header)
{
    if(header==message)
    {
        append(buff,"000");
    }
    else if(header==heartbeat)
    {
        append(buff,"100");
    }
}

send_type get_message_header(char* buff)
{
    int now=0;
    for(int i=0;i<3;i++)
    {
        now=now*10+(buff[i]-'0');
    }
    return (send_type)now;
}