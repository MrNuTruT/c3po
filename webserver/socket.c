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

void traitement_signal(int sig){
  printf("Signal %d recu\n",sig);
  waitpid(-1,NULL,0);
}

void initialiser_signaux(void) {
  struct sigaction sa ;
  sa.sa_handler = traitement_signal ;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART ;
  if (sigaction(SIGCHLD, &sa, NULL) == -1){
    perror("sigaction(SIGCHLD)");
  }
}

int creer_serveur(int port){
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = INADDR_ANY;
  int socketServeur;
  int socketClient;
  
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

  const char *messageBienvenue = "Bonjour, je m'appelle C3PO, interprete du serveur web code en C et voici mes createurs Ali Douali et Paul Dumont.\nJe suis dispose a repondre a vos demandes jour et nuit.\nVous allez etre conduits dans les profondeurs du serveur web, le repere des tout puissants createurs.\nVous decouvrirez une nouvelle forme de douleur et de souffrance, en etant lentement codes pendant plus de... 1000 ans.\n";
 
  while(1){
    if((socketClient = accept(socketServeur, NULL, NULL)) == -1){
      perror("accept");
      return -1;
    }
  
  if(write(socketClient, messageBienvenue, strlen(messageBienvenue)) == -1){
      perror("write");
      return -1;
  }
    
    if( fork() == 0){
      close(socketServeur);
      int readClient;
      while(1){
        char* buffer[1024];
	readClient = read(socketClient,buffer,sizeof(buffer));
	if(readClient == 0) {
	  perror("read client");
	  return 0; 
	} else if (readClient == -1) {
	  return -1; 
	}
	write(socketClient, buffer, readClient);
      }
      exit(0);
    } 
    close(socketClient);
  }
  close(socketServeur);
  return socketServeur;
}


