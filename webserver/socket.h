#ifndef __SOCKET_H__
#define __SOCKET_H__

void traitement_signal(int sig);
int creer_serveur(int port);
void initialiser_signaux(void);
int accept_client(int socketServeur);

enum http_method {
  HTTP_GET,
  HTTP_UNSUPPORTED,
};

typedef struct {
  enum http_method method;
  int major_version;
  int minor_version;
  char *url;
} http_request;

int parse_http_request(char *request_line, http_request *request);
void skip_headers(FILE *file);
void send_status (FILE *client, int code, const char *reason_phrase);
void send_response(FILE *client, int code, const char *reason_phrase, const char *message_body);
#endif
