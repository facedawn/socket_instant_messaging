#pragma once
#include "common.h"
#include "message.h"
class Sender
{
    std::atomic<int> end;
    Message *message;
    int client_socketfd;

    void input_message()
    {
        while (true)
        {
            message->set_message_header(Message::message);
            scanf("%s", message->buff + Message::PREFIX);
            if (strlen(message->buff) != 0)
                send(client_socketfd, message->buff, strlen(message->buff), 0);
            bzero(message->buff, BUFFSIZE);
        }
    }

public:
    Sender(int client_socketfd)
    {
        message = new Message();
        this->client_socketfd = client_socketfd;
        end = 0;
    }
    Sender(Sender &&a)
    {
        this->message = a.message;
        this->client_socketfd = a.client_socketfd;
        if (a.end == 0)
            this->end = 0;
        else
            this->end = 1;
    }
    ~Sender() { Sender::end = 1; }
    void set_client_socketfd(int client_socketfd) { this->client_socketfd = client_socketfd; }
    void start()
    {
        auto lambda_func = [&]()
        {
            while (true)
            {
                message->set_message_header(Message::message);
                scanf("%s", message->buff + Message::PREFIX);
                if (strlen(message->buff) != 0)
                    send(client_socketfd, message->buff, strlen(message->buff), 0);
                bzero(message->buff, BUFFSIZE);
            }
        };
        std::thread input(lambda_func);
        input.detach();
    }
    void stop()
    {
        Sender::end = 1;
    }
};