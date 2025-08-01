#ifndef TCP_H
#define TCP_H

#include "ip.h"

int tcp_init(void);
int tcp_open_rfc793(struct ip_endpoint *local, struct ip_endpoint *foreign, int active);
int tcp_close(int id);

#endif