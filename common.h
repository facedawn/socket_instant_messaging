#pragma once 
#include <iostream>
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
#include <map>
#include <string>
#include <unordered_map>
#include <list>
#include <set>
#include <vector>
using namespace std;

#define PREFIX 3
#define MAX_LOSE_HEARTBEAT_TIME 5
#define MAX_RECONNECT_TIME 5
#define BUFFSIZE 2048
#define HEADER_LENGTH 3

enum send_type
{
    message = 0,
    heartbeat = 100,
    set_channal = 200,
    set_username = 201
};

void append(char *buff, const char *str)
{
    int initlen = strlen(buff);
    int sumlen = initlen + strlen(str);
    for (int i = initlen; i < sumlen; i++)
    {
        buff[i] = str[i - initlen];
    }
}

void set_message_header(char *buff, send_type header)
{
    if (header == message)
    {
        append(buff, "000");
    }
    else if (header == heartbeat)
    {
        append(buff, "100");
    }
    else if(header == set_username)
    {
        append(buff,"201");
    }
}

send_type get_message_header(char *buff)
{
    int now = 0;
    for (int i = 0; i < 3; i++)
    {
        now = now * 10 + (buff[i] - '0');
    }
    return (send_type)now;
}
/*
class buff_class
{
public:
    send_type header;
    char message_buff[BUFFSIZE];
    char tostring_ans[10];

    buff(){bzero(message_buff,BUFFSIZE);}

    void set_message_header(send_type header)
    {
        tostring((int)header);
        for (int i = 0; i < HEADER_LENGTH; i++)
        {
            message_buff[i] = tostring_ans[i];
        }
    }

    void get_message_header()
    {
        int now = 0;
        for (int i = 0; i < HEADER_LENGTH; i++)
        {
            now = now * 10 + (message_buff[i] - '0');
        }
        header = (send_type)now;
    }

    void set_message_info(char *info)
    {
        int i = HEADER_LENGTH;
        while (info[i - HEADER_LENGTH] != 0)
        {
            message_buff[i] = info[i - HEADER_LENGTH];
        }
    }

    char *get_message_info()
    {
        return message_buff + HEADER_LENGTH;
    }

    void clear()
    {
        bzero(message_buff,BUFFSIZE);
    }

    void append(const char *str)
    {
        int initlen = strlen(message_buff);
        int sumlen = initlen + strlen(str);
        for (int i = initlen; i < sumlen; i++)
        {
            message_buff[i] = str[i - initlen];
        }
    }

    char *tostring(int a)
    {
        int cnt = 0;
        while (a > 0)
        {
            tostring_ans[cnt++] = a % 10 + '0';
            a = a / 10;
        }
        tostring_ans[cnt++] = 0;
        // printf("%s\n",tostring_ans);
        return tostring_ans;
    }
};
*/