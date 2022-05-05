#pragma once
#include "common.h"
#include "keeper.h"
#include "message.h"
class Address_storehouse : public Keeper
{
private:
    std::unordered_map<int, Message *> address;
    Address_storehouse() {}
    static Address_storehouse *address_storehouse;

public:
    static Address_storehouse *get_address_storehouse();
    void store(int fd, Message *message);
    void remove(int fd);
    Message* get_address_by_fd(int fd);
    virtual void delete_connect(int fd);
    virtual void new_connect(int fd) {}
};
Address_storehouse *Address_storehouse::address_storehouse = NULL;
Address_storehouse *Address_storehouse::get_address_storehouse()
{
    if (address_storehouse == NULL)
    {
        address_storehouse = new Address_storehouse();
    }
    return address_storehouse;
}

void Address_storehouse::delete_connect(int fd)
{
    get_address_storehouse()->address.erase(fd);
}

void Address_storehouse::store(int fd, Message *message)
{
    delete address[fd];
    address[fd] = message;
}
void Address_storehouse::remove(int fd)
{
    delete address[fd];
    address.erase(fd);
}
Message* Address_storehouse::get_address_by_fd(int fd)
{
    return address[fd];
}