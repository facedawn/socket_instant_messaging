#pragma once 
#include "common.h"
#include "message.h"

class Storehouse{
    public:
    Storehouse(){}
    virtual int store(Message message){return -1;}
    virtual void remove(int index){}
    virtual ~Storehouse(){}
};