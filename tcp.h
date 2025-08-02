#ifndef TCP_H
#define TCP_H

#include "ip.h"

int tcp_init(void);
int tcp_open_rfc793(struct ip_endpoint *local, struct ip_endpoint *foreign, int active);
ssize_t tcp_send(int id, uint8_t *data, size_t len);
ssize_t tcp_receive(int id, uint8_t *buf, size_t size);
int tcp_close(int id);

#endif