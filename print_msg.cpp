#include "functions.h"
#include "print_msg.h"
#include "error_func.h"

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

void set_user(std::string nick_name, char** names)
{
    // <username> <hostname> <servername> <realname>
    strcpy(name_client[nick_name].user_name, names[0]);
    strcpy(name_client[nick_name].real_name, names[3]+1);

    // hostname and servername are used for server vs. server
}

void print_join(int connfd, std::vector<std::string> join_channel)
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
        
        std::string nick_name = fd_name[connfd];
        char msg[MSG_SIZE];
        
        // join the channel
        channels[channel].connected.push_back(connfd);
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s JOIN %s\n", nick_name.data(), channel.data());
        write(connfd, msg, strlen(msg));
        
        // show the topic
        memset(msg, '\0', MSG_SIZE);
        if (channels[channel].topic.size() == 0) 
            sprintf(msg, ":%s 331 %s %s :No topic is set\n", SERVER_NAME, nick_name.data(), channel.data());
        else
            sprintf(msg, ":%s 332 %s %s :%s\n", SERVER_NAME, nick_name.data(), channel.data(), channels[channel].topic.data());
        write(connfd, msg, strlen(msg));

        // show all clients in this channel
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 353 %s %s :", SERVER_NAME, nick_name.data(), channel.data());
        std::string s_msg(msg);
        for (auto user : channels[channel].connected)
        {
            s_msg += fd_name[user] + " ";
        }
        s_msg += "\n";
        write(connfd, s_msg.data(), s_msg.size());
        // print_all_users(connfd);
        

        // end of list
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 366 %s %s :End of Names List\n", SERVER_NAME, nick_name.data(), channel.data());
        write(connfd, msg, strlen(msg));
    }
}

void print_part(int connfd, std::string channel_name) 
{
    if (channels.find(channel_name) == channels.end())
    {
        no_such_channel(connfd, channel_name);
        return;
    }

    auto in = find (channels[channel_name].connected.begin(), channels[channel_name].connected.end(), connfd);
    if (in == channels[channel_name].connected.end()) 
    {
        not_on_channel(connfd, channel_name);
        return;
    }

    channels[channel_name].connected.erase(in);

    if (channels[channel_name].connected.size() == 0) channels.erase(channel_name);
    
    char msg[MSG_SIZE];
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s PART :%s\n", fd_name[connfd].data(), channel_name.data());
    write(connfd, msg, strlen(msg));
}

void print_topic(int connfd, std::string topic, std::string channel_name) 
{
    char msg[MSG_SIZE];
    memset(msg, '\0', MSG_SIZE);
    
    auto in = find (channels[channel_name].connected.begin(), channels[channel_name].connected.end(), connfd);
    if (in == end(channels[channel_name].connected)) 
    {
        not_on_channel(connfd, channel_name);
        return; 
    }    
    
    if (topic.size() > 0) channels[channel_name].topic = topic;

    if (channels[channel_name].topic.size() > 0)
        sprintf(msg, ":%s 332 %s %s :%s\n", SERVER_NAME, fd_name[connfd].data(), channel_name.data(), channels[channel_name].topic.data());
    else 
        sprintf(msg, ":%s 331 %s %s :No topic is set\n", SERVER_NAME, fd_name[connfd].data(), channel_name.data());

    write(connfd, msg, strlen(msg));
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

void print_channel_users(int connfd)
{
    for (auto channel : channels)
    {
        char msg[MSG_SIZE];
        for (auto user : channel.second.connected)
        {
            memset(msg, '\0', MSG_SIZE);
            sprintf(msg, ":%s 353 %s %s :%s\n", SERVER_NAME, fd_name[connfd].data(), channel.first.data(), fd_name[user].data());
            write(connfd, msg, strlen(msg));
        }        

        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 366 %s %s :End of Names List\n", SERVER_NAME, fd_name[connfd].data(), channel.first.data());
        write(connfd, msg, strlen(msg));
    }
}

void print_channel_users(int connfd, std::vector<std::string> wanted_channels)
{
    for (auto channel : wanted_channels)
    {
        char msg[MSG_SIZE];
        if (channels.find(channel) != channels.end())
        {
            for (auto user : channels[channel].connected)
            {
                memset(msg, '\0', MSG_SIZE);
                sprintf(msg, ":%s 353 %s %s :%s\n", SERVER_NAME, fd_name[connfd].data(), channel.data(), fd_name[user].data());
                write(connfd, msg, strlen(msg));
            }
        }

        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 366 %s %s :End of Names List\n", SERVER_NAME, fd_name[connfd].data(), channel.data());
        write(connfd, msg, strlen(msg));
    }
} 

void print_channel_info(int connfd)
{
    char msg[MSG_SIZE];
    std::string nick_name = fd_name[connfd];

    // list start
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 321 %s Channel :Users Name\n", SERVER_NAME, nick_name.data());
    write(connfd, msg, strlen(msg));

    // list body
    for (auto it : channels) 
    {
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 322 %s %s %ld :%s\n", SERVER_NAME, nick_name.data(), it.first.data(), it.second.connected.size(), it.second.topic.data());
        write(connfd, msg, strlen(msg));
    }

    //list end
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 323 %s :End of Liset\n", SERVER_NAME, nick_name.data());
    write(connfd, msg, strlen(msg));
}

void print_channel_info(int connfd, std::vector<std::string> wanted_channels)
{
    char msg[MSG_SIZE];
    std::string nick_name = fd_name[connfd];

    // list start
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 321 %s Channel :Users Name\n", SERVER_NAME, nick_name.data());
    write(connfd, msg, strlen(msg));

    // list body
    for (auto it : wanted_channels) 
    {
        if (channels.find(it) == channels.end()) {
            no_such_channel(connfd, it);
            continue;
        }
        memset(msg, '\0', MSG_SIZE);
        sprintf(msg, ":%s 322 %s %s %ld :%s\n", SERVER_NAME, nick_name.data(), it.data(), channels[it].connected.size(), channels[it].topic.data());
        write(connfd, msg, strlen(msg));
    }

    //list end
    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s 323 %s :End of Liset\n", SERVER_NAME, nick_name.data());
    write(connfd, msg, strlen(msg));
}

void print_msg_channel(int connfd, char *text, std::string channel_name)
{
    char msg[MSG_SIZE];

    memset(msg, '\0', MSG_SIZE);
    sprintf(msg, ":%s PRIVMSG %s :%s\n", fd_name[connfd].data(), channel_name.data(), text);
    
    for (auto user: channels[channel_name].connected)
    {
        if (user == connfd) continue;
        write(user, msg, strlen(msg));    
    }
}

// void print_private_msg(char *msg, std::string to)
// {
//     char msg[MSG_SIZE];

//     memset(msg, '\0', MSG_SIZE);
//     sprintf(msg, ":%s 323 %s :End of Liset\n", SERVER_NAME, nick_name.data());
//     write(connfd, msg, strlen(msg));
// }