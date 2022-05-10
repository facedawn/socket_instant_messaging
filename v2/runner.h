#pragma once
#include "common.h"
#include "message.h"

class Runner
{
public:
    virtual void run(Message &message)
    {
        printf("recv: %s\n", message.buff + Message::PREFIX - 1);
    }
};