#pragma once
#include "common.h"
#include "message.h"
#include "storehouse.h"

class Message_storehouse : public Storehouse
{

public:
    static Message_storehouse *message_storehouse;
    int maxsize;
    int nownum;
    int round;
    std::unordered_map<int, Message *> *message_index;
    std::unordered_map<int, int> *has_recived;

    static Message_storehouse *get_message_storehouse();
    int store(Message *message);
    void remove(int index);

private:
    Message_storehouse(int maxsize=10000)
    {
        nownum = 0;
        round = 0;
        this->maxsize = maxsize;
        message_index = new std::unordered_map<int, Message *>(maxsize);
        has_recived = new std::unordered_map<int, int>(maxsize);
    };
};
Message_storehouse *Message_storehouse::message_storehouse=nullptr;

Message_storehouse *Message_storehouse::get_message_storehouse()
{
    if (Message_storehouse::message_storehouse == nullptr)
    {
        Message_storehouse::message_storehouse = new Message_storehouse();
    }
    return Message_storehouse::message_storehouse;
}

int Message_storehouse::store(Message *message)
{
    if (round > 0)
    {
        delete message_index->at(nownum);
    }
    message_index->insert({nownum, message});
    has_recived[nownum] = 0;
    if (nownum >= maxsize)
    {
        nownum -= maxsize;
        ++round;
    } //要不要留备份接口呢emmm

    return (nownum + maxsize - 1) % maxsize;
}

void Message_storehouse::remove(int index)
{
    has_recived.erase(index);
}