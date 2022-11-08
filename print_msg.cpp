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
    sprintf(msg, ":%s PONG :%s\n", SERVER_NAME, SERVER_NAME);
    write(connfd, msg, strlen(msg));
}

void print_nick(std::string old_nick, std::string new_nick) 
{
    // int connfd = name_client[old_nick].connfd;
    // char rename_msg[MSG_SIZE];
    // memset(rename_msg, '\0', MSG_SIZE);
    // sprintf(rename_msg, ":%s %s NICK %s", SERVER_NAME, old_nick.data(), new_nick.data());
    // write(connfd, rename_msg, strlen(rename_msg));

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

void print_all_users(int connfd)
{
    char msg[MSG_SIZE];
    // start
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 392 %s :%-8s %-9s %-8s\n", SERVER_NAME, fd_name[connfd].data(), "UserID", "Terminal", "Host");
    write(connfd, msg, strlen(msg));

    // body
    for (auto it : name_client)
    {
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 393 %s :%-8s %-9s %-8s\n", SERVER_NAME, fd_name[connfd].data(), it.first.data(), "-", inet_ntoa(it.second.addr.sin_addr));
        write(connfd, msg, strlen(msg));
    }

    // end
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 394 %s :End of users\n", SERVER_NAME, fd_name[connfd].data());
    write(connfd, msg, strlen(msg));
}

void print_join(std::vector<std::string> join_channel, int client_connfd)
{
    for (int i = 0; i < join_channel.size(); i++) 
    {    
        std::string channel = join_channel[i];
        // create a new channel
        if (channels.find(channel) == channels.end())
        {
            channel_info tmp;
            tmp.topic = "";
            channels[channel] = tmp;
        }
        
        std::string nick_name = fd_name[client_connfd];
        char msg[MSG_SIZE];
        
        // join the channel
        channels[channel].connected.push_back(client_connfd);
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s JOIN %s\n", nick_name.data(), channel.data());
        write(client_connfd, msg, strlen(msg));
        
        // show the topic
        memset(msg, '\0', MSG_SIZE);
        if (channels[channel].topic.size() == 0) 
            sprintf(msg, ":%s 331 %s %s :No topic is set\n", SERVER_NAME, nick_name.data(), channel.data());
        else
            sprintf(msg, ":%s 332 %s %s :%s\n", SERVER_NAME, nick_name.data(), channel.data(), channels[channel].topic.data());
        write(client_connfd, msg, strlen(msg));

        // announce new clients enter this channel
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 353 %s %s :%s\n", SERVER_NAME, nick_name.data(), channel.data(), nick_name.data());
        write(client_connfd, msg, strlen(msg));
        
        // end of list
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 366 %s %s :End of Names List\n", SERVER_NAME, nick_name.data(), channel.data());
        write(client_connfd, msg, strlen(msg));
    }
}

void print_part(int connfd, std::string channel_name) 
{
    auto in = find (channels[channel_name].connected.begin(), channels[channel_name].connected.end(), connfd);
    if (in == end(channels[channel_name].connected)) return;

    channels[channel_name].connected.erase(in);

    if (channels[channel_name].connected.size() == 0) channels.erase(channel_name);
    
    char msg[MSG_SIZE];
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s PART :%s\n", fd_name[connfd].data(), channel_name.data());
    write(connfd, msg, strlen(msg));
}

void print_topic(std::string topic, std::string channel_name, int connfd) 
{
    char msg[MSG_SIZE];
    memset(msg, '\0', MSG_SIZE);
    
    auto in = find (channels[channel_name].connected.begin(), channels[channel_name].connected.end(), connfd);
    if (in == end(channels[channel_name].connected)) 
    {
        sprintf(msg, ":%s 442 %s %s :you are not on that channel\n", SERVER_NAME, fd_name[connfd].data(), channel_name.data());
        write (connfd, msg, strlen(msg));
        return; 
    }    
    
    if (topic.size() > 0) channels[channel_name].topic = topic;

    if (channels[channel_name].topic.size() > 0)
        sprintf(msg, ":%s 332 %s %s :%s\n", SERVER_NAME, fd_name[connfd].data(), channel_name.data(), channels[channel_name].topic.data());
    else 
        sprintf(msg, ":%s 331 %s %s :No topic is set\n", SERVER_NAME, fd_name[connfd].data(), channel_name.data());

    write(connfd, msg, strlen(msg));
}

void print_all_channels(int connfd)
{
    char msg[MSG_SIZE];
    std::string nick_name = fd_name[connfd];

    // list start
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 321 %s Channel :Users Name\n", SERVER_NAME, nick_name.data());
    write(connfd, msg, strlen(msg));

    // list body
    for (auto it = channels.begin(); it != channels.end(); it++) 
    {
        memset(msg, '\0', MSG_SIZE);
        // sprintf(msg, ":%s 322 %s %s %ld :%s\n", SERVER_NAME, nick_name.data(), "#channel1", (long int) 0, "topic");
        sprintf(msg, ":%s 322 %s %s %ld :%s\n", SERVER_NAME, nick_name.data(), it->first.data(), it->second.connected.size(), it->second.topic.data());
        write(connfd, msg, strlen(msg));
    }

    //list end
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 323 %s :End of Liset\n", SERVER_NAME, nick_name.data());
    write(connfd, msg, strlen(msg));
}