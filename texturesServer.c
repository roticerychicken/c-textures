#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <openssl/sha.h>
#include <pthread.h>

#define ERROR -1
//amount of clients
#define MAX_CLIENTS	5
//size of the buffer
#define MAX_DATA	1024




void* run_server(void* arg)
{
	long new = (long)arg;
	int data_len;
	char data[MAX_DATA];
	
	while(1) {
		
		data_len = 1;	
		while(data_len){
			data_len = recv(new,data,MAX_DATA,0);
			printf("sent msg: %s\n",data);
			if(data_len >= 2) {
				data[data_len] = '\0';
				printf("sent msg: %s\n",data);
			}	
		}
		close(new);
	}



}
int main(int argc,char *argv[]) {

	struct sockaddr_in server;
	struct sockaddr_in client;
	int server_sock = socket(AF_INET,SOCK_STREAM,0);
	long client_sock;
	int sockaddr_len = sizeof(struct sockaddr_in);
	


	server.sin_family = AF_INET;
    	server.sin_port = htons(8081);
    	//inaddr instructs the kernal to listen on all interfaces
    	server.sin_addr.s_addr = INADDR_ANY;
    	bzero(&server.sin_zero,8);

	pthread_t tid[5];
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	
	if ((bind(server_sock, (struct sockaddr *)&server, sockaddr_len)) >= ERROR) {
        	printf("socket created");
		if ((listen(server_sock, MAX_CLIENTS)) == ERROR) {
			perror("listen");
			exit(-1);
   		}
		while(1)
		{
			for(int i = 0; i < 5; i++) {
				client_sock = accept(server_sock, NULL,NULL);
				printf("conected to client %dn",client_sock);
				pthread_create(&tid[i],NULL,run_server,(void *)client_sock);
			}
		}
		close(server_sock);
    	} else {
		printf("socket creation failed");
	}

	
	return 0;
}






