#include "functions.h"

int maxi, num_user;
std::map<std::string, Client_info> name_client;
pollfd client[OPEN_MAX];
std::vector<broadcast_msg> b_msg;
std::map<std::string, channel_info> channels;

void init() 
{
    maxi = 0; num_user = 0;
    name_client.clear();
    for (int i = 1; i < OPEN_MAX; i++) client[i].fd = -1; /* -1: available entry */
    b_msg.clear();
}

void tolower_str(char *str) 
{
    for (int i = 0; ; i++) 
    {
        if (str[i] == '\0') return;
        str[i] = tolower(str[i]);
    }
}

void close_client(int index) 
{
    int connfd = client[index].fd;

    close(connfd);
    client[index].fd = -1;
    num_user--;

    for (auto it = name_client.begin(); it != name_client.end(); it++) {
        if (it->second.connfd == connfd) {
            name_client.erase(it);
            break;
        }
    }
}