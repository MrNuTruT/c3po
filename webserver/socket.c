#include <errno.h>

#include <ctype.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#include <unistd.h>
#include "socket.h"


const char *messageBienvenue = "Bonjour, je m'appelle C3PO, interprete du serveur web code en C et voici mes createurs Ali Douali et Paul Dumont.\nJe suis dispose a repondre a vos demandes jour et nuit.\nVous allez etre conduits dans les profondeurs du serveur web, le repere des tout puissants createurs.\nVous decouvrirez une nouvelle forme de douleur et de souffrance, en etant lentement codes pendant plus de... 1000 ans.";
const char *c3po = "<C-3PO>";
const char *message_size = "Content-Length: ";

const char *message_200 = "HTTP/1.1 200 Ok\r\n";
const char *message_400_bad_request = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\r\n400 Bad Request\r\n";
const char *message_404 = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 15\r\n\r\n404 Not Found\r\n";

void traitement_signal(int sig){
  printf("Signal %d recu\n",sig);
  waitpid(-1,NULL,0);
}

void fgets_or_exit(char *buffer, int size, FILE *stream){
  if(fgets(buffer, size, stream) == NULL){
    perror("Cannot read");
    exit(1);
  }
  if(buffer[0] == '/'){
    if (!(strcmp(buffer, "\r\n") == 0 || strcmp(buffer, "\n") == 0)) {
      printf("%s", buffer);
    }
  }
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
  //char *line;

  fgets_or_exit(buffer,1024,file);
  
  char *c = buffer;

  if(c[0] != '/'){
    return 404;
  }
  //char buf[128];
  /*
  if((line = fgets(buf,sizeof(buf),file)) == NULL){
    perror("line buffer");
    return -1;
  }
   // marche pas vas direct dans le else 
  if(strncmp("GET", line, 3) == 0){
    char tab[3][10];
    int i = 0;
    int j = 0;
    int mots = 0;
    for(j = 0;j < (int)strlen(line);j++){
      if(isspace(line[i]) != 0 || line[i] == '\n'){
	mots++;
	j=0;
      }
      tab[mots][j] = line[i];
      j++;
    }
    if((mots -1 != 3) || (strcmp("HTTP/1.0",tab[2]) != 0 || strcmp("HTTP/1.1",tab[2]) != 0)){
      return 400;
    }
  }else{
  return 357;
  }*/
 
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


      switch(headerError){
      case 200:
	if ((strcmp(buffer, "\r\n") != 0 && strcmp(buffer, "\n") != 0)) {
	  //if(parse_http_request(buffer,)){
	    
	    fprintf(file, "%s%s%d\r\n\r\n%s %s\r\n", message_200, message_size, (int) (strlen(c3po) + strlen(messageBienvenue) + 3), c3po, messageBienvenue);
	    //  }
	}else{
	  fprintf(file, message_400_bad_request);
	}
	break;
      case 357:
	fprintf(file,"marche pas");
	break;
      case 400:
	fprintf(file, message_400_bad_request);
	break;
      case 404:
	fprintf(file,message_404);
	break;
      default:
	fprintf(file, message_400_bad_request);
	break;
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

//int parse_http_request(const char *request_line, http_request *request){
  
//}
