#include "functions.h"

int maxi = 0, num_user = 0;
Client_info client_info[OPEN_MAX];
pollfd client[OPEN_MAX];
std::vector<broadcast_msg> b_msg;

void tolower_str(char *str) 
{
    for (int i = 0; ; i++) 
    {
        if (str[i] == '\0') return;
        str[i] = tolower(str[i]);
    }
}

void time_format(char* tt, int tt_size)
{  
    tm* cur;
    time_t curtime;
    time(&curtime);
    cur = localtime(&curtime);
    
    memset(tt, '\0', tt_size);
    sprintf(tt, "%-4d-%-2d-%-2d %-2d:%-2d:%-2d ",cur->tm_year+1900, cur->tm_mon+1, cur->tm_mday, cur->tm_hour, cur->tm_min, cur->tm_sec);    
}

void write_time(int index) 
{
    time_t curtime;
    time(&curtime);

    char tt[30];
    time_format(tt, sizeof(tt));

    int flg = 0;
    int connfd = client[index].fd;
    if (connfd < 0) return;

    flg = send(connfd, tt, sizeof(tt)-1, MSG_NOSIGNAL);
    
    if (flg < 0) close_client(index);
}

void reset_client(int index) 
{
    memset(client_info[index].nick_name, '\0', NAME_LENGTH);
    memset(client_info[index].user_name, '\0', NAME_LENGTH);
    memset(client_info[index].real_name, '\0', NAME_LENGTH);
}

void close_client(int index) 
{
    int connfd = client[index].fd;

    close(connfd);
    client[index].fd = -1;
    num_user--;

    broadcast_msg tmp;
    memset(tmp.msg, '\0', MSG_SIZE);
    sprintf(tmp.msg, "*** User <%s> has left the server\n", client_info[index].nick_name);
    tmp.set(-1);
    b_msg.push_back(tmp);

    printf("* client %s:%d disconnected\n", inet_ntoa(client_info[index].addr.sin_addr), client_info[index].addr.sin_port);
}
 
// void error_cmd(char *cmd, int to) 
// {
//     char msg[MSG_SIZE];
//     memset(msg, '\0', MSG_SIZE);
//     sprintf(msg, ":%s %s %s :Unknown command\n", SERVER_NAME, nnn, cmd);
//     write(client[to].fd, msg, sizeof(msg));
// }

// void err_sys(const char *err) 
// {
//     perror(err);
//     exit(1);
// }
