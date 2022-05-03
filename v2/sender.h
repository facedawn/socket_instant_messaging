#pragma once
#include "common.h"
#include "message.h"
#include "message_storehouse.h"
#include "keeper.h"

// emm是不是该写成单例
class Sender : public Keeper
{
    Message *send_message_to_server;
    Message *send_message_to_client;
    Message *heartbeat_message;
    int client_socketfd;

    std::unordered_set<int> connections;
    std::unordered_set<int>::iterator connections_iter;

public:
    Sender(int client_socketfd);

    void set_client_socketfd(int client_socketfd) { this->client_socketfd = client_socketfd; }

    virtual void delete_connect(int fd);
    virtual void new_connect(int fd);

    void send_message_storehouse();
    void start_send();
    void start_heartbeat();
};
Sender::Sender(int client_socketfd)
{
    send_message_to_server = new Message();
    send_message_to_client = new Message();
    heartbeat_message = new Message();
    this->client_socketfd = client_socketfd;
    connections_iter = connections.begin();
}

void Sender::delete_connect(int fd)
{
    this->connections.erase(fd);
}
void Sender::new_connect(int fd)
{
    this->connections.insert(fd);
}

void Sender::send_message_storehouse() //这个函数是服务器使用
{
    std::unordered_map<int, int> *temp_recived_map = Message_storehouse::get_message_storehouse()->has_recived;
    std::unordered_map<int, Message *> *temp_index_map = Message_storehouse::get_message_storehouse()->message_index;

    auto last_iter = temp_recived_map->end();
    for (auto i : (*temp_recived_map))
    {
        //每次remove掉前一个，避免向后遍历时迭代器失效。
        if (last_iter != temp_recived_map->end() && (*last_iter).second == -1)
        {
            Message_storehouse::get_message_storehouse()->remove((*last_iter).first);
            ++last_iter;
        }
        else
        {
            last_iter = temp_recived_map->begin();
        }

        if (connections_iter == connections.end())
            connections_iter = connections.begin();
        auto tempfd = (*connections_iter);
        if ((++connections_iter) == connections.end())
            connections_iter = connections.begin();

        long long nowtime = time(NULL);
        if (i.second != nowtime && i.second != -1)
        {
            // printf("send (%d/%d):%s ... to %d  state:%d  nowtime:%d\n", i.first, temp_recived_map->size(), (*temp_index_map)[i.first]->buff, tempfd,i.second,nowtime);
            // printf("(%d)(%d)   (%d)(%d)\n",temp_recived_map,Message_storehouse::get_message_storehouse()->has_recived,temp_index_map,Message_storehouse::get_message_storehouse()->message_index);
            send(tempfd, (*temp_index_map)[i.first]->buff, strlen((*temp_index_map)[i.first]->buff), 0);

            (*temp_recived_map)[i.first] = nowtime;
            (*temp_recived_map)[i.first] = -1; //这个要丢到messageback
        }
    }
}

void Sender::start_send() //这个函数是客户端发送到服务器使用
{
    auto send_thread = [&]()
    {
        while (true)
        {
            send_message_to_server->clear();
            send_message_to_server->set_message_header(Message::message);
            scanf("%s", send_message_to_server->buff + Message::PREFIX);
            if (strlen(send_message_to_server->buff) != 0)
                send(client_socketfd, send_message_to_server->buff, strlen(send_message_to_server->buff), 0);
        }
    };
    std::thread input(send_thread);
    input.detach();
}

void Sender::start_heartbeat()
{
    auto heartbeat_thread = [&]()
    {
        while (true)
        {
            sleep(1);
            heartbeat_message->clear();
            heartbeat_message->set_message_header(Message::heartbeat);
            send(client_socketfd, heartbeat_message->buff, strlen(heartbeat_message->buff), 0);
        }
    };
    std::thread heartbeat(heartbeat_thread);
    heartbeat.detach();
}