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


const char *messageBienvenue = "Bonjour, je m'appelle C3PO, interprete du serveur web code en C et voici mes createurs Ali Douali et Paul Dumont.\nJe suis dispose a repondre a vos demandes jour et nuit.\nVous allez etre conduits dans les profondeurs du serveur web, le repere des tout puissants createurs.\nVous decouvrirez une nouvelle forme de douleur et de souffrance, en etant lentement codes pendant plus de... 1000 ans.\n";

void traitement_signal(int sig){
  printf("Signal %d recu\n" ,sig);
  wait(&sig);
}

void initialiser_signaux(void){
  struct sigaction sa;
  sa.sa_handler = traitement_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if(sigaction(SIGCHLD, &sa, NULL) == -1){
    perror("sigaction(SIGCHLD)");
  }
}

int creer_serveur(int port){
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = INADDR_ANY;
  int socketServeur;
  
  if((socketServeur = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("socketServeur");
    return -1;
  }

  int optval = 1;
  if(setsockopt(socketServeur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
    perror("Can not set SO_REUSSEADDR option");
    return -1;
  }

  initialiser_signaux();
  
  if(bind(socketServeur, (struct sockaddr *)&saddr, sizeof(saddr)) == -1){
    perror("bind socketServeur");
    return -1;
  }

  if(listen(socketServeur, 10) == -1){
    perror("listen socketServeur");
    return -1;
  }

  return socketServeur;
}

int accept_client(int socketServeur){
  int socketClient;

  if((socketClient = accept(socketServeur, NULL, NULL)) == -1){
    if (errno != EINTR) {
      perror("accept");
    }
    return -1;
  }

  if(fork() == 0){
    char* buffer[1024];
    int readed = 0;
    
    write(socketClient, messageBienvenue, strlen(messageBienvenue));
    
    while(1) {
      if((readed = read(socketClient, buffer, sizeof(buffer))) == -1) {
        perror("read client");
	return -1;
      }
      write(socketClient, buffer, readed);
    }
    close(socketServeur);
    close(socketClient);
    exit(1);
  }
  close(socketClient);
  
  return 0;
}


