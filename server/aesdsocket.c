#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include<sys/types.h>
#include<sys/stat.h>

#define PORT "9000"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

#define MAXDATASIZE 100000 // max number of bytes we can get at once 
#define LOG_FILE "/var/tmp/aesdsocketdata"
int sockfd;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        syslog(LOG_USER, "Caught signal, exiting");
        fprintf(stdout , "Caught signal, exiting\n");
        
        close(sockfd);
   	remove(LOG_FILE);  
   	closelog();
   	exit(0);
    }
}

void process_clients(int new_fd) {
	
	FILE *fd = NULL;
	if(!(fd = fopen(LOG_FILE,"a+"))) {
		fprintf(stderr, "Unable to open file /tmp/aesdsocketdata \n"); 
		exit(7);
 	}
 	
    	int read_size;
    	int buf_size; ;
    	char *tmp_buf = NULL;
    	char *buf = NULL;
	buf = malloc(sizeof(char) * 1); 
	buf_size = 1;
	buf[0] = '\0'; // start off with a null terminated buffer
	tmp_buf = malloc(MAXDATASIZE);
	
  	while (1) {
		///send(new_fd, "1234", 5, 0);
        	memset(tmp_buf, '\0', sizeof(char) * MAXDATASIZE );  // clear the tmp buffer with null termination
        	if ((read_size = recv(new_fd, tmp_buf, MAXDATASIZE-1, 0)) == -1) {
        		free(buf);
        		free(tmp_buf);
               	fprintf(stderr, "recv return an error\n"); 
                       syslog(LOG_USER, "recv return an error");        
                       break; // exit read loop
               }
               else if ( read_size == 0) {
               	free(buf);
               	free(tmp_buf);
               	printf("Client closed the connection\n");
               	break;
               } 
               else {  // read_size > 0   
               	printf("%s",tmp_buf);                            
               	buf_size += read_size - 1;
               	buf = realloc(buf, buf_size);
       		if ( buf == NULL) {
       			fprintf(stderr, "realloc error\n"); 
       			exit(11); // exit for now 
       		}
       		strncat(buf+ (buf_size-read_size), tmp_buf, read_size); 
       		            		
       		// to keep the code simple we always assume a certaisn pkt structure 
       		// example data<> data <> data <> data<endpkt>           	
   			if (tmp_buf[read_size-1] == '\n') {
   				//copy buffer to file
   				fwrite(buf , 1, buf_size, fd); 
   				fflush(fd);
   				
   				//release buf pointer back to 
				free(buf);
				buf = malloc(sizeof(char) * 1); 
				buf_size = 1;
				buf[0] = '\0'; // start off with a null terminated buffer	
  				    	
  				char * line = NULL;
				size_t len = 0;
				int read;
				
				//send back the entire file
   				rewind(fd);
   				while ((read = getline(&line, &len, fd)) != -1) {
			        	if ( send(new_fd, line, read, 0) < 0) {
        					fprintf(stderr, " unable to write to socket\n");
        					exit(12);
    					} 
   				}
   				free(line);  
   				fclose(fd);             	
               	}                
        	}
	}   	
			
}

int main(int argc, char *argv[])
{
    //configure syslog
    openlog("aesdsocket", LOG_PID, LOG_USER);

    struct sockaddr_storage their_addr;
    //socklen_t addr_size;
    struct addrinfo hints, *res;
    int new_fd;
    int status;
    int opt = 1;
    char s[INET6_ADDRSTRLEN];
    socklen_t sin_size;

    remove(LOG_FILE); 
    printf("start server program !!!\n");
    
    //regisgter signal handlers 
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // getaddressinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
      exit(1);
    }

    // make socket
    if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      fprintf(stderr, "error creating socket\n");
      exit(2);   
    }
    
    // Forcefully attaching socket
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        fprintf(stderr, "error in setting setsockopt\n");
        exit(3);
    }
    
    //bind
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        close(sockfd);
        fprintf(stderr, "error binding to port\n");
        exit(4);
    }
          
    freeaddrinfo(res); // all done with this structure
    
    // after binding we see if we can go into deamon mode
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error forking process");
            exit(5);
        }

        if (pid > 0) {
            // Parent process in daemon mode
            exit(0);
        }

        // Child process continues
        setsid();
        chdir("/");
        umask(0);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
     
    // listen to in comming connections 
    if (listen(sockfd, BACKLOG) == -1) {
        fprintf(stderr, "error listning on port \n");
        exit(6);
    }
    
    while(1) {  // main accept() loop
	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd == -1) {
	    fprintf(stderr, "error accepting connection \n");
	    continue;
	}

	inet_ntop(their_addr.ss_family,
	    get_in_addr((struct sockaddr *)&their_addr),
	    s, sizeof s);
	printf("Accepted connection from %s\n", s);    
	syslog(LOG_USER, "Accepted 1 connection from %s\n", s);

        process_clients(new_fd);
       	
   }
   
   remove(LOG_FILE);  
   closelog();
   return 0;  
}
