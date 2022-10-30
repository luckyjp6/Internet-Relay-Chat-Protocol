#ifndef PRINT_MSG_H
#define PRINT_MSG_H

#include "functions.h"

void broadcast();
void welcome_new_client(int client_index, int connfd, sockaddr_in &cliaddr);
void print_chat(char *buf, int client_index);
void print_name(char *buf, int client_index, char* nick_name);
void print_who(int client_index);

#endif