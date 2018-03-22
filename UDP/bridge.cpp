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
		printf("USAGE:%s MYSELF_PORT DESTINATION_IP DESTINATION_PORT\n", argv[0]);
		return (-1);
	}

	//Dichiarazione variabili
	char* myselfIp, *clientIP, *destIp;
	char* msg, *buffer;
	int sockId;
	int myselfPort, clientPort, destPort;
	int i;

	//Assegnazione dei valori ricevuti tramite parametro alle variabili
	myselfPort = atoi(argv[1]);
	destIp = argv[2];
	destPort = atoi(argv[3]);

	SocketUDP* socket = new SocketUDP(myselfPort, true);

	//Creazione della struttura che indica il mittente
	Address mitt;

	//Attesa del messaggio da parte del mittente
	buffer = socket->ricevi(&mitt);
	printf("Il messaggio arrivato è: %s", buffer);

	//Creazione della struttura che identifica il nuovo mittente
	Address* dest = new Address(destIp, destPort);

	//Invio della risposta al client
	socket->invia(buffer, *dest);
	
	//Eliminazione del socket
	delete(socket);

	return 0;
}


/* gestisce gli errori bloccanti */
void errore(int retCode, char* description){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}
