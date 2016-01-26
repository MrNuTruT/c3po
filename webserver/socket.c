#include <stdio.h>
#include <stdlib.h>

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
