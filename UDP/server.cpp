#include "SocketUDP.hpp"
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Definizione della costante che indica la lunghezza massima di un messaggio
#define MAX_MSG 4096

//Prototipi
void errore(int, char*);

int main(int argc, char *argv[])
{
	//Controllo del numero di parametri
	if(argc!=4){
		printf("USAGE:%s SERVER_PORT LOOPBACK(0=true/1=false) MESSAGE", argv[0]);
		return (-1);
	}

	//Dichiarazione variabili
	char* myselfIp, *clientIP;
	char* msg,* buffer;
	int sockId;
	int myselfPort, clientPort;
	int i;
	int loopback;

	//Assegnazione dei valori ricevuti tramite parametro alle variabili
	myselfPort = atoi(argv[1]);
	loopback = atoi(argv[2]);
	msg = argv[3];
	
	SocketUDP* socket;


	printf("opening socket on port %d...", myselfPort);
	fflush(stdout);

	//Creazione di un socket relativo ad un server in locale
	if(loopback == 0){	
		 socket = new SocketUDP(myselfPort, true);
	}else{
		 socket = new SocketUDP(myselfPort, false);
	}
	printf("ok\n");
	fflush(stdout);


	//Creazione della struttura che indica il mittente
	Address mitt;

	//Attesa del messaggio da parte del mittente

	printf("listening...");
	fflush(stdout);

	buffer = socket->ricevi(&mitt);
	printf("ok\n");
	fflush(stdout);

	if (!buffer) errore(-7,"Ricevuto messaggio vuoto");

	printf("Il messaggio ricevuto è: %s\n", buffer);
	fflush(stdout);

	/*//Costruzione della struttura che identifica il client
	struct sockaddr_in dest;
	dest.sin_family=AF_INET_
	inet_aton(*clientIP, &dest.sin_addr);
	dest.sin_port = htons(clientPort);
	for(i=0; i<8; i++){
		set.sin_zero[1] = 0;
	}*/

	//Invio della risposta al client
	printf("sending an answer...");
	fflush(stdout);
	socket->invia(msg, mitt);
	printf("ok\n");
	fflush(stdout);
	
	//Eliminazione del socket
	printf("closing the socket...");
	fflush(stdout);

	delete(socket);
	printf("OK\n");
	fflush(stdout);

	return 0;
}


/* gestisce gli errori bloccanti */
void errore(int retCode, char* description){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}
