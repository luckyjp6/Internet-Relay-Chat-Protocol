#ifndef FUNSTIONS_H
#define FUNSTIONS_H

#include <cerrno>
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <time.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define OPEN_MAX 1024
#define MSG_SIZE 300

struct Client_info {
    char name[25];
    sockaddr_in addr;
}; 

struct broadcast_msg{
    int except;
    char msg[MSG_SIZE];
    int msg_size;
    void set(int e) { //, char m[MSG_SIZE], int s) {
        except = e;
        msg_size = sizeof(msg);
        // memset(msg, '\0', sizeof(msg));
        // strcpy(msg, m);
    }
};

extern int maxi, num_user;
extern Client_info client_info[OPEN_MAX];
extern pollfd client[OPEN_MAX];
extern std::vector<broadcast_msg> b_msg;

void time_format(char* tt, int tt_size);
void write_time(int index);

void close_client(int index);

void error_cmd(char *cmd, int to);
void err_sys(const char *err);

#endif