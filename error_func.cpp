#include "functions.h"

bool check_nick_name(int client_index, char *nick_name) 
{
    // no nickname
    if (nick_name == NULL) {
        char error[MSG_SIZE];
        memset(error, '\0', MSG_SIZE);
        sprintf(error, ":%s 431 : No nickname given\n", SERVER_NAME);
        write(client[client_index].fd, error, strlen(error));
        return true;
    }
    
    // check conflict nickname
    if (name_client.find(nick_name) != name_client.end()) 
    {
        char error[MSG_SIZE];
        memset(error, '\0', MSG_SIZE);
        sprintf(error, ":%s 436 %s :Nickname collision KILL\n", SERVER_NAME, nick_name);
        write(client[client_index].fd, error, strlen(error));
        return true;
    }
    
    return false;
}

void not_enough_args(char *command) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 461 %s :Not enough parameters\n", SERVER_NAME, command);
}

void not_registerd(int connfd, std::string nick_name) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 451 %s :You have not registered\n", SERVER_NAME, nick_name.data());
    write(connfd, error, strlen(error));
}

void not_on_channel(int connfd, std::string channel)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 442 %s %s :You're not on that channel\n", SERVER_NAME, fd_name[connfd].data(), channel.data());
    write(connfd, error, strlen(error));
}

void no_recipient(char *command, int connfd)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 411 %s :No recipient given (%s)\n", SERVER_NAME, fd_name[connfd].data(), command);
    write(connfd, error, strlen(error));
}

void no_text_send(int connfd)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 412 %s :No text to send\n", SERVER_NAME, fd_name[connfd].data());
    write(connfd, error, strlen(error));
}

void no_such_channel(int connfd, std::string channel)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 403 %s %s :No such channel\n", SERVER_NAME, fd_name[connfd].data(), channel.data());
    write(connfd, error, strlen(error));
}

void error_cmd(char *command, int connfd) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 421 %s %s :Unknown command\n", SERVER_NAME, fd_name[connfd].data(), command);
    write(connfd, error, strlen(error));
}