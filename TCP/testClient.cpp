#include "./ClientTCP.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if(argc != 4){
		printf("USAGE:%s IP PORT MSG\n",argv[0]); return -1;
	}

	printf("istanzio socket client... ");
	ClientTCP* client = new ClientTCP();
	printf("check.\n");
	printf("instanzio address server... ");
	Address* server = new Address(argv[1],atoi(argv[2]));
	printf("check.\n");
	printf("tento connessione server... ");
	if(client->connetti(*server)){
		printf("ERRORE CONNESSIONE.\n");
		return -2;
	} 
	printf("check.\n");
	printf("invio messaggio... ");
	if(client->invia(argv[3])){
		printf("ERRORE INVIO.\n");
		return -3;
	}
	printf("check.\n");
	printf("ricevo messaggio... ");
	char* msg = client->ricevi();
	if(!msg){
		printf("ERRORE RICEZIONE.\n");
		return -4;
	}
	printf("check.\n");
	printf("[%s:%d]%s\n",server->getIp(),server->getPort(),msg);

	printf("chiudo tutto... ");
	delete(client); delete(server); free(msg);
	printf("check.\n");
	return 0;
}