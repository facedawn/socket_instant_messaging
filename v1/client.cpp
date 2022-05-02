#include "client.h"
#include "client_message_index.h"
int client_socketfd;
char buff[BUFFSIZE];
char sendbuff[BUFFSIZE];
char heartbeatbuff[BUFFSIZE];
char back_buff[BUFFSIZE];

Client_message_index client_message_index;

int socket_create()
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        printf("socket 创建失败\n");
        return -1;
    }
    else
    {
        printf("socket 创建成功\n");
        return socketfd;
    }
}

int socket_connect(int socketfd)
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = IP_FAMILY;
    inet_pton(IP_FAMILY, SERVER_IP, &server_addr.sin_addr);
    server_addr.sin_port = htons(DEFAULT_PORT);

    printf("connecting...\n");
    int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
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
    while (true)
    {
        scanf("%s", sendbuff + PREFIX);
        set_split_header(sendbuff);
        set_message_header(sendbuff, message);
        set_end(sendbuff);
        if (strlen(sendbuff) != 0)
            send(client_socketfd, sendbuff, strlen(sendbuff), 0);
        bzero(sendbuff, BUFFSIZE);
    }
}

void send_heartbeat()
{
    set_split_header(heartbeatbuff);
    set_message_header(heartbeatbuff, heartbeat);
    set_end(sendbuff);
    while (true)
    {
        send(client_socketfd, heartbeatbuff, strlen(heartbeatbuff), MSG_NOSIGNAL);
        sleep(1);
    }
}

int heartbeat_left_time;
void heartbeat_left_time_dec_ps()
{
    while (true)
    {
        // printf("%d\n",heartbeat_left_time);
        if (heartbeat_left_time <= -MAX_RECONNECT_TIME)
        {
            printf("reconnect fail\n");
            close(client_socketfd);
            printf("Close Client\n");
            exit(0);
        }
        else if (heartbeat_left_time <= 0 && heartbeat_left_time > -5)
        {
            printf("lose connection\n");
            --heartbeat_left_time;
        }
        else
        {
            --heartbeat_left_time;
        }
        sleep(1);
    }
}

int main()
{
    std::string username;
    printf("请输入用户名：\n");
    cin >> username;
    std::thread input(&input_message);
    input.detach();

    heartbeat_left_time = MAX_LOSE_HEARTBEAT_TIME;

    signal(SIGINT, stopServerRunning); // 这句用于在输入Ctrl+C的时候关闭服务器

    client_socketfd = socket_create();
    if (-1 == client_socketfd)
        return -1;

    if (-1 == socket_connect(client_socketfd))
    {
        close(client_socketfd);
        return -1;
    }
    int cnt = 0;

    bzero(buff, BUFFSIZE);
    set_split_header(buff);
    set_message_header(buff, set_username);
    for (auto i : username)
    {
        buff[cnt + PREFIX] = i;
        cnt++;
    }
    buff[cnt + PREFIX] = 0;
    set_end(buff);
    send(client_socketfd, buff, strlen(buff), 0);
    //printf("username:%s %d\n", buff + PREFIX, strlen(buff));
    bzero(buff, BUFFSIZE);

    sleep(1); //防止username和心跳混到一起
    std::thread heartbeat_thread(&send_heartbeat);
    heartbeat_thread.detach();
    std::thread heartbeat_dec(&heartbeat_left_time_dec_ps);
    heartbeat_dec.detach();

    while (true)
    {
        memset(buff,0,sizeof(buff));
        recv(client_socketfd, buff, BUFFSIZE - 1, 0);
        // printf("%s........\n",buff);
        char* buff_ptr=buff;
        while (buff_ptr != NULL)
        {
            char *newbuff = split(buff_ptr);
            
            if (strlen(buff_ptr) >= 3)
            {
                send_type stype = get_message_header(buff_ptr);
                // printf("%s %d\n",buff_ptr,stype);
                if (stype == message)
                {
                    pair<int,int>nowindex=client_message_index.get_message_index(buff_ptr);
                    
                    if(client_message_index.get(nowindex)==std::time(nullptr)%1000000007)
                    {
                        continue;
                    }
                    if(!client_message_index.has(nowindex))
                        printf("Recv: %s\n", buff_ptr + HEADER_LENGTH);

                    client_message_index.send_back(client_socketfd,buff_ptr);
                    client_message_index.insert(nowindex);
                }
                else if (stype == heartbeat)
                {
                    heartbeat_left_time = MAX_LOSE_HEARTBEAT_TIME;
                }
                else if(stype==send_type::message_index)
                {
                    memset(back_buff,0,sizeof(back_buff));
                    back_buff[0]=split_header;
                    set_message_header(back_buff+1,send_type::message_back);
                    int cnt=HEADER_LENGTH;
                    for(int i=PREFIX;buff_ptr[cnt]!=0;cnt++,i++)
                    {
                        back_buff[i]=buff_ptr[cnt];
                    }
                    send(client_socketfd,back_buff,strlen(back_buff),MSG_NOSIGNAL);
                }
            }
            buff_ptr = newbuff;
        }
        bzero(buff, BUFFSIZE);
    }
    close(client_socketfd);
}