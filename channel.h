#pragma once
#include "common.h"
#include "user.h"
#include "message_factory.h"
class channel{
public:
    channel* root;
    std::string now_node_val;
    std::unordered_map<std::string,channel*>son_channel;
    channel* fa;
    std::set<user*>connections;
    int connection_cnt;

    channel(std::string new_channel_name)
    {
        root=NULL;
        now_node_val=new_channel_name;
        connection_cnt=0;
    }

    channel(channel *fa,std::string new_channel_name)
    {
        root=fa->root;
        now_node_val=fa->now_node_val+"/"+new_channel_name;
        connection_cnt=0;
    }

    bool find_in_local_channel(std::string son_channel_name)
    {
        if(son_channel.find(son_channel_name)==son_channel.end()){
            return false;
        }
        return true;
    }

    bool exist_son_channel(std::string son_channel_name)
    {
        int cnt=0;
        for(auto i:son_channel_name)
        {
            if(i=='/')break;
            cnt++;
        }
        return find_in_local_channel(son_channel_name.substr(0,cnt))&&exist_son_channel(son_channel_name.substr(cnt));
    }

    void create_new_channel(std::string new_channel_name)
    {
        son_channel[new_channel_name]=new channel(this,new_channel_name);
        son_channel[new_channel_name]->fa=this;
    }

    channel* get_son_channel(std::string channel_name)
    {
        if(son_channel.find(channel_name)!=son_channel.end())
        {
            return son_channel[channel_name];
        }
        return NULL;
    }

    void delete_all_son_channel()
    {
        for(auto &i:son_channel)
        {
            delete i.second;
        }
    }

    bool delete_son_channel(std::string channel_name)
    {
        if(son_channel.find(channel_name)!=son_channel.end())
        {
            son_channel[channel_name]->delete_all_son_channel();
            delete son_channel[channel_name];
            return son_channel.erase(channel_name);
        }
    }

    channel* enter_channel(std::string channel_path)
    {
        int cnt=0;
        for(auto i:channel_path){
            if(i=='/')break;
            ++cnt;
        }
        channel* son_channel=get_son_channel(channel_path.substr(0,cnt));
        return son_channel->enter_channel(channel_path.substr(cnt+1));
    }

    channel* enter_channel_from_root(std::string channel_path)
    {
        int cnt=0;
        for(auto i:channel_path){
            if(i=='/')break;
            ++cnt;
        }
        channel* son_channel=root->get_son_channel(channel_path.substr(0,cnt));
        return son_channel->enter_channel(channel_path.substr(cnt+1));
    }

    void someone_enter(user* client_user)
    {
        connections.insert(client_user);
        ++connection_cnt;
    }

    void someone_leave(user* client_user)
    {
        auto i=connections.find(client_user);
        if(i!=connections.end())
        {
            connections.erase(client_user);
        }
    }

    void send_only_local_cannel(char *buff,pair<int,int>index)
    {   
        //printf("%d emm\n",connections.size());
        for(auto i:connections)
        {
            i->recive(buff,index);
            //send(i,buff,strlen(buff),MSG_NOSIGNAL);
        }
    }

    void send_to_all_son_channel(char *buff,pair<int,int>index)
    {
        send_only_local_cannel(buff,index);
        for(auto i:son_channel)
        {
            i.second->send_to_all_son_channel(buff,index);
        }
    }
};