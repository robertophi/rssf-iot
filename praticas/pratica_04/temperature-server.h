#include <contiki.h>
#include <contiki-net.h>
#include <net/rpl/rpl.h>
#include <dev/leds.h>
#include <stdio.h>

#include "utils.h"

static struct uip_udp_conn *temp_server_conn;

static void temperature_udp_handler(void);

/*---------------------------------------------------------------------------*/
PROCESS(temperature_udp_server_process, "Temperature Server Process");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(temperature_udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Temperature UDP Server Started\n");

  // Abre porta UDP
  temp_server_conn = udp_new(NULL, UIP_HTONS(CONN_PORT_TEMP), NULL);
  udp_bind(temp_server_conn, UIP_HTONS(CONN_PORT_TEMP));

  while(1)
  {
    PROCESS_YIELD();
    if(ev == tcpip_event)
    {
      temperature_udp_handler();
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
static void temperature_udp_handler(void)
{
    temp_payload_t payload, response;
    uip_ipaddr_t src_ipaddr;

    if(uip_newdata()) // Tem dado novo na pilha uIP?
    {
        //copia pacote e origem para variáveis locais antes que chegue outro pacote!
        memcpy(payload.i8, uip_appdata, sizeof(payload));
        udp_get_srcaddr(&src_ipaddr);

        // Imprime dados recebidos
        printf("TEMPERATURE,");
        printipv6(&src_ipaddr);
        printf(",%ld", payload.i32);
        printf("\n");

        // Faz alguma coisa com o dado
        if (payload.i32 > 24)
        {
          response.i32 = 2; // Temperatura alta!
        }
        else if (payload.i32 < 24)
        {
          response.i32 = 1; // Temperatura baixa!
        }
        else // payload.i32 == 24
        {
          response.i32 = 0; // Temperatura OK!
        }
    }
    else
    {
      response.i32 = 0;
    }

    temp_server_conn->ripaddr = src_ipaddr;
    uip_udp_packet_send(temp_server_conn, response.i8, sizeof(response));

    /* Restore server connection to allow data from any node */
    memset(&temp_server_conn->ripaddr, 0, sizeof(temp_server_conn->ripaddr));

    return;
}
