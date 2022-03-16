#include "server.h"
#include <list>
#include <map>
#include <unordered_map>
using namespace std;
int client_socketfd;
int socketfd;
list<int>client_socketfd_list[2];
int now_list_num=0;

unordered_map<int,int>heartbeat_cnt;

char buff[BUFFSIZE];
char heartbeat_buff[BUFFSIZE];
int maxfd=-1;


fd_set readfds_back;
fd_set writefds_back;
fd_set errorfds_back;
fd_set readfds,writefds,errorfds;

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
    for(auto client_socketfd:client_socketfd_list[now_list_num])
    {
        close(client_socketfd);
    }
    printf("Close Server\n");
    exit(0);
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
    for(auto client_socketfd:client_socketfd_list[now_list_num])
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
    for(auto client_socketfd:client_socketfd_list[now_list_num])
    {
        maxfd=max(maxfd,client_socketfd);
    }
    return maxfd;
}

void heartbeat_dec_thread()
{            
    while(true){
        for(auto i=client_socketfd_list[now_list_num].begin();i!=client_socketfd_list[now_list_num].end();i++)
        {
            int x=(*i);
            if(x!=socketfd && --heartbeat_cnt[x]<=0)
            {
                printf("%d off\n",x);
                client_socketfd_list[now_list_num].erase(i--);
                FD_CLR(x,&readfds_back);
                //close(x);
            }
        }
        sleep(1);
    }
}



int main()
{
    std::thread heartbeat_dec(&heartbeat_dec_thread);
    heartbeat_dec.detach();

    socketfd=socket_create();
    if(socketfd==-1)return -1;

    int flags=fcntl(socketfd,F_GETFL,0);
    fcntl(socketfd,F_SETFL,flags|O_NONBLOCK);

    if(socket_bind(socketfd)==-1)return -1;

    if(socket_listen(socketfd)==-1)return -1;
    printf("Listening...\n");

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
        timeout.tv_sec=2;
        timeout.tv_usec=0;
        client_socketfd_list[now_list_num].push_back(socketfd);
        maxfd=update_maxfd();
        //printf("maxfd:%d\n",maxfd);
        int res=0;
        res=select(maxfd+1,&readfds,NULL,NULL,&timeout);
        //printf("select  res:%d\n",res);
        if(res==-1){printf("select error\n");return -1;}
        for(int i=0;i<=1023;i++)
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
                client_socketfd_list[now_list_num].push_back(client_socketfd);

                heartbeat_cnt[client_socketfd]=5;

                maxfd=update_maxfd();
            }
            else
            {
                bzero(buff,BUFFSIZE);
                recv(i,buff,BUFFSIZE-1,0);
                int stype=get_message_header(buff);
                //set_message_header(buff,message);
                if(strlen(buff)>3&&stype==message)
                {
                    printf("recv: %s from %d\n",buff+PREFIX,i);
                    sendtoall(buff,writefds_back,i);
                }
                else if(stype==heartbeat)
                {
                    //收到心跳，维持连接
                    //map计数，每秒-1，每次收到心跳重置为5
                    int next_list_num=1-now_list_num;
                    //printf("recv heartbeat from %d\n",i);
                    heartbeat_cnt[i]=5;
                }
                bzero(buff,BUFFSIZE);
            }
        }
        // now_list_num=1-now_list_num;
    }
}