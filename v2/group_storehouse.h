#pragma once
#include "common.h"
#include "keeper.h"
#include "message.h"

class Group_storehouse : public Keeper
{
private:
    std::unordered_map<int, Message *> fa;
    std::unordered_map<std::string, std::unordered_set<int>> group;
    
    Group_storehouse() {}
    static Group_storehouse *group_storehouse;

public:
    static Group_storehouse *get_group_storehouse();
    void store(int fd, Message *message);
    void remove(int fd);
    char *get_group_by_fd(int fd);
    virtual void delete_connect(int fd);
    virtual void new_connect(int fd) {}
    int rand_select_fd_in_group(std::string &group_name);
};
Group_storehouse * Group_storehouse::group_storehouse = NULL;
Group_storehouse *Group_storehouse::get_group_storehouse()
{
    if (group_storehouse == NULL)
    {
        group_storehouse = new Group_storehouse();
    }
    return group_storehouse;
}
void Group_storehouse::store(int fd, Message *message)//带分隔符
{
    delete fa[fd];
    fa[fd] = message;
    std::string group_name(message->buff + Message::PREFIX-1);
    group[group_name].insert(fd);
}
void Group_storehouse::remove(int fd)
{
    std::string group_name((fa[fd]->buff) + 3);
    group[group_name].erase(fd);
    if (group[group_name].size() == 0)
    {
        group.erase(group_name);
    }
    Message *temp = fa[fd];
    fa.erase(fd);
    delete temp;
}

void Group_storehouse::delete_connect(int fd)
{
    remove(fd);
}

int Group_storehouse::rand_select_fd_in_group(std::string &group_name)
{
    int len = group[group_name].size();
    if (len == 0)
    {
        group.erase(group_name);
        return -1;
    }
    int rand = random() % len;
    int nowfd;
    for (auto i : group[group_name])
    {
        nowfd = i;
        if ((rand--) == 0)
            break;
    }
    return nowfd;
}