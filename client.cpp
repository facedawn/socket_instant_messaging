#include "client.h"

int client_socketfd;
char buff[BUFFSIZE];
char sendbuff[BUFFSIZE];
int socket_create()
{
    int socketfd=socket(AF_INET,SOCK_STREAM,0);
    if(socketfd==-1){
        printf("socket 创建失败\n");
        return -1;
    }
    else{
        printf("socket 创建成功\n");
        return socketfd;
    }
}

int socket_connect(int socketfd)
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family=IP_FAMILY;
    inet_pton(IP_FAMILY,SERVER_IP,&server_addr.sin_addr);
    server_addr.sin_port=htons(DEFAULT_PORT);

    printf("connecting...\n");
    int connect_status=connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == connect_status)
    {
        printf("Connect error(%d): %s\n", errno, strerror(errno));
        close(socketfd);
    }
    else
    {
        printf("connect success\n");
    }
    return connect_status;
}

void stopServerRunning(int p)
{
    close(client_socketfd);
    printf("Close Client\n");
    exit(0);
}

void input_message()
{
    while(true){
        scanf("%s",sendbuff);
        if(strlen(sendbuff)!=0)
            send(client_socketfd,sendbuff,strlen(sendbuff),0);
        bzero(sendbuff, BUFFSIZE);
    }
}

int main()
{
    std::thread input(&input_message);
    input.detach();
    while(true)
    {
        signal(SIGINT, stopServerRunning);    // 这句用于在输入Ctrl+C的时候关闭服务器

        client_socketfd=socket_create();
        if(-1==client_socketfd)return -1;

        if(-1==socket_connect(client_socketfd)){close(client_socketfd);return -1;}

        while(true)
        {
            //scanf("%s",buff);

            //send(client_socketfd,buff,strlen(buff),0);

            //bzero(buff, BUFFSIZE);
            
            recv(client_socketfd,buff,BUFFSIZE-1,0);
            if(strlen(buff)!=0)
                printf("Recv: %s\n",buff);    
            bzero(buff, BUFFSIZE);
        }
        close(client_socketfd);
    }
}