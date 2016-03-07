#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <signal.h>

const char *messageBienvenue = "Bonjour, je m'appelle C3PO, interprete du serveur web code en C et voici mes createurs Ali Douali et Paul Dumont.\nJe suis dispose a repondre a vos demandes jour et nuit.\nVous allez etre conduits dans les profondeurs du serveur web, le repere des tout puissants createurs.\nVous decouvrirez une nouvelle forme de douleur et de souffrance, en etant lentement codes pendant plus de... 1000 ans.";
const char *c3po = "<C-3PO>";
const char *message_200 = "HTTP/1.1 200 Ok\r\n";
const char *message_size = "Content-Length: ";

void traitement_signal(int sig){
  printf("Signal %d recu\n",sig);
  waitpid(-1,NULL,0);
}

void initialiser_signaux(void) {
  struct sigaction sa ;
  sa.sa_handler = traitement_signal ;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  if (sigaction(SIGCHLD, &sa, NULL) == -1){
    perror("sigaction(SIGCHLD)");
  }
}

/*int verifie_client_entete(FILE *file) {
  char buffer[1024];

  if(fgets(buffer, 1024, file) == NULL){
    perror("Cannot read");
    exit(1);
  }
  
  //printf("%s", buffer);

  char *c = buffer;
  //char *words[2];

  if (strlen(c) < 3 && (c[0] != 'G' || c[1] != 'E' || c[2] != 'T')) {
    return 400;
  }
  
  int word = 0;

  while(c[0] != '\0') {
    if(c[0] == ' ') {
     
      if (word == 2) {
	return 400;
      }
      words[word] = c;
      word ++;
    }
    c++;
  }

  
  if (strcmp(" HTTP/1.0\r\n", words[1]) != 0 || strcmp(" HTTP/1.1\r\n", words[1]) != 0) {
    return 400;
  }
  
    if (words[0][1] != '/' || words[0][2] != ' ') {
    return 404;
    }

  return 200;
} */

int creer_serveur(int port) {
  int socketServeur;

  socketServeur = socket(AF_INET, SOCK_STREAM, 0);

  if (socketServeur == -1) {
    perror("server_socket");
    return -1;
  }

  int optval = 1;
  if(setsockopt(socketServeur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1){
    perror("Can not set SO_REUSEADDR option");
  }

  initialiser_signaux();

  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(socketServeur, (struct sockaddr *) &saddr, sizeof(saddr)) == -1) {
    perror("bind socketServeur");
  }

  if (listen(socketServeur, 10) == -1) {
    perror("lister socketServeur");
  }

  return socketServeur;
}

int verifie_client_entete(FILE *file) {
  char buffer[1024];

  if(fgets(buffer, 1024, file) == NULL){
    perror("Cannot read");
    exit(1);
  }

  printf("%s", buffer);
  
  char *c = buffer;

  if (strlen(c) < 3 && (c[0] != 'G' || c[1] != 'E' || c[2] != 'T')) {
    return 400;
  }
 
  return 200;
} 

int accept_client(int socketServeur) {
  int socketClient;
  socketClient = accept(socketServeur, NULL, NULL);
  
  if(socketClient == -1) {
    perror("accept");
    return -1;
  }

  if(fork() == 0){
    FILE *file = fdopen(socketClient, "w+");
    char buffer[1024];

    while(1) {
      int headerError = verifie_client_entete(file);

      if(headerError == 200){
	if ((strcmp(buffer, "\r\n") != 0 && strcmp(buffer, "\n") != 0)) {
	  fprintf(file, "%s%s%d\r\n\r\n%s %s\r\n", message_200, message_size, (int) (strlen(c3po) + strlen(messageBienvenue) + 3), c3po, messageBienvenue);
	}
      }else{
	fprintf(file, "different de 200\r\n");
      }
    }
   
    fflush(file);   
    close(socketClient);
    close(socketServeur);
    exit(1);
  }
  close(socketClient);

  return 0;
}








