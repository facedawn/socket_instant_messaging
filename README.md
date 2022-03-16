# socket_instant_messaging

该仓库只是学习使用socket并且尝试select时随手写的，代码结构很乱，以后有闲心可能会重构一下emmm。

### 编译运行

Linux系统下进入到相应目录。

编译运行server

```shell
g++ -pthread server.cpp -o server && ./server
```



编译运行client

```shell
g++ -pthread client.cpp -o client && ./client
```



需要先运行server再运行client



## 使用

server没有输入线程，不提供任何操作。

client中直接输入再回车就可以向server发送信息，server会将信息转发给所有连接的client。

server和client连接采用心跳机制维持。client异常退出时，server会检测到并且断开连接。



## 代码逻辑

### server

server有两个线程。

一个主线程进行socket创建，连接，以及使用select（）进行非阻塞的网络IO（select好像最多支持1024个连接）。每次select会处理FD_SET内的数据，留下有数据到达的连接号，我们找到这些连接号进行读取并处理。

另一个线程是与心跳机制相关的。这个线程对于每一个连接维护一个计数器，每秒钟每个计数器都减一，当计数器<=0时，视为server与client断开连接。每当收到一个心跳信息，相对应的计数器会重置。

由于多线程，发现server与client断开之后，需要保持连接一段时间，以避免另一线程出错。在这里又建立了一个线程进行连接的关闭。



### client

client有三个线程。

主线程建立连接并接收信息。

input线程读取本地输入并发出信息。

heartbeat_thread线程用来发送心跳信号。

