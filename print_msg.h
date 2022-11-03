#ifndef PRINT_MSG_H
#define PRINT_MSG_H

void broadcast();
void welcome_new_client(std::string nick_name);
void print_ping(int connfd);
void print_chat(char* buf, int client_index);
void print_nick(std::string old_nick, std::string new_nick);
void print_user(std::string nick_name, char** names);
void print_all_channels(int sockfd);
  
#endif