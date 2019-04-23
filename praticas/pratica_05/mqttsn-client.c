#include <contiki.h>
#include <dev/leds.h>
#include <string.h>
#include "project-conf.h"
#include "utils.h"
#include "mqtt-sn.h"

static struct mqtt_sn_connection mqtt_sn_c;
static enum mqttsn_connection_status connection_state = MQTTSN_DISCONNECTED;

#include "cetic-6lbr-client.h"
#include "ctrl_subscription.h"
#include "publish.h"

static char mqtt_client_id[17];
static publish_packet_t incoming_packet;
static uint16_t mqtt_keep_alive=10;
static process_event_t mqttsn_connack_event;
static struct ctimer connection_timer;
static process_event_t connection_timeout_event;


/*-----FUNÇÕES AUXILIARES: CALLBACKS DA BIBLIOTECA MQTT-SN-----*/
static void connack_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen);
static void regack_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen);
static void publish_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen);
static void puback_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen);
static void suback_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen);
static void pingreq_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen);
static void connection_timer_callback(void *mqc);

/*----- Cria estrutura com callbacks para MQTT-SN -----*/
static const struct mqtt_sn_callbacks mqtt_sn_call = {
  publish_receiver,
  pingreq_receiver,
  NULL,
  connack_receiver,
  regack_receiver,
  puback_receiver,
  suback_receiver,
  NULL,
  NULL
};


/*---------------------------------------------------------------------------*/
/* This main process will create connection and register topics              */
/*---------------------------------------------------------------------------*/

PROCESS(mqttsn_process, "Configure Connection and MQTT-SN Usage");

AUTOSTART_PROCESSES(&mqttsn_process);

