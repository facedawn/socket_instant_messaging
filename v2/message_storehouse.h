#pragma once
#include "common.h"
#include "message.h"

class Message_storehouse
{

public:
    static Message_storehouse *message_storehouse;
    int maxsize;
    int nownum;
    int round;
    std::unordered_map<int, Message *> *message_index;
    std::unordered_map<int, int> *has_recived;

    static Message_storehouse *get_message_storehouse(int maxsize);
    int store(Message *message);
    void remove(int index);

private:
    Message_storehouse(int maxsize)
    {
        nownum = 0;
        round = 0;
        this->maxsize = maxsize;
        message_index = new std::unordered_map<int, Message *>(maxsize+10);
        has_recived = new std::unordered_map<int, int>(maxsize+10);
    };
};
Message_storehouse *Message_storehouse::message_storehouse = nullptr;

Message_storehouse *Message_storehouse::get_message_storehouse(int maxsize=10000)
{
    if (Message_storehouse::message_storehouse == nullptr)
    {
        Message_storehouse::message_storehouse = new Message_storehouse(maxsize);
    }
    return Message_storehouse::message_storehouse;
}

int Message_storehouse::store(Message *message)
{
    if (nownum >= maxsize)
    {
        nownum -= maxsize;
        ++round;
    }
    //存储时添加index，方便client_message_back
    message->append(Message::split_header);
    message->set_message_header(Message::server_message_back);
    message->append(std::to_string(nownum).c_str());

    if (round > 0)
    {
        delete message_index->at(nownum);
        (*message_index)[nownum] = message;
        (*has_recived)[nownum] = 0;
    }
    else
    {
        (*message_index)[nownum] = message;
        (*has_recived)[nownum] = 0;
    }
    // printf("%d:\n",nownum);
    // printf("%s...(1))\n",message->buff);
    //要不要留备份接口呢emmm
    return (nownum++);
}

void Message_storehouse::remove(int index)
{
    has_recived->erase(index);
}