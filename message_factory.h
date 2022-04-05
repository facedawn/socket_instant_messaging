#include "common.h"
class message_factory{
    map<pair<int,int>,string*> message_storehouse;
    int nowround=0;
    int nownum=0;
    int mod=1e9+7;
    char buff[BUFFSIZE];

    bool create_message(char* buff)
    {
        if(get_message_header(buff)!=message)return false;
        message_storehouse.insert({{nowround,nownum},new string(buff+3)});
        nownum++;
        nowround=nownum/mod;
        nownum=nownum%mod;
        return true;
    }

    char* get_message_by_index(pair<int,int>index)
    {
        map<pair<int,int>,string*>::iterator iter=message_storehouse.find(index);
        if(iter==message_storehouse.end())return NULL;

        set_message_header(buff,message);
        string* message_ptr=message_storehouse[iter];
        int len=message_ptr->size();
        for(int i=0;i<len;i++)
        {
            buff[i+HEADER_LENGTH]=message_ptr->at(i);
        }
        return buff;
    }
};