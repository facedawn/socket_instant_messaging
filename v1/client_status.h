#include "common.h"
class client_status{
public:
    int fd;

    client_status(int now_fd)
    {
        fd=now_fd;
    }
};