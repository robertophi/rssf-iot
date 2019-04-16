#include <contiki.h>
#include <contiki-net.h>
#include <net/rpl/rpl.h>
#include <dev/leds.h>
#include <stdio.h>

#include "utils.h"

static struct uip_udp_conn *leds_server_conn;

static void leds_udp_handler(void);

/*---------------------------------------------------------------------------*/
PROCESS(leds_udp_server_process, "LEDs Server Process");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(leds_udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  printf("LEDs UDP Server Started\n");

  // Abre porta UDP
  leds_server_conn = udp_new(NULL, UIP_HTONS(CONN_PORT_LEDS), NULL);
  udp_bind(leds_server_conn, UIP_HTONS(CONN_PORT_LEDS));

  while(1)
  {
    PROCESS_YIELD();
    if(ev == tcpip_event)
    {
      leds_udp_handler();
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
static void leds_udp_handler(void)
{
    leds_payload_t payload;
    uip_ipaddr_t src_ipaddr;

    if(uip_newdata()) // Tem dado novo na pilha uIP?
    {
        //copia pacote e origem para variáveis locais antes que chegue outro pacote!
        memcpy(payload.i8, uip_appdata, sizeof(payload));
        udp_get_srcaddr(&src_ipaddr);

        // Imprime dados recebidos
        printf("LEDS,");
        printipv6(&src_ipaddr);
        printf(",%d,%d,", payload.comando.cmd, payload.comando.led);
        printf("\n");
    }


    int succeded = 1;
    switch(payload.comando.led)
    {
        /*** Inspire-se no trecho do código do cliente
         *** que modifica os LEDs da placa para implementar
         *** esta parte do programa.
         ***/
        case 0: //verde
            /*** COLOQUE AQUI SEU CÓDIGO
             ***   Modifique o estado do LED verde
             ***   conforme o comando recebido (cmd)
             ***/
            if((leds_get() & LEDS_GREEN) == 0){
                leds_on(LEDS_GREEN);
            }
            else{
                leds_off(LEDS_GREEN);
            }
            break;

        case 1: //vermelho
            /*** COLOQUE AQUI SEU CÓDIGO
             ***   Modifique o estado do LED vermelho
             ***   conforme o comando recebido (cmd)
             ***/
            if((leds_get() & LEDS_RED) == 0){
               leds_on(LEDS_RED);
           }
           else{
               leds_off(LEDS_RED);
           }
            break;

        case 2: //ambos
            /*** COLOQUE AQUI SEU CÓDIGO
             ***   Modifique o estado de todos os LEDs
             ***   conforme o comando recebido (cmd)
             ***/
            if((leds_get() & LEDS_RED) == 0){
               leds_on(LEDS_RED);
           }
           else{
               leds_off(LEDS_RED);
           }

            if((leds_get() & LEDS_GREEN) == 0){
                leds_on(LEDS_GREEN);
            }
            else{
                leds_off(LEDS_GREEN);
            }
            break;

        default:
            succeded = 0;
    }

    if(!succeded) printf("Bad command!\r\n");

    /* Restore server connection to allow data from any node */
    memset(&leds_server_conn->ripaddr, 0, sizeof(leds_server_conn->ripaddr));

    return;
}
