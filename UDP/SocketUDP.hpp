/************************************************************
SocketUDP.hpp: Libreria che comprende 4 funzioni relative allo sviluppo
di client, server e bridge.

************************************************************/
#include "./Address.hpp" //Si trova nella stessa cartella di questa
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define LOOPBACK "127.0.0.1"
#define MYSELF_IP "0.0.0.0"

#define MAX_MSG 4096

class SocketUDP{
	private:	int sock_id;
	public:	SocketUDP(int port, bool loopback);
			~SocketUDP();
			bool invia(char*, Address);
			// bool inviaraw(Address, void*, int);
			char* ricevi(Address*);
			// void* riceviraw(Address*, int*);
			void errore(int, char*);
            bool broadcast(bool b);
};

//Se io non indico i parametri usa la porta 0 e loopback a false
//La chiamata senza parametri è il client, con la porta è il serverUDP in rete, con porta e loopback a true è il serverUDP in loopback(interno)
SocketUDP::SocketUDP(int port = 0, bool loopback = false) {
	struct sockaddr_in myself;
	int ret;

	sock_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_id == -1){
		errore(-2, "socket()");
	}

	//Se ho la porta faccio la bind altrimenti è un client e non fa la bind
	if(port){
		myself.sin_family = AF_INET;
		//? : è l' equivalente di una iif in visualbasic
		inet_aton(loopback?"127.0.0.1":"0.0.0.0", &myself.sin_addr);
		myself.sin_port = htons(port);

		if(bind(sock_id, (struct sockaddr*)&myself, (socklen_t) sizeof(struct sockaddr_in))){
			errore(-3, "bind()");
		}
	}
}

SocketUDP::~SocketUDP() {
	if(close(sock_id)){
		errore(-3, "shutdown()");
	}
}

char* SocketUDP::ricevi(Address* mitt){
	struct sockaddr_in mitt_addr;
	int len_addr;
	int ret;
	char buffer[MAX_MSG+1];


	len_addr = sizeof(struct sockaddr_in);
	ret = recvfrom(	sock_id,
					buffer,
					MAX_MSG,
					0,
					(struct sockaddr*) &mitt_addr,
					(socklen_t*) &len_addr);


	if(ret<=0) return NULL;
	//if(ret) chiamata a errore o retun null e basta?

	buffer[ret]='\0';

	//mette all' interno del mittente qualcosa
	mitt->setAddress(mitt_addr);
	//Quando torniamo otteniamo un' istanza contente l' indirizzo del mittente



	return strdup(buffer);
}

bool SocketUDP::invia(char* msg, Address dest){
	struct sockaddr_in dest_addr;
	int len_addr;
	int len_msg;
	int ret;

	dest_addr = dest.getAddress();
	len_addr = sizeof(struct sockaddr_in);
	len_msg = strlen(msg) + 1;
	ret = sendto(sock_id, msg, len_msg, 0,(struct sockaddr*) &dest_addr,(socklen_t) len_addr);
	
	//Restituisce se è andata a buon fine
	return (ret == len_msg);
}

/*
bool SocketUDP::inviaraw(Address, void*, int){
	

}

// riceve un file, valorizza il mittente e la lunghezza del file, restituisce il puntatore
void* SocketUDP::riceviraw(Address*,int*){
	


}
*/



bool SocketUDP::broadcast(bool b){
	int len_so= sizeof(int); int val = b? 1: 0;
	return (!setsockopt(sock_id, SOL_SOCKET,SO_BROADCAST, &val,(socklen_t) len_so));
}

/* gestisce gli errori bloccanti */
void SocketUDP::errore(int retCode, char* description){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}
