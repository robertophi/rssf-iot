#ifndef __UTILS_H__
#define __UTILS_H__


#include "contiki.h"
#include "net/ip/resolv.h"

/* From uip_utils.c */
extern void set_global_address(void);
extern void print_local_addresses(void);
extern void printipv6(uip_ipaddr_t * ipaddr);
extern void udp_get_srcaddr(uip_ipaddr_t * ipaddr);

/* From mdns_utils.c */
extern resolv_status_t set_connection_address(uip_ipaddr_t *ipaddr, char *udp_connection_addr);

#endif /* __UTILS_H__ */
