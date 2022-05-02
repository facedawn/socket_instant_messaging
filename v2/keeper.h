#pragma once
#include "common.h"

class Keeper
{
public:
    virtual void delete_connect(int fd)
    {
        printf("delete!\n");
    };
    virtual void new_connect(int fd){
        printf("new!\n");
    }
};