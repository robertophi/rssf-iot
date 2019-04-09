#include <contiki.h>
#include <contiki-net.h>
#include <net/ip/resolv.h>
#include <stdio.h>
#include "utils.h"
#include "sensor-timer.h"

#define SEND_INTERVAL		(5 * CLOCK_SECOND)

static struct etimer et;
static struct uip_udp_conn *client_conn;
static uip_ipaddr_t ipaddr;

static void udp_handler(void);
static void send_packet(void);

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "Processo UDP cliente");

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&resolv_process, &udp_client_process, &sensor_process);

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
  sprintf(contiki_hostname,"sensor %02X%02X",linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);
  resolv_set_hostname(contiki_hostname);
  printf("Configurando hostname para %s\r\n",contiki_hostname);


  /********** CONECTANDO AO SERVIDOR UDP **********/
  // Encontrar endereço IPv6 do servidor
  static resolv_status_t status = RESOLV_STATUS_UNCACHED;
  while(status != RESOLV_STATUS_CACHED)
  {
      status = set_connection_address(&ipaddr, UDP_CONNECTION_ADDR);

      if(status == RESOLV_STATUS_RESOLVING)
      {
          PROCESS_WAIT_EVENT();
      }
      else if(status != RESOLV_STATUS_CACHED)
      {
          printf("Nao foi possivel obter um IPv6 do servidor.\r\n");
          PROCESS_WAIT_EVENT();
      }
  }
  // Se não utilizar mDNS, use a linha abaixo para registrar IPv6 destino
  // uip_ip6addr(&ipaddr, 0xfe80, 0, 0, 0, 0x215, 0x2000, 0x0002, 0x2145);

  // Criando socket UDP para conexão com host:porta remoto
  client_conn = udp_new(&ipaddr, UIP_HTONS(CONN_PORT), NULL);
  udp_bind(client_conn, UIP_HTONS(CONN_PORT));

  printf("Servidor UDP: ");
  printipv6(&client_conn->ripaddr);
  printf("\r\nPorta local: %u.\r\nPorta remota: %u.\r\n", UIP_HTONS(client_conn->lport),
                                                          UIP_HTONS(client_conn->rport));

  // Se chegou até aqui, então a conexão foi bem sucedida!
  // Agora vamos enviar e receber dados!

  etimer_set(&et, SEND_INTERVAL);

  while(1)
  {
    PROCESS_YIELD();
    // PROCESS_WAIT_EVENT();
    if(etimer_expired(&et))
    {
      send_packet();
      etimer_restart(&et);
    }
    else if(ev == tcpip_event)
    {
      udp_handler();
    }
  }

  PROCESS_END();
}


/*------------------FUNÇÕES AUXILIARES---------------------------------------*/
static void send_packet(void)
{
    int_payload_t payload;

    payload.i32 = get_temp_average();
    printf("Vai enviar: %ld\n", payload.i32);

    if(uip_ds6_get_global(ADDR_PREFERRED) == NULL) {
      printf("Aguardando auto-configuracao de IP\n");
      return;
    }

    uip_udp_packet_send(client_conn, payload.i8, sizeof(payload));
}
/*---------------------------------------------------------------------------*/
static void udp_handler(void)
{
    int_payload_t payload;

    if(uip_newdata()) {
      memcpy(payload.i8, uip_appdata, sizeof(payload));
      printf("Resposta do servidor: '%ld'\n", payload.i32);
    }
}
/*---------------------------------------------------------------------------*/
