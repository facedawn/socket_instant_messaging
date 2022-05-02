#pragma once
#include "common.h"
#include "client_status.h"
#include "message_factory.h"
class user
{
public:
    string username;
    vector<pair<int, int>> message_index; // TODO:改成set，所有信息先进一次，只有用户端收到之后才会删掉
    client_status *client = NULL;
    int status = 0; // 0 off   ,   1 on
    char addbuff[BUFFSIZE];

    user(string username)
    {
        this->username = username;
        client = NULL;
    }

    void send_message_index(pair<int, int> index)
    {

        char addbuff[BUFFSIZE];
        memset(addbuff, 0, sizeof(addbuff));
        addbuff[0] = split_header;
        set_message_header(addbuff + 1, send_type::message_index);
        string adds = "(" + to_string(index.first) + "," + to_string(index.second) + ")";
        append(addbuff, adds.c_str());

        send(client->fd, addbuff, strlen(addbuff), MSG_NOSIGNAL);
    }

    void send_all(Message_factory &message_factory)
    {
        for (auto i : message_index)
        {
            // printf("enter\n");
            // printf("(%d %d):", i.first, i.second);
            char *buff = message_factory.get_message_by_index(i);
            buff = message_factory.buff;

            
            memset(addbuff, 0, sizeof(addbuff));
            addbuff[0] = index_header;
            set_message_header(addbuff + 1, send_type::message_index);
            string adds = "(" + to_string(i.first) + "," + to_string(i.second) + ")";

            append(addbuff, adds.c_str());
            addbuff[adds.size()+PREFIX]=0;
            int end_index=adds.size()+PREFIX+strlen(buff);
            append(buff, addbuff);
            buff[end_index]=0;
            printf("send:%s\n",buff);///
            send(client->fd, buff, strlen(buff), MSG_NOSIGNAL);
        }
    }

    void user_connect(client_status *new_client, Message_factory &message_factory)
    {
        client = new_client;
        this->status = 1;
        send_all(message_factory);
    }

    void recive(char *buff, pair<int, int> index)
    {
        // if (status == 1)
        // {
        //     char addbuff[BUFFSIZE];
        //     memset(addbuff, 0, sizeof(addbuff));
        //     addbuff[0] = split_header;
        //     set_message_header(addbuff + 1, send_type::message_index);
        //     string adds = "(" + to_string(index.first) + "," + to_string(index.second) + ")";
        //     append(addbuff, adds.c_str());
        //     append(buff, addbuff);

        //     send(client->fd, buff, strlen(buff), MSG_NOSIGNAL);
        //     printf("%s\n", buff);
        // }
        message_index.push_back(index);
    }
};