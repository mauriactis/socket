class Dictionary{
private:
	FILE* fp;
public:
	Dictionary(char*);
	~Dictionary();
	char* read();
};

Dictionary::Dictionary(char* nomeFile){
	this->fp = fopen(nomeFile,"r");
}

Dictionary::~Dictionary(){
	fclose(fp);
}

/*
* legge una riga dal file, se siamo alla fine restituisce NULL altrimenti restituisce una copia della riga -> da qualche parte c'è la free
*/
char* Dictionary::read(){
	char* ret = read_row_from_file(this-fp);
	if(ret == EOF) return NULL;
	return strdup(ret);
}

class AntiPV{
private:
	Dictionary* userids; //-> lista di tutti gli username
	Dictionary* passwds; //-> lista di tutte le password
public:
	AntiPV(char* ,char*);	// costruttore, valorizza semplicemente le proprietà
	~AntiPV();				// distruttore, fa la delete dei due Dictionary*
	void crack(Address*);	// metodo principale, descritto con commento in testa al metodo
};

class ClientPV{
private:
	ClientTCP* socket;		//-> tramite tra la classe AntiPV e il server da hackerare
public:
	ClientPV(ClientTCP* client);	// costruttore, valorizza semplicemente la proprietà
	~ClientPV();					// distruttore, fa la delete della proprietà
	bool try_user(char*);			// prova l'username che riceve come stringa e restituisce vero se il server gli risponde con USROK
	bool try_password(char*);		// prova la passwprd che riceve come stringa e restituisce vero se il server gli risponde con PSWOK
};


AntiPV::AntiPV(char* users, char* passwords){
	this->userids = new Dictionary(users);
	this->passwds = new Dictionary(passwords);
}

AntiPV::~AntiPV(){
	delete(userids);
	delete(passwds);
}

/*
	LEGGIMI
	questo è il metodo principale su cui si basa il programma, difatti nel main viene semplicemente richiamato questo metodo e basta
	qui descrivo come funziona passo passo:
	iniziamo instanziando tutta la merda che ci serve, di seguito descrivo tutte le variabili a cosa servono:
	char msg_username[MAX_MSG];						->	conterrà il messaggio "PV v1.0 USR " + l'username corrente, viene valorizzato con la sprintf
	char msg_password[MAX_MSG];						->	conterrà il messaggio "PV v1.0 PSW " + la password corrente, viene valorizzato con la sprintf
	ClientPV* clientpv;								->	si occuperà di fare la try_user e la try_password con ognuna di queste, finchè non restituirà vero
	ClientTCP* clienttcp = new ClientTCP();			->	viene utilizzato come mezzo di comunicazione tra il clientpv e il server
	char* current_username;							->	stringa che contiene l'attuale username con cui il clientpv farà la try_user
	bool trovato = false;							->	booleano che ci permette di uscire dal while quando troviamo l'username o la password
	char* current_password;							->	stringa che contiene l'attuale password con cui il clientpv farà la try_password

	il metodo è diviso principalmente in due parti logiche, ovvero la prova dell'username e la prova della password
	praticamente è diviso in due while, il primo che prova tutti gli username finchè non ci restituisce USROK e il secondo che fa la stessa cosa con le password


	DESCRIZIONE PRIMO WHILE: USERNAME
		nella condizione abbiamo una AND tra !trovato e (current_username = this->userids->read()) != NULL
		questo ci permette di fare 2 cose-> innanzitutto valorizza current_username con un nuovo user, e verifica di non essere arrivata a fine del file
		ma con la !trovato prima ci permette di uscire dal while (dato che l'username dentro current_username è quello giusto) senza sovrascriverlo con la read()

		come prima cosa nel while ci connettiamo al server
		una volta connessi istanziamo il nostro clientpv in modo che abbia all'interno un clienttcp connesso al server, per potergli mandare le richieste
		con la riga sprintf(msg_username, "PV v1.0 USR %s",current_username); costruiamo la stringa da inviare al server (come da foglio di verifica)

		dopo di ciò valoriziamo trovato con quello che ci restituisce la try_user

		controlliamo se la try_user ci ha restituito false perchè in questo caso dobbiamo ripetere tutto quanto, quindi de-istanziamo un po' di schifo 
		de-istanziamo clienttcp e lo re-istanziamo perchè se no rimarrebbe col socket aperto e quando proverebbe a fare connetti restituirebbe un errore
		facciamo la free di current_username perchè essendo restituito dal metodo read() è frutto di una strdup e va ripulito

	usciamo dal while per due motivi: 	1. il file è finito e non abbiamo trovato nessun username
										2. abbiamo trovato un username giusto e trovato vale TRUE
	per cui controlliamo se siamo usciti per il secondo motivo e se è così stampiamo l'ultimo username provato, ovvero quello contenuto da current_username


	riportiamo trovato a false così lo riutilizziamo per le password


	SECONDO CICLO WHILE: PASSWORD
		il secondo ciclo while è molto simile al primo dato che il concetto è lo stesso
		le uniche cose che cambiano sono:
		1.leggiamo dal dizionario delle password e non da quello degli username
		2.non costruiamo di nuovo il msg_username dato che sarebbe sempre uguale all'ultimo usato (l'username non cambia, abbiamo quello giusto)
		3. costruiamo il msg_password e facciamo la try_password 
	
	alla fine se abbiamo trovato una password(come subito dopo il primo ciclo while) la stampiamo a video
*/

