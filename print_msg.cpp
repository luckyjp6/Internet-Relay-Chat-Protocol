#include "functions.h"
#include "print_msg.h"

void broadcast() {
    // for (int i = 0; i < b_msg.size(); i++) 
    // { 
    //     for (int j = 1; j <= maxi; j++) 
    //     {
            
    //         if (j == b_msg[i].except) continue;
            
    //         write_time(j);
    //         if (client[j].fd < 0) continue;

    //         if (send(client[j].fd, b_msg[i].msg, b_msg[i].msg_size, MSG_NOSIGNAL) < 0) close_client(j);
    //     }
    // }
    // b_msg.clear();
}

void welcome_new_client(std::string nick_name) 
{
    int connfd = name_client[nick_name].connfd;

    char welcome_msg[MSG_SIZE];
    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 001 %s :Welcom to worm's IRC server!\n", SERVER_NAME, nick_name.data());
    write(connfd, welcome_msg, strlen(welcome_msg));

    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 251 %s There are %d users and 0 invisible on 1 servers\n", SERVER_NAME, nick_name.data(), num_user);
    write(connfd, welcome_msg, strlen(welcome_msg));

    // start of motd
    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 375 %s :- %s Message of the day -\n", SERVER_NAME, nick_name.data(), SERVER_NAME);
    write(connfd, welcome_msg, strlen(welcome_msg));

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
        sprintf(welcome_msg, ":%s 372 %s :- %s\n", SERVER_NAME, nick_name.data(), cool_msg[i]);
        write(connfd, welcome_msg, strlen(welcome_msg));
    }
    

    // end of motd
    memset(welcome_msg, '\0', MSG_SIZE);
    sprintf(welcome_msg, ":%s 376 %s :- End of message of the day -\n", SERVER_NAME, nick_name.data());
    write(connfd, welcome_msg, strlen(welcome_msg));
}
  
void print_chat(char *buf, int client_index) 
{
    // broadcast_msg tmp;
    // memset(tmp.msg, '\0', MSG_SIZE);
    // sprintf(tmp.msg, "*** User <%s> %s", client_info[client_index].nick_name, buf);
    // tmp.set(client_index);
    // b_msg.push_back(tmp);
}

void print_ping(int connfd) 
{
    char msg[MSG_SIZE];
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, "PONG %s\n", SERVER_NAME);
    write(connfd, msg, sizeof(msg));
}

void print_nick(std::string old_nick, std::string new_nick) 
{
    // int connfd = name_client[old_nick].connfd;
    // char rename_msg[MSG_SIZE];
    // memset(rename_msg, '\0', MSG_SIZE);
    // sprintf(rename_msg, ":%s %s NICK %s", SERVER_NAME, old_nick.data(), new_nick.data());
    // write(connfd, rename_msg, sizeof(rename_msg));

    name_client[new_nick] = name_client[old_nick];
    name_client.erase(old_nick);
}

void print_user(std::string nick_name, char** names)
{
    // <username> <hostname> <servername> <realname>
    strcpy(name_client[nick_name].user_name, names[0]);
    strcpy(name_client[nick_name].real_name, names[3]+1);

    // hostname and servername are used for server vs. server
}

void print_join(std::string channel, std::string nick_name)
{
    if (channels.find(channel) == channels.end())
    {
        channel_info tmp;
        tmp.topic = NULL;
        channels[channel] = tmp;
    }
    channels[channel].connected.push_back(nick_name);

    char msg[MSG_SIZE];
    memset(msg, '\0', MSG_SIZE);
    if (channels[channel].topic == NULL) 
        sprintf(msg, ":%s 331 %s :No topic is set\n", SERVER_NAME, channel.data());
    else
        sprintf(msg, ":%s 332 %s :%s\n", SERVER_NAME, channel.data(), channels[channel].topic);
    write(name_client[nick_name].connfd, msg, sizeof(msg));
}

void print_all_channels(int connfd)
{
    char msg[MSG_SIZE];

    // list start
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 321 Channel :Users Name\n", SERVER_NAME);
    write(connfd, msg, sizeof(msg));

    // list body
    for (auto it = channels.begin(); it != channels.end(); it++) 
    {
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 322 %s %ld :%s", SERVER_NAME, it->first.data(), it->second.connected.size(), it->second.topic);
        write(connfd, msg, sizeof(msg));
    }

    //list end
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 323 :End of /LIST\n", SERVER_NAME);
    write(connfd, msg, sizeof(msg));
}