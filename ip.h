#ifndef IP_H
#define IP_H

#include "net.h"

#define IP_VERSION_IPV4 4

#define IP_HDR_SIZE_MIN 20
#define IP_HDR_SIZE_MAX 60

#define IP_TOTAL_SIZE_MAX UINT16_MAX /* maximum value of uint16 */
#define IP_PAYLOAD_SIZE_MAX (IP_TOTAL_SIZE_MAX - IP_HDR_SIZE_MIN)

#define IP_ADDR_LEN 4
#define IP_ADDR_STR_LEN 16 /* "ddd.ddd.ddd.ddd\0" */

#define IP_PROTOCOL_ICMP  1
#define IP_PROTOCOL_TCP   6
#define IP_PROTOCOL_UDP  17

typedef uint32_t ip_addr_t;

// SAFETY: the first memmber must be `struct net_iface`.
struct ip_iface {
    struct net_iface iface;
    struct ip_iface *next;
    ip_addr_t unicast;
    ip_addr_t netmask;
    ip_addr_t broadcast;
};

#define IP_ENDPOINT_STR_LEN (IP_ADDR_STR_LEN + 6) /* xxx.xxx.xxx.xxx:yyyyy\0 */

struct ip_endpoint {
    ip_addr_t addr;
    uint16_t port;
};

extern const ip_addr_t IP_ADDR_ANY;
extern const ip_addr_t IP_ADDR_BROADCAST;

int ip_endpoint_pton(const char *p, struct ip_endpoint *n);
char * ip_endpoint_ntop(const struct ip_endpoint *n, char *p, size_t size);

int ip_addr_pton(const char *p, ip_addr_t *n);
char *ip_addr_ntop(ip_addr_t n, char *p, size_t size);
int ip_route_set_default_gateway(struct ip_iface *iface, const char *gateway);
struct ip_iface * ip_route_get_iface(ip_addr_t dst);

struct ip_iface *ip_iface_alloc(const char *unicast, const char *netmask);
int ip_iface_register(struct net_device *dev, struct ip_iface *iface);
int ip_protocol_register(uint8_t type, void (*handler)(const uint8_t *data, size_t len, ip_addr_t src, ip_addr_t dst, struct ip_iface *iface));
struct ip_iface *ip_iface_select(ip_addr_t addr);
int ip_init(void);
ssize_t ip_output(uint8_t protocol, const uint8_t *data, size_t len, ip_addr_t src, ip_addr_t dst);

#endif