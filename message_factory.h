#pragma once
#include "common.h"
#include <string>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
class Message_factory{
public:
    map<pair<int,int>,string*> message_storehouse;//TODO: link sql
    int nowround=0;
    int nownum=0;
    int mod=1e9+7;
    char buff[2048];

    pair<int,int> create_message(char* buff)
    {
        buff=buff-1;
        *buff=split_header;
        printf("%s.......save\n",buff);
        if(get_message_header(buff+1)!=send_type::message)return {-1,-1};
        message_storehouse.insert({{nowround,nownum},new string(buff)});
        pair<int,int>ret={nowround,nownum};
        nownum++;
        nowround=nownum/mod;
        nownum=nownum%mod;
        return ret;
    }

    char* get_message_by_index(pair<int,int>index)
    {
        map<pair<int,int>,string*>::iterator iter=message_storehouse.find(index);
        if(iter==message_storehouse.end())return NULL;

        string* message_ptr=(*iter).second;
        int len=message_ptr->size();
        for(int i=0;i<len;i++)
        {
            this->buff[i]=message_ptr->at(i);
        }
        this->buff[len]=0;
        // printf("%s\n",this->buff);
        return this->buff;
    }
};