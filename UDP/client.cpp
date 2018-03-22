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
		printf("USAGE:%s SERVER_IP SERVER_PORT MESSAGE", argv[0]);
		return (-1);
	}

	//Dichiarazione variabili
	char* myselfIp, *serverIp;
	char* msg, *buffer;
	int sockId;
	int myselfPort, serverPort;
	int i;

	//Assegnazione dei valori ricevuti tramite parametro alle variabili
	serverIp = argv[1];
	serverPort = atoi(argv[2]);
	msg = argv[3];

	//Creazione di un socket relativo ad un client
	SocketUDP* socket = new SocketUDP();

	//creazione dell' istanza che identifica la destinazione ossia il server
	Address* dest = new Address(serverIp, serverPort);

	/*//Costruzione della struttura che identifica il server
	struct sockaddr_in dest;
	dest.sin_family=AF_INET_
	inet_aton(*clientIP, &dest.sin_addr);
	dest.sin_port = htons(clientPort);
	for(i=0; i<8; i++){
		set.sin_zero[1] = 0;
	}*/

	//Invio del messaggio al server
	if(!socket->invia(msg, *dest)){
		printf("Il messaggio non è stato inviato...\n");
		return (-2);
	}else{
		printf("Il messaggio è stato inviato correttamente.");
	}

	//Creazione di un Address che identifica il mittente (server)
	Address* mitt = new Address();

	//Attesa del messaggio da parte del server
	buffer = socket->ricevi(mitt);
	printf("Il messaggio che mi è stato mandato è: %s", buffer);
	
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
