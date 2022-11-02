#include "functions.h"
#include "print_msg.h"
#include "name.h"

void broadcast() {
    for (int i = 0; i < b_msg.size(); i++) 
    { 
        for (int j = 1; j <= maxi; j++) 
        {
            
            if (j == b_msg[i].except) continue;
            
            write_time(j);
            if (client[j].fd < 0) continue;

            if (send(client[j].fd, b_msg[i].msg, b_msg[i].msg_size, MSG_NOSIGNAL) < 0) close_client(j);
        }
    }
    b_msg.clear();
}

void welcome_new_client(int client_index) 
{
    
    char welcome_msg[MSG_SIZE];
    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 001 %s :Welcom to worm's IRC server!\n", SERVER_NAME, client_info[client_index].nick_name);
    write(client[client_index].fd, welcome_msg, strlen(welcome_msg));

    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 251 %s There are %d users and 0 invisible on 1 servers\n", SERVER_NAME, client_info[client_index].nick_name, num_user);
    write(client[client_index].fd, welcome_msg, strlen(welcome_msg));

    // start of motd
    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 375 %s :- %s Message of the day -\n", SERVER_NAME, client_info[client_index].nick_name, SERVER_NAME);
    write(client[client_index].fd, welcome_msg, strlen(welcome_msg));

    // motd body
    char cool_msg[8][100] = {
        " __       __       __   ______    __ __    ____  ____",
        " \\ \\     /  \\     / /  /  __  \\   | '__/  / __ `' __  \\",
        "  \\ \\   / /\\ \\   / /  /  /  \\  \\  | |    |  | |  |  |  |",
        "   \\ \\_/ /  \\ \\_/ /   \\  \\__/  /  | |    |  | |  |  |  |",
        "    \\___/    \\___/     \\______/   |_|    |__| |__|  |__|",
        "{\\__/}" ,
        "( • - •)",
        "/ > $$"
    };
    for (int i = 0; i < 8; i++) {
        memset(welcome_msg, '\0', MSG_SIZE);
        sprintf(welcome_msg, ":%s 372 %s :- %s\n", SERVER_NAME, client_info[client_index].nick_name, cool_msg[i]);
        write(client[client_index].fd, welcome_msg, strlen(welcome_msg));
    }
    

    // end of motd
    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 376 %s :- End of message of the day -\n", SERVER_NAME, client_info[client_index].nick_name);
    write(client[client_index].fd, welcome_msg, strlen(welcome_msg));
}
  
void print_chat(char *buf, int client_index) 
{
    broadcast_msg tmp;
    memset(tmp.msg, '\0', MSG_SIZE);
    sprintf(tmp.msg, "*** User <%s> %s", client_info[client_index].nick_name, buf);
    tmp.set(client_index);
    b_msg.push_back(tmp);
}

void print_ping(int client_index) 
{
    char msg[MSG_SIZE];
    memset(msg, 0, MSG_SIZE);
    sprintf(msg, "PING %s\n", client_info[client_index].nick_name);
    write(client[client_index].fd, msg, sizeof(msg));
}

void print_nick(int client_index, char *nick_name) 
{
    char rename_msg[MSG_SIZE];
    memset(rename_msg, '\0', sizeof(rename_msg));
    sprintf(rename_msg, ":%s %s NICK %s", SERVER_NAME, client_info[client_index].nick_name, nick_name);
    write(client[client_index].fd, rename_msg, sizeof(rename_msg));
    
    // memset(client_info[client_index].nick_name, ' ', sizeof(client_info[client_index].nick_name));
    // strcpy(client_info[client_index].nick_name, nick_name);
}

void print_user(int client_index, char** names)
{
    // <username> <hostname> <servername> <realname>
    strcpy(client_info[client_index].user_name, names[0]);
    strcpy(client_info[client_index].real_name, names[3]+1);

    // hostname and servername are used for server vs. server
}

void print_who(int client_index) 
{
    char line[] = "--------------------------------------------------\n";
    write(client[client_index].fd, line, sizeof(line));
    for (int i = 1; i <= maxi; i++) {
        if (client[i].fd < 0) continue;
        char info[MSG_SIZE], tag[2];
        
        if (i == client_index) strcpy(tag, "*");
        else strcpy(tag, " ");

        memset(info, '\0', sizeof(info));
        sprintf(info, "%s %-20s %s:%d\n", tag, client_info[i].nick_name, inet_ntoa(client_info[i].addr.sin_addr), client_info[i].addr.sin_port);
        write(client[client_index].fd, info, sizeof(info));
    }
    write(client[client_index].fd, line, sizeof(line));
}