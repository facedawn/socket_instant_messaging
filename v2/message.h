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
    Message()
    {
        buff = new char[2048];
        has_memory = true;
    }
    Message(char *buff)
    {
        this->buff = buff;
        has_memory = false;
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
        message_back = 102,
        set_channal = 200,
        set_username = 201,
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
    }
    void clear()
    {
        memset(buff, 0, sizeof(buff));
    }
    void set_split_header()
    {
        buff[0] = split_header;
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
        else if (header == message_back)
        {
            append("102");
        }
        else if (header == set_username)
        {
            append("201");
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
        int i = Message::PREFIX-1;
        while (buff[i] != 0)
        {
            for (i = Message::PREFIX-1; buff[i] != 0 && buff[i] != ch; i++)
            {
                now = now * 10 + buff[i] - '0';
            }
            ans.push_back(now);
            now = 0;
        }
        return ans;
    }
};