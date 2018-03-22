#include "myLibs.hpp"
#define EXIT_CMD "fuckoff"
#define MAX_STR 4096

void* ascolta(void* param);
char* inputStr();
char* trovafile(char* msg);

typedef struct {
	ServerTCP* server;
	char* path;
}Params;

int main(int argc, char *argv[]){
	int port;
	pthread_t receiver_id;
	char* msg;
	ServerTCP* myself;
	char* path_cartella;
	if(argc!=3){
		printf("USAGE:%s PORTA CARTELLA\n",argv[0]);
		return -1;
	}
	port = atoi(argv[1]);
	path_cartella = argv[2];
	myself = new ServerTCP(port,false);

	Params parametri;
	parametri.server = myself;
	parametri.path = path_cartella;

	pthread_create(&receiver_id,NULL, ascolta, (void*) &parametri);

	while(!strcmp(msg = inputStr(),EXIT_CMD)){}
	return 0;
}

char* inputStr(){
	char buffer[MAX_STR + 1];
	int i;
	for(i=0;(buffer[i]=getchar())!='\n';i++){}
	buffer[i+1] = '\0';
	return strdup(buffer);
}

void* ascolta(void* param){
	FILE* f;
	Params* params = (Params*) param;
	ServerTCP* myself = (ServerTCP*) params->server;
	char* path = (char*) params->path; 
	while(1){
		Connessione* conn = myself->accetta();
		char* msg = conn->ricevi();
		char* nomefile = trovafile(msg);
		printf("%s\n",nomefile );
		char totalefile[MAX_STR];
		printf("creo stringa totalefile\n");
		int n = sprintf(totalefile, "%s%s",path,nomefile);
		printf("stringa totalefile\n");
		f = fopen(totalefile,"r");
		char* risultato; char pagina[MAX_STR]; char messaggio[MAX_STR];
		if(f == NULL){
			risultato = "404 NOT FOUND";
		}else{
			risultato = "200 OK\n\n";
			fscanf(f,"%s",pagina);
			printf("%s\n",totalefile);
			printf("%s\n",pagina);
			printf("letto da file\n");
			sprintf(messaggio,"%s%s",risultato,pagina);
			printf("pinobello\n");
			conn->invia(messaggio);
	
			printf("inviato\n");
			fclose(f);
			delete(conn);
		}

	}
}


char* trovafile(char* msg){
	char ret[MAX_STR];
	int i = 4; int j=0;
	while(msg[i]!=' '){
		ret[j] = msg[i];
		i++; j++;
	}
	ret[j] = '\0';
	return strdup(ret);
}
