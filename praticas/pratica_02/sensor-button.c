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

  // Activate batmon and button sensors
  SENSORS_ACTIVATE(batmon_sensor);
  SENSORS_ACTIVATE(button_sensor);


  while(1) {

    // Wait for some event to occur, print ev and data
    PROCESS_WAIT_EVENT();
	printf("(ev: %d -  data: %d)\n",  ev, data);

	// If ev and data correspond to a button press - print the battery sensor's value
	if(ev == PROCESS_EVENT_BUTTON && data == &button_sensor){
        int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);
		printf("Leitura : %d\n", val);
	}
    
  }

  PROCESS_END();
}   
