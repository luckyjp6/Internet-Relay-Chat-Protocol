#ifndef ERROR_H
#define ERROR_H

bool check_nick_name(int client_index, char *nick_name);
void not_enough_args(char *command);
void not_registerd(int connfd, std::string nick_name);
void not_on_channel(int connfd, std::string channel);
void no_recipient(int connfd, char *command);
void no_text_send(int connfd);
void no_such_channel(int connfd, std::string channel);
void error_cmd(int connfd, char *command);

#endif