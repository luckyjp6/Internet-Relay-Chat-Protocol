#include "functions.h"

bool check_nick_name(int client_index, char *nickname) 
{
    // no nickname
    if (nickname == NULL) {
        char error[MSG_SIZE];
        memset(error, '\0', MSG_SIZE);
        sprintf(error, ":%s 431 : No nickname given\n", SERVER_NAME);
        write(client[client_index].fd, error, sizeof(error));
        return true;
    }
    
    // check conflict nickname
    for (int i = 0; i < maxi; i++) {
        if (strlen(client_info[i].nick_name) == 0) continue;
        if (strcmp(client_info[i].nick_name, nickname) == 0) {
            char error[MSG_SIZE];
            memset(error, '\0', MSG_SIZE);
            sprintf(error, ":%s 436 %s :Nickname collision KILL\n", SERVER_NAME, client_info[i].nick_name);
            write(client[client_index].fd, error, sizeof(error));
            return true;
        }
    }

    return false;
}

void not_enough_args(int client_index) 
{
    char error[MSG_SIZE];
    memset(error, '\0', MSG_SIZE);
    sprintf(error, ":%s 461 %s :Not enough parameters\n", SERVER_NAME, client_info[client_index].nick_name);
}