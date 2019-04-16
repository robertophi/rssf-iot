#include <contiki.h>
#include <contiki-net.h>
#include <net/ip/resolv.h>
#include <stdio.h>
#include "utils.h"

static struct etimer et;
static struct uip_udp_conn *client_conn;
static uip_ipaddr_t ipaddr;


/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "Processo UDP cliente");

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&resolv_process, &udp_client_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Processo UDP cliente -- iniciou\r\n");

  // Bloqueia até que configuração IPv6 seja bem sucedida (SLAAC)
  etimer_set(&et, 2*CLOCK_SECOND);
  while(uip_ds6_get_global(ADDR_PREFERRED) == NULL)
  {
      PROCESS_WAIT_EVENT();
      if(etimer_expired(&et))
      {
          etimer_set(&et, 2*CLOCK_SECOND);
          printf("Aguardando auto-configuracao de IPv6\r\n");
      }
  }
  // Imprime IPv6.
  print_local_addresses();


  // Configura nome local (mDNS).
  char contiki_hostname[16];
  sprintf(contiki_hostname,"sensor%02X%02X",linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);
  resolv_set_hostname(contiki_hostname);
  printf("Configurando hostname para %s\r\n",contiki_hostname);


  /********** CONECTANDO AO SERVIDOR UDP **********/
  // Encontrar endereço IPv6 do servidor
  /*static resolv_status_t status = RESOLV_STATUS_UNCACHED;
  while(status != RESOLV_STATUS_CACHED)
  {
      status = set_connection_address(&ipaddr, UDP_CONNECTION_ADDR);
<<<<<<< HEAD

      if(status == RESOLV_STATUS_RESOLVING)
      {
          PROCESS_WAIT_EVENT();
      }
      else if(status != RESOLV_STATUS_CACHED)
      {
          printf("Nao foi possivel obter IPv6 do servidor.\r\n");
          PROCESS_WAIT_EVENT();
      }
  }*/
  // Se não utilizar mDNS, use a linha abaixo para registrar IPv6 destino
  uip_ip6addr(&ipaddr, 0xfd00, 0, 0, 0, 0x212, 0x4b00, 0x1376, 0x4e03);
=======
      PROCESS_WAIT_EVENT();
  }
  // Se não utilizar mDNS, use a linha abaixo para registrar IPv6 destino
  // uip_ip6addr(&ipaddr, 0xfd00, 0, 0, 0, 0x212, 0x4b00, 0x1376, 0x4e03);
>>>>>>> 2c8e60f9f55a1f6e6bef04203c7cd9e3187d4ced

  // Criando socket UDP para conexão com host:porta remoto
  client_conn = udp_new(&ipaddr, UIP_HTONS(CONN_PORT), NULL);
  udp_bind(client_conn, UIP_HTONS(CONN_PORT));

  printf("Servidor UDP: ");
  printipv6(&client_conn->ripaddr);
  printf("\r\nPorta local: %u.\r\nPorta remota: %u.\r\n", UIP_HTONS(client_conn->lport),
                                                          UIP_HTONS(client_conn->rport));

  // Se chegou até aqui, então a conexão foi bem sucedida!
  // Toca para o próximo experimento! =)

  PROCESS_END();
}
