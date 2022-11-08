#ifndef PRINT_MSG_H
#define PRINT_MSG_H

void broadcast();
void welcome_new_client(std::string nick_name);
void print_ping(int connfd);
void print_chat(char* buf, int client_index);
void print_nick(std::string old_nick, std::string new_nick);
void print_join(std::vector<std::string> join_channel, int client_connfd);
void print_part(int connfd, std::string channel_name);
void print_topic(std::string topic, std::string channel_name, int connfd);
void print_all_users(int connfd);
void print_user_in_channel(std::vector<std::string> wanted_channels, int connfd);
void print_user(std::string nick_name, char** names);
void print_all_channels(int sockfd);
void print_msg_channel(char *text, std::string channel_name, int connfd);  
#endif