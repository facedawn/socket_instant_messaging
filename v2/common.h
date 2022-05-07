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
#include <memory>
#include <stdio.h>
#include <atomic>
#include <unordered_set>

class Configure
{
public:
    static const char* namesrv_ip;
    static int namesrv_port;

    static const char* server_ip;
    static int server_port;

    static const char *group;
    static const char *ip;
    static int port;
};
const char* Configure::namesrv_ip="127.0.0.1";
int Configure::namesrv_port=12345;
const char* Configure::server_ip="127.0.0.1";
int Configure::server_port=1234;
const char* Configure::group="default";
const char* Configure::ip="127.0.0.1";
int Configure::port=1234;

int char2int(char *buff)
{
    int ans = 0;
    int len = strlen(buff);
    for (int i = 0; i < len; i++)
    {
        ans = ans * 10 + buff[i] - '0';
    }
    return ans;
}

std::pair<const char *, int> getaddress(int argc, char *argv[])
{
    int opt;
    const char *optstring = "i:p:g:";
    const char *ip = "127.0.0.1";
    int port=1234;
    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        if (opt == 'p')
        {
            port=char2int(optarg);
            Configure::port = port;
        }
        else if (opt == 'i')
        {
            ip=optarg;
            Configure::ip = optarg;
        }
        else if (opt == 'g')
        {
            Configure::group = optarg;
        }
        // printf("opt=%c\n", opt);
        // printf("optarg=%s\n", optarg);
        // printf("optind=%d\n", optind);
        // printf("argv[optind-1]=%s\n\n", argv[optind - 1]);
    }
    return {ip, port};
}