#include "common.h"
#include "client_status.h"
class user{
    string username;
    vector<pair<int,int>>message_index;
    client_status *client=NULL;

    void user_connect(client_status *new_client,fd_set &readfds_back)
    {
        //TODO:old_fd close
        client_status *old_client=client;
        if(client!=NULL)
        {
            old_client->now_channel->someone_leave(old_client->fd);
            int fd=client->fd;
            FD_CLR(fd,&readfds_back);
            close(fd);
        }
        client=new_client;
        delete old_client;
    }
}