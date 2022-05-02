#include "common.h"
#include <map>
#include <unordered_map>
#include <ctime>
using namespace std;

struct hashfunc
{
    template <typename T, typename U>
    size_t operator()(const pair<T, U> &i) const
    {
        return hash<T>()(i.first) ^ hash<U>()(i.second);
    }
};

class Client_message_index
{

public:
    int lasttime = 0;
    int nowuse = 0;
    char back_buff[BUFFSIZE];
    std::unordered_map<pair<int, int>,int,hashfunc> um[2];

    int now_temp_index_start_ptr;

    Client_message_index()
    {
        lasttime = std::time(nullptr);
        nowuse = 0;
    };

    int nextuse()
    {
        nowuse = (nowuse + 1) % 2;
        return nowuse;
    }

    pair<int, int> get_message_index(char *buff)
    {
        int f_startindex = 0;
        int s_startindex = 0;
        for (int i = 0; buff[i] != 0; i++)
        {
            if (buff[i] == index_header)
            {
                f_startindex = i + 5;
            }
            if (buff[i] == ',' && f_startindex != 0)
            {
                s_startindex = i + 1;
                buff[i] = 0;
            }
        }
        int end=strlen(buff+s_startindex);
        buff[s_startindex+end-1]=0;
        int first = char2int(buff+f_startindex);
        int second = char2int(buff+s_startindex);
        buff[s_startindex-1] = ',';
        buff[s_startindex+end-1]=')';
        
        now_temp_index_start_ptr=f_startindex-1;
        // printf("buff:%s\ntest:(%d,%d) %d %d %d %c %c __%s\n",buff,first,second,now_temp_index_start_ptr,f_startindex,s_startindex,buff[f_startindex],buff[s_startindex],buff+s_startindex);
        
        return {first, second};
    }

    bool has(pair<int, int> &index)
    {
        return (um[0].find(index) != um[0].end()) || (um[1].find(index) != um[1].end());
    }

    void insert(pair<int, int> index)
    {
        if (std::time(nullptr) - lasttime > 15)
        {
            um[nextuse()].clear();
        }
        um[nowuse].insert({index, std::time(nullptr)%1000000007});
    }

    void send_back(int& client_socketfd,char*buff_ptr)
    {
        memset(back_buff, 0, sizeof(back_buff));
        back_buff[0] = split_header;
        set_message_header(back_buff + 1, send_type::message_back);
        int cnt = now_temp_index_start_ptr;
        for (int i = PREFIX; buff_ptr[cnt] != 0; cnt++, i++)
        {
            back_buff[i] = buff_ptr[cnt];
        }

        // printf("back_buff:%s %d\n",back_buff,cnt);
        send(client_socketfd, back_buff, strlen(back_buff), MSG_NOSIGNAL);
    }

    int get(pair<int,int>index)
    {
        if(um[nowuse].find(index)!=um[nowuse].end())
            return um[nowuse][index];
        return -1;
    }
};