#ifndef ERROR_H
#define ERROR_H

void no_such_channel(int connfd, std::string channel); // 403
void no_host(int connfd); // 409

void no_recipient(int connfd, char *command); // 411
void no_text_send(int connfd); // 412

void error_cmd(int connfd, char *command); //421

bool check_nick_name(int connfd, char *nick_name); // 431, 432, 436
void nickname_in_use(int connfd, char *nick_name); // 433

void not_on_channel(int connfd, std::string channel); // 442
void not_registered(int connfd, std::string nick_name); // 451
void not_enough_args(int connfd, char *command); // 461
void reregister_error(int connfd); // 462
#endif