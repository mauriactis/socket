#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define MAX_CONN 50
#define MAX_MSG 4096
#define LOOPBACK "127.0.0.1"


//Prototipi
void errore(int, char*);

int main(int argc, char* argv[]) {
	
	if (argc != 3) {
		printf("USAGE:%s PORT MSG_ANSWER\n", argv[0]);
		return(-1);
	}

	int sock_id;
	char buffer[MAX_MSG + 1];
	printf("creo socket... ");
	sock_id = socket(AF_INET, SOCK_STREAM, 0);
	printf("check.\n");
	struct sockaddr_in myself;
	struct sockaddr_in mitt;

	myself.sin_family = AF_INET;
	inet_aton(LOOPBACK, &myself.sin_addr);
	myself.sin_port = htons(atoi(argv[1]));
	for (int i = 0; i < 8; i++) {
		myself.sin_zero[i] = 0;
	}
	printf("bind in corso... ");
	if (bind(sock_id, (struct sockaddr*)&myself, (socklen_t)sizeof(struct sockaddr_in))) {
		errore(-2, "bind()");
	}
	printf("check.\n");
	printf("listen in corso... ");
	if(listen(sock_id, MAX_CONN)) {
		errore(-3, "listen()");
	}
	printf("check.\n");
	printf("ascolto nuove connessioni\n");
	int length = sizeof(struct sockaddr_in);
	
	int conn_id = accept(sock_id, (struct sockaddr*)&mitt, (socklen_t*)&length);
	printf("check.\n");
	printf("ricezione in corso... ");
	int ret = recv(conn_id, buffer, MAX_MSG, 0);
	if (ret <= 0) {
		errore(-4, "recv()");
	}
	printf("ricevuto messaggio.\n");
	buffer[ret] = '\0';
	printf("ho ricevuto questo: %s\n",buffer);
	printf("invio risposta... ");
	ret = send(conn_id, argv[2], strlen(argv[2]) + 1, 0);
	if (ret != strlen(argv[2])+1) {
		errore(-5, "send()");
	}
	printf("check.\n");
	printf("chiudo la connessione... ");
	shutdown(conn_id, SHUT_RDWR);
	printf("check.\n");

	printf("chiudo il socket... ");
	close(sock_id);
	printf("check.\n");
	return 0;
}

/* gestisce gli errori bloccanti */
void errore(int retCode, char* description){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}