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

  const char *messageBienvenue = "Bonjour, je m'appelle C3PO, interprête du serveur web codé en C et voici mes créateurs Ali Douali et Paul Dumont.\nJe suis disposé à répondre à vos demandes jour et nuit.\nVous allez être conduits dans les profondeurs du serveur web, le repère des tout puissants créateurs.\nVous découvrirez une nouvelle forme de douleur et de souffrance, en étant lentement codés pendant plus de... 1000 ans.\n";
  
  if(write(socketClient, messageBienvenue, strlen(messageBienvenue))==-1){
    perror("write");
    return -1;
  }
  
  return socketServeur;
}
