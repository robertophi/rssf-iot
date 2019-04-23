#include <contiki.h>
#include <contiki-net.h>
#include <stdio.h>

/*this process will create a subscription and monitor for incoming traffic*/

PROCESS(ctrl_subscription_process, "subscribe to a device control channel");

static mqtt_sn_subscribe_request subreq;
static char ctrl_topic[22] = "0000000000000000/ctrl\0";//of form "0011223344556677/ctrl" it is null terminated, and is 21 charactes
static uint16_t ctrl_topic_id;
static uint16_t ctrl_topic_msg_id;
static char device_id[17];


PROCESS_THREAD(ctrl_subscription_process, ev, data)
{
  static uint8_t subscription_tries;
  static mqtt_sn_subscribe_request *sreq = &subreq;
  static struct etimer periodic_timer;

  PROCESS_BEGIN();

  sprintf(device_id,"%02X%02X%02X%02X%02X%02X%02X%02X",linkaddr_node_addr.u8[0],
          linkaddr_node_addr.u8[1],linkaddr_node_addr.u8[2],linkaddr_node_addr.u8[3],
          linkaddr_node_addr.u8[4],linkaddr_node_addr.u8[5],linkaddr_node_addr.u8[6],
          linkaddr_node_addr.u8[7]);

  subscription_tries = 0;
  memcpy(ctrl_topic,device_id,16);
  printf("requesting subscription\n");
  while(subscription_tries < REQUEST_RETRIES)
  {
      printf("subscribing... topic: %s\n", ctrl_topic);
      ctrl_topic_msg_id = mqtt_sn_subscribe_try(sreq,&mqtt_sn_c,ctrl_topic,0,REPLY_TIMEOUT);

      //PROCESS_WAIT_EVENT_UNTIL(mqtt_sn_request_returned(sreq));
      etimer_set(&periodic_timer, 5*CLOCK_SECOND);
      PROCESS_WAIT_EVENT();
      if (mqtt_sn_request_success(sreq))
      {
          subscription_tries = 4;
          printf("subscription acked\n");
      }
      else
      {
          subscription_tries++;
          if (sreq->state == MQTTSN_REQUEST_FAILED)
          {
              printf("Suback error: %s\n", mqtt_sn_return_code_string(sreq->return_code));
          }
      }
  }
  PROCESS_END();
}
