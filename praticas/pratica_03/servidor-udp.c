#include <contiki.h>
#include <contiki-net.h>
#include <net/rpl/rpl.h>
// #include <dev/leds.h>
#include <stdio.h>

#include "utils.h"

static struct uip_udp_conn *server_conn;

static void udp_handler(void);

/*---------------------------------------------------------------------------*/
PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&resolv_process,&udp_server_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  uip_ipaddr_t ipaddr;
  rpl_dag_t *dag;

  PROCESS_BEGIN();

  // printf("UDP server started\n");

  // Suporte a mDNS
  resolv_set_hostname(UDP_CONNECTION_ADDR);
  // printf("Setting hostname to %s\n", UDP_CONNECTION_ADDR);

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
    // printf("Created a new RPL dag with ID: ");
    printipv6(&dag->dag_id);
    printf("\n");
  }

  // Abre servidor UDP
  server_conn = udp_new(NULL, UIP_HTONS(CONN_PORT), NULL);
  udp_bind(server_conn, UIP_HTONS(CONN_PORT));

  while(1)
  {
    PROCESS_YIELD();
    if(ev == tcpip_event)
    {
      udp_handler();
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
static void udp_handler(void)
{
    int_payload_t payload, response;
    uip_ipaddr_t src_ipaddr;

    if(uip_newdata()) // Tem dado novo na pilha uIP?
    {
        //copia pacote e origem para variáveis locais antes que chegue outro pacote!
        memcpy(payload.i8, uip_appdata, sizeof(payload));
        udp_get_srcaddr(&src_ipaddr);

        // Imprime dados recebidos
        // printf("Server received: '%ld' from ", payload.i32);
        printf("%ld,", payload.i32);
        printipv6(&src_ipaddr);
        printf("\n");

        // Faz alguma coisa com o dado
        if (payload.i32 > 22)
        {
          response.i32 = 2; // Temperatura alta!
        }
        else if (payload.i32 < 22)
        {
          response.i32 = 1; // Temperatura baixa!
        }
        else // payload.i32 == 22
        {
          response.i32 = 0; // Temperatura OK!
        }
    }
    else
    {
      response.i32 = 0;
    }

    server_conn->ripaddr = src_ipaddr;
    uip_udp_packet_send(server_conn, response.i8, sizeof(response));

    /* Restore server connection to allow data from any node */
    memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));

    return;
}
