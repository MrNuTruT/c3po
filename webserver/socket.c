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
const char *http_version = "HTTP-1.1";

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
  http_request request;

  fgets_or_exit(buffer, 1024, file);

  if (parse_http_request(buffer, &request) == 1) {
    if (request.method != HTTP_GET) {
      skip_headers(file);
      send_response(file, 400, "Bad request", "Bad request\r\n");
      return 400;
    } else if (strcmp(request.url, "/")) {
      skip_headers(file);
      send_response(file, 404, "Not Found", "Not Found\r\n");
      return 404;
    } else {
      skip_headers(file);
      return 200;
    }
  } else {
    skip_headers(file);
    send_response(file, 400, "Bad Request", "Bad request\r\n");
    return 400;
  }

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
    
    int headerError = verifie_client_entete(file);

    if(headerError == 200){
      fprintf(file, "%s%s%d\r\n\r\n%s %s\r\n", message_200, message_size, (int) (strlen(c3po) + strlen(messageBienvenue) + 3), c3po, messageBienvenue);
    }    
   
    fflush(file);   
    close(socketClient);
    close(socketServeur);
    exit(1);
  }
  close(socketClient);

  return 0;
}

int parse_http_request(char *request_line, http_request *request){
  char* token = strtok(request_line, " ");
  int line = 0;
  while(token != NULL) {
    if (line == 0 && strcmp("GET", token) == 0) {
      request->method = HTTP_GET;
    } else if (line == 0) {
      request->method = HTTP_UNSUPPORTED;
    } else if (line == 1) {
      request->url = token;
    } else if (line == 2) {
      if (strcmp(" HTTP/1.0\r\n", token) == 0) {
	request->major_version = 1;
	request->minor_version = 0;
      } else if (strcmp(" HTTP/1.1\r\n", token) != 0) {
	request->major_version = 1;
	request->minor_version = 1;
      } else {
	return 0;
      }
    } else {
      return 0;
    }
    line++;
    token = strtok(NULL, " ");
  }
  return line == 3;
}

void skip_headers(FILE *file){
  char buffer[1024];           

  while (1) {
    fgets_or_exit(buffer, 1024, file);
    if (strcmp(buffer, "\r\n") == 0 || strcmp(buffer, "\n") == 0) {
      return;
    }
  }
}

void send_status (FILE *client, int code, const char *reason_phrase){
  char buf[128];

  sprintf(buf, "%d %s\r\n", code, reason_phrase);
  fprintf(client, "%s %d %s\r\nConnection: close\r\n%s %d\r\n\r\n%s", http_version, code, reason_phrase, message_size, (int) (strlen(buf) + 1), buf);
}

void send_response(FILE *client, int code, const char *reason_phrase, const char *message_body){
  if(message_body == NULL) {
  }
  if(code == 400)
    send_status(client, 400, reason_phrase);
  else if(code == 404)
    send_status(client, 404, reason_phrase);
  else
    send_status(client, 400, reason_phrase);
}