PROCESS_THREAD(mqttsn_process, ev, data)
{
  static struct etimer periodic_timer;
  static uip_ipaddr_t broker_addr;
  static uint8_t connection_retries = 0;

  PROCESS_BEGIN();

  // Bloqueia até que configuração IPv6 seja bem sucedida (SLAAC)
  etimer_set(&periodic_timer, 2*CLOCK_SECOND);
  while(uip_ds6_get_global(ADDR_PREFERRED) == NULL)
  {
      PROCESS_WAIT_EVENT();
      if(etimer_expired(&periodic_timer))
      {
          etimer_set(&periodic_timer, 2*CLOCK_SECOND);
          printf("Aguardando auto-configuracao de IPv6\r\n");
      }
  }
  // Imprime IPv6.
  print_local_addresses();

  // Agora vamos criar uma conexão UDP com o servidor MQTT-SN
  mqttsn_connack_event = process_alloc_event();

  // Vamos utilizar o processo cliente do 6LBR para tratar do roteamento
  process_start(&cetic_6lbr_client_process, NULL);

  // Configura IPv6 do broker MQTT-SN
  uip_ip6addr(&broker_addr, 0x2801, 0x84, 0x0, 0x1010, 0x9a4a, 0x2439, 0xc1b5, 0x563a);

  // Cria socket para MQTT-SN
  mqtt_sn_create_socket(&mqtt_sn_c, UDP_CONNECTION_PORT, &broker_addr, UDP_CONNECTION_PORT);
  // Registra call-backs para da biblioteca MQTT-SN
  (&mqtt_sn_c)->mc = &mqtt_sn_call;

  // Monta ID do cliente MQTT-SN
  sprintf(mqtt_client_id,"sens%02X%02X%02X%02X",linkaddr_node_addr.u8[4],linkaddr_node_addr.u8[5],linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);


  /* Requer conexão e aguarda por CONNACK */
  printf("requesting connection \n ");
  connection_timeout_event = process_alloc_event();
  connection_retries = 0;
  ctimer_set( &connection_timer, REPLY_TIMEOUT, connection_timer_callback, NULL);
  mqtt_sn_send_connect(&mqtt_sn_c, mqtt_client_id, mqtt_keep_alive);
  connection_state = MQTTSN_WAITING_CONNACK;
  while (connection_retries < 15)
  {
    PROCESS_WAIT_EVENT();
    if (ev == mqttsn_connack_event)
    {
      //if success
      printf("connection acked\n");
      ctimer_stop(&connection_timer);
      connection_state = MQTTSN_CONNECTED;
      connection_retries = 15;//using break here may mess up switch statement of process
    }
    else if (ev == connection_timeout_event)
    {
      connection_state = MQTTSN_CONNECTION_FAILED;
      connection_retries++;
      printf("connection timeout\n");
      ctimer_restart(&connection_timer);
      if (connection_retries < 15)
      {
        mqtt_sn_send_connect(&mqtt_sn_c,mqtt_client_id,mqtt_keep_alive);
        connection_state = MQTTSN_WAITING_CONNACK;
      }
    }
  }

  ctimer_stop(&connection_timer);
  if (connection_state == MQTTSN_CONNECTED)
  {
    process_start(&ctrl_subscription_process, 0);
    etimer_set(&periodic_timer, 3*CLOCK_SECOND);
    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    while(!etimer_expired(&periodic_timer))
    {
        PROCESS_WAIT_EVENT();
    }
    process_start(&publish_process, 0);
    etimer_set(&periodic_timer, 2*CLOCK_SECOND);
    while(1)
    {
      PROCESS_WAIT_EVENT();
      if(etimer_expired(&periodic_timer))
      {
        leds_toggle(LEDS_ALL);
        etimer_restart(&periodic_timer);
      }
    }
  }
  else
  {
      printf("unable to connect\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/


/*----- FUNÇÕES AUXILIARES: CALLBACKS DA BIBLIOTECA MQTT-SN -----*/

/*---------------------------------------------------------------------------*/
/* Esta função é chamada quando um ACK do pedido de conexão é recebido       */
static void connack_receiver(struct mqtt_sn_connection *mqc,
							 const uip_ipaddr_t *source_addr,
							 const uint8_t *data,
							 uint16_t datalen)
{
  uint8_t connack_return_code;
  connack_return_code = *(data + 3);
  printf("Connack received\n");
  if (connack_return_code == ACCEPTED)
  {
    process_post(&mqttsn_process, mqttsn_connack_event, NULL);
  }
  else
  {
    printf("Connack error: %s\n", mqtt_sn_return_code_string(connack_return_code));
  }
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Esta função é chamada quando um ACK do pedido de registro (subscribe) é   */
/* recebido.                                                                 */
static void regack_receiver(struct mqtt_sn_connection *mqc,
							const uip_ipaddr_t *source_addr,
							const uint8_t *data,
							uint16_t datalen)
{
  regack_packet_t incoming_regack;
  memcpy(&incoming_regack, data, datalen);
  printf("Regack received\n");
  if (incoming_regack.message_id == reg_topic_msg_id)
  {
    if (incoming_regack.return_code == ACCEPTED)
    {
      publisher_topic_id = uip_htons(incoming_regack.topic_id);
    }
    else
    {
      printf("Regack error: %s\n", mqtt_sn_return_code_string(incoming_regack.return_code));
    }
  }
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Esta função é chamada quando uma publicação é recebida.                   */
static void publish_receiver(struct mqtt_sn_connection *mqc,
							 const uip_ipaddr_t *source_addr,
							 const uint8_t *data,
							 uint16_t datalen)
{
  //publish_packet_t* pkt = (publish_packet_t*)data;
  memcpy(&incoming_packet, data, datalen);
  incoming_packet.data[datalen-7] = 0x00;
  printf("Published message received: %s\n", incoming_packet.data);


  printf("Data 0 : %d \n",incoming_packet.data[0]);
  printf("Data 1 : %d \n",incoming_packet.data[1]);
  printf("Data 2 : %d \n",incoming_packet.data[2]);
  printf("Data 3 : %d \n",incoming_packet.data[3]);

  if(incoming_packet.data[1] == 110){
      leds_on(LEDS_RED);
      printf("turn LED on \n");
  }
  if(incoming_packet.data[1] == 102){
      leds_off(LEDS_RED);
      printf("turn LED off \n");
  }


  //see if this message corresponds to ctrl channel subscription request
  if (uip_htons(incoming_packet.topic_id) == ctrl_topic_id)
  {
    //the new message interval will be read from the first byte of the received packet
    send_interval = 10 * CLOCK_CONF_SECOND;
  }
  else
  {
    printf("unknown publication received\n");
  }
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Esta função é chamada quando um ACK de uma publicação é recebido.         */
static void puback_receiver(struct mqtt_sn_connection *mqc,
	                        const uip_ipaddr_t *source_addr,
	                        const uint8_t *data,
	                        uint16_t datalen)
{
  printf("Puback received\n");
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Esta função é chamada quando um ACK de um subscribe é recebido.           */
static void suback_receiver(struct mqtt_sn_connection *mqc,
							const uip_ipaddr_t *source_addr,
							const uint8_t *data,
							uint16_t datalen)
{
  suback_packet_t incoming_suback;
  memcpy(&incoming_suback, data, datalen);
  printf("Suback received\n");
  if (incoming_suback.message_id == ctrl_topic_msg_id)
  {
    if (incoming_suback.return_code == ACCEPTED)
    {
      ctrl_topic_id = uip_htons(incoming_suback.topic_id);
    }
    else
    {
      printf("Suback error: %s\n", mqtt_sn_return_code_string(incoming_suback.return_code));
    }
  }
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Esta função é chamada quando uma requisição de "keep alive" é recebida.   */
static void pingreq_receiver(struct mqtt_sn_connection *mqc,
							 const uip_ipaddr_t *source_addr,
							 const uint8_t *data,
							 uint16_t datalen)
{
  printf("PingReq received\n");
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Esta função é chamada quando connection_timer expira.                     */
static void connection_timer_callback(void *mqc)
{
  process_post(&mqttsn_process, connection_timeout_event, NULL);
}
/*---------------------------------------------------------------------------*/
