#include "contiki.h"
#include "sys/etimer.h"
#include "button-sensor.h"
#include "batmon-sensor.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
#define PROCESS_EVENT_BUTTON      142


/*---------------------------------------------------------------------------*/
PROCESS(sensor_process, "Sensor process");
/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&sensor_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(batmon_sensor);
  SENSORS_ACTIVATE(button_sensor);


  while(1) {


    PROCESS_WAIT_EVENT();
	printf("(ev: %d -  data: %d)\n",  ev, data);
	if(ev == PROCESS_EVENT_BUTTON && data == &button_sensor){
        int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP); // lê sensor
		printf("Leitura : %d\n", val);
	}
    
  }

  PROCESS_END();
}
