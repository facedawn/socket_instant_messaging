#include "server.h"
#include <list>
using namespace std;
int client_socketfd;
int socketfd;
list<int>client_socketfd_list;
char buff[BUFFSIZE];
char pix[BUFFSIZE];
int maxfd=-1;

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

int socket_bind(int socketfd)
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family=IP_FAMILY;
    server_addr.sin_addr.s_addr=htonl(IP_ADDR);
    server_addr.sin_port=htons(DEFAULT_PORT);
    int bind_status=bind(socketfd,(struct  sockaddr*)&server_addr,sizeof(server_addr));
    if(bind_status==-1)
    {
        printf("bind error(%d): %s\n",errno,strerror(errno));
    }
    return bind_status;
}

int socket_listen(int socketfd)
{
    int listen_status=listen(socketfd,BACKLOG);
    return listen_status;
}

int socket_accept(int socketfd)
{
    struct sockaddr_in client_addr;
    bzero(&client_addr,sizeof(client_addr));
    socklen_t len=sizeof(client_addr);
    int accept_status=accept(socketfd,NULL,NULL);

    
    
    // if(accept_status==-1)
    // {
        //printf("accept error(%d):%s",errno,strerror(errno));
    // }
    return accept_status;
}

void stopServerRunning(int p)
{
    for(auto client_socketfd:client_socketfd_list)
    {
        close(client_socketfd);
    }
    printf("Close Server\n");
    exit(0);
}

void append(char *buff,const char *str)
{
    int initlen=strlen(buff);
    int sumlen=initlen+strlen(str);
    for(int i=initlen;i<sumlen;i++)
    {
        buff[i]=str[i-initlen];
    }
}

char tostring_ans[10];
char *tostring(int a)
{
    int cnt=0;
    while(a>0)
    {
        tostring_ans[cnt++]=a%10+'0';
        a=a/10;
    }
    tostring_ans[cnt++]=0;
    //printf("%s\n",tostring_ans);
    return tostring_ans;
}

void sendtoall(char *buff,fd_set &writefds,int srcfds)
{
    for(auto client_socketfd:client_socketfd_list)
    {
        if (client_socketfd==socketfd||client_socketfd==srcfds)continue;
        
        if(strlen(buff)!=0)
        {
            append(buff," from ");append(buff,tostring(srcfds));
        //printf("sendto:%d %d\n",client_socketfd,socketfd);
        
            send(client_socketfd,buff,strlen(buff),0);
        }
    }
}

int update_maxfd()
{
    int maxfd=0;
    for(auto client_socketfd:client_socketfd_list)
    {
        maxfd=max(maxfd,client_socketfd);
    }
    return maxfd;
}

int main()
{
    socketfd=socket_create();
    if(socketfd==-1)return -1;

    int flags=fcntl(socketfd,F_GETFL,0);
    fcntl(socketfd,F_SETFL,flags|O_NONBLOCK);

    if(socket_bind(socketfd)==-1)return -1;

    if(socket_listen(socketfd)==-1)return -1;
    printf("Listening...\n");

    fd_set readfds_back;
    fd_set writefds_back;
    fd_set errorfds_back;
    fd_set readfds,writefds,errorfds;
    FD_ZERO(&readfds_back);
    FD_ZERO(&writefds_back);
    FD_ZERO(&errorfds_back);
    FD_SET(socketfd,&readfds_back);
    struct timeval timeout;
    timeout.tv_sec=1;
    timeout.tv_usec=0;


    while(true)
    {
        signal(SIGINT, stopServerRunning);    // 这句用于在输入Ctrl+C的时候关闭服务器
        
        //client_socketfd=socket_accept(socketfd);
        
        readfds=readfds_back;
        writefds=writefds_back;
        errorfds=errorfds_back;
        timeout.tv_sec=1;
        timeout.tv_usec=0;
        client_socketfd_list.push_back(socketfd);
        maxfd=update_maxfd();
        //printf("maxfd:%d\n",maxfd);
        int res=0;
        res=select(maxfd+1,&readfds,NULL,NULL,&timeout);
        //printf("select  res:%d\n",res);
        if(res==-1){printf("select error\n");return -1;}
        for(int i=0;i<=maxfd;i++)
        {
            if(!FD_ISSET(i,&readfds))continue;
            if(i==socketfd)
            {
                client_socketfd=socket_accept(i);
                if(client_socketfd==-1){continue;}
                printf("accept success newfds:%d\n",client_socketfd);
                FD_SET(client_socketfd,&readfds_back);
                FD_SET(client_socketfd,&writefds_back);
                FD_SET(client_socketfd,&errorfds_back);
                client_socketfd_list.push_back(client_socketfd);
                maxfd=update_maxfd();
            }
            else
            {
                bzero(buff,BUFFSIZE);
                recv(i,buff,BUFFSIZE-1,0);
                if(strlen(buff)!=0)
                    printf("recv: %s from %d\n",buff,i);
                sendtoall(buff,writefds_back,i);
                bzero(buff,BUFFSIZE);
            }
        }
        //send(client_socketfd,buff,strlen(buff),0);
    }
}