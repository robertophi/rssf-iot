#include <contiki.h>
#include <contiki-net.h>
#include <net/rpl/rpl.h>
#include <stdio.h>

#include "utils.h"
#include "temperature-server.h"
#include "leds-server.h"

/*---------------------------------------------------------------------------*/
PROCESS(udp_server_process, "Main UDP Server Process");
AUTOSTART_PROCESSES(&resolv_process,
                    &udp_server_process,
                    &temperature_udp_server_process,
                    &leds_udp_server_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  uip_ipaddr_t ipaddr;
  rpl_dag_t *dag;

  PROCESS_BEGIN();

  printf("UDP server started\n");

  // Suporte a mDNS
  resolv_set_hostname(UDP_CONNECTION_ADDR);
  printf("Setting hostname to %s\n", UDP_CONNECTION_ADDR);

  // Configura configuração para roteamento (dag/rpl)
  uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  print_local_addresses();

  dag = rpl_set_root(RPL_DEFAULT_INSTANCE,
                     &uip_ds6_get_global(ADDR_PREFERRED)->ipaddr);
  if(dag != NULL)
  {
    uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
    rpl_set_prefix(dag, &ipaddr, 64);
    printf("Created a new RPL dag with ID: ");
    printipv6(&dag->dag_id);
    printf("\n");
  }

  PROCESS_END();
}

