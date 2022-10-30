#include "print_msg.h"
#include "name.h"

void broadcast() {
    for (int i = 0; i < b_msg.size(); i++) { 
        for (int j = 1; j <= maxi; j++) {
            
            if (j == b_msg[i].except) continue;
            
            write_time(j);
            if (client[j].fd < 0) continue;

            if (send(client[j].fd, b_msg[i].msg, b_msg[i].msg_size, MSG_NOSIGNAL) < 0) close_client(j);
        }
    }
    b_msg.clear();
}

void welcome_new_client(int client_index, int connfd, sockaddr_in &cliaddr) {
    
    char welcome_msg[] = "*** Welcome to the simple CHAT server\n";
    write_time(client_index);
    write(connfd, welcome_msg, sizeof(welcome_msg));
    
    write_time(client_index);
    char nick_name[25];
    sprintf(nick_name, "%s %s", adjs[rand()%num_animals], animals[rand()%num_adjs]);
    memset(client_info[client_index].name, ' ', sizeof(nick_name));
    strcpy(client_info[client_index].name, nick_name);

    char msg[MSG_SIZE];
    memset(msg, '\0', sizeof(msg));
    sprintf(msg, "Total %d users online now. Your name is <%s>\n", num_user, nick_name);
    write(connfd, msg, sizeof(msg));

    broadcast_msg announce;
    memset(announce.msg, '\0', MSG_SIZE);
    sprintf(announce.msg, "*** User <%s> has just landed on the server\n", nick_name);
    announce.set(client_index);
    b_msg.push_back(announce);
    
    client[client_index].events = POLLRDNORM;
    if (client_index > maxi) maxi = client_index;

    printf("* client connected from %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

}
  
void print_chat(char *buf, int client_index) {
    broadcast_msg tmp;
    memset(tmp.msg, '\0', MSG_SIZE);
    sprintf(tmp.msg, "*** User <%s> %s", client_info[client_index].name, buf);
    tmp.set(client_index);
    b_msg.push_back(tmp);
}

void print_name(char *buf, int client_index, char *nick_name) {
    write_time(client_index);
    char rename_msg[MSG_SIZE];
    memset(rename_msg, '\0', sizeof(rename_msg));
    sprintf(rename_msg, "*** Nickname changed to <%s>\n", nick_name);
    write(client[client_index].fd, rename_msg, sizeof(rename_msg));
    
    broadcast_msg tmp;
    memset(tmp.msg, '\0', MSG_SIZE);
    sprintf(tmp.msg, "*** User <%s> renamed to <%s>\n", client_info[client_index].name, nick_name);
    tmp.set(client_index);
    b_msg.push_back(tmp);
    
    memset(client_info[client_index].name, ' ', sizeof(client_info[client_index].name));
    strcpy(client_info[client_index].name, nick_name);
}

void print_who(int client_index) {
    char line[] = "--------------------------------------------------\n";
    write(client[client_index].fd, line, sizeof(line));
    for (int i = 1; i <= maxi; i++) {
        if (client[i].fd < 0) continue;
        char info[MSG_SIZE], tag[2];
        
        if (i == client_index) strcpy(tag, "*");
        else strcpy(tag, " ");

        memset(info, '\0', sizeof(info));
        sprintf(info, "%s %-20s %s:%d\n", tag, client_info[i].name, inet_ntoa(client_info[i].addr.sin_addr), client_info[i].addr.sin_port);
        write(client[client_index].fd, info, sizeof(info));
    }
    write(client[client_index].fd, line, sizeof(line));
}