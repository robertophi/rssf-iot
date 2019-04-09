#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "button-sensor.h"
#include "batmon-sensor.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/


static struct ctimer ct_sensor;
static void timer_interrupt(void *ptr){
    int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP); // lê sensor
	printf(" Temperatura : %d \n", val);

    struct ctimer* ct_ptr = ptr;
    ctimer_reset(ct_ptr);
}


/*---------------------------------------------------------------------------*/
PROCESS(sensor_process, "Sensor process");
/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&sensor_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(batmon_sensor);

  
  void *ct_ptr = &ct_sensor;
  ctimer_set(&ct_sensor, 1*CLOCK_SECOND, timer_interrupt, ct_ptr ); // a cada segundo

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
