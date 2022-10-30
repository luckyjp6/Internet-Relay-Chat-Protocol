#include "functions.h"
#include "print_msg.h"

extern int maxi, num_user;
extern Client_info client_info[OPEN_MAX];
extern pollfd client[OPEN_MAX];
extern std::vector<broadcast_msg> b_msg;

int main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
    int	i, nready;

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

    for (i = 1; i < OPEN_MAX; i++) client[i].fd = -1; /* -1: available entry */

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
                    client_info[i].addr = cliaddr;
                    break;
                }
            }

            if (i == OPEN_MAX) 
            {
                printf("too many clients\n");
                continue;
            }

            welcome_new_client(i, connfd, cliaddr);

            if (--nready <= 0) 
            {
                broadcast();
                continue;
            }
        }
        
        /* check all clients */
		int sockfd, n;
        char buf[MSG_SIZE-50];
        
        for (i = 1; i <= maxi; i++) 
        {
            if ( (sockfd = client[i].fd) < 0) continue;
            if (client[i].revents & (POLLRDNORM | POLLERR)) 
            {
                /* read input*/
                memset(buf, '\0', sizeof(buf));
                if ( (n = read(sockfd, buf, MSG_SIZE-50)) < 0) 
                { 
                    if (errno == ECONNRESET) close_client(i); /* connection reset by client */
                    else err_sys("read error");
                } 
                else if (n == 0) close_client(i); /* connection closed by client */
                else /* read command */
                {   
                    /* chat */
                    if (buf[0] != '/') 
                    {
                        print_chat(buf, i);

                        if (--nready <= 0) break;
                        else continue;
                    }
                    
                    const char *new_line = " \n";
                    char *command;
                    command = strtok(buf, new_line);

                    if (strcmp(command, "/name") == 0) 
                    {
                        char *nick_name;
                        

                        nick_name = strtok(NULL, new_line);
                        if (nick_name == NULL) {
                            error_cmd(buf, i);
                            if (--nready <= 0) break;
                            else continue;
                        }

                        print_name(buf, i, nick_name);
                    }
                    else if (strcmp(command, "/who") == 0) print_who(i);
                    else error_cmd(buf, i);
                }
                
                if (--nready <= 0) break; /* no more readable descs */
            }
        }
		
        broadcast();	
	}
    /* parent closes connected socket */
    close(connfd);
}