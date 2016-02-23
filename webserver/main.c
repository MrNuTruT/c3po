#include <stdio.h>
#include <string.h>
#include "socket.h"

int main(void){
  
  if(creer_serveur(8080) == -1){
    perror("creer serveur");
    return -1;
  }
  return 0;
}
