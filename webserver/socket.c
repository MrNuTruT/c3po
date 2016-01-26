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

  const char *messageBienvenue = "Bonjour, je m'appelle C3PO, interpr�te du serveur web cod� en C et voici mes cr�ateurs Ali Douali et Paul Dumont.\nJe suis dispos� � r�pondre � vos demandes jour et nuit.\nVous allez �tre conduits dans les profondeurs du serveur web, le rep�re des tout puissants cr�ateurs.\nVous d�couvrirez une nouvelle forme de douleur et de souffrance, en �tant lentement cod�s pendant plus de... 1000 ans.\n";
  
  if(write(socketClient, messageBienvenue, strlen(messageBienvenue))==-1){
    perror("write");
    return -1;
  }
  
  return socketServeur;
}
