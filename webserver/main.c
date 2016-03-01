#include <stdio.h>
#include <string.h>
#include "socket.h"

int main(void){

  int socketServeur = creer_serveur(8080);
  if(socketServeur == -1){
    perror("creer serveur");
    return -1;
  }

  while(1){
    accept_client(socketServeur);
  }
  return 0;
}
