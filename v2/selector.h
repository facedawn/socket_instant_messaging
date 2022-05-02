#include "common.h"
#include "handler.h"
#include "keeper.h"
class Selector : public Keeper
{
private:
    fd_set readfds, readfds_back;
    struct timeval timeout;

public:
    Selector()
    {
        FD_ZERO(&readfds_back);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    }
    void new_connect(int fd)
    {
        FD_SET(fd, &readfds_back);
    }
    void select_fds()
    {
        readfds = readfds_back;
        int res = 0;
        res = select(1024, &readfds, NULL, NULL, &timeout);
        if (res == -1)
        {
            printf("select error\n");
        }
    }
    bool ready_on(int fd)
    {
        return FD_ISSET(fd, &readfds);
    }

    virtual void delete_connect(int fd)
    {
        FD_CLR(fd, &readfds_back);
    }
};