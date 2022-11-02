#ifndef PRINT_MSG_H
#define PRINT_MSG_H

void broadcast();
void welcome_new_client(int client_index);
void print_ping(int client_index);
void print_chat(char* buf, int client_index);
void print_nick(int client_index, char* nick_name);
void print_user(int client_index, char** names);
void print_who(int client_index);
  
#endif