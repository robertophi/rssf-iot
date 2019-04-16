#include <contiki.h>
#include <contiki-net.h>
#include <net/ip/resolv.h>
#include <button-sensor.h>
#include <dev/leds.h>
#include <stdio.h>

#include <platform/srf06-cc26xx/srf06/button-sensor.h>
#include "utils.h"
#include "sensor-timer.h"

#define SEND_INTERVAL		(5 * CLOCK_SECOND)

static struct etimer et;
static struct uip_udp_conn *temp_client_conn;
static struct uip_udp_conn *leds_client_conn;
static uip_ipaddr_t ipaddr;

static void udp_handler(void);
static void send_packet(void);
static void send_led_cmd(int8_t cmd, int8_t led);

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
  sprintf(contiki_hostname,"sensor%02X%02X",linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);
  resolv_set_hostname(contiki_hostname);
  printf("Configurando hostname para %s\r\n",contiki_hostname);


  /********** CONECTANDO AO SERVIDOR UDP **********/
  // Encontrar endereço IPv6 do servidor
  // static resolv_status_t status = RESOLV_STATUS_UNCACHED;
  // while(status != RESOLV_STATUS_CACHED)
  // {
  //     status = set_connection_address(&ipaddr, UDP_CONNECTION_ADDR);
  //     PROCESS_WAIT_EVENT();
  // }
  // Se não utilizar mDNS, use a linha abaixo para registrar IPv6 destino
  uip_ip6addr(&ipaddr, 0xfd00, 0x0000, 0x0000, 0x0000, 0x0212, 0x4b00, 0x0d2d, 0x0306);

  // Criando sockets UDP para conexão com host:porta remoto
  temp_client_conn = udp_new(&ipaddr, UIP_HTONS(CONN_PORT_TEMP), NULL);
  udp_bind(temp_client_conn, UIP_HTONS(CONN_PORT_TEMP));

  /*** AS LINHAS ABAIXO ABREM UMA SEGUNDA PORTA COM O MESMO SERVIDOR ***/
  leds_client_conn = udp_new(&ipaddr, UIP_HTONS(CONN_PORT_LEDS), NULL);
  udp_bind(leds_client_conn, UIP_HTONS(CONN_PORT_LEDS));


  etimer_set(&et, SEND_INTERVAL);

  while(1)
  {
      PROCESS_YIELD();

      if(etimer_expired(&et))
      {
          send_packet();
          etimer_restart(&et);
      }
      else if(ev == tcpip_event)
      {
          udp_handler();
      }
      /*** AS LINHAS ABAIXO VERIFICAM EVENTOS GERADOS PELOS BOTÕES ***/
      else if(ev == sensors_event)
      {
          if(data == &button_left_sensor) //botão esquerdo
          {
              send_led_cmd(2, 0); // Inverte led verde
          }
          else if(data == &button_right_sensor) // botão direito
          {
              send_led_cmd(2, 1); // inverte led vermelho
          }
      }
  }

  PROCESS_END();
}


/*------------------FUNÇÕES AUXILIARES---------------------------------------*/
static void send_packet(void)
{
    temp_payload_t payload;

    payload.i32 = get_temp_average();
    printf("Vai enviar: %ld\n", payload.i32);

    if(uip_ds6_get_global(ADDR_PREFERRED) == NULL) {
      printf("Aguardando auto-configuracao de IP\n");
      return;
    }

    uip_udp_packet_send(temp_client_conn, payload.i8, sizeof(payload));
}
/*---------------------------------------------------------------------------*/
static void udp_handler(void)
{
    temp_payload_t payload;

    if(uip_newdata()) {
      memcpy(payload.i8, uip_appdata, sizeof(payload));
      printf("TEMP : Resposta do servidor: '%ld'\n", payload.i32);
    }

    switch(payload.i32)
    {
      case 0:
        leds_off(LEDS_ALL);
        break;
      case 1:
        leds_off(LEDS_GREEN);
        leds_on(LEDS_RED);
        break;
      case 2:
        leds_off(LEDS_RED);
        leds_on(LEDS_GREEN);
        break;
      default: printf("Resposta inválida.\r\n");
    }
}
/*---------------------------------------------------------------------------*/
static void send_led_cmd(int8_t cmd, int8_t led)
{
    /*** AQUI VOCÊ DEVE ENVIAR COMANDOS PARA OS LEDS DO SERVIDOR
     ***     - INSPIRI-SE NA FUNÇÃO send_packet ACIMA.
     ***     - VOCÊ DEVE UTILIZAR A ESTRUTURA DE MENSAGEM EM leds_payload_t
     ***     - OS COMANDOS DISPONÍVEIS SÃO (cmd):
     ***       - 0: DESLIGA
     ***       - 1: LIGA
     ***       - 2: INVERTE
     ***     - OS LEDS SÃO (led):
     ***       - 0: VERDE
     ***       - 1: VERMELHO
     ***       - 2: AMBOS
     ***/
	leds_payload_t leds_payload;
	leds_payload.comando.cmd = cmd;
	leds_payload.comando.led = led;

    printf("LED : Vai enviar cmd:%d e led:%d\n", cmd, led);

    if(uip_ds6_get_global(ADDR_PREFERRED) == NULL) {
      printf("Aguardando auto-configuracao de IP\n");
      return;
    }

    uip_udp_packet_send(leds_client_conn, leds_payload.i8, sizeof(leds_payload));
}
/*---------------------------------------------------------------------------*/
