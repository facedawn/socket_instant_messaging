#pragma once
#include "common.h"

class Message
{

public:
    const static char split_header = '|';
    const static char split_ender = '#';
    const static char index_header = '&';
    const static int PREFIX = 4;
    // std::shared_ptr<char> sptr = std::make_shared<char>(200);

    char *buff;
    bool has_memory;
    Message(int size=2048)
    {
        buff = new char[size];
        has_memory = true;
        clear();
    }
    Message(char *buff, int flag = 0) //默认浅拷贝
    {
        if (flag == 0)
        {
            this->buff = buff;
            has_memory = false;
        }
        else
        {
            int len = strlen(buff);
            this->buff = new char[len + 10];
            clear();
            for (int i = 0; i < len; i++)
            {
                this->buff[i] = buff[i];
            }
            this->buff[len]=0;
            has_memory = true;
        }
    }

    virtual ~Message()
    {
        if (has_memory)
        {
            delete[] buff;
            buff = NULL;
        }
        else
        {
            buff = NULL;
        }
    }
    enum send_type
    {
        unknow = 0,
        message = 1,
        heartbeat = 100,
        message_index = 101,
        server_message_back = 102,
        client_message_back = 103,
        address = 104,
        set_channal = 200,
        set_username = 201,
        set_address = 202,
        set_group=203,
        get_address=302
    };

    send_type get_message_header()
    {
        // printf("%s\n-------------\n",buff);
        int now = 0;
        for (int i = 0; i < 3; i++)
        {
            now = now * 10 + (buff[i] - '0');
        }
        // printf("%d....\n",now);
        return (send_type)now;
    }

    void append(const char *str)
    {
        int initlen = strlen(buff);
        int sumlen = initlen + strlen(str);
        for (int i = initlen; i < sumlen; i++)
        {
            buff[i] = str[i - initlen];
        }
        buff[sumlen]=0;
    }
    void append(char ch)
    {
        int initlen = strlen(buff);
        buff[initlen] = ch;
        buff[initlen+1]=0;
    }
    void clear()
    {
        memset(buff, 0, sizeof(buff));
    }
    void set_split_header()
    {
        append(split_header);
    }
    void set_message_header(send_type header)
    {
        set_split_header();
        if (header == message)
        {
            append("001");
        }
        else if (header == heartbeat)
        {
            append("100");
        }
        else if (header == message_index)
        {
            append("101");
        }
        else if (header == server_message_back)
        {
            append("102");
        }
        else if (header == client_message_back)
        {
            append("103");
        }
        else if(header==address)
        {
            append("104");
        }
        else if (header == set_username)
        {
            append("201");
        }
        else if(header==set_address)
        {
            append("202");
        }
        else if(header==set_group)
        {
            append("203");
        }
        else if(header==get_address)
        {
            append("302");
        }
    }

    void set_message(send_type type, char *buff)
    {
        clear();
        set_message_header(type);
        append(buff);
    }

    std::vector<int> spilt_string2int(char ch)
    {
        std::vector<int> ans;
        int now = 0;
        int i = Message::PREFIX - 1;
        while (buff[i] != 0)
        {
            for (i = Message::PREFIX - 1; buff[i] != 0 && buff[i] != ch; i++)
            {
                now = now * 10 + buff[i] - '0';
            }
            ans.push_back(now);
            now = 0;
        }
        return ans;
    }

    void replace_header(send_type type,int startpos)//不能替换成message
    {
        const char *header=std::to_string(type).c_str();
        int len=strlen(header);
        int cnt=0;
        for(int i=startpos;i<startpos+len;i++,cnt++)
        {
            buff[i]=header[cnt];
        }
    }

    std::vector<char*>split(char ch)
    {
        std::vector<char*>ans;
        ans.push_back(buff+PREFIX-1);
        int len=strlen(buff);
        for(int i=PREFIX;i<len;i++)
        {
            if(buff[i]==ch)
            {
                buff[i]=0;
                ans.push_back(buff+i+1);
            }
        }
        return ans;
    }
};