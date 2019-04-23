#include <contiki.h>
#include <stdio.h>
#include <inttypes.h>


/*this process will publish data at regular intervals*/

PROCESS(publish_process, "register topic and publish data");

static mqtt_sn_register_request regreq;
static clock_time_t send_interval;
static char pub_topic[21] = "0000000000000000/msg\0";
static char device_id[17];
static uint16_t reg_topic_msg_id;
static uint16_t publisher_topic_id;
static int8_t qos = 0;
static uint8_t retain = FALSE;


PROCESS_THREAD(publish_process, ev, data)
{
  static uint8_t registration_tries;
  static struct etimer send_timer;
  static uint8_t buf_len;
  static uint32_t message_number;
  static char buf[20];
  static mqtt_sn_register_request *rreq = &regreq;

  PROCESS_BEGIN();
  send_interval = DEFAULT_SEND_INTERVAL;
  memcpy(pub_topic,device_id,16);
  printf("registering topic\n");
  registration_tries =0;
  while (registration_tries < REQUEST_RETRIES)
  {

    reg_topic_msg_id = mqtt_sn_register_try(rreq,&mqtt_sn_c,pub_topic,REPLY_TIMEOUT);
    //PROCESS_WAIT_EVENT_UNTIL(mqtt_sn_request_returned(rreq));
    etimer_set(&send_timer, 5*CLOCK_SECOND);
    PROCESS_WAIT_EVENT();
    if (mqtt_sn_request_success(rreq))
    {
      registration_tries = 4;
      printf("registration acked\n");
    }
    else
    {
      registration_tries++;
      if (rreq->state == MQTTSN_REQUEST_FAILED)
      {
          printf("Regack error: %s\n", mqtt_sn_return_code_string(rreq->return_code));
      }
    }
  }
  if (mqtt_sn_request_success(rreq))
  {
    //start topic publishing to topic at regular intervals
    etimer_set(&send_timer, send_interval);
    while(1)
    {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
      sprintf(buf, " %" PRIu32, message_number); //removendo o warning do GCC para o uint32_t
      printf("publishing at topic: %s -> msg: %s\n", pub_topic, buf);
      message_number++;
      buf_len = strlen(buf);
      mqtt_sn_send_publish(&mqtt_sn_c, publisher_topic_id,MQTT_SN_TOPIC_TYPE_NORMAL,buf, buf_len,qos,retain);
      /*if (ctimer_expired(&(mqtt_sn_c.receive_timer)))
      {
          process_post(&example_mqttsn_process, (process_event_t)(NULL), (process_event_t)(41));
      }*/
      etimer_set(&send_timer, send_interval);
    }
  }
  else
  {
    printf("unable to register topic\n");
  }
  PROCESS_END();
}
