#include <contiki.h>
#include <sys/etimer.h>
#include <button-sensor.h>
#include <batmon-sensor.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/
#define BUF_SIZE 8
static int buffer[BUF_SIZE];
static int buf_c = 0;

static struct etimer et_sensor;

/*---------------------------------------------------------------------------*/
PROCESS(sensor_process, "Temperature Sensor Process");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(batmon_sensor);

  etimer_set(&et_sensor, 1*CLOCK_SECOND); // a cada segundo

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)  // se passou um segundo
    {
        etimer_reset(&et_sensor); // reinicia timer

        int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP); // lê sensor

        /* Insira seu código aqui */
        buffer[buf_c++] = val; // atualiza buffer
        buf_c = buf_c % BUF_SIZE;

        // printf("Leu %d\n", val);
    }
  }

  PROCESS_END();
}

int get_temp_average(void)
{
  int avg = 0;
  for(int i=0; i<BUF_SIZE; i++)
    avg += buffer[i];
  return avg/BUF_SIZE;
}
