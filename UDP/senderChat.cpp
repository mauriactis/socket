#include "SocketUDP.hpp"
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_STR 4096
#define EXIT_CMD "bye"

int main(int argc, char *argv[])
{
	int server_port;
	char* server_ip;
	SocketUDP* myself;
	Address* server;
	char* msg;
	if(argc!=3){
		printf("USAGE:%s BROADCAST_IP PORT\n",argv[0]);
		return -1;
	}

	server_ip = argv[1];
	server_port = atoi(argv[2]);

	server = new Address(server_ip,server_port);
	myself = new SocketUDP();
	myself->broadcast(true);
	while(cmpstr(msg = inputStr(),EXIT_CMD)){
		myself->invia(msg, *server);
		free(msg);
	}
	free(msg);
	delete(myself);
	delete(server);
	return 0;
}


char* inputStr(){
	char buffer[MAX_STR + 1];
	int i;
	for(i=0;(buffer[i]=getchar())!='\n';i++){}
	buffer[i] = '\0';
	return dupstr(buffer);
}