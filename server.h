#ifndef _server_h
#define _server_h

#include "cJSON.h"

#define SERVER_IP "192.168.6.100"
#define SERVER_PORT 6401

int server_init(const char *ip, int port);
void wait_for_connection();
cJSON *receive_data();
int server_send(cJSON *json_data);
void server_close();

#endif
