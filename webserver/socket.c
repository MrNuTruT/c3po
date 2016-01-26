#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>

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
  
  if(bind(socketServeur, (struct sockaddr *)&saddr, sizeof(saddr)) == -1){
    perror("bind socketServeur");
    return -1;
  }

  if(listen(socketServeur, 10) == -1){
    perror("listen socketServeur");
    return -1;
  }

  if((socketClient = accept(socketServeur, NULL, NULL)) == -1){
    perror("accept");
  }

  const char *messageBienvenue = "Bonjour, je m'appelle C3PO, interprete du serveur web code en C et voici mes createurs Ali Douali et Paul Dumont.\nJe suis dispose a repondre a vos demandes jour et nuit.\nVous allez etre conduits dans les profondeurs du serveur web, le repere des tout puissants createurs.\nVous decouvrirez une nouvelle forme de douleur et de souffrance, en etant lentement codes pendant plus de... 1000 ans.\n";

  const char* buffer = malloc(128*sizeof(char));
  if(write(socketClient, messageBienvenue, strlen(messageBienvenue)) == -1){
     perror("write");
     return -1;
  }
  
  while(1){
    if(read(0,buffer,sizeof(buffer)) == -1) {
      perror("read client");
      return -1;
    }

    if(write(0, buffer, strlen(buffer)) == -1){
      perror("write client");
      return -1;
    }
    sleep(1);
  }

  free(buffer);
  close(socketServeur);
  close(socketClient);
  return socketServeur;
}
