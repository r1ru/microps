#ifndef UDP_H
#define UDP_H

#include <stddef.h>
#include <stdint.h>

#include "ip.h"

ssize_t udp_output(struct ip_endpoint *src, struct ip_endpoint *dst, const uint8_t *buf, size_t len);
int udp_init(void);
int udp_open(void);
int udp_bind(int index, struct ip_endpoint *local);
int udp_close(int id);
int udp_bind(int index, struct ip_endpoint *local);
ssize_t udp_sendto(int id, uint8_t *buf, size_t len, struct ip_endpoint *foreign);
ssize_t udp_recvfrom(int id, uint8_t *buf, size_t size, struct ip_endpoint *foreign);

#endif