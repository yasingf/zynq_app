#ifndef _server_h
#define _server_h

#include "cJSON.h"

#define SERVER_IP "192.168.6.40"
#define SERVER_PORT 6401

int server_init(const char *ip, int port);
cJSON* server_listen();
int server_send(cJSON *json_data);
void server_close();

#endif
