**# socket_instant_messaging**

v1 是硬写上来的，大概实现的是server和client之间的可靠传输，只是对每个message进行了编号，client收到消息后会回发信息说明收到。server和client之间每秒互传信号，表示还在连接。



v2 是重构过的，不过功能上应该是会写成消息队列。目前大概是重构完成，client向server发送message，server会按顺序将message下发给某一个已连接的client。



不过目前server还是单机版的



之后大概是要在v2上再加一个namesrv，把多个server弄成一个server group，client订阅相应的server group并获取信息。



# Getting started

```
cd ./v2
```

start server

```shell
g++ -pthread server.cpp -o server && ./server -p 8010 -i 127.0.0.1
```

start client

```shell
g++ -pthread client.cpp -o client &&./client -i 127.0.0.1 -p 8010
```



