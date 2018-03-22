#include "myLibs.hpp"
#include <list>

using namespace std;

int main(int argc, char *argv[]){
	
	if(argc!=4){
		printf("USAGE:%s LOOPBACK (1 TRUE) PORT MSG\n",argv[0]);
		return -1;
	}

	char* msg = argv[3];
	int port = atoi(argv[2]);
	bool loop;
	
	if (atoi(argv[1]) == 1){
		loop = true;
	}else{
		loop = false;
	}

	printf("apro il server... ");
	ServerTCP* myself = new ServerTCP(port, loop);
	printf("fatto.\n");
	list<Connessione*> connessioni;

	printf("accetto prima connessione... ");
	connessioni.push_back(myself->accetta());
	printf("fatto.\naccetto seconda connessione...");
	connessioni.push_back(myself->accetta());
	printf("fatto.\n");

	for(Connessione* conn : connessioni){
		char* ricevuto = conn->ricevi();
		printf("ho ricevuto questo: %s\n", ricevuto);
		free(ricevuto);
	}

	for(Connessione* conn : connessioni){
		conn->invia(msg);
		printf("ho mandato una risposta\n");
	}

	delete(myself);
	free(msg);

	delete(&connessioni);
	return 0;
}