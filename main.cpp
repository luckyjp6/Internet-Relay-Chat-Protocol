#include "functions.h"
#include "print_msg.h"
#include "error_func.h"

int main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	sockaddr_in	        cliaddr, servaddr;
	void				sig_chld(int);
    int	                i, nready;

    init();

    clilen = sizeof(cliaddr);
    
    my_connect(listenfd, argv[1], servaddr);

    Client_info tmp_client;
	for ( ; ; ) 
    {
        nready = poll(client, maxi+1, -1);
        
        // new client
        if (client[0].revents & POLLRDNORM) 
        {
            nready--;
            connfd = accept(listenfd, (sockaddr *) &cliaddr, &clilen);
            num_user ++;

            // save descriptor
            for (i = 1; i < OPEN_MAX; i++)
            {
                if (client[i].fd < 0) 
                {
                    client[i].fd = connfd;
                    tmp_client.addr = cliaddr;
                    tmp_client.connfd = connfd;
                    break;
                }
            }

            if (i == OPEN_MAX) printf("too many clients\n");
            else
            {
                client[i].events = POLLRDNORM;
                if (i > maxi) maxi = i;
            }  

            if (nready == 0) break;          
        }
        
        /* check all clients */
		int sockfd, n;
        char buf[MSG_SIZE];
        
        for (i = 1; i <= maxi; i++) 
        {
            if ( (sockfd = client[i].fd) < 0) continue;
            if (client[i].revents & (POLLRDNORM | POLLERR)) 
            {
                /* read input*/
                memset(buf, '\0', MSG_SIZE);

                if ( (n = read(sockfd, buf, MSG_SIZE-50)) < 0) 
                { 
                    if (errno == ECONNRESET) close_client(i); /* connection reset by client */
                    // else err_sys("read error");
                } 
                else if (n == 0) close_client(i); /* connection closed by client */
                else /* read command */
                {                 
                    const char *new_line = " \n\r\0";
                    char *command = strtok(buf, new_line);

                    if (command == NULL) goto next;
                    
                    if (buf[0] == ':') 
                    {
                        command += 1;
                        if (name_client.find(command) == name_client.end()) {
                            not_registered(sockfd, command);
                            goto next;
                        }
                        command = strtok(NULL, new_line);
                    }

                    /* set nickname */
                    if (strcmp(command, "NICK") == 0) 
                    {
                        char *new_nick;
                        new_nick = strtok(NULL, new_line);
                        
                        std::string s(new_nick);

                        // new client set nickname
                        if (fd_name[sockfd].size() == 0) 
                        {   
                            if (check_nick_name(sockfd, new_nick)) goto next;
                            name_client[s] = tmp_client;
                            fd_name[sockfd] = s;
                        }
                        // current client rename
                        else 
                        {   
                            if (name_client.find(new_nick) != name_client.end()) 
                            {
                                nickname_in_use(sockfd, new_nick);
                                goto next;
                            }
                            name_client[s] = name_client[fd_name[sockfd]];
                            name_client.erase(fd_name[sockfd]);
                            fd_name[sockfd] = s;
                        }
                    }
                    /* register new client */
                    else if (strcmp(command, "USER") == 0) 
                    {
                        char *args[4]; // <username> <hostname> <servername> <realname>
                        for (int j = 0; j < 4; j++) 
                        {
                            args[j] = strtok(NULL, new_line);
                            if (args[j] == NULL) {
                                not_enough_args(sockfd, command);
                                goto next;
                            }
                        }
                        
                        // register new user and show welcome message
                        if (fd_name[sockfd].size() == 0) reregister_error(sockfd);
                        else set_user(fd_name[sockfd], args);

                        welcome_new_client(fd_name[sockfd]);
                    }
                    else if (strcmp(command, "PING") == 0) {
                        char *host = strtok(NULL, new_line);
                        if (host == NULL) 
                        {
                            no_host(sockfd);
                        }
                        print_ping(sockfd);
                    }
                    /* list all wanted channels and their information */
                    else if (strcmp(command, "LIST") == 0) 
                    {
                        char *channel = strtok(NULL, new_line);
                        // list all channels
                        if (channel == NULL)
                        {
                            print_channel_info(sockfd);
                        }
                        // only list specified channels
                        else
                        {
                            std::vector<std::string> wanted_channels;
                            do
                            {
                                wanted_channels.push_back(channel);
                                channel = strtok(NULL, new_line);
                            }while (channel != NULL);
                            print_channel_info(sockfd, wanted_channels);
                            
                        }
                    }
                    /* join a channel */
                    else if (strcmp(command, "JOIN") == 0)
                    {
                        const char *new_line = " ,\n\r\0";
                        char *channel = strtok(NULL, new_line);
                        if (channel == NULL)
                        {
                            not_enough_args(sockfd, command);
                            goto next;
                        }
                        
                        if (channel[0] != '#') no_such_channel(sockfd, channel);
                        std::vector<std::string> join_channel;
                        do 
                        {
                            join_channel.push_back(channel);
                            
                            channel = strtok(NULL, ",\0");
                        } while (channel != NULL);

                        print_join(sockfd, join_channel);
                    }
                    else if (strcmp(command, "TOPIC") == 0)
                    {
                        char *channel_char = strtok(NULL, new_line);
                        if (channel_char == NULL)
                        {
                            not_enough_args(sockfd, command);
                            goto next;
                        }

                        std::string channel(channel_char);
                        if (channels.find(channel) == channels.end()) 
                        {
                            no_such_channel(sockfd, channel);
                            goto next;
                        }

                        char *topic_char = strtok(NULL, "\0");
                        if (topic_char == NULL) 
                            print_topic(sockfd, "", channel);
                        else 
                        {
                            topic_char += 1;
                            std::string topic(topic_char);
                            print_topic(sockfd, topic, channel);
                        }
                    }
                    /* list all clients' nickname in some channels */
                    else if (strcmp(command, "NAMES") == 0)
                    {
                        char *channel_char = strtok(NULL, new_line);

                        // list every channels' client
                        if (channel_char == NULL)
                        {
                            print_channel_users(sockfd);
                        }
                        // only list specified channels' clients
                        else
                        {
                            std::vector<std::string> wanted_channels;
                            do
                            {
                                wanted_channels.push_back(std::string (channel_char));
                                channel_char = strtok(NULL, new_line);
                            }while (channel_char != NULL);
                            print_channel_users(sockfd, wanted_channels);
                        } 
                    }
                    else if (strcmp(command, "PART") == 0)
                    {
                        char *channel = strtok(NULL, new_line);
                        if (channel == NULL) 
                        {
                            not_enough_args(sockfd, command);
                            goto next;
                        }
                        
                        print_part(sockfd, channel);
                    }
                    /* print all users' and their information */
                    else if (strcmp(command, "USERS") == 0)
                    {
                        print_all_users(sockfd);
                    }
                    else if (strcmp(command, "PRIVMSG") == 0) 
                    {
                        char *to_char = strtok(NULL, new_line);
                        if (to_char == NULL) {
                            no_recipient(sockfd, command);
                            goto next;
                        }
                        std::string to(to_char);

                        char *msg = strtok(NULL, new_line);
                        if (msg == NULL) 
                        {
                            no_text_send(sockfd);
                            goto next;
                        }

                        msg += 1;
                        if (to[0] == '#') print_msg_channel(sockfd, msg, to);
                        // else print_private_msg(msg, to);
                    }
                    else if (strcmp(command, "QUIT") == 0) close_client(i);
                    else error_cmd(connfd, command);
                }
next:                
                if (--nready <= 0) break; /* no more readable descs */
            }
        }	
	}
    /* parent closes connected socket */
    close(connfd);
}