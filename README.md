**# socket_instant_messaging**

v1 是硬写上来的，大概实现的是server和client之间的可靠传输，只是对每个message进行了编号，client收到消息后会回发信息说明收到。server和client之间每秒互传信号，表示还在连接。



v2 是重构过的，不过功能上应该是会写成消息队列。目前大概是重构完成，client向server发送message，server会按顺序将message下发给某一个已连接的client。



不过目前server还是单机版的



之后大概是要在v2上再加一个namesrv，把多个server弄成一个server group，client订阅相应的server group并获取信息。


# Requirement

linux
g++

# Getting started

```
cd ./v2
```

start namesrv

namesrv地址在Configure.h内配置,namesrv和server启动时都会从Configure.h读取相应配置

```shell
g++ -pthread namesrv.cpp -o namesrv && ./namesrv
```

start server

```shell
g++ -pthread server.cpp -o server && ./server -p <server_port> -i <server_ip>
```

start client

```shell
g++ -pthread client.cpp -o client &&./client -i <namesrv_ip> -p <namesrv_port> -g <group_name>
```

or use like testmain.cpp

```c++
#include "client.h"
#include "client.h"
int main()
{
    const char* namesrv_ip="127.0.0.1";
    int namesrv_port=12345;
    Runner *runner=new Runner();
    //you can define your own runner class, just need to rewrite run() which decide what you want to do after you recive message
    Client *client=new Client(namesrv_ip,namesrv_port);
    client->start(runner);
}
```
and then 

```shell
g++ -pthread testmain.cpp -o testmain &&./testmain
```

sender
```c++
#include "client.h"
int main()
{
    const char* namesrv_ip="127.0.0.1";
    int namesrv_port=12345;
    
    Client *client=new Client(namesrv_ip,namesrv_port);
    Message message;
    message.clear();
    message.set_message_header(Message::message);

    message.append("1234123");//your message
    
    for(int i=0;i<10;i++)//send 10 times
    {
        client->send_message(message.buff);
    }
    client->close_connection();   
}
```

# notes

需要先启动namesrv ，再server，最后client/sender

当使用多个server注册到同一个servergroup时，确保client数量多于group内server数量，不然有些数据接受不到。（只多出一点也是可能收不到的，目前是随机分配server，有可能server没有client连接，数据无法被消耗，之后需要再改改emm）


