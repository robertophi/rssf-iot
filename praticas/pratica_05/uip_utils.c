#include "utils.h"

#include "contiki-net.h"

#include <stdio.h>

void set_global_address(void)
{
  uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
}


void printipv6(uip_ipaddr_t * ipaddr)
{
	uint8_t * ipc = (*ipaddr).u8;
	printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
		ipc[0], ipc[1],  ipc[2],  ipc[3],  ipc[4],  ipc[5],  ipc[6], ipc[7], ipc[8],
		ipc[9], ipc[10], ipc[11], ipc[12], ipc[13], ipc[14], ipc[15]);
}

void print_local_addresses(void)
{
  int i;
  uint8_t state;

  printf("Encerecos IPv6: ");

  for(i = 0; i < UIP_DS6_ADDR_NB; i++)
  {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED))
    {
      printipv6(&uip_ds6_if.addr_list[i].ipaddr);
      printf("\r\n");
    }
  }
}

void udp_get_srcaddr(uip_ipaddr_t * ipaddr)
{
	uip_ipaddr_copy(ipaddr, &((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])->srcipaddr);
}
