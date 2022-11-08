#include "functions.h"

void no_such_channel(int connfd, std::string channel)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 403 %s %s :No such channel\n", SERVER_NAME, fd_name[connfd].data(), channel.data());
    write(connfd, error, strlen(error));
}

void no_recipient( int connfd, char *command)
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

void error_cmd(int connfd, char *command) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 421 %s %s :Unknown command\n", SERVER_NAME, fd_name[connfd].data(), command);
    write(connfd, error, strlen(error));
}

bool check_nick_name(int connfd, char *nick_name) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    
    // no nickname
    if (nick_name == NULL) {
        sprintf(error, ":%s 431 : No nickname given\n", SERVER_NAME);
        write(connfd, error, strlen(error));
        return true;
    }
    
    // invalid nickname
    if (strlen(nick_name) > 9)
    {
        sprintf(error, ":%s 432 %s :Erroneus nickname\n", SERVER_NAME, nick_name);
        write(connfd, error, strlen(error));
        return true;
    }

    // check conflict nickname
    std::string s_nick(nick_name);
    if (name_client.find(s_nick) != name_client.end()) 
    {
        sprintf(error, ":%s 436 %s :Nickname collision KILL\n", SERVER_NAME, nick_name);
        write(connfd, error, strlen(error));
        return true;
    }
    
    return false;
}

void nickname_in_use(int connfd, char *nick_name)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 433 %s :Nickname is already in use\n", SERVER_NAME, nick_name);
    write(connfd, error, strlen(error));
}

void not_on_channel(int connfd, std::string channel)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 442 %s %s :You're not on that channel\n", SERVER_NAME, fd_name[connfd].data(), channel.data());
    write(connfd, error, strlen(error));
}

void not_registered(int connfd, std::string nick_name) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 451 %s :You have not registered\n", SERVER_NAME, nick_name.data());
    write(connfd, error, strlen(error));
}

void not_enough_args(int connfd, char *command) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 461 %s :Not enough parameters\n", SERVER_NAME, command);
    write(connfd, error, strlen(error));
}

void reregister_error(int connfd)
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 462 %s :You may not reregister\n", SERVER_NAME, fd_name[connfd].data());
    write(connfd, error, strlen(error));
}