#ifndef __SOCKET_H__
#define __SOCKET_H__

void traitement_signal(int sig);
int creer_serveur(int port);
void initialiser_signaux(void);
int accept_client(int socketServeur);
#endif
