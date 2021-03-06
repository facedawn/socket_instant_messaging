#pragma once
#include "common.h"
#include "handler.h"
#include "keeper.h"
#include "heartbeat_storehouse.h"

class Heartbeat_handler : public Handler, Keeper
{
private:
    std::unordered_map<int, int> heartbeat_cnt;
    std::vector<Keeper *> keeper_list;

public:
    const static int MAX_HEARTBEAT_CNT = 2;
    Heartbeat_handler();
    virtual void delete_connect(int fd);
    virtual void new_connect(int fd);
    virtual void handle(Message &message, Message::send_type type, int fd);
    void append_keeper(Keeper *keeper) { keeper_list.push_back(keeper); }
    void all_new_connect(int fd);
};

Heartbeat_handler::Heartbeat_handler()
{
    auto heartbeat_dec_ps = [&]()
    {
        while (true)
        {
            sleep(1);
            for (auto i : Heartbeat_storehouse::get_heartbeat_storehouse()->heartbeat_cnt)
            {
                Heartbeat_storehouse::get_heartbeat_storehouse()->heartbeat_cnt[i.first] = (--i.second);
                // printf("%d:%d\n",i.first,i.second);
                if (i.second < 0)
                {
                    printf("close:%d\n", i.first);

                    // Heartbeat_storehouse::get_heartbeat_storehouse()->heartbeat_cnt.erase(i.first);
                    delete_connect(i.first);
                    for (auto keeper : keeper_list)
                    {
                        keeper->delete_connect(i.first);
                    }
                    close(i.first);
                }
            }
        }
    };
    std::thread heartbeat_send(heartbeat_dec_ps);
    heartbeat_send.detach();
}

void Heartbeat_handler::new_connect(int fd)
{
    printf("%d heartbeat connect\n", fd);
    Heartbeat_storehouse::get_heartbeat_storehouse()->heartbeat_cnt[fd] = MAX_HEARTBEAT_CNT;
}

void Heartbeat_handler::delete_connect(int fd)
{
    printf("heartbeat.\n");
    Heartbeat_storehouse::get_heartbeat_storehouse()->heartbeat_cnt.erase(fd);
}

void Heartbeat_handler::handle(Message &message, Message::send_type type, int fd)
{
    if (type != Message::heartbeat)
        return;
    // printf("recive heartbeat from %d\n",fd);
    Heartbeat_storehouse::get_heartbeat_storehouse()->heartbeat_cnt[fd] = MAX_HEARTBEAT_CNT;
}

void Heartbeat_handler::all_new_connect(int fd)
{
    for (auto i : keeper_list)
    {
        i->new_connect(fd);
    }
}