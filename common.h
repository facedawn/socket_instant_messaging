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
#include <deque>

using namespace std;

#define PREFIX 4
#define MAX_LOSE_HEARTBEAT_TIME 5
#define MAX_RECONNECT_TIME 5
#define BUFFSIZE 2048
#define HEADER_LENGTH 3
const char split_header = '|';
const char split_ender = '#';
const char index_header='&';

enum send_type
{
    message = 0,
    heartbeat = 100,
    message_index = 101,
    message_back = 102,
    set_channal = 200,
    set_username = 201,

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

void set_split_header(char *buff)
{
    buff[0] = split_header;
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
    else if (header == message_index)
    {
        append(buff, "101");
    }
    else if (header == message_back)
    {
        append(buff, "102");
    }
    else if (header == set_username)
    {
        append(buff, "201");
    }
    
}

send_type get_message_header(char *buff)
{
    // printf("%s\n-------------\n",buff);
    int now = 0;
    for (int i = 0; i < 3; i++)
    {
        now = now * 10 + (buff[i] - '0');
    }
    // printf("%d....\n",now);
    return (send_type)now;
}

void set_end(char *buff)
{
    buff[strlen(buff)] = split_ender;
}

char *split(char *buff)
{
    int i = 0;
    while (buff[i] != 0 && buff[i] != split_header)
    {
        i++;
    }
    // printf("%s......\n",buff+i+1);
    if (buff[i] == 0)
        return NULL;
    else
    {
        buff[i] = 0;
        return (buff + i + 1);
    }
}

int char2int(char* begin)
{

    int ans=0;
    for(int i=0;*(begin+i)!=0;i++)
    {
        ans=ans*10+(*(begin+i))-'0';
    }
    // printf("%s:%d\n",begin,ans);
    return ans;
}