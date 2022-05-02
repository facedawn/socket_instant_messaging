#pragma once
#include "message.h"
class Handler{
private:
    //应该是一堆unordered map
    
public:
    Handler(){}
    ~Handler(){}
    virtual void handle(Message &message){}
};
