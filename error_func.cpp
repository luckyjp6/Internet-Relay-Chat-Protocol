#include "functions.h"

bool check_nick_name(int client_index, char *nick_name) 
{
    // no nickname
    if (nick_name == NULL) {
        char error[MSG_SIZE];
        memset(error, '\0', MSG_SIZE);
        sprintf(error, ":%s 431 : No nickname given\n", SERVER_NAME);
        write(client[client_index].fd, error, sizeof(error));
        return true;
    }
    
    // check conflict nickname
    if (name_client.find(nick_name) != name_client.end()) 
    {
        char error[MSG_SIZE];
        memset(error, '\0', MSG_SIZE);
        sprintf(error, ":%s 436 %s :Nickname collision KILL\n", SERVER_NAME, nick_name);
        write(client[client_index].fd, error, sizeof(error));
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