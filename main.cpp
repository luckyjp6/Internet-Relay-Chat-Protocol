#include "functions.h"
#include "print_msg.h"
#include "error_func.h"

int main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
    int	i, nready;

    init();

    clilen = sizeof(cliaddr);
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuse, sizeof(reuse));
    
    if (argc < 2) 
    {
        printf("Usage: ./a.out [port]\n");
        return -1;
    }

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(0);//INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if (bind(listenfd, (const sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
    {
		printf("failed to bind\n");
		return -1;
	}

	listen(listenfd, 1024);
    
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;

    Client_info tmp_client;
	for ( ; ; ) 
    {
        nready = poll(client, maxi+1, -1);
        
        // new client
        if (client[0].revents & POLLRDNORM) 
        {
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

            if (i == OPEN_MAX) 
            {
                printf("too many clients\n");
                continue;
            }
        
            client[i].events = POLLRDNORM;
            if (i > maxi) maxi = i;
        
            // printf("* client connected from %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
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

printf("\nreceived: %s", buf);                    
                    const char *new_line = " \n\r\0";
                    char *command = strtok(buf, new_line);

                    if (command == NULL) goto next;
                    
                    if (buf[0] == ':') 
                    {
                        command += 1;
                        // if (name_client.find(command) == name_client.end()) goto next;
                        command = strtok(NULL, new_line);
                    }
                    
                    // tolower_str(command);
// printf("command: %s, %ld, %d\n", command, strlen(command), (int)command[strlen(command)-1]);
                    if (strcmp(command, "NICK") == 0) 
                    {
                        char *new_nick;
                        new_nick = strtok(NULL, new_line);

                        
                        if (check_nick_name(i, new_nick)) goto next;
                        
                        std::string s(new_nick);
                        if (fd_name[sockfd].size() == 0) 
                        {
                            name_client[s] = tmp_client;
                            fd_name[sockfd] = s;
                        }
                        else 
                        {
                            name_client[s] = name_client[fd_name[sockfd]];
                            name_client.erase(fd_name[sockfd]);
                            fd_name[sockfd] = s;
                        }
                        // for (auto it = name_client.begin(); it != name_client.end(); it++) 
                        // {
                        //     if (it->second.connfd == sockfd) 
                        //     {
                        //         name_client[s] = it->second;
                        //         name_client.erase(it);
                        //         goto next;
                        //     }
                        // }
                    }
                    else if (strcmp(command, "USER") == 0) 
                    {

                        char *args[4]; // <username> <hostname> <servername> <realname>
                        for (int j = 0; j < 4; j++) 
                        {
                            args[j] = strtok(NULL, new_line);
                            if (args[j] == NULL) {
                                not_enough_args(command);
                                goto next;
                            }
                        }
                        
                        // register new user and show welcome message
                        for (auto it = name_client.begin(); it != name_client.end(); it++)
                        {
                            if (it->second.connfd == sockfd)
                            {
                                print_user(it->first, args);
                                welcome_new_client(it->first);
                                break;
                            }
                        }
                        
                    }
                    else if (strcmp(command, "USERS") == 0)
                    {
                        print_all_users(sockfd);
                    }
                    else if (strcmp(command, "LIST") == 0) 
                    { 
                        /*read all wanted channel and list them all*/
                        char *channel = strtok(NULL, new_line);
                        if (channel == NULL)
                            print_all_channels(sockfd);
                        else
                        {
                            printf("channel: %d\n", (int)channel[0]);
                            // print_specified_channels();
                            
                        }
                    }
                    else if (strcmp(command, "JOIN") == 0)
                    {
                        char *channel = strtok(NULL, new_line);
                        if (channel == NULL)
                        {
                            not_enough_args(command);
                            goto next;
                        }

                        std::vector<std::string> join_channel;
                        do 
                        {
                            join_channel.push_back(channel);
                            channel = strtok(NULL, new_line);
                        } while (channel != NULL);

                        print_join(join_channel, sockfd);
                    }
                    else if (strcmp(command, "PART") == 0)
                    {
                        char *channel = strtok(NULL, new_line);
                        if (channel == NULL) 
                        {
                            not_enough_args(command);
                            goto next;
                        }
                        
                        print_part(sockfd, channel);
                    }
                    else if (strcmp(command, "TOPIC") == 0)
                    {
                        char *channel_char = strtok(NULL, new_line);
                        if (channel_char == NULL)
                        {
                            not_enough_args(command);
                            goto next;
                        }

                        std::string channel(channel_char);

                        char *topic_char = strtok(NULL, new_line);
                        if (topic_char == NULL) 
                            print_topic("", channel, sockfd);
                        else 
                        {
                            std::string topic(topic_char);
                            print_topic(topic, channel, sockfd);
                        }
                    }
                    // else if (strcmp(command, "PRIVMSG") == 0) 
                    // {
                    //     char *to = strtok(NULL, new_line);
                    //     char *msg = strtok(NULL, new_line);
                    //     if (to[0] == '#') print_msg_channel(msg+1, to);
                    //     else print_msg(msg+1, to);
                    // }
                    else if (strcmp(command, "PING") == 0) print_ping(sockfd);
                    // else if (strcmp(command, "close") == 0) close_client(i);
                    else if (strcmp(command, "QUIT") == 0) close_client(i);
                    else error_cmd(command, connfd);
                }
next:                
                if (--nready <= 0) break; /* no more readable descs */
            }
        }
		
        broadcast();	
	}
    /* parent closes connected socket */
    close(connfd);
}