#include "common.h"
#include "message.h"

class Heartbeat_storehouse
{
private:
    static Heartbeat_storehouse *heartbeat_storehouse;
    Heartbeat_storehouse(){};

public:
    std::unordered_map<int, int> heartbeat_cnt;
    static Heartbeat_storehouse *get_heartbeat_storehouse();
};
Heartbeat_storehouse* Heartbeat_storehouse::heartbeat_storehouse=NULL;
Heartbeat_storehouse* Heartbeat_storehouse::get_heartbeat_storehouse()
{
    if(heartbeat_storehouse==NULL)
    {
        heartbeat_storehouse=new Heartbeat_storehouse();
    }
    return heartbeat_storehouse;
}