void AntiPV::crack(Address* a){
	char msg_username[MAX_MSG];
	char msg_password[MAX_MSG];
	ClientPV* clientpv;
	ClientTCP* clienttcp = new ClientTCP();
	char* current_username;
	bool trovato = false;
	char* current_password;
	while(!trovato && (current_username = this->userids->read()) != NULL){
		clienttcp->connetti(a);
		clientpv = new ClientPV(clienttcp);
		sprintf(msg_username, "PV v1.0 USR %s",current_username);
		trovato = clientpv->try_user(msg_username);
		if(!trovato){
			delete(clientpv);
			delete(clienttcp);
			clienttcp = new ClientTCP();
			free(current_username);
		}
	}
	if(trovato){
		printf("user: %s\n", current_username);
	}
	trovato = false;
	
	while(!trovato && (current_password = this->passwds->read()) != NULL){
		clienttcp->connetti(a);
		clientpv = new ClientPV(clienttcp);
		clientpv->try_user(msg_username);
		sprintf(msg_password, "PV v1.0 PSW %s",current_password);
		trovato = clientpv->try_password(msg_password);
		if(!trovato){
			delete(clientpv);
			delete(clienttcp);
			clienttcp = new ClientTCP();
			free(current_password);
		}
	}
	if(trovato){
		printf("pass: %s\n", current_password);
	}
}

ClientPV::ClientPV(ClientTCP* client){
	this->socket = client;
}

ClientPV::~ClientPV(){
	delete(this->socket);
}

/*
invia il msg_username ricevuto da crack tramite la proprietà socket (ClientTCP*), se riesce a mandarlo riceve una risposta e la salva come stringa
controlla che la risposta sia di tipo positivo, se sì restituisce true, altrimenti false
*/
bool ClientPV::try_user(char* s){
	if(this->socket->invia(s)){
		char* risposta = this->socket->ricevi();
		if(cmp_str(risposta, "PV v1.0 USROK")){
			return true;
		}else{
			return false;
		}
	}
}

/*
invia il msg_password ricevuto da crack tramite la proprietà socket (ClientTCP*), se riesce a mandarlo riceve una risposta e la salva come stringa
controlla che la risposta sia di tipo positivo, se sì restituisce true, altrimenti false
*/
bool ClientPV::try_password(char* s){
	if(this->socket->invia(s)){
		char* risposta = this->socket->ricevi();
		if(cmp_str(risposta, "PV v1.0 PSWOK")){
			return true;
		}else{
			return false;
		}
	}
}

/*
	main di test del programma
*/

int main(int argc; char** argv){
	if(argc!=3){
		printf("USAGE:%s IP PORT\n",argv[0]);
		return -1;
	}
	int port = atoi(argv[2]);
	char* ip = argv[1];
	Address* server = new Address(ip, port);
	AntiPV myself = new AntiPV("./users.txt", "./passwords.txt"); // i due file sono inventati
	myself->crack(server);
	delete(myself);
	delete(server);
	return 0;
}
