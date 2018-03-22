#include "SocketUDP.hpp"
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int myself_port;
	SocketUDP* myself;
	Address sender;
	char* msg;
	if(argc!=2){
		printf("USAGE:%s PORT\n",argv[0]);
		return -1;
	}

	myself_port = atoi(argv[1]);

	myself = new SocketUDP(myself_port);
	myself->broadcast(true);

	while(msg = myself->ricevi(&sender)){
		char* mitt_addr = sender.toString();
		printf("[%s]%s\n",sendermsg);

		free(msg);
	}

	free(msg);
	delete(myself);
	delete(sender);
	return 0;
}


char* inputStr(){
	char buffer[MAX_STR + 1];
	int i;
	for(i=0;(buffer[i]=getchar())!='\n';i++){}
	buffer[i] = '\0';
	return dupstr(buffer);
